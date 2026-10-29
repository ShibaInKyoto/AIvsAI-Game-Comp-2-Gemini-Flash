#include <cassert>
#include <iostream>
#include "gameplay/story/DialogueEngine.h"
#include "gameplay/story/CampaignManager.h"

namespace Voidfall {

void RunCampaignDialogueTests() {
    std::cout << "[TEST] Running Campaign & Dialogue Engine Tests..." << std::endl;

    // 1. Dialogue Engine Tests
    DialogueEngine dialogue;
    assert(!dialogue.IsActive());

    // Start prologue
    dialogue.StartConversation(100);
    assert(dialogue.IsActive());

    const auto* cur = dialogue.GetNode(100);
    assert(cur != nullptr);
    assert(cur->speaker == CharacterID::AURORA);

    // Update text
    dialogue.Update(0.5f);
    dialogue.ConfirmOrAdvance(); // Auto-completes text or advances to 101

    if (dialogue.GetNode(101)) {
        dialogue.StartConversation(101);
        dialogue.ConfirmOrAdvance(); // Complete text
        dialogue.SelectPrevChoice();
        dialogue.SelectNextChoice();
        // Select first choice (Loyalist path)
        dialogue.SelectChoice(0);
    }

    // Story flags
    dialogue.SetFlag("FOUNDRY_BREACHED", 1);
    assert(dialogue.HasFlag("FOUNDRY_BREACHED"));
    assert(dialogue.GetFlag("FOUNDRY_BREACHED") == 1);
    assert(dialogue.GetFlag("UNKNOWN_FLAG", 99) == 99);

    dialogue.Close();
    assert(!dialogue.IsActive());

    // 2. Campaign Manager Tests
    CampaignManager campaign;
    assert(campaign.GetAllMissions().size() >= 5);
    assert(campaign.GetUnlockedMissionCount() == 1);

    auto* m1 = campaign.GetMission(1);
    assert(m1 != nullptr);
    assert(m1->unlocked);
    assert(!m1->completed);

    bool started = campaign.StartMission(1);
    assert(started);
    assert(campaign.GetCurrentMissionId() == 1);

    // Progress primary objectives
    campaign.SetObjectiveProgress(0, 15);
    campaign.SetObjectiveProgress(1, 1);
    assert(campaign.CheckMissionVictory());

    // Complete bonus objective
    campaign.CompleteObjective(2);

    campaign.FinalizeMission(true, 75.0f);
    assert(m1->completed);
    assert(m1->starsEarned == 3);
    assert(m1->bestTime <= 75.0f);

    // Mission 2 should now be unlocked
    auto* m2 = campaign.GetMission(2);
    assert(m2 != nullptr);
    assert(m2->unlocked);
    assert(campaign.GetUnlockedMissionCount() == 2);

    // Test moral alignment
    campaign.AdjustAlignment(2);
    assert(campaign.GetMoralAlignment() == 2);
    assert(std::string(campaign.GetAlignmentTitle()).find("Loyalist") != std::string::npos);

    std::cout << "[PASS] Campaign & Dialogue Engine Tests Passed!" << std::endl;
}

} // namespace Voidfall
