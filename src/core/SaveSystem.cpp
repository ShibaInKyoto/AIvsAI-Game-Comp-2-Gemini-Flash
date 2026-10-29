#include "core/SaveSystem.h"
#include <fstream>
#include <iostream>

namespace Voidfall {

PlayerProfile SaveSystem::s_profile{};

std::string SaveSystem::GetSaveFilePath() {
    return "save_profile.dat";
}

void SaveSystem::Load() {
    std::ifstream file(GetSaveFilePath(), std::ios::binary);
    if (!file.is_open()) {
        // Default new profile
        s_profile = PlayerProfile();
        return;
    }

    file.read(reinterpret_cast<char*>(&s_profile), sizeof(PlayerProfile));
    if (file.bad() || file.gcount() != sizeof(PlayerProfile)) {
        // Fallback if save is corrupt or changed size
        s_profile = PlayerProfile();
    }
}

void SaveSystem::Save() {
    std::ofstream file(GetSaveFilePath(), std::ios::binary | std::ios::trunc);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&s_profile), sizeof(PlayerProfile));
    }
}

void SaveSystem::AddNanites(int amount) {
    s_profile.totalNanitesBanked += amount;
    Save();
}

int SaveSystem::GetUpgradeCost(int upgradeType) {
    int currentLevel = 0;
    switch (upgradeType) {
        case 0: currentLevel = s_profile.upgrades.hullPlatingLevel; break;
        case 1: currentLevel = s_profile.upgrades.coolingTurbinesLevel; break;
        case 2: currentLevel = s_profile.upgrades.shieldCapacitorLevel; break;
        case 3: currentLevel = s_profile.upgrades.scavengerNanitesLevel; break;
        case 4: currentLevel = s_profile.upgrades.dashCoolingLevel; break;
        default: return 999999;
    }
    if (currentLevel >= 5) return -1; // Maxed out
    return (currentLevel + 1) * 150;  // 150, 300, 450, 600, 750
}

bool SaveSystem::PurchaseUpgrade(int upgradeType) {
    int cost = GetUpgradeCost(upgradeType);
    if (cost < 0 || s_profile.totalNanitesBanked < cost) return false;

    bool bought = false;
    switch (upgradeType) {
        case 0:
            if (s_profile.upgrades.hullPlatingLevel < 5) {
                s_profile.upgrades.hullPlatingLevel++;
                bought = true;
            }
            break;
        case 1:
            if (s_profile.upgrades.coolingTurbinesLevel < 5) {
                s_profile.upgrades.coolingTurbinesLevel++;
                bought = true;
            }
            break;
        case 2:
            if (s_profile.upgrades.shieldCapacitorLevel < 5) {
                s_profile.upgrades.shieldCapacitorLevel++;
                bought = true;
            }
            break;
        case 3:
            if (s_profile.upgrades.scavengerNanitesLevel < 5) {
                s_profile.upgrades.scavengerNanitesLevel++;
                bought = true;
            }
            break;
        case 4:
            if (s_profile.upgrades.dashCoolingLevel < 5) {
                s_profile.upgrades.dashCoolingLevel++;
                bought = true;
            }
            break;
        default:
            break;
    }

    if (bought) {
        s_profile.totalNanitesBanked -= cost;
        Save();
    }
    return bought;
}

void SaveSystem::ResetUpgrades() {
    int refundTotal = 0;
    auto CalcInvested = [](int level) {
        int sum = 0;
        for (int l = 1; l <= level; ++l) {
            sum += l * 150;
        }
        return sum;
    };

    refundTotal += CalcInvested(s_profile.upgrades.hullPlatingLevel);
    refundTotal += CalcInvested(s_profile.upgrades.coolingTurbinesLevel);
    refundTotal += CalcInvested(s_profile.upgrades.shieldCapacitorLevel);
    refundTotal += CalcInvested(s_profile.upgrades.scavengerNanitesLevel);
    refundTotal += CalcInvested(s_profile.upgrades.dashCoolingLevel);

    s_profile.upgrades = HangarUpgrades();
    s_profile.totalNanitesBanked += refundTotal;
    Save();
}

void SaveSystem::UpdateEndlessRecord(int wave, float timeSec, int kills, int nanites) {
    if (wave > s_profile.endlessStats.maxWave) {
        s_profile.endlessStats.maxWave = wave;
    }
    if (timeSec > s_profile.endlessStats.maxTimeSurvived) {
        s_profile.endlessStats.maxTimeSurvived = timeSec;
    }
    s_profile.endlessStats.totalKills += kills;
    s_profile.endlessStats.nanitesHarvested += nanites;
    s_profile.totalNanitesBanked += nanites;
    Save();
}

} // namespace Voidfall
