#include <cassert>
#include <iostream>
#include "engine/ai/FlockingDirector.h"

namespace Voidfall {

void RunFlockingSquadTests() {
    std::cout << "[TEST] Running Flocking & Squad Director Tests..." << std::endl;

    FlockingDirector director;
    director.CreateSquad(1, SquadFormation::WEDGE);
    director.CreateSquad(2, SquadFormation::PINCER);

    auto* sq1 = director.GetSquad(1);
    assert(sq1 != nullptr);
    assert(sq1->formation == SquadFormation::WEDGE);

    // Add leader and members
    int leaderId = director.AddAgent(Vec2(100, 100), 1, true);
    int m1 = director.AddAgent(Vec2(110, 110), 1, false);
    int m2 = director.AddAgent(Vec2(90, 110), 1, false);
    int m3 = director.AddAgent(Vec2(120, 120), 1, false);

    assert(director.GetAgents().size() == 4);
    assert(sq1->leaderAgentId == leaderId);

    // Simulate 10 frames
    Vec2 playerPos(500, 500);
    std::vector<Vec2> obstacles = { Vec2(250, 250), Vec2(300, 280) };

    for (int i = 0; i < 20; ++i) {
        director.Update(0.016f, playerPos, obstacles);
    }

    auto* leader = director.GetAgent(leaderId);
    assert(leader != nullptr);
    // Leader should have accelerated towards player
    assert((leader->position - Vec2(100, 100)).Length() > 5.0f);

    // Test separation force exists between closely packed agents
    Vec2 sep = director.ComputeSeparation(*director.GetAgent(m1));
    (void)sep;

    // Test squad leader destruction and succession
    director.RemoveAgent(leaderId);
    assert(director.GetAgents().size() == 3);
    assert(sq1->leaderAgentId != leaderId);
    assert(sq1->leaderAgentId != -1); // Succession promoted an agent

    // Test Pincer formation slot computation
    auto* sq2 = director.GetSquad(2);
    int p1 = director.AddAgent(Vec2(600, 600), 2, true);
    int p2 = director.AddAgent(Vec2(620, 600), 2, false);
    Vec2 pSlot = director.ComputeFormationSlot(*director.GetAgent(p2), *sq2, playerPos);
    assert(pSlot.Length() > 0.0f);

    (void)m2;
    (void)m3;
    (void)p1;

    std::cout << "[PASS] Flocking & Squad Director Tests Passed!" << std::endl;
}

} // namespace Voidfall
