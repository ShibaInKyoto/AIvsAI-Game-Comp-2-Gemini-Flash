#include "gameplay/SectorMap.h"
#include "core/Random.h"
#include <raylib.h>
#include <cmath>

namespace Voidfall {

SectorMap::SectorMap() {
    Generate(1);
}

void SectorMap::Generate(int sectorIndex) {
    m_sectorIndex = sectorIndex;
    m_nodes.clear();
    m_currentLayer = 0;
    m_currentNodeId = 0;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    int idCounter = 0;
    int layerCount = 5;

    // Layer 0: Starting Jump Point (1 node)
    MapNode startNode;
    startNode.id = idCounter++;
    startNode.layer = 0;
    startNode.type = NodeType::STANDARD_COMBAT;
    startNode.name = "Perimeter Beacon";
    startNode.description = "Forward vanguard drones scanning the jump zone.";
    startNode.screenPos = {180.0f, screenH * 0.5f};
    startNode.isAccessible = true;
    m_nodes.push_back(startNode);

    // Layers 1 to 3: Branching Path (2-3 nodes per layer)
    std::vector<std::vector<int>> layerNodeIds(layerCount);
    layerNodeIds[0].push_back(0);

    for (int l = 1; l <= 3; ++l) {
        int nodesInLayer = Random::RangeI(2, 3);
        float x = 180.0f + l * ((screenW - 360.0f) / 4.0f);

        for (int n = 0; n < nodesInLayer; ++n) {
            MapNode node;
            node.id = idCounter++;
            node.layer = l;

            float yStep = screenH / static_cast<float>(nodesInLayer + 1);
            node.screenPos = {x, yStep * (n + 1) + Random::RangeF(-20.0f, 20.0f)};

            // Procedural node types
            float roll = Random::Normal();
            if (roll < 0.45f) {
                node.type = NodeType::STANDARD_COMBAT;
                node.name = "Patrol Fleet";
                node.description = "Hostile drone squadrons patrolling resource asteroids.";
            } else if (roll < 0.70f) {
                node.type = NodeType::DERELICT_SALVAGE;
                node.name = "Abandoned Frigate";
                node.description = "Drifting derelict hull with salvageable nanite cores.";
            } else if (roll < 0.85f) {
                node.type = NodeType::ANOMALY_EVENT;
                node.name = "Spacetime Anomaly";
                node.description = "Unstable tachyon readings with unpredictable outcome.";
            } else {
                node.type = NodeType::ELITE_COMBAT;
                node.name = "Elite Heavy Cruiser";
                node.description = "Armored champion mech guarding high-tier tech relics.";
            }

            layerNodeIds[l].push_back(node.id);
            m_nodes.push_back(node);
        }

        // Connect previous layer to this layer
        for (int prevId : layerNodeIds[l - 1]) {
            // Pick at least one connection, plus optional second
            int targetA = layerNodeIds[l][Random::RangeI(0, static_cast<int>(layerNodeIds[l].size() - 1))];
            m_nodes[prevId].connectedNodeIds.push_back(targetA);

            // Connect nearest node too
            for (int currId : layerNodeIds[l]) {
                if (std::abs(m_nodes[prevId].screenPos.y - m_nodes[currId].screenPos.y) < 140.0f) {
                    bool alreadyConnected = false;
                    for (int c : m_nodes[prevId].connectedNodeIds) {
                        if (c == currId) { alreadyConnected = true; break; }
                    }
                    if (!alreadyConnected) {
                        m_nodes[prevId].connectedNodeIds.push_back(currId);
                    }
                }
            }
        }
    }

    // Layer 4: Final Boss Flagship Node (1 node)
    MapNode bossNode;
    bossNode.id = idCounter++;
    bossNode.layer = 4;
    bossNode.type = NodeType::SECTOR_BOSS;
    bossNode.name = (sectorIndex == 1) ? "Goliath-IV Flagship" : (sectorIndex == 2) ? "Aethel-Core Nexus" : "Protocol Zero Sovereign";
    bossNode.description = "Sector Commander Capital Vessel. Defeat to initiate extraction.";
    bossNode.screenPos = {screenW - 180.0f, screenH * 0.5f};
    m_nodes.push_back(bossNode);

    // Connect all layer 3 nodes to Boss
    for (int l3Id : layerNodeIds[3]) {
        m_nodes[l3Id].connectedNodeIds.push_back(bossNode.id);
    }
}

const MapNode* SectorMap::GetCurrentNode() const {
    if (m_currentNodeId >= 0 && m_currentNodeId < static_cast<int>(m_nodes.size())) {
        return &m_nodes[m_currentNodeId];
    }
    return nullptr;
}

bool SectorMap::IsAtBoss() const {
    const auto* curr = GetCurrentNode();
    return curr && curr->type == NodeType::SECTOR_BOSS;
}

void SectorMap::SelectNode(int nodeId) {
    if (nodeId < 0 || nodeId >= static_cast<int>(m_nodes.size())) return;
    if (!m_nodes[nodeId].isAccessible) return;

    m_currentNodeId = nodeId;
    m_currentLayer = m_nodes[nodeId].layer;
    m_nodes[nodeId].isCompleted = true;

    // Update accessible nodes for next jump
    for (auto& n : m_nodes) {
        n.isAccessible = false;
    }
    for (int nextId : m_nodes[nodeId].connectedNodeIds) {
        m_nodes[nextId].isAccessible = true;
    }

    if (m_nodes[nodeId].type == NodeType::ANOMALY_EVENT || m_nodes[nodeId].type == NodeType::DERELICT_SALVAGE) {
        GenerateAnomalyEvent();
    }
}

void SectorMap::GenerateAnomalyEvent() {
    m_currentEvent = AnomalyEvent();

    if (Random::Bool(0.5f)) {
        m_currentEvent.title = "DERELICT RESEARCH STATION OMEGA";
        m_currentEvent.scenarioText = "A silent science station drifts in orbit around a dying pulsar. Scanners detect pressurized air and faint reactor hums in the containment lab.";

        SalvageChoice c1;
        c1.text = "1. Force open containment vaults (Risky)";
        c1.outcomeDesc = "Vault breached! Acquired 85 Nanite Cores, but radiation leaked (-20 Hull).";
        c1.naniteReward = 85;
        c1.hullCost = 20.0f;
        m_currentEvent.choices.push_back(c1);

        SalvageChoice c2;
        c2.text = "2. Siphon emergency cooling tanks";
        c2.outcomeDesc = "Coolant fluid harvested. Hull integrity repaired (+35 Hull).";
        c2.hullRepair = 35.0f;
        m_currentEvent.choices.push_back(c2);

        SalvageChoice c3;
        c3.text = "3. Bypass station and maintain stealth";
        c3.outcomeDesc = "Station left undisturbed. Power levels stabilized.";
        m_currentEvent.choices.push_back(c3);
    } else {
        m_currentEvent.title = "STRANDED REBEL MERCENARY";
        m_currentEvent.scenarioText = "An escape pod broadcasts an encrypted pilot distress ping. The pilot offers custom chassis tech in exchange for emergency fuel.";

        SalvageChoice c1;
        c1.text = "1. Offer 40 Nanite Cores for Weapon Matrix Calibration";
        c1.outcomeDesc = "Weapon calibration complete. Projectile damage increased!";
        c1.naniteCost = 40;
        c1.givesRelic = true;
        m_currentEvent.choices.push_back(c1);

        SalvageChoice c2;
        c2.text = "2. Salvage pod components by force";
        c2.outcomeDesc = "Pod dismantled for 50 Nanite Cores.";
        c2.naniteReward = 50;
        m_currentEvent.choices.push_back(c2);

        SalvageChoice c3;
        c3.text = "3. Ignore transmission and continue jump";
        c3.outcomeDesc = "Signal fades into the cosmic background.";
        m_currentEvent.choices.push_back(c3);
    }
}

void SectorMap::Draw(int selectedNodeId) const {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Background panel
    DrawRectangle(0, 0, screenW, screenH, {6, 10, 18, 240});
    DrawText("TACTICAL SECTOR NAVIGATION MATRIX", 60, 45, 26, {100, 220, 255, 255});
    DrawText(("SECTOR " + std::to_string(m_sectorIndex) + " HYPERLANE NETWORK • SELECT JUMP VECTOR").c_str(), 60, 78, 15, {160, 190, 220, 200});

    // Draw connection lines
    for (const auto& node : m_nodes) {
        for (int nextId : node.connectedNodeIds) {
            const auto& nextNode = m_nodes[nextId];
            Color lineCol = {40, 70, 110, 140};
            if (node.isCompleted && nextNode.isAccessible) {
                lineCol = {80, 220, 255, 220};
            }
            DrawLineEx({node.screenPos.x, node.screenPos.y}, {nextNode.screenPos.x, nextNode.screenPos.y}, 2.0f, lineCol);
        }
    }

    // Draw nodes
    for (const auto& node : m_nodes) {
        Vector2 c = {node.screenPos.x, node.screenPos.y};
        float radius = (node.type == NodeType::SECTOR_BOSS) ? 28.0f : 20.0f;

        Color bgCol = {15, 25, 40, 255};
        Color ringCol = {60, 90, 130, 255};

        if (node.type == NodeType::SECTOR_BOSS) {
            ringCol = {255, 60, 60, 255};
        } else if (node.type == NodeType::ELITE_COMBAT) {
            ringCol = {255, 140, 40, 255};
        } else if (node.type == NodeType::DERELICT_SALVAGE) {
            ringCol = {100, 255, 140, 255};
        } else if (node.type == NodeType::ANOMALY_EVENT) {
            ringCol = {220, 80, 255, 255};
        }

        if (node.isCompleted) {
            ringCol = {80, 100, 120, 180};
            bgCol = {10, 15, 22, 255};
        } else if (node.isAccessible) {
            // Highlight pulsing ring
            ringCol = {255, 255, 255, 255};
        }

        if (node.id == selectedNodeId) {
            DrawCircleLines(static_cast<int>(c.x), static_cast<int>(c.y), radius + 6.0f, {255, 230, 80, 255});
        }

        DrawCircleV(c, radius, bgCol);
        DrawCircleLines(static_cast<int>(c.x), static_cast<int>(c.y), radius, ringCol);

        // Icon text
        const char* symbol = "!";
        if (node.type == NodeType::STANDARD_COMBAT) symbol = "C";
        if (node.type == NodeType::ELITE_COMBAT) symbol = "E";
        if (node.type == NodeType::DERELICT_SALVAGE) symbol = "$";
        if (node.type == NodeType::ANOMALY_EVENT) symbol = "?";
        if (node.type == NodeType::SECTOR_BOSS) symbol = "B";

        DrawText(symbol, static_cast<int>(c.x) - 5, static_cast<int>(c.y) - 8, 16, ringCol);
    }

    // Node preview tooltip at bottom
    if (selectedNodeId >= 0 && selectedNodeId < static_cast<int>(m_nodes.size())) {
        const auto& sel = m_nodes[selectedNodeId];
        int panelY = screenH - 120;
        DrawRectangle(60, panelY, screenW - 120, 85, {12, 18, 30, 230});
        DrawRectangleLines(60, panelY, screenW - 120, 85, {60, 110, 180, 200});

        DrawText(sel.name.c_str(), 80, panelY + 14, 18, {255, 255, 255, 255});
        DrawText(sel.description.c_str(), 80, panelY + 42, 14, {180, 200, 230, 220});

        if (sel.isAccessible) {
            DrawText("[ENTER/SPACE/CLICK] INITIATE HYPERSPACE JUMP", screenW - 480, panelY + 30, 15, {100, 255, 140, 255});
        } else if (sel.isCompleted) {
            DrawText("SECTOR CLEARED", screenW - 280, panelY + 30, 15, {120, 140, 160, 200});
        } else {
            DrawText("OUT OF RANGE / INACCESSIBLE", screenW - 350, panelY + 30, 15, {255, 80, 80, 200});
        }
    }
}

} // namespace Voidfall
