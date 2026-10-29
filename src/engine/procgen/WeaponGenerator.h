#pragma once

#include "core/Types.h"
#include <raylib.h>
#include <string>
#include <vector>
#include <cstdint>

namespace Voidfall::ProcGen {

enum class ItemRarity {
    COMMON,       // White / Grey
    SUPERIOR,     // Blue
    PROTOTYPE,    // Purple
    SINGULARITY   // Gold / Prismatic
};

struct ProceduralWeapon {
    std::string name;
    std::string manufacturer;
    std::string flavorText;
    ItemRarity rarity{ItemRarity::COMMON};

    float damage{20.0f};
    float fireRate{0.2f};
    float heatPerShot{5.0f};
    float projectileSpeed{900.0f};
    float projectileRadius{4.0f};
    int projectilesPerShot{1};
    float spreadAngle{0.0f};
    int pierceCount{0};

    bool isHoming{false};
    bool isExplosive{false};
    float explosionRadius{0.0f};
    float shieldPenetration{0.0f};

    Color energyColor{255, 255, 255, 255};
};

class WeaponGenerator {
public:
    static ProceduralWeapon Generate(uint64_t seed, ItemRarity targetRarity = ItemRarity::COMMON);
    static ProceduralWeapon GenerateForSector(int sectorLevel, uint64_t seed);

private:
    static const std::vector<std::string> s_manufacturers;
    static const std::vector<std::string> s_prefixes;
    static const std::vector<std::string> s_baseTypes;
    static const std::vector<std::string> s_suffixes;
};

} // namespace Voidfall::ProcGen
