#include "gameplay/Reactor.h"
#include <cassert>
#include <iostream>
#include <cmath>

using namespace Voidfall;

void RunReactorTests() {
    std::cout << "[TEST] Running Reactor & Heat Economy Tests..." << std::endl;

    Reactor r;
    assert(r.GetProfile() == PowerProfile::BALANCED);

    // Profile Switching
    r.SetProfile(PowerProfile::ASSAULT);
    assert(r.GetProfile() == PowerProfile::ASSAULT);
    assert(r.GetAllocation().weapons == 0.70f);

    r.SetProfile(PowerProfile::BULWARK);
    assert(r.GetAllocation().shields == 0.70f);

    r.SetProfile(PowerProfile::INTERCEPTOR);
    assert(r.GetAllocation().engines == 0.70f);

    // Heat Addition
    r.AddHeat(50.0f);
    assert(r.GetHeat() == 50.0f);
    assert(r.IsInSweetSpot()); // 50% is inside [45%, 75%]

    // Active Purge in sweet spot
    bool sweetSpotHit = false;
    bool purged = r.TryActivePurge(sweetSpotHit);
    assert(purged == true);
    assert(sweetSpotHit == true);
    assert(r.GetHeat() == 0.0f);

    // Overheat Lockout
    r.AddHeat(110.0f);
    assert(r.IsOverheated() == true);
    assert(r.GetHeat() == 100.0f);

    // During lockout, active purge is rejected
    purged = r.TryActivePurge(sweetSpotHit);
    assert(purged == false);

    // Update through cooldown
    r.Update(2.5f);
    assert(r.IsOverheated() == false);
    assert(r.GetHeat() == 0.0f);

    std::cout << " -> Reactor & Heat Economy Tests Passed!" << std::endl;
}
