#include "engine/AudioSynth.h"
#include "core/Config.h"
#include "core/Random.h"
#include <cmath>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstring>

namespace Voidfall {

std::vector<Sound> AudioSynth::s_sounds;
bool AudioSynth::s_initialized = false;

Wave AudioSynth::GenerateLaser(float duration, float startFreq, float endFreq) {
    int sampleRate = 44100;
    int sampleCount = static_cast<int>(duration * sampleRate);
    std::vector<int16_t> buffer(sampleCount);

    float phase = 0.0f;
    for (int i = 0; i < sampleCount; ++i) {
        float t = static_cast<float>(i) / sampleCount;
        float freq = startFreq + (endFreq - startFreq) * (t * t);
        phase += 2.0f * 3.14159265f * freq / sampleRate;

        // Exponential decay envelope
        float env = (1.0f - t) * std::exp(-t * 3.5f);
        // Saturated sine with mild harmonic warmth
        float wave = std::sin(phase) + 0.3f * std::sin(phase * 2.0f);
        float sample = std::clamp(wave * env, -1.0f, 1.0f);
        buffer[i] = static_cast<int16_t>(sample * 30000.0f);
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

Wave AudioSynth::GenerateExplosion(float duration, float lowpass) {
    int sampleRate = 44100;
    int sampleCount = static_cast<int>(duration * sampleRate);
    std::vector<int16_t> buffer(sampleCount);

    float prevSample = 0.0f;
    for (int i = 0; i < sampleCount; ++i) {
        float t = static_cast<float>(i) / sampleCount;
        float whiteNoise = (Random::RangeF(-1.0f, 1.0f));

        // One-pole lowpass filter for deep concussive rumble
        prevSample = prevSample + lowpass * (whiteNoise - prevSample);

        float env = std::pow(1.0f - t, 2.5f);
        float subBass = std::sin(2.0f * 3.14159f * (65.0f - 30.0f * t) * i / sampleRate) * 0.45f;
        float sample = std::clamp((prevSample * 0.7f + subBass) * env, -1.0f, 1.0f);
        buffer[i] = static_cast<int16_t>(sample * 31000.0f);
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

Wave AudioSynth::GenerateChime(float duration, float freq) {
    int sampleRate = 44100;
    int sampleCount = static_cast<int>(duration * sampleRate);
    std::vector<int16_t> buffer(sampleCount);

    for (int i = 0; i < sampleCount; ++i) {
        float t = static_cast<float>(i) / sampleCount;
        float env = std::exp(-t * 6.0f);
        float wave = std::sin(2.0f * 3.14159f * freq * i / sampleRate) * 0.7f +
                     std::sin(2.0f * 3.14159f * (freq * 2.01f) * i / sampleRate) * 0.3f;
        float sample = std::clamp(wave * env, -1.0f, 1.0f);
        buffer[i] = static_cast<int16_t>(sample * 28000.0f);
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

Wave AudioSynth::GenerateNoiseBurst(float duration) {
    int sampleRate = 44100;
    int sampleCount = static_cast<int>(duration * sampleRate);
    std::vector<int16_t> buffer(sampleCount);

    for (int i = 0; i < sampleCount; ++i) {
        float t = static_cast<float>(i) / sampleCount;
        float noise = Random::RangeF(-1.0f, 1.0f);
        float env = (1.0f - t) * (1.0f - t);
        buffer[i] = static_cast<int16_t>(noise * env * 25000.0f);
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

void AudioSynth::Initialize() {
    auto& cfg = ConfigManager::Get();
    if (!cfg.masterAudioEnabled) return;

    InitAudioDevice();
    if (!IsAudioDeviceReady()) return;

    s_sounds.resize(static_cast<size_t>(SoundID::COUNT));

    // Synthesize all sound effects in memory
    Wave wVulcan = GenerateLaser(0.12f, 850.0f, 180.0f);
    s_sounds[static_cast<size_t>(SoundID::SHOOT_VULCAN)] = LoadSoundFromWave(wVulcan);
    UnloadWave(wVulcan);

    Wave wLance = GenerateLaser(0.35f, 1400.0f, 600.0f);
    s_sounds[static_cast<size_t>(SoundID::SHOOT_LANCE)] = LoadSoundFromWave(wLance);
    UnloadWave(wLance);

    Wave wFlak = GenerateExplosion(0.25f, 0.4f);
    s_sounds[static_cast<size_t>(SoundID::SHOOT_FLAK)] = LoadSoundFromWave(wFlak);
    UnloadWave(wFlak);

    Wave wMissile = GenerateLaser(0.3f, 320.0f, 750.0f);
    s_sounds[static_cast<size_t>(SoundID::SHOOT_MISSILE)] = LoadSoundFromWave(wMissile);
    UnloadWave(wMissile);

    Wave wExpSmall = GenerateExplosion(0.4f, 0.25f);
    s_sounds[static_cast<size_t>(SoundID::EXPLOSION_SMALL)] = LoadSoundFromWave(wExpSmall);
    UnloadWave(wExpSmall);

    Wave wExpLarge = GenerateExplosion(0.85f, 0.12f);
    s_sounds[static_cast<size_t>(SoundID::EXPLOSION_LARGE)] = LoadSoundFromWave(wExpLarge);
    UnloadWave(wExpLarge);

    Wave wShield = GenerateChime(0.2f, 920.0f);
    s_sounds[static_cast<size_t>(SoundID::SHIELD_HIT)] = LoadSoundFromWave(wShield);
    UnloadWave(wShield);

    Wave wParry = GenerateChime(0.6f, 1480.0f);
    s_sounds[static_cast<size_t>(SoundID::SHIELD_PARRY)] = LoadSoundFromWave(wParry);
    UnloadWave(wParry);

    Wave wDash = GenerateNoiseBurst(0.22f);
    s_sounds[static_cast<size_t>(SoundID::DASH)] = LoadSoundFromWave(wDash);
    UnloadWave(wDash);

    Wave wPurge = GenerateLaser(0.45f, 900.0f, 120.0f);
    s_sounds[static_cast<size_t>(SoundID::HEAT_PURGE_SUCCESS)] = LoadSoundFromWave(wPurge);
    UnloadWave(wPurge);

    Wave wOverheat = GenerateChime(0.3f, 380.0f);
    s_sounds[static_cast<size_t>(SoundID::HEAT_OVERHEAT)] = LoadSoundFromWave(wOverheat);
    UnloadWave(wOverheat);

    Wave wPower = GenerateChime(0.18f, 620.0f);
    s_sounds[static_cast<size_t>(SoundID::POWER_SHIFT)] = LoadSoundFromWave(wPower);
    UnloadWave(wPower);

    Wave wUpgrade = GenerateChime(0.5f, 1046.5f); // High C chime
    s_sounds[static_cast<size_t>(SoundID::UPGRADE_SELECT)] = LoadSoundFromWave(wUpgrade);
    UnloadWave(wUpgrade);

    Wave wBoss = GenerateExplosion(1.2f, 0.08f);
    s_sounds[static_cast<size_t>(SoundID::BOSS_WARNING)] = LoadSoundFromWave(wBoss);
    UnloadWave(wBoss);

    s_initialized = true;
}

void AudioSynth::Shutdown() {
    if (!s_initialized) return;
    for (auto& sound : s_sounds) {
        if (sound.frameCount > 0) {
            UnloadSound(sound);
        }
    }
    s_sounds.clear();
    CloseAudioDevice();
    s_initialized = false;
}

void AudioSynth::Play(SoundID id, float volumeMult, float pitch) {
    auto& cfg = ConfigManager::Get();
    if (!s_initialized || !cfg.masterAudioEnabled) return;

    size_t idx = static_cast<size_t>(id);
    if (idx < s_sounds.size() && s_sounds[idx].frameCount > 0) {
        SetSoundVolume(s_sounds[idx], cfg.sfxVolume * volumeMult);
        SetSoundPitch(s_sounds[idx], pitch);
        PlaySound(s_sounds[idx]);
    }
}

void AudioSynth::UpdateMusic(float dt) {
    (void)dt;
    // Music can be expanded with procedural stream synthesizer
}

} // namespace Voidfall
