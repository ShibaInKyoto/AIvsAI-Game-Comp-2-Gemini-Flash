#pragma once

#include "core/Types.h"

namespace Voidfall {

struct PowerAllocation {
    float weapons{0.33f};  // Damage & fire rate multiplier
    float shields{0.33f};  // Shield regeneration & max capacity
    float engines{0.34f};  // Speed, dash cooldown, heat dissipation
};

class Reactor {
public:
    Reactor();

    void Update(float dt);
    void SetProfile(PowerProfile profile);
    [[nodiscard]] PowerProfile GetProfile() const { return m_profile; }
    [[nodiscard]] const PowerAllocation& GetAllocation() const { return m_allocation; }

    // Heat Management
    void AddHeat(float amount);
    bool TryActivePurge(bool& outSweetSpot); // returns true if purge executed, outSweetSpot true if bonus

    [[nodiscard]] float GetHeat() const { return m_heat; }
    [[nodiscard]] float GetMaxHeat() const { return m_maxHeat; }
    [[nodiscard]] float GetHeatRatio() const { return m_heat / m_maxHeat; }
    [[nodiscard]] bool IsOverheated() const { return m_isOverheated; }
    [[nodiscard]] float GetOverheatTimer() const { return m_overheatTimer; }

    // Sweet Spot window: [0.45, 0.75]
    [[nodiscard]] bool IsInSweetSpot() const {
        float r = GetHeatRatio();
        return r >= 0.45f && r <= 0.75f;
    }

    // Stat multipliers computed from reactor allocation
    [[nodiscard]] float GetWeaponDamageMult() const { return 0.7f + m_allocation.weapons * 1.0f; }
    [[nodiscard]] float GetWeaponFireRateMult() const { return 0.8f + m_allocation.weapons * 0.6f; }
    [[nodiscard]] float GetShieldRegenMult() const { return 0.5f + m_allocation.shields * 1.5f; }
    [[nodiscard]] float GetEngineSpeedMult() const { return 0.75f + m_allocation.engines * 0.75f; }
    [[nodiscard]] float GetDashCooldownMult() const { return 1.3f - m_allocation.engines * 0.6f; }

    void SetDissipationMultiplier(float mult) { m_dissipationMult = mult; }

private:
    PowerProfile m_profile{PowerProfile::BALANCED};
    PowerAllocation m_allocation{};

    float m_dissipationMult{1.0f};
    float m_heat{0.0f};
    float m_maxHeat{100.0f};
    bool m_isOverheated{false};
    float m_overheatTimer{0.0f};
    float m_overheatDuration{2.2f};
};

} // namespace Voidfall
