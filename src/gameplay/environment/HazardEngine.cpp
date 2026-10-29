#include "HazardEngine.h"
#include "core/Random.h"
#include <cmath>
#include <algorithm>

namespace Voidfall {

HazardEngine::HazardEngine() {
    Initialize(HazardType::ASTEROID_FIELD, 1280, 720);
}

void HazardEngine::Initialize(HazardType primaryHazard, int screenWidth, int screenHeight) {
    m_currentHazardType = primaryHazard;
    m_asteroids.clear();
    m_singularities.clear();
    m_zones.clear();
    m_nextAsteroidId = 1;
    m_solarStormActive = false;
    m_solarStormTimer = 0.0f;
    m_solarStormIntensity = 0.0f;

    if (primaryHazard == HazardType::ASTEROID_FIELD) {
        SpawnAsteroids(18);
    } else if (primaryHazard == HazardType::GRAVITATIONAL_SINGULARITY) {
        SingularityEntity s;
        s.position = Vec2(static_cast<float>(screenWidth) * 0.5f, static_cast<float>(screenHeight) * 0.5f);
        s.mass = 160000.0f;
        s.eventHorizonRadius = 40.0f;
        s.accretionDiskRadius = 260.0f;
        m_singularities.push_back(s);

        // A few orbiting asteroids around the singularity
        SpawnAsteroids(8);
    } else if (primaryHazard == HazardType::ION_NEBULA_CLOUD) {
        HazardZone z1;
        z1.id = 1;
        z1.type = HazardType::ION_NEBULA_CLOUD;
        z1.position = Vec2(300.0f, 300.0f);
        z1.radius = 240.0f;
        z1.tintColor = Color{ 40, 180, 220, 70 };
        m_zones.push_back(z1);

        HazardZone z2;
        z2.id = 2;
        z2.type = HazardType::ION_NEBULA_CLOUD;
        z2.position = Vec2(950.0f, 480.0f);
        z2.radius = 280.0f;
        z2.tintColor = Color{ 80, 120, 240, 60 };
        m_zones.push_back(z2);
    } else if (primaryHazard == HazardType::SOLAR_FLARE_STORM) {
        m_solarStormCycle = 15.0f;
    }
}

void HazardEngine::GenerateAsteroidPolygon(AsteroidEntity& ast) {
    ast.polygonVertices.clear();
    const int numVerts = 10;
    for (int i = 0; i < numVerts; ++i) {
        float angle = (static_cast<float>(i) / static_cast<float>(numVerts)) * (Math::PI * 2.0f);
        float radiusVariation = ast.radius * (0.75f + Random::Range(0.0f, 0.5f));
        ast.polygonVertices.push_back(Vec2(std::cos(angle) * radiusVariation, std::sin(angle) * radiusVariation));
    }
}

void HazardEngine::SpawnAsteroids(int count) {
    for (int i = 0; i < count; ++i) {
        AsteroidEntity ast;
        ast.id = m_nextAsteroidId++;
        ast.position = Vec2(Random::Range(50.0f, 1500.0f), Random::Range(50.0f, 1100.0f));
        ast.velocity = Vec2(Random::Range(-35.0f, 35.0f), Random::Range(-35.0f, 35.0f));
        ast.rotation = Random::Range(0.0f, Math::PI * 2.0f);
        ast.rotSpeed = Random::Range(-0.8f, 0.8f);
        ast.tier = (i % 3 == 0) ? 3 : 2;
        ast.radius = (ast.tier == 3) ? 55.0f : 32.0f;
        ast.maxHealth = (ast.tier == 3) ? 140.0f : 70.0f;
        ast.health = ast.maxHealth;
        ast.mineralValue = (ast.tier == 3) ? 35 : 18;

        GenerateAsteroidPolygon(ast);
        m_asteroids.push_back(ast);
    }
}

bool HazardEngine::DamageAsteroid(int id, float damage, std::vector<AsteroidEntity>& outSplinters) {
    outSplinters.clear();
    for (size_t i = 0; i < m_asteroids.size(); ++i) {
        if (m_asteroids[i].id == id) {
            m_asteroids[i].health -= damage;
            if (m_asteroids[i].health <= 0.0f) {
                // If tier > 1, splinter into 2 smaller asteroids
                if (m_asteroids[i].tier > 1) {
                    for (int s = 0; s < 2; ++s) {
                        AsteroidEntity splinter;
                        splinter.id = m_nextAsteroidId++;
                        float angle = Random::Range(0.0f, Math::PI * 2.0f);
                        splinter.position = m_asteroids[i].position + Vec2(std::cos(angle) * 15.0f, std::sin(angle) * 15.0f);
                        splinter.velocity = m_asteroids[i].velocity + Vec2(std::cos(angle) * 60.0f, std::sin(angle) * 60.0f);
                        splinter.rotation = angle;
                        splinter.rotSpeed = Random::Range(-1.5f, 1.5f);
                        splinter.tier = m_asteroids[i].tier - 1;
                        splinter.radius = (splinter.tier == 2) ? 30.0f : 16.0f;
                        splinter.maxHealth = (splinter.tier == 2) ? 60.0f : 25.0f;
                        splinter.health = splinter.maxHealth;
                        splinter.mineralValue = 10;
                        GenerateAsteroidPolygon(splinter);
                        outSplinters.push_back(splinter);
                    }
                }
                m_asteroids.erase(m_asteroids.begin() + i);
                return true;
            }
            return false;
        }
    }
    return false;
}

bool HazardEngine::IsInIonNebula(const Vec2& pos) const {
    for (const auto& z : m_zones) {
        if (z.type == HazardType::ION_NEBULA_CLOUD) {
            if ((pos - z.position).Length() < z.radius) {
                return true;
            }
        }
    }
    return false;
}

void HazardEngine::Update(float dt, Vec2& playerPos, Vec2& playerVel, float& playerHeat, float& playerShields, float& playerHp) {
    (void)playerHp;
    // 1. Update Asteroids
    for (auto& ast : m_asteroids) {
        ast.position = ast.position + ast.velocity * dt;
        ast.rotation += ast.rotSpeed * dt;

        // Wrap around boundaries
        if (ast.position.x < -100.0f) ast.position.x = 1800.0f;
        if (ast.position.x > 1800.0f) ast.position.x = -100.0f;
        if (ast.position.y < -100.0f) ast.position.y = 1300.0f;
        if (ast.position.y > 1300.0f) ast.position.y = -100.0f;

        // Elastic push vs player
        float distToPlayer = (ast.position - playerPos).Length();
        if (distToPlayer < (ast.radius + 24.0f) && distToPlayer > 0.001f) {
            Vec2 normal = (playerPos - ast.position).Normalized();
            float overlap = (ast.radius + 24.0f) - distToPlayer;
            playerPos = playerPos + normal * overlap;
            playerVel = playerVel + normal * 120.0f;
            ast.velocity = ast.velocity - normal * 30.0f;
        }
    }

    // 2. Singularities Gravity Well (inverse square law)
    for (auto& s : m_singularities) {
        s.rotationAngle += dt * 1.8f;
        Vec2 toSing = s.position - playerPos;
        float d = toSing.Length();
        if (d > s.eventHorizonRadius && d < s.accretionDiskRadius) {
            float forceMag = (s.mass) / (d * d + 1500.0f);
            Vec2 pull = toSing.Normalized() * forceMag;
            playerVel = playerVel + pull * dt;
        }

        // Pull nearby asteroids into orbit or consumption
        for (auto& ast : m_asteroids) {
            Vec2 toSingAst = s.position - ast.position;
            float dAst = toSingAst.Length();
            if (dAst > s.eventHorizonRadius && dAst < s.accretionDiskRadius * 1.4f) {
                float f = (s.mass * 0.7f) / (dAst * dAst + 1000.0f);
                ast.velocity = ast.velocity + toSingAst.Normalized() * f * dt;
            }
        }
    }

    // 3. Ion Nebula Cloud Effects
    if (IsInIonNebula(playerPos)) {
        // Suppress shield regeneration slightly
        if (playerShields > 20.0f) {
            playerShields -= dt * 6.0f;
        }
    }

    // 4. Solar Storm Flares
    if (m_currentHazardType == HazardType::SOLAR_FLARE_STORM) {
        m_solarStormTimer += dt;
        if (m_solarStormTimer >= m_solarStormCycle) {
            m_solarStormTimer = 0.0f;
        }

        // Flare peaks in middle of cycle
        if (m_solarStormTimer > 10.0f && m_solarStormTimer < 14.0f) {
            m_solarStormActive = true;
            m_solarStormIntensity = (m_solarStormTimer - 10.0f) / 2.0f;
            if (m_solarStormTimer > 12.0f) {
                m_solarStormIntensity = (14.0f - m_solarStormTimer) / 2.0f;
            }
            playerHeat = std::min(100.0f, playerHeat + dt * 14.0f * m_solarStormIntensity);
        } else {
            m_solarStormActive = false;
            m_solarStormIntensity = 0.0f;
        }
    }
}

void HazardEngine::Render() {
    // 1. Render Ion Nebula Clouds
    for (const auto& z : m_zones) {
        DrawCircleGradient(static_cast<int>(z.position.x), static_cast<int>(z.position.y),
                           z.radius, z.tintColor, Color{ 0, 0, 0, 0 });
        DrawCircleLines(static_cast<int>(z.position.x), static_cast<int>(z.position.y),
                        z.radius, Color{ z.tintColor.r, z.tintColor.g, z.tintColor.b, 100 });
    }

    // 2. Render Singularities & Accretion Disks
    for (const auto& s : m_singularities) {
        // Outer swirl rings
        for (int r = 0; r < 5; ++r) {
            float ringR = s.eventHorizonRadius + static_cast<float>(r) * 38.0f;
            float alpha = 180.0f - static_cast<float>(r) * 30.0f;
            DrawCircleLines(static_cast<int>(s.position.x), static_cast<int>(s.position.y), ringR,
                            Color{ 180, 80, 255, static_cast<unsigned char>(alpha) });
        }
        // Black event horizon center
        DrawCircle(static_cast<int>(s.position.x), static_cast<int>(s.position.y), s.eventHorizonRadius, Color{ 0, 0, 0, 255 });
        DrawCircleLines(static_cast<int>(s.position.x), static_cast<int>(s.position.y), s.eventHorizonRadius + 2.0f, Color{ 255, 230, 180, 240 });
    }

    // 3. Render Asteroids with polygon wireframe & shading
    for (const auto& ast : m_asteroids) {
        if (ast.polygonVertices.size() < 3) continue;

        // Transform vertices
        std::vector<Vector2> screenVerts;
        float cosR = std::cos(ast.rotation);
        float sinR = std::sin(ast.rotation);

        for (const auto& v : ast.polygonVertices) {
            float rx = v.x * cosR - v.y * sinR;
            float ry = v.x * sinR + v.y * cosR;
            screenVerts.push_back({ ast.position.x + rx, ast.position.y + ry });
        }

        // Draw solid polygon hull
        Color rockCol = (ast.tier == 3) ? Color{ 75, 70, 85, 255 } : Color{ 90, 85, 95, 255 };
        Color edgeCol = Color{ 140, 135, 155, 255 };

        for (size_t v = 0; v < screenVerts.size(); ++v) {
            Vector2 p1 = screenVerts[v];
            Vector2 p2 = screenVerts[(v + 1) % screenVerts.size()];
            DrawLineEx(p1, p2, 2.0f, edgeCol);
        }

        // Draw interior detail lines (crater fissures)
        Vector2 c = { ast.position.x, ast.position.y };
        DrawLineEx(c, screenVerts[0], 1.0f, Color{ 60, 55, 70, 180 });
        DrawLineEx(c, screenVerts[screenVerts.size() / 2], 1.0f, Color{ 60, 55, 70, 180 });

        // Health bar if damaged
        if (ast.health < ast.maxHealth) {
            float barW = ast.radius * 1.5f;
            float pct = ast.health / ast.maxHealth;
            DrawRectangle(static_cast<int>(ast.position.x - barW * 0.5f), static_cast<int>(ast.position.y - ast.radius - 8.0f),
                          static_cast<int>(barW), 3, Color{ 30, 30, 30, 200 });
            DrawRectangle(static_cast<int>(ast.position.x - barW * 0.5f), static_cast<int>(ast.position.y - ast.radius - 8.0f),
                          static_cast<int>(barW * pct), 3, Color{ 220, 180, 60, 255 });
        }
    }

    // 4. Solar Storm ambient flash
    if (m_solarStormActive && m_solarStormIntensity > 0.05f) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                      Color{ 255, 120, 20, static_cast<unsigned char>(m_solarStormIntensity * 60.0f) });
    }
}

} // namespace Voidfall
