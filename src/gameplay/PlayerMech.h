#pragma once

#include "core/Types.h"
#include "gameplay/Reactor.h"
#include "gameplay/Weapons.h"
#include <raylib.h>

namespace Voidfall {

class ParticleSystem;
class ProjectileManager;

class PlayerMech {
public:
    PlayerMech();

    void Reset(MechFrame frame = MechFrame::STRIKER);
    void Update(float dt, ProjectileManager& projManager, ParticleSystem& particles);
    void Draw() const;

    void TakeDamage(float amount, const Vec2& hitDir, ParticleSystem& particles);
    void Heal(float amount);

    [[nodiscard]] Vec2 GetPosition() const { return m_pos; }
    [[nodiscard]] Vec2 GetVelocity() const { return m_vel; }
    [[nodiscard]] float GetRadius() const { return m_collisionRadius; }
    [[nodiscard]] float GetHull() const { return m_hull; }
    [[nodiscard]] float GetMaxHull() const { return m_maxHull; }
    [[nodiscard]] float GetShield() const { return m_shield; }
    [[nodiscard]] float GetMaxShield() const { return m_maxShield; }
    [[nodiscard]] float GetShieldAngle() const { return m_aimAngle; }
    [[nodiscard]] bool IsAlive() const { return m_hull > 0.0f; }
    [[nodiscard]] bool IsDashing() const { return m_dashTimer > 0.0f; }

    [[nodiscard]] Reactor& GetReactor() { return m_reactor; }
    [[nodiscard]] const Reactor& GetReactor() const { return m_reactor; }

    [[nodiscard]] WeaponType GetPrimaryWeapon() const { return m_primaryWeapon; }
    [[nodiscard]] WeaponType GetOrdnanceWeapon() const { return m_ordnanceWeapon; }
    void SetPrimaryWeapon(WeaponType w) { m_primaryWeapon = w; }
    void SetOrdnanceWeapon(WeaponType w) { m_ordnanceWeapon = w; }

    [[nodiscard]] MechFrame GetFrame() const { return m_frame; }
    void SetFrame(MechFrame frame) { m_frame = frame; Reset(frame); }

    [[nodiscard]] int GetNaniteCores() const { return m_naniteCores; }
    void AddNaniteCores(int amount) { m_naniteCores += amount; }

private:
    void FirePrimary(ProjectileManager& projManager, ParticleSystem& particles);
    void FireOrdnance(ProjectileManager& projManager, ParticleSystem& particles);
    void ExecuteDash(ParticleSystem& particles);
    void ExecuteHeatPurge(ProjectileManager& projManager, ParticleSystem& particles);

    MechFrame m_frame{MechFrame::STRIKER};
    Vec2 m_pos{0.0f, 0.0f};
    Vec2 m_vel{0.0f, 0.0f};
    float m_facingAngle{0.0f};
    float m_aimAngle{0.0f};
    float m_collisionRadius{16.0f};

    // Vitals
    float m_hull{100.0f};
    float m_maxHull{100.0f};
    float m_shield{80.0f};
    float m_maxShield{80.0f};
    float m_shieldRegenDelay{2.5f};
    float m_shieldRegenTimer{0.0f};
    float m_shieldArc{1.9f}; // ~110 degrees

    // Movement & Dash
    float m_baseSpeed{280.0f};
    float m_dashCooldown{2.0f};
    float m_dashCooldownTimer{0.0f};
    float m_dashTimer{0.0f};
    float m_dashDuration{0.22f};
    Vec2 m_dashDir{0.0f, 0.0f};

    // Weapons & Reactor
    Reactor m_reactor{};
    WeaponType m_primaryWeapon{WeaponType::KINETIC_VULCAN};
    WeaponType m_ordnanceWeapon{WeaponType::SWARM_MISSILE};
    float m_primaryCooldownTimer{0.0f};
    float m_ordnanceCooldownTimer{0.0f};

    // Progression currency
    int m_naniteCores{0};
};

} // namespace Voidfall
