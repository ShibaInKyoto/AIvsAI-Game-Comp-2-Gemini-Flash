#include "gameplay/EndlessSurvival.h"
#include "gameplay/PlayerMech.h"
#include "gameplay/Enemies.h"
#include "gameplay/Bosses.h"
#include "gameplay/Weapons.h"
#include "engine/ParticleSystem.h"
#include "engine/AudioSynth.h"
#include "core/Random.h"
#include "core/SaveSystem.h"
#include <cmath>
#include <algorithm>

namespace Voidfall {

EndlessSurvival::EndlessSurvival() {
    Init();
}

void EndlessSurvival::Init() {
    m_currentWave = 1;
    m_survivalTimer = 0.0f;
    m_totalKills = 0;
    m_nanitesHarvested = 0;
    m_waveTimer = 0.0f;
    m_bossActive = false;
    m_currentHazard = EnvironmentalHazard::NONE;
    m_hazardName = "SECTOR ATMOSPHERE STABLE";
    m_hazardTimer = 0.0f;
    m_currentArenaRadius = m_baseArenaRadius;
    m_meteors.clear();
    m_extractionAvailable = false;
    m_extractionCharge = 0.0f;
    m_extracted = false;
}

void EndlessSurvival::ShiftHazard(ParticleSystem& particles) {
    int next = Random::RangeI(1, 4);
    m_currentHazard = static_cast<EnvironmentalHazard>(next);
    m_hazardTimer = 0.0f;

    switch (m_currentHazard) {
        case EnvironmentalHazard::SOLAR_FLARE:
            m_hazardName = "CRITICAL: CORONAL SOLAR FLARE - HIGH RADIATION";
            AudioSynth::Play(SoundID::WARNING_KLAXON);
            break;
        case EnvironmentalHazard::ION_STORM:
            m_hazardName = "WARNING: IONIC DISCHARGE STORM - EMP ANOMALY";
            AudioSynth::Play(SoundID::WARNING_KLAXON);
            break;
        case EnvironmentalHazard::VOID_DECAY:
            m_hazardName = "DANGER: VOID DECAY HORIZON EXPANDING";
            AudioSynth::Play(SoundID::WARNING_KLAXON);
            break;
        case EnvironmentalHazard::METEOR_SHOWER:
            m_hazardName = "WARNING: DENSE KINETIC METEOR BELT INTERCEPTION";
            AudioSynth::Play(SoundID::WARNING_KLAXON);
            break;
        default:
            m_hazardName = "SECTOR ATMOSPHERE STABLE";
            break;
    }

    particles.SpawnShockwave({0.0f, 0.0f}, 600.0f, {255, 100, 150, 200});
}

void EndlessSurvival::UpdateHazards(float dt, PlayerMech& player, ParticleSystem& particles) {
    m_hazardTimer += dt;
    if (m_hazardTimer >= m_hazardDuration) {
        ShiftHazard(particles);
    }

    Vec2 pPos = player.GetPosition();

    switch (m_currentHazard) {
        case EnvironmentalHazard::SOLAR_FLARE: {
            // Radiant heat pulses every 4 seconds
            if (std::fmod(m_hazardTimer, 4.0f) < dt) {
                player.GetReactor().AddHeat(18.0f);
                particles.SpawnSpark(pPos, Vec2{Random::RangeF(-30.0f, 30.0f), Random::RangeF(-30.0f, 30.0f)}, {255, 200, 60, 255}, 3.5f, 0.3f);
            }
            break;
        }

        case EnvironmentalHazard::ION_STORM: {
            // Ion storms accelerate cooling but periodically crackle shields
            if (std::fmod(m_hazardTimer, 2.5f) < dt) {
                particles.SpawnPlasma(pPos + Vec2{Random::RangeF(-30.0f, 30.0f), Random::RangeF(-30.0f, 30.0f)}, Vec2{Random::RangeF(-20.0f, 20.0f), Random::RangeF(-20.0f, 20.0f)}, {180, 100, 255, 255}, 14.0f, 0.4f);
            }
            break;
        }

        case EnvironmentalHazard::VOID_DECAY: {
            // Arena contracts from 1400 down to 750
            float contractProgress = m_hazardTimer / m_hazardDuration;
            m_currentArenaRadius = m_baseArenaRadius - (contractProgress * 650.0f);
            break;
        }

        case EnvironmentalHazard::METEOR_SHOWER: {
            m_meteorSpawnTimer += dt;
            if (m_meteorSpawnTimer >= 0.35f) {
                m_meteorSpawnTimer = 0.0f;
                SpawnMeteors(2);
            }
            break;
        }

        default:
            m_currentArenaRadius = m_baseArenaRadius;
            break;
    }

    // Boundary constraint & void damage
    float distFromCenter = pPos.Length();
    if (distFromCenter > m_currentArenaRadius) {
        float pen = distFromCenter - m_currentArenaRadius;
        player.TakeDamage(pen * dt * 35.0f, (pPos * -1.0f).Normalized(), particles);
    }

    // Update Meteors
    for (size_t i = 0; i < m_meteors.size(); ) {
        auto& m = m_meteors[i];
        m.pos += m.vel * dt;

        // Check player collision
        if (m.pos.DistanceSq(pPos) <= (m.radius + player.GetRadius()) * (m.radius + player.GetRadius())) {
            player.TakeDamage(m.damage, m.vel.Normalized(), particles);
            particles.SpawnExplosion(m.pos, 20, {255, 120, 50, 255}, 40.0f);
            AudioSynth::Play(SoundID::EXPLOSION_SMALL);
            m_meteors[i] = m_meteors.back();
            m_meteors.pop_back();
            continue;
        }

        if (m.pos.LengthSq() > (m_baseArenaRadius * 1.5f) * (m_baseArenaRadius * 1.5f)) {
            m_meteors[i] = m_meteors.back();
            m_meteors.pop_back();
            continue;
        }

        ++i;
    }
}

void EndlessSurvival::SpawnMeteors(int count) {
    for (int i = 0; i < count; ++i) {
        Meteor m;
        float angle = Random::RangeF(0.0f, 6.283185f);
        m.pos = Vec2::FromAngle(angle, m_baseArenaRadius * 1.2f);
        Vec2 target = Vec2{Random::RangeF(-300.0f, 300.0f), Random::RangeF(-300.0f, 300.0f)};
        m.vel = (target - m.pos).Normalized() * Random::RangeF(450.0f, 750.0f);
        m.radius = Random::RangeF(14.0f, 26.0f);
        m.damage = m.radius * 2.2f;
        m_meteors.push_back(m);
    }
}

void EndlessSurvival::Update(float dt, PlayerMech& player, EnemyManager& enemies, BossEntity& boss, ProjectileManager& projManager, ParticleSystem& particles) {
    (void)projManager;
    m_survivalTimer += dt;
    m_waveTimer += dt;

    UpdateHazards(dt, player, particles);

    // Wave spawning cadence
    if (m_waveTimer >= m_waveInterval && !m_bossActive) {
        m_waveTimer = 0.0f;
        m_currentWave++;

        // Scaling spawn formula
        enemies.SpawnWave(m_currentWave, player.GetPosition(), m_currentArenaRadius);

        // Every 5 waves, trigger a boss incursion
        if (m_currentWave % 5 == 0) {
            m_bossActive = true;
            BossID bId = BossID::GOLIATH_IV;
            if (m_currentWave == 10) bId = BossID::AETHEL_CORE;
            else if (m_currentWave >= 15) bId = BossID::PROTOCOL_ZERO_TITAN;

            boss.Init(bId, {0.0f, -m_currentArenaRadius * 0.5f});
            AudioSynth::Play(SoundID::BOSS_WARNING);
        }

        // Open tactical extraction vector after wave 5 milestones
        if (m_currentWave >= 5 && (m_currentWave % 5 == 1)) {
            m_extractionAvailable = true;
            m_extractionPos = {0.0f, 0.0f};
            m_extractionCharge = 0.0f;
        }
    }

    if (m_bossActive && boss.IsDefeated()) {
        m_bossActive = false;
        m_nanitesHarvested += 150;
        player.AddNaniteCores(150);
    }

    // Check Extraction Terminal interaction
    if (m_extractionAvailable) {
        float distToBeacon = player.GetPosition().Distance(m_extractionPos);
        if (distToBeacon < 55.0f) {
            m_extractionCharge += dt;
            particles.SpawnSpark(m_extractionPos, Vec2{Random::RangeF(-30.0f, 30.0f), Random::RangeF(-30.0f, 30.0f)}, {100, 255, 200, 255}, 4.0f, 0.3f);
            if (m_extractionCharge >= 5.0f) {
                TriggerExtraction(player);
            }
        } else {
            m_extractionCharge = std::max(0.0f, m_extractionCharge - dt * 0.5f);
        }
    }
}

void EndlessSurvival::TriggerExtraction(PlayerMech& player) {
    m_extracted = true;
    m_extractionAvailable = false;
    SaveSystem::UpdateEndlessRecord(m_currentWave, m_survivalTimer, m_totalKills, player.GetNaniteCores());
}

void EndlessSurvival::ContinueNextTier() {
    m_extractionAvailable = false;
    m_extractionCharge = 0.0f;
    AudioSynth::Play(SoundID::UPGRADE_SELECT);
}

void EndlessSurvival::Draw() const {
    // 1. Draw Arena Boundary
    Color boundaryColor = (m_currentHazard == EnvironmentalHazard::VOID_DECAY) ? Color{220, 40, 60, 220} : Color{60, 120, 200, 180};
    DrawCircleLines(0, 0, m_currentArenaRadius, boundaryColor);
    DrawCircleLines(0, 0, m_currentArenaRadius + 4.0f, {boundaryColor.r, boundaryColor.g, boundaryColor.b, 80});

    // 2. Draw Meteors
    for (const auto& m : m_meteors) {
        DrawCircleV({m.pos.x, m.pos.y}, m.radius, m.color);
        DrawCircleLines(static_cast<int>(m.pos.x), static_cast<int>(m.pos.y), m.radius * 1.3f, {255, 200, 80, 180});
    }

    // 3. Draw Extraction Beacon if available
    if (m_extractionAvailable) {
        DrawCircleLines(static_cast<int>(m_extractionPos.x), static_cast<int>(m_extractionPos.y), 55.0f, {100, 255, 180, 255});
        float progressAngle = (m_extractionCharge / 5.0f) * 360.0f;
        DrawRing({m_extractionPos.x, m_extractionPos.y}, 50.0f, 56.0f, 0.0f, progressAngle, 32, {100, 255, 180, 240});
        DrawText("EXTRACTION BEACON", static_cast<int>(m_extractionPos.x) - 75, static_cast<int>(m_extractionPos.y) - 70, 14, {100, 255, 180, 255});
    }
}

} // namespace Voidfall
