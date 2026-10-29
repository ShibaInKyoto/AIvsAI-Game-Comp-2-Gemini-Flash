#include "core/math/Geometry.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace Voidfall::Math {

Vec2 ConvexPolygon::GetCentroid() const noexcept {
    if (vertices.empty()) return {0.0f, 0.0f};
    Vec2 sum{0.0f, 0.0f};
    for (const auto& v : vertices) {
        sum += v;
    }
    return sum / static_cast<float>(vertices.size());
}

void ConvexPolygon::Translate(const Vec2& offset) {
    for (auto& v : vertices) {
        v += offset;
    }
}

void ConvexPolygon::Rotate(float radians, const Vec2& pivot) {
    float c = std::cos(radians);
    float s = std::sin(radians);
    for (auto& v : vertices) {
        Vec2 rel = v - pivot;
        v.x = pivot.x + (rel.x * c - rel.y * s);
        v.y = pivot.y + (rel.x * s + rel.y * c);
    }
}

std::vector<Vec2> ConvexPolygon::GetNormals() const {
    std::vector<Vec2> normals;
    size_t count = vertices.size();
    normals.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        Vec2 p1 = vertices[i];
        Vec2 p2 = vertices[(i + 1) % count];
        Vec2 edge = p2 - p1;
        Vec2 normal = Vec2{-edge.y, edge.x}.Normalized();
        normals.push_back(normal);
    }
    return normals;
}

ConvexPolygon Geometry::ComputeConvexHull(std::vector<Vec2> points) {
    if (points.size() <= 3) {
        return ConvexPolygon{points};
    }

    // Sort points lexicographically by x then y
    std::sort(points.begin(), points.end(), [](const Vec2& a, const Vec2& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    });

    auto Cross = [](const Vec2& o, const Vec2& a, const Vec2& b) {
        return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
    };

    std::vector<Vec2> hull;
    hull.reserve(points.size() * 2);

    // Build lower hull
    for (const auto& pt : points) {
        while (hull.size() >= 2 && Cross(hull[hull.size() - 2], hull.back(), pt) <= 0.0f) {
            hull.pop_back();
        }
        hull.push_back(pt);
    }

    // Build upper hull
    size_t lowerCount = hull.size() + 1;
    for (auto it = points.rbegin(); it != points.rend(); ++it) {
        while (hull.size() >= lowerCount && Cross(hull[hull.size() - 2], hull.back(), *it) <= 0.0f) {
            hull.pop_back();
        }
        hull.push_back(*it);
    }

    hull.pop_back(); // Remove duplicate point
    return ConvexPolygon{hull};
}

bool Geometry::IntersectsSAT(const ConvexPolygon& polyA, const ConvexPolygon& polyB, Vec2* outMtv) {
    if (polyA.vertices.empty() || polyB.vertices.empty()) return false;

    auto Project = [](const ConvexPolygon& poly, const Vec2& axis, float& minP, float& maxP) {
        minP = axis.Dot(poly.vertices[0]);
        maxP = minP;
        for (size_t i = 1; i < poly.vertices.size(); ++i) {
            float p = axis.Dot(poly.vertices[i]);
            if (p < minP) minP = p;
            if (p > maxP) maxP = p;
        }
    };

    float minOverlap = std::numeric_limits<float>::max();
    Vec2 smallestAxis{0.0f, 0.0f};

    auto TestAxes = [&](const std::vector<Vec2>& normals) {
        for (const auto& axis : normals) {
            float minA, maxA, minB, maxB;
            Project(polyA, axis, minA, maxA);
            Project(polyB, axis, minB, maxB);

            if (maxA < minB || maxB < minA) {
                return false; // Separating axis found!
            }

            float overlap = std::min(maxA, maxB) - std::max(minA, minB);
            if (overlap < minOverlap) {
                minOverlap = overlap;
                smallestAxis = axis;
            }
        }
        return true;
    };

    if (!TestAxes(polyA.GetNormals())) return false;
    if (!TestAxes(polyB.GetNormals())) return false;

    if (outMtv) {
        // Ensure MTV points from A to B
        Vec2 dir = polyB.GetCentroid() - polyA.GetCentroid();
        if (smallestAxis.Dot(dir) < 0.0f) {
            smallestAxis = smallestAxis * -1.0f;
        }
        *outMtv = smallestAxis * minOverlap;
    }

    return true;
}

bool Geometry::RaycastCircle(const Ray2D& ray, const Circle& circle, RaycastHit2D& outHit) {
    Vec2 m = ray.origin - circle.center;
    float b = m.Dot(ray.direction);
    float c = m.Dot(m) - circle.radius * circle.radius;

    if (c > 0.0f && b > 0.0f) return false;

    float discr = b * b - c;
    if (discr < 0.0f) return false;

    float t = -b - std::sqrt(discr);
    if (t < 0.0f) t = 0.0f;

    outHit.hit = true;
    outHit.distance = t;
    outHit.point = ray.GetPoint(t);
    outHit.normal = (outHit.point - circle.center).Normalized();
    return true;
}

bool Geometry::RaycastSegment(const Ray2D& ray, const Vec2& segA, const Vec2& segB, RaycastHit2D& outHit) {
    Vec2 v1 = ray.origin - segA;
    Vec2 v2 = segB - segA;
    Vec2 v3 = Vec2{-ray.direction.y, ray.direction.x};

    float dot = v2.Dot(v3);
    if (std::abs(dot) < 0.00001f) return false;

    float t1 = (v2.x * v1.y - v2.y * v1.x) / dot;
    float t2 = v1.Dot(v3) / dot;

    if (t1 >= 0.0f && (t2 >= 0.0f && t2 <= 1.0f)) {
        outHit.hit = true;
        outHit.distance = t1;
        outHit.point = ray.GetPoint(t1);
        Vec2 edge = segB - segA;
        outHit.normal = Vec2{-edge.y, edge.x}.Normalized();
        return true;
    }
    return false;
}

bool Geometry::ContainsPoint(const ConvexPolygon& poly, const Vec2& pt) {
    if (poly.vertices.size() < 3) return false;
    for (size_t i = 0; i < poly.vertices.size(); ++i) {
        Vec2 p1 = poly.vertices[i];
        Vec2 p2 = poly.vertices[(i + 1) % poly.vertices.size()];
        Vec2 edge = p2 - p1;
        Vec2 toPt = pt - p1;
        if (edge.Cross(toPt) < 0.0f) return false;
    }
    return true;
}

} // namespace Voidfall::Math
