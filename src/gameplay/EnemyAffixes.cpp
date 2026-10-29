#include "gameplay/EnemyAffixes.h"
#include "core/Random.h"
#include <raylib.h>

namespace Voidfall {

EnemyAffix AffixSystem::RollRandomAffix(float probability) {
    if (!Random::Bool(probability)) return EnemyAffix::NONE;

    int roll = Random::RangeI(1, 5);
    return static_cast<EnemyAffix>(roll);
}

std::string AffixSystem::GetAffixName(EnemyAffix affix) {
    switch (affix) {
        case EnemyAffix::SHIELDED: return "SHIELDED";
        case EnemyAffix::VOLATILE: return "VOLATILE";
        case EnemyAffix::TELEPORTING: return "WARP-PHASE";
        case EnemyAffix::BERSERK: return "BERSERK";
        case EnemyAffix::VAMPIRIC: return "LEECH-CORE";
        default: return "";
    }
}

Color AffixSystem::GetAffixColor(EnemyAffix affix) {
    switch (affix) {
        case EnemyAffix::SHIELDED: return {60, 200, 255, 255};
        case EnemyAffix::VOLATILE: return {255, 120, 40, 255};
        case EnemyAffix::TELEPORTING: return {200, 80, 255, 255};
        case EnemyAffix::BERSERK: return {255, 40, 40, 255};
        case EnemyAffix::VAMPIRIC: return {120, 255, 100, 255};
        default: return {255, 255, 255, 255};
    }
}

} // namespace Voidfall
