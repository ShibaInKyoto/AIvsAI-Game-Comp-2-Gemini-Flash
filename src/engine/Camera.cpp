#include "engine/Camera.h"
#include "core/Random.h"
#include "core/Config.h"
#include <algorithm>
#include <cmath>

namespace Voidfall {

GameCamera::GameCamera() {
    m_camera.target = {0.0f, 0.0f};
    m_camera.offset = {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
    m_camera.rotation = 0.0f;
    m_camera.zoom = 1.0f;
}

void GameCamera::AddTrauma(float amount) {
    if (!ConfigManager::Get().screenShakeEnabled) return;
    m_trauma = std::clamp(m_trauma + amount, 0.0f, 1.0f);
}

void GameCamera::Update(const Vec2& targetPos, const Vec2& lookAheadTarget, float dt) {
    // Keep camera offset centered on screen dimensions
    m_camera.offset = {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};

    // Look-ahead framing: bias camera 20% toward cursor
    Vec2 desiredPos = targetPos + (lookAheadTarget - targetPos) * 0.22f;

    // Smooth exponential damping
    float smoothSpeed = 8.0f;
    float alpha = 1.0f - std::exp(-smoothSpeed * dt);
    m_pos = m_pos + (desiredPos - m_pos) * alpha;

    // Smooth zoom interpolation
    float zoomSpeed = 4.0f;
    m_camera.zoom += (m_targetZoom - m_camera.zoom) * (1.0f - std::exp(-zoomSpeed * dt));

    // Non-linear trauma shake
    if (m_trauma > 0.001f) {
        float shakeFactor = m_trauma * m_trauma; // quadratic falloff
        float maxOffset = 22.0f;
        float maxRot = 3.5f;

        m_shakeOffset = Vec2(
            Random::RangeF(-1.0f, 1.0f) * maxOffset * shakeFactor,
            Random::RangeF(-1.0f, 1.0f) * maxOffset * shakeFactor
        );
        m_camera.rotation = Random::RangeF(-1.0f, 1.0f) * maxRot * shakeFactor;

        // Decay trauma over time
        m_trauma = std::max(0.0f, m_trauma - 1.8f * dt);
    } else {
        m_shakeOffset = {0.0f, 0.0f};
        m_camera.rotation = 0.0f;
        m_trauma = 0.0f;
    }

    m_camera.target = {m_pos.x + m_shakeOffset.x, m_pos.y + m_shakeOffset.y};
}

void GameCamera::BeginMode() {
    BeginMode2D(m_camera);
}

void GameCamera::EndMode() {
    EndMode2D();
}

} // namespace Voidfall
