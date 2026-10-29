#include "gameplay/Enemies.h"
#include "gameplay/PlayerMech.h"
#include "gameplay/Weapons.h"
#include "engine/ParticleSystem.h"
#include "engine/AudioSynth.h"
#include "core/Random.h"
#include <cmath>
#include <algorithm>

namespace Voidfall {

EnemyManager::EnemyManager() {
    m_enemies.reserve(256);
}

void EnemyManager::Clear() {
    m_enemies.clear();
}

size_t EnemyManager::GetActiveCount() const {
    size_t count = 0;
    for (const auto& e : m_enemies) {
        if (e.isAlive) ++count;
    }
    return count;
}

void EnemyManager::SpawnEnemy(EnemyType type, const Vec2& pos) {
    EnemyUnit e;
    e.type = type;
    e.pos = pos;
    e.isAlive = true;
    e.affix = AffixSystem::RollRandomAffix(0.35f);

    switch (type) {
        case EnemyType::DRONE_SWARMER:
            e.radius = 12.0f;
            e.maxHp = 35.0f;
            e.hp = e.maxHp;
            e.speed = 220.0f;
            e.shootCooldown = 1.1f;
            e.naniteReward = 8;
            break;

        case EnemyType::SIEGE_CRAWLER:
            e.radius = 24.0f;
            e.maxHp = 160.0f;
            e.hp = e.maxHp;
            e.maxShield = 80.0f;
            e.shield = e.maxShield;
            e.speed = 110.0f;
            e.shootCooldown = 2.4f;
            e.naniteReward = 30;
            break;

        case EnemyType::PHASE_HUNTER:
            e.radius = 15.0f;
            e.maxHp = 70.0f;
            e.hp = e.maxHp;
            e.speed = 270.0f;
            e.shootCooldown = 1.4f;
            e.isCloaked = true;
            e.cloakAlpha = 0.2f;
            e.naniteReward = 20;
            break;

        case EnemyType::MINE_LAYER:
            e.radius = 18.0f;
            e.maxHp = 90.0f;
            e.hp = e.maxHp;
            e.speed = 160.0f;
            e.shootCooldown = 3.0f;
            e.naniteReward = 18;
            break;

        case EnemyType::ELITE_COMMANDER:
            e.radius = 28.0f;
            e.maxHp = 320.0f;
            e.hp = e.maxHp;
            e.maxShield = 150.0f;
            e.shield = e.maxShield;
            e.speed = 140.0f;
            e.shootCooldown = 1.8f;
            e.naniteReward = 65;
            break;
    }

    // Apply affix stat adjustments
    if (e.affix == EnemyAffix::SHIELDED) {
        e.maxShield += 60.0f;
        e.shield = e.maxShield;
        e.naniteReward += 15;
    } else if (e.affix == EnemyAffix::BERSERK) {
        e.speed *= 1.25f;
        e.naniteReward += 15;
    } else if (e.affix != EnemyAffix::NONE) {
        e.naniteReward += 10;
    }

    m_enemies.push_back(e);
}

void EnemyManager::SpawnWave(int waveNumber, const Vec2& playerPos, float arenaRadius) {
    int swarmCount = 3 + waveNumber * 2;
    int crawlerCount = (waveNumber >= 2) ? (1 + waveNumber / 2) : 0;
    int hunterCount = (waveNumber >= 3) ? (1 + waveNumber / 3) : 0;
    int commanderCount = (waveNumber % 3 == 0) ? 1 : 0;

    auto GetSpawnPos = [&](float minDist, float maxDist) {
        float angle = Random::RangeF(0.0f, 6.283185f);
        float dist = Random::RangeF(minDist, maxDist);
        return playerPos + Vec2::FromAngle(angle, dist);
    };

    for (int i = 0; i < swarmCount; ++i) {
        SpawnEnemy(EnemyType::DRONE_SWARMER, GetSpawnPos(350.0f, arenaRadius * 0.9f));
    }
    for (int i = 0; i < crawlerCount; ++i) {
        SpawnEnemy(EnemyType::SIEGE_CRAWLER, GetSpawnPos(450.0f, arenaRadius * 0.9f));
    }
    for (int i = 0; i < hunterCount; ++i) {
        SpawnEnemy(EnemyType::PHASE_HUNTER, GetSpawnPos(400.0f, arenaRadius * 0.9f));
    }
    for (int i = 0; i < commanderCount; ++i) {
        SpawnEnemy(EnemyType::ELITE_COMMANDER, GetSpawnPos(500.0f, arenaRadius * 0.95f));
    }
}

void EnemyManager::Update(float dt, PlayerMech& player, ProjectileManager& projManager, ParticleSystem& particles) {
    Vec2 playerPos = player.GetPosition();

    for (auto& e : m_enemies) {
        if (!e.isAlive) continue;

        e.shootTimer -= dt;
        e.aiTimer += dt;

        Vec2 toPlayer = (playerPos - e.pos);
        float distToPlayer = toPlayer.Length();
        Vec2 dirToPlayer = distToPlayer > 0.001f ? (toPlayer / distToPlayer) : Vec2(1, 0);

        e.facingAngle = dirToPlayer.Angle();

        // Archetype specific AI movement and firing
        switch (e.type) {
            case EnemyType::DRONE_SWARMER: {
                // Circle strafe around player at 220-300 units distance
                float desiredDist = 260.0f;
                Vec2 strafeDir = dirToPlayer.Rotated(1.57f);
                Vec2 moveVector = (distToPlayer > desiredDist) ? (dirToPlayer * 0.7f + strafeDir * 0.8f) :
                                  (distToPlayer < desiredDist * 0.8f) ? (dirToPlayer * -0.8f + strafeDir * 0.6f) : strafeDir;

                e.vel = moveVector.Normalized() * e.speed;
                e.pos += e.vel * dt;

                if (e.shootTimer <= 0.0f && distToPlayer < 480.0f) {
                    e.shootTimer = e.shootCooldown + Random::RangeF(-0.2f, 0.2f);
                    Projectile p;
                    p.pos = e.pos + dirToPlayer * (e.radius + 6.0f);
                    p.vel = dirToPlayer * 460.0f;
                    p.damage = 10.0f;
                    p.radius = 3.5f;
                    p.color = {255, 80, 80, 255};
                    p.isEnemy = true;
                    projManager.SpawnProjectile(p);
                    particles.SpawnPlasma(p.pos, dirToPlayer * 30.0f, {255, 80, 80, 200}, 4.0f, 0.15f);
                }
                break;
            }

            case EnemyType::SIEGE_CRAWLER: {
                // Maintain long range, fire heavy mortars
                float desiredDist = 420.0f;
                Vec2 moveVector = (distToPlayer > desiredDist) ? dirToPlayer : (dirToPlayer * -0.5f);
                e.vel = moveVector.Normalized() * e.speed;
                e.pos += e.vel * dt;

                if (e.shootTimer <= 0.0f && distToPlayer < 650.0f) {
                    e.shootTimer = e.shootCooldown;
                    // Fire 3-spread heavy artillery shells
                    for (int s = -1; s <= 1; ++s) {
                        Vec2 mortarDir = dirToPlayer.Rotated(s * 0.22f);
                        Projectile p;
                        p.pos = e.pos + mortarDir * (e.radius + 8.0f);
                        p.vel = mortarDir * 380.0f;
                        p.damage = 24.0f;
                        p.radius = 6.0f;
                        p.color = {255, 140, 40, 255};
                        p.isEnemy = true;
                        p.isExplosive = true;
                        p.explosionRadius = 35.0f;
                        projManager.SpawnProjectile(p);
                    }
                    AudioSynth::Play(SoundID::SHOOT_FLAK, 0.8f, 0.8f);
                }
                break;
            }

            case EnemyType::PHASE_HUNTER: {
                // Cloak and flank, uncloak on strike
                if (e.isCloaked) {
                    e.cloakAlpha = std::max(0.15f, e.cloakAlpha - dt * 2.0f);
                    // Flank rapidly toward player's rear
                    Vec2 targetPos = playerPos - dirToPlayer * 100.0f;
                    e.vel = (targetPos - e.pos).Normalized() * (e.speed * 1.3f);
                    e.pos += e.vel * dt;

                    if (distToPlayer < 120.0f) {
                        e.isCloaked = false;
                        e.cloakAlpha = 1.0f;
                        particles.SpawnShockwave(e.pos, 45.0f, {180, 80, 255, 255}, 0.25f);
                    }
                } else {
                    e.vel = dirToPlayer * e.speed;
                    e.pos += e.vel * dt;

                    if (e.shootTimer <= 0.0f) {
                        e.shootTimer = e.shootCooldown;
                        // Twin arc blades
                        for (int s = -1; s <= 1; s += 2) {
                            Vec2 bladeDir = dirToPlayer.Rotated(s * 0.18f);
                            Projectile p;
                            p.pos = e.pos + bladeDir * (e.radius + 6.0f);
                            p.vel = bladeDir * 580.0f;
                            p.damage = 18.0f;
                            p.radius = 4.5f;
                            p.color = {200, 60, 255, 255};
                            p.isEnemy = true;
                            projManager.SpawnProjectile(p);
                        }
                        // Re-cloak after attack
                        e.isCloaked = true;
                    }
                }
                break;
            }

            case EnemyType::ELITE_COMMANDER: {
                // Tactical leader: fires barrage of seeking missiles
                e.vel = dirToPlayer * (e.speed * (distToPlayer > 300.0f ? 1.0f : -0.6f));
                e.pos += e.vel * dt;

                if (e.shootTimer <= 0.0f && distToPlayer < 600.0f) {
                    e.shootTimer = e.shootCooldown;
                    for (int m = 0; m < 3; ++m) {
                        Vec2 mDir = dirToPlayer.Rotated(Random::RangeF(-0.35f, 0.35f));
                        Projectile p;
                        p.pos = e.pos + mDir * (e.radius + 10.0f);
                        p.vel = mDir * 320.0f;
                        p.damage = 22.0f;
                        p.radius = 5.0f;
                        p.color = {255, 50, 80, 255};
                        p.isEnemy = true;
                        p.isHoming = true;
                        p.targetPos = playerPos;
                        p.isExplosive = true;
                        p.explosionRadius = 30.0f;
                        projManager.SpawnProjectile(p);
                    }
                    AudioSynth::Play(SoundID::SHOOT_MISSILE, 0.9f, 0.9f);
                }
                break;
            }

            default:
                break;
        }

        // Projectile vs Enemy collision detection
        auto& projs = projManager.GetProjectiles();
        for (size_t pIdx = 0; pIdx < projs.size(); ++pIdx) {
            auto& p = projs[pIdx];
            if (p.isEnemy) continue;

            float hitDistSq = (e.radius + p.radius) * (e.radius + p.radius);
            if (e.pos.DistanceSq(p.pos) <= hitDistSq) {
                // Apply damage (shield first, then hull)
                float dmg = p.damage;
                if (e.shield > 0.0f) {
                    e.shield -= dmg;
                    particles.SpawnSpark(p.pos, p.vel * -0.3f, {80, 220, 255, 255}, 4.0f, 0.2f);
                    if (e.shield < 0.0f) {
                        e.hp += e.shield;
                        e.shield = 0.0f;
                    }
                } else {
                    e.hp -= dmg;
                    particles.SpawnSpark(p.pos, p.vel * -0.4f, p.color, 4.5f, 0.25f);
                }

                particles.SpawnDamageNumber(e.pos, static_cast<int>(dmg), dmg > 40.0f);

                // Handle pierce count
                if (p.pierceCount > 0) {
                    p.pierceCount--;
                } else {
                    // Remove projectile
                    projs[pIdx] = projs.back();
                    projs.pop_back();
                    --pIdx;
                }

                // Check death
                if (e.hp <= 0.0f) {
                    e.isAlive = false;
                    player.AddNaniteCores(e.naniteReward);
                    AudioSynth::Play(SoundID::EXPLOSION_SMALL, 0.9f, Random::RangeF(0.9f, 1.2f));
                    particles.SpawnExplosion(e.pos, 16, {255, 140, 50, 255}, e.radius * 2.2f);
                    particles.SpawnFloatingText(e.pos, "+" + std::to_string(e.naniteReward) + " CORES", {100, 255, 120, 255});

                    // Volatile Champion Death Burst
                    if (e.affix == EnemyAffix::VOLATILE) {
                        particles.SpawnShockwave(e.pos, 75.0f, {255, 120, 40, 255}, 0.35f);
                        for (int f = 0; f < 6; ++f) {
                            Vec2 fragDir = Vec2::FromAngle(f * 1.047f);
                            Projectile frag;
                            frag.pos = e.pos;
                            frag.vel = fragDir * 360.0f;
                            frag.damage = 18.0f;
                            frag.radius = 4.0f;
                            frag.color = {255, 100, 30, 255};
                            frag.isEnemy = true;
                            projManager.SpawnProjectile(frag);
                        }
                    }
                    break;
                }
            }
        }

        // Enemy vs Player body contact collision
        if (e.pos.DistanceSq(playerPos) <= (e.radius + player.GetRadius()) * (e.radius + player.GetRadius())) {
            player.TakeDamage(18.0f * dt, dirToPlayer, particles);
        }
    }

    // Clean up dead enemies
    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(), [](const EnemyUnit& e) { return !e.isAlive; }),
        m_enemies.end()
    );
}

