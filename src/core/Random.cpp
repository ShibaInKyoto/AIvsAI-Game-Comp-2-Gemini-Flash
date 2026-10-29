#include "core/Random.h"

namespace Voidfall {

namespace {
// xoshiro256** implementation
uint64_t s[4] = {
    0x8a5cd789635d2dffULL,
    0x121fd2abc5cffd91ULL,
    0xb6acbe586762808aULL,
    0x9c3779b97f4a7c15ULL
};

static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

uint64_t splitmix64(uint64_t& x) {
    uint64_t z = (x += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}
} // namespace

void Random::Seed(uint64_t seedValue) {
    uint64_t smState = seedValue;
    s[0] = splitmix64(smState);
    s[1] = splitmix64(smState);
    s[2] = splitmix64(smState);
    s[3] = splitmix64(smState);
}

uint64_t Random::NextU64() {
    const uint64_t result = rotl(s[1] * 5, 7) * 9;
    const uint64_t t = s[1] << 17;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;
    s[3] = rotl(s[3], 45);

    return result;
}

float Random::Normal() {
    // 53 bits of precision in [0, 1)
    return static_cast<float>((NextU64() >> 11) * (1.0 / 9007199254740992.0));
}

float Random::RangeF(float min, float max) {
    if (min >= max) return min;
    return min + Normal() * (max - min);
}

int Random::RangeI(int min, int max) {
    if (min >= max) return min;
    uint64_t range = static_cast<uint64_t>(max - min + 1);
    return min + static_cast<int>(NextU64() % range);
}

bool Random::Bool(float probability) {
    return Normal() < probability;
}

} // namespace Voidfall
