#include "ui/HUD.h"
#include "gameplay/PlayerMech.h"
#include "gameplay/Bosses.h"
#include "gameplay/SectorManager.h"
#include "gameplay/Achievements.h"
#include <cmath>
#include <algorithm>
#include <string>

namespace Voidfall {

void HUD::Draw(const PlayerMech& player, const BossEntity& boss, const SectorManager& sector) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Achievement Notification Banner
    if (AchievementSystem::GetNotificationTimer() > 0.0f) {
        const auto& note = AchievementSystem::GetRecentNotification();
        int bannerW = 420;
        int bannerH = 40;
        int bx = (screenW - bannerW) / 2;
        int by = 12;

        DrawRectangle(bx, by, bannerW, bannerH, {15, 25, 40, 240});
        DrawRectangleLines(bx, by, bannerW, bannerH, {255, 215, 0, 255});
        DrawText(note.c_str(), bx + 20, by + 12, 15, {255, 230, 80, 255});
    }

    // 1. Top-Left: Player Hull and Shield Vitals
    DrawVitals(player, 30, 30);

    // 2. Bottom-Left: Tri-Power Reactor Distribution
    DrawReactorDisplay(player, 30, screenH - 140);

    // 3. Bottom-Center: Thermal Heat Gauge & Active Sweet Spot Flush
    DrawHeatGauge(player, screenW / 2 - 180, screenH - 60, 360, 24);

    // 4. Bottom-Right: Weapons Loadout & Ordnance status
    DrawWeapons(player, screenW - 240, screenH - 120);

    // 5. Top-Right: Radar Minimap & Nanite Cores & Sector Indicator
    DrawRadar(player, boss, sector, screenW - 85, 85, 55);

    int cores = player.GetNaniteCores();
    DrawText(("CORES: " + std::to_string(cores)).c_str(), screenW - 230, 35, 18, {100, 240, 255, 255});
    std::string waveStr = sector.IsBossWave() ? "SECTOR BOSS ENGAGED" : ("WAVE " + std::to_string(sector.GetWave()) + " / " + std::to_string(sector.GetMaxWaves()));
    DrawText(waveStr.c_str(), screenW - 230, 60, 15, {220, 230, 245, 200});

    // 6. Top-Center: Boss HP Bar if engaged
    if (boss.IsActive() && !boss.IsDefeated()) {
        DrawBossBar(boss, screenW);
    }
}

void HUD::DrawVitals(const PlayerMech& player, int x, int y) {
    int barW = 220;
    int barH = 14;

    // Shield Bar
    float shieldPct = std::clamp(player.GetShield() / player.GetMaxShield(), 0.0f, 1.0f);
    DrawRectangle(x, y, barW, barH, {20, 30, 45, 220});
    DrawRectangle(x, y, static_cast<int>(barW * shieldPct), barH, {60, 200, 255, 255});
    DrawRectangleLines(x, y, barW, barH, {100, 220, 255, 180});
    DrawText("SHIELD", x + 6, y + 2, 10, {255, 255, 255, 220});

    // Hull Bar
    float hullPct = std::clamp(player.GetHull() / player.GetMaxHull(), 0.0f, 1.0f);
    DrawRectangle(x, y + 20, barW, barH, {45, 20, 20, 220});
    DrawRectangle(x, y + 20, static_cast<int>(barW * hullPct), barH, {255, 70, 70, 255});
    DrawRectangleLines(x, y + 20, barW, barH, {255, 100, 100, 180});
    DrawText("HULL", x + 6, y + 22, 10, {255, 255, 255, 220});

    // Hull Number
    DrawText((std::to_string(static_cast<int>(player.GetHull())) + " / " + std::to_string(static_cast<int>(player.GetMaxHull()))).c_str(), x + barW + 12, y + 20, 14, {255, 200, 200, 255});
}

void HUD::DrawReactorDisplay(const PlayerMech& player, int x, int y) {
    const auto& alloc = player.GetReactor().GetAllocation();

    DrawRectangle(x, y, 160, 95, {15, 22, 35, 200});
    DrawRectangleLines(x, y, 160, 95, {60, 100, 160, 150});

    DrawText("REACTOR ROUTING", x + 10, y + 8, 12, {180, 220, 255, 255});

    // Power Profiles & percentage bars
    auto DrawBar = [&](int lineY, const char* label, float pct, Color col, const char* hotkey) {
        DrawText(label, x + 10, lineY, 11, col);
        DrawRectangle(x + 75, lineY + 2, 55, 8, {30, 40, 55, 255});
        DrawRectangle(x + 75, lineY + 2, static_cast<int>(55 * pct), 8, col);
        DrawText(hotkey, x + 138, lineY, 11, {160, 180, 200, 200});
    };

    DrawBar(y + 28, "WPN [1]", alloc.weapons, {255, 120, 60, 255}, "70%");
    DrawBar(y + 48, "SHD [2]", alloc.shields, {60, 200, 255, 255}, "70%");
    DrawBar(y + 68, "ENG [3]", alloc.engines, {120, 255, 100, 255}, "70%");
}

