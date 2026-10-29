#include "ui/Menu.h"
#include "gameplay/PlayerMech.h"
#include "gameplay/Weapons.h"
#include <cmath>

namespace Voidfall {

void Menu::DrawTitle(int selectedIndex, float animTime) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Background gradient effect
    DrawRectangleGradientV(0, 0, screenW, screenH, {8, 12, 22, 255}, {14, 20, 36, 255});

    // Subtitle & Title
    float glow = (std::sin(animTime * 2.5f) + 1.0f) * 0.5f;
    Color titleGlow = {
        static_cast<unsigned char>(120 + 80 * glow),
        static_cast<unsigned char>(200 + 55 * glow),
        255,
        255
    };

    DrawText("AI vs AI GAME COMPETITION 2 • GEMINI FLASH", screenW / 2 - 210, screenH / 2 - 180, 16, {140, 160, 200, 200});
    DrawText("VOIDFALL: PROTOCOL ZERO", screenW / 2 - 290, screenH / 2 - 140, 38, titleGlow);
    DrawText("TACTICAL HIGH-VELOCITY MECH COMBAT & EXTRACTION", screenW / 2 - 250, screenH / 2 - 95, 15, {160, 220, 255, 220});

    // Menu options
    const char* options[] = {
        "1. LAUNCH EXPEDITION (CAMPAIGN)",
        "2. TITAN COLOSSEUM (BOSS RUSH)",
        "3. MECH HANGAR & WEAPONS",
        "4. TACTICAL CODEX & MANUAL",
        "5. EXIT TO DESKTOP"
    };

    for (int i = 0; i < 5; ++i) {
        int y = screenH / 2 - 20 + i * 44;
        bool isSel = (i == selectedIndex);

        if (isSel) {
            DrawRectangle(screenW / 2 - 180, y - 6, 360, 36, {40, 80, 140, 180});
            DrawRectangleLines(screenW / 2 - 180, y - 6, 360, 36, {100, 200, 255, 255});
            DrawText(">", screenW / 2 - 165, y + 2, 20, {255, 230, 80, 255});
            DrawText(options[i], screenW / 2 - 140, y + 2, 18, {255, 255, 255, 255});
        } else {
            DrawText(options[i], screenW / 2 - 140, y + 2, 18, {160, 175, 200, 200});
        }
    }

    DrawText("[W/S or UP/DOWN] Navigate • [ENTER/SPACE/LMB] Engage", screenW / 2 - 210, screenH - 45, 14, {120, 140, 170, 200});
}

void Menu::DrawHangar(const PlayerMech& player, int selectedFrame, int selectedPrim, int selectedOrd) {
    (void)player;
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangle(0, 0, screenW, screenH, {10, 15, 25, 245});
    DrawText("MECH HANGAR & TACTICAL OUTFITTER", 50, 40, 28, {100, 220, 255, 255});

    // 1. Frame Selection
    DrawText("CHASSIS FRAME:", 50, 100, 18, {220, 230, 250, 255});
    const char* frames[] = {"STRIKER (Balanced & Swift)", "TITAN (Heavy Armor & Fortress)", "PHANTOM (Stealth & Crit)", "AEGIS (Reflective Shields)"};
    for (int i = 0; i < 4; ++i) {
        Color col = (i == selectedFrame) ? Color{255, 220, 80, 255} : Color{140, 160, 190, 200};
        DrawText(frames[i], 70, 130 + i * 28, 15, col);
    }

    // 2. Primary Weapons
    DrawText("LEFT ARM (PRIMARY):", 50, 260, 18, {220, 230, 250, 255});
    const char* prims[] = {"Kinetic Rail-Vulcan", "Particle Beam Lance", "Quantum Flak Shotgun", "Arc Tempest Emitter"};
    for (int i = 0; i < 4; ++i) {
        Color col = (i == selectedPrim) ? Color{255, 220, 80, 255} : Color{140, 160, 190, 200};
        DrawText(prims[i], 70, 290 + i * 28, 15, col);
    }

    // 3. Ordnance Weapons
    DrawText("RIGHT ARM (ORDNANCE):", 50, 420, 18, {220, 230, 250, 255});
    const char* ords[] = {"Hydra Swarm Missiles", "Singularity Projector", "Cryo-Stasis Rail Driver"};
    for (int i = 0; i < 3; ++i) {
        Color col = (i == selectedOrd) ? Color{255, 220, 80, 255} : Color{140, 160, 190, 200};
        DrawText(ords[i], 70, 450 + i * 28, 15, col);
    }

    DrawText("Press [SPACE/ENTER] to Deploy Mech • [ESC] Return to Title", 50, screenH - 50, 16, {120, 220, 255, 255});
}

