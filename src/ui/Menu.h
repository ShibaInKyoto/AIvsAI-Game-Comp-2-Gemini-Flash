#pragma once

#include "core/Types.h"
#include "gameplay/UpgradeTree.h"
#include <raylib.h>
#include <vector>

namespace Voidfall {

class PlayerMech;

class Menu {
public:
    static void DrawTitle(int selectedIndex, float animTime);
    static void DrawHangar(const PlayerMech& player, int hangarTab, int selectedFrame, int selectedPrim, int selectedOrd, int selectedUpgrade);
    static void DrawUpgradeDraft(const std::vector<UpgradeCard>& cards, int selectedIdx, int naniteCores);
    static void DrawPause();
    static void DrawGameOver(int wavesCleared, int naniteCores);
    static void DrawEndlessSummary(int wavesSurvived, float survivalTime, int nanitesEarned, bool extracted);
    static void DrawVictory(int totalCores, float runTimeSeconds);
    static void DrawCodex();
};

} // namespace Voidfall
