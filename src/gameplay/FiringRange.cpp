#include "gameplay/FiringRange.h"
#include "gameplay/PlayerMech.h"
#include "gameplay/Weapons.h"
#include "engine/ParticleSystem.h"
#include "engine/AudioSynth.h"
#include <cmath>
#include <algorithm>
#include <string>

namespace Voidfall {

FiringRange::FiringRange() {
    Init();
}

void FiringRange::Init() {
    m_dummies.clear();
    m_totalSessionDamage = 0.0f;
    m_peakDps = 0.0f;

    // Dummy 1: Light Mobile Target
    TargetDummy d1;
    d1.pos = {0.0f, -280.0f};
    d1.radius = 18.0f;
    d1.maxHp = 500.0f;
    d1.hp = d1.maxHp;
    d1.shield = 0.0f;
    d1.maxShield = 0.0f;
    d1.label = "Light Drone Dummy";
    m_dummies.push_back(d1);

    // Dummy 2: Armored Bulwark Target with Shield
    TargetDummy d2;
    d2.pos = {-220.0f, -320.0f};
    d2.radius = 28.0f;
    d2.maxHp = 1500.0f;
    d2.hp = d2.maxHp;
    d2.maxShield = 800.0f;
    d2.shield = d2.maxShield;
    d2.label = "Heavy Bulwark Dummy";
    m_dummies.push_back(d2);

    // Dummy 3: Super-Dreadnought Titan Hull
    TargetDummy d3;
    d3.pos = {220.0f, -320.0f};
    d3.radius = 45.0f;
    d3.maxHp = 5000.0f;
    d3.hp = d3.maxHp;
    d3.maxShield = 2500.0f;
    d3.shield = d3.maxShield;
    d3.label = "Titan Hull Dummy";
    m_dummies.push_back(d3);
}

void FiringRange::Update(float dt, PlayerMech& player, ProjectileManager& projManager, ParticleSystem& particles) {
    (void)player;

    for (auto& d : m_dummies) {
        // Regenerate shields and hp automatically
        if (d.shield < d.maxShield) {
            d.shield = std::min(d.maxShield, d.shield + 80.0f * dt);
        }
        if (d.hp < d.maxHp) {
            d.hp = std::min(d.maxHp, d.hp + 50.0f * dt);
        }

        // DPS calculation window (1 second rolling window)
        d.windowTimer += dt;
        if (d.windowTimer >= 1.0f) {
            d.dps = d.damageTakenInWindow / d.windowTimer;
            if (d.dps > m_peakDps) m_peakDps = d.dps;
            d.damageTakenInWindow = 0.0f;
            d.windowTimer = 0.0f;
        }

        // Projectile hit detection
        auto& projs = projManager.GetProjectiles();
        for (size_t pIdx = 0; pIdx < projs.size(); ++pIdx) {
            auto& p = projs[pIdx];
            if (p.isEnemy) continue;

            if (d.pos.DistanceSq(p.pos) <= (d.radius + p.radius) * (d.radius + p.radius)) {
                float dmg = p.damage;
                d.damageTakenInWindow += dmg;
                m_totalSessionDamage += dmg;

                if (d.shield > 0.0f) {
                    d.shield -= dmg;
                    particles.SpawnSpark(p.pos, p.vel * -0.3f, {60, 200, 255, 255}, 4.0f, 0.2f);
                } else {
                    d.hp -= dmg;
                    particles.SpawnSpark(p.pos, p.vel * -0.4f, {255, 180, 50, 255}, 4.5f, 0.25f);
                }

                particles.SpawnDamageNumber(d.pos, static_cast<int>(dmg), dmg > 45.0f);

                if (p.pierceCount > 0) {
                    p.pierceCount--;
                } else {
                    projs[pIdx] = projs.back();
                    projs.pop_back();
                    --pIdx;
                }
            }
        }
    }
}

void FiringRange::Draw() const {
    int screenW = GetScreenWidth();

    // Draw Targets
    for (const auto& d : m_dummies) {
        Vector2 c = {d.pos.x, d.pos.y};

        DrawPoly(c, 6, d.radius, 0.0f, {45, 50, 65, 255});
        DrawPolyLinesEx(c, 6, d.radius, 0.0f, 2.0f, {255, 220, 80, 255});

        if (d.shield > 0.0f) {
            DrawCircleLines(static_cast<int>(c.x), static_cast<int>(c.y), d.radius + 6.0f, {60, 200, 255, 200});
        }

        DrawText(d.label.c_str(), static_cast<int>(d.pos.x - d.label.length() * 3.5f), static_cast<int>(d.pos.y - d.radius - 20), 12, {220, 230, 250, 255});
        DrawText(("DPS: " + std::to_string(static_cast<int>(d.dps))).c_str(), static_cast<int>(d.pos.x - 25), static_cast<int>(d.pos.y + d.radius + 8), 13, {255, 140, 50, 255});
    }

    // Benchmark HUD Overlay
    DrawRectangle(screenW / 2 - 160, 15, 320, 55, {15, 22, 35, 220});
    DrawRectangleLines(screenW / 2 - 160, 15, 320, 55, {60, 100, 160, 180});
    DrawText("WEAPONS BENCHMARK SANDBOX", screenW / 2 - 130, 22, 14, {100, 240, 255, 255});
    DrawText(("PEAK DPS: " + std::to_string(static_cast<int>(m_peakDps)) + "  |  TOTAL DMG: " + std::to_string(static_cast<int>(m_totalSessionDamage))).c_str(), screenW / 2 - 145, 44, 12, {255, 220, 100, 255});
}

} // namespace Voidfall
