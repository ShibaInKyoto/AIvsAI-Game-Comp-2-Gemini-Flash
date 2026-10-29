#pragma once

#include "core/Types.h"
#include <raylib.h>

namespace Voidfall {

class GameCamera {
public:
    GameCamera();

    void Update(const Vec2& targetPos, const Vec2& lookAheadTarget, float dt);
    void AddTrauma(float amount); // [0.0f, 1.0f]

    void BeginMode();
    void EndMode();

    [[nodiscard]] Vec2 GetPosition() const { return m_pos; }
    [[nodiscard]] float GetZoom() const { return m_camera.zoom; }
    void SetZoom(float targetZoom) { m_targetZoom = targetZoom; }

    [[nodiscard]] Camera2D& GetRaylibCamera() { return m_camera; }

private:
    Camera2D m_camera{};
    Vec2 m_pos{0.0f, 0.0f};
    Vec2 m_shakeOffset{0.0f, 0.0f};
    float m_trauma{0.0f};
    float m_targetZoom{1.0f};
};

} // namespace Voidfall
