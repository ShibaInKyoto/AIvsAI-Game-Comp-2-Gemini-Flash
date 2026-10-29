#include "gameplay/PlayerMech.h"
#include "engine/Input.h"
#include "engine/ParticleSystem.h"
#include "engine/AudioSynth.h"
#include "core/Random.h"
#include <cmath>
#include <algorithm>

namespace Voidfall {

PlayerMech::PlayerMech() {
    Reset(MechFrame::STRIKER);
}

void PlayerMech::Reset(MechFrame frame) {
    m_frame = frame;
    m_pos = {0.0f, 0.0f};
    m_vel = {0.0f, 0.0f};
    m_facingAngle = 0.0f;
    m_aimAngle = 0.0f;

    switch (frame) {
        case MechFrame::TITAN:
            m_maxHull = 160.0f;
            m_maxShield = 130.0f;
            m_baseSpeed = 220.0f;
            m_collisionRadius = 20.0f;
            m_shieldArc = 2.4f; // ~140 degrees
            break;
        case MechFrame::PHANTOM:
            m_maxHull = 75.0f;
            m_maxShield = 60.0f;
            m_baseSpeed = 330.0f;
            m_collisionRadius = 14.0f;
            m_dashDuration = 0.28f;
            break;
        case MechFrame::AEGIS:
            m_maxHull = 110.0f;
            m_maxShield = 110.0f;
            m_baseSpeed = 260.0f;
            m_shieldRegenDelay = 1.6f;
            break;
        case MechFrame::STRIKER:
        default:
            m_maxHull = 100.0f;
            m_maxShield = 80.0f;
            m_baseSpeed = 280.0f;
            m_collisionRadius = 16.0f;
            break;
    }

    m_hull = m_maxHull;
    m_shield = m_maxShield;
    m_dashCooldownTimer = 0.0f;
    m_dashTimer = 0.0f;
    m_primaryCooldownTimer = 0.0f;
    m_ordnanceCooldownTimer = 0.0f;
    m_reactor.SetProfile(PowerProfile::BALANCED);
}

void PlayerMech::Heal(float amount) {
    m_hull = std::min(m_maxHull, m_hull + amount);
}

void PlayerMech::TakeDamage(float amount, const Vec2& hitDir, ParticleSystem& particles) {
    if (m_dashTimer > 0.0f) {
        // Invulnerable during phase blink
        particles.SpawnFloatingText(m_pos, "PHASE EVADE", {100, 240, 255, 255});
        return;
    }

    // Check if incoming attack hits the directional shield arc
    Vec2 toAttacker = hitDir.Normalized();
    Vec2 shieldForward = Vec2::FromAngle(m_aimAngle);
    float dot = shieldForward.Dot(toAttacker);

    // Angle test for directional shield
    float minDot = std::cos(m_shieldArc * 0.5f);
    bool hitShield = (m_shield > 0.0f) && (dot >= minDot);

    if (hitShield) {
        // Perfect parry check if shield was recently activated or reactor is Bulwark
        if (m_reactor.GetProfile() == PowerProfile::BULWARK && Random::Bool(0.25f)) {
            AudioSynth::Play(SoundID::SHIELD_PARRY, 1.2f, 1.1f);
            particles.SpawnShockwave(m_pos + shieldForward * 18.0f, 40.0f, {80, 240, 255, 255}, 0.25f);
            particles.SpawnFloatingText(m_pos, "PERFECT DEFLECT", {80, 255, 255, 255});
            return;
        }

        AudioSynth::Play(SoundID::SHIELD_HIT, 0.9f, Random::RangeF(0.95f, 1.05f));
        m_shield -= amount;
        m_shieldRegenTimer = m_shieldRegenDelay;

        // Shield impact sparks
        Vec2 sparkPos = m_pos + shieldForward * 18.0f;
        particles.SpawnSpark(sparkPos, toAttacker * -80.0f, {60, 200, 255, 255}, 4.0f, 0.3f);

        if (m_shield < 0.0f) {
            float bleedThrough = -m_shield;
            m_shield = 0.0f;
            m_hull -= bleedThrough;
            particles.SpawnShockwave(m_pos, 35.0f, {255, 100, 100, 255}, 0.3f);
            particles.SpawnFloatingText(m_pos, "SHIELD COLLAPSED", {255, 80, 80, 255});
        }
    } else {
        // Direct hull damage
        m_hull -= amount;
        m_shieldRegenTimer = m_shieldRegenDelay;
        AudioSynth::Play(SoundID::EXPLOSION_SMALL, 0.7f, 1.4f);
        particles.SpawnExplosion(m_pos, 10, {255, 120, 50, 255}, 20.0f);
        particles.SpawnDamageNumber(m_pos, static_cast<int>(amount), true);
    }

    if (m_hull <= 0.0f) {
        m_hull = 0.0f;
        AudioSynth::Play(SoundID::EXPLOSION_LARGE, 1.3f, 0.9f);
        particles.SpawnExplosion(m_pos, 40, {255, 80, 40, 255}, 65.0f);
    }
}

void PlayerMech::ExecuteDash(ParticleSystem& particles) {
    auto& input = Input::Get();
    Vec2 dir = input.moveDir.LengthSq() > 0.01f ? input.moveDir : Vec2::FromAngle(m_aimAngle);

    m_dashDir = dir.Normalized();
    m_dashTimer = m_dashDuration;
    m_dashCooldownTimer = m_dashCooldown * m_reactor.GetDashCooldownMult();

    m_reactor.AddHeat(10.0f);
    AudioSynth::Play(SoundID::DASH, 1.0f, Random::RangeF(0.95f, 1.05f));

    // Dash trail effect
    particles.SpawnShockwave(m_pos, 30.0f, {100, 220, 255, 255}, 0.25f);
    for (int i = 0; i < 8; ++i) {
        particles.SpawnPlasma(m_pos, m_dashDir * -60.0f + Vec2(Random::RangeF(-20, 20), Random::RangeF(-20, 20)), {80, 200, 255, 200}, 5.0f, 0.35f);
    }
}

void PlayerMech::ExecuteHeatPurge(ProjectileManager& projManager, ParticleSystem& particles) {
    bool isSweetSpot = false;
    if (m_reactor.TryActivePurge(isSweetSpot)) {
        if (isSweetSpot) {
            AudioSynth::Play(SoundID::HEAT_PURGE_SUCCESS, 1.2f, 1.2f);
            particles.SpawnShockwave(m_pos, 110.0f, {255, 220, 80, 255}, 0.4f);
            particles.SpawnFloatingText(m_pos, "FLASH PURGE!", {255, 240, 80, 255});

            // Wipe nearby hostile projectiles in shockwave blast
            auto& projs = projManager.GetProjectiles();
            for (size_t i = 0; i < projs.size(); ) {
                if (projs[i].isEnemy && projs[i].pos.DistanceSq(m_pos) <= (110.0f * 110.0f)) {
                    particles.SpawnSpark(projs[i].pos, projs[i].vel * -0.4f, {255, 220, 100, 255}, 3.0f, 0.2f);
                    projs[i] = projs.back();
                    projs.pop_back();
                } else {
                    ++i;
                }
            }
        } else {
            AudioSynth::Play(SoundID::HEAT_PURGE_SUCCESS, 0.8f, 0.9f);
            particles.SpawnShockwave(m_pos, 60.0f, {200, 150, 80, 255}, 0.3f);
            particles.SpawnFloatingText(m_pos, "THERMAL FLUSH", {220, 180, 120, 255});
        }
    }
}

void PlayerMech::FirePrimary(ProjectileManager& projManager, ParticleSystem& particles) {
    if (m_primaryCooldownTimer > 0.0f || m_reactor.IsOverheated()) return;

    WeaponDefinition def = WeaponFactory::GetDefinition(m_primaryWeapon);
    float fireRateMult = m_reactor.GetWeaponFireRateMult();
    m_primaryCooldownTimer = def.fireRate / fireRateMult;

    m_reactor.AddHeat(def.heatCost);

    // Sound effect
    switch (m_primaryWeapon) {
        case WeaponType::PARTICLE_LANCE:
            AudioSynth::Play(SoundID::SHOOT_LANCE, 0.9f, Random::RangeF(0.95f, 1.05f));
            break;
        case WeaponType::FLAK_SHOTGUN:
            AudioSynth::Play(SoundID::SHOOT_FLAK, 1.0f, Random::RangeF(0.95f, 1.05f));
            break;
        case WeaponType::KINETIC_VULCAN:
        default:
            AudioSynth::Play(SoundID::SHOOT_VULCAN, 0.8f, Random::RangeF(0.95f, 1.05f));
            break;
    }

    // Spawn Projectiles
    Vec2 fireDir = Vec2::FromAngle(m_aimAngle);
    Vec2 muzzlePos = m_pos + fireDir * (m_collisionRadius + 8.0f) + fireDir.Rotated(1.57f) * 8.0f; // Left arm

    float damageMult = m_reactor.GetWeaponDamageMult();

    for (int i = 0; i < def.projectilesPerShot; ++i) {
        float angleOffset = 0.0f;
        if (def.projectilesPerShot > 1) {
            angleOffset = Random::RangeF(-def.spreadAngle * 0.5f, def.spreadAngle * 0.5f);
        } else if (def.spreadAngle > 0.0f) {
            angleOffset = Random::RangeF(-def.spreadAngle, def.spreadAngle);
        }

        Vec2 projDir = fireDir.Rotated(angleOffset);

        Projectile p;
        p.pos = muzzlePos;
        p.vel = projDir * def.projectileSpeed;
        p.damage = def.baseDamage * damageMult;
        p.radius = def.projectileRadius;
        p.color = def.projectileColor;
        p.pierceCount = def.pierceCount;
        p.isEnemy = false;
        projManager.SpawnProjectile(p);

        // Muzzle flash sparks
        particles.SpawnSpark(muzzlePos, projDir * 120.0f, def.projectileColor, 3.5f, 0.15f);
    }
}

void PlayerMech::FireOrdnance(ProjectileManager& projManager, ParticleSystem& particles) {
    if (m_ordnanceCooldownTimer > 0.0f || m_reactor.IsOverheated()) return;

    WeaponDefinition def = WeaponFactory::GetDefinition(m_ordnanceWeapon);
    m_ordnanceCooldownTimer = def.fireRate;

    m_reactor.AddHeat(def.heatCost);
    AudioSynth::Play(SoundID::SHOOT_MISSILE, 1.1f, Random::RangeF(0.95f, 1.05f));

    Vec2 fireDir = Vec2::FromAngle(m_aimAngle);
    Vec2 muzzlePos = m_pos + fireDir * (m_collisionRadius + 8.0f) - fireDir.Rotated(1.57f) * 8.0f; // Right arm
    float damageMult = m_reactor.GetWeaponDamageMult();

    for (int i = 0; i < def.projectilesPerShot; ++i) {
        float angleOffset = Random::RangeF(-def.spreadAngle * 0.5f, def.spreadAngle * 0.5f);
        Vec2 projDir = fireDir.Rotated(angleOffset);

        Projectile p;
        p.pos = muzzlePos;
        p.vel = projDir * def.projectileSpeed;
        p.damage = def.baseDamage * damageMult;
        p.radius = def.projectileRadius;
        p.color = def.projectileColor;
        p.isEnemy = false;
        p.isHoming = def.isHoming;
        p.targetPos = Input::Get().mouseWorldPos;
        p.isExplosive = def.isExplosive;
        p.explosionRadius = def.explosionRadius;
        projManager.SpawnProjectile(p);

        particles.SpawnPlasma(muzzlePos, projDir * 40.0f, {255, 100, 80, 255}, 6.0f, 0.2f);
    }
}

void PlayerMech::Update(float dt, ProjectileManager& projManager, ParticleSystem& particles) {
    if (!IsAlive()) return;

    const auto& input = Input::Get();

    // Reactor Profile Hotkeys
    if (input.profileAssault) { m_reactor.SetProfile(PowerProfile::ASSAULT); AudioSynth::Play(SoundID::POWER_SHIFT); }
    if (input.profileBulwark) { m_reactor.SetProfile(PowerProfile::BULWARK); AudioSynth::Play(SoundID::POWER_SHIFT); }
    if (input.profileInterceptor) { m_reactor.SetProfile(PowerProfile::INTERCEPTOR); AudioSynth::Play(SoundID::POWER_SHIFT); }
    if (input.profileBalanced) { m_reactor.SetProfile(PowerProfile::BALANCED); AudioSynth::Play(SoundID::POWER_SHIFT); }

    // Aim Angle
    m_aimAngle = input.aimDir.Angle();

    // Dash Execution
    if (m_dashCooldownTimer > 0.0f) m_dashCooldownTimer -= dt;
    if (m_dashTimer > 0.0f) {
        m_dashTimer -= dt;
        m_pos += m_dashDir * (m_baseSpeed * 2.8f) * dt;

        // Dash trail particles
        if (Random::Bool(0.6f)) {
            particles.SpawnSpark(m_pos, m_dashDir * -40.0f, {80, 220, 255, 200}, 4.0f, 0.2f);
        }
    } else {
        // Normal Thruster Movement Physics
        float speedMult = m_reactor.GetEngineSpeedMult();
        Vec2 targetVel = input.moveDir * (m_baseSpeed * speedMult);

        // Smooth acceleration and drag
        float accelRate = 12.0f;
        m_vel = m_vel + (targetVel - m_vel) * (1.0f - std::exp(-accelRate * dt));
        m_pos += m_vel * dt;

        // Exhaust Thruster particles
        if (input.moveDir.LengthSq() > 0.1f) {
            Vec2 exhaustDir = input.moveDir * -1.0f;
            Vec2 exhaustPos = m_pos + exhaustDir * (m_collisionRadius * 0.9f);
            particles.SpawnPlasma(exhaustPos, exhaustDir * 70.0f + Vec2(Random::RangeF(-15, 15), Random::RangeF(-15, 15)),
                                  {60, 180, 255, 180}, Random::RangeF(3.0f, 6.0f), 0.18f);
        }
    }

    if (input.dash && m_dashCooldownTimer <= 0.0f && !m_reactor.IsOverheated()) {
        ExecuteDash(particles);
    }

    // Heat Purge Execution
    if (input.heatPurge) {
        ExecuteHeatPurge(projManager, particles);
    }

    // Weapons Cooldown
    if (m_primaryCooldownTimer > 0.0f) m_primaryCooldownTimer -= dt;
    if (m_ordnanceCooldownTimer > 0.0f) m_ordnanceCooldownTimer -= dt;

    if (input.firePrimaryHeld) {
        FirePrimary(projManager, particles);
    }
    if (input.fireOrdnanceHeld) {
        FireOrdnance(projManager, particles);
    }

    // Reactor Update
    m_reactor.Update(dt);

    // Directional Shield Passive Regeneration
    if (m_shieldRegenTimer > 0.0f) {
        m_shieldRegenTimer -= dt;
    } else if (m_shield < m_maxShield) {
        float regenRate = 22.0f * m_reactor.GetShieldRegenMult();
        m_shield = std::min(m_maxShield, m_shield + regenRate * dt);
    }
}

void PlayerMech::Draw() const {
    if (!IsAlive()) return;

    // Draw Thruster / Chassis
    Vector2 center = {m_pos.x, m_pos.y};

    // Chassis body: Hexagonal / Polygonal armored silhouette
    Color chassisColor = {35, 45, 65, 255};
    Color armorHighlight = {70, 110, 180, 255};
    if (m_frame == MechFrame::TITAN) {
        chassisColor = {45, 40, 55, 255};
        armorHighlight = {160, 80, 220, 255};
    } else if (m_frame == MechFrame::PHANTOM) {
        chassisColor = {25, 35, 45, 255};
        armorHighlight = {40, 220, 180, 255};
    }

    DrawPoly(center, 6, m_collisionRadius, m_aimAngle * 57.29578f, chassisColor);
    DrawPolyLinesEx(center, 6, m_collisionRadius, m_aimAngle * 57.29578f, 2.0f, armorHighlight);

    // Turret / Cockpit Canopy
    Color canopyColor = {80, 220, 255, 255};
    if (m_reactor.IsOverheated()) {
        canopyColor = {255, 60, 60, 255}; // Red warning tint
    }
    DrawCircleV(center, m_collisionRadius * 0.45f, canopyColor);

    // Arm Hardpoints
    Vec2 aimDir = Vec2::FromAngle(m_aimAngle);
    Vec2 leftArm = m_pos + aimDir * (m_collisionRadius * 0.8f) + aimDir.Rotated(1.57f) * (m_collisionRadius * 0.65f);
    Vec2 rightArm = m_pos + aimDir * (m_collisionRadius * 0.8f) - aimDir.Rotated(1.57f) * (m_collisionRadius * 0.65f);

    // Left Arm Gun Barrel
    DrawLineEx({leftArm.x, leftArm.y}, {leftArm.x + aimDir.x * 14.0f, leftArm.y + aimDir.y * 14.0f}, 3.5f, {180, 190, 210, 255});
    // Right Arm Gun Barrel / Pod
    DrawLineEx({rightArm.x, rightArm.y}, {rightArm.x + aimDir.x * 12.0f, rightArm.y + aimDir.y * 12.0f}, 4.5f, {220, 90, 90, 255});

    // Directional Shield Arc
    if (m_shield > 0.0f) {
        float startDeg = (m_aimAngle - m_shieldArc * 0.5f) * 57.29578f;
        float endDeg = (m_aimAngle + m_shieldArc * 0.5f) * 57.29578f;

        float shieldAlpha = std::clamp(m_shield / m_maxShield, 0.2f, 1.0f);
        Color shieldCol = {60, 200, 255, static_cast<unsigned char>(160 * shieldAlpha)};

        BeginBlendMode(BLEND_ADDITIVE);
        DrawRing(center, m_collisionRadius + 6.0f, m_collisionRadius + 9.0f, startDeg, endDeg, 24, shieldCol);
        DrawRing(center, m_collisionRadius + 7.5f, m_collisionRadius + 8.5f, startDeg, endDeg, 24, {255, 255, 255, static_cast<unsigned char>(200 * shieldAlpha)});
        EndBlendMode();
    }
}

} // namespace Voidfall
