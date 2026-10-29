cmake_minimum_required(VERSION 3.21)

project(VoidfallProtocolZero
    VERSION 1.0.0
    DESCRIPTION "Voidfall: Protocol Zero - Tactical Kinetic Mech Combat & Sector Extraction"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Static MSVC runtime for standalone Windows .exe
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

option(VOIDFALL_BUILD_TESTS "Build the unit test executable" ON)
option(VOIDFALL_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Fetch raylib 5.5
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

# Include directories
include_directories(
    src
    ${raylib_SOURCE_DIR}/src
)

# Source files
set(VOIDFALL_SOURCES
    src/main.cpp
    src/core/Random.cpp
    src/core/Config.cpp
    src/core/SaveSystem.cpp
    src/engine/Input.cpp
    src/engine/Camera.cpp
    src/engine/AudioSynth.cpp
    src/engine/MusicEngine.cpp
    src/engine/ParticleSystem.cpp
    src/engine/Starfield.cpp
    src/engine/PostProcessing.cpp
    src/gameplay/Reactor.cpp
    src/gameplay/Weapons.cpp
    src/gameplay/PlayerMech.cpp
    src/gameplay/Enemies.cpp
    src/gameplay/EnemyAffixes.cpp
    src/gameplay/Bosses.cpp
    src/gameplay/SectorManager.cpp
    src/gameplay/SectorMap.cpp
    src/gameplay/UpgradeTree.cpp
    src/gameplay/RelicInventory.cpp
    src/gameplay/Achievements.cpp
    src/gameplay/Drones.cpp
    src/gameplay/FiringRange.cpp
    src/ui/HUD.cpp
    src/ui/Menu.cpp
)

add_executable(VoidfallProtocolZero ${VOIDFALL_SOURCES})
target_link_libraries(VoidfallProtocolZero PRIVATE raylib)

if(WIN32)
    target_link_libraries(VoidfallProtocolZero PRIVATE winmm)
    if(MSVC)
        target_compile_options(VoidfallProtocolZero PRIVATE /W3 /utf-8)
    endif()
else()
    target_link_libraries(VoidfallProtocolZero PRIVATE m pthread dl)
    target_compile_options(VoidfallProtocolZero PRIVATE -Wall -Wextra)
endif()

# Unit Tests
if(VOIDFALL_BUILD_TESTS)
    enable_testing()

    set(TEST_SOURCES
        tests/test_main.cpp
        tests/test_math.cpp
        tests/test_reactor.cpp
        tests/test_weapons.cpp
        tests/test_collision.cpp
        tests/test_saves.cpp
        tests/test_drones.cpp
        src/core/Random.cpp
        src/core/Config.cpp
        src/core/SaveSystem.cpp
        src/engine/ParticleSystem.cpp
        src/gameplay/Reactor.cpp
        src/gameplay/Weapons.cpp
        src/gameplay/Drones.cpp
    )

    add_executable(voidfall_tests ${TEST_SOURCES})
    target_link_libraries(voidfall_tests PRIVATE raylib)
    if(NOT WIN32)
        target_link_libraries(voidfall_tests PRIVATE m pthread dl)
    endif()

    add_test(NAME AllUnitTests COMMAND voidfall_tests)
endif()
