#include "engine/Starfield.h"
#include "core/Random.h"
#include <cmath>

namespace Voidfall {

Starfield::Starfield(int starCount) {
    m_stars.reserve(starCount);

    for (int i = 0; i < starCount; ++i) {
        Star s;
        s.pos = Vec2(
            Random::RangeF(-m_fieldWidth * 0.5f, m_fieldWidth * 0.5f),
            Random::RangeF(-m_fieldHeight * 0.5f, m_fieldHeight * 0.5f)
        );

        float layerRoll = Random::Normal();
        if (layerRoll < 0.6f) {
            s.layer = 1.0f; // Distant
            s.size = Random::RangeF(1.0f, 1.8f);
            s.color = {160, 180, 220, static_cast<unsigned char>(Random::RangeI(100, 180))};
        } else if (layerRoll < 0.9f) {
            s.layer = 2.0f; // Mid
            s.size = Random::RangeF(2.0f, 3.0f);
            s.color = {120, 200, 255, static_cast<unsigned char>(Random::RangeI(180, 240))};
        } else {
            s.layer = 3.5f; // Foreground dust
            s.size = Random::RangeF(3.0f, 4.5f);
            s.color = {220, 240, 255, 255};
        }
        m_stars.push_back(s);
    }
}

void Starfield::Update(const Vec2& playerVel, float dt) {
    float halfW = m_fieldWidth * 0.5f;
    float halfH = m_fieldHeight * 0.5f;

    for (auto& s : m_stars) {
        // Subtle drift opposite to player motion for rich parallax depth
        s.pos -= playerVel * (s.layer * 0.08f) * dt;

        // Wrap around boundaries
        if (s.pos.x < -halfW) s.pos.x += m_fieldWidth;
        if (s.pos.x > halfW) s.pos.x -= m_fieldWidth;
        if (s.pos.y < -halfH) s.pos.y += m_fieldHeight;
        if (s.pos.y > halfH) s.pos.y -= m_fieldHeight;
    }
}

void Starfield::Draw(const Vec2& cameraPos) const {
    (void)cameraPos;
    BeginBlendMode(BLEND_ADDITIVE);

    for (const auto& s : m_stars) {
        DrawCircleV({s.pos.x, s.pos.y}, s.size * 0.5f, s.color);
    }

    EndBlendMode();
}

} // namespace Voidfall
