#pragma once

#include <string>
#include <vector>

namespace Voidfall {

class PlayerMech;

struct UpgradeCard {
    std::string id;
    std::string name;
    std::string description;
    std::string category;
    int cost{50};
    bool isApplied{false};
};

class UpgradeTree {
public:
    static std::vector<UpgradeCard> GetDraftChoices(int count = 3);
    static void ApplyUpgrade(const std::string& upgradeId, PlayerMech& player);
};

} // namespace Voidfall
