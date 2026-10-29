#pragma once

#include <string>
#include <vector>

namespace Voidfall {

struct Synergy {
    std::string name;
    std::string description;
    std::string relicA;
    std::string relicB;
    bool isActive{false};
};

class RelicInventory {
public:
    RelicInventory();

    void AddRelic(const std::string& relicId);
    [[nodiscard]] bool HasRelic(const std::string& relicId) const;
    [[nodiscard]] const std::vector<std::string>& GetRelics() const { return m_relics; }
    [[nodiscard]] const std::vector<Synergy>& GetActiveSynergies() const { return m_activeSynergies; }

    void Clear();

private:
    void CheckSynergies();

    std::vector<std::string> m_relics;
    std::vector<Synergy> m_activeSynergies;
};

} // namespace Voidfall
