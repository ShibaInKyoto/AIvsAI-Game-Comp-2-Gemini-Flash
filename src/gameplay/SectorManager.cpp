#include "gameplay/SectorManager.h"
#include "gameplay/PlayerMech.h"
#include "gameplay/Enemies.h"
#include "gameplay/Bosses.h"
#include "gameplay/Weapons.h"
#include "engine/ParticleSystem.h"
#include "engine/AudioSynth.h"
#include "core/Random.h"
#include <cmath>
#include <algorithm>

namespace Voidfall {

SectorManager::SectorManager() {
    InitSector(SectorType::ASTEROID_OUTPOST, 1);
}

void SectorManager::InitSector(SectorType type, int sectorIndex) {
    m_currentType = type;
    m_sectorIndex = sectorIndex;
    m_currentWave = 1;
    m_sectorComplete = false;
    m_waveActive = false;
    m_waveCooldownTimer = 1.5f;

    switch (type) {
        case SectorType::ASTEROID_OUTPOST:
            m_maxWaves = 3;
            m_arenaRadius = 1100.0f;
            break;
        case SectorType::DERELICT_SHIPYARD:
            m_maxWaves = 4;
            m_arenaRadius = 1350.0f;
            break;
        case SectorType::TITAN_CORE_FLAGSHIP:
            m_maxWaves = 4;
            m_arenaRadius = 1500.0f;
            break;
        default:
            m_maxWaves = 3;
            m_arenaRadius = 1200.0f;
            break;
    }

    m_beacon.pos = {0.0f, 0.0f};
    m_beacon.charge = 0.0f;
    m_beacon.isActive = false;
    m_beacon.isCompleted = false;

    GenerateAsteroids();
}

void SectorManager::GenerateAsteroids() {
    m_asteroids.clear();
    int count = 18 + m_sectorIndex * 5;

    for (int i = 0; i < count; ++i) {
        float angle = Random::RangeF(0.0f, 6.283185f);
        float dist = Random::RangeF(250.0f, m_arenaRadius * 0.88f);

        Asteroid ast;
        ast.pos = Vec2::FromAngle(angle, dist);
        ast.radius = Random::RangeF(22.0f, 48.0f);
        ast.hp = ast.radius * 3.5f;
        ast.rotation = Random::RangeF(0.0f, 6.28f);
        ast.rotSpeed = Random::RangeF(-0.3f, 0.3f);
        ast.color = {
            static_cast<unsigned char>(Random::RangeI(65, 85)),
            static_cast<unsigned char>(Random::RangeI(70, 95)),
            static_cast<unsigned char>(Random::RangeI(85, 115)),
            255
        };
        m_asteroids.push_back(ast);
    }
}

void SectorManager::ProceedToNextSector() {
    if (m_sectorIndex == 1) {
        InitSector(SectorType::DERELICT_SHIPYARD, 2);
    } else if (m_sectorIndex == 2) {
        InitSector(SectorType::TITAN_CORE_FLAGSHIP, 3);
    }
}

void SectorManager::Update(float dt, PlayerMech& player, EnemyManager& enemies, BossEntity& boss, ProjectileManager& projManager, ParticleSystem& particles) {
    // Rotate asteroids
    for (auto& ast : m_asteroids) {
        ast.rotation += ast.rotSpeed * dt;
    }

    // Player vs Arena Boundary constraint
    Vec2 pPos = player.GetPosition();
    if (pPos.Length() > m_arenaRadius) {
        Vec2 clampedPos = pPos.Normalized() * m_arenaRadius;
        // Minor boundary bounce
        player.TakeDamage(10.0f * dt, pPos * -1.0f, particles);
        particles.SpawnSpark(clampedPos, pPos.Normalized() * -50.0f, {80, 200, 255, 255}, 3.0f, 0.2f);
    }

    // Projectile vs Asteroid collisions
    auto& projs = projManager.GetProjectiles();
    for (size_t pIdx = 0; pIdx < projs.size(); ++pIdx) {
        auto& p = projs[pIdx];
        for (auto& ast : m_asteroids) {
            if (ast.hp <= 0.0f) continue;

            if (p.pos.DistanceSq(ast.pos) <= (ast.radius + p.radius) * (ast.radius + p.radius)) {
                ast.hp -= p.damage;
                particles.SpawnSpark(p.pos, p.vel * -0.3f, {180, 190, 210, 255}, 3.5f, 0.2f);

                if (ast.hp <= 0.0f) {
                    AudioSynth::Play(SoundID::EXPLOSION_SMALL, 0.7f, 1.3f);
                    particles.SpawnExplosion(ast.pos, 14, {140, 145, 160, 255}, ast.radius);
                    player.AddNaniteCores(5);
                }

                projs[pIdx] = projs.back();
                projs.pop_back();
                --pIdx;
                break;
            }
        }
    }

    // Remove destroyed asteroids
    m_asteroids.erase(
        std::remove_if(m_asteroids.begin(), m_asteroids.end(), [](const Asteroid& a) { return a.hp <= 0.0f; }),
        m_asteroids.end()
    );

    // Wave spawning and progression logic
    if (!m_waveActive && !boss.IsActive()) {
        m_waveCooldownTimer -= dt;
        if (m_waveCooldownTimer <= 0.0f) {
            if (m_currentWave <= m_maxWaves) {
                enemies.SpawnWave(m_currentWave, player.GetPosition(), m_arenaRadius);
                particles.SpawnFloatingText(player.GetPosition(), "WAVE " + std::to_string(m_currentWave) + " ENGAGING", {255, 100, 100, 255});
                m_waveActive = true;
            } else {
                // Spawn Sector Boss!
                BossID bId = BossID::GOLIATH_IV;
                if (m_sectorIndex == 2) bId = BossID::AETHEL_CORE;
                if (m_sectorIndex == 3) bId = BossID::PROTOCOL_ZERO_TITAN;

                boss.Init(bId, {0.0f, -400.0f});
                particles.SpawnFloatingText(player.GetPosition(), "WARNING: TITAN DETECTED", {255, 30, 30, 255});
            }
        }
    } else if (m_waveActive) {
        // Check if wave enemies are cleared
        if (enemies.GetActiveCount() == 0) {
            m_waveActive = false;
            m_currentWave++;
            m_waveCooldownTimer = 2.5f;
            AudioSynth::Play(SoundID::UPGRADE_SELECT, 0.8f, 1.1f);
            particles.SpawnFloatingText(player.GetPosition(), "WAVE CLEARED", {100, 255, 120, 255});
        }
    }

    // Check Boss Defeat -> Activate Extraction Beacon
    if (boss.IsActive() && boss.IsDefeated() && !m_beacon.isActive) {
        m_beacon.isActive = true;
        m_beacon.pos = boss.GetPosition();
        AudioSynth::Play(SoundID::UPGRADE_SELECT, 1.2f, 1.0f);
        particles.SpawnFloatingText(m_beacon.pos, "EXTRACTION BEACON ONLINE", {80, 240, 255, 255});
    }

    // Extraction Beacon Charging
    if (m_beacon.isActive && !m_sectorComplete) {
        float distToBeacon = player.GetPosition().Distance(m_beacon.pos);
        if (distToBeacon <= m_beacon.radius) {
            m_beacon.charge += dt;
            particles.SpawnPlasma(m_beacon.pos, Vec2(Random::RangeF(-30, 30), Random::RangeF(-30, 30)), {80, 220, 255, 200}, 5.0f, 0.25f);

            if (m_beacon.charge >= m_beacon.maxCharge) {
                m_beacon.isCompleted = true;
                m_sectorComplete = true;
                AudioSynth::Play(SoundID::UPGRADE_SELECT, 1.5f, 1.2f);
                particles.SpawnShockwave(m_beacon.pos, 180.0f, {100, 255, 150, 255}, 0.6f);
            }
        }
    }
}

void SectorManager::Draw() const {
    // Draw Arena Boundary Ring
    BeginBlendMode(BLEND_ADDITIVE);
    Color boundColor = {40, 80, 160, 90};
    if (m_sectorIndex == 2) boundColor = {120, 40, 160, 90};
    if (m_sectorIndex == 3) boundColor = {160, 40, 60, 90};

    DrawCircleLines(0, 0, m_arenaRadius, boundColor);
    DrawCircleLines(0, 0, m_arenaRadius + 6.0f, boundColor);
    EndBlendMode();

    // Draw Asteroids
    for (const auto& ast : m_asteroids) {
        Vector2 c = {ast.pos.x, ast.pos.y};
        DrawPoly(c, 7, ast.radius, ast.rotation * 57.29578f, ast.color);
        Color outlineCol = {
            static_cast<unsigned char>(std::min(255, ast.color.r + 30)),
            static_cast<unsigned char>(std::min(255, ast.color.g + 30)),
            static_cast<unsigned char>(std::min(255, ast.color.b + 30)),
            255
        };
        DrawPolyLinesEx(c, 7, ast.radius, ast.rotation * 57.29578f, 2.0f, outlineCol);
    }

    // Draw Extraction Beacon
    if (m_beacon.isActive) {
        Vector2 bV = {m_beacon.pos.x, m_beacon.pos.y};
        float pct = std::clamp(m_beacon.charge / m_beacon.maxCharge, 0.0f, 1.0f);

        BeginBlendMode(BLEND_ADDITIVE);
        DrawCircleLines(static_cast<int>(bV.x), static_cast<int>(bV.y), m_beacon.radius, {60, 220, 255, 180});
        DrawCircle(static_cast<int>(bV.x), static_cast<int>(bV.y), m_beacon.radius * pct, {60, 220, 255, 90});
        EndBlendMode();

        DrawText("EXTRACTION ZONE", static_cast<int>(bV.x - 65), static_cast<int>(bV.y - m_beacon.radius - 18), 14, {180, 240, 255, 255});
    }
}

} // namespace Voidfall
