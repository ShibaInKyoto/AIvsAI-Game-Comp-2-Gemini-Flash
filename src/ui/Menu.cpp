#include "ui/Menu.h"
#include "gameplay/PlayerMech.h"
#include "gameplay/Weapons.h"
#include "core/SaveSystem.h"
#include <cmath>

namespace Voidfall {

void Menu::DrawTitle(int selectedIndex, float animTime) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangleGradientV(0, 0, screenW, screenH, {8, 12, 22, 255}, {14, 20, 36, 255});

    float glow = (std::sin(animTime * 2.5f) + 1.0f) * 0.5f;
    Color titleGlow = {
        static_cast<unsigned char>(120 + 80 * glow),
        static_cast<unsigned char>(200 + 55 * glow),
        255,
        255
    };

    DrawText("AI vs AI GAME COMPETITION 2 • GEMINI FLASH", screenW / 2 - 210, screenH / 2 - 210, 16, {140, 160, 200, 200});
    DrawText("VOIDFALL: PROTOCOL ZERO", screenW / 2 - 290, screenH / 2 - 175, 38, titleGlow);
    DrawText("TACTICAL HIGH-VELOCITY KINETIC MECH COMBAT & EXTRACTION", screenW / 2 - 275, screenH / 2 - 130, 15, {160, 220, 255, 220});

    const char* options[] = {
        "1. LAUNCH EXPEDITION (CAMPAIGN)",
        "2. ENDLESS PROTOCOL (SURVIVAL HORDE)",
        "3. TITAN COLOSSEUM (BOSS RUSH)",
        "4. WEAPONS BENCHMARK (FIRING RANGE)",
        "5. MECH HANGAR & ENGINEERING",
        "6. TACTICAL CODEX & MANUAL",
        "7. EXIT TO DESKTOP"
    };

    for (int i = 0; i < 7; ++i) {
        int y = screenH / 2 - 70 + i * 40;
        bool isSel = (i == selectedIndex);

        if (isSel) {
            DrawRectangle(screenW / 2 - 200, y - 4, 400, 32, {40, 80, 140, 180});
            DrawRectangleLines(screenW / 2 - 200, y - 4, 400, 32, {100, 200, 255, 255});
            DrawText(">", screenW / 2 - 185, y + 2, 18, {255, 230, 80, 255});
            DrawText(options[i], screenW / 2 - 160, y + 2, 17, {255, 255, 255, 255});
        } else {
            DrawText(options[i], screenW / 2 - 160, y + 2, 17, {160, 175, 200, 200});
        }
    }

    DrawText("[W/S or UP/DOWN] Navigate • [ENTER/SPACE] Engage", screenW / 2 - 185, screenH - 40, 14, {120, 140, 170, 200});
}