void Menu::DrawUpgradeDraft(const std::vector<UpgradeCard>& cards, int selectedIdx, int naniteCores) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangle(0, 0, screenW, screenH, {8, 12, 20, 235});
    DrawText("SECTOR EXTRACTION SUCCESSFUL", screenW / 2 - 220, 60, 26, {100, 255, 140, 255});
    DrawText("DRAFT TACTICAL UPGRADE MATRIX", screenW / 2 - 160, 95, 18, {180, 210, 240, 200});

    DrawText(("AVAILABLE NANITE CORES: " + std::to_string(naniteCores)).c_str(), screenW / 2 - 130, 130, 16, {100, 240, 255, 255});

    int cardW = 320;
    int cardH = 260;
    int gap = 30;
    int totalW = static_cast<int>(cards.size()) * cardW + (static_cast<int>(cards.size()) - 1) * gap;
    int startX = (screenW - totalW) / 2;
    int startY = screenH / 2 - 100;

    for (size_t i = 0; i < cards.size(); ++i) {
        int x = startX + static_cast<int>(i) * (cardW + gap);
        bool isSel = (static_cast<int>(i) == selectedIdx);

        Color borderCol = isSel ? Color{255, 230, 80, 255} : Color{60, 90, 140, 180};
        Color bgCol = isSel ? Color{25, 45, 75, 240} : Color{15, 25, 40, 220};

        DrawRectangle(x, startY, cardW, cardH, bgCol);
        DrawRectangleLines(x, startY, cardW, cardH, borderCol);

        DrawText(cards[i].category.c_str(), x + 20, startY + 20, 12, {140, 180, 220, 200});
        DrawText(cards[i].name.c_str(), x + 20, startY + 40, 18, {255, 255, 255, 255});

        // Description word wrap
        DrawText(cards[i].description.c_str(), x + 20, startY + 80, 14, {180, 200, 230, 230});

        DrawText(("COST: " + std::to_string(cards[i].cost) + " CORES").c_str(), x + 20, startY + cardH - 35, 14, {100, 240, 255, 255});
    }

    DrawText("[A/D or LEFT/RIGHT] Select • [ENTER/SPACE] Acquire Upgrade & Proceed", screenW / 2 - 270, screenH - 60, 15, {140, 170, 200, 200});
}

void Menu::DrawPause() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangle(0, 0, screenW, screenH, {5, 10, 18, 200});
    DrawText("TACTICAL SYSTEM PAUSED", screenW / 2 - 160, screenH / 2 - 60, 26, {100, 220, 255, 255});
    DrawText("Press [P] or [ESC] to Resume Combat", screenW / 2 - 135, screenH / 2, 16, {180, 200, 230, 200});
    DrawText("Controls: [WASD] Thrusters • [LMB/RMB] Weapons • [SPACE] Phase Dash", screenW / 2 - 260, screenH / 2 + 50, 14, {140, 160, 190, 180});
    DrawText("[1,2,3,4] Reactor Profiles • [F] Flash Purge", screenW / 2 - 150, screenH / 2 + 75, 14, {140, 160, 190, 180});
}

