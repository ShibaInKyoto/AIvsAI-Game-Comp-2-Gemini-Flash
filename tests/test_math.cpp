#include "core/Types.h"
#include <cassert>
#include <iostream>
#include <cmath>

using namespace Voidfall;

void RunMathTests() {
    std::cout << "[TEST] Running Vector2 & Math Tests..." << std::endl;

    // Vector addition and subtraction
    Vec2 v1(3.0f, 4.0f);
    Vec2 v2(1.0f, 2.0f);
    Vec2 vSum = v1 + v2;
    assert(vSum.x == 4.0f && vSum.y == 6.0f);

    Vec2 vDiff = v1 - v2;
    assert(vDiff.x == 2.0f && vDiff.y == 2.0f);

    // Length and Normalization
    assert(std::abs(v1.Length() - 5.0f) < 0.0001f);
    Vec2 vNorm = v1.Normalized();
    assert(std::abs(vNorm.Length() - 1.0f) < 0.0001f);

    // Dot product
    Vec2 right(1.0f, 0.0f);
    Vec2 up(0.0f, 1.0f);
    assert(right.Dot(up) == 0.0f);
    assert(right.Dot(right) == 1.0f);

    // Rotation
    Vec2 rotated = right.Rotated(1.5707963f); // 90 degrees
    assert(std::abs(rotated.x) < 0.001f);
    assert(std::abs(rotated.y - 1.0f) < 0.001f);

    std::cout << " -> Vector2 & Math Tests Passed!" << std::endl;
}
