#pragma once

#include <raylib.h>
#include <vector>
#include <cstdint>

namespace Voidfall {

enum class MusicIntensity {
    AMBIENT,
    COMBAT_NORMAL,
    COMBAT_HIGH,
    BOSS_BATTLE
};

class MusicEngine {
public:
    static void Initialize();
    static void Shutdown();
    static void Update(float dt);
    static void SetIntensity(MusicIntensity intensity);
    static void SetVolume(float volume);

private:
    static void AudioStreamCallback(void* buffer, unsigned int frames);

    static AudioStream s_stream;
    static bool s_active;
    static float s_volume;
    static MusicIntensity s_intensity;

    // Music sequencer state
    static float s_bpm;
    static float s_stepTimer;
    static int s_currentStep;
    static int s_currentBar;

    // Synthesizer voice states
    static float s_bassPhase;
    static float s_leadPhase;
    static float s_padPhase;
    static float s_drumTimer;
};

} // namespace Voidfall
