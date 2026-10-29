#include "gameplay/mech/PartCatalog.h"
#include "gameplay/mech/MechAssembly.h"
#include <cassert>
#include <iostream>

namespace Voidfall {

void RunMechAssemblyTests() {
    std::cout << "[TEST] Running Mech Assembly & Part Catalog Tests...\n";

    // 1. Part Catalog
    PartCatalog& catalog = PartCatalog::Instance();
    assert(catalog.GetTotalPartsCount() > 10);

    const MechPart* fusionReactor = catalog.GetPart(1);
    assert(fusionReactor != nullptr);
    assert(fusionReactor->slot == PartSlot::REACTOR_CORE);
    assert(fusionReactor->powerOutputMW > 0.0f);

    // 2. Mech Assembly & Loadout
    MechAssembly mech;
    const auto& stats = mech.GetStats();
    assert(stats.totalMassKg > 2000.0f);
    assert(stats.powerGenerationMW > 0.0f);
    assert(stats.powerConsumptionMW > 0.0f);
    assert(stats.isOverpowered); // Default build has sufficient wattage
    assert(stats.topSpeed > 100.0f);
    assert(stats.acceleration > 100.0f);

    // 3. Synergy Bonus
    // Default loadout has 3 Aegis parts (Reactor #1, Plating #7, Thruster #10, Phase Shield #15)
    bool hasAegisSynergy = false;
    for (const auto& syn : mech.GetActiveSynergies()) {
        if (syn.faction == FactionID::AEGIS_DIRECTORATE) {
            hasAegisSynergy = true;
            break;
        }
    }
    assert(hasAegisSynergy);

    // 4. Unequip & Swap Part
    bool unequipped = mech.UnequipPart(PartSlot::REACTOR_CORE);
    assert(unequipped);
    assert(!mech.IsValidBuild()); // No reactor -> power failure!

    // Equip high-tier reactor
    bool equippedNew = mech.EquipPart(PartSlot::REACTOR_CORE, 2); // Solar Forge Tokamak
    assert(equippedNew);
    assert(mech.IsValidBuild());

    std::cout << "[PASS] Mech Assembly & Part Catalog Tests passed!\n";
}

} // namespace Voidfall
