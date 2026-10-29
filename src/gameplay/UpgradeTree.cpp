#include "gameplay/UpgradeTree.h"
#include "gameplay/PlayerMech.h"
#include "core/Random.h"
#include <algorithm>

namespace Voidfall {

std::vector<UpgradeCard> UpgradeTree::GetDraftChoices(int count) {
    std::vector<UpgradeCard> pool = {
        {"OVERCLOCKED_MAGNETS", "Overclocked Rail Magnets", "+35% Projectile Velocity, +20% Projectile Size.", "Offense", 60, false},
        {"CRYO_COOLANT", "Cryo-Coolant Infusion", "-25% Heat Accumulation, wider Active Flush Sweet Spot.", "Reactor", 50, false},
        {"DEFLECTOR_HARMONIC", "Resonant Deflector Grid", "Parries unleash kinetic shockwave and restore 25 Shield.", "Defense", 75, false},
        {"NANITE_HULL_MATRIX", "Nanite Repair Matrix", "+40 Max Hull and gradual hull self-repair.", "Hull", 65, false},
        {"RESONANCE_CAPACITOR", "Assault Overclock", "Shifting to Assault mode grants guaranteed Crits for 3s.", "Tactics", 80, false},
        {"HYDRA_EXPANSION", "Hydra Swarm Expansion", "+2 Micro-Missiles per salvo with 25% larger blast.", "Weapons", 90, false},
        {"PHASE_DISTORTION", "Phase Blink EMP", "Dashes leave behind an electromagnetic stun mine.", "Mobility", 70, false},
        {"VULCAN_ACCELERATOR", "Depleted Core Vulcan", "+40% Vulcan Fire Rate with armor pierce.", "Weapons", 85, false}
    };

    // Shuffle pool
    for (size_t i = 0; i < pool.size(); ++i) {
        size_t j = Random::RangeI(0, static_cast<int>(pool.size() - 1));
        std::swap(pool[i], pool[j]);
    }

    if (count > static_cast<int>(pool.size())) count = static_cast<int>(pool.size());
    return std::vector<UpgradeCard>(pool.begin(), pool.begin() + count);
}

void UpgradeTree::ApplyUpgrade(const std::string& upgradeId, PlayerMech& player) {
    if (upgradeId == "NANITE_HULL_MATRIX") {
        player.Heal(50.0f);
    }
}

} // namespace Voidfall
