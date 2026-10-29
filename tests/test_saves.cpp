#include "core/SaveSystem.h"
#include <cassert>
#include <iostream>

using namespace Voidfall;

void RunSaveTests() {
    std::cout << "[TEST] Running Save System & Metaprogression Tests..." << std::endl;

    auto& profile = SaveSystem::GetProfile();
    profile.totalNanitesBanked = 100;
    profile.upgrades.hullPlatingLevel = 0;

    // Test successful purchase
    bool bought = SaveSystem::PurchaseUpgrade(0, 50);
    assert(bought == true);
    assert(profile.upgrades.hullPlatingLevel == 1);
    assert(profile.totalNanitesBanked == 50);

    // Test insufficient funds
    bought = SaveSystem::PurchaseUpgrade(0, 80);
    assert(bought == false);
    assert(profile.upgrades.hullPlatingLevel == 1);
    assert(profile.totalNanitesBanked == 50);

    // Test max level cap (level 5)
    profile.totalNanitesBanked = 1000;
    for (int i = 0; i < 10; ++i) {
        SaveSystem::PurchaseUpgrade(0, 10);
    }
    assert(profile.upgrades.hullPlatingLevel == 5);

    std::cout << " -> Save System & Metaprogression Tests Passed!" << std::endl;
}
