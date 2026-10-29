#include "core/math/Splines.h"
#include <algorithm>
#include <cmath>

namespace Voidfall::Math {

Vec2 Spline2D::EvaluateBezierQuadratic(const Vec2& p0, const Vec2& p1, const Vec2& p2, float t) noexcept {
    t = std::clamp(t, 0.0f, 1.0f);
    float u = 1.0f - t;
    return p0 * (u * u) + p1 * (2.0f * u * t) + p2 * (t * t);
}

Vec2 Spline2D::EvaluateBezierCubic(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, float t) noexcept {
    t = std::clamp(t, 0.0f, 1.0f);
    float u = 1.0f - t;
    float uu = u * u;
    float tt = t * t;
    return p0 * (uu * u) + p1 * (3.0f * uu * t) + p2 * (3.0f * u * tt) + p3 * (tt * t);
}

Vec2 Spline2D::EvaluateCatmullRom(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, float t) noexcept {
    t = std::clamp(t, 0.0f, 1.0f);
    float t2 = t * t;
    float t3 = t2 * t;

    return (p0 * (-t3 + 2.0f * t2 - t) +
            p1 * (3.0f * t3 - 5.0f * t2 + 2.0f) +
            p2 * (-3.0f * t3 + 4.0f * t2 + t) +
            p3 * (t3 - t2)) * 0.5f;
}

Vec2 Spline2D::EvaluateHermite(const Vec2& p0, const Vec2& t0, const Vec2& p1, const Vec2& t1, float t) noexcept {
    t = std::clamp(t, 0.0f, 1.0f);
    float t2 = t * t;
    float t3 = t2 * t;

    float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
    float h10 = t3 - 2.0f * t2 + t;
    float h01 = -2.0f * t3 + 3.0f * t2;
    float h11 = t3 - t2;

    return p0 * h00 + t0 * h10 + p1 * h01 + t1 * h11;
}

void Spline2D::AddPoint(const Vec2& pt) {
    m_points.push_back(pt);
}

void Spline2D::Clear() {
    m_points.clear();
}

Vec2 Spline2D::SamplePath(float globalT) const {
    if (m_points.empty()) return {0.0f, 0.0f};
    if (m_points.size() == 1) return m_points[0];
    if (m_points.size() == 2) return Vec2::Lerp(m_points[0], m_points[1], globalT);

    globalT = std::clamp(globalT, 0.0f, 1.0f);
    size_t numSegments = m_points.size() - 1;
    float scaled = globalT * static_cast<float>(numSegments);
    size_t idx = static_cast<size_t>(std::floor(scaled));
    if (idx >= numSegments) idx = numSegments - 1;
    float localT = scaled - static_cast<float>(idx);

    size_t i0 = (idx > 0) ? idx - 1 : 0;
    size_t i1 = idx;
    size_t i2 = idx + 1;
    size_t i3 = (idx + 2 < m_points.size()) ? idx + 2 : idx + 1;

    return EvaluateCatmullRom(m_points[i0], m_points[i1], m_points[i2], m_points[i3], localT);
}

float Spline2D::GetTotalLength() const {
    if (m_points.size() < 2) return 0.0f;
    float len = 0.0f;
    for (size_t i = 1; i < m_points.size(); ++i) {
        len += m_points[i - 1].Distance(m_points[i]);
    }
    return len;
}

} // namespace Voidfall::Math
