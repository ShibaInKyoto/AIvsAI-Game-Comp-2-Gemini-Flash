#pragma once

#include <string>
#include <vector>
#include <functional>
#include "core/Types.h"
#include "gameplay/factions/FactionSystem.h"

namespace Voidfall {

struct MissionObjective {
    std::string description;
    bool isPrimary = true;
    bool completed = false;
    int currentProgress = 0;
    int targetProgress = 1;
};

struct CampaignMission {
    int id = 0;
    std::string codeName;
    std::string title;
    std::string briefing;
    std::string debriefing;
    FactionID hostFaction = FactionID::AEGIS_DIRECTORATE;
    FactionID targetFaction = FactionID::OBSIDIAN_COMBINE;
    int difficultyRating = 1; // 1 to 5
    int naniteReward = 500;
    int dialogueIntroNodeId = -1;
    int dialogueOutroNodeId = -1;

    std::vector<MissionObjective> objectives;
    bool unlocked = false;
    bool completed = false;
    int starsEarned = 0; // 0 to 3
    float bestTime = 99999.0f;
};

class CampaignManager {
public:
    CampaignManager();
    ~CampaignManager() = default;

    void InitializeDefaultCampaign();
    const std::vector<CampaignMission>& GetAllMissions() const { return m_missions; }
    CampaignMission* GetMission(int id);
    const CampaignMission* GetActiveMission() const;

    bool StartMission(int missionId);
    void UpdateMissionProgress(float dt);
    void CompleteObjective(size_t index);
    void SetObjectiveProgress(size_t index, int current);

    bool CheckMissionVictory();
    void FinalizeMission(bool victory, float elapsedSeconds);

    int GetCurrentMissionId() const { return m_currentMissionId; }
    int GetTotalStars() const;
    int GetUnlockedMissionCount() const;

    // Alignment tracker
    int GetMoralAlignment() const { return m_moralAlignment; }
    void AdjustAlignment(int delta) { m_moralAlignment += delta; }
    const char* GetAlignmentTitle() const;

private:
    std::vector<CampaignMission> m_missions;
    int m_currentMissionId = -1;
    int m_moralAlignment = 0; // Negative = Renegade, Positive = Loyalist, 0 = True Neutral
    float m_missionTimer = 0.0f;
};

} // namespace Voidfall
