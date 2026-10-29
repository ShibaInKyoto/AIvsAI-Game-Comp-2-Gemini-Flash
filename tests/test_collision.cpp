#include "core/Types.h"
#include <cassert>
#include <iostream>

using namespace Voidfall;

void RunCollisionTests() {
    std::cout << "[TEST] Running Collision Detection Tests..." << std::endl;

    Circle c1{{0.0f, 0.0f}, 10.0f};
    Circle c2{{15.0f, 0.0f}, 10.0f};
    Circle c3{{30.0f, 0.0f}, 10.0f};

    assert(c1.Intersects(c2) == true);  // dist 15 <= 20
    assert(c1.Intersects(c3) == false); // dist 30 > 20

    assert(c1.Contains({5.0f, 5.0f}) == true);
    assert(c1.Contains({12.0f, 0.0f}) == false);

    RectAABB r1{0.0f, 0.0f, 20.0f, 20.0f};
    RectAABB r2{10.0f, 10.0f, 20.0f, 20.0f};
    RectAABB r3{30.0f, 30.0f, 10.0f, 10.0f};

    assert(r1.Intersects(r2) == true);
    assert(r1.Intersects(r3) == false);

    std::cout << " -> Collision Detection Tests Passed!" << std::endl;
}
