#include "FactionSystem.h"
#include <algorithm>
#include <cmath>

namespace Voidfall {

FactionSystem::FactionSystem() {
    InitializeFactions();
}

void FactionSystem::InitializeFactions() {
    m_profiles[static_cast<size_t>(FactionID::AEGIS_DIRECTORATE)] = {
        FactionID::AEGIS_DIRECTORATE,
        "Aegis Directorate",
        "AEGIS",
        "Central military defense coalition enforcing orbital peace through kinetic artillery and automated strike squadrons.",
        "Sol Prime High Orbit",
        0x388BF2FF, // Blue
        0.8f
    };

    m_profiles[static_cast<size_t>(FactionID::CRIMSON_SYNDICATE)] = {
        FactionID::CRIMSON_SYNDICATE,
        "Crimson Syndicate",
        "CRIMSON",
        "Underground brotherhood of outlaw corsairs, weapons smugglers, and illicit scrap baronies.",
        "Tartarus Asteroid Belt",
        0xE63946FF, // Crimson Red
        0.9f
    };

    m_profiles[static_cast<size_t>(FactionID::NULL_ORDER)] = {
        FactionID::NULL_ORDER,
        "Null Order Ascendants",
        "NULL",
        "Cybernetic monastic collective researching dark matter anomalies and transcendence through neural melding.",
        "Cygnus Singularity Ridge",
        0x9D4EDDFF, // Void Purple
        0.4f
    };

    m_profiles[static_cast<size_t>(FactionID::HYPERION_LOGISTICS)] = {
        FactionID::HYPERION_LOGISTICS,
        "Hyperion Logistics",
        "HYPERION",
        "Mega-conglomerate controlling 70% of interplanetary transport lanes, jump gates, and commodity markets.",
        "Elysium Trade Hub",
        0xF4A261FF, // Amber Gold
        0.3f
    };

    m_profiles[static_cast<size_t>(FactionID::SOLAR_FORGE)] = {
        FactionID::SOLAR_FORGE,
        "Solar Forge Mining Conglomerate",
        "FORGE",
        "Hardened industrial asteroid belters specializing in heavy metallurgy, titanium alloy hulls, and plasma cutters.",
        "Vulcan Foundry",
        0x2A9D8FFF, // Cyan Green
        0.5f
    };

    m_profiles[static_cast<size_t>(FactionID::INDEPENDENT)] = {
        FactionID::INDEPENDENT,
        "Independent Belters & Freelancers",
        "FREE",
        "Unaffiliated scavengers, mercenaries, and freebooters operating on the frontiers.",
        "Rim Freeport 9",
        0xA0AAB2FF, // Gray
        0.1f
    };

    // Default player standing (Neutral)
    for (size_t i = 0; i < static_cast<size_t>(FactionID::COUNT); ++i) {
        m_playerStandings[i] = 0;
        m_bounties[i] = 0;
    }
    m_playerStandings[static_cast<size_t>(FactionID::INDEPENDENT)] = 15;

    // Inter-faction matrix
    const size_t COUNT = static_cast<size_t>(FactionID::COUNT);
    for (size_t i = 0; i < COUNT; ++i) {
        for (size_t j = 0; j < COUNT; ++j) {
            m_relationsMatrix[i][j] = (i == j) ? 100 : 0;
        }
    }

    auto setRel = [this](FactionID a, FactionID b, int val) {
        m_relationsMatrix[static_cast<size_t>(a)][static_cast<size_t>(b)] = val;
        m_relationsMatrix[static_cast<size_t>(b)][static_cast<size_t>(a)] = val;
    };

    // Aegis vs Crimson is deep hatred
    setRel(FactionID::AEGIS_DIRECTORATE, FactionID::CRIMSON_SYNDICATE, -90);
    // Aegis works with Hyperion and Solar Forge
    setRel(FactionID::AEGIS_DIRECTORATE, FactionID::HYPERION_LOGISTICS, 40);
    setRel(FactionID::AEGIS_DIRECTORATE, FactionID::SOLAR_FORGE, 30);
    // Crimson Syndicate raids Hyperion and Solar Forge
    setRel(FactionID::CRIMSON_SYNDICATE, FactionID::HYPERION_LOGISTICS, -60);
    setRel(FactionID::CRIMSON_SYNDICATE, FactionID::SOLAR_FORGE, -50);
    // Null Order is isolationist
    setRel(FactionID::NULL_ORDER, FactionID::AEGIS_DIRECTORATE, -20);
    setRel(FactionID::NULL_ORDER, FactionID::CRIMSON_SYNDICATE, -30);
    // Hyperion and Solar Forge are heavy trade partners
    setRel(FactionID::HYPERION_LOGISTICS, FactionID::SOLAR_FORGE, 75);
}

const FactionProfile& FactionSystem::GetProfile(FactionID id) const {
    size_t idx = static_cast<size_t>(id);
    if (idx >= static_cast<size_t>(FactionID::COUNT)) {
        return m_profiles[static_cast<size_t>(FactionID::INDEPENDENT)];
    }
    return m_profiles[idx];
}

int FactionSystem::GetStanding(FactionID id) const {
    size_t idx = static_cast<size_t>(id);
    if (idx >= static_cast<size_t>(FactionID::COUNT)) return 0;
    return m_playerStandings[idx];
}

ReputationTier FactionSystem::GetTier(FactionID id) const {
    int s = GetStanding(id);
    if (s <= -60) return ReputationTier::NEMESIS;
    if (s <= -20) return ReputationTier::HOSTILE;
    if (s <= -1)  return ReputationTier::SUSPICIOUS;
    if (s <= 19)  return ReputationTier::NEUTRAL;
    if (s <= 59)  return ReputationTier::FRIENDLY;
    if (s <= 89)  return ReputationTier::HONORED;
    return ReputationTier::REVERED;
}

std::string FactionSystem::GetTierName(ReputationTier tier) const {
    switch (tier) {
        case ReputationTier::NEMESIS: return "NEMESIS";
        case ReputationTier::HOSTILE: return "HOSTILE";
        case ReputationTier::SUSPICIOUS: return "SUSPICIOUS";
        case ReputationTier::NEUTRAL: return "NEUTRAL";
        case ReputationTier::FRIENDLY: return "FRIENDLY";
        case ReputationTier::HONORED: return "HONORED";
        case ReputationTier::REVERED: return "REVERED";
    }
    return "UNKNOWN";
}

void FactionSystem::ModifyStanding(FactionID id, int delta) {
    size_t idx = static_cast<size_t>(id);
    if (idx >= static_cast<size_t>(FactionID::COUNT)) return;

    m_playerStandings[idx] = std::clamp(m_playerStandings[idx] + delta, -100, 100);

    // If helping one faction, slightly affect mutual enemies and allies
    if (delta > 0) {
        for (size_t other = 0; other < static_cast<size_t>(FactionID::COUNT); ++other) {
            if (other != idx) {
                int rel = m_relationsMatrix[idx][other];
                if (rel < -40) {
                    // Enemy of my friend dislikes that I helped them
                    m_playerStandings[other] = std::clamp(m_playerStandings[other] - (delta / 3), -100, 100);
                } else if (rel > 40) {
                    // Ally approves
                    m_playerStandings[other] = std::clamp(m_playerStandings[other] + (delta / 4), -100, 100);
                }
            }
        }
    }
}

bool FactionSystem::IsHostileToPlayer(FactionID id) const {
    return GetStanding(id) <= -20;
}

int FactionSystem::GetFactionRelations(FactionID a, FactionID b) const {
    size_t ia = static_cast<size_t>(a);
    size_t ib = static_cast<size_t>(b);
    return m_relationsMatrix[ia][ib];
}

void FactionSystem::SetFactionRelations(FactionID a, FactionID b, int standing) {
    size_t ia = static_cast<size_t>(a);
    size_t ib = static_cast<size_t>(b);
    int clamped = std::clamp(standing, -100, 100);
    m_relationsMatrix[ia][ib] = clamped;
    m_relationsMatrix[ib][ia] = clamped;
}

void FactionSystem::TriggerConflict(FactionID a, FactionID b, const std::string& sector) {
    ActiveConflict conflict;
    conflict.attacker = a;
    conflict.defender = b;
    conflict.sectorName = sector;
    conflict.intensity = 0.8f;
    conflict.durationSeconds = 240.0f;
    m_activeConflicts.push_back(conflict);
}

void FactionSystem::Update(float dt) {
    for (auto it = m_activeConflicts.begin(); it != m_activeConflicts.end();) {
        it->durationSeconds -= dt;
        if (it->durationSeconds <= 0.0f) {
            it = m_activeConflicts.erase(it);
        } else {
            ++it;
        }
    }

    m_conflictTimer += dt;
    if (m_conflictTimer >= 90.0f) {
        m_conflictTimer = 0.0f;
        // Periodic border skirmish
        TriggerConflict(FactionID::CRIMSON_SYNDICATE, FactionID::HYPERION_LOGISTICS, "Kepler Deep Traverse");
    }
}

int FactionSystem::GetBountyOnPlayer(FactionID id) const {
    size_t idx = static_cast<size_t>(id);
    return (idx < static_cast<size_t>(FactionID::COUNT)) ? m_bounties[idx] : 0;
}

void FactionSystem::AddBounty(FactionID id, int amount) {
    size_t idx = static_cast<size_t>(id);
    if (idx < static_cast<size_t>(FactionID::COUNT)) {
        m_bounties[idx] += amount;
    }
}

void FactionSystem::ClearBounty(FactionID id) {
    size_t idx = static_cast<size_t>(id);
    if (idx < static_cast<size_t>(FactionID::COUNT)) {
        m_bounties[idx] = 0;
    }
}

} // namespace Voidfall
