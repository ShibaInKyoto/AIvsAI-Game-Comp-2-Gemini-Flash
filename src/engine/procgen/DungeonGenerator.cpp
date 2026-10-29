#include "engine/procgen/DungeonGenerator.h"
#include "core/Random.h"
#include <algorithm>
#include <cmath>

namespace Voidfall::ProcGen {

StationDungeonGenerator::StationDungeonGenerator(uint64_t seed) : m_seed(seed) {}

void StationDungeonGenerator::SplitBSP(std::vector<RectAABB>& subZones, const RectAABB& current, int depth) {
    if (depth <= 0 || current.width < 320.0f || current.height < 320.0f) {
        subZones.push_back(current);
        return;
    }

    bool splitH = (current.height > current.width);
    if (std::abs(current.width - current.height) < 40.0f) {
        splitH = Random::RangeF(0.0f, 1.0f) > 0.5f;
    }

    if (splitH) {
        float splitY = current.y + current.height * Random::RangeF(0.4f, 0.6f);
        RectAABB top{current.x, current.y, current.width, splitY - current.y};
        RectAABB bottom{current.x, splitY, current.width, current.y + current.height - splitY};
        SplitBSP(subZones, top, depth - 1);
        SplitBSP(subZones, bottom, depth - 1);
    } else {
        float splitX = current.x + current.width * Random::RangeF(0.4f, 0.6f);
        RectAABB left{current.x, current.y, splitX - current.x, current.height};
        RectAABB right{splitX, current.y, current.x + current.width - splitX, current.height};
        SplitBSP(subZones, left, depth - 1);
        SplitBSP(subZones, right, depth - 1);
    }
}

StationLayout StationDungeonGenerator::Generate(float areaWidth, float areaHeight, int roomCount) {
    StationLayout layout;
    Random::Seed(m_seed);

    RectAABB rootBounds{-areaWidth * 0.5f, -areaHeight * 0.5f, areaWidth, areaHeight};
    std::vector<RectAABB> zones;
    SplitBSP(zones, rootBounds, 4);

    int id = 0;
    for (const auto& zone : zones) {
        if (static_cast<int>(layout.rooms.size()) >= roomCount) break;

        // Inscribe room with padding inside zone
        float padX = zone.width * 0.15f;
        float padY = zone.height * 0.15f;
        float rW = zone.width - padX * 2.0f;
        float rH = zone.height - padY * 2.0f;

        if (rW > 120.0f && rH > 120.0f) {
            Room r;
            r.id = id++;
            r.bounds = RectAABB{zone.x + padX, zone.y + padY, rW, rH};
            layout.rooms.push_back(r);
        }
    }

    if (layout.rooms.empty()) return layout;

    // Identify start room and furthest boss room
    layout.rooms[0].isStartRoom = true;
    Vec2 startCenter{
        layout.rooms[0].bounds.x + layout.rooms[0].bounds.width * 0.5f,
        layout.rooms[0].bounds.y + layout.rooms[0].bounds.height * 0.5f
    };

    float maxDistSq = 0.0f;
    size_t bossIdx = 0;
    for (size_t i = 1; i < layout.rooms.size(); ++i) {
        Vec2 c{
            layout.rooms[i].bounds.x + layout.rooms[i].bounds.width * 0.5f,
            layout.rooms[i].bounds.y + layout.rooms[i].bounds.height * 0.5f
        };
        float dSq = startCenter.DistanceSq(c);
        if (dSq > maxDistSq) {
            maxDistSq = dSq;
            bossIdx = i;
        }
    }
    layout.rooms[bossIdx].isBossRoom = true;
    layout.extractionPoint = Vec2{
        layout.rooms[bossIdx].bounds.x + layout.rooms[bossIdx].bounds.width * 0.5f,
        layout.rooms[bossIdx].bounds.y + layout.rooms[bossIdx].bounds.height * 0.5f
    };

    // Connect rooms using Minimum Spanning Tree / nearest neighbor corridors
    ConnectRooms(layout);

    // Generate enemy and relic spawn points in non-start rooms
    for (size_t i = 1; i < layout.rooms.size(); ++i) {
        Vec2 center{
            layout.rooms[i].bounds.x + layout.rooms[i].bounds.width * 0.5f,
            layout.rooms[i].bounds.y + layout.rooms[i].bounds.height * 0.5f
        };
        layout.spawnPoints.push_back(center + Vec2{-30.0f, -30.0f});
        layout.spawnPoints.push_back(center + Vec2{30.0f, 30.0f});
    }

    return layout;
}

void StationDungeonGenerator::ConnectRooms(StationLayout& layout) {
    if (layout.rooms.size() < 2) return;

    // Simple nearest-neighbor spanning tree
    std::vector<bool> inTree(layout.rooms.size(), false);
    inTree[0] = true;

    for (size_t step = 1; step < layout.rooms.size(); ++step) {
        float minDistSq = 999999999.0f;
        size_t bestU = 0;
        size_t bestV = 0;

        for (size_t u = 0; u < layout.rooms.size(); ++u) {
            if (!inTree[u]) continue;
            Vec2 cu{
                layout.rooms[u].bounds.x + layout.rooms[u].bounds.width * 0.5f,
                layout.rooms[u].bounds.y + layout.rooms[u].bounds.height * 0.5f
            };

            for (size_t v = 0; v < layout.rooms.size(); ++v) {
                if (inTree[v]) continue;
                Vec2 cv{
                    layout.rooms[v].bounds.x + layout.rooms[v].bounds.width * 0.5f,
                    layout.rooms[v].bounds.y + layout.rooms[v].bounds.height * 0.5f
                };

                float dSq = cu.DistanceSq(cv);
                if (dSq < minDistSq) {
                    minDistSq = dSq;
                    bestU = u;
                    bestV = v;
                }
            }
        }

        inTree[bestV] = true;

        Vec2 p1{
            layout.rooms[bestU].bounds.x + layout.rooms[bestU].bounds.width * 0.5f,
            layout.rooms[bestU].bounds.y + layout.rooms[bestU].bounds.height * 0.5f
        };
        Vec2 p2{
            layout.rooms[bestV].bounds.x + layout.rooms[bestV].bounds.width * 0.5f,
            layout.rooms[bestV].bounds.y + layout.rooms[bestV].bounds.height * 0.5f
        };

        // L-shaped corridor
        Vec2 corner{p2.x, p1.y};
        layout.corridors.push_back(Corridor{p1, corner, 14.0f});
        layout.corridors.push_back(Corridor{corner, p2, 14.0f});
    }
}

} // namespace Voidfall::ProcGen
