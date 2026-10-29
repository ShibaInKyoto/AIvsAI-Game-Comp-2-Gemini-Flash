#pragma once

#include "core/Types.h"
#include "gameplay/EnemyAffixes.h"
#include <raylib.h>
#include <vector>

namespace Voidfall {

class ParticleSystem;
class ProjectileManager;
class PlayerMech;

enum class EnemyType {
    DRONE_SWARMER,
    SIEGE_CRAWLER,
    PHASE_HUNTER,
    MINE_LAYER,
    ELITE_COMMANDER
};

struct EnemyUnit {
    EnemyType type{EnemyType::DRONE_SWARMER};
    EnemyAffix affix{EnemyAffix::NONE};
    Vec2 pos{0.0f, 0.0f};
    Vec2 vel{0.0f, 0.0f};
    float facingAngle{0.0f};
    float radius{14.0f};

    float hp{40.0f};
    float maxHp{40.0f};
    float shield{0.0f};
    float maxShield{0.0f};

    float speed{180.0f};
    float shootCooldown{1.2f};
    float shootTimer{0.0f};
    float aiTimer{0.0f};

    bool isCloaked{false};
    float cloakAlpha{1.0f};
    bool hasTeleported{false};

    int naniteReward{10};
    bool isAlive{true};
};

class EnemyManager {
public:
    EnemyManager();

    void Update(float dt, PlayerMech& player, ProjectileManager& projManager, ParticleSystem& particles);
    void Draw() const;
    void Clear();

    void SpawnEnemy(EnemyType type, const Vec2& pos);
    void SpawnWave(int waveNumber, const Vec2& playerPos, float arenaRadius);

    [[nodiscard]] std::vector<EnemyUnit>& GetEnemies() { return m_enemies; }
    [[nodiscard]] const std::vector<EnemyUnit>& GetEnemies() const { return m_enemies; }
    [[nodiscard]] size_t GetActiveCount() const;

private:
    std::vector<EnemyUnit> m_enemies;
};

} // namespace Voidfall
