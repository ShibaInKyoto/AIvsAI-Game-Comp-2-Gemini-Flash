#include "ShaderPipeline.h"
#include <cmath>
#include <algorithm>

namespace Voidfall {

static inline uint32_t PackRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) {
    return (static_cast<uint32_t>(a) << 24) |
           (static_cast<uint32_t>(b) << 16) |
           (static_cast<uint32_t>(g) << 8)  |
           (static_cast<uint32_t>(r));
}

ShaderPipeline::ShaderPipeline() {
    BuildLookupTables();
}

void ShaderPipeline::Initialize(int width, int height) {
    m_width = width;
    m_height = height;
    m_pulseAberration = 0.0f;
    m_animTime = 0.0f;
}

void ShaderPipeline::Resize(int width, int height) {
    m_width = width;
    m_height = height;
}

void ShaderPipeline::BuildLookupTables() {
    // 1. Thermal IR Palette (Black -> Dark Blue -> Purple -> Orange -> Yellow -> White)
    for (int i = 0; i < 256; ++i) {
        float t = static_cast<float>(i) / 255.0f;
        unsigned char r = 0, g = 0, b = 0;

        if (t < 0.25f) {
            float f = t / 0.25f;
            b = static_cast<unsigned char>(f * 200.0f);
        } else if (t < 0.5f) {
            float f = (t - 0.25f) / 0.25f;
            r = static_cast<unsigned char>(f * 220.0f);
            b = static_cast<unsigned char>((1.0f - f * 0.5f) * 200.0f);
        } else if (t < 0.75f) {
            float f = (t - 0.5f) / 0.25f;
            r = 240;
            g = static_cast<unsigned char>(f * 200.0f);
            b = static_cast<unsigned char>((1.0f - f) * 100.0f);
        } else {
            float f = (t - 0.75f) / 0.25f;
            r = 255;
            g = 255;
            b = static_cast<unsigned char>(f * 255.0f);
        }
        m_thermalLUT[i] = PackRGBA(r, g, b);
    }

    // 2. Night Vision Phosphor Green LUT
    for (int i = 0; i < 256; ++i) {
        float t = static_cast<float>(i) / 255.0f;
        unsigned char g = static_cast<unsigned char>(std::min(255.0f, t * 1.5f * 255.0f));
        unsigned char r = static_cast<unsigned char>(t * 40.0f);
        unsigned char b = static_cast<unsigned char>(t * 40.0f);
        m_nvgLUT[i] = PackRGBA(r, g, b);
    }
}

void ShaderPipeline::Update(float dt) {
    m_animTime += dt;
    if (m_pulseAberration > 0.0f) {
        m_pulseAberration = std::max(0.0f, m_pulseAberration - dt * 10.0f);
    }
}

void ShaderPipeline::TriggerAberrationPulse(float strength) {
    m_pulseAberration = strength;
}

void ShaderPipeline::CycleVisionMode() {
    int cur = static_cast<int>(m_settings.visionMode);
    m_settings.visionMode = static_cast<VisionFilterMode>((cur + 1) % 4);
}

void ShaderPipeline::ApplyToBuffer(uint32_t* colorBuffer, int width, int height) {
    if (!colorBuffer || width <= 0 || height <= 0) return;

    const int totalPixels = width * height;

    // Apply Vision Mode Palette re-mapping if non-standard
    if (m_settings.visionMode == VisionFilterMode::THERMAL_IR) {
        for (int i = 0; i < totalPixels; ++i) {
            uint32_t p = colorBuffer[i];
            unsigned char r = (p) & 0xFF;
            unsigned char g = (p >> 8) & 0xFF;
            unsigned char b = (p >> 16) & 0xFF;
            int lum = (r * 77 + g * 150 + b * 29) >> 8;
            colorBuffer[i] = m_thermalLUT[lum];
        }
    } else if (m_settings.visionMode == VisionFilterMode::NIGHT_VISION_NVG) {
        for (int i = 0; i < totalPixels; ++i) {
            uint32_t p = colorBuffer[i];
            unsigned char r = (p) & 0xFF;
            unsigned char g = (p >> 8) & 0xFF;
            unsigned char b = (p >> 16) & 0xFF;
            int lum = (r * 77 + g * 150 + b * 29) >> 8;
            colorBuffer[i] = m_nvgLUT[lum];
        }
    }
}

void ShaderPipeline::RenderPostProcessingOverlay(int screenWidth, int screenHeight) {
    // 1. CRT Scanlines
    if (m_settings.enableCRTScanlines) {
        int spacing = std::max(2, m_settings.scanlineSpacing);
        unsigned char alpha = static_cast<unsigned char>(m_settings.scanlineIntensity * 255.0f);
        Color scanColor = { 0, 0, 0, alpha };

        for (int y = 0; y < screenHeight; y += spacing) {
            DrawRectangle(0, y, screenWidth, 1, scanColor);
        }
    }

    // 2. Chromatic aberration vignette border / pulse
    float totalAberration = m_settings.aberrationDistance + m_pulseAberration;
    if (m_settings.enableChromaticAberration && totalAberration > 0.1f) {
        int offset = static_cast<int>(totalAberration);
        // Red fringe
        DrawRectangleLines(offset, offset, screenWidth - offset * 2, screenHeight - offset * 2,
                           Color{ 255, 30, 40, static_cast<unsigned char>(std::min(120.0f, totalAberration * 15.0f)) });
        // Blue fringe
        DrawRectangleLines(offset * 2, offset * 2, screenWidth - offset * 4, screenHeight - offset * 4,
                           Color{ 40, 120, 255, static_cast<unsigned char>(std::min(90.0f, totalAberration * 12.0f)) });
    }

    // 3. Vision Mode HUD Watermark
    if (m_settings.visionMode != VisionFilterMode::STANDARD) {
        const char* modeLabel = "MODE: THERMAL IR [320-750nm]";
        Color tagCol = Color{ 255, 140, 40, 220 };
        if (m_settings.visionMode == VisionFilterMode::NIGHT_VISION_NVG) {
            modeLabel = "MODE: MIL-NVG GEN-IV [PHOSPHOR GREEN]";
            tagCol = Color{ 50, 255, 100, 220 };
        } else if (m_settings.visionMode == VisionFilterMode::TACTICAL_WIREFRAME) {
            modeLabel = "MODE: TACTICAL GEOMETRIC SCAN";
            tagCol = Color{ 70, 200, 255, 220 };
        }

        DrawText(modeLabel, screenWidth - MeasureText(modeLabel, 13) - 24, 20, 13, tagCol);
        DrawRectangleLines(15, 15, screenWidth - 30, screenHeight - 30, Color{ tagCol.r, tagCol.g, tagCol.b, 60 });
    }
}

} // namespace Voidfall
