#include "gameplay/Drones.h"
#include "gameplay/Enemies.h"
#include "gameplay/Weapons.h"
#include "engine/ParticleSystem.h"
#include "engine/AudioSynth.h"
#include <cmath>

namespace Voidfall {

DroneManager::DroneManager() {
    m_drones.reserve(8);
}

void DroneManager::Clear() {
    m_drones.clear();
}

void DroneManager::AddDrone(DroneType type) {
    if (m_drones.size() >= 4) return;
    DroneUnit d;
    d.type = type;
    d.orbitAngle = static_cast<float>(m_drones.size()) * 1.57f;
    d.orbitRadius = 55.0f;
    d.orbitSpeed = 2.2f;
    m_drones.push_back(d);
}

void DroneManager::Update(float dt, const Vec2& playerPos, EnemyManager& enemies, ProjectileManager& projManager, ParticleSystem& particles) {
    for (auto& d : m_drones) {
        d.orbitAngle += d.orbitSpeed * dt;
        d.pos = playerPos + Vec2::FromAngle(d.orbitAngle, d.orbitRadius);
        d.fireTimer -= dt;

        if (d.type == DroneType::ATTACK_DRONE && d.fireTimer <= 0.0f) {
            // Find nearest alive enemy within 380 units
            EnemyUnit* closestEnemy = nullptr;
            float minDistSq = 380.0f * 380.0f;

            for (auto& e : enemies.GetEnemies()) {
                if (!e.isAlive) continue;
                float dSq = d.pos.DistanceSq(e.pos);
                if (dSq < minDistSq) {
                    minDistSq = dSq;
                    closestEnemy = &e;
                }
            }

            if (closestEnemy) {
                d.fireTimer = d.fireCooldown;
                Vec2 dir = (closestEnemy->pos - d.pos).Normalized();

                Projectile p;
                p.pos = d.pos + dir * 8.0f;
                p.vel = dir * 720.0f;
                p.damage = 10.0f;
                p.radius = 3.0f;
                p.color = {100, 240, 255, 255};
                p.isEnemy = false;
                projManager.SpawnProjectile(p);

                particles.SpawnSpark(p.pos, dir * 50.0f, {80, 220, 255, 255}, 3.0f, 0.15f);
            }
        }
    }
}

void DroneManager::Draw() const {
    for (const auto& d : m_drones) {
        Vector2 c = {d.pos.x, d.pos.y};

        Color droneCol = {80, 220, 255, 255};
        if (d.type == DroneType::SHIELD_DRONE) droneCol = {60, 255, 160, 255};
        if (d.type == DroneType::EMP_SENTRY) droneCol = {220, 100, 255, 255};

        DrawPoly(c, 4, 8.0f, d.orbitAngle * 57.29578f, {30, 40, 60, 255});
        DrawPolyLinesEx(c, 4, 8.0f, d.orbitAngle * 57.29578f, 1.5f, droneCol);
        DrawCircleV(c, 3.0f, droneCol);
    }
}

} // namespace Voidfall
