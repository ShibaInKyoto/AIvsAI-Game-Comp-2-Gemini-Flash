#pragma once

#include "core/Types.h"
#include <raylib.h>
#include <vector>
#include <string>

namespace Voidfall {

enum class ParticleType {
    SPARK,
    PLASMA,
    SMOKE,
    RING_SHOCKWAVE,
    DEBRIS,
    FLOATING_TEXT
};

struct Particle {
    Vec2 pos{0.0f, 0.0f};
    Vec2 vel{0.0f, 0.0f};
    Color startColor{255, 255, 255, 255};
    Color endColor{255, 255, 255, 0};
    float sizeStart{4.0f};
    float sizeEnd{0.0f};
    float life{1.0f};
    float maxLife{1.0f};
    float rotation{0.0f};
    float rotSpeed{0.0f};
    ParticleType type{ParticleType::SPARK};
    std::string text{""};
};

class ParticleSystem {
public:
    ParticleSystem(size_t maxCapacity = 2000);

    void Update(float dt);
    void Draw() const;
    void Clear();

    void SpawnSpark(const Vec2& pos, const Vec2& vel, Color color, float size = 3.0f, float life = 0.4f);
    void SpawnPlasma(const Vec2& pos, const Vec2& vel, Color color, float size = 6.0f, float life = 0.6f);
    void SpawnSmoke(const Vec2& pos, const Vec2& vel, float size = 10.0f, float life = 0.8f);
    void SpawnShockwave(const Vec2& pos, float maxRadius, Color color, float life = 0.35f);
    void SpawnExplosion(const Vec2& pos, int count, Color primaryColor, float blastRadius = 40.0f);
    void SpawnDamageNumber(const Vec2& pos, int damage, bool isCrit = false);
    void SpawnFloatingText(const Vec2& pos, const std::string& text, Color color);

private:
    std::vector<Particle> m_particles;
};

} // namespace Voidfall
