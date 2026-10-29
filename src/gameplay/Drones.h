#pragma once

#include "core/Types.h"
#include <raylib.h>
#include <vector>

namespace Voidfall {

class ProjectileManager;
class ParticleSystem;
class EnemyManager;

enum class DroneType {
    ATTACK_DRONE,
    SHIELD_DRONE,
    EMP_SENTRY
};

struct DroneUnit {
    DroneType type{DroneType::ATTACK_DRONE};
    Vec2 pos{0.0f, 0.0f};
    float orbitAngle{0.0f};
    float orbitRadius{55.0f};
    float orbitSpeed{2.0f};
    float fireTimer{0.0f};
    float fireCooldown{0.35f};
};

class DroneManager {
public:
    DroneManager();

    void Update(float dt, const Vec2& playerPos, EnemyManager& enemies, ProjectileManager& projManager, ParticleSystem& particles);
    void Draw() const;
    void Clear();

    void AddDrone(DroneType type);
    [[nodiscard]] size_t GetDroneCount() const { return m_drones.size(); }

private:
    std::vector<DroneUnit> m_drones;
};

} // namespace Voidfall
