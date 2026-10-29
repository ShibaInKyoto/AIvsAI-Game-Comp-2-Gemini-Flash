#pragma once

#include <raylib.h>
#include <vector>

namespace Voidfall {

enum class SoundID {
    SHOOT_VULCAN,
    SHOOT_LANCE,
    SHOOT_FLAK,
    SHOOT_MISSILE,
    SHOOT_DISRUPTOR,
    SHOOT_TORPEDO,
    EXPLOSION_SMALL,
    EXPLOSION_LARGE,
    EXPLOSION_CLUSTER,
    SHIELD_HIT,
    SHIELD_PARRY,
    DASH,
    HEAT_PURGE_SUCCESS,
    HEAT_OVERHEAT,
    POWER_SHIFT,
    UPGRADE_SELECT,
    PURCHASE_UPGRADE,
    BOSS_WARNING,
    WARNING_KLAXON,
    SQUELCH_DISPATCH,
    COUNT
};

class AudioSynth {
public:
    static void Initialize();
    static void Shutdown();

    static void Play(SoundID id, float volumeMult = 1.0f, float pitch = 1.0f);
    static void UpdateMusic(float dt);

private:
    static Wave GenerateLaser(float duration, float startFreq, float endFreq);
    static Wave GenerateExplosion(float duration, float lowpass);
    static Wave GenerateChime(float duration, float freq);
    static Wave GenerateNoiseBurst(float duration);

    static std::vector<Sound> s_sounds;
    static bool s_initialized;
};

} // namespace Voidfall
