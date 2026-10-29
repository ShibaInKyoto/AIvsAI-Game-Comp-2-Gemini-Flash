#pragma once

#include <raylib.h>

namespace Voidfall {

class PostProcessing {
public:
    static void Initialize(int width, int height);
    static void Shutdown();

    static void Begin();
    static void End();
    static void Draw(float hitAberration = 0.0f);

    static void SetCrtEnabled(bool enabled) { s_crtEnabled = enabled; }
    static bool IsCrtEnabled() { return s_crtEnabled; }

private:
    static RenderTexture2D s_target;
    static Shader s_postShader;
    static bool s_initialized;
    static bool s_crtEnabled;

    static int s_aberrationLoc;
    static int s_timeLoc;
};

} // namespace Voidfall
