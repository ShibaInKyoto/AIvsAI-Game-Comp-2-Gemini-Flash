#pragma once

#include <vector>
#include <raylib.h>
#include "core/Types.h"
#include "core/math/Vector.h"

namespace Voidfall {

enum class HazardType {
    ASTEROID_FIELD,
    SOLAR_FLARE_STORM,
    ION_NEBULA_CLOUD,
    GRAVITATIONAL_SINGULARITY,
    CORROSIVE_NANITE_FOG
};

struct AsteroidEntity {
    int id = 0;
    Vec2 position;
    Vec2 velocity;
    float rotation = 0.0f;
    float rotSpeed = 0.0f;
    float radius = 30.0f;
    float health = 80.0f;
    float maxHealth = 80.0f;
    int tier = 2; // 3 = Large, 2 = Medium, 1 = Shard
    int mineralValue = 15;
    std::vector<Vec2> polygonVertices;
};

struct SingularityEntity {
    Vec2 position;
    float mass = 120000.0f;
    float eventHorizonRadius = 45.0f;
    float accretionDiskRadius = 240.0f;
    float rotationAngle = 0.0f;
};

struct HazardZone {
    int id = 0;
    HazardType type = HazardType::ION_NEBULA_CLOUD;
    Vec2 position;
    float radius = 220.0f;
    float intensity = 1.0f;
    float pulseTimer = 0.0f;
    Color tintColor;
};

class HazardEngine {
public:
    HazardEngine();
    ~HazardEngine() = default;

    void Initialize(HazardType primaryHazard, int screenWidth, int screenHeight);
    void Update(float dt, Vec2& playerPos, Vec2& playerVel, float& playerHeat, float& playerShields, float& playerHp);
    void Render();

    // Asteroid interactions
    bool DamageAsteroid(int id, float damage, std::vector<AsteroidEntity>& outSplinters);
    const std::vector<AsteroidEntity>& GetAsteroids() const { return m_asteroids; }
    const std::vector<SingularityEntity>& GetSingularities() const { return m_singularities; }
    const std::vector<HazardZone>& GetZones() const { return m_zones; }

    bool IsInIonNebula(const Vec2& pos) const;
    bool IsSolarStormActive() const { return m_solarStormActive; }
    float GetSolarStormIntensity() const { return m_solarStormIntensity; }

private:
    void SpawnAsteroids(int count);
    void GenerateAsteroidPolygon(AsteroidEntity& ast);

    HazardType m_currentHazardType = HazardType::ASTEROID_FIELD;
    std::vector<AsteroidEntity> m_asteroids;
    std::vector<SingularityEntity> m_singularities;
    std::vector<HazardZone> m_zones;

    bool m_solarStormActive = false;
    float m_solarStormTimer = 0.0f;
    float m_solarStormCycle = 18.0f;
    float m_solarStormIntensity = 0.0f;

    int m_nextAsteroidId = 1;
};

} // namespace Voidfall
