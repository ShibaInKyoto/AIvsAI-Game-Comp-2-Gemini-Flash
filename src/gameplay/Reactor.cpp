#include "gameplay/Reactor.h"
#include <algorithm>

namespace Voidfall {

Reactor::Reactor() {
    SetProfile(PowerProfile::BALANCED);
}

void Reactor::SetProfile(PowerProfile profile) {
    m_profile = profile;
    switch (profile) {
        case PowerProfile::ASSAULT:
            m_allocation = {0.70f, 0.15f, 0.15f};
            break;
        case PowerProfile::BULWARK:
            m_allocation = {0.15f, 0.70f, 0.15f};
            break;
        case PowerProfile::INTERCEPTOR:
            m_allocation = {0.15f, 0.15f, 0.70f};
            break;
        case PowerProfile::BALANCED:
        default:
            m_allocation = {0.33f, 0.33f, 0.34f};
            break;
    }
}

void Reactor::AddHeat(float amount) {
    if (m_isOverheated) return;

    m_heat += amount;
    if (m_heat >= m_maxHeat) {
        m_heat = m_maxHeat;
        m_isOverheated = true;
        m_overheatTimer = m_overheatDuration;
    }
}

bool Reactor::TryActivePurge(bool& outSweetSpot) {
    outSweetSpot = false;
    if (m_isOverheated) {
        // Can't purge during hard lockout
        return false;
    }

    if (m_heat < 25.0f) {
        // Not enough heat to purge
        return false;
    }

    if (IsInSweetSpot()) {
        outSweetSpot = true;
        m_heat = 0.0f; // Perfect flash purge!
        return true;
    } else {
        // Standard purge with small penalty
        m_heat = std::max(0.0f, m_heat - 40.0f);
        return true;
    }
}

void Reactor::Update(float dt) {
    if (m_isOverheated) {
        m_overheatTimer -= dt;
        // Cool down rapidly during emergency vent
        m_heat -= (m_maxHeat / m_overheatDuration) * dt;
        if (m_overheatTimer <= 0.0f || m_heat <= 0.0f) {
            m_isOverheated = false;
            m_heat = 0.0f;
            m_overheatTimer = 0.0f;
        }
        return;
    }

    // Passive dissipation based on engine allocation
    float baseDissipation = 14.0f; // per second
    float engineBonus = m_allocation.engines * 22.0f;
    float totalDissipation = baseDissipation + engineBonus;

    m_heat = std::max(0.0f, m_heat - totalDissipation * dt);
}

} // namespace Voidfall
