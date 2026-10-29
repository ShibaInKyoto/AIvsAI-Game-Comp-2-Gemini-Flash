#include "gameplay/factions/FactionSystem.h"
#include "gameplay/economy/TradeMarket.h"
#include "gameplay/galaxy/GalaxyGenerator.h"
#include <cassert>
#include <iostream>

namespace Voidfall {

void RunGalaxyEconomyTests() {
    std::cout << "[TEST] Running Galaxy, Factions & Trade Economy Tests...\n";

    // 1. Faction System
    FactionSystem factions;
    assert(factions.GetStanding(FactionID::AEGIS_DIRECTORATE) == 0);
    assert(factions.GetTier(FactionID::AEGIS_DIRECTORATE) == ReputationTier::NEUTRAL);

    factions.ModifyStanding(FactionID::AEGIS_DIRECTORATE, 50);
    assert(factions.GetStanding(FactionID::AEGIS_DIRECTORATE) == 50);
    assert(factions.GetTier(FactionID::AEGIS_DIRECTORATE) == ReputationTier::FRIENDLY);

    // Negative standing & hostility
    factions.ModifyStanding(FactionID::CRIMSON_SYNDICATE, -40);
    assert(factions.IsHostileToPlayer(FactionID::CRIMSON_SYNDICATE));

    // 2. Economy & Dynamic Station Trade
    StationMarket market("Kepler Station 01", FactionID::AEGIS_DIRECTORATE);
    const auto* entry = market.GetEntry(CommodityID::HELIUM3_FUEL);
    assert(entry != nullptr);
    assert(entry->quantity > 0);

    int playerCredits = 5000;
    std::vector<CargoItem> cargo;
    int maxMass = 200;

    int buyPrice = market.CalculateBuyPriceForPlayer(CommodityID::HELIUM3_FUEL, 50);
    bool bought = market.BuyCommodity(CommodityID::HELIUM3_FUEL, 5, playerCredits, cargo, maxMass, 50);
    assert(bought);
    assert(cargo.size() == 1);
    assert(cargo[0].quantity == 5);
    assert(playerCredits == 5000 - buyPrice * 5);

    bool sold = market.SellCommodity(CommodityID::HELIUM3_FUEL, 2, playerCredits, cargo, 50);
    assert(sold);
    assert(cargo[0].quantity == 3);

    // Market Event Test
    market.SetEvent(MarketEvent::WAR_MOBILIZATION);
    assert(market.GetCurrentEvent() == MarketEvent::WAR_MOBILIZATION);

    // 3. Galaxy Generator & Hyperlane Routing
    GalaxyGenerator galaxy;
    galaxy.GenerateSector(42, 25);
    assert(galaxy.GetSystems().size() == 25);

    const StarSystem* sys0 = galaxy.GetSystem(0);
    assert(sys0 != nullptr);
    assert(!sys0->jumpGateConnections.empty());

    // Pathfinding
    WarpRoute route = galaxy.CalculateRoute(0, 10);
    if (route.isValid) {
        assert(!route.systemPath.empty());
        assert(route.systemPath.front() == 0);
        assert(route.systemPath.back() == 10);
    }

    std::cout << "[PASS] Galaxy, Factions & Trade Economy Tests passed!\n";
}

} // namespace Voidfall
