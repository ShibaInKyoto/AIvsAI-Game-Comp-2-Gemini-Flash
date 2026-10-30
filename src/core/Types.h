#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace Voidfall {

struct Vec2 {
    float x{0.0f};
    float y{0.0f};

    constexpr Vec2() = default;
    constexpr Vec2(float inX, float inY) : x(inX), y(inY) {}

    constexpr Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    constexpr Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    constexpr Vec2 operator*(float s) const { return {x * s, y * s}; }
    constexpr Vec2 operator/(float s) const { return s != 0.0f ? Vec2{x / s, y / s} : Vec2{0.0f, 0.0f}; }

    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
    Vec2& operator*=(float s) { x *= s; y *= s; return *this; }

    [[nodiscard]] float LengthSq() const { return x * x + y * y; }
    [[nodiscard]] float Length() const { return std::sqrt(LengthSq()); }

    [[nodiscard]] Vec2 Normalized() const {
        float len = Length();
        return len > 0.0001f ? Vec2{x / len, y / len} : Vec2{0.0f, 0.0f};
    }

    [[nodiscard]] float Dot(const Vec2& o) const { return x * o.x + y * o.y; }
    [[nodiscard]] float Cross(const Vec2& o) const { return x * o.y - y * o.x; }

    [[nodiscard]] float Distance(const Vec2& o) const { return (*this - o).Length(); }
    [[nodiscard]] float DistanceSq(const Vec2& o) const { return (*this - o).LengthSq(); }

    [[nodiscard]] float Angle() const { return std::atan2(y, x); }

    [[nodiscard]] Vec2 Rotated(float angleRad) const {
        float cosA = std::cos(angleRad);
        float sinA = std::sin(angleRad);
        return {x * cosA - y * sinA, x * sinA + y * cosA};
    }

    static Vec2 FromAngle(float angleRad, float length = 1.0f) {
        return {std::cos(angleRad) * length, std::sin(angleRad) * length};
    }

    static Vec2 Lerp(const Vec2& a, const Vec2& b, float t) {
        return a + (b - a) * std::clamp(t, 0.0f, 1.0f);
    }
};

struct Circle {
    Vec2 center;
    float radius{0.0f};

    [[nodiscard]] bool Intersects(const Circle& o) const {
        float rSum = radius + o.radius;
        return center.DistanceSq(o.center) <= (rSum * rSum);
    }

    [[nodiscard]] bool Contains(const Vec2& pt) const {
        return center.DistanceSq(pt) <= (radius * radius);
    }
};

struct RectAABB {
    float x{0.0f};
    float y{0.0f};
    float width{0.0f};
    float height{0.0f};

    [[nodiscard]] bool Intersects(const RectAABB& o) const {
        return x < o.x + o.width && x + width > o.x &&
               y < o.y + o.height && y + height > o.y;
    }

    [[nodiscard]] bool Contains(const Vec2& pt) const {
        return pt.x >= x && pt.x <= x + width &&
               pt.y >= y && pt.y <= y + height;
    }
};

enum class GameState {
    TITLE_SCREEN,
    HANGAR_LOADOUT,
    SECTOR_MAP,
    SECTOR_EXPEDITION,
    ENDLESS_SURVIVAL,
    BOSS_FIGHT,
    UPGRADE_DRAFT,
    ANOMALY_EVENT,
    FIRING_RANGE,
    PAUSED,
    GAME_OVER,
    VICTORY_SCREEN,
    CODEX,
    MISSION_DEBRIEFING,
    FLIGHT_REPLAY
};

enum class SectorType {
    ASTEROID_OUTPOST,
    DERELICT_SHIPYARD,
    ION_NEBULA,
    TITAN_CORE_FLAGSHIP
};

enum class EnvironmentalHazard {
    NONE,
    SOLAR_FLARE,
    ION_STORM,
    VOID_DECAY,
    METEOR_SHOWER
};

struct EndlessStats {
    int maxWave{0};
    float maxTimeSurvived{0.0f};
    int totalKills{0};
    int nanitesHarvested{0};
};

enum class PowerProfile {
    BALANCED,     // 33% WPN, 33% SHD, 34% ENG
    ASSAULT,      // 70% WPN, 15% SHD, 15% ENG
    BULWARK,      // 15% WPN, 70% SHD, 15% ENG
    INTERCEPTOR   // 15% WPN, 15% SHD, 70% ENG
};

enum class WeaponType {
    KINETIC_VULCAN,
    PARTICLE_LANCE,
    FLAK_SHOTGUN,
    ARC_TEMPEST,
    NEUTRON_DISRUPTOR,
    SWARM_MISSILE,
    SINGULARITY_VORTEX,
    CRYO_RAIL,
    NUCLEAR_CLUSTER_TORPEDO
};

enum class MechFrame {
    STRIKER,     // High mobility, faster heat venting
    TITAN,       // Heavy armor, colossal shield capacity
    PHANTOM,     // Stealth blink, critical strike bonus
    AEGIS        // Automated point defense, reflective shields
};

} // namespace Voidfall
