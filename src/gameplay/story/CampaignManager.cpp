#include "CampaignManager.h"
#include <algorithm>

namespace Voidfall {

CampaignManager::CampaignManager() {
    InitializeDefaultCampaign();
}

void CampaignManager::InitializeDefaultCampaign() {
    m_missions.clear();

    // Mission 1: The Iron Wake
    {
        CampaignMission m;
        m.id = 1;
        m.codeName = "OP: IRON WAKE";
        m.title = "The Iron Wake";
        m.briefing = "Awaken auxiliary power in Sector Sigma-9 and purge rogue scavenger units threatening the relay array.";
        m.debriefing = "Scavenger strike suppressed. Comms link with Aegis Directorate established.";
        m.hostFaction = FactionID::AEGIS_DIRECTORATE;
        m.targetFaction = FactionID::OBSIDIAN_COMBINE;
        m.difficultyRating = 1;
        m.naniteReward = 750;
        m.dialogueIntroNodeId = 100;
        m.dialogueOutroNodeId = 102;
        m.unlocked = true;

        m.objectives.push_back({ "Destroy 15 Hostile Interceptor Drones", true, false, 0, 15 });
        m.objectives.push_back({ "Eliminate Scavenger Corvette flagship", true, false, 0, 1 });
        m.objectives.push_back({ "Bonus: Finish under 90 seconds", false, false, 0, 90 });
        m_missions.push_back(m);
    }

    // Mission 2: Prometheus Drift
    {
        CampaignMission m;
        m.id = 2;
        m.codeName = "OP: PROMETHEUS DRIFT";
        m.title = "Prometheus Drift";
        m.briefing = "Defend the Helium-3 plasma siphon platform from an elite Obsidian Combine assault wing.";
        m.debriefing = "Siphon integrity secured. Combine vanguard retreated with heavy casualties.";
        m.hostFaction = FactionID::CIVILIAN_TRADERS;
        m.targetFaction = FactionID::OBSIDIAN_COMBINE;
        m.difficultyRating = 2;
        m.naniteReward = 1200;
        m.dialogueIntroNodeId = 103;
        m.unlocked = false;

        m.objectives.push_back({ "Defend Siphon Platform for 3 Waves", true, false, 0, 3 });
        m.objectives.push_back({ "Destroy Obsidian Heavy Siege Mech", true, false, 0, 1 });
        m.objectives.push_back({ "Bonus: Maintain Reactor Heat below 75%", false, false, 0, 1 });
        m_missions.push_back(m);
    }

    // Mission 3: Aegis Gate Anomaly
    {
        CampaignMission m;
        m.id = 3;
        m.codeName = "OP: QUANTUM BREACH";
        m.title = "Aegis Gate Anomaly";
        m.briefing = "Navigate the spatial distortion field surrounding the derelict Aegis Gate and override the lockdown core.";
        m.debriefing = "Jump gate stabilized. Hyperspace lane opened to inner systems.";
        m.hostFaction = FactionID::AEGIS_DIRECTORATE;
        m.targetFaction = FactionID::NULL_SEC_SYNDICATE;
        m.difficultyRating = 3;
        m.naniteReward = 1800;
        m.unlocked = false;

        m.objectives.push_back({ "Neutralize 4 Automated Gate Defense Turrets", true, false, 0, 4 });
        m.objectives.push_back({ "Download Encrypted Gate Telemetry", true, false, 0, 1 });
        m.objectives.push_back({ "Bonus: No hull structural damage taken (Shields only)", false, false, 0, 1 });
        m_missions.push_back(m);
    }

    // Mission 4: Obsidian Core Incursion
    {
        CampaignMission m;
        m.id = 4;
        m.codeName = "OP: OBSIDIAN VEIL";
        m.title = "Obsidian Core Incursion";
        m.briefing = "Infiltrate the Combine's mobile orbital foundry. Overload the main coolant rods and extract before thermal collapse.";
        m.debriefing = "Orbital foundry disabled. Combine war production halted across three sectors.";
        m.hostFaction = FactionID::AEGIS_DIRECTORATE;
        m.targetFaction = FactionID::OBSIDIAN_COMBINE;
        m.difficultyRating = 4;
        m.naniteReward = 2500;
        m.unlocked = false;

        m.objectives.push_back({ "Infiltrate Foundry Perimeter and breach bulkheads", true, false, 0, 1 });
        m.objectives.push_back({ "Destroy 2 Super-Heavy Siege Walkers", true, false, 0, 2 });
        m.objectives.push_back({ "Defeat Arch-Inquisitor Malakor's Flagship", true, false, 0, 1 });
        m.objectives.push_back({ "Bonus: Finish with 80%+ accuracy", false, false, 0, 1 });
        m_missions.push_back(m);
    }

    // Mission 5: Protocol Zero
    {
        CampaignMission m;
        m.id = 5;
        m.codeName = "OP: PROTOCOL ZERO";
        m.title = "Protocol Zero: Final Stand";
        m.briefing = "The Sovereign Rogue AI Core has initiated the sector annihilation sequence. Engage and eradicate the Core before the countdown reaches zero.";
        m.debriefing = "Protocol Zero aborted. Galactic network cleansed of synthetic contamination. You are the Voidfall Sovereign.";
        m.hostFaction = FactionID::CHILDREN_OF_IRON;
        m.targetFaction = FactionID::OBSIDIAN_COMBINE;
        m.difficultyRating = 5;
        m.naniteReward = 5000;
        m.unlocked = false;

        m.objectives.push_back({ "Phase 1: Shatter Neural Shield Nodes", true, false, 0, 4 });
        m.objectives.push_back({ "Phase 2: Sever Core Kinetic Rail Cannons", true, false, 0, 2 });
        m.objectives.push_back({ "Phase 3: Obliterate Sovereign Nexus Core", true, false, 0, 1 });
        m.objectives.push_back({ "Bonus: Complete on Hardcore difficulty", false, false, 0, 1 });
        m_missions.push_back(m);
    }
}

CampaignMission* CampaignManager::GetMission(int id) {
    for (auto& m : m_missions) {
        if (m.id == id) return &m;
    }
    return nullptr;
}

const CampaignMission* CampaignManager::GetActiveMission() const {
    for (const auto& m : m_missions) {
        if (m.id == m_currentMissionId) return &m;
    }
    return nullptr;
}

bool CampaignManager::StartMission(int missionId) {
    auto* m = GetMission(missionId);
    if (!m || !m->unlocked) return false;

    m_currentMissionId = missionId;
    m_missionTimer = 0.0f;

    // Reset objectives
    for (auto& obj : m->objectives) {
        obj.completed = false;
        obj.currentProgress = 0;
    }
    return true;
}

void CampaignManager::UpdateMissionProgress(float dt) {
    if (m_currentMissionId < 0) return;
    m_missionTimer += dt;
}

void CampaignManager::CompleteObjective(size_t index) {
    auto* m = GetMission(m_currentMissionId);
    if (!m || index >= m->objectives.size()) return;

    m->objectives[index].completed = true;
    m->objectives[index].currentProgress = m->objectives[index].targetProgress;
}

void CampaignManager::SetObjectiveProgress(size_t index, int current) {
    auto* m = GetMission(m_currentMissionId);
    if (!m || index >= m->objectives.size()) return;

    m->objectives[index].currentProgress = current;
    if (m->objectives[index].currentProgress >= m->objectives[index].targetProgress) {
        m->objectives[index].completed = true;
    }
}

bool CampaignManager::CheckMissionVictory() {
    auto* m = GetMission(m_currentMissionId);
    if (!m) return false;

    for (const auto& obj : m->objectives) {
        if (obj.isPrimary && !obj.completed) {
            return false;
        }
    }
    return true;
}

void CampaignManager::FinalizeMission(bool victory, float elapsedSeconds) {
    auto* m = GetMission(m_currentMissionId);
    if (!m) return;

    if (victory) {
        m->completed = true;
        if (elapsedSeconds < m->bestTime) {
            m->bestTime = elapsedSeconds;
        }

        // Calculate stars
        int stars = 1; // base for victory
        for (const auto& obj : m->objectives) {
            if (!obj.isPrimary && obj.completed) {
                stars++;
            }
        }
        m->starsEarned = std::max(m->starsEarned, std::min(3, stars));

        // Unlock next mission
        for (size_t i = 0; i < m_missions.size(); ++i) {
            if (m_missions[i].id == m_currentMissionId && i + 1 < m_missions.size()) {
                m_missions[i + 1].unlocked = true;
                break;
            }
        }
    }
    m_currentMissionId = -1;
}

int CampaignManager::GetTotalStars() const {
    int total = 0;
    for (const auto& m : m_missions) {
        total += m.starsEarned;
    }
    return total;
}

int CampaignManager::GetUnlockedMissionCount() const {
    int count = 0;
    for (const auto& m : m_missions) {
        if (m.unlocked) count++;
    }
    return count;
}

const char* CampaignManager::GetAlignmentTitle() const {
    if (m_moralAlignment >= 3) return "Aegis Fleet Marshal [Pure Loyalist]";
    if (m_moralAlignment >= 1) return "Directorate Vanguard [Loyalist]";
    if (m_moralAlignment <= -3) return "Combine Scourge [Renegade Desolator]";
    if (m_moralAlignment <= -1) return "Null-Sec Outlaw [Renegade]";
    return "Independent Freeblade [True Neutral]";
}

} // namespace Voidfall
