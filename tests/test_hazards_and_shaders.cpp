#include <cassert>
#include <iostream>
#include "gameplay/environment/HazardEngine.h"
#include "engine/render/ShaderPipeline.h"

namespace Voidfall {

void RunHazardsAndShaderTests() {
    std::cout << "[TEST] Running Environmental Hazards & Shader Tests..." << std::endl;

    // 1. Hazard Engine Tests
    HazardEngine hazards;
    hazards.Initialize(HazardType::ASTEROID_FIELD, 1280, 720);
    assert(hazards.GetAsteroids().size() >= 10);

    // Damage and splinter test
    int testAstId = hazards.GetAsteroids().front().id;
    std::vector<AsteroidEntity> splinters;
    hazards.DamageAsteroid(testAstId, 500.0f, splinters);
    // Large asteroid destroyed and spawned 2 shards
    assert(splinters.size() == 2);

    // Singularity Gravitational Pull Test
    hazards.Initialize(HazardType::GRAVITATIONAL_SINGULARITY, 1280, 720);
    assert(!hazards.GetSingularities().empty());

    Vec2 pPos(640, 300); // Above singularity center
    Vec2 pVel(0, 0);
    float heat = 0.0f;
    float shields = 100.0f;
    float hp = 100.0f;

    hazards.Update(0.1f, pPos, pVel, heat, shields, hp);
    // Velocity should have pulled downwards towards center (640, 360)
    assert(pVel.y > 0.0f);

    // 2. Shader Pipeline Tests
    ShaderPipeline shaders;
    shaders.Initialize(800, 600);

    assert(shaders.GetSettings().enableCRTScanlines);
    assert(shaders.GetSettings().visionMode == VisionFilterMode::STANDARD);

    shaders.CycleVisionMode();
    assert(shaders.GetSettings().visionMode == VisionFilterMode::THERMAL_IR);

    // Test buffer processing with Thermal palette
    std::vector<uint32_t> testPixels(100, 0xFF808080);
    shaders.ApplyToBuffer(testPixels.data(), 10, 10);
    // Pixels should be converted through thermal LUT
    assert(testPixels[0] != 0xFF808080);

    shaders.CycleVisionMode();
    assert(shaders.GetSettings().visionMode == VisionFilterMode::NIGHT_VISION_NVG);

    shaders.TriggerAberrationPulse(12.0f);
    shaders.Update(0.016f);

    std::cout << "[PASS] Environmental Hazards & Shader Tests Passed!" << std::endl;
}

} // namespace Voidfall
