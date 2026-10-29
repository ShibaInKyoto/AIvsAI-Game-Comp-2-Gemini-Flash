#pragma once

#include <string>
#include <vector>
#include "core/math/MathCore.h"
#include "gameplay/factions/FactionSystem.h"

namespace Voidfall {

enum class StarType {
    YELLOW_MAIN_SEQUENCE,
    BLUE_HYPERGIANT,
    RED_DWARF,
    BINARY_SYSTEM,
    NEUTRON_PULSAR,
    VOID_SINGULARITY
};

enum class HazardLevel {
    SAFE = 0,
    LOW_RADIATION,
    ION_STORM_ACTIVITY,
    ASTEROID_DENSE,
    VOID_CORROSION_ZONE,
    EXTREME_PULSAR_EMISSION
};

struct StarSystem {
    int id{0};
    std::string name;
    Vec2 galacticCoords;
    StarType starType{StarType::YELLOW_MAIN_SEQUENCE};
    FactionID controllingFaction{FactionID::INDEPENDENT};
    float securityLevel{0.5f}; // 0.0 (Lawless) to 1.0 (Fortress)
    HazardLevel environmentalHazard{HazardLevel::SAFE};
    int stationCount{1};
    int asteroidBelts{2};
    std::vector<int> jumpGateConnections; // System IDs
    std::string primaryStationName;
};

struct WarpRoute {
    std::vector<int> systemPath;
    float totalWarpDistance{0.0f};
    float estimatedHazardScore{0.0f};
    bool isValid{false};
};

enum class EncounterType {
    NONE = 0,
    CORSAIR_INTERDICTION,
    FACTION_PATROL_INSPECTION,
    DERELICT_VOID_CARRIER,
    ASTEROID_COLLISION_SWARM,
    DISTRESS_BEACON,
    DARK_MATTER_RIFT
};

struct TacticalTravelEncounter {
    EncounterType type{EncounterType::NONE};
    std::string title;
    std::string description;
    FactionID involvedFaction{FactionID::INDEPENDENT};
    int threatLevel{1};
    int potentialScrapReward{0};
};

class GalaxyGenerator {
public:
    GalaxyGenerator() = default;

    void GenerateSector(uint32_t seed = 1337, int systemCount = 30);
    const std::vector<StarSystem>& GetSystems() const noexcept { return m_systems; }
    const StarSystem* GetSystem(int id) const;
    const StarSystem* FindSystemByName(const std::string& name) const;

    // Hyperlane Pathfinding (Dijkstra / A*)
    WarpRoute CalculateRoute(int startSystemId, int targetSystemId, bool avoidLawless = false) const;

    // Dynamic travel encounter generation
    TacticalTravelEncounter RollEncounter(int fromSystemId, int toSystemId, float playerThreatRating) const;

private:
    void GenerateJumpLanes();

    std::vector<StarSystem> m_systems;
};

} // namespace Voidfall
