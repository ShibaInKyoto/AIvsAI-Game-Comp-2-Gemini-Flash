#include "gameplay/Weapons.h"
#include <cmath>

namespace Voidfall {

WeaponDefinition WeaponFactory::GetDefinition(WeaponType type) {
    WeaponDefinition def;
    def.type = type;

    switch (type) {
        case WeaponType::KINETIC_VULCAN:
            def.name = "Kinetic Rail-Vulcan";
            def.description = "Rapid-fire depleted uranium kinetic slugs with hyper velocity.";
            def.baseDamage = 14.0f;
            def.fireRate = 0.085f;
            def.heatCost = 2.4f;
            def.projectileSpeed = 850.0f;
            def.projectileRadius = 3.5f;
            def.projectileColor = {255, 210, 80, 255};
            def.projectilesPerShot = 1;
            def.spreadAngle = 0.05f;
            break;

        case WeaponType::PARTICLE_LANCE:
            def.name = "Particle Beam Lance";
            def.description = "Coherent thermal plasma bolt that pierces multiple hull plates.";
            def.baseDamage = 58.0f;
            def.fireRate = 0.42f;
            def.heatCost = 14.0f;
            def.projectileSpeed = 1600.0f;
            def.projectileRadius = 5.0f;
            def.projectileColor = {60, 220, 255, 255};
            def.projectilesPerShot = 1;
            def.pierceCount = 4;
            break;

        case WeaponType::FLAK_SHOTGUN:
            def.name = "Quantum Flak Cannon";
            def.description = "Fires a devastating arc of explosive micro-shrapnel pellets.";
            def.baseDamage = 16.0f;
            def.fireRate = 0.48f;
            def.heatCost = 15.0f;
            def.projectileSpeed = 680.0f;
            def.projectileRadius = 3.0f;
            def.projectileColor = {255, 120, 40, 255};
            def.projectilesPerShot = 7;
            def.spreadAngle = 0.42f;
            break;

        case WeaponType::ARC_TEMPEST:
            def.name = "Arc Tempest Emitter";
            def.description = "High-voltage lightning discharges that shatter energy barriers.";
            def.baseDamage = 32.0f;
            def.fireRate = 0.30f;
            def.heatCost = 9.5f;
            def.projectileSpeed = 950.0f;
            def.projectileRadius = 4.5f;
            def.projectileColor = {160, 100, 255, 255};
            def.projectilesPerShot = 2;
            def.spreadAngle = 0.15f;
            def.pierceCount = 2;
            break;

        case WeaponType::SWARM_MISSILE:
            def.name = "Hydra Swarm Missiles";
            def.description = "Salvo of micro-rockets with autonomous heat-seeking guidance.";
            def.baseDamage = 45.0f;
            def.fireRate = 0.95f;
            def.heatCost = 20.0f;
            def.projectileSpeed = 480.0f;
            def.projectileRadius = 4.0f;
            def.projectileColor = {255, 60, 90, 255};
            def.projectilesPerShot = 4;
            def.spreadAngle = 0.5f;
            def.isHoming = true;
            def.isExplosive = true;
            def.explosionRadius = 45.0f;
            break;

        case WeaponType::SINGULARITY_VORTEX:
            def.name = "Singularity Projector";
            def.description = "Compresses spacetime into an imploding gravity well.";
            def.baseDamage = 110.0f;
            def.fireRate = 1.8f;
            def.heatCost = 32.0f;
            def.projectileSpeed = 380.0f;
            def.projectileRadius = 8.0f;
            def.projectileColor = {180, 50, 255, 255};
            def.projectilesPerShot = 1;
            def.isExplosive = true;
            def.explosionRadius = 85.0f;
            break;

        case WeaponType::CRYO_RAIL:
            def.name = "Cryo-Stasis Rail Driver";
            def.description = "Extreme-range magnetic accelerator with cryogenic payload.";
            def.baseDamage = 160.0f;
            def.fireRate = 1.35f;
            def.heatCost = 26.0f;
            def.projectileSpeed = 1900.0f;
            def.projectileRadius = 6.0f;
            def.projectileColor = {100, 240, 240, 255};
            def.projectilesPerShot = 1;
            def.pierceCount = 3;
            break;
    }

    return def;
}

ProjectileManager::ProjectileManager(size_t reserveCount) {
    m_projectiles.reserve(reserveCount);
}

void ProjectileManager::Clear() {
    m_projectiles.clear();
}

void ProjectileManager::SpawnProjectile(const Projectile& p) {
    m_projectiles.push_back(p);
}

void ProjectileManager::Update(float dt) {
    for (size_t i = 0; i < m_projectiles.size(); ) {
        auto& p = m_projectiles[i];
        p.life -= dt;
        if (p.life <= 0.0f) {
            m_projectiles[i] = m_projectiles.back();
            m_projectiles.pop_back();
            continue;
        }

        // Homing missile logic
        if (p.isHoming && p.targetPos.LengthSq() > 0.1f) {
            Vec2 toTarget = (p.targetPos - p.pos).Normalized();
            float currentSpeed = p.vel.Length();
            Vec2 currentDir = p.vel.Normalized();

            // Smooth steering towards target
            float turnRate = 4.5f * dt;
            Vec2 newDir = Vec2::Lerp(currentDir, toTarget, turnRate).Normalized();
            p.vel = newDir * currentSpeed;
        }

        p.pos += p.vel * dt;
        ++i;
    }
}

void ProjectileManager::Draw() const {
    BeginBlendMode(BLEND_ADDITIVE);

    for (const auto& p : m_projectiles) {
        // Draw elongated glowing tracer in flight direction
        Vec2 dir = p.vel.Normalized();
        float speed = p.vel.Length();
        float tailLen = std::clamp(speed * 0.025f, 6.0f, 25.0f);

        Vec2 tailPos = p.pos - dir * tailLen;

        Color glowCol = p.color;
        glowCol.a = 180;
        DrawLineEx({tailPos.x, tailPos.y}, {p.pos.x, p.pos.y}, p.radius * 1.8f, glowCol);

        Color coreCol = {255, 255, 255, 255};
        DrawCircleV({p.pos.x, p.pos.y}, p.radius * 0.8f, coreCol);
    }

    EndBlendMode();
}

} // namespace Voidfall
