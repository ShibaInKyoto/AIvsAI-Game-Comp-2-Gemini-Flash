#pragma once

#include "core/Types.h"
#include <raylib.h>
#include <vector>

namespace Voidfall {

class ParticleSystem;
class ProjectileManager;
class PlayerMech;
class EnemyManager;
class BossEntity;

struct Asteroid {
    Vec2 pos{0.0f, 0.0f};
    float radius{24.0f};
    float hp{80.0f};
    float rotation{0.0f};
    float rotSpeed{0.1f};
    Color color{80, 85, 95, 255};
};

struct ExtractionBeacon {
    Vec2 pos{0.0f, 0.0f};
    float radius{45.0f};
    bool isActive{false};
    float charge{0.0f};
    float maxCharge{5.0f}; // seconds standing inside beacon
    bool isCompleted{false};
};

class SectorManager {
public:
    SectorManager();

    void InitSector(SectorType type, int sectorIndex);
    void Update(float dt, PlayerMech& player, EnemyManager& enemies, BossEntity& boss, ProjectileManager& projManager, ParticleSystem& particles);
    void Draw() const;

    [[nodiscard]] SectorType GetCurrentType() const { return m_currentType; }
    [[nodiscard]] int GetSectorIndex() const { return m_sectorIndex; }
    [[nodiscard]] int GetWave() const { return m_currentWave; }
    [[nodiscard]] int GetMaxWaves() const { return m_maxWaves; }
    [[nodiscard]] float GetArenaRadius() const { return m_arenaRadius; }
    [[nodiscard]] bool IsSectorComplete() const { return m_sectorComplete; }
    [[nodiscard]] bool IsBossWave() const { return m_currentWave > m_maxWaves; }

    void ProceedToNextSector();

private:
    void GenerateAsteroids();

    SectorType m_currentType{SectorType::ASTEROID_OUTPOST};
    int m_sectorIndex{1};
    int m_currentWave{1};
    int m_maxWaves{3};
    float m_arenaRadius{1200.0f};

    bool m_sectorComplete{false};
    bool m_waveActive{false};
    float m_waveCooldownTimer{2.0f};

    ExtractionBeacon m_beacon{};
    std::vector<Asteroid> m_asteroids;
};

} // namespace Voidfall
