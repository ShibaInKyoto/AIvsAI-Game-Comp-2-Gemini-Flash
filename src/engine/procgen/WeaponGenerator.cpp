#include "engine/procgen/WeaponGenerator.h"
#include "core/Random.h"
#include <algorithm>

namespace Voidfall::ProcGen {

const std::vector<std::string> WeaponGenerator::s_manufacturers = {
    "Aethelgard Dynamics",
    "Hyperion Kinetics",
    "OmniTech Foundry",
    "Vanguard Heavy Industries",
    "Void-Crest Arms",
    "Zenith Orbital Works"
};

const std::vector<std::string> WeaponGenerator::s_prefixes = {
    "Hyper-Velocity",
    "Overclocked",
    "Singular",
    "Cryo-Tempered",
    "Vampiric",
    "Quantum-Coherent",
    "Heavy-Caliber",
    "Sub-Zero",
    "Voltaic",
    "Relativistic"
};

const std::vector<std::string> WeaponGenerator::s_baseTypes = {
    "Rail-Vulcan",
    "Particle Lance",
    "Flak Cannon",
    "Arc Emitter",
    "Disruptor Array",
    "Cluster Projector",
    "Stasis Driver",
    "Plasma Mortar"
};

const std::vector<std::string> WeaponGenerator::s_suffixes = {
    "of the Void",
    "of Annihilation",
    "Mark IV",
    "Protocol IX",
    "Executioner",
    "Vortex",
    "Tempest",
    "Omega"
};

ProceduralWeapon WeaponGenerator::Generate(uint64_t seed, ItemRarity targetRarity) {
    Random::Seed(seed);
    ProceduralWeapon w;
    w.rarity = targetRarity;

    std::string manu = s_manufacturers[Random::RangeI(0, static_cast<int>(s_manufacturers.size()) - 1)];
    std::string prefix = s_prefixes[Random::RangeI(0, static_cast<int>(s_prefixes.size()) - 1)];
    std::string base = s_baseTypes[Random::RangeI(0, static_cast<int>(s_baseTypes.size()) - 1)];
    std::string suffix = s_suffixes[Random::RangeI(0, static_cast<int>(s_suffixes.size()) - 1)];

    w.manufacturer = manu;
    w.name = prefix + " " + base + " " + suffix;
    w.flavorText = "Forged by " + manu + " for sector suppression ops.";

    // Base stat rolling
    float rarityMult = 1.0f;
    switch (targetRarity) {
        case ItemRarity::COMMON:
            rarityMult = 1.0f;
            w.energyColor = {200, 220, 240, 255};
            break;
        case ItemRarity::SUPERIOR:
            rarityMult = 1.35f;
            w.energyColor = {60, 160, 255, 255};
            break;
        case ItemRarity::PROTOTYPE:
            rarityMult = 1.8f;
            w.energyColor = {200, 70, 255, 255};
            break;
        case ItemRarity::SINGULARITY:
            rarityMult = 2.4f;
            w.energyColor = {255, 210, 60, 255};
            break;
    }

    w.damage = Random::RangeF(18.0f, 45.0f) * rarityMult;
    w.fireRate = Random::RangeF(0.08f, 0.45f);
    w.heatPerShot = std::max(2.0f, w.damage * 0.22f);
    w.projectileSpeed = Random::RangeF(750.0f, 1500.0f);
    w.projectileRadius = Random::RangeF(3.5f, 6.5f);
    w.spreadAngle = Random::RangeF(0.02f, 0.15f);

    if (targetRarity == ItemRarity::PROTOTYPE || targetRarity == ItemRarity::SINGULARITY) {
        if (Random::RangeF(0.0f, 1.0f) > 0.4f) {
            w.isExplosive = true;
            w.explosionRadius = Random::RangeF(35.0f, 75.0f);
        }
        if (Random::RangeF(0.0f, 1.0f) > 0.6f) {
            w.isHoming = true;
        }
        w.pierceCount = Random::RangeI(1, 3);
    }

    return w;
}

ProceduralWeapon WeaponGenerator::GenerateForSector(int sectorLevel, uint64_t seed) {
    ItemRarity rarity = ItemRarity::COMMON;
    float roll = Random::RangeF(0.0f, 1.0f) + sectorLevel * 0.15f;
    if (roll > 1.35f) rarity = ItemRarity::SINGULARITY;
    else if (roll > 1.0f) rarity = ItemRarity::PROTOTYPE;
    else if (roll > 0.65f) rarity = ItemRarity::SUPERIOR;

    return Generate(seed, rarity);
}

} // namespace Voidfall::ProcGen
