#include "gameplay/Drones.h"
#include <cassert>
#include <iostream>

using namespace Voidfall;

void RunDroneTests() {
    std::cout << "[TEST] Running Drone Companion Tests..." << std::endl;

    DroneManager dm;
    assert(dm.GetDroneCount() == 0);

    dm.AddDrone(DroneType::ATTACK_DRONE);
    assert(dm.GetDroneCount() == 1);

    dm.AddDrone(DroneType::SHIELD_DRONE);
    dm.AddDrone(DroneType::EMP_SENTRY);
    dm.AddDrone(DroneType::ATTACK_DRONE);
    assert(dm.GetDroneCount() == 4);

    // Limit capped at 4
    dm.AddDrone(DroneType::ATTACK_DRONE);
    assert(dm.GetDroneCount() == 4);

    dm.Clear();
    assert(dm.GetDroneCount() == 0);

    std::cout << " -> Drone Companion Tests Passed!" << std::endl;
}
