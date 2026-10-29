#pragma once

#include <vector>
#include <raylib.h>
#include "core/Types.h"

namespace Voidfall {

enum class VisionFilterMode {
    STANDARD,
    THERMAL_IR,
    NIGHT_VISION_NVG,
    TACTICAL_WIREFRAME
};

struct PostProcessSettings {
    bool enableCRTScanlines = true;
    float scanlineIntensity = 0.22f;
    int scanlineSpacing = 3;

    bool enableChromaticAberration = true;
    float aberrationDistance = 2.5f;

    bool enableBloom = true;
    float bloomThreshold = 0.75f;
    float bloomIntensity = 0.45f;

    bool enableFilmGrain = true;
    float grainIntensity = 0.08f;

    VisionFilterMode visionMode = VisionFilterMode::STANDARD;
};

class ShaderPipeline {
public:
    ShaderPipeline();
    ~ShaderPipeline() = default;

    void Initialize(int width, int height);
    void Resize(int width, int height);
    void Update(float dt);

    // Apply software filter directly to a 32-bit RGBA pixel buffer
    void ApplyToBuffer(uint32_t* colorBuffer, int width, int height);

    // Render screen-space overlays using Raylib drawing
    void RenderPostProcessingOverlay(int screenWidth, int screenHeight);

    PostProcessSettings& GetSettings() { return m_settings; }
    const PostProcessSettings& GetSettings() const { return m_settings; }

    void CycleVisionMode();
    void SetVisionMode(VisionFilterMode mode) { m_settings.visionMode = mode; }
    void TriggerAberrationPulse(float strength = 8.0f);

private:
    PostProcessSettings m_settings;
    int m_width = 1280;
    int m_height = 720;
    float m_pulseAberration = 0.0f;
    float m_animTime = 0.0f;

    // Fast integer thermal color LUT
    uint32_t m_thermalLUT[256];
    uint32_t m_nvgLUT[256];
    void BuildLookupTables();
};

} // namespace Voidfall
