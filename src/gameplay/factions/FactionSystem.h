#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <array>

namespace Voidfall {

enum class FactionID : uint8_t {
    AEGIS_DIRECTORATE = 0,
    CRIMSON_SYNDICATE,
    NULL_ORDER,
    HYPERION_LOGISTICS,
    SOLAR_FORGE,
    INDEPENDENT,
    COUNT
};

enum class ReputationTier {
    NEMESIS = 0,    // -100 to -60: Hunted on sight, elite bounty hunters sent
    HOSTILE,        // -59 to -20: Attack on sight
    SUSPICIOUS,     // -19 to -1: Increased tariffs, weapons monitored
    NEUTRAL,        // 0 to +19: Standard trade & dock access
    FRIENDLY,       // +20 to +59: Trade discount, mission access
    HONORED,        // +60 to +89: Military prototype access
    REVERED         // +90 to +100: Faction champion, fleet reinforcement beacon
};

struct FactionProfile {
    FactionID id;
    std::string name;
    std::string tag;
    std::string description;
    std::string homeSector;
    uint32_t colorHex; // RGBA
    float hostilityRate;
};

struct ActiveConflict {
    FactionID attacker;
    FactionID defender;
    std::string sectorName;
    float intensity{1.0f}; // 0 to 1
    float durationSeconds{300.0f};
};

class FactionSystem {
public:
    FactionSystem();

    const FactionProfile& GetProfile(FactionID id) const;
    int GetStanding(FactionID id) const;
    ReputationTier GetTier(FactionID id) const;
    std::string GetTierName(ReputationTier tier) const;

    void ModifyStanding(FactionID id, int delta);
    bool IsHostileToPlayer(FactionID id) const;

    // Inter-faction matrix
    int GetFactionRelations(FactionID a, FactionID b) const;
    void SetFactionRelations(FactionID a, FactionID b, int standing);

    // Dynamic conflict simulation
    void Update(float dt);
    void TriggerConflict(FactionID a, FactionID b, const std::string& sector);
    const std::vector<ActiveConflict>& GetActiveConflicts() const noexcept { return m_activeConflicts; }

    // Bounties & Perks
    int GetBountyOnPlayer(FactionID id) const;
    void AddBounty(FactionID id, int amount);
    void ClearBounty(FactionID id);

private:
    void InitializeFactions();

    std::array<FactionProfile, static_cast<size_t>(FactionID::COUNT)> m_profiles;
    std::array<int, static_cast<size_t>(FactionID::COUNT)> m_playerStandings;
    std::array<int, static_cast<size_t>(FactionID::COUNT)> m_bounties;
    int m_relationsMatrix[static_cast<size_t>(FactionID::COUNT)][static_cast<size_t>(FactionID::COUNT)];
    std::vector<ActiveConflict> m_activeConflicts;
    float m_conflictTimer{0.0f};
};

} // namespace Voidfall
