#pragma once

#include "core/Types.h"
#include <raylib.h>
#include <vector>
#include <string>

namespace Voidfall {

class PlayerMech;
class EnemyManager;
class BossEntity;
class ProjectileManager;
class ParticleSystem;

struct Meteor {
    Vec2 pos{0.0f, 0.0f};
    Vec2 vel{0.0f, 0.0f};
    float radius{18.0f};
    float damage{35.0f};
    Color color{255, 140, 40, 255};
};

class EndlessSurvival {
public:
    EndlessSurvival();

    void Init();
    void Update(float dt, PlayerMech& player, EnemyManager& enemies, BossEntity& boss, ProjectileManager& projManager, ParticleSystem& particles);
    void Draw() const;

    [[nodiscard]] int GetCurrentWave() const noexcept { return m_currentWave; }
    [[nodiscard]] float GetSurvivalTime() const noexcept { return m_survivalTimer; }
    [[nodiscard]] int GetTotalKills() const noexcept { return m_totalKills; }
    [[nodiscard]] int GetNanitesHarvested() const noexcept { return m_nanitesHarvested; }
    [[nodiscard]] EnvironmentalHazard GetCurrentHazard() const noexcept { return m_currentHazard; }
    [[nodiscard]] const std::string& GetHazardName() const noexcept { return m_hazardName; }
    [[nodiscard]] float GetHazardTimeRemaining() const noexcept { return m_hazardDuration - m_hazardTimer; }

    [[nodiscard]] bool IsExtractionAvailable() const noexcept { return m_extractionAvailable; }
    [[nodiscard]] bool IsExtracted() const noexcept { return m_extracted; }
    void TriggerExtraction(PlayerMech& player);
    void ContinueNextTier();

private:
    void ShiftHazard(ParticleSystem& particles);
    void UpdateHazards(float dt, PlayerMech& player, ParticleSystem& particles);
    void SpawnMeteors(int count);

    int m_currentWave{1};
    float m_survivalTimer{0.0f};
    int m_totalKills{0};
    int m_nanitesHarvested{0};

    float m_waveTimer{0.0f};
    float m_waveInterval{25.0f};
    bool m_bossActive{false};

    EnvironmentalHazard m_currentHazard{EnvironmentalHazard::NONE};
    std::string m_hazardName{"SECTOR STABLE"};
    float m_hazardTimer{0.0f};
    float m_hazardDuration{40.0f};

    // Void Decay arena shrinkage
    float m_baseArenaRadius{1400.0f};
    float m_currentArenaRadius{1400.0f};

    // Meteor storm
    std::vector<Meteor> m_meteors;
    float m_meteorSpawnTimer{0.0f};

    // Extraction terminal
    bool m_extractionAvailable{false};
    Vec2 m_extractionPos{0.0f, 0.0f};
    float m_extractionCharge{0.0f};
    bool m_extracted{false};
};

} // namespace Voidfall
