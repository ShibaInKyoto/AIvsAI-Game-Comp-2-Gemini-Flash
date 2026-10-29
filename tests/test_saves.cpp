#include "core/SaveSystem.h"
#include <cassert>
#include <iostream>

using namespace Voidfall;

void RunSaveTests() {
    std::cout << "[TEST] Running Save System & Metaprogression Tests..." << std::endl;

    auto& profile = SaveSystem::GetProfile();
    profile.totalNanitesBanked = 200;
    profile.upgrades.hullPlatingLevel = 0;

    // Test successful purchase (Level 1 cost = 150)
    bool bought = SaveSystem::PurchaseUpgrade(0);
    assert(bought == true);
    assert(profile.upgrades.hullPlatingLevel == 1);
    assert(profile.totalNanitesBanked == 50);

    // Test insufficient funds (Level 2 cost = 300, only have 50)
    bought = SaveSystem::PurchaseUpgrade(0);
    assert(bought == false);
    assert(profile.upgrades.hullPlatingLevel == 1);
    assert(profile.totalNanitesBanked == 50);

    // Test max level cap (level 5)
    profile.totalNanitesBanked = 10000;
    for (int i = 0; i < 10; ++i) {
        SaveSystem::PurchaseUpgrade(0);
    }
    assert(profile.upgrades.hullPlatingLevel == 5);

    // Test Respec
    SaveSystem::ResetUpgrades();
    assert(profile.upgrades.hullPlatingLevel == 0);

    std::cout << " -> Save System & Metaprogression Tests Passed!" << std::endl;
}
