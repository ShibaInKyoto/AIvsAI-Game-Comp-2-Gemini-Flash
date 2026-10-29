#pragma once

#include <string>
#include <vector>

namespace Voidfall {

struct HangarUpgrades {
    int hullPlatingLevel{0};     // +15 Max Hull per level (max 5)
    int coolingTurbinesLevel{0}; // +10% Heat Dissipation per level (max 5)
    int shieldCapacitorLevel{0}; // +10 Max Shield per level (max 5)
    int scavengerNanitesLevel{0};// +15% Nanite drops per level (max 5)
    int dashCoolingLevel{0};     // +10% Dash recharge speed (max 5)
};

struct PlayerProfile {
    int totalNanitesBanked{0};
    int totalRunsAttempted{0};
    int totalVictories{0};
    int totalKills{0};
    int titansDefeated{0};

    HangarUpgrades upgrades{};
    EndlessStats endlessStats{};

    bool unlockedTitanFrame{true};
    bool unlockedPhantomFrame{false};
    bool unlockedAegisFrame{false};

    bool unlockedSingularity{false};
    bool unlockedCryoRail{false};
    bool unlockedNeutronDisruptor{false};
    bool unlockedClusterTorpedo{false};
};

class SaveSystem {
public:
    static PlayerProfile& GetProfile() { return s_profile; }
    static void Load();
    static void Save();
    static void AddNanites(int amount);
    static int GetUpgradeCost(int upgradeType);
    static bool PurchaseUpgrade(int upgradeType);
    static void ResetUpgrades(); // Refunds all invested nanites back to the bank
    static void UpdateEndlessRecord(int wave, float timeSec, int kills, int nanites);

private:
    static std::string GetSaveFilePath();
    static PlayerProfile s_profile;
};

} // namespace Voidfall
