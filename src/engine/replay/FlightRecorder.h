#pragma once

#include <vector>
#include <string>
#include "core/Types.h"
#include "core/math/Vector.h"

namespace Voidfall {

struct FlightTelemetrySample {
    float timestamp = 0.0f;
    Vec2 playerPos;
    Vec2 playerVel;
    float playerFacingDeg = 0.0f;
    float playerHealth = 100.0f;
    float playerShields = 100.0f;
    float playerHeat = 0.0f;
    bool isFiringPrimary = false;
    bool isFiringOrdnance = false;
    bool isDashing = false;
    int activeEnemyCount = 0;
    float bossHealthPercent = 0.0f;
};

struct CombatEventRecord {
    float timestamp = 0.0f;
    std::string description;
    float damageValue = 0.0f;
    bool isCritical = false;
    Vec2 worldPos;
};

struct PostCombatReport {
    float totalFlightTime = 0.0f;
    float totalDamageDealt = 0.0f;
    float totalDamageReceived = 0.0f;
    int totalEnemiesEliminated = 0;
    int shotsFired = 0;
    int shotsHit = 0;
    float accuracyPercent = 0.0f;
    float peakHeatPercent = 0.0f;
    float heatOverloadDuration = 0.0f;
    int perfectDodges = 0;
    std::string rankGrade = "A"; // S+, S, A, B, C, D
    int bonusNanitesAwarded = 0;
};

class FlightRecorder {
public:
    FlightRecorder();
    ~FlightRecorder() = default;

    void StartRecording();
    void StopRecording();
    bool IsRecording() const { return m_recording; }

    void RecordSample(float dt, const Vec2& pos, const Vec2& vel, float facingDeg,
                      float hp, float shields, float heat,
                      bool firingPrim, bool firingOrd, bool dashing,
                      int enemyCount, float bossHpPct);

    void RecordCombatEvent(const std::string& desc, float damage, bool isCrit, const Vec2& pos);
    void RegisterShot(bool didHit);

    // Playback control
    void StartPlayback();
    void PausePlayback();
    void ResumePlayback();
    void SeekPlayback(float timeSec);
    void SetPlaybackTimeScale(float scale) { m_timeScale = scale; }
    void UpdatePlayback(float dt);
    bool IsPlaybackActive() const { return m_playbackActive; }
    bool IsPlaybackPaused() const { return m_playbackPaused; }

    const FlightTelemetrySample* GetCurrentPlaybackSample() const;
    float GetPlaybackCursor() const { return m_playbackCursor; }
    float GetTotalDuration() const;

    // Analytics calculation
    PostCombatReport GenerateCombatReport() const;
    void Clear();

    const std::vector<FlightTelemetrySample>& GetSamples() const { return m_samples; }
    const std::vector<CombatEventRecord>& GetEvents() const { return m_events; }

private:
    bool m_recording = false;
    bool m_playbackActive = false;
    bool m_playbackPaused = false;
    float m_playbackCursor = 0.0f;
    float m_timeScale = 1.0f;

    std::vector<FlightTelemetrySample> m_samples;
    std::vector<CombatEventRecord> m_events;

    int m_totalShotsFired = 0;
    int m_totalShotsHit = 0;
    float m_totalDamageDealt = 0.0f;
    float m_totalDamageReceived = 0.0f;
    int m_enemiesDestroyed = 0;
    int m_perfectDodges = 0;
};

} // namespace Voidfall