void Menu::DrawGameOver(int wavesCleared, int naniteCores) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangle(0, 0, screenW, screenH, {20, 8, 12, 230});
    DrawText("MECH HULL DESTROYED", screenW / 2 - 180, screenH / 2 - 80, 32, {255, 60, 60, 255});
    DrawText("PROTOCOL ZERO EXTRACTION FAILED", screenW / 2 - 170, screenH / 2 - 35, 16, {255, 140, 140, 200});

    DrawText(("WAVES CLEARED: " + std::to_string(wavesCleared)).c_str(), screenW / 2 - 90, screenH / 2 + 15, 18, {255, 220, 200, 255});
    DrawText(("NANITE CORES RETRIEVED: " + std::to_string(naniteCores)).c_str(), screenW / 2 - 120, screenH / 2 + 45, 18, {100, 240, 255, 255});

    DrawText("Press [SPACE/ENTER] to Rebuild Mech • [ESC] Title", screenW / 2 - 190, screenH / 2 + 105, 16, {200, 220, 255, 220});
}

void Menu::DrawVictory(int totalCores, float runTimeSeconds) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangle(0, 0, screenW, screenH, {8, 22, 18, 235});
    DrawText("SECTOR CORE ANNIHILATED!", screenW / 2 - 220, screenH / 2 - 90, 32, {100, 255, 140, 255});
    DrawText("VOIDFALL CAMPAIGN COMPLETED", screenW / 2 - 160, screenH / 2 - 45, 18, {180, 240, 220, 220});

    DrawText(("TOTAL NANITE CORES: " + std::to_string(totalCores)).c_str(), screenW / 2 - 110, screenH / 2 + 10, 18, {100, 240, 255, 255});
    DrawText(("MISSION TIME: " + std::to_string(static_cast<int>(runTimeSeconds)) + " SECONDS").c_str(), screenW / 2 - 110, screenH / 2 + 40, 18, {255, 230, 100, 255});

    DrawText("Press [SPACE/ENTER] to Return to Command Hangar", screenW / 2 - 190, screenH / 2 + 105, 16, {200, 240, 255, 220});
}

void Menu::DrawCodex() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangle(0, 0, screenW, screenH, {10, 15, 25, 250});
    DrawText("VOIDFALL: PROTOCOL ZERO - TACTICAL CODEX", 50, 40, 26, {100, 220, 255, 255});

    int y = 90;
    DrawText("1. POWER ROUTING (TRI-SYSTEM REACTOR):", 50, y, 16, {255, 220, 80, 255});
    DrawText("- [1] ASSAULT (70% WPN): Massive fire rate & damage bonus. For melting bosses and dense swarms.", 70, y + 24, 14, {200, 220, 245, 255});
    DrawText("- [2] BULWARK (70% SHD): Rapid shield regeneration & shockwave parries. Essential when under artillery fire.", 70, y + 46, 14, {200, 220, 245, 255});
    DrawText("- [3] INTERCEPTOR (70% ENG): Hyper-velocity evasion and rapid dash recharge.", 70, y + 68, 14, {200, 220, 245, 255});

    y += 115;
    DrawText("2. ACTIVE THERMAL HEAT FLUSH (SWEET SPOT):", 50, y, 16, {255, 220, 80, 255});
    DrawText("- Firing weapons generates Core Heat. At 100% heat, the mech suffers a hard weapons lockout.", 70, y + 24, 14, {200, 220, 245, 255});
    DrawText("- When Heat is between 45% and 75%, press [F] to execute a FLASH PURGE.", 70, y + 46, 14, {200, 220, 245, 255});
    DrawText("- Flash Purge vaporizes incoming enemy projectiles and instantly resets heat to 0%!", 70, y + 68, 14, {200, 220, 245, 255});

    y += 115;
    DrawText("3. DIRECTIONAL DEFLECTOR SHIELD:", 50, y, 16, {255, 220, 80, 255});
    DrawText("- Your shield projects in a 110-degree forward arc facing your aiming cursor.", 70, y + 24, 14, {200, 220, 245, 255});
    DrawText("- Always aim your cockpit toward incoming heavy projectile salvos to mitigate damage.", 70, y + 46, 14, {200, 220, 245, 255});

    DrawText("Press [ESC] to Return to Title", 50, screenH - 50, 16, {100, 220, 255, 255});
}

} // namespace Voidfall
