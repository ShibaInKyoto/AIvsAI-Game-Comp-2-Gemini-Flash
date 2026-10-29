#include "core/math/Noise.h"
#include <cmath>
#include <numeric>
#include <algorithm>

namespace Voidfall::Math {

NoiseGenerator::NoiseGenerator(uint64_t seed) {
    Reseed(seed);
}

void NoiseGenerator::Reseed(uint64_t seed) {
    std::array<uint8_t, 256> permutation{};
    std::iota(permutation.begin(), permutation.end(), 0);

    // Deterministic linear congruential shuffle
    uint64_t state = seed != 0 ? seed : 1337;
    for (int i = 255; i > 0; --i) {
        state = state * 6364136223846793005ULL + 1442695040888963407ULL;
        int j = static_cast<int>((state >> 32) % (i + 1));
        std::swap(permutation[i], permutation[j]);
    }

    for (int i = 0; i < 256; ++i) {
        m_p[i] = permutation[i];
        m_p[256 + i] = permutation[i];
    }
}

float NoiseGenerator::Fade(float t) noexcept {
    // 6t^5 - 15t^4 + 10t^3 (Perlin's quintic polynomial)
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float NoiseGenerator::Grad1D(int hash, float x) noexcept {
    return (hash & 1) == 0 ? x : -x;
}

float NoiseGenerator::Grad2D(int hash, float x, float y) noexcept {
    switch (hash & 3) {
        case 0: return  x + y;
        case 1: return -x + y;
        case 2: return  x - y;
        case 3: return -x - y;
        default: return 0.0f;
    }
}

float NoiseGenerator::Grad3D(int hash, float x, float y, float z) noexcept {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float NoiseGenerator::Perlin1D(float x) const noexcept {
    int X = static_cast<int>(std::floor(x)) & 255;
    x -= std::floor(x);
    float u = Fade(x);
    return (1.0f - u) * Grad1D(m_p[X], x) + u * Grad1D(m_p[X + 1], x - 1.0f);
}

float NoiseGenerator::Perlin2D(float x, float y) const noexcept {
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;

    x -= std::floor(x);
    y -= std::floor(y);

    float u = Fade(x);
    float v = Fade(y);

    int A = m_p[X] + Y;
    int B = m_p[X + 1] + Y;

    float g00 = Grad2D(m_p[A], x, y);
    float g10 = Grad2D(m_p[B], x - 1.0f, y);
    float g01 = Grad2D(m_p[A + 1], x, y - 1.0f);
    float g11 = Grad2D(m_p[B + 1], x - 1.0f, y - 1.0f);

    float x1 = (1.0f - u) * g00 + u * g10;
    float x2 = (1.0f - u) * g01 + u * g11;

    return (1.0f - v) * x1 + v * x2;
}

float NoiseGenerator::Perlin3D(float x, float y, float z) const noexcept {
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    int Z = static_cast<int>(std::floor(z)) & 255;

    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);

    float u = Fade(x);
    float v = Fade(y);
    float w = Fade(z);

    int A  = m_p[X] + Y;
    int AA = m_p[A] + Z;
    int AB = m_p[A + 1] + Z;
    int B  = m_p[X + 1] + Y;
    int BA = m_p[B] + Z;
    int BB = m_p[B + 1] + Z;

    float c000 = Grad3D(m_p[AA], x, y, z);
    float c100 = Grad3D(m_p[BA], x - 1.0f, y, z);
    float c010 = Grad3D(m_p[AB], x, y - 1.0f, z);
    float c110 = Grad3D(m_p[BB], x - 1.0f, y - 1.0f, z);
    float c001 = Grad3D(m_p[AA + 1], x, y, z - 1.0f);
    float c101 = Grad3D(m_p[BA + 1], x - 1.0f, y, z - 1.0f);
    float c011 = Grad3D(m_p[AB + 1], x, y - 1.0f, z - 1.0f);
    float c111 = Grad3D(m_p[BB + 1], x - 1.0f, y - 1.0f, z - 1.0f);

    float x00 = (1.0f - u) * c000 + u * c100;
    float x10 = (1.0f - u) * c010 + u * c110;
    float x01 = (1.0f - u) * c001 + u * c101;
    float x11 = (1.0f - u) * c011 + u * c111;

    float y0 = (1.0f - v) * x00 + v * x10;
    float y1 = (1.0f - v) * x01 + v * x11;

    return (1.0f - w) * y0 + w * y1;
}

float NoiseGenerator::FBm2D(float x, float y, int octaves, float lacunarity, float gain) const noexcept {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxAmp = 0.0f;

    for (int i = 0; i < octaves; ++i) {
        total += Perlin2D(x * frequency, y * frequency) * amplitude;
        maxAmp += amplitude;
        frequency *= lacunarity;
        amplitude *= gain;
    }

    return maxAmp > 0.0f ? total / maxAmp : 0.0f;
}

float NoiseGenerator::Turbulence2D(float x, float y, int octaves) const noexcept {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxAmp = 0.0f;

    for (int i = 0; i < octaves; ++i) {
        total += std::abs(Perlin2D(x * frequency, y * frequency)) * amplitude;
        maxAmp += amplitude;
        frequency *= 2.0f;
        amplitude *= 0.5f;
    }

    return maxAmp > 0.0f ? total / maxAmp : 0.0f;
}

float NoiseGenerator::Worley2D(float x, float y) const noexcept {
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));

    float minDist = 999.0f;

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int cx = xi + dx;
            int cy = yi + dy;

            // Deterministic hash point in cell
            int h = m_p[(m_p[cx & 255] + (cy & 255)) & 255];
            float px = static_cast<float>(cx) + (static_cast<float>(h) / 255.0f);
            float py = static_cast<float>(cy) + (static_cast<float>((h * 17) & 255) / 255.0f);

            float dist = std::sqrt((x - px) * (x - px) + (y - py) * (y - py));
            if (dist < minDist) {
                minDist = dist;
            }
        }
    }

    return minDist;
}

} // namespace Voidfall::Math
