#include "engine/PostProcessing.h"
#include <string>

namespace Voidfall {

RenderTexture2D PostProcessing::s_target{};
Shader PostProcessing::s_postShader{};
bool PostProcessing::s_initialized = false;
bool PostProcessing::s_crtEnabled = true;
int PostProcessing::s_aberrationLoc = -1;
int PostProcessing::s_timeLoc = -1;

static const char* s_postShaderFs = R"(
#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float hitAberration;
uniform float time;

void main() {
    vec2 uv = fragTexCoord;

    // Chromatic aberration on impact
    float r = texture(texture0, uv + vec2(hitAberration * 0.006, 0.0)).r;
    float g = texture(texture0, uv).g;
    float b = texture(texture0, uv - vec2(hitAberration * 0.006, 0.0)).b;
    float a = texture(texture0, uv).a;

    vec3 col = vec3(r, g, b);

    // Subtle scanlines
    float scanline = sin(uv.y * 720.0 * 3.14159) * 0.04;
    col -= scanline;

    // Subtle vignette
    vec2 d = abs(uv - 0.5) * 1.4;
    float vig = clamp(1.0 - dot(d, d), 0.0, 1.0);
    col *= vig;

    finalColor = vec4(col, a);
}
)";

void PostProcessing::Initialize(int width, int height) {
    if (s_initialized) return;

    s_target = LoadRenderTexture(width, height);
    s_postShader = LoadShaderFromMemory(nullptr, s_postShaderFs);

    s_aberrationLoc = GetShaderLocation(s_postShader, "hitAberration");
    s_timeLoc = GetShaderLocation(s_postShader, "time");

    s_initialized = true;
}

void PostProcessing::Shutdown() {
    if (!s_initialized) return;
    UnloadShader(s_postShader);
    UnloadRenderTexture(s_target);
    s_initialized = false;
}

void PostProcessing::Begin() {
    if (!s_initialized) return;
    BeginTextureMode(s_target);
}

void PostProcessing::End() {
    if (!s_initialized) return;
    EndTextureMode();
}

void PostProcessing::Draw(float hitAberration) {
    if (!s_initialized) return;

    static float shaderTime = 0.0f;
    shaderTime += GetFrameTime();

    if (s_aberrationLoc != -1) {
        SetShaderValue(s_postShader, s_aberrationLoc, &hitAberration, SHADER_UNIFORM_FLOAT);
    }
    if (s_timeLoc != -1) {
        SetShaderValue(s_postShader, s_timeLoc, &shaderTime, SHADER_UNIFORM_FLOAT);
    }

    if (s_crtEnabled && s_postShader.id > 0) {
        BeginShaderMode(s_postShader);
    }

    // Draw render texture (flipped vertically due to OpenGL texture coordinates)
    Rectangle srcRec = {0.0f, 0.0f, static_cast<float>(s_target.texture.width), -static_cast<float>(s_target.texture.height)};
    Rectangle dstRec = {0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
    DrawTexturePro(s_target.texture, srcRec, dstRec, {0.0f, 0.0f}, 0.0f, WHITE);

    if (s_crtEnabled && s_postShader.id > 0) {
        EndShaderMode();
    }
}

} // namespace Voidfall
