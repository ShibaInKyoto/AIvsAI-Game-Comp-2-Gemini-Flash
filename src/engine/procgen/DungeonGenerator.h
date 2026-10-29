#pragma once

#include "core/Types.h"
#include <vector>
#include <cstdint>

namespace Voidfall::ProcGen {

struct Room {
    int id{0};
    RectAABB bounds;
    bool isConnected{false};
    bool isStartRoom{false};
    bool isBossRoom{false};
};

struct Corridor {
    Vec2 start;
    Vec2 end;
    float width{12.0f};
};

struct StationLayout {
    std::vector<Room> rooms;
    std::vector<Corridor> corridors;
    std::vector<Vec2> spawnPoints;
    Vec2 extractionPoint;
};

class StationDungeonGenerator {
public:
    explicit StationDungeonGenerator(uint64_t seed = 42);

    void SetSeed(uint64_t seed) { m_seed = seed; }
    [[nodiscard]] StationLayout Generate(float areaWidth = 2400.0f, float areaHeight = 2400.0f, int roomCount = 12);

private:
    void SplitBSP(std::vector<RectAABB>& subZones, const RectAABB& current, int depth);
    void ConnectRooms(StationLayout& layout);

    uint64_t m_seed{42};
};

} // namespace Voidfall::ProcGen
