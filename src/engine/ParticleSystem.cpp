#include "engine/ParticleSystem.h"
#include "core/Random.h"
#include <cmath>

namespace Voidfall {

ParticleSystem::ParticleSystem(size_t maxCapacity) {
    m_particles.reserve(maxCapacity);
}

void ParticleSystem::Clear() {
    m_particles.clear();
}

void ParticleSystem::Update(float dt) {
    for (size_t i = 0; i < m_particles.size(); ) {
        auto& p = m_particles[i];
        p.life -= dt;
        if (p.life <= 0.0f) {
            // Swap and pop for O(1) removal
            m_particles[i] = m_particles.back();
            m_particles.pop_back();
            continue;
        }

        // Apply physics
        p.pos += p.vel * dt;
        p.rotation += p.rotSpeed * dt;

        // Damping based on particle type
        if (p.type == ParticleType::SMOKE) {
            p.vel *= (1.0f - 1.5f * dt);
        } else if (p.type == ParticleType::SPARK || p.type == ParticleType::DEBRIS) {
            p.vel *= (1.0f - 2.8f * dt);
        } else if (p.type == ParticleType::FLOATING_TEXT) {
            p.vel.y -= 15.0f * dt; // gentle rise
        }

        ++i;
    }
}

void ParticleSystem::Draw() const {
    BeginBlendMode(BLEND_ADDITIVE);

    for (const auto& p : m_particles) {
        float t = 1.0f - (p.life / p.maxLife); // [0.0, 1.0]
        float size = p.sizeStart + (p.sizeEnd - p.sizeStart) * t;

        // Color interpolation
        Color col;
        col.r = static_cast<unsigned char>(p.startColor.r + (p.endColor.r - p.startColor.r) * t);
        col.g = static_cast<unsigned char>(p.startColor.g + (p.endColor.g - p.startColor.g) * t);
        col.b = static_cast<unsigned char>(p.startColor.b + (p.endColor.b - p.startColor.b) * t);
        col.a = static_cast<unsigned char>(p.startColor.a + (p.endColor.a - p.startColor.a) * t);

        if (p.type == ParticleType::RING_SHOCKWAVE) {
            DrawRing({p.pos.x, p.pos.y}, size * 0.75f, size, 0.0f, 360.0f, 32, col);
        } else if (p.type == ParticleType::FLOATING_TEXT) {
            // Texts are drawn in standard alpha mode
            continue;
        } else {
            DrawCircleV({p.pos.x, p.pos.y}, size, col);
        }
    }

    EndBlendMode();

    // Draw floating texts with crisp outlines
    for (const auto& p : m_particles) {
        if (p.type != ParticleType::FLOATING_TEXT) continue;
        float t = 1.0f - (p.life / p.maxLife);
        Color col = p.startColor;
        col.a = static_cast<unsigned char>(255 * (1.0f - t * t));
        DrawText(p.text.c_str(), static_cast<int>(p.pos.x) - 10, static_cast<int>(p.pos.y), static_cast<int>(p.sizeStart), col);
    }
}

void ParticleSystem::SpawnSpark(const Vec2& pos, const Vec2& vel, Color color, float size, float life) {
    if (m_particles.size() >= 2000) return;
    Particle p;
    p.pos = pos;
    p.vel = vel;
    p.startColor = color;
    p.endColor = {color.r, color.g, color.b, 0};
    p.sizeStart = size;
    p.sizeEnd = 0.5f;
    p.life = life;
    p.maxLife = life;
    p.type = ParticleType::SPARK;
    m_particles.push_back(p);
}

void ParticleSystem::SpawnPlasma(const Vec2& pos, const Vec2& vel, Color color, float size, float life) {
    if (m_particles.size() >= 2000) return;
    Particle p;
    p.pos = pos;
    p.vel = vel;
    p.startColor = color;
    p.endColor = {0, 0, 0, 0};
    p.sizeStart = size;
    p.sizeEnd = size * 2.2f;
    p.life = life;
    p.maxLife = life;
    p.type = ParticleType::PLASMA;
    m_particles.push_back(p);
}

void ParticleSystem::SpawnSmoke(const Vec2& pos, const Vec2& vel, float size, float life) {
    if (m_particles.size() >= 2000) return;
    Particle p;
    p.pos = pos;
    p.vel = vel;
    p.startColor = {80, 90, 110, 160};
    p.endColor = {20, 25, 35, 0};
    p.sizeStart = size * 0.4f;
    p.sizeEnd = size * 1.8f;
    p.life = life;
    p.maxLife = life;
    p.type = ParticleType::SMOKE;
    m_particles.push_back(p);
}

void ParticleSystem::SpawnShockwave(const Vec2& pos, float maxRadius, Color color, float life) {
    if (m_particles.size() >= 2000) return;
    Particle p;
    p.pos = pos;
    p.vel = {0.0f, 0.0f};
    p.startColor = color;
    p.endColor = {color.r, color.g, color.b, 0};
    p.sizeStart = 4.0f;
    p.sizeEnd = maxRadius;
    p.life = life;
    p.maxLife = life;
    p.type = ParticleType::RING_SHOCKWAVE;
    m_particles.push_back(p);
}

void ParticleSystem::SpawnExplosion(const Vec2& pos, int count, Color primaryColor, float blastRadius) {
    SpawnShockwave(pos, blastRadius * 1.5f, primaryColor, 0.35f);

    for (int i = 0; i < count; ++i) {
        float angle = Random::RangeF(0.0f, 6.283185f);
        float speed = Random::RangeF(40.0f, 280.0f);
        Vec2 vel = Vec2::FromAngle(angle, speed);

        if (Random::Bool(0.6f)) {
            SpawnSpark(pos, vel, primaryColor, Random::RangeF(3.0f, 6.0f), Random::RangeF(0.3f, 0.7f));
        } else {
            SpawnPlasma(pos, vel * 0.5f, {255, 200, 50, 255}, Random::RangeF(5.0f, 10.0f), Random::RangeF(0.2f, 0.5f));
        }
    }

    for (int i = 0; i < 4; ++i) {
        float angle = Random::RangeF(0.0f, 6.283185f);
        Vec2 vel = Vec2::FromAngle(angle, Random::RangeF(10.0f, 50.0f));
        SpawnSmoke(pos, vel, Random::RangeF(12.0f, 22.0f), Random::RangeF(0.5f, 1.0f));
    }
}

void ParticleSystem::SpawnDamageNumber(const Vec2& pos, int damage, bool isCrit) {
    if (m_particles.size() >= 2000) return;
    Particle p;
    p.pos = pos + Vec2(Random::RangeF(-10.0f, 10.0f), Random::RangeF(-5.0f, 5.0f));
    p.vel = Vec2(Random::RangeF(-15.0f, 15.0f), Random::RangeF(-50.0f, -80.0f));
    p.startColor = isCrit ? Color{255, 60, 60, 255} : Color{255, 230, 100, 255};
    p.endColor = p.startColor;
    p.sizeStart = isCrit ? 20.0f : 14.0f;
    p.sizeEnd = p.sizeStart;
    p.life = 0.65f;
    p.maxLife = 0.65f;
    p.type = ParticleType::FLOATING_TEXT;
    p.text = (isCrit ? "CRIT " : "") + std::to_string(damage);
    m_particles.push_back(p);
}

void ParticleSystem::SpawnFloatingText(const Vec2& pos, const std::string& text, Color color) {
    if (m_particles.size() >= 2000) return;
    Particle p;
    p.pos = pos;
    p.vel = Vec2(0.0f, -40.0f);
    p.startColor = color;
    p.endColor = color;
    p.sizeStart = 16.0f;
    p.sizeEnd = 16.0f;
    p.life = 0.8f;
    p.maxLife = 0.8f;
    p.type = ParticleType::FLOATING_TEXT;
    p.text = text;
    m_particles.push_back(p);
}

} // namespace Voidfall
