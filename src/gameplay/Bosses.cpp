#include "gameplay/Bosses.h"
#include "gameplay/PlayerMech.h"
#include "gameplay/Weapons.h"
#include "engine/ParticleSystem.h"
#include "engine/AudioSynth.h"
#include "core/Random.h"
#include <cmath>
#include <algorithm>

namespace Voidfall {

BossEntity::BossEntity() = default;

void BossEntity::Init(BossID id, const Vec2& spawnPos) {
    m_id = id;
    m_pos = spawnPos;
    m_vel = {0.0f, 0.0f};
    m_rotation = 0.0f;
    m_phase = 1;
    m_attackTimer = 0.0f;
    m_specialTimer = 0.0f;
    m_beamChargeTimer = 0.0f;
    m_isFiringBeam = false;
    m_isActive = true;
    m_isDefeated = false;
    m_subsystems.clear();

    switch (id) {
        case BossID::GOLIATH_IV:
            m_name = "GOLIATH-IV DREAD FORTRESS";
            m_radius = 58.0f;
            m_maxHp = 1400.0f;
            m_maxShield = 500.0f;
            // 4 Core Shield Nodes
            m_subsystems.push_back({"Port Shield Gen", {-35.0f, -35.0f}, 16.0f, 220.0f, 220.0f, false});
            m_subsystems.push_back({"Starboard Shield Gen", {35.0f, -35.0f}, 16.0f, 220.0f, 220.0f, false});
            m_subsystems.push_back({"Aft Power Node", {-35.0f, 35.0f}, 16.0f, 220.0f, 220.0f, false});
            m_subsystems.push_back({"Bow Battery", {35.0f, 35.0f}, 16.0f, 220.0f, 220.0f, false});
            break;

        case BossID::AETHEL_CORE:
            m_name = "AETHEL-CORE ORBITAL CARRIER";
            m_radius = 65.0f;
            m_maxHp = 2200.0f;
            m_maxShield = 800.0f;
            m_subsystems.push_back({"Launch Bay Alpha", {-45.0f, 0.0f}, 20.0f, 300.0f, 300.0f, false});
            m_subsystems.push_back({"Launch Bay Beta", {45.0f, 0.0f}, 20.0f, 300.0f, 300.0f, false});
            break;

        case BossID::PROTOCOL_ZERO_TITAN:
            m_name = "PROTOCOL ZERO: SOVEREIGN TITAN";
            m_radius = 70.0f;
            m_maxHp = 3200.0f;
            m_maxShield = 1200.0f;
            break;
    }

    m_hp = m_maxHp;
    m_shield = m_maxShield;
    AudioSynth::Play(SoundID::BOSS_WARNING, 1.4f, 0.9f);
}

void BossEntity::Update(float dt, PlayerMech& player, ProjectileManager& projManager, ParticleSystem& particles) {
    if (!m_isActive || m_isDefeated) return;

    m_rotation += 0.4f * dt;
    m_attackTimer -= dt;
    m_specialTimer -= dt;

    // Check phase transitions
    float hpPct = GetHpRatio();
    if (hpPct <= 0.6f && m_phase == 1) {
        m_phase = 2;
        AudioSynth::Play(SoundID::BOSS_WARNING, 1.2f, 1.1f);
        particles.SpawnShockwave(m_pos, 160.0f, {255, 60, 60, 255}, 0.5f);
        particles.SpawnFloatingText(m_pos, "PHASE II: OVERDRIVE!", {255, 50, 50, 255});
    } else if (hpPct <= 0.25f && m_phase == 2) {
        m_phase = 3;
        AudioSynth::Play(SoundID::BOSS_WARNING, 1.3f, 1.3f);
        particles.SpawnShockwave(m_pos, 220.0f, {255, 20, 20, 255}, 0.6f);
        particles.SpawnFloatingText(m_pos, "CRITICAL MELTDOWN PROTOCOL!", {255, 20, 20, 255});
    }

    switch (m_id) {
        case BossID::GOLIATH_IV:
            UpdateGoliath(dt, player, projManager, particles);
            break;
        case BossID::AETHEL_CORE:
            UpdateAethelCore(dt, player, projManager, particles);
            break;
        case BossID::PROTOCOL_ZERO_TITAN:
            UpdateProtocolZero(dt, player, projManager, particles);
            break;
    }

    // Check player projectiles hitting boss and subsystems
    auto& projs = projManager.GetProjectiles();
    for (size_t pIdx = 0; pIdx < projs.size(); ++pIdx) {
        auto& p = projs[pIdx];
        if (p.isEnemy) continue;

        bool hitSomething = false;

        // Subsystems hit test
        for (auto& sub : m_subsystems) {
            if (sub.isDestroyed) continue;
            Vec2 subWorldPos = m_pos + sub.relativeOffset.Rotated(m_rotation);
            if (subWorldPos.DistanceSq(p.pos) <= (sub.radius + p.radius) * (sub.radius + p.radius)) {
                sub.hp -= p.damage;
                hitSomething = true;
                particles.SpawnSpark(p.pos, p.vel * -0.3f, {255, 180, 50, 255}, 4.0f, 0.2f);
                particles.SpawnDamageNumber(subWorldPos, static_cast<int>(p.damage));

                if (sub.hp <= 0.0f) {
                    sub.isDestroyed = true;
                    AudioSynth::Play(SoundID::EXPLOSION_LARGE, 1.1f, 1.1f);
                    particles.SpawnExplosion(subWorldPos, 25, {255, 100, 30, 255}, 45.0f);
                    particles.SpawnFloatingText(subWorldPos, sub.name + " DESTROYED!", {255, 200, 80, 255});
                }
                break;
            }
        }

        // Main hull hit test
        if (!hitSomething && m_pos.DistanceSq(p.pos) <= (m_radius + p.radius) * (m_radius + p.radius)) {
            hitSomething = true;

            // Shield check: If subsystems remain, boss shields reduce damage by 60%
            bool shieldsBoosted = false;
            for (const auto& s : m_subsystems) {
                if (!s.isDestroyed) { shieldsBoosted = true; break; }
            }

            float finalDamage = shieldsBoosted ? (p.damage * 0.4f) : p.damage;

            if (m_shield > 0.0f) {
                m_shield -= finalDamage;
                particles.SpawnSpark(p.pos, p.vel * -0.4f, {80, 220, 255, 255}, 4.5f, 0.25f);
                if (m_shield < 0.0f) {
                    m_hp += m_shield;
                    m_shield = 0.0f;
                }
            } else {
                m_hp -= finalDamage;
                particles.SpawnSpark(p.pos, p.vel * -0.4f, {255, 120, 40, 255}, 5.0f, 0.25f);
            }

            particles.SpawnDamageNumber(m_pos, static_cast<int>(finalDamage), finalDamage > 50.0f);

            if (m_hp <= 0.0f) {
                m_hp = 0.0f;
                m_isDefeated = true;
                AudioSynth::Play(SoundID::EXPLOSION_LARGE, 1.5f, 0.7f);
                particles.SpawnExplosion(m_pos, 80, {255, 200, 80, 255}, 120.0f);
                particles.SpawnFloatingText(m_pos, "TITAN ANNIHILATED!", {255, 240, 60, 255});
                player.AddNaniteCores(500);
            }
        }

        if (hitSomething) {
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

void BossEntity::UpdateGoliath(float dt, PlayerMech& player, ProjectileManager& projManager, ParticleSystem& particles) {
    (void)particles;
    Vec2 toPlayer = (player.GetPosition() - m_pos).Normalized();

    // Attack 1: Rotating 8-way flak barrage
    if (m_attackTimer <= 0.0f) {
        m_attackTimer = (m_phase == 1) ? 1.6f : 0.95f;
        int bulletCount = (m_phase == 1) ? 8 : 12;
        for (int i = 0; i < bulletCount; ++i) {
            float angle = m_rotation + (6.283185f / bulletCount) * i;
            Vec2 bDir = Vec2::FromAngle(angle);

            Projectile p;
            p.pos = m_pos + bDir * (m_radius + 10.0f);
            p.vel = bDir * 320.0f;
            p.damage = 18.0f;
            p.radius = 5.0f;
            p.color = {255, 140, 40, 255};
            p.isEnemy = true;
            projManager.SpawnProjectile(p);
        }
        AudioSynth::Play(SoundID::SHOOT_FLAK, 0.7f, 0.8f);
    }

    // Special: Mortar bombardment at player location
    if (m_specialTimer <= 0.0f) {
        m_specialTimer = (m_phase == 1) ? 4.5f : 2.8f;
        for (int i = 0; i < 3; ++i) {
            Vec2 spreadDir = toPlayer.Rotated(Random::RangeF(-0.3f, 0.3f));
            Projectile p;
            p.pos = m_pos + spreadDir * (m_radius + 12.0f);
            p.vel = spreadDir * 420.0f;
            p.damage = 28.0f;
            p.radius = 7.0f;
            p.color = {255, 40, 40, 255};
            p.isEnemy = true;
            p.isExplosive = true;
            p.explosionRadius = 45.0f;
            projManager.SpawnProjectile(p);
        }
    }
}

void BossEntity::UpdateAethelCore(float dt, PlayerMech& player, ProjectileManager& projManager, ParticleSystem& particles) {
    (void)dt;
    (void)particles;
    Vec2 toPlayer = (player.GetPosition() - m_pos).Normalized();

    // Spiral Laser flower pattern
    if (m_attackTimer <= 0.0f) {
        m_attackTimer = 0.12f;
        static float spiralAngle = 0.0f;
        spiralAngle += 0.35f;

        for (int arm = 0; arm < 3; ++arm) {
            float angle = spiralAngle + (6.283185f / 3.0f) * arm;
            Vec2 dir = Vec2::FromAngle(angle);

            Projectile p;
            p.pos = m_pos + dir * (m_radius + 8.0f);
            p.vel = dir * 360.0f;
            p.damage = 14.0f;
            p.radius = 4.0f;
            p.color = {120, 220, 255, 255};
            p.isEnemy = true;
            projManager.SpawnProjectile(p);
        }
    }

    // Seeker missile salvos
    if (m_specialTimer <= 0.0f) {
        m_specialTimer = 3.5f;
        for (int i = 0; i < 4; ++i) {
            Vec2 mDir = toPlayer.Rotated(Random::RangeF(-0.6f, 0.6f));
            Projectile p;
            p.pos = m_pos + mDir * (m_radius + 15.0f);
            p.vel = mDir * 280.0f;
            p.damage = 25.0f;
            p.radius = 5.0f;
            p.color = {255, 80, 140, 255};
            p.isEnemy = true;
            p.isHoming = true;
            p.targetPos = player.GetPosition();
            p.isExplosive = true;
            p.explosionRadius = 35.0f;
            projManager.SpawnProjectile(p);
        }
        AudioSynth::Play(SoundID::SHOOT_MISSILE, 1.0f, 0.85f);
    }
}

void BossEntity::UpdateProtocolZero(float dt, PlayerMech& player, ProjectileManager& projManager, ParticleSystem& particles) {
    (void)dt;
    (void)particles;
    Vec2 toPlayer = (player.GetPosition() - m_pos).Normalized();

    // High velocity particle spreads
    if (m_attackTimer <= 0.0f) {
        m_attackTimer = (m_phase == 3) ? 0.45f : 0.8f;
        int count = (m_phase == 3) ? 16 : 10;
        for (int i = 0; i < count; ++i) {
            float angle = (6.283185f / count) * i + m_rotation;
            Vec2 dir = Vec2::FromAngle(angle);

            Projectile p;
            p.pos = m_pos + dir * (m_radius + 10.0f);
            p.vel = dir * 420.0f;
            p.damage = 20.0f;
            p.radius = 5.0f;
            p.color = {255, 50, 80, 255};
            p.isEnemy = true;
            projManager.SpawnProjectile(p);
        }
    }

    // Phase 3 Singularity shockwave
    if (m_specialTimer <= 0.0f) {
        m_specialTimer = 2.4f;
        for (int s = -2; s <= 2; ++s) {
            Vec2 sDir = toPlayer.Rotated(s * 0.15f);
            Projectile p;
            p.pos = m_pos + sDir * (m_radius + 15.0f);
            p.vel = sDir * 550.0f;
            p.damage = 32.0f;
            p.radius = 7.0f;
            p.color = {200, 40, 255, 255};
            p.isEnemy = true;
            p.isExplosive = true;
            p.explosionRadius = 45.0f;
            projManager.SpawnProjectile(p);
        }
    }
}

void BossEntity::Draw() const {
    if (!m_isActive || m_isDefeated) return;

    Vector2 center = {m_pos.x, m_pos.y};

    // Outer Armored Hull
    Color hullColor = {50, 40, 60, 255};
    Color glowColor = {255, 60, 60, 255};
    if (m_phase == 2) glowColor = {255, 140, 40, 255};
    if (m_phase == 3) glowColor = {255, 40, 220, 255};

    DrawPoly(center, 8, m_radius, m_rotation * 57.29578f, hullColor);
    DrawPolyLinesEx(center, 8, m_radius, m_rotation * 57.29578f, 4.0f, glowColor);

    // Glowing Core Reactor
    Color coreCol = {255, 80, 80, 255};
    DrawCircleV(center, m_radius * 0.4f, coreCol);

    // Subsystems
    for (const auto& sub : m_subsystems) {
        Vec2 subPos = m_pos + sub.relativeOffset.Rotated(m_rotation);
        Vector2 subV = {subPos.x, subPos.y};

        if (sub.isDestroyed) {
            DrawCircleV(subV, sub.radius * 0.7f, {40, 40, 40, 255});
            DrawCircleLinesV(subV, sub.radius, {80, 80, 80, 255});
        } else {
            DrawCircleV(subV, sub.radius, {70, 90, 130, 255});
            DrawCircleLinesV(subV, sub.radius, {80, 220, 255, 255});
        }
    }

    // Shield Dome
    if (m_shield > 0.0f) {
        BeginBlendMode(BLEND_ADDITIVE);
        float shieldAlpha = std::clamp(m_shield / m_maxShield, 0.2f, 1.0f);
        Color shieldCol = {60, 180, 255, static_cast<unsigned char>(140 * shieldAlpha)};
        DrawCircleLinesV(center, m_radius + 12.0f, shieldCol);
        EndBlendMode();
    }
}

} // namespace Voidfall
