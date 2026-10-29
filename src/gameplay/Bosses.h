#pragma once

#include "core/Types.h"
#include <raylib.h>
#include <string>
#include <vector>

namespace Voidfall {

class ParticleSystem;
class ProjectileManager;
class PlayerMech;

enum class BossID {
    GOLIATH_IV,
    AETHEL_CORE,
    PROTOCOL_ZERO_TITAN
};

struct BossSubsystem {
    std::string name;
    Vec2 relativeOffset;
    float radius{18.0f};
    float hp{200.0f};
    float maxHp{200.0f};
    bool isDestroyed{false};
};

class BossEntity {
public:
    BossEntity();

    void Init(BossID id, const Vec2& spawnPos);
    void Update(float dt, PlayerMech& player, ProjectileManager& projManager, ParticleSystem& particles);
    void Draw() const;

    [[nodiscard]] bool IsActive() const { return m_isActive; }
    [[nodiscard]] bool IsDefeated() const { return m_isDefeated; }
    [[nodiscard]] float GetHp() const { return m_hp; }
    [[nodiscard]] float GetMaxHp() const { return m_maxHp; }
    [[nodiscard]] float GetHpRatio() const { return m_hp / m_maxHp; }
    [[nodiscard]] const std::string& GetName() const { return m_name; }
    [[nodiscard]] int GetPhase() const { return m_phase; }
    [[nodiscard]] Vec2 GetPosition() const { return m_pos; }

private:
    void UpdateGoliath(float dt, PlayerMech& player, ProjectileManager& projManager, ParticleSystem& particles);
    void UpdateAethelCore(float dt, PlayerMech& player, ProjectileManager& projManager, ParticleSystem& particles);
    void UpdateProtocolZero(float dt, PlayerMech& player, ProjectileManager& projManager, ParticleSystem& particles);

    BossID m_id{BossID::GOLIATH_IV};
    std::string m_name{"GOLIATH-IV DREAD FORTRESS"};
    Vec2 m_pos{0.0f, 0.0f};
    Vec2 m_vel{0.0f, 0.0f};
    float m_rotation{0.0f};
    float m_radius{55.0f};

    float m_hp{1500.0f};
    float m_maxHp{1500.0f};
    float m_shield{400.0f};
    float m_maxShield{400.0f};

    int m_phase{1};
    float m_attackTimer{0.0f};
    float m_specialTimer{0.0f};
    float m_beamChargeTimer{0.0f};
    bool m_isFiringBeam{false};
    float m_beamAngle{0.0f};

    bool m_isActive{false};
    bool m_isDefeated{false};

    std::vector<BossSubsystem> m_subsystems;
};

} // namespace Voidfall
