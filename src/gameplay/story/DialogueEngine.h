#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <raylib.h>
#include "core/Types.h"
#include "gameplay/factions/FactionSystem.h"

namespace Voidfall {

enum class CharacterID {
    VANCE,          // Commander Samantha Vance (Aegis Directorate)
    MALAKOR,        // Arch-Inquisitor Malakor (Obsidian Combine)
    ROOK,           // Kaelen "Rook" Vex (Null-Sec Syndicate)
    MARA,           // Sister Mara (Children of Iron)
    AURORA,         // A.U.R.O.R.A. (Synthetic Mech Co-Pilot)
    UNKNOWN
};

enum class DialogueMood {
    CALM,
    URGENT,
    AGGRESSIVE,
    DIAGNOSTIC,
    CRYPTIC,
    TRIUMPHANT
};

struct DialogueChoice {
    std::string text;
    int targetNodeId;
    FactionID requiredFaction = FactionID::CIVILIAN_TRADERS;
    int requiredReputation = -100; // minimum rep needed
    int naniteCost = 0;
    int alignmentDelta = 0; // -1 = Renegade, +1 = Loyalist
    std::function<void()> onSelected = nullptr;
};

struct DialogueNode {
    int id = 0;
    CharacterID speaker = CharacterID::UNKNOWN;
    std::string speakerTitle;
    std::string message;
    DialogueMood mood = DialogueMood::CALM;
    std::vector<DialogueChoice> choices;
    int nextAutoNodeId = -1; // -1 if requires choice or ends
    float autoAdvanceDelay = 0.0f; // 0 if wait for confirm
    std::string questTriggerId = "";
};

class DialogueEngine {
public:
    DialogueEngine();
    ~DialogueEngine() = default;

    void LoadDefaultStoryTree();
    void StartConversation(int startNodeId);
    void Update(float dt);
    void Render(int screenWidth, int screenHeight);

    bool IsActive() const { return m_active; }
    void Close();

    void SelectChoice(int choiceIndex);
    void ConfirmOrAdvance();

    // Node registration
    void AddNode(const DialogueNode& node);
    const DialogueNode* GetNode(int id) const;

    // Narrative flags
    void SetFlag(const std::string& key, int value);
    int GetFlag(const std::string& key, int defaultVal = 0) const;
    bool HasFlag(const std::string& key) const;

    int GetSelectedChoice() const { return m_selectedChoice; }
    void SelectNextChoice();
    void SelectPrevChoice();

private:
    void PlayVoiceChirp(CharacterID speaker, DialogueMood mood);

    bool m_active = false;
    int m_currentNodeId = -1;
    float m_textTimer = 0.0f;
    int m_revealedChars = 0;
    float m_autoAdvanceTimer = 0.0f;
    int m_selectedChoice = 0;

    std::unordered_map<int, DialogueNode> m_nodes;
    std::unordered_map<std::string, int> m_storyFlags;

    Color GetSpeakerColor(CharacterID speaker) const;
    const char* GetSpeakerName(CharacterID speaker) const;
};

} // namespace Voidfall
