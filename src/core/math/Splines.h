#pragma once

#include "core/Types.h"
#include <vector>

namespace Voidfall::Math {

class Spline2D {
public:
    static Vec2 EvaluateBezierCubic(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, float t) noexcept;
    static Vec2 EvaluateBezierQuadratic(const Vec2& p0, const Vec2& p1, const Vec2& p2, float t) noexcept;
    static Vec2 EvaluateCatmullRom(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, float t) noexcept;
    static Vec2 EvaluateHermite(const Vec2& p0, const Vec2& t0, const Vec2& p1, const Vec2& t1, float t) noexcept;

    // Multi-segment path
    void AddPoint(const Vec2& pt);
    void Clear();
    [[nodiscard]] Vec2 SamplePath(float globalT) const;
    [[nodiscard]] float GetTotalLength() const;
    [[nodiscard]] size_t GetPointCount() const { return m_points.size(); }

private:
    std::vector<Vec2> m_points;
};

} // namespace Voidfall::Math
