#include "engine/audio/VoiceSynthesizer.h"
#include "engine/audio/DSPCore.h"
#include "core/Config.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace Voidfall::Audio {

std::unordered_map<TacticalAlert, Sound> VoiceSynthesizer::s_alertSounds;
bool VoiceSynthesizer::s_initialized = false;

Wave VoiceSynthesizer::SynthesizePhrase(const std::string& phoneticPhrase) {
    (void)phoneticPhrase;
    int sampleRate = 44100;
    float duration = 1.25f;
    int sampleCount = static_cast<int>(duration * sampleRate);

    std::vector<int16_t> buffer(sampleCount);

    // Formant filter pair (F1 = 700 Hz, F2 = 1800 Hz) typical of vocal tract
    BiquadFilter f1;
    BiquadFilter f2;
    f1.Configure(FilterType::BANDPASS, 650.0f, 6.0f, static_cast<float>(sampleRate));
    f2.Configure(FilterType::BANDPASS, 1750.0f, 5.0f, static_cast<float>(sampleRate));

    float phase = 0.0f;
    float fundamentalFreq = 110.0f; // Robotic pilot fundamental pitch

    for (int i = 0; i < sampleCount; ++i) {
        float t = static_cast<float>(i) / sampleCount;

        // Robotic pitch modulation
        float pitchMod = std::sin(t * 18.0f) * 12.0f;
        float currentFreq = fundamentalFreq + pitchMod;
        phase += 2.0f * 3.14159f * currentFreq / sampleRate;

        // Glottal excitation pulse (sawtooth with rapid falloff)
        float glottal = (std::fmod(phase, 2.0f * 3.14159f) / (2.0f * 3.14159f)) - 0.5f;

        // Radio static & friction noise
        float noise = (static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f) * 0.15f;
        float excitation = glottal + noise;

        // Formant filtering
        float vocal = f1.Process(excitation) * 1.4f + f2.Process(excitation) * 0.8f;

        // Radio transmission squelch envelope
        float env = 1.0f;
        if (t < 0.08f) env = t / 0.08f;
        if (t > 0.85f) env = (1.0f - t) / 0.15f;

        // Slight distortion
        float sample = std::tanh(vocal * 2.2f) * env;
        buffer[i] = static_cast<int16_t>(std::clamp(sample, -1.0f, 1.0f) * 26000.0f);
    }

    Wave wave{};
    wave.frameCount = sampleCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = std::malloc(sampleCount * sizeof(int16_t));
    std::memcpy(wave.data, buffer.data(), sampleCount * sizeof(int16_t));
    return wave;
}

void VoiceSynthesizer::Initialize() {
    auto& cfg = ConfigManager::Get();
    if (!cfg.masterAudioEnabled || !IsAudioDeviceReady()) return;

    // Synthesize procedural voice clips into memory
    auto LoadAlert = [](TacticalAlert alert, const std::string& phonetic) {
        Wave w = SynthesizePhrase(phonetic);
        Sound s = LoadSoundFromWave(w);
        UnloadWave(w);
        s_alertSounds[alert] = s;
    };

    LoadAlert(TacticalAlert::REACTOR_CRITICAL, "REACTOR CRITICAL");
    LoadAlert(TacticalAlert::SHIELD_BREACHED, "SHIELDS OFFLINE");
    LoadAlert(TacticalAlert::TITAN_DETECTED, "HOSTILE TITAN DETECTED");
    LoadAlert(TacticalAlert::EXTRACTION_READY, "EXTRACTION VECTOR OPEN");
    LoadAlert(TacticalAlert::VOID_STORM_IMMINENT, "WARNING VOID STORM");
    LoadAlert(TacticalAlert::HEAT_PURGE_OFFLINE, "PURGE SYSTEM READY");
    LoadAlert(TacticalAlert::NANITES_HARVESTED, "NANITES SECURED");

    s_initialized = true;
}

void VoiceSynthesizer::Shutdown() {
    if (!s_initialized) return;
    for (auto& [alert, sound] : s_alertSounds) {
        if (sound.frameCount > 0) {
            UnloadSound(sound);
        }
    }
    s_alertSounds.clear();
    s_initialized = false;
}

void VoiceSynthesizer::Speak(TacticalAlert alert, float volume) {
    auto& cfg = ConfigManager::Get();
    if (!s_initialized || !cfg.masterAudioEnabled) return;

    auto it = s_alertSounds.find(alert);
    if (it != s_alertSounds.end() && it->second.frameCount > 0) {
        SetSoundVolume(it->second, cfg.sfxVolume * volume);
        PlaySound(it->second);
    }
}

} // namespace Voidfall::Audio
