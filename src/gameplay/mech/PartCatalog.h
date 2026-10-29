#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "gameplay/factions/FactionSystem.h"

namespace Voidfall {

enum class PartSlot : uint8_t {
    REACTOR_CORE = 0,
    RADIATOR_COOLER,
    ARMOR_CHASSIS,
    THRUSTER_BOOSTER,
    FIRE_CONTROL_SYSTEM,
    WEAPON_LEFT_ARM,
    WEAPON_RIGHT_ARM,
    SHOULDER_POD_LEFT,
    SHOULDER_POD_RIGHT,
    UTILITY_MODULE,
    COUNT
};

enum class PartRarity : uint8_t {
    STANDARD = 0,
    REINFORCED,
    MILITARY_SPEC,
    PROTOTYPE,
    ANOMALOUS_VOID
};

struct MechPart {
    int id{0};
    std::string name;
    PartSlot slot;
    PartRarity rarity;
    FactionID manufacturer;

    // Physical & Power attributes
    float massKg{500.0f};
    float powerOutputMW{0.0f};
    float powerDrawMW{0.0f};
    float heatDissipationRate{0.0f};
    float heatGenerationRate{0.0f};

    // Performance modifiers
    float thrustNewtons{0.0f};
    float maxHealthBonus{0.0f};
    float maxShieldBonus{0.0f};
    float shieldRegenBonus{0.0f};
    float kineticResist{0.0f};
    float energyResist{0.0f};
    float explosiveResist{0.0f};

    // Weapon/FCS specifics
    float weaponDamageMultiplier{1.0f};
    float weaponCooldownMultiplier{1.0f};
    float sensorRangeBonus{0.0f};
    float criticalChanceBonus{0.0f};

    int scrapCost{250};
    std::string description;
};

class PartCatalog {
public:
    static PartCatalog& Instance();

    void InitializeCatalog();
    const MechPart* GetPart(int id) const;
    const MechPart* FindByName(const std::string& name) const;
    std::vector<const MechPart*> GetPartsBySlot(PartSlot slot) const;
    std::vector<const MechPart*> GetPartsByManufacturer(FactionID faction) const;

    size_t GetTotalPartsCount() const noexcept { return m_parts.size(); }
    const std::vector<MechPart>& GetAllParts() const noexcept { return m_parts; }

private:
    PartCatalog();
    void RegisterPart(const MechPart& part);

    std::vector<MechPart> m_parts;
};

} // namespace Voidfall
