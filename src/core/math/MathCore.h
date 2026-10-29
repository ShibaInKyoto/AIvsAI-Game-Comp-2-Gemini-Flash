#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <algorithm>
#include <array>
#include "core/Types.h"

#ifdef PI
#undef PI
#endif

namespace Voidfall::Math {

using Vec2 = Voidfall::Vec2;

constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 6.28318530717958647692f;
constexpr float HALF_PI = 1.57079632679489661923f;
constexpr float DEG_TO_RAD = 3.14159265358979323846f / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / 3.14159265358979323846f;
constexpr float EPSILON = 1e-6f;

[[nodiscard]] constexpr float ToRadians(float degrees) noexcept { return degrees * DEG_TO_RAD; }
[[nodiscard]] constexpr float ToDegrees(float radians) noexcept { return radians * RAD_TO_DEG; }

[[nodiscard]] inline bool ApproxEqual(float a, float b, float epsilon = EPSILON) noexcept {
    return std::abs(a - b) <= epsilon;
}

[[nodiscard]] inline float Clamp(float value, float minVal, float maxVal) noexcept {
    return std::max(minVal, std::min(maxVal, value));
}

[[nodiscard]] inline float Lerp(float a, float b, float t) noexcept {
    return a + (b - a) * Clamp(t, 0.0f, 1.0f);
}

[[nodiscard]] inline float SmoothStep(float edge0, float edge1, float x) noexcept {
    float t = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

[[nodiscard]] inline float InverseLerp(float a, float b, float value) noexcept {
    return (ApproxEqual(a, b)) ? 0.0f : Clamp((value - a) / (b - a), 0.0f, 1.0f);
}

[[nodiscard]] inline float Remap(float iMin, float iMax, float oMin, float oMax, float v) noexcept {
    float t = InverseLerp(iMin, iMax, v);
    return Lerp(oMin, oMax, t);
}

// Vector3
struct Vec3 {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};

    constexpr Vec3() = default;
    constexpr Vec3(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ) {}

    constexpr Vec3 operator+(const Vec3& o) const noexcept { return {x + o.x, y + o.y, z + o.z}; }
    constexpr Vec3 operator-(const Vec3& o) const noexcept { return {x - o.x, y - o.y, z - o.z}; }
    constexpr Vec3 operator*(float s) const noexcept { return {x * s, y * s, z * s}; }
    constexpr Vec3 operator*(const Vec3& o) const noexcept { return {x * o.x, y * o.y, z * o.z}; }
    constexpr Vec3 operator/(float s) const noexcept {
        return s != 0.0f ? Vec3{x / s, y / s, z / s} : Vec3{};
    }

    Vec3& operator+=(const Vec3& o) noexcept { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(const Vec3& o) noexcept { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Vec3& operator*=(float s) noexcept { x *= s; y *= s; z *= s; return *this; }

    [[nodiscard]] float LengthSq() const noexcept { return x * x + y * y + z * z; }
    [[nodiscard]] float Length() const noexcept { return std::sqrt(LengthSq()); }

    [[nodiscard]] Vec3 Normalized() const noexcept {
        float len = Length();
        return len > EPSILON ? *this / len : Vec3{};
    }

    [[nodiscard]] float Dot(const Vec3& o) const noexcept { return x * o.x + y * o.y + z * o.z; }

    [[nodiscard]] Vec3 Cross(const Vec3& o) const noexcept {
        return {
            y * o.z - z * o.y,
            z * o.x - x * o.z,
            x * o.y - y * o.x
        };
    }

    [[nodiscard]] float Distance(const Vec3& o) const noexcept { return (*this - o).Length(); }
    [[nodiscard]] float DistanceSq(const Vec3& o) const noexcept { return (*this - o).LengthSq(); }
};

struct AABB3D {
    Vec3 min{-1.0f, -1.0f, -1.0f};
    Vec3 max{1.0f, 1.0f, 1.0f};

    constexpr AABB3D() = default;
    constexpr AABB3D(const Vec3& inMin, const Vec3& inMax) : min(inMin), max(inMax) {}
};

[[nodiscard]] inline Vec3 Lerp(const Vec3& a, const Vec3& b, float t) noexcept {
    return { a.x + (b.x - a.x) * Clamp(t, 0.0f, 1.0f),
             a.y + (b.y - a.y) * Clamp(t, 0.0f, 1.0f),
             a.z + (b.z - a.z) * Clamp(t, 0.0f, 1.0f) };
}

// Vector4
struct Vec4 {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    float w{0.0f};

    constexpr Vec4() = default;
    constexpr Vec4(float inX, float inY, float inZ, float inW) : x(inX), y(inY), z(inZ), w(inW) {}

    constexpr Vec4 operator+(const Vec4& o) const noexcept { return {x + o.x, y + o.y, z + o.z, w + o.w}; }
    constexpr Vec4 operator-(const Vec4& o) const noexcept { return {x - o.x, y - o.y, z - o.z, w - o.w}; }
    constexpr Vec4 operator*(float s) const noexcept { return {x * s, y * s, z * s, w * s}; }

    [[nodiscard]] float Dot(const Vec4& o) const noexcept { return x * o.x + y * o.y + z * o.z + w * o.w; }
};

// 3x3 Matrix for 2D Affine Transforms and Rotations
struct Mat3 {
    std::array<float, 9> m{
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    static Mat3 Identity() noexcept {
        return Mat3{};
    }

    static Mat3 Translation(float tx, float ty) noexcept {
        Mat3 r;
        r.m[6] = tx;
        r.m[7] = ty;
        return r;
    }

    static Mat3 Rotation(float radians) noexcept {
        Mat3 r;
        float c = std::cos(radians);
        float s = std::sin(radians);
        r.m[0] = c;  r.m[1] = s;
        r.m[3] = -s; r.m[4] = c;
        return r;
    }

    static Mat3 Scale(float sx, float sy) noexcept {
        Mat3 r;
        r.m[0] = sx;
        r.m[4] = sy;
        return r;
    }

    Mat3 operator*(const Mat3& o) const noexcept {
        Mat3 res;
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                float sum = 0.0f;
                for (int k = 0; k < 3; ++k) {
                    sum += m[row * 3 + k] * o.m[k * 3 + col];
                }
                res.m[row * 3 + col] = sum;
            }
        }
        return res;
    }

    [[nodiscard]] Vec3 TransformPoint(const Vec3& pt) const noexcept {
        return {
            m[0] * pt.x + m[3] * pt.y + m[6] * pt.z,
            m[1] * pt.x + m[4] * pt.y + m[7] * pt.z,
            m[2] * pt.x + m[5] * pt.y + m[8] * pt.z
        };
    }
};

// 4x4 Matrix for 3D Camera & Projection
struct Mat4 {
    std::array<float, 16> m{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    static Mat4 Identity() noexcept { return Mat4{}; }

    static Mat4 Orthographic(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept {
        Mat4 r;
        r.m[0] = 2.0f / (right - left);
        r.m[5] = 2.0f / (top - bottom);
        r.m[10] = -2.0f / (farZ - nearZ);
        r.m[12] = -(right + left) / (right - left);
        r.m[13] = -(top + bottom) / (top - bottom);
        r.m[14] = -(farZ + nearZ) / (farZ - nearZ);
        r.m[15] = 1.0f;
        return r;
    }

    static Mat4 Translation(float tx, float ty, float tz) noexcept {
        Mat4 r;
        r.m[3] = tx;
        r.m[7] = ty;
        r.m[11] = tz;
        return r;
    }

    static Mat4 Scale(float sx, float sy, float sz) noexcept {
        Mat4 r;
        r.m[0] = sx;
        r.m[5] = sy;
        r.m[10] = sz;
        return r;
    }

    static Mat4 RotationX(float rad) noexcept {
        Mat4 r;
        float c = std::cos(rad);
        float s = std::sin(rad);
        r.m[5] = c;  r.m[6] = -s;
        r.m[9] = s;  r.m[10] = c;
        return r;
    }

    static Mat4 RotationY(float rad) noexcept {
        Mat4 r;
        float c = std::cos(rad);
        float s = std::sin(rad);
        r.m[0] = c;   r.m[2] = s;
        r.m[8] = -s;  r.m[10] = c;
        return r;
    }

    static Mat4 RotationZ(float rad) noexcept {
        Mat4 r;
        float c = std::cos(rad);
        float s = std::sin(rad);
        r.m[0] = c;  r.m[1] = -s;
        r.m[4] = s;  r.m[5] = c;
        return r;
    }

    static Mat4 Perspective(float fovRad, float aspect, float nearZ, float farZ) noexcept {
        Mat4 r;
        float tanHalfFov = std::tan(fovRad * 0.5f);
        r.m[0] = 1.0f / (aspect * tanHalfFov);
        r.m[5] = 1.0f / tanHalfFov;
        r.m[10] = -(farZ + nearZ) / (farZ - nearZ);
        r.m[11] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
        r.m[14] = -1.0f;
        r.m[15] = 0.0f;
        return r;
    }

    static Mat4 LookAt(const Vec3& eye, const Vec3& target, const Vec3& up) noexcept {
        Vec3 forward = (eye - target).Normalized();
        Vec3 right = up.Cross(forward).Normalized();
        Vec3 trueUp = forward.Cross(right);

        Mat4 r;
        r.m[0] = right.x;    r.m[1] = right.y;    r.m[2] = right.z;    r.m[3] = -right.Dot(eye);
        r.m[4] = trueUp.x;   r.m[5] = trueUp.y;   r.m[6] = trueUp.z;   r.m[7] = -trueUp.Dot(eye);
        r.m[8] = forward.x;  r.m[9] = forward.y;  r.m[10] = forward.z; r.m[11] = -forward.Dot(eye);
        r.m[12] = 0.0f;      r.m[13] = 0.0f;      r.m[14] = 0.0f;      r.m[15] = 1.0f;
        return r;
    }

    [[nodiscard]] Vec4 TransformVec4(const Vec4& v) const noexcept {
        return {
            m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3] * v.w,
            m[4] * v.x + m[5] * v.y + m[6] * v.z + m[7] * v.w,
            m[8] * v.x + m[9] * v.y + m[10] * v.z + m[11] * v.w,
            m[12] * v.x + m[13] * v.y + m[14] * v.z + m[15] * v.w
        };
    }

    [[nodiscard]] Vec3 TransformPoint(const Vec3& p) const noexcept {
        Vec4 res = TransformVec4(Vec4(p.x, p.y, p.z, 1.0f));
        if (std::abs(res.w) > 0.00001f) {
            float invW = 1.0f / res.w;
            return { res.x * invW, res.y * invW, res.z * invW };
        }
        return { res.x, res.y, res.z };
    }

    [[nodiscard]] Vec3 TransformVector(const Vec3& v) const noexcept {
        return {
            m[0] * v.x + m[1] * v.y + m[2] * v.z,
            m[4] * v.x + m[5] * v.y + m[6] * v.z,
            m[8] * v.x + m[9] * v.y + m[10] * v.z
        };
    }

    Mat4 operator*(const Mat4& o) const noexcept {
        Mat4 res;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    sum += m[i * 4 + k] * o.m[k * 4 + j];
                }
                res.m[i * 4 + j] = sum;
            }
        }
        return res;
    }
};

