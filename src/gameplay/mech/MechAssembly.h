#pragma once

#include <array>
#include <vector>
#include <string>
#include "PartCatalog.h"

namespace Voidfall {

struct SynergyBonus {
    FactionID faction;
    std::string name;
    std::string description;
    bool isActive{false};
};

struct MechDerivedStats {
    float totalMassKg{0.0f};
    float powerGenerationMW{0.0f};
    float powerConsumptionMW{0.0f};
    float netPowerMW{0.0f};
    bool isOverpowered{true}; // Gen >= Consumption

    float heatGenerationRate{0.0f};
    float heatDissipationRate{0.0f};

    float maxHealth{500.0f};
    float maxShield{200.0f};
    float shieldRegenRate{25.0f};

    float kineticResist{0.0f};
    float energyResist{0.0f};
    float explosiveResist{0.0f};

    float topSpeed{350.0f};
    float acceleration{800.0f};
    float turnRateRadPerSec{3.5f};

    float weaponDamageMult{1.0f};
    float weaponCooldownMult{1.0f};
    float criticalHitChance{0.05f};
    float sensorRange{1200.0f};
};

class MechAssembly {
public:
    MechAssembly();

    bool EquipPart(PartSlot slot, int partId);
    bool UnequipPart(PartSlot slot);
    const MechPart* GetEquippedPart(PartSlot slot) const;

    void RecalculateStats();
    const MechDerivedStats& GetStats() const noexcept { return m_stats; }
    const std::vector<SynergyBonus>& GetActiveSynergies() const noexcept { return m_synergies; }

    bool IsValidBuild() const noexcept { return m_stats.isOverpowered; }

private:
    void EvaluateSynergies();

    std::array<const MechPart*, static_cast<size_t>(PartSlot::COUNT)> m_equippedParts;
    MechDerivedStats m_stats;
    std::vector<SynergyBonus> m_synergies;
};

} // namespace Voidfall
