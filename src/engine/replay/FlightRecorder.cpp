#include "FlightRecorder.h"
#include <algorithm>
#include <cmath>

namespace Voidfall {

FlightRecorder::FlightRecorder() = default;

void FlightRecorder::StartRecording() {
    Clear();
    m_recording = true;
    m_playbackActive = false;
    m_playbackPaused = false;
    m_playbackCursor = 0.0f;
}

void FlightRecorder::StopRecording() {
    m_recording = false;
}

void FlightRecorder::Clear() {
    m_samples.clear();
    m_events.clear();
    m_recording = false;
    m_playbackActive = false;
    m_playbackPaused = false;
    m_playbackCursor = 0.0f;
    m_totalShotsFired = 0;
    m_totalShotsHit = 0;
    m_totalDamageDealt = 0.0f;
    m_totalDamageReceived = 0.0f;
    m_enemiesDestroyed = 0;
    m_perfectDodges = 0;
}

void FlightRecorder::RecordSample(float dt, const Vec2& pos, const Vec2& vel, float facingDeg,
                                  float hp, float shields, float heat,
                                  bool firingPrim, bool firingOrd, bool dashing,
                                  int enemyCount, float bossHpPct) {
    if (!m_recording) return;

    float currentTimestamp = m_samples.empty() ? 0.0f : (m_samples.back().timestamp + dt);

    FlightTelemetrySample s;
    s.timestamp = currentTimestamp;
    s.playerPos = pos;
    s.playerVel = vel;
    s.playerFacingDeg = facingDeg;
    s.playerHealth = hp;
    s.playerShields = shields;
    s.playerHeat = heat;
    s.isFiringPrimary = firingPrim;
    s.isFiringOrdnance = firingOrd;
    s.isDashing = dashing;
    s.activeEnemyCount = enemyCount;
    s.bossHealthPercent = bossHpPct;

    m_samples.push_back(s);
}

void FlightRecorder::RecordCombatEvent(const std::string& desc, float damage, bool isCrit, const Vec2& pos) {
    if (!m_recording) return;

    float t = m_samples.empty() ? 0.0f : m_samples.back().timestamp;
    CombatEventRecord ev;
    ev.timestamp = t;
    ev.description = desc;
    ev.damageValue = damage;
    ev.isCritical = isCrit;
    ev.worldPos = pos;
    m_events.push_back(ev);

    if (desc.find("DAMAGE_DEALT") != std::string::npos) {
        m_totalDamageDealt += damage;
    } else if (desc.find("DAMAGE_TAKEN") != std::string::npos) {
        m_totalDamageReceived += damage;
    } else if (desc.find("ENEMY_KILLED") != std::string::npos) {
        m_enemiesDestroyed++;
    } else if (desc.find("PERFECT_DODGE") != std::string::npos) {
        m_perfectDodges++;
    }
}

void FlightRecorder::RegisterShot(bool didHit) {
    if (!m_recording) return;
    m_totalShotsFired++;
    if (didHit) {
        m_totalShotsHit++;
    }
}

void FlightRecorder::StartPlayback() {
    if (m_samples.empty()) return;
    m_recording = false;
    m_playbackActive = true;
    m_playbackPaused = false;
    m_playbackCursor = 0.0f;
}

void FlightRecorder::PausePlayback() {
    m_playbackPaused = true;
}

void FlightRecorder::ResumePlayback() {
    m_playbackPaused = false;
}

void FlightRecorder::SeekPlayback(float timeSec) {
    float maxT = GetTotalDuration();
    m_playbackCursor = std::max(0.0f, std::min(maxT, timeSec));
}

void FlightRecorder::UpdatePlayback(float dt) {
    if (!m_playbackActive || m_playbackPaused || m_samples.empty()) return;

    m_playbackCursor += dt * m_timeScale;
    if (m_playbackCursor > GetTotalDuration()) {
        m_playbackCursor = GetTotalDuration();
        m_playbackPaused = true;
    }
}

float FlightRecorder::GetTotalDuration() const {
    if (m_samples.empty()) return 0.0f;
    return m_samples.back().timestamp;
}

const FlightTelemetrySample* FlightRecorder::GetCurrentPlaybackSample() const {
    if (m_samples.empty()) return nullptr;

    // Binary search for closest timestamp
    auto it = std::lower_bound(m_samples.begin(), m_samples.end(), m_playbackCursor,
        [](const FlightTelemetrySample& s, float t) {
            return s.timestamp < t;
        });

    if (it == m_samples.end()) {
        return &m_samples.back();
    }
    return &(*it);
}

PostCombatReport FlightRecorder::GenerateCombatReport() const {
    PostCombatReport rep;
    rep.totalFlightTime = GetTotalDuration();
    rep.totalDamageDealt = m_totalDamageDealt;
    rep.totalDamageReceived = m_totalDamageReceived;
    rep.totalEnemiesEliminated = m_enemiesDestroyed;
    rep.shotsFired = m_totalShotsFired;
    rep.shotsHit = m_totalShotsHit;
    rep.perfectDodges = m_perfectDodges;

    if (m_totalShotsFired > 0) {
        rep.accuracyPercent = (static_cast<float>(m_totalShotsHit) / static_cast<float>(m_totalShotsFired)) * 100.0f;
    } else {
        rep.accuracyPercent = 100.0f;
    }

    // Heat peak & overload tracking
    float peakHeat = 0.0f;
    float overloadTime = 0.0f;
    for (size_t i = 0; i < m_samples.size(); ++i) {
        if (m_samples[i].playerHeat > peakHeat) {
            peakHeat = m_samples[i].playerHeat;
        }
        if (m_samples[i].playerHeat >= 90.0f && i > 0) {
            overloadTime += (m_samples[i].timestamp - m_samples[i - 1].timestamp);
        }
    }
    rep.peakHeatPercent = peakHeat;
    rep.heatOverloadDuration = overloadTime;

    // Calculate Rank
    int score = 0;
    if (rep.accuracyPercent >= 80.0f) score += 30;
    else if (rep.accuracyPercent >= 60.0f) score += 20;
    else if (rep.accuracyPercent >= 40.0f) score += 10;

    if (rep.totalDamageReceived < 150.0f) score += 30;
    else if (rep.totalDamageReceived < 400.0f) score += 20;
    else if (rep.totalDamageReceived < 800.0f) score += 10;

    score += std::min(25, rep.totalEnemiesEliminated * 2);
    score += std::min(15, rep.perfectDodges * 3);

    if (score >= 90) rep.rankGrade = "S+";
    else if (score >= 75) rep.rankGrade = "S";
    else if (score >= 60) rep.rankGrade = "A";
    else if (score >= 45) rep.rankGrade = "B";
    else if (score >= 30) rep.rankGrade = "C";
    else rep.rankGrade = "D";

    rep.bonusNanitesAwarded = score * 12;
    return rep;
}

} // namespace Voidfall
