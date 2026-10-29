#pragma once

#include <string>

namespace Voidfall {

struct GameConfig {
    int screenWidth{1280};
    int screenHeight{720};
    int targetFPS{60};
    bool fullscreen{false};
    bool masterAudioEnabled{true};
    float sfxVolume{0.8f};
    float musicVolume{0.6f};
    bool screenShakeEnabled{true};
    bool damageNumbersEnabled{true};

    // Headless / CI Autotest flags
    bool autotestMode{false};
    int autotestFrames{0};
    std::string screenshotPath{""};
};

class ConfigManager {
public:
    static GameConfig& Get() {
        static GameConfig instance;
        return instance;
    }

    static void ParseCommandLine(int argc, char** argv);
};

} // namespace Voidfall
