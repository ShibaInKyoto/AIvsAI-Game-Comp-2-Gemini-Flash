#pragma once

#include "core/Types.h"
#include <raylib.h>
#include <vector>

namespace Voidfall {

class PlayerMech;
class ProjectileManager;
class ParticleSystem;

struct TargetDummy {
    Vec2 pos{0.0f, 0.0f};
    float radius{25.0f};
    float hp{1000.0f};
    float maxHp{1000.0f};
    float shield{500.0f};
    float maxShield{500.0f};
    float damageTakenInWindow{0.0f};
    float windowTimer{0.0f};
    float dps{0.0f};
    std::string label{"Standard Target"};
};

class FiringRange {
public:
    FiringRange();

    void Init();
    void Update(float dt, PlayerMech& player, ProjectileManager& projManager, ParticleSystem& particles);
    void Draw() const;

private:
    std::vector<TargetDummy> m_dummies;
    float m_totalSessionDamage{0.0f};
    float m_peakDps{0.0f};
};

} // namespace Voidfall
