#include "engine/physics/RigidBody.h"
#include <algorithm>

namespace Voidfall::Physics {

RigidBody2D::RigidBody2D(BodyType type, float mass, float radius, float restitution, float friction)
    : m_type(type), m_radius(radius), m_restitution(restitution), m_friction(friction) {
    SetMass(mass);
}

void RigidBody2D::SetMass(float mass) {
    m_mass = mass;
    if (m_type == BodyType::DYNAMIC && mass > 0.0f) {
        m_invMass = 1.0f / mass;
    } else {
        m_invMass = 0.0f;
    }
    UpdateInertia();
}

void RigidBody2D::SetRadius(float r) noexcept {
    m_radius = r;
    UpdateInertia();
}

void RigidBody2D::UpdateInertia() {
    if (m_type == BodyType::DYNAMIC && m_mass > 0.0f) {
        // Solid cylinder moment of inertia: 0.5 * m * r^2
        m_inertia = 0.5f * m_mass * m_radius * m_radius;
        m_invInertia = 1.0f / m_inertia;
    } else {
        m_inertia = 0.0f;
        m_invInertia = 0.0f;
    }
}

void RigidBody2D::ApplyForce(const Vec2& force) {
    if (m_type != BodyType::DYNAMIC) return;
    m_forceAccumulator += force;
}

void RigidBody2D::ApplyImpulse(const Vec2& impulse) {
    if (m_type != BodyType::DYNAMIC) return;
    m_vel += impulse * m_invMass;
}

void RigidBody2D::ApplyTorque(float torque) {
    if (m_type != BodyType::DYNAMIC) return;
    m_torqueAccumulator += torque;
}

void RigidBody2D::ApplyAngularImpulse(float angularImpulse) {
    if (m_type != BodyType::DYNAMIC) return;
    m_angularVelocity += angularImpulse * m_invInertia;
}

void RigidBody2D::Integrate(float dt) {
    if (m_type == BodyType::STATIC) return;

    if (m_type == BodyType::DYNAMIC) {
        // Linear velocity Verlet
        Vec2 accel = m_forceAccumulator * m_invMass;
        m_vel += accel * dt;
        m_vel *= std::pow(m_linearDamping, dt * 60.0f);

        // Angular velocity
        float angAccel = m_torqueAccumulator * m_invInertia;
        m_angularVelocity += angAccel * dt;
        m_angularVelocity *= std::pow(m_angularDamping, dt * 60.0f);
    }

    m_pos += m_vel * dt;
    m_rotation += m_angularVelocity * dt;

    // Reset forces for next step
    m_forceAccumulator = {0.0f, 0.0f};
    m_torqueAccumulator = 0.0f;
}

CollisionManifold RigidBody2D::CheckCircleCircle(const RigidBody2D& a, const RigidBody2D& b) {
    CollisionManifold manifold;
    Vec2 delta = b.m_pos - a.m_pos;
    float distSq = delta.LengthSq();
    float radiusSum = a.m_radius + b.m_radius;

    if (distSq >= radiusSum * radiusSum || distSq < 0.000001f) {
        return manifold;
    }

    float dist = std::sqrt(distSq);
    manifold.hasCollision = true;
    manifold.normal = delta / dist;
    manifold.penetration = radiusSum - dist;
    manifold.contactPoint = a.m_pos + manifold.normal * a.m_radius;

    return manifold;
}

void RigidBody2D::ResolveCollision(RigidBody2D& a, RigidBody2D& b, const CollisionManifold& manifold) {
    if (!manifold.hasCollision) return;

    float totalInvMass = a.m_invMass + b.m_invMass;
    if (totalInvMass <= 0.00001f) return;

    // Positional separation to prevent sinking
    const float percent = 0.8f;
    const float slop = 0.01f;
    Vec2 correction = manifold.normal * (std::max(manifold.penetration - slop, 0.0f) / totalInvMass * percent);

    if (a.m_type == BodyType::DYNAMIC) a.m_pos -= correction * a.m_invMass;
    if (b.m_type == BodyType::DYNAMIC) b.m_pos += correction * b.m_invMass;

    // Relative velocity
    Vec2 relVel = b.m_vel - a.m_vel;
    float velAlongNormal = relVel.Dot(manifold.normal);

    // Do not resolve if velocities are separating
    if (velAlongNormal > 0.0f) return;

    // Restitution (elasticity)
    float e = std::min(a.m_restitution, b.m_restitution);
    float j = -(1.0f + e) * velAlongNormal / totalInvMass;

    Vec2 impulse = manifold.normal * j;
    if (a.m_type == BodyType::DYNAMIC) a.m_vel -= impulse * a.m_invMass;
    if (b.m_type == BodyType::DYNAMIC) b.m_vel += impulse * b.m_invMass;

    // Friction impulse
    Vec2 tangent = relVel - manifold.normal * velAlongNormal;
    float tanLen = tangent.Length();
    if (tanLen > 0.0001f) {
        tangent = tangent / tanLen;
        float jt = -relVel.Dot(tangent) / totalInvMass;
        float mu = std::sqrt(a.m_friction * b.m_friction);

        Vec2 frictionImpulse;
        if (std::abs(jt) < j * mu) {
            frictionImpulse = tangent * jt;
        } else {
            frictionImpulse = tangent * (-j * mu);
        }

        if (a.m_type == BodyType::DYNAMIC) a.m_vel -= frictionImpulse * a.m_invMass;
        if (b.m_type == BodyType::DYNAMIC) b.m_vel += frictionImpulse * b.m_invMass;
    }
}

} // namespace Voidfall::Physics