void EnemyManager::Draw() const {
    for (const auto& e : m_enemies) {
        if (!e.isAlive) continue;

        Vector2 center = {e.pos.x, e.pos.y};

        Color bodyColor = {200, 50, 60, 255};
        Color outlineColor = {255, 90, 100, 255};

        if (e.type == EnemyType::SIEGE_CRAWLER) {
            bodyColor = {90, 70, 60, 255};
            outlineColor = {220, 150, 80, 255};
            DrawPoly(center, 4, e.radius, e.facingAngle * 57.29578f + 45.0f, bodyColor);
            DrawPolyLinesEx(center, 4, e.radius, e.facingAngle * 57.29578f + 45.0f, 2.5f, outlineColor);
        } else if (e.type == EnemyType::PHASE_HUNTER) {
            unsigned char a = static_cast<unsigned char>(255 * e.cloakAlpha);
            bodyColor = {140, 40, 190, a};
            outlineColor = {210, 80, 255, a};
            DrawPoly(center, 3, e.radius, e.facingAngle * 57.29578f, bodyColor);
            DrawPolyLinesEx(center, 3, e.radius, e.facingAngle * 57.29578f, 2.0f, outlineColor);
        } else if (e.type == EnemyType::ELITE_COMMANDER) {
            bodyColor = {40, 60, 90, 255};
            outlineColor = {255, 50, 50, 255};
            DrawPoly(center, 8, e.radius, e.facingAngle * 57.29578f, bodyColor);
            DrawPolyLinesEx(center, 8, e.radius, e.facingAngle * 57.29578f, 3.0f, outlineColor);
        } else {
            // Drone swarmer
            DrawPoly(center, 3, e.radius, e.facingAngle * 57.29578f, bodyColor);
            DrawPolyLinesEx(center, 3, e.radius, e.facingAngle * 57.29578f, 1.8f, outlineColor);
        }

        // Draw health bar for tough units
        if (e.maxHp > 60.0f) {
            float barW = e.radius * 2.2f;
            float barH = 4.0f;
            float hpPct = std::clamp(e.hp / e.maxHp, 0.0f, 1.0f);
            DrawRectangle(static_cast<int>(e.pos.x - barW * 0.5f), static_cast<int>(e.pos.y - e.radius - 8.0f), static_cast<int>(barW), static_cast<int>(barH), {40, 40, 40, 200});
            DrawRectangle(static_cast<int>(e.pos.x - barW * 0.5f), static_cast<int>(e.pos.y - e.radius - 8.0f), static_cast<int>(barW * hpPct), static_cast<int>(barH), {255, 60, 60, 255});
        }

        // Draw Affix Halo & Label if Champion
        if (e.affix != EnemyAffix::NONE) {
            Color affixCol = AffixSystem::GetAffixColor(e.affix);
            DrawCircleLinesV(center, e.radius + 5.0f, affixCol);
            std::string affixName = AffixSystem::GetAffixName(e.affix);
            DrawText(affixName.c_str(), static_cast<int>(e.pos.x - affixName.length() * 3), static_cast<int>(e.pos.y - e.radius - 20.0f), 10, affixCol);
        }
    }
}

} // namespace Voidfall
