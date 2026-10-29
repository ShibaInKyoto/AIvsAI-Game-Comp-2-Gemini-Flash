#include "DialogueEngine.h"
#include "engine/AudioSynth.h"
#include <algorithm>
#include <cmath>

namespace Voidfall {

DialogueEngine::DialogueEngine() {
    LoadDefaultStoryTree();
}

void DialogueEngine::AddNode(const DialogueNode& node) {
    m_nodes[node.id] = node;
}

const DialogueNode* DialogueEngine::GetNode(int id) const {
    auto it = m_nodes.find(id);
    if (it != m_nodes.end()) return &it->second;
    return nullptr;
}

void DialogueEngine::SetFlag(const std::string& key, int value) {
    m_storyFlags[key] = value;
}

int DialogueEngine::GetFlag(const std::string& key, int defaultVal) const {
    auto it = m_storyFlags.find(key);
    if (it != m_storyFlags.end()) return it->second;
    return defaultVal;
}

bool DialogueEngine::HasFlag(const std::string& key) const {
    return m_storyFlags.find(key) != m_storyFlags.end();
}

void DialogueEngine::StartConversation(int startNodeId) {
    const auto* node = GetNode(startNodeId);
    if (!node) return;

    m_active = true;
    m_currentNodeId = startNodeId;
    m_textTimer = 0.0f;
    m_revealedChars = 0;
    m_autoAdvanceTimer = 0.0f;
    m_selectedChoice = 0;

    PlayVoiceChirp(node->speaker, node->mood);
}

void DialogueEngine::Close() {
    m_active = false;
    m_currentNodeId = -1;
}

void DialogueEngine::PlayVoiceChirp(CharacterID speaker, DialogueMood mood) {
    (void)speaker;
    (void)mood;
    // Play electronic communication chirp
    AudioSynth::Play(SoundID::POWER_SHIFT);
}

void DialogueEngine::Update(float dt) {
    if (!m_active) return;

    const auto* node = GetNode(m_currentNodeId);
    if (!node) {
        Close();
        return;
    }

    const int totalLen = static_cast<int>(node->message.length());
    if (m_revealedChars < totalLen) {
        m_textTimer += dt;
        // 45 chars per second
        float speed = 45.0f;
        char lastChar = (m_revealedChars > 0 && m_revealedChars <= totalLen) ? node->message[m_revealedChars - 1] : ' ';
        if (lastChar == '.' || lastChar == '!' || lastChar == '?') {
            speed = 15.0f; // Pause at sentences
        } else if (lastChar == ',' || lastChar == ';') {
            speed = 25.0f; // Pause at commas
        }

        if (m_textTimer >= (1.0f / speed)) {
            m_textTimer = 0.0f;
            m_revealedChars++;
        }
    } else {
        // Text is fully revealed
        if (node->choices.empty() && node->nextAutoNodeId >= 0 && node->autoAdvanceDelay > 0.0f) {
            m_autoAdvanceTimer += dt;
            if (m_autoAdvanceTimer >= node->autoAdvanceDelay) {
                StartConversation(node->nextAutoNodeId);
            }
        }
    }
}

void DialogueEngine::SelectNextChoice() {
    const auto* node = GetNode(m_currentNodeId);
    if (!node || node->choices.empty()) return;

    m_selectedChoice = (m_selectedChoice + 1) % static_cast<int>(node->choices.size());
    AudioSynth::Play(SoundID::POWER_SHIFT);
}

void DialogueEngine::SelectPrevChoice() {
    const auto* node = GetNode(m_currentNodeId);
    if (!node || node->choices.empty()) return;

    int n = static_cast<int>(node->choices.size());
    m_selectedChoice = (m_selectedChoice + n - 1) % n;
    AudioSynth::Play(SoundID::POWER_SHIFT);
}

void DialogueEngine::SelectChoice(int choiceIndex) {
    const auto* node = GetNode(m_currentNodeId);
    if (!node || choiceIndex < 0 || choiceIndex >= static_cast<int>(node->choices.size())) return;

    const auto& choice = node->choices[choiceIndex];
    if (choice.onSelected) {
        choice.onSelected();
    }

    if (choice.targetNodeId >= 0) {
        StartConversation(choice.targetNodeId);
    } else {
        Close();
    }
}

void DialogueEngine::ConfirmOrAdvance() {
    if (!m_active) return;
    const auto* node = GetNode(m_currentNodeId);
    if (!node) return;

    const int totalLen = static_cast<int>(node->message.length());
    if (m_revealedChars < totalLen) {
        // Quick complete text
        m_revealedChars = totalLen;
        return;
    }

    if (!node->choices.empty()) {
        SelectChoice(m_selectedChoice);
    } else if (node->nextAutoNodeId >= 0) {
        StartConversation(node->nextAutoNodeId);
    } else {
        Close();
    }
}

Color DialogueEngine::GetSpeakerColor(CharacterID speaker) const {
    switch (speaker) {
        case CharacterID::VANCE:   return Color{ 70, 150, 255, 255 };  // Aegis Cyan-Blue
        case CharacterID::MALAKOR: return Color{ 230, 40, 50, 255 };   // Obsidian Crimson
        case CharacterID::ROOK:    return Color{ 255, 180, 40, 255 };  // Syndicate Gold
        case CharacterID::MARA:    return Color{ 190, 70, 255, 255 };  // Iron Purple
        case CharacterID::AURORA:  return Color{ 60, 255, 190, 255 };  // AI Emerald
        default:                   return Color{ 200, 200, 200, 255 };
    }
}

const char* DialogueEngine::GetSpeakerName(CharacterID speaker) const {
    switch (speaker) {
        case CharacterID::VANCE:   return "CMDR. SAMANTHA VANCE [AEGIS COMMAND]";
        case CharacterID::MALAKOR: return "ARCH-INQUISITOR MALAKOR [OBSIDIAN]";
        case CharacterID::ROOK:    return "KAELEN 'ROOK' VEX [NULL-SEC BROKER]";
        case CharacterID::MARA:    return "SISTER MARA [CHILDREN OF IRON]";
        case CharacterID::AURORA:  return "A.U.R.O.R.A. [SUB-ROUTINE MK-IV]";
        default:                   return "UNKNOWN TRANSMISSION";
    }
}

void DialogueEngine::Render(int screenWidth, int screenHeight) {
    if (!m_active) return;

    const auto* node = GetNode(m_currentNodeId);
    if (!node) return;

    // Window dimensions
    const int boxW = std::min(860, screenWidth - 60);
    const int boxH = 210;
    const int boxX = (screenWidth - boxW) / 2;
    const int boxY = screenHeight - boxH - 30;

    // Background panel with alpha tint
    DrawRectangle(boxX, boxY, boxW, boxH, Color{ 10, 14, 24, 235 });
    Color themeCol = GetSpeakerColor(node->speaker);

    // Frame border
    DrawRectangleLines(boxX, boxY, boxW, boxH, Color{ 40, 60, 90, 255 });
    DrawRectangleLines(boxX + 2, boxY + 2, boxW - 4, boxH - 4, Color{ 20, 30, 50, 180 });

    // Top accent bar
    DrawRectangle(boxX, boxY, boxW, 4, themeCol);

    // Speaker portrait box
    const int portSize = 74;
    const int portX = boxX + 16;
    const int portY = boxY + 16;
    DrawRectangle(portX, portY, portSize, portSize, Color{ 15, 22, 35, 255 });
    DrawRectangleLines(portX, portY, portSize, portSize, themeCol);

    // Symbolic geometric avatar inside portrait box
    Vector2 center = { static_cast<float>(portX + portSize / 2), static_cast<float>(portY + portSize / 2) };
    if (node->speaker == CharacterID::VANCE) {
        DrawPoly(center, 3, 22.0f, 0.0f, themeCol); // Upward shield triangle
    } else if (node->speaker == CharacterID::MALAKOR) {
        DrawPoly(center, 4, 22.0f, 45.0f, themeCol); // Diamond
    } else if (node->speaker == CharacterID::ROOK) {
        DrawPoly(center, 6, 22.0f, 0.0f, themeCol); // Hexagon
    } else if (node->speaker == CharacterID::MARA) {
        DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), 20.0f, themeCol);
        DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), 10.0f, themeCol);
    } else {
        DrawPoly(center, 8, 20.0f, 22.5f, themeCol);
    }

    // Speaker name badge
    const char* spkName = GetSpeakerName(node->speaker);
    DrawText(spkName, boxX + 104, boxY + 16, 16, themeCol);

    // Mood indicator tag
    const char* moodText = "NOMINAL";
    if (node->mood == DialogueMood::URGENT) moodText = "ALERT: PRIORITY COMM";
    else if (node->mood == DialogueMood::AGGRESSIVE) moodText = "HOSTILE INTENT";
    else if (node->mood == DialogueMood::DIAGNOSTIC) moodText = "SYS_DIAGNOSTIC";
    else if (node->mood == DialogueMood::CRYPTIC) moodText = "ENCRYPTED SIGNAL";
    else if (node->mood == DialogueMood::TRIUMPHANT) moodText = "OBJECTIVE SECURED";
    DrawText(moodText, boxX + boxW - MeasureText(moodText, 12) - 18, boxY + 18, 12, Color{ 150, 170, 200, 200 });

    // Divider line
    DrawLine(boxX + 104, boxY + 38, boxX + boxW - 16, boxY + 38, Color{ 50, 70, 100, 160 });

    // Animated text snippet
    std::string visibleText = node->message.substr(0, m_revealedChars);
    
    // Draw wrapped text
    int textX = boxX + 104;
    int textY = boxY + 48;
    int maxLineW = boxW - 120;
    
    // Word wrap rendering
    std::string currentLine = "";
    size_t startIdx = 0;
    int lineOffset = 0;

    for (size_t i = 0; i <= visibleText.length(); ++i) {
        if (i == visibleText.length() || visibleText[i] == ' ' || visibleText[i] == '\n') {
            std::string word = visibleText.substr(startIdx, i - startIdx);
            std::string testLine = currentLine.empty() ? word : (currentLine + " " + word);

            if (MeasureText(testLine.c_str(), 14) > maxLineW || (i < visibleText.length() && visibleText[i] == '\n')) {
                DrawText(currentLine.c_str(), textX, textY + lineOffset, 14, Color{ 220, 230, 245, 255 });
                lineOffset += 18;
                currentLine = word;
            } else {
                currentLine = testLine;
            }
            startIdx = i + 1;
        }
    }
    if (!currentLine.empty()) {
        DrawText(currentLine.c_str(), textX, textY + lineOffset, 14, Color{ 220, 230, 245, 255 });
    }

    // Render choices or advance prompt
    if (m_revealedChars >= static_cast<int>(node->message.length())) {
        if (!node->choices.empty()) {
            int choiceY = boxY + 115;
            for (size_t c = 0; c < node->choices.size(); ++c) {
                const auto& ch = node->choices[c];
                bool isSelected = (static_cast<int>(c) == m_selectedChoice);

                Rectangle choiceRect = {
                    static_cast<float>(boxX + 104),
                    static_cast<float>(choiceY + static_cast<int>(c) * 26),
                    static_cast<float>(boxW - 120),
                    22.0f
                };

                if (isSelected) {
                    DrawRectangleRec(choiceRect, Color{ 35, 65, 110, 220 });
                    DrawRectangleLinesEx(choiceRect, 1.0f, themeCol);
                    DrawText(">>", static_cast<int>(choiceRect.x + 6), static_cast<int>(choiceRect.y + 4), 13, themeCol);
                } else {
                    DrawRectangleRec(choiceRect, Color{ 18, 25, 40, 160 });
                    DrawRectangleLinesEx(choiceRect, 1.0f, Color{ 40, 55, 80, 140 });
                }

                DrawText(ch.text.c_str(), static_cast<int>(choiceRect.x + 28), static_cast<int>(choiceRect.y + 4), 13, isSelected ? Color{ 255, 255, 255, 255 } : Color{ 170, 190, 215, 220 });
            }
        } else {
            // Blink prompt
            if (fmod(GetTime() * 2.0, 1.0) < 0.6) {
                const char* advText = "[PRESS ENTER / CLICK TO CONTINUE]";
                int advW = MeasureText(advText, 12);
                DrawText(advText, boxX + boxW - advW - 20, boxY + boxH - 24, 12, Color{ 100, 200, 255, 200 });
            }
        }
    }
}

