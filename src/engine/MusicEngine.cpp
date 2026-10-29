#include "engine/MusicEngine.h"
#include "core/Config.h"
#include "core/Random.h"
#include <cmath>
#include <algorithm>
#include <cstring>

namespace Voidfall {

AudioStream MusicEngine::s_stream{};
bool MusicEngine::s_active = false;
float MusicEngine::s_volume = 0.5f;
MusicIntensity MusicEngine::s_intensity = MusicIntensity::COMBAT_NORMAL;

float MusicEngine::s_bpm = 124.0f;
float MusicEngine::s_stepTimer = 0.0f;
int MusicEngine::s_currentStep = 0;
int MusicEngine::s_currentBar = 0;

float MusicEngine::s_bassPhase = 0.0f;
float MusicEngine::s_leadPhase = 0.0f;
float MusicEngine::s_padPhase = 0.0f;
float MusicEngine::s_drumTimer = 0.0f;

// Musical note frequencies (A minor / D minor progression)
static const float s_bassFreqs[4] = { 55.0f, 65.41f, 43.65f, 49.0f }; // A1, C2, F1, G1
static const float s_leadScale[8] = { 220.0f, 246.94f, 261.63f, 293.66f, 329.63f, 349.23f, 392.0f, 440.0f };

void MusicEngine::AudioStreamCallback(void* buffer, unsigned int frames) {
    float* out = static_cast<float*>(buffer);
    const float sampleRate = 44100.0f;
    const float dt = 1.0f / sampleRate;

    float stepDuration = (60.0f / s_bpm) * 0.25f; // 16th note

    for (unsigned int i = 0; i < frames; ++i) {
        s_stepTimer += dt;
        if (s_stepTimer >= stepDuration) {
            s_stepTimer -= stepDuration;
            s_currentStep = (s_currentStep + 1) % 16;
            if (s_currentStep == 0) {
                s_currentBar = (s_currentBar + 1) % 4;
            }
        }

        // 1. Kick Drum (Steps 0, 4, 8, 12)
        float kick = 0.0f;
        int kickBeat = s_currentStep % 4;
        if (kickBeat == 0 && s_intensity != MusicIntensity::AMBIENT) {
            float kickT = s_stepTimer / (stepDuration * 1.5f);
            if (kickT < 1.0f) {
                float freq = 120.0f * std::exp(-kickT * 14.0f);
                kick = std::sin(freq * s_stepTimer * 6.283185f) * (1.0f - kickT);
            }
        }

        // 2. Snare Drum (Steps 4, 12)
        float snare = 0.0f;
        if ((s_currentStep == 4 || s_currentStep == 12) && s_intensity != MusicIntensity::AMBIENT) {
            float snareT = s_stepTimer / (stepDuration * 1.8f);
            if (snareT < 1.0f) {
                float noise = (std::fmod(s_stepTimer * 98765.4321f, 2.0f) - 1.0f);
                float tone = std::sin(180.0f * s_stepTimer * 6.283185f);
                snare = (noise * 0.7f + tone * 0.3f) * (1.0f - snareT);
            }
        }

        // 3. Hi-Hat (Every odd 16th note)
        float hat = 0.0f;
        if (s_currentStep % 2 == 1 && s_intensity != MusicIntensity::AMBIENT) {
            float hatT = s_stepTimer / (stepDuration * 0.6f);
            if (hatT < 1.0f) {
                float noise = (std::fmod(s_stepTimer * 123456.7f, 2.0f) - 1.0f);
                hat = noise * (1.0f - hatT) * 0.25f;
            }
        }

        // 4. Bassline Voice (Rolling 16th note synthwave bass)
        float bassTargetFreq = s_bassFreqs[s_currentBar];
        if (s_currentStep % 4 == 2) bassTargetFreq *= 1.5f; // octave/fifth harmonic jump
        s_bassPhase += 6.283185f * bassTargetFreq * dt;
        if (s_bassPhase > 6.283185f) s_bassPhase -= 6.283185f;

        // Saturated Sawtooth bass
        float saw = (s_bassPhase / 3.14159265f) - 1.0f;
        float bassEnv = (1.0f - (s_stepTimer / stepDuration) * 0.4f);
        float bass = std::clamp(saw * 1.4f, -1.0f, 1.0f) * bassEnv * 0.35f;

        // 5. Arpeggio / Lead Voice
        float lead = 0.0f;
        if (s_intensity == MusicIntensity::COMBAT_HIGH || s_intensity == MusicIntensity::BOSS_BATTLE) {
            int noteIdx = (s_currentStep * 3 + s_currentBar) % 8;
            float leadFreq = s_leadScale[noteIdx];
            s_leadPhase += 6.283185f * leadFreq * dt;
            if (s_leadPhase > 6.283185f) s_leadPhase -= 6.283185f;

            float leadEnv = std::exp(-(s_stepTimer / stepDuration) * 3.5f);
            lead = std::sin(s_leadPhase) * leadEnv * 0.22f;
        }

        // Mix all channels
        float mixed = (kick * 0.55f + snare * 0.45f + hat * 0.3f + bass * 0.5f + lead * 0.4f) * s_volume;
        out[i] = std::clamp(mixed, -0.95f, 0.95f);
    }
}

void MusicEngine::Initialize() {
    auto& cfg = ConfigManager::Get();
    if (!cfg.masterAudioEnabled || s_active) return;

    if (!IsAudioDeviceReady()) {
        InitAudioDevice();
    }

    SetAudioStreamBufferSizeDefault(1024);
    s_stream = LoadAudioStream(44100, 32, 1);
    SetAudioStreamCallback(s_stream, AudioStreamCallback);
    PlayAudioStream(s_stream);
    s_active = true;
    s_volume = cfg.musicVolume;
}

void MusicEngine::Shutdown() {
    if (!s_active) return;
    StopAudioStream(s_stream);
    UnloadAudioStream(s_stream);
    s_active = false;
}

void MusicEngine::Update(float dt) {
    (void)dt;
    if (s_active) {
        // Stream processes via hardware callback
    }
}

void MusicEngine::SetIntensity(MusicIntensity intensity) {
    s_intensity = intensity;
    if (intensity == MusicIntensity::BOSS_BATTLE) {
        s_bpm = 138.0f;
    } else if (intensity == MusicIntensity::COMBAT_HIGH) {
        s_bpm = 128.0f;
    } else {
        s_bpm = 120.0f;
    }
}

void MusicEngine::SetVolume(float volume) {
    s_volume = std::clamp(volume, 0.0f, 1.0f);
}

} // namespace Voidfall
