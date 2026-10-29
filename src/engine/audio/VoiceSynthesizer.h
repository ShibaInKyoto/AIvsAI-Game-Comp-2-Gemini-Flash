#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace Voidfall::Audio {

enum class TacticalAlert {
    REACTOR_CRITICAL,
    SHIELD_BREACHED,
    TITAN_DETECTED,
    EXTRACTION_READY,
    VOID_STORM_IMMINENT,
    HEAT_PURGE_OFFLINE,
    NANITES_HARVESTED
};

class VoiceSynthesizer {
public:
    static void Initialize();
    static void Shutdown();

    static void Speak(TacticalAlert alert, float volume = 1.0f);

private:
    static Wave SynthesizePhrase(const std::string& phoneticPhrase);
    static std::unordered_map<TacticalAlert, Sound> s_alertSounds;
    static bool s_initialized;
};

} // namespace Voidfall::Audio