void DialogueEngine::LoadDefaultStoryTree() {
    // Node 100: Prologue Awakening
    {
        DialogueNode n;
        n.id = 100;
        n.speaker = CharacterID::AURORA;
        n.mood = DialogueMood::DIAGNOSTIC;
        n.message = "System diagnostics nominal. Pilot neural link synchronized. We are adrift in Sector Sigma-9 following the reactor containment breach. Long-range sensors detect hostile scavenger signals.";
        n.nextAutoNodeId = 101;
        n.autoAdvanceDelay = 0.0f;
        AddNode(n);
    }

    // Node 101: Vance incoming transmission
    {
        DialogueNode n;
        n.id = 101;
        n.speaker = CharacterID::VANCE;
        n.mood = DialogueMood::URGENT;
        n.message = "Vanguard unit, this is Commander Vance of the Aegis Directorate. We intercepted your emergency beacon. The Obsidian Combine has launched a sector-wide purge protocol. We need you to secure the relay!";
        
        DialogueChoice c1;
        c1.text = "Understood, Commander. Powering up kinetic rails and engaging hostiles.";
        c1.targetNodeId = 102;
        c1.alignmentDelta = +1;

        DialogueChoice c2;
        c2.text = "My chassis is heavily damaged. What bounty is the Directorate offering?";
        c2.targetNodeId = 103;
        c2.alignmentDelta = 0;

        DialogueChoice c3;
        c3.text = "I fight for my own survival now. Stay out of my targeting arc.";
        c3.targetNodeId = 104;
        c3.alignmentDelta = -1;

        n.choices.push_back(c1);
        n.choices.push_back(c2);
        n.choices.push_back(c3);
        AddNode(n);
    }

    // Node 102: Loyalist path
    {
        DialogueNode n;
        n.id = 102;
        n.speaker = CharacterID::VANCE;
        n.mood = DialogueMood::CALM;
        n.message = "Excellent. Patching tactical telemetry to your HUD. Clear the vanguard scout drones and link with our extraction frigate at Jump Gate Epsilon.";
        n.nextAutoNodeId = -1;
        AddNode(n);
    }

    // Node 103: Mercenary path
    {
        DialogueNode n;
        n.id = 103;
        n.speaker = CharacterID::ROOK;
        n.mood = DialogueMood::CALM;
        n.message = "Now that's my kind of pilot. Hey friend, Rook here. If Vance gets stingy with the credits, Null-Sec pays triple for salvaged Combine quantum cores. Keep your eyes open.";
        n.nextAutoNodeId = -1;
        AddNode(n);
    }

    // Node 104: Renegade path
    {
        DialogueNode n;
        n.id = 104;
        n.speaker = CharacterID::MALAKOR;
        n.mood = DialogueMood::AGGRESSIVE;
        n.message = "Defiance is an inefficient evolutionary strategy, pilot. Your machine belongs to the Combine. Lay down your weapons or be dismantled for alloy feedstock.";
        n.nextAutoNodeId = -1;
        AddNode(n);
    }
}

} // namespace Voidfall