// Quaternion
struct Quat {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    float w{1.0f};

    constexpr Quat() = default;
    constexpr Quat(float inX, float inY, float inZ, float inW) : x(inX), y(inY), z(inZ), w(inW) {}

    static Quat Identity() noexcept { return Quat(0.0f, 0.0f, 0.0f, 1.0f); }

    static Quat FromAxisAngle(const Vec3& axis, float angleRad) noexcept {
        float half = angleRad * 0.5f;
        float s = std::sin(half);
        Vec3 norm = axis.Normalized();
        return {norm.x * s, norm.y * s, norm.z * s, std::cos(half)};
    }

    Quat operator*(const Quat& o) const noexcept {
        return {
            w * o.x + x * o.w + y * o.z - z * o.y,
            w * o.y - x * o.z + y * o.w + z * o.x,
            w * o.z + x * o.y - y * o.x + z * o.w,
            w * o.w - x * o.x - y * o.y - z * o.z
        };
    }

    Mat4 ToMat4() const noexcept {
        Mat4 res;
        float xx = x * x, yy = y * y, zz = z * z;
        float xy = x * y, xz = x * z, yz = y * z;
        float wx = w * x, wy = w * y, wz = w * z;

        res.m[0] = 1.0f - 2.0f * (yy + zz);
        res.m[1] = 2.0f * (xy - wz);
        res.m[2] = 2.0f * (xz + wy);

        res.m[4] = 2.0f * (xy + wz);
        res.m[5] = 1.0f - 2.0f * (xx + zz);
        res.m[6] = 2.0f * (yz - wx);

        res.m[8] = 2.0f * (xz - wy);
        res.m[9] = 2.0f * (yz + wx);
        res.m[10] = 1.0f - 2.0f * (xx + yy);
        return res;
    }

