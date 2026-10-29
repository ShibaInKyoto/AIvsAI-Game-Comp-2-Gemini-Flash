#include "core/Config.h"
#include <cstring>
#include <cstdlib>

namespace Voidfall {

void ConfigManager::ParseCommandLine(int argc, char** argv) {
    auto& cfg = Get();
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--no-audio") == 0) {
            cfg.masterAudioEnabled = false;
        } else if (std::strcmp(argv[i], "--windowed") == 0) {
            cfg.fullscreen = false;
        } else if (std::strcmp(argv[i], "--autotest") == 0 && i + 1 < argc) {
            cfg.autotestMode = true;
            cfg.autotestFrames = std::atoi(argv[++i]);
        } else if (std::strcmp(argv[i], "--screenshot") == 0 && i + 1 < argc) {
            cfg.screenshotPath = argv[++i];
        } else if (std::strcmp(argv[i], "--size") == 0 && i + 1 < argc) {
            std::string sizeStr = argv[++i];
            size_t xPos = sizeStr.find('x');
            if (xPos != std::string::npos) {
                cfg.screenWidth = std::atoi(sizeStr.substr(0, xPos).c_str());
                cfg.screenHeight = std::atoi(sizeStr.substr(xPos + 1).c_str());
            }
        }
    }
}

} // namespace Voidfall
