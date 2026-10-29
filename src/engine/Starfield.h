#pragma once

#include "core/Types.h"
#include <raylib.h>
#include <vector>

namespace Voidfall {

struct Star {
    Vec2 pos{0.0f, 0.0f};
    float layer{1.0f}; // 1.0 = distant, 2.0 = mid, 3.0 = foreground
    float size{1.5f};
    Color color{255, 255, 255, 255};
};

class Starfield {
public:
    Starfield(int starCount = 350);

    void Update(const Vec2& playerVel, float dt);
    void Draw(const Vec2& cameraPos) const;

private:
    std::vector<Star> m_stars;
    float m_fieldWidth{3000.0f};
    float m_fieldHeight{2200.0f};
};

} // namespace Voidfall
