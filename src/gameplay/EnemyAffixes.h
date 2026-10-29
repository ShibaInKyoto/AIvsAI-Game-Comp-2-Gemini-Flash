#pragma once

#include "core/Types.h"
#include <raylib.h>
#include <string>
#include <vector>

namespace Voidfall {

enum class EnemyAffix {
    NONE,
    SHIELDED,
    VOLATILE,
    TELEPORTING,
    BERSERK,
    VAMPIRIC
};

class AffixSystem {
public:
    static EnemyAffix RollRandomAffix(float probability = 0.35f);
    static std::string GetAffixName(EnemyAffix affix);
    static Color GetAffixColor(EnemyAffix affix);
};

} // namespace Voidfall
