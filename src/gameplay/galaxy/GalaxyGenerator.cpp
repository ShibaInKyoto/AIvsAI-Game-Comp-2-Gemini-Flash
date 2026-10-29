#include "GalaxyGenerator.h"
#include "core/math/Noise.h"
#include <queue>
#include <algorithm>
#include <cmath>

namespace Voidfall {

using namespace Math;

static const char* s_systemPrefixes[] = {
    "Astra", "Kepler", "Sol", "Vesper", "Cygnus", "Tartarus", "Orion", "Hyperion",
    "Apex", "Chronos", "Elysium", "Helios", "Nemesis", "Prometheus", "Styx",
    "Titan", "Valkyrie", "Zephyr", "Obsidian", "Vanguard", "Aegis", "Gorgon", "Hydra"
};

static const char* s_systemSuffixes[] = {
    "Prime", "Secundus", "Major", "Minor", "Ridge", "Deep", "Reach", "Nexus",
    "Gate", "Terminus", "Void", "Hollow", "Vortex", "Horizon", "Drift"
};

void GalaxyGenerator::GenerateSector(uint32_t seed, int systemCount) {
    m_systems.clear();
    m_systems.reserve(systemCount);

    NoiseGenerator noise(seed);

    for (int i = 0; i < systemCount; ++i) {
        StarSystem sys;
        sys.id = i;

        // Spiral cluster coordinate distribution
        float angle = static_cast<float>(i) * 0.45f + noise.FBm2D(i * 0.1f, 0.0f) * 0.5f;
        float dist = 80.0f + static_cast<float>(i) * 22.0f + noise.Perlin2D(0.0f, i * 0.1f) * 40.0f;
        sys.galacticCoords = Vec2(std::cos(angle) * dist, std::sin(angle) * dist);

        // Name generation
        int pIdx = (i + static_cast<int>(seed)) % 23;
        int sIdx = (i * 3 + static_cast<int>(seed * 2)) % 15;
        sys.name = std::string(s_systemPrefixes[pIdx]) + " " + s_systemSuffixes[sIdx];

        // Star type
        float typeSample = noise.Perlin2D(sys.galacticCoords.x * 0.005f, sys.galacticCoords.y * 0.005f);
        if (typeSample < -0.3f) sys.starType = StarType::RED_DWARF;
        else if (typeSample < 0.1f) sys.starType = StarType::YELLOW_MAIN_SEQUENCE;
        else if (typeSample < 0.35f) sys.starType = StarType::BLUE_HYPERGIANT;
        else if (typeSample < 0.5f) sys.starType = StarType::BINARY_SYSTEM;
        else if (typeSample < 0.7f) sys.starType = StarType::NEUTRON_PULSAR;
        else sys.starType = StarType::VOID_SINGULARITY;

        // Faction assignment & security level
        if (dist < 200.0f) {
            sys.controllingFaction = FactionID::AEGIS_DIRECTORATE;
            sys.securityLevel = 0.9f;
            sys.environmentalHazard = HazardLevel::SAFE;
            sys.primaryStationName = sys.name + " Citadel Command";
        } else if (angle > 2.0f && angle < 5.0f && dist > 350.0f) {
            sys.controllingFaction = FactionID::CRIMSON_SYNDICATE;
            sys.securityLevel = 0.1f;
            sys.environmentalHazard = HazardLevel::ASTEROID_DENSE;
            sys.primaryStationName = sys.name + " Corsair Anchorage";
        } else if (sys.starType == StarType::VOID_SINGULARITY || sys.starType == StarType::NEUTRON_PULSAR) {
            sys.controllingFaction = FactionID::NULL_ORDER;
            sys.securityLevel = 0.3f;
            sys.environmentalHazard = HazardLevel::VOID_CORROSION_ZONE;
            sys.primaryStationName = sys.name + " Ascendant Spire";
        } else if (dist < 400.0f && dist >= 200.0f) {
            sys.controllingFaction = (i % 2 == 0) ? FactionID::HYPERION_LOGISTICS : FactionID::SOLAR_FORGE;
            sys.securityLevel = 0.65f;
            sys.environmentalHazard = (i % 3 == 0) ? HazardLevel::ION_STORM_ACTIVITY : HazardLevel::SAFE;
            sys.primaryStationName = sys.name + " Megastructure Depot";
        } else {
            sys.controllingFaction = FactionID::INDEPENDENT;
            sys.securityLevel = 0.2f;
            sys.environmentalHazard = HazardLevel::LOW_RADIATION;
            sys.primaryStationName = sys.name + " Free Haven";
        }

        sys.stationCount = 1 + (i % 3);
        sys.asteroidBelts = 1 + ((i * 2) % 4);

        m_systems.push_back(sys);
    }

    GenerateJumpLanes();
}

void GalaxyGenerator::GenerateJumpLanes() {
    // Connect each star system to its nearest 2 to 4 neighbors
    for (size_t i = 0; i < m_systems.size(); ++i) {
        struct Neighbor {
            int id;
            float distSq;
        };
        std::vector<Neighbor> candidates;
        for (size_t j = 0; j < m_systems.size(); ++j) {
            if (i == j) continue;
            float dSq = (m_systems[i].galacticCoords - m_systems[j].galacticCoords).LengthSq();
            candidates.push_back({ static_cast<int>(j), dSq });
        }
        std::sort(candidates.begin(), candidates.end(), [](const Neighbor& a, const Neighbor& b) {
            return a.distSq < b.distSq;
        });

        int connectionsToMake = std::min(3, static_cast<int>(candidates.size()));
        for (int c = 0; c < connectionsToMake; ++c) {
            int targetId = candidates[c].id;
            auto& currLanes = m_systems[i].jumpGateConnections;
            if (std::find(currLanes.begin(), currLanes.end(), targetId) == currLanes.end()) {
                currLanes.push_back(targetId);
            }
            auto& tgtLanes = m_systems[targetId].jumpGateConnections;
            if (std::find(tgtLanes.begin(), tgtLanes.end(), static_cast<int>(i)) == tgtLanes.end()) {
                tgtLanes.push_back(static_cast<int>(i));
            }
        }
    }
}

const StarSystem* GalaxyGenerator::GetSystem(int id) const {
    if (id >= 0 && id < static_cast<int>(m_systems.size())) {
        return &m_systems[id];
    }
    return nullptr;
}

const StarSystem* GalaxyGenerator::FindSystemByName(const std::string& name) const {
    for (const auto& sys : m_systems) {
        if (sys.name == name) return &sys;
    }
    return nullptr;
}

WarpRoute GalaxyGenerator::CalculateRoute(int startId, int targetId, bool avoidLawless) const {
    WarpRoute route;
    if (startId < 0 || startId >= static_cast<int>(m_systems.size()) ||
        targetId < 0 || targetId >= static_cast<int>(m_systems.size())) {
        return route;
    }

    if (startId == targetId) {
        route.systemPath = { startId };
        route.isValid = true;
        return route;
    }

    struct Node {
        int id;
        float cost;
        bool operator>(const Node& o) const { return cost > o.cost; }
    };

    std::vector<float> dist(m_systems.size(), 1e9f);
    std::vector<int> prev(m_systems.size(), -1);
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    dist[startId] = 0.0f;
    pq.push({ startId, 0.0f });

    while (!pq.empty()) {
        Node curr = pq.top();
        pq.pop();

        if (curr.id == targetId) break;
        if (curr.cost > dist[curr.id]) continue;

        const auto& sys = m_systems[curr.id];
        for (int neighborId : sys.jumpGateConnections) {
            const auto& nbr = m_systems[neighborId];

            float hopDist = (sys.galacticCoords - nbr.galacticCoords).Length();
            float hazardWeight = 1.0f;
            if (avoidLawless && nbr.securityLevel < 0.3f) {
                hazardWeight += 4.0f; // Strongly penalize pirate space if avoidance requested
            }
            if (nbr.environmentalHazard != HazardLevel::SAFE) {
                hazardWeight += 1.5f;
            }

            float newCost = curr.cost + hopDist * hazardWeight;
            if (newCost < dist[neighborId]) {
                dist[neighborId] = newCost;
                prev[neighborId] = curr.id;
                pq.push({ neighborId, newCost });
            }
        }
    }

    if (prev[targetId] == -1) {
        return route; // No path found
    }

    // Reconstruct path
    int curr = targetId;
    while (curr != -1) {
        route.systemPath.push_back(curr);
        curr = prev[curr];
    }
    std::reverse(route.systemPath.begin(), route.systemPath.end());

    // Calculate total stats
    route.isValid = true;
    for (size_t i = 0; i + 1 < route.systemPath.size(); ++i) {
        int a = route.systemPath[i];
        int b = route.systemPath[i + 1];
        route.totalWarpDistance += (m_systems[a].galacticCoords - m_systems[b].galacticCoords).Length();
        route.estimatedHazardScore += (1.0f - m_systems[b].securityLevel) * 10.0f;
    }

    return route;
}

TacticalTravelEncounter GalaxyGenerator::RollEncounter(int fromId, int toId, float playerThreat) const {
    TacticalTravelEncounter enc;
    const auto* toSys = GetSystem(toId);
    if (!toSys) return enc;

    // Roll based on security
    float roll = static_cast<float>(rand() % 100) / 100.0f;

    if (toSys->securityLevel <= 0.25f) {
        // Lawless sector
        if (roll < 0.55f) {
            enc.type = EncounterType::CORSAIR_INTERDICTION;
            enc.title = "Syndicate Corsair Interdiction";
            enc.description = "Warp bubble disrupted! 3 Crimson Voidfang raiders drop out of hyperspace with lock-on!";
            enc.involvedFaction = FactionID::CRIMSON_SYNDICATE;
            enc.threatLevel = std::max(1, static_cast<int>(playerThreat * 1.3f));
            enc.potentialScrapReward = 450;
        } else if (roll < 0.80f) {
            enc.type = EncounterType::DERELICT_VOID_CARRIER;
            enc.title = "Derelict Dreadnought Hulk";
            enc.description = "Scanners detect an abandoned Aegis heavy transport adrift in the debris belt.";
            enc.potentialScrapReward = 800;
        } else {
            enc.type = EncounterType::DARK_MATTER_RIFT;
            enc.title = "Unstable Dark Matter Anomaly";
            enc.description = "Singularity gravitational waves distort local space, stressing mech hull integrity.";
        }
    } else if (toSys->securityLevel >= 0.75f) {
        // HighSec sector
        if (roll < 0.35f) {
            enc.type = EncounterType::FACTION_PATROL_INSPECTION;
            enc.title = "Aegis Directorate Customs Sweep";
            enc.description = "Aegis automated cruisers run deep biometric and contraband cargo scans.";
            enc.involvedFaction = FactionID::AEGIS_DIRECTORATE;
        } else if (roll < 0.65f) {
            enc.type = EncounterType::DISTRESS_BEACON;
            enc.title = "Civilian Freighter Distress Call";
            enc.description = "A civilian transport engine manifold has ruptured near the local jump gate.";
            enc.potentialScrapReward = 300;
        }
    } else {
        // MidSec sector
        if (roll < 0.30f) {
            enc.type = EncounterType::ASTEROID_COLLISION_SWARM;
            enc.title = "Hypervelocity Asteroid Field";
            enc.description = "Entering a dense tungsten asteroid field; manual evasive piloting required!";
        }
    }

    return enc;
}

} // namespace Voidfall
