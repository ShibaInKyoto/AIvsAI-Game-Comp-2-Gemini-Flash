#include "MechAssembly.h"
#include <cmath>
#include <algorithm>

namespace Voidfall {

MechAssembly::MechAssembly() {
    for (size_t i = 0; i < static_cast<size_t>(PartSlot::COUNT); ++i) {
        m_equippedParts[i] = nullptr;
    }

    // Default loadout
    EquipPart(PartSlot::REACTOR_CORE, 1);       // Aegis Mk-IV Fusion Cell
    EquipPart(PartSlot::RADIATOR_COOLER, 4);     // Hyperion Cryo-Loop 3000
    EquipPart(PartSlot::ARMOR_CHASSIS, 7);       // Aegis Composite Bastion Plating
    EquipPart(PartSlot::THRUSTER_BOOSTER, 10);   // Aegis Vector Ion Thrusters
    EquipPart(PartSlot::FIRE_CONTROL_SYSTEM, 13);// Hyperion Lock-On Matrix
    EquipPart(PartSlot::UTILITY_MODULE, 15);     // Aegis Phase Shield Generator

    RecalculateStats();
}

bool MechAssembly::EquipPart(PartSlot slot, int partId) {
    const MechPart* part = PartCatalog::Instance().GetPart(partId);
    if (!part || part->slot != slot) return false;

    size_t idx = static_cast<size_t>(slot);
    m_equippedParts[idx] = part;
    RecalculateStats();
    return true;
}

bool MechAssembly::UnequipPart(PartSlot slot) {
    size_t idx = static_cast<size_t>(slot);
    if (m_equippedParts[idx] == nullptr) return false;
    m_equippedParts[idx] = nullptr;
    RecalculateStats();
    return true;
}

const MechPart* MechAssembly::GetEquippedPart(PartSlot slot) const {
    size_t idx = static_cast<size_t>(slot);
    return m_equippedParts[idx];
}

void MechAssembly::EvaluateSynergies() {
    m_synergies.clear();

    std::array<int, static_cast<size_t>(FactionID::COUNT)> factionCounts{};
    for (const auto* part : m_equippedParts) {
        if (part) {
            size_t fIdx = static_cast<size_t>(part->manufacturer);
            if (fIdx < factionCounts.size()) {
                factionCounts[fIdx]++;
            }
        }
    }

    // Aegis Doctrine (3+ parts)
    if (factionCounts[static_cast<size_t>(FactionID::AEGIS_DIRECTORATE)] >= 3) {
        m_synergies.push_back({
            FactionID::AEGIS_DIRECTORATE,
            "Aegis Bastion Protocol",
            "+25% Shield Capacity, +15% Kinetic Damage Resistance",
            true
        });
    }

    // Solar Forge Foundry (3+ parts)
    if (factionCounts[static_cast<size_t>(FactionID::SOLAR_FORGE)] >= 3) {
        m_synergies.push_back({
            FactionID::SOLAR_FORGE,
            "Vulcan Overforge Resilience",
            "+35% Max Hull Armor, +20 Heat Dissipation Rate",
            true
        });
    }

    // Crimson Syndicate (3+ parts)
    if (factionCounts[static_cast<size_t>(FactionID::CRIMSON_SYNDICATE)] >= 3) {
        m_synergies.push_back({
            FactionID::CRIMSON_SYNDICATE,
            "Syndicate Raider Blitz",
            "+25% Thruster Boost Acceleration, +20% Critical Strike Chance",
            true
        });
    }

    // Null Order (3+ parts)
    if (factionCounts[static_cast<size_t>(FactionID::NULL_ORDER)] >= 3) {
        m_synergies.push_back({
            FactionID::NULL_ORDER,
            "Singularity Attunement",
            "+30% Energy Beam Damage, -15% Weapon Heat Generation",
            true
        });
    }

    // Hyperion (3+ parts)
    if (factionCounts[static_cast<size_t>(FactionID::HYPERION_LOGISTICS)] >= 3) {
        m_synergies.push_back({
            FactionID::HYPERION_LOGISTICS,
            "Logistics Optimization",
            "-25% Subsystem Power Consumption, +500m Sensor Array Range",
            true
        });
    }
}

void MechAssembly::RecalculateStats() {
    EvaluateSynergies();

    float baseChassisMass = 2500.0f; // Base skeletal weight
    float totalMass = baseChassisMass;

    float powerGen = 0.0f;
    float powerDraw = 10.0f; // Base avionics & cockpit life support

    float heatGen = 0.0f;
    float heatDissipation = 15.0f; // Base hull passive radiation

    float healthBonus = 0.0f;
    float shieldBonus = 0.0f;
    float shieldRegen = 15.0f;

    float kineticResist = 0.0f;
    float energyResist = 0.0f;
    float explosiveResist = 0.0f;

    float thrust = 0.0f;
    float weaponDmg = 1.0f;
    float weaponCd = 1.0f;
    float critChance = 0.05f;
    float sensorRange = 1000.0f;

    for (const auto* part : m_equippedParts) {
        if (!part) continue;

        totalMass += part->massKg;
        powerGen += part->powerOutputMW;
        powerDraw += part->powerDrawMW;
        heatGen += part->heatGenerationRate;
        heatDissipation += part->heatDissipationRate;

        healthBonus += part->maxHealthBonus;
        shieldBonus += part->maxShieldBonus;
        shieldRegen += part->shieldRegenBonus;

        kineticResist += part->kineticResist;
        energyResist += part->energyResist;
        explosiveResist += part->explosiveResist;

        thrust += part->thrustNewtons;
        weaponDmg *= part->weaponDamageMultiplier;
        weaponCd *= part->weaponCooldownMultiplier;
        critChance += part->criticalChanceBonus;
        sensorRange += part->sensorRangeBonus;
    }

    // Apply Active Synergies
    for (const auto& syn : m_synergies) {
        if (!syn.isActive) continue;
        if (syn.faction == FactionID::AEGIS_DIRECTORATE) {
            shieldBonus *= 1.25f;
            kineticResist += 0.15f;
        } else if (syn.faction == FactionID::SOLAR_FORGE) {
            healthBonus += 200.0f;
            heatDissipation += 20.0f;
        } else if (syn.faction == FactionID::CRIMSON_SYNDICATE) {
            thrust *= 1.25f;
            critChance += 0.20f;
        } else if (syn.faction == FactionID::NULL_ORDER) {
            weaponDmg *= 1.30f;
            heatGen *= 0.85f;
        } else if (syn.faction == FactionID::HYPERION_LOGISTICS) {
            powerDraw *= 0.75f;
            sensorRange += 500.0f;
        }
    }

    m_stats.totalMassKg = totalMass;
    m_stats.powerGenerationMW = powerGen;
    m_stats.powerConsumptionMW = powerDraw;
    m_stats.netPowerMW = powerGen - powerDraw;
    m_stats.isOverpowered = (m_stats.netPowerMW >= 0.0f);

    m_stats.heatGenerationRate = heatGen;
    m_stats.heatDissipationRate = heatDissipation;

    m_stats.maxHealth = 400.0f + healthBonus;
    m_stats.maxShield = 150.0f + shieldBonus;
    m_stats.shieldRegenRate = shieldRegen;

    m_stats.kineticResist = std::min(0.75f, kineticResist);
    m_stats.energyResist = std::min(0.75f, energyResist);
    m_stats.explosiveResist = std::min(0.75f, explosiveResist);

    // Thrust-to-weight ratio physics
    float twr = (totalMass > 0.0f) ? (thrust / (totalMass * 9.81f)) : 1.0f;
    m_stats.topSpeed = std::clamp(180.0f + twr * 120.0f, 150.0f, 650.0f);
    m_stats.acceleration = std::clamp(twr * 700.0f, 300.0f, 2200.0f);
    m_stats.turnRateRadPerSec = std::clamp(2.0f + twr * 1.5f, 1.5f, 6.0f);

    m_stats.weaponDamageMult = weaponDmg;
    m_stats.weaponCooldownMult = weaponCd;
    m_stats.criticalHitChance = std::min(0.80f, critChance);
    m_stats.sensorRange = sensorRange;
}

} // namespace Voidfall