    static Quat Slerp(const Quat& q1, const Quat& q2, float t) noexcept {
        float cosHalfTheta = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
        Quat q2Mod = q2;
        if (cosHalfTheta < 0.0f) {
            q2Mod = Quat(-q2.x, -q2.y, -q2.z, -q2.w);
            cosHalfTheta = -cosHalfTheta;
        }

        if (std::abs(cosHalfTheta) >= 1.0f) {
            return q1;
        }

        float halfTheta = std::acos(cosHalfTheta);
        float sinHalfTheta = std::sqrt(1.0f - cosHalfTheta * cosHalfTheta);

        if (std::abs(sinHalfTheta) < 0.001f) {
            return Quat(
                q1.x * 0.5f + q2Mod.x * 0.5f,
                q1.y * 0.5f + q2Mod.y * 0.5f,
                q1.z * 0.5f + q2Mod.z * 0.5f,
                q1.w * 0.5f + q2Mod.w * 0.5f
            );
        }

        float ratioA = std::sin((1.0f - t) * halfTheta) / sinHalfTheta;
        float ratioB = std::sin(t * halfTheta) / sinHalfTheta;

        return Quat(
            q1.x * ratioA + q2Mod.x * ratioB,
            q1.y * ratioA + q2Mod.y * ratioB,
            q1.z * ratioA + q2Mod.z * ratioB,
            q1.w * ratioA + q2Mod.w * ratioB
        );
    }
};

} // namespace Voidfall::Math
