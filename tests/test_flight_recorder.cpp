#include <cassert>
#include <iostream>
#include "engine/replay/FlightRecorder.h"

namespace Voidfall {

void RunFlightRecorderTests() {
    std::cout << "[TEST] Running Flight Recorder & Telemetry Tests..." << std::endl;

    FlightRecorder recorder;
    assert(!recorder.IsRecording());

    recorder.StartRecording();
    assert(recorder.IsRecording());

    // Record 60 frames of telemetry (1 second at 60fps)
    Vec2 p(100, 100);
    Vec2 v(150, 0);
    for (int i = 0; i < 60; ++i) {
        float dt = 1.0f / 60.0f;
        p = p + v * dt;
        recorder.RecordSample(dt, p, v, 45.0f, 95.0f, 80.0f, static_cast<float>(i),
                              (i % 10 == 0), false, false, 5, 1.0f);
    }

    assert(recorder.GetSamples().size() == 60);
    assert(recorder.GetTotalDuration() > 0.95f);

    // Record combat events
    recorder.RecordCombatEvent("DAMAGE_DEALT_KINETIC", 450.0f, true, Vec2(200, 200));
    recorder.RecordCombatEvent("DAMAGE_TAKEN", 65.0f, false, Vec2(150, 100));
    recorder.RecordCombatEvent("ENEMY_KILLED", 0.0f, false, Vec2(200, 200));
    recorder.RecordCombatEvent("PERFECT_DODGE", 0.0f, false, Vec2(180, 120));

    // Register shots
    recorder.RegisterShot(true);
    recorder.RegisterShot(true);
    recorder.RegisterShot(false);

    recorder.StopRecording();
    assert(!recorder.IsRecording());

    // Generate combat report
    PostCombatReport report = recorder.GenerateCombatReport();
    assert(report.totalDamageDealt == 450.0f);
    assert(report.totalDamageReceived == 65.0f);
    assert(report.totalEnemiesEliminated == 1);
    assert(report.shotsFired == 3);
    assert(report.shotsHit == 2);
    assert(report.perfectDodges == 1);
    assert(report.accuracyPercent > 66.0f);
    assert(report.rankGrade == "S+" || report.rankGrade == "S" || report.rankGrade == "A");

    // Playback scrubbing tests
    recorder.StartPlayback();
    assert(recorder.IsPlaybackActive());
    assert(!recorder.IsPlaybackPaused());

    recorder.UpdatePlayback(0.5f);
    assert(recorder.GetPlaybackCursor() >= 0.5f);

    const auto* sample = recorder.GetCurrentPlaybackSample();
    assert(sample != nullptr);
    assert(sample->timestamp <= 0.6f);

    recorder.PausePlayback();
    assert(recorder.IsPlaybackPaused());

    recorder.SeekPlayback(0.2f);
    assert(recorder.GetPlaybackCursor() == 0.2f);

    std::cout << "[PASS] Flight Recorder & Telemetry Tests Passed!" << std::endl;
}

} // namespace Voidfall
