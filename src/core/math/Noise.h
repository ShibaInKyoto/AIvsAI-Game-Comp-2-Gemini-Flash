#pragma once

#include <cstdint>
#include <array>

namespace Voidfall::Math {

class NoiseGenerator {
public:
    explicit NoiseGenerator(uint64_t seed = 1337);

    void Reseed(uint64_t seed);

    // 1D, 2D, 3D Perlin Noise (-1.0f to 1.0f)
    [[nodiscard]] float Perlin1D(float x) const noexcept;
    [[nodiscard]] float Perlin2D(float x, float y) const noexcept;
    [[nodiscard]] float Perlin3D(float x, float y, float z) const noexcept;

    // Fractional Brownian Motion (Multi-octave noise)
    [[nodiscard]] float FBm2D(float x, float y, int octaves = 4, float lacunarity = 2.0f, float gain = 0.5f) const noexcept;
    [[nodiscard]] float Turbulence2D(float x, float y, int octaves = 4) const noexcept;

    // Cellular / Worley noise (returns distance to closest seed point)
    [[nodiscard]] float Worley2D(float x, float y) const noexcept;

private:
    [[nodiscard]] static float Fade(float t) noexcept;
    [[nodiscard]] static float Grad1D(int hash, float x) noexcept;
    [[nodiscard]] static float Grad2D(int hash, float x, float y) noexcept;
    [[nodiscard]] static float Grad3D(int hash, float x, float y, float z) noexcept;

    std::array<uint8_t, 512> m_p{};
};

} // namespace Voidfall::Math
