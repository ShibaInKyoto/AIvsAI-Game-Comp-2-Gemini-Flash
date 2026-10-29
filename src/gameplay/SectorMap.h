#pragma once

#include "core/Types.h"
#include <string>
#include <vector>

namespace Voidfall {

enum class NodeType {
    STANDARD_COMBAT,
    ELITE_COMBAT,
    DERELICT_SALVAGE,
    REPAIR_DEPOT,
    ANOMALY_EVENT,
    SECTOR_BOSS
};

struct MapNode {
    int id{0};
    int layer{0}; // Column in graph (0 to 4)
    NodeType type{NodeType::STANDARD_COMBAT};
    std::string name{"Hostile Patrol"};
    std::string description{"Standard automated drone defense perimeter."};
    Vec2 screenPos{0.0f, 0.0f};
    std::vector<int> connectedNodeIds;
    bool isCompleted{false};
    bool isAccessible{false};
};

struct SalvageChoice {
    std::string text;
    std::string outcomeDesc;
    int naniteCost{0};
    int naniteReward{0};
    float hullCost{0.0f};
    float hullRepair{0.0f};
    bool givesRelic{false};
};

struct AnomalyEvent {
    std::string title;
    std::string scenarioText;
    std::vector<SalvageChoice> choices;
};

class SectorMap {
public:
    SectorMap();

    void Generate(int sectorIndex);
    void Draw(int selectedNodeId) const;
    void SelectNode(int nodeId);

    [[nodiscard]] const std::vector<MapNode>& GetNodes() const { return m_nodes; }
    [[nodiscard]] const MapNode* GetCurrentNode() const;
    [[nodiscard]] int GetCurrentLayer() const { return m_currentLayer; }
    [[nodiscard]] bool IsAtBoss() const;

    // Interactive event resolution
    [[nodiscard]] const AnomalyEvent& GetCurrentEvent() const { return m_currentEvent; }
    void GenerateAnomalyEvent();

private:
    std::vector<MapNode> m_nodes;
    int m_currentNodeId{0};
    int m_currentLayer{0};
    int m_sectorIndex{1};

    AnomalyEvent m_currentEvent{};
};

} // namespace Voidfall