void HUD::DrawHeatGauge(const PlayerMech& player, int x, int y, int width, int height) {
    const auto& reactor = player.GetReactor();
    float heatRatio = reactor.GetHeatRatio();

    // Heat background
    DrawRectangle(x, y, width, height, {20, 25, 35, 220});

    // Sweet Spot Zone Highlight [45% - 75%]
    int ssStart = static_cast<int>(width * 0.45f);
    int ssWidth = static_cast<int>(width * 0.30f);
    DrawRectangle(x + ssStart, y, ssWidth, height, {180, 220, 60, 70});
    DrawRectangleLines(x + ssStart, y, ssWidth, height, {220, 255, 80, 180});

    // Current Heat Fill
    Color heatCol = {255, 180, 40, 255};
    if (reactor.IsOverheated()) {
        heatCol = {255, 40, 40, 255};
    } else if (reactor.IsInSweetSpot()) {
        heatCol = {220, 255, 60, 255};
    }
    DrawRectangle(x, y, static_cast<int>(width * heatRatio), height, heatCol);
    DrawRectangleLines(x, y, width, height, {80, 100, 130, 200});

    // Text status
    if (reactor.IsOverheated()) {
        DrawText("CRITICAL OVERHEAT! WEAPONS LOCKED", x + width / 2 - 135, y - 22, 14, {255, 50, 50, 255});
    } else if (reactor.IsInSweetSpot()) {
        DrawText(">> PRESS [F] FOR FLASH PURGE <<", x + width / 2 - 120, y - 22, 14, {220, 255, 80, 255});
    } else {
        DrawText("CORE HEAT GAUGE", x + width / 2 - 60, y - 18, 12, {180, 190, 210, 200});
    }
}

void HUD::DrawWeapons(const PlayerMech& player, int x, int y) {
    WeaponDefinition prim = WeaponFactory::GetDefinition(player.GetPrimaryWeapon());
    WeaponDefinition ord = WeaponFactory::GetDefinition(player.GetOrdnanceWeapon());

    DrawRectangle(x, y, 210, 80, {15, 22, 35, 200});
    DrawRectangleLines(x, y, 210, 80, {60, 100, 160, 150});

    DrawText("L-ARM [LMB]:", x + 10, y + 12, 11, {160, 180, 200, 255});
    DrawText(prim.name.c_str(), x + 10, y + 26, 12, prim.projectileColor);

    DrawText("R-ARM [RMB]:", x + 10, y + 46, 11, {160, 180, 200, 255});
    DrawText(ord.name.c_str(), x + 10, y + 60, 12, ord.projectileColor);
}

void HUD::DrawBossBar(const BossEntity& boss, int screenWidth) {
    int barW = 500;
    int barH = 18;
    int x = (screenWidth - barW) / 2;
    int y = 45;

    DrawText(boss.GetName().c_str(), x + 10, y - 22, 15, {255, 100, 100, 255});

    float hpPct = std::clamp(boss.GetHpRatio(), 0.0f, 1.0f);
    DrawRectangle(x, y, barW, barH, {30, 20, 25, 220});
    DrawRectangle(x, y, static_cast<int>(barW * hpPct), barH, {255, 50, 70, 255});
    DrawRectangleLines(x, y, barW, barH, {255, 120, 120, 200});

    std::string phaseStr = "PHASE " + std::to_string(boss.GetPhase());
    DrawText(phaseStr.c_str(), x + barW - 65, y - 20, 13, {255, 200, 100, 255});
}

void HUD::DrawRadar(const PlayerMech& player, const BossEntity& boss, const SectorManager& sector, int x, int y, int radius) {
    Vector2 radarCenter = {static_cast<float>(x), static_cast<float>(y)};
    DrawCircleV(radarCenter, static_cast<float>(radius), {15, 20, 30, 200});
    DrawCircleLines(x, y, radius, {60, 120, 180, 180});

    // Radar blip scale
    float scale = static_cast<float>(radius) / sector.GetArenaRadius();
    Vec2 pPos = player.GetPosition();

    // Player blip in center
    DrawCircle(x, y, 3, {80, 240, 255, 255});

    // Boss blip if active
    if (boss.IsActive() && !boss.IsDefeated()) {
        Vec2 rel = boss.GetPosition() - pPos;
        float bX = x + rel.x * scale;
        float bY = y + rel.y * scale;
        DrawCircle(static_cast<int>(bX), static_cast<int>(bY), 5, {255, 40, 40, 255});
    }
}

} // namespace Voidfall
