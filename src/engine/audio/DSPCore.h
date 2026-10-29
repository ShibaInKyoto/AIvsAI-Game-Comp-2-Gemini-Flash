#pragma once

#include <vector>
#include <cmath>
#include <cstdint>

namespace Voidfall::Audio {

enum class FilterType {
    LOWPASS,
    HIGHPASS,
    BANDPASS,
    NOTCH,
    PEAKING
};

class BiquadFilter {
public:
    BiquadFilter();

    void Configure(FilterType type, float cutoffHz, float q, float sampleRate = 44100.0f);
    [[nodiscard]] float Process(float input) noexcept;
    void Reset() noexcept;

private:
    float m_b0{1.0f}, m_b1{0.0f}, m_b2{0.0f};
    float m_a1{0.0f}, m_a2{0.0f};
    float m_x1{0.0f}, m_x2{0.0f};
    float m_y1{0.0f}, m_y2{0.0f};
};

class DelayLine {
public:
    explicit DelayLine(size_t maxDelaySamples = 44100);

    void SetDelay(float delaySeconds, float sampleRate = 44100.0f);
    void SetFeedback(float feedback) noexcept { m_feedback = feedback; }
    void SetWetMix(float wet) noexcept { m_wet = wet; }

    [[nodiscard]] float Process(float input) noexcept;
    void Clear() noexcept;

private:
    std::vector<float> m_buffer;
    size_t m_writePos{0};
    size_t m_delaySamples{4410};
    float m_feedback{0.35f};
    float m_wet{0.4f};
};

class Bitcrusher {
public:
    void SetBits(int bits) noexcept { m_bits = bits; }
    void SetDownsampleRate(int rate) noexcept { m_downsample = rate; }

    [[nodiscard]] float Process(float input) noexcept;

private:
    int m_bits{8};
    int m_downsample{2};
    int m_sampleCounter{0};
    float m_heldSample{0.0f};
};

class ReverbTank {
public:
    ReverbTank();

    void SetRoomSize(float size);
    void SetDamping(float damp);
    void SetWet(float wet) noexcept { m_wet = wet; }

    [[nodiscard]] float Process(float input) noexcept;
    void Clear() noexcept;

private:
    std::vector<DelayLine> m_combs;
    std::vector<DelayLine> m_allpasses;
    float m_wet{0.25f};
};

} // namespace Voidfall::Audio
