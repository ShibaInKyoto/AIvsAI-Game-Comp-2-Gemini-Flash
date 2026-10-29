#include "engine/audio/DSPCore.h"
#include <algorithm>

namespace Voidfall::Audio {

constexpr float PI = 3.14159265358979323846f;

BiquadFilter::BiquadFilter() {
    Reset();
}

void BiquadFilter::Reset() noexcept {
    m_x1 = m_x2 = m_y1 = m_y2 = 0.0f;
}

void BiquadFilter::Configure(FilterType type, float cutoffHz, float q, float sampleRate) {
    cutoffHz = std::clamp(cutoffHz, 20.0f, sampleRate * 0.49f);
    q = std::max(q, 0.1f);

    float w0 = 2.0f * PI * cutoffHz / sampleRate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2.0f * q);

    float a0 = 1.0f;

    switch (type) {
        case FilterType::LOWPASS: {
            m_b0 = (1.0f - cosw0) * 0.5f;
            m_b1 = 1.0f - cosw0;
            m_b2 = (1.0f - cosw0) * 0.5f;
            a0 = 1.0f + alpha;
            m_a1 = -2.0f * cosw0;
            m_a2 = 1.0f - alpha;
            break;
        }
        case FilterType::HIGHPASS: {
            m_b0 = (1.0f + cosw0) * 0.5f;
            m_b1 = -(1.0f + cosw0);
            m_b2 = (1.0f + cosw0) * 0.5f;
            a0 = 1.0f + alpha;
            m_a1 = -2.0f * cosw0;
            m_a2 = 1.0f - alpha;
            break;
        }
        case FilterType::BANDPASS: {
            m_b0 = alpha;
            m_b1 = 0.0f;
            m_b2 = -alpha;
            a0 = 1.0f + alpha;
            m_a1 = -2.0f * cosw0;
            m_a2 = 1.0f - alpha;
            break;
        }
        case FilterType::NOTCH: {
            m_b0 = 1.0f;
            m_b1 = -2.0f * cosw0;
            m_b2 = 1.0f;
            a0 = 1.0f + alpha;
            m_a1 = -2.0f * cosw0;
            m_a2 = 1.0f - alpha;
            break;
        }
        case FilterType::PEAKING: {
            m_b0 = 1.0f + alpha;
            m_b1 = -2.0f * cosw0;
            m_b2 = 1.0f - alpha;
            a0 = 1.0f + alpha;
            m_a1 = -2.0f * cosw0;
            m_a2 = 1.0f - alpha;
            break;
        }
    }

    // Normalize coefficients
    m_b0 /= a0;
    m_b1 /= a0;
    m_b2 /= a0;
    m_a1 /= a0;
    m_a2 /= a0;
}

float BiquadFilter::Process(float input) noexcept {
    float output = m_b0 * input + m_b1 * m_x1 + m_b2 * m_x2 - m_a1 * m_y1 - m_a2 * m_y2;
    m_x2 = m_x1;
    m_x1 = input;
    m_y2 = m_y1;
    m_y1 = output;
    return output;
}

DelayLine::DelayLine(size_t maxDelaySamples) {
    m_buffer.resize(maxDelaySamples, 0.0f);
}

void DelayLine::SetDelay(float delaySeconds, float sampleRate) {
    m_delaySamples = std::clamp(static_cast<size_t>(delaySeconds * sampleRate), size_t{1}, m_buffer.size() - 1);
}

float DelayLine::Process(float input) noexcept {
    size_t readPos = (m_writePos + m_buffer.size() - m_delaySamples) % m_buffer.size();
    float delayedSample = m_buffer[readPos];

    m_buffer[m_writePos] = input + delayedSample * m_feedback;
    m_writePos = (m_writePos + 1) % m_buffer.size();

    return input * (1.0f - m_wet) + delayedSample * m_wet;
}

void DelayLine::Clear() noexcept {
    std::fill(m_buffer.begin(), m_buffer.end(), 0.0f);
    m_writePos = 0;
}

float Bitcrusher::Process(float input) noexcept {
    m_sampleCounter++;
    if (m_sampleCounter >= m_downsample) {
        m_sampleCounter = 0;
        float steps = std::pow(2.0f, static_cast<float>(m_bits));
        m_heldSample = std::round(input * steps) / steps;
    }
    return m_heldSample;
}

ReverbTank::ReverbTank() {
    // Schroeder reverb topology: 4 comb filters in parallel + 2 allpass filters in series
    m_combs.resize(4, DelayLine(88200));
    m_combs[0].SetDelay(0.0297f, 44100.0f); m_combs[0].SetFeedback(0.78f);
    m_combs[1].SetDelay(0.0371f, 44100.0f); m_combs[1].SetFeedback(0.76f);
    m_combs[2].SetDelay(0.0411f, 44100.0f); m_combs[2].SetFeedback(0.74f);
    m_combs[3].SetDelay(0.0437f, 44100.0f); m_combs[3].SetFeedback(0.72f);

    m_allpasses.resize(2, DelayLine(44100));
    m_allpasses[0].SetDelay(0.0050f, 44100.0f); m_allpasses[0].SetFeedback(0.6f);
    m_allpasses[1].SetDelay(0.0017f, 44100.0f); m_allpasses[1].SetFeedback(0.6f);
}

void ReverbTank::SetRoomSize(float size) {
    float fb = std::clamp(size, 0.2f, 0.95f);
    for (auto& c : m_combs) {
        c.SetFeedback(fb);
    }
}

void ReverbTank::SetDamping(float damp) {
    (void)damp;
}

float ReverbTank::Process(float input) noexcept {
    float combSum = 0.0f;
    for (auto& c : m_combs) {
        combSum += c.Process(input);
    }
    combSum *= 0.25f;

    float apOut = m_allpasses[0].Process(combSum);
    apOut = m_allpasses[1].Process(apOut);

    return input * (1.0f - m_wet) + apOut * m_wet;
}

void ReverbTank::Clear() noexcept {
    for (auto& c : m_combs) c.Clear();
    for (auto& a : m_allpasses) a.Clear();
}

} // namespace Voidfall::Audio
