#pragma once

#include <cstdint>

namespace Voidfall {

class Random {
public:
    static void Seed(uint64_t seedValue);
    static uint64_t NextU64();
    static float RangeF(float min, float max);
    static int RangeI(int min, int max); // inclusive
    static bool Bool(float probability = 0.5f);
    static float Normal(); // [0.0f, 1.0f)
};

} // namespace Voidfall
