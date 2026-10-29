#pragma once

#include "core/Types.h"
#include <cmath>

namespace Voidfall::Physics {

enum class BodyType {
    DYNAMIC,
    STATIC,
    KINEMATIC
};

struct CollisionManifold {
    bool hasCollision{false};
    Vec2 normal{0.0f, 0.0f};     // Contact normal pointing from A to B
    float penetration{0.0f};     // Overlap distance
    Vec2 contactPoint{0.0f, 0.0f};
};

class RigidBody2D {
public:
    RigidBody2D() = default;
    RigidBody2D(BodyType type, float mass, float radius, float restitution = 0.5f, float friction = 0.2f);

    void Integrate(float dt);
    void ApplyForce(const Vec2& force);
    void ApplyImpulse(const Vec2& impulse);
    void ApplyTorque(float torque);
    void ApplyAngularImpulse(float angularImpulse);

    static CollisionManifold CheckCircleCircle(const RigidBody2D& a, const RigidBody2D& b);
    static void ResolveCollision(RigidBody2D& a, RigidBody2D& b, const CollisionManifold& manifold);

    // Getters and Setters
    [[nodiscard]] BodyType GetType() const noexcept { return m_type; }
    void SetType(BodyType type) noexcept { m_type = type; }

    [[nodiscard]] Vec2 GetPosition() const noexcept { return m_pos; }
    void SetPosition(const Vec2& pos) noexcept { m_pos = pos; }

    [[nodiscard]] Vec2 GetVelocity() const noexcept { return m_vel; }
    void SetVelocity(const Vec2& vel) noexcept { m_vel = vel; }

    [[nodiscard]] float GetRotation() const noexcept { return m_rotation; }
    void SetRotation(float rad) noexcept { m_rotation = rad; }

    [[nodiscard]] float GetAngularVelocity() const noexcept { return m_angularVelocity; }
    void SetAngularVelocity(float w) noexcept { m_angularVelocity = w; }

    [[nodiscard]] float GetMass() const noexcept { return m_mass; }
    [[nodiscard]] float GetInvMass() const noexcept { return m_invMass; }
    void SetMass(float mass);

    [[nodiscard]] float GetRadius() const noexcept { return m_radius; }
    void SetRadius(float r) noexcept;

    [[nodiscard]] float GetRestitution() const noexcept { return m_restitution; }
    void SetRestitution(float e) noexcept { m_restitution = e; }

    [[nodiscard]] float GetFriction() const noexcept { return m_friction; }
    void SetFriction(float mu) noexcept { m_friction = mu; }

    [[nodiscard]] float GetLinearDamping() const noexcept { return m_linearDamping; }
    void SetLinearDamping(float d) noexcept { m_linearDamping = d; }

    [[nodiscard]] float GetAngularDamping() const noexcept { return m_angularDamping; }
    void SetAngularDamping(float d) noexcept { m_angularDamping = d; }

private:
    void UpdateInertia();

    BodyType m_type{BodyType::DYNAMIC};
    Vec2 m_pos{0.0f, 0.0f};
    Vec2 m_vel{0.0f, 0.0f};
    Vec2 m_forceAccumulator{0.0f, 0.0f};

    float m_rotation{0.0f};
    float m_angularVelocity{0.0f};
    float m_torqueAccumulator{0.0f};

    float m_mass{1.0f};
    float m_invMass{1.0f};
    float m_inertia{1.0f};
    float m_invInertia{1.0f};

    float m_radius{16.0f};
    float m_restitution{0.45f};
    float m_friction{0.25f};
    float m_linearDamping{0.985f};
    float m_angularDamping{0.95f};
};

} // namespace Voidfall::Physics