void Menu::DrawHangar(const PlayerMech& player, int hangarTab, int selectedFrame, int selectedPrim, int selectedOrd, int selectedUpgrade) {
    (void)player;
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangle(0, 0, screenW, screenH, {10, 15, 25, 245});
    DrawText("COMMAND HANGAR & ENGINEERING DOCK", 50, 35, 28, {100, 220, 255, 255});

    auto& prof = SaveSystem::GetProfile();
    std::string naniteBankText = "BANKED NANITE CORES: " + std::to_string(prof.totalNanitesBanked);
    DrawText(naniteBankText.c_str(), screenW - 380, 42, 18, {100, 255, 180, 255});

    // Tab Headers
    Color tab1Col = (hangarTab == 0) ? Color{255, 220, 80, 255} : Color{140, 160, 190, 200};
    Color tab2Col = (hangarTab == 1) ? Color{255, 220, 80, 255} : Color{140, 160, 190, 200};
    DrawText("[TAB 1: MECH OUTFITTER]", 50, 80, 18, tab1Col);
    DrawText("[TAB 2: ENGINEERING SHOP]", 320, 80, 18, tab2Col);
    DrawText("(Press [TAB] to Switch)", 590, 82, 14, {160, 180, 210, 180});

    if (hangarTab == 0) {
        // TAB 1: Outfitter
        DrawText("CHASSIS FRAME [W/S]:", 50, 130, 18, {220, 230, 250, 255});
        const char* frames[] = {
            "STRIKER (Balanced kinetic platform, rapid cooling)",
            "TITAN (Heavy armor plating, colossal forward shield)",
            "PHANTOM (Ultra-light stealth frame, high crit multiplier)",
            "AEGIS (Automated point defense, reflective shields)"
        };
        for (int i = 0; i < 4; ++i) {
            Color col = (i == selectedFrame) ? Color{255, 220, 80, 255} : Color{140, 160, 190, 200};
            DrawText(frames[i], 70, 160 + i * 28, 15, col);
        }

        DrawText("PRIMARY WEAPON [A/D]:", 50, 290, 18, {220, 230, 250, 255});
        const char* prims[] = {
            "Kinetic Rail-Vulcan (High fire rate)",
            "Particle Beam Lance (Piercing plasma)",
            "Quantum Flak Cannon (Close-range spread)",
            "Arc Tempest Emitter (Chain electricity)",
            "Neutron Disruptor (Anti-shield EMP beam)"
        };
        for (int i = 0; i < 5; ++i) {
            Color col = (i == selectedPrim) ? Color{255, 220, 80, 255} : Color{140, 160, 190, 200};
            DrawText(prims[i], 70, 320 + i * 26, 15, col);
        }

        DrawText("ORDNANCE WEAPON [Z/X]:", 50, 470, 18, {220, 230, 250, 255});
        const char* ords[] = {
            "Hydra Swarm Missiles (Homing salvo)",
            "Singularity Projector (Gravity vortex)",
            "Cryo-Stasis Rail Driver (Extreme kinetic sniper)",
            "Nuclear Cluster Torpedo (Detonates into 8 bomblets)"
        };
        for (int i = 0; i < 4; ++i) {
            Color col = (i == selectedOrd) ? Color{255, 220, 80, 255} : Color{140, 160, 190, 200};
            DrawText(ords[i], 70, 500 + i * 26, 15, col);
        }
    } else {
        // TAB 2: Hangar Engineering Shop
        DrawText("PERMANENT CHASSIS ENGINEERING PERKS [UP/DOWN & ENTER]:", 50, 130, 18, {220, 230, 250, 255});

        struct UpgradeInfo {
            const char* name;
            int level;
            const char* desc;
        };

        UpgradeInfo upList[5] = {
            {"HULL COMPOSITE REINFORCEMENT", prof.upgrades.hullPlatingLevel, "+15 Max Hull Integrity per level"},
            {"COOLING TURBINES & HEAT SINKS", prof.upgrades.coolingTurbinesLevel, "+10% Faster Core Heat Dissipation"},
            {"SHIELD OVERCHARGE CAPACITORS", prof.upgrades.shieldCapacitorLevel, "+10 Max Shield Capacity per level"},
            {"SUB-MOLECULAR NANITE HARVESTERS", prof.upgrades.scavengerNanitesLevel, "+15% Extra Nanite drops per salvage"},
            {"PHASE DASH CAPACITOR COILS", prof.upgrades.dashCoolingLevel, "+8% Faster Dash Thruster Cooldown"}
        };

        for (int i = 0; i < 5; ++i) {
            int y = 175 + i * 80;
            bool isSel = (i == selectedUpgrade);

            Color borderCol = isSel ? Color{255, 230, 80, 255} : Color{60, 90, 140, 160};
            DrawRectangle(50, y, screenW - 100, 70, {20, 30, 50, 180});
            DrawRectangleLines(50, y, screenW - 100, 70, borderCol);

            DrawText(upList[i].name, 70, y + 12, 16, isSel ? Color{255, 255, 255, 255} : Color{200, 215, 235, 220});
            DrawText(upList[i].desc, 70, y + 36, 13, {150, 175, 200, 200});

            // Level pips [ ||||| ..... ]
            std::string pips = "LVL " + std::to_string(upList[i].level) + "/5  [";
            for (int p = 0; p < 5; ++p) {
                pips += (p < upList[i].level) ? "■ " : "□ ";
            }
            pips += "]";
            DrawText(pips.c_str(), screenW - 420, y + 22, 16, {100, 240, 255, 255});

            int cost = SaveSystem::GetUpgradeCost(i);
            std::string costText = (cost < 0) ? "MAX LEVEL" : ("COST: " + std::to_string(cost) + " CORES");
            Color costCol = (cost < 0) ? Color{120, 255, 140, 255} : (prof.totalNanitesBanked >= cost ? Color{255, 220, 80, 255} : Color{255, 100, 100, 255});
            DrawText(costText.c_str(), screenW - 220, y + 22, 15, costCol);
        }

        DrawText("Press [ENTER/SPACE] to Purchase Upgrade • [R] Respec & Refund All Cores", 50, screenH - 85, 15, {140, 220, 255, 255});
    }

    DrawText("Press [SPACE/ENTER] Confirm Loadout • [ESC] Return to Title", 50, screenH - 45, 16, {120, 220, 255, 255});
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

void Menu::DrawEndlessSummary(int wavesSurvived, float survivalTime, int nanitesEarned, bool extracted) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    Color bg = extracted ? Color{8, 24, 20, 240} : Color{24, 8, 12, 240};
    DrawRectangle(0, 0, screenW, screenH, bg);

    if (extracted) {
        DrawText("SECTOR EXTRACTION CONFIRMED", screenW / 2 - 220, screenH / 2 - 110, 30, {100, 255, 180, 255});
        DrawText("TACTICAL EVACUATION COMPLETED • BONUS NANITES GRANTED", screenW / 2 - 240, screenH / 2 - 70, 16, {160, 230, 200, 220});
    } else {
        DrawText("VOID DECAY TERMINATION", screenW / 2 - 190, screenH / 2 - 110, 30, {255, 80, 80, 255});
        DrawText("ENDLESS HORDE OVERWHELMED CORE SYSTEMS", screenW / 2 - 180, screenH / 2 - 70, 16, {255, 140, 140, 200});
    }

    auto& prof = SaveSystem::GetProfile();

    DrawText(("WAVES SURVIVED: " + std::to_string(wavesSurvived)).c_str(), screenW / 2 - 120, screenH / 2 - 15, 18, {255, 230, 80, 255});
    DrawText(("SURVIVAL DURATION: " + std::to_string(static_cast<int>(survivalTime)) + " SECONDS").c_str(), screenW / 2 - 140, screenH / 2 + 15, 18, {200, 220, 255, 255});
    DrawText(("NANITE CORES SECURED: " + std::to_string(nanitesEarned)).c_str(), screenW / 2 - 130, screenH / 2 + 45, 18, {100, 255, 180, 255});

    DrawText(("ALL-TIME RECORD: WAVE " + std::to_string(prof.endlessStats.maxWave) + " • " + std::to_string(static_cast<int>(prof.endlessStats.maxTimeSurvived)) + "s").c_str(), screenW / 2 - 160, screenH / 2 + 85, 15, {140, 180, 220, 200});

    DrawText("Press [SPACE/ENTER] Return to Hangar • [ESC] Title", screenW / 2 - 200, screenH / 2 + 135, 16, {200, 230, 255, 220});
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
