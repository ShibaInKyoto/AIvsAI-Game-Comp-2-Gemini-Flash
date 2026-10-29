#include "gameplay/RelicInventory.h"
#include <algorithm>

namespace Voidfall {

RelicInventory::RelicInventory() {
    m_relics.reserve(16);
    m_activeSynergies.reserve(8);
}

void RelicInventory::Clear() {
    m_relics.clear();
    m_activeSynergies.clear();
}

bool RelicInventory::HasRelic(const std::string& relicId) const {
    return std::find(m_relics.begin(), m_relics.end(), relicId) != m_relics.end();
}

void RelicInventory::AddRelic(const std::string& relicId) {
    if (!HasRelic(relicId)) {
        m_relics.push_back(relicId);
        CheckSynergies();
    }
}

void RelicInventory::CheckSynergies() {
    m_activeSynergies.clear();

    if (HasRelic("OVERCLOCKED_MAGNETS") && HasRelic("VULCAN_ACCELERATOR")) {
        m_activeSynergies.push_back({
            "KINETIC HYPER-DRIVER",
            "+100% Projectile Velocity, critical hits pierce infinite targets.",
            "OVERCLOCKED_MAGNETS",
            "VULCAN_ACCELERATOR",
            true
        });
    }

    if (HasRelic("CRYO_COOLANT") && HasRelic("DEFLECTOR_HARMONIC")) {
        m_activeSynergies.push_back({
            "ABSOLUTE ZERO BULWARK",
            "Perfect parries freeze all hostiles in 200 radius for 2.5 seconds.",
            "CRYO_COOLANT",
            "DEFLECTOR_HARMONIC",
            true
        });
    }

    if (HasRelic("HYDRA_EXPANSION") && HasRelic("RESONANCE_CAPACITOR")) {
        m_activeSynergies.push_back({
            "MACRO-CLUSTER APOCALYPSE",
            "Missile explosions duplicate into 3 secondary micro-warheads.",
            "HYDRA_EXPANSION",
            "RESONANCE_CAPACITOR",
            true
        });
    }
}

} // namespace Voidfall
