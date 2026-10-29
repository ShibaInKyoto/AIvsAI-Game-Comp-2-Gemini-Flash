#pragma once

#include "core/Types.h"
#include <raylib.h>

namespace Voidfall {

class PlayerMech;
class BossEntity;
class SectorManager;

class HUD {
public:
    static void Draw(const PlayerMech& player, const BossEntity& boss, const SectorManager& sector);

private:
    static void DrawReactorDisplay(const PlayerMech& player, int x, int y);
    static void DrawHeatGauge(const PlayerMech& player, int x, int y, int width, int height);
    static void DrawVitals(const PlayerMech& player, int x, int y);
    static void DrawWeapons(const PlayerMech& player, int x, int y);
    static void DrawBossBar(const BossEntity& boss, int screenWidth);
    static void DrawRadar(const PlayerMech& player, const BossEntity& boss, const SectorManager& sector, int x, int y, int radius);
};

} // namespace Voidfall
