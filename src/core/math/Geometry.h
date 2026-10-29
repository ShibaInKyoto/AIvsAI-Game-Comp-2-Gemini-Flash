#pragma once

#include "core/Types.h"
#include <vector>

namespace Voidfall::Math {

struct Ray2D {
    Vec2 origin{0.0f, 0.0f};
    Vec2 direction{1.0f, 0.0f}; // Should be normalized

    [[nodiscard]] Vec2 GetPoint(float distance) const noexcept {
        return origin + direction * distance;
    }
};

struct RaycastHit2D {
    bool hit{false};
    float distance{0.0f};
    Vec2 point{0.0f, 0.0f};
    Vec2 normal{0.0f, 0.0f};
};

struct ConvexPolygon {
    std::vector<Vec2> vertices;

    [[nodiscard]] Vec2 GetCentroid() const noexcept;
    void Translate(const Vec2& offset);
    void Rotate(float radians, const Vec2& pivot);
    [[nodiscard]] std::vector<Vec2> GetNormals() const;
};

class Geometry {
public:
    // Convex Hull generation from point cloud (Monotone Chain / Graham Scan)
    static ConvexPolygon ComputeConvexHull(std::vector<Vec2> points);

    // Separating Axis Theorem (SAT) Intersection for arbitrary convex polygons
    static bool IntersectsSAT(const ConvexPolygon& polyA, const ConvexPolygon& polyB, Vec2* outMtv = nullptr);

    // Raycast against Circle
    static bool RaycastCircle(const Ray2D& ray, const Circle& circle, RaycastHit2D& outHit);

    // Raycast against Line Segment
    static bool RaycastSegment(const Ray2D& ray, const Vec2& segA, const Vec2& segB, RaycastHit2D& outHit);

    // Point in Polygon test (Winding number / Ray casting)
    static bool ContainsPoint(const ConvexPolygon& poly, const Vec2& pt);
};

} // namespace Voidfall::Math
