#include <iostream>

// Declarations of individual test suites
void RunMathTests();
void RunReactorTests();
void RunWeaponTests();
void RunCollisionTests();
void RunSaveTests();
void RunDroneTests();
void RunExpandedEngineTests();

namespace Voidfall {
    void RunRasterizerTests();
    void RunScriptingVMTests();
    void RunGalaxyEconomyTests();
    void RunMechAssemblyTests();
    void RunCampaignDialogueTests();
    void RunFlockingSquadTests();
    void RunFlightRecorderTests();
    void RunHazardsAndShaderTests();
}

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
    RunExpandedEngineTests();

    Voidfall::RunRasterizerTests();
    Voidfall::RunScriptingVMTests();
    Voidfall::RunGalaxyEconomyTests();
    Voidfall::RunMechAssemblyTests();
    Voidfall::RunCampaignDialogueTests();
    Voidfall::RunFlockingSquadTests();
    Voidfall::RunFlightRecorderTests();
    Voidfall::RunHazardsAndShaderTests();

    std::cout << "========================================" << std::endl;
    std::cout << "ALL UNIT TESTS (15 SUITES) PASSED!" << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
