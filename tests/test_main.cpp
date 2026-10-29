#include <iostream>

// Declarations of individual test suites
void RunMathTests();
void RunReactorTests();
void RunWeaponTests();
void RunCollisionTests();
void RunSaveTests();
void RunDroneTests();

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "VOIDFALL: PROTOCOL ZERO - UNIT TEST SUITE" << std::endl;
    std::cout << "========================================" << std::endl;

    RunMathTests();
    RunReactorTests();
    RunWeaponTests();
    RunCollisionTests();
    RunSaveTests();
    RunDroneTests();

    std::cout << "========================================" << std::endl;
    std::cout << "ALL UNIT TESTS (6 SUITES) PASSED!" << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
