#include "PartCatalog.h"

namespace Voidfall {

PartCatalog& PartCatalog::Instance() {
    static PartCatalog instance;
    return instance;
}

PartCatalog::PartCatalog() {
    InitializeCatalog();
}

void PartCatalog::RegisterPart(const MechPart& part) {
    m_parts.push_back(part);
}

void PartCatalog::InitializeCatalog() {
    m_parts.clear();
    int nextId = 1;

    // --- 1. REACTORS ---
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Aegis Mk-IV Fusion Cell";
        p.slot = PartSlot::REACTOR_CORE;
        p.rarity = PartRarity::STANDARD;
        p.manufacturer = FactionID::AEGIS_DIRECTORATE;
        p.massKg = 1200.0f;
        p.powerOutputMW = 180.0f;
        p.heatGenerationRate = 12.0f;
        p.scrapCost = 300;
        p.description = "Standard military-grade magnetic confinement fusion core.";
        RegisterPart(p);
    }
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Solar Forge Supercharged Tokamak";
        p.slot = PartSlot::REACTOR_CORE;
        p.rarity = PartRarity::MILITARY_SPEC;
        p.manufacturer = FactionID::SOLAR_FORGE;
        p.massKg = 1600.0f;
        p.powerOutputMW = 260.0f;
        p.heatGenerationRate = 22.0f;
        p.scrapCost = 750;
        p.description = "Industrial-grade high-output reactor delivering massive wattage at higher operating temps.";
        RegisterPart(p);
    }
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Null Order Singularity Sphere";
        p.slot = PartSlot::REACTOR_CORE;
        p.rarity = PartRarity::PROTOTYPE;
        p.manufacturer = FactionID::NULL_ORDER;
        p.massKg = 900.0f;
        p.powerOutputMW = 340.0f;
        p.heatGenerationRate = 8.0f;
        p.maxShieldBonus = 40.0f;
        p.scrapCost = 1500;
        p.description = "Micro-singularity core siphoning void energy directly from folded sub-space dimensions.";
        RegisterPart(p);
    }

    // --- 2. RADIATORS ---
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Hyperion Cryo-Loop 3000";
        p.slot = PartSlot::RADIATOR_COOLER;
        p.rarity = PartRarity::STANDARD;
        p.manufacturer = FactionID::HYPERION_LOGISTICS;
        p.massKg = 400.0f;
        p.powerDrawMW = 15.0f;
        p.heatDissipationRate = 25.0f;
        p.scrapCost = 250;
        p.description = "Dual-stage freon cooling loop with automated heat-sink vents.";
        RegisterPart(p);
    }
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Vulcan Liquid-Nitrogen Heat Exchanger";
        p.slot = PartSlot::RADIATOR_COOLER;
        p.rarity = PartRarity::MILITARY_SPEC;
        p.manufacturer = FactionID::SOLAR_FORGE;
        p.massKg = 650.0f;
        p.powerDrawMW = 28.0f;
        p.heatDissipationRate = 48.0f;
        p.scrapCost = 650;
        p.description = "Heavy forged radiator blades radiating extreme thermal loads under full salvo.";
        RegisterPart(p);
    }
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Zero-Point Entropic Sink";
        p.slot = PartSlot::RADIATOR_COOLER;
        p.rarity = PartRarity::ANOMALOUS_VOID;
        p.manufacturer = FactionID::NULL_ORDER;
        p.massKg = 300.0f;
        p.powerDrawMW = 40.0f;
        p.heatDissipationRate = 75.0f;
        p.scrapCost = 1800;
        p.description = "Discharges excess heat entropy into microscopic parallel vacuum rifts.";
        RegisterPart(p);
    }

    // --- 3. ARMOR CHASSIS ---
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Aegis Composite Bastion Plating";
        p.slot = PartSlot::ARMOR_CHASSIS;
        p.rarity = PartRarity::STANDARD;
        p.manufacturer = FactionID::AEGIS_DIRECTORATE;
        p.massKg = 2500.0f;
        p.maxHealthBonus = 150.0f;
        p.kineticResist = 0.25f;
        p.energyResist = 0.15f;
        p.scrapCost = 400;
        p.description = "Layered titanium-carbide armor standard issue on Aegis frontliners.";
        RegisterPart(p);
    }
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Crimson Chobham Reactive Shell";
        p.slot = PartSlot::ARMOR_CHASSIS;
        p.rarity = PartRarity::REINFORCED;
        p.manufacturer = FactionID::CRIMSON_SYNDICATE;
        p.massKg = 2100.0f;
        p.maxHealthBonus = 120.0f;
        p.explosiveResist = 0.45f;
        p.criticalChanceBonus = 0.10f;
        p.scrapCost = 600;
        p.description = "Explosive reactive cassettes designed to neutralize incoming torpedoes.";
        RegisterPart(p);
    }
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Solar Forge Dense Adamantine Hull";
        p.slot = PartSlot::ARMOR_CHASSIS;
        p.rarity = PartRarity::PROTOTYPE;
        p.manufacturer = FactionID::SOLAR_FORGE;
        p.massKg = 3600.0f;
        p.maxHealthBonus = 320.0f;
        p.kineticResist = 0.40f;
        p.energyResist = 0.30f;
        p.explosiveResist = 0.35f;
        p.scrapCost = 1400;
        p.description = "Ultra-dense forged adamantine plating capable of withstanding direct point-blank impacts.";
        RegisterPart(p);
    }

    // --- 4. THRUSTER BOOSTERS ---
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Aegis Vector Ion Thruster Quad";
        p.slot = PartSlot::THRUSTER_BOOSTER;
        p.rarity = PartRarity::STANDARD;
        p.manufacturer = FactionID::AEGIS_DIRECTORATE;
        p.massKg = 800.0f;
        p.powerDrawMW = 30.0f;
        p.thrustNewtons = 85000.0f;
        p.scrapCost = 350;
        p.description = "Omni-directional vectoring thrusters offering balanced strafing and forward acceleration.";
        RegisterPart(p);
    }
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Crimson Voidfang Afterburner Rig";
        p.slot = PartSlot::THRUSTER_BOOSTER;
        p.rarity = PartRarity::MILITARY_SPEC;
        p.manufacturer = FactionID::CRIMSON_SYNDICATE;
        p.massKg = 600.0f;
        p.powerDrawMW = 45.0f;
        p.thrustNewtons = 135000.0f;
        p.heatGenerationRate = 18.0f;
        p.scrapCost = 800;
        p.description = "Illegal hydrazine afterburner injecting raw chemical accelerant for explosive bursts.";
        RegisterPart(p);
    }
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Gravimetric Warp Drive Thruster";
        p.slot = PartSlot::THRUSTER_BOOSTER;
        p.rarity = PartRarity::PROTOTYPE;
        p.manufacturer = FactionID::NULL_ORDER;
        p.massKg = 500.0f;
        p.powerDrawMW = 65.0f;
        p.thrustNewtons = 160000.0f;
        p.scrapCost = 1650;
        p.description = "Locally warps spacetime around the chassis, achieving instantaneous inertial redirection.";
        RegisterPart(p);
    }

    // --- 5. FIRE CONTROL SYSTEMS (FCS) ---
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Hyperion Target Lock-On Matrix";
        p.slot = PartSlot::FIRE_CONTROL_SYSTEM;
        p.rarity = PartRarity::STANDARD;
        p.manufacturer = FactionID::HYPERION_LOGISTICS;
        p.massKg = 250.0f;
        p.powerDrawMW = 18.0f;
        p.sensorRangeBonus = 400.0f;
        p.weaponCooldownMultiplier = 0.90f;
        p.scrapCost = 300;
        p.description = "Multi-target scanning array with predictive lead-target trajectory calculation.";
        RegisterPart(p);
    }
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Aegis Ballistic Autotune Processor";
        p.slot = PartSlot::FIRE_CONTROL_SYSTEM;
        p.rarity = PartRarity::MILITARY_SPEC;
        p.manufacturer = FactionID::AEGIS_DIRECTORATE;
        p.massKg = 320.0f;
        p.powerDrawMW = 24.0f;
        p.weaponDamageMultiplier = 1.25f;
        p.criticalChanceBonus = 0.15f;
        p.scrapCost = 750;
        p.description = "Synchronizes barrel harmonics and micro-thruster stabilization for maximum armor penetration.";
        RegisterPart(p);
    }

    // --- 6. UTILITY MODULES ---
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Aegis Phase Shield Generator";
        p.slot = PartSlot::UTILITY_MODULE;
        p.rarity = PartRarity::STANDARD;
        p.manufacturer = FactionID::AEGIS_DIRECTORATE;
        p.massKg = 500.0f;
        p.powerDrawMW = 35.0f;
        p.maxShieldBonus = 100.0f;
        p.shieldRegenBonus = 15.0f;
        p.scrapCost = 450;
        p.description = "Projected elliptical plasma envelope deflecting kinetic and energy rounds.";
        RegisterPart(p);
    }
    {
        MechPart p;
        p.id = nextId++;
        p.name = "Solar Forge Automated Repair Nanobots";
        p.slot = PartSlot::UTILITY_MODULE;
        p.rarity = PartRarity::MILITARY_SPEC;
        p.manufacturer = FactionID::SOLAR_FORGE;
        p.massKg = 600.0f;
        p.powerDrawMW = 20.0f;
        p.maxHealthBonus = 80.0f;
        p.kineticResist = 0.10f;
        p.scrapCost = 850;
        p.description = "Autonomous swarms of micro-welders continually sealing hull breaches.";
        RegisterPart(p);
    }
}

const MechPart* PartCatalog::GetPart(int id) const {
    for (const auto& part : m_parts) {
        if (part.id == id) return &part;
    }
    return nullptr;
}

const MechPart* PartCatalog::FindByName(const std::string& name) const {
    for (const auto& part : m_parts) {
        if (part.name == name) return &part;
    }
    return nullptr;
}

std::vector<const MechPart*> PartCatalog::GetPartsBySlot(PartSlot slot) const {
    std::vector<const MechPart*> res;
    for (const auto& part : m_parts) {
        if (part.slot == slot) res.push_back(&part);
    }
    return res;
}

std::vector<const MechPart*> PartCatalog::GetPartsByManufacturer(FactionID faction) const {
    std::vector<const MechPart*> res;
    for (const auto& part : m_parts) {
        if (part.manufacturer == faction) res.push_back(&part);
    }
    return res;
}

} // namespace Voidfall
