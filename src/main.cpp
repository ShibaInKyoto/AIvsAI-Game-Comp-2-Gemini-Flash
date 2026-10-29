#include <raylib.h>
#include "core/Config.h"
#include "core/Random.h"
#include "core/Types.h"
#include "core/SaveSystem.h"
#include "engine/Input.h"
#include "engine/Camera.h"
#include "engine/AudioSynth.h"
#include "engine/MusicEngine.h"
#include "engine/ParticleSystem.h"
#include "engine/Starfield.h"
#include "engine/PostProcessing.h"
#include "gameplay/PlayerMech.h"
#include "gameplay/Enemies.h"
#include "gameplay/Bosses.h"
#include "gameplay/SectorManager.h"
#include "gameplay/SectorMap.h"
#include "gameplay/UpgradeTree.h"
#include "gameplay/RelicInventory.h"
#include "gameplay/Achievements.h"
#include "gameplay/Drones.h"
#include "gameplay/FiringRange.h"
#include "ui/HUD.h"
#include "ui/Menu.h"

using namespace Voidfall;

int main(int argc, char** argv) {
    // 1. Parse command line arguments
    ConfigManager::ParseCommandLine(argc, argv);
    auto& cfg = ConfigManager::Get();

    // 2. Window Configuration
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(cfg.screenWidth, cfg.screenHeight, "VOIDFALL: PROTOCOL ZERO");
    SetTargetFPS(cfg.targetFPS);

    // 3. Audio, Shaders & Save Profile
    Random::Seed(1337);
    SaveSystem::Load();
    AchievementSystem::Initialize();
    AudioSynth::Initialize();
    MusicEngine::Initialize();
    PostProcessing::Initialize(cfg.screenWidth, cfg.screenHeight);

    // 4. Instantiate Core Systems
    GameCamera camera;
    Starfield starfield(450);
    ParticleSystem particles(2500);
    ProjectileManager projManager(1200);
    PlayerMech player;
    DroneManager drones;
    EnemyManager enemies;
    BossEntity boss;
    SectorManager sector;
    SectorMap sectorMap;
    FiringRange firingRange;
    RelicInventory relics;

    GameState state = cfg.autotestMode ? GameState::SECTOR_EXPEDITION : GameState::TITLE_SCREEN;

    int titleSelection = 0;
    int hangarFrame = 0;
    int hangarPrim = 0;
    int hangarOrd = 0;

    int mapSelectedNode = 0;
    std::vector<UpgradeCard> draftCards;
    int draftSelection = 0;

    float animTime = 0.0f;
    float runTime = 0.0f;
    float hitAberration = 0.0f;
    int totalWavesCleared = 0;
    int frameCount = 0;

    // Fixed timestep accumulator
    const float dt = 1.0f / 60.0f;

    while (!WindowShouldClose()) {
        frameCount++;
        animTime += dt;
        AchievementSystem::Update(dt);

        if (hitAberration > 0.0f) {
            hitAberration = std::max(0.0f, hitAberration - dt * 2.5f);
        }

        // Autotest completion check
        if (cfg.autotestMode && frameCount >= cfg.autotestFrames) {
            if (!cfg.screenshotPath.empty()) {
                TakeScreenshot(cfg.screenshotPath.c_str());
            }
            break;
        }

        // Update Input state
        Input::Update(camera.GetPosition(), camera.GetZoom());
        const auto& input = Input::Get();

        // State Machine Handling
        switch (state) {
            case GameState::TITLE_SCREEN: {
                MusicEngine::SetIntensity(MusicIntensity::AMBIENT);
                if (input.menuUp) { titleSelection = (titleSelection + 5) % 6; AudioSynth::Play(SoundID::POWER_SHIFT); }
                if (input.menuDown) { titleSelection = (titleSelection + 1) % 6; AudioSynth::Play(SoundID::POWER_SHIFT); }
                if (input.menuConfirm) {
                    AudioSynth::Play(SoundID::UPGRADE_SELECT);
                    if (titleSelection == 0) {
                        // Launch Campaign via Sector Map
                        player.Reset(static_cast<MechFrame>(hangarFrame));
                        player.SetPrimaryWeapon(static_cast<WeaponType>(hangarPrim));
                        player.SetOrdnanceWeapon(static_cast<WeaponType>(hangarOrd + 4));
                        sectorMap.Generate(1);
                        mapSelectedNode = 0;
                        drones.Clear();
                        drones.AddDrone(DroneType::ATTACK_DRONE);
                        relics.Clear();
                        runTime = 0.0f;
                        SaveSystem::GetProfile().totalRunsAttempted++;
                        SaveSystem::Save();
                        state = GameState::SECTOR_MAP;
                    } else if (titleSelection == 1) {
                        // Titan Colosseum (Direct Boss Rush)
                        player.Reset(static_cast<MechFrame>(hangarFrame));
                        sector.InitSector(SectorType::ASTEROID_OUTPOST, 1);
                        boss.Init(BossID::GOLIATH_IV, {0.0f, -380.0f});
                        drones.Clear();
                        drones.AddDrone(DroneType::ATTACK_DRONE);
                        drones.AddDrone(DroneType::SHIELD_DRONE);
                        enemies.Clear();
                        projManager.Clear();
                        particles.Clear();
                        runTime = 0.0f;
                        MusicEngine::SetIntensity(MusicIntensity::BOSS_BATTLE);
                        state = GameState::SECTOR_EXPEDITION;
                    } else if (titleSelection == 2) {
                        // Firing Range Sandbox
                        player.Reset(static_cast<MechFrame>(hangarFrame));
                        firingRange.Init();
                        enemies.Clear();
                        projManager.Clear();
                        particles.Clear();
                        state = GameState::FIRING_RANGE;
                    } else if (titleSelection == 3) {
                        state = GameState::HANGAR_LOADOUT;
                    } else if (titleSelection == 4) {
                        state = GameState::CODEX;
                    } else if (titleSelection == 5) {
                        CloseWindow();
                        return 0;
                    }
                }
                break;
            }

            case GameState::SECTOR_MAP: {
                MusicEngine::SetIntensity(MusicIntensity::AMBIENT);

                // Navigate accessible nodes
                if (input.menuRight || input.menuDown) {
                    mapSelectedNode = (mapSelectedNode + 1) % static_cast<int>(sectorMap.GetNodes().size());
                    AudioSynth::Play(SoundID::POWER_SHIFT);
                } else if (input.menuLeft || input.menuUp) {
                    mapSelectedNode = (mapSelectedNode + static_cast<int>(sectorMap.GetNodes().size()) - 1) % static_cast<int>(sectorMap.GetNodes().size());
                    AudioSynth::Play(SoundID::POWER_SHIFT);
                }

                if (input.menuConfirm) {
                    const auto& nodes = sectorMap.GetNodes();
                    if (mapSelectedNode >= 0 && mapSelectedNode < static_cast<int>(nodes.size()) && nodes[mapSelectedNode].isAccessible) {
                        sectorMap.SelectNode(mapSelectedNode);
                        AudioSynth::Play(SoundID::DASH);

                        if (nodes[mapSelectedNode].type == NodeType::ANOMALY_EVENT || nodes[mapSelectedNode].type == NodeType::DERELICT_SALVAGE) {
                            state = GameState::ANOMALY_EVENT;
                        } else {
                            // Enter Combat Encounter
                            sector.InitSector(SectorType::ASTEROID_OUTPOST, sectorMap.GetCurrentLayer() + 1);
                            enemies.Clear();
                            projManager.Clear();
                            particles.Clear();
                            if (nodes[mapSelectedNode].type == NodeType::SECTOR_BOSS) {
                                boss.Init(BossID::GOLIATH_IV, {0.0f, -400.0f});
                            }
                            state = GameState::SECTOR_EXPEDITION;
                        }
                    }
                }
                break;
            }

            case GameState::ANOMALY_EVENT: {
                const auto& ev = sectorMap.GetCurrentEvent();
                int choiceIdx = -1;
                if (IsKeyPressed(KEY_ONE) && ev.choices.size() >= 1) choiceIdx = 0;
                if (IsKeyPressed(KEY_TWO) && ev.choices.size() >= 2) choiceIdx = 1;
                if (IsKeyPressed(KEY_THREE) && ev.choices.size() >= 3) choiceIdx = 2;

                if (choiceIdx >= 0) {
                    const auto& ch = ev.choices[choiceIdx];
                    player.AddNaniteCores(ch.naniteReward - ch.naniteCost);
                    if (ch.hullCost > 0.0f) player.TakeDamage(ch.hullCost, {0, 1}, particles);
                    if (ch.hullRepair > 0.0f) player.Heal(ch.hullRepair);
                    if (ch.givesRelic) relics.AddRelic("OVERCLOCKED_MAGNETS");
                    AudioSynth::Play(SoundID::UPGRADE_SELECT);
                    state = GameState::SECTOR_MAP;
                }
                break;
            }

            case GameState::FIRING_RANGE: {
                if (input.pause) {
                    state = GameState::TITLE_SCREEN;
                    break;
                }
                player.Update(dt, projManager, particles);
                firingRange.Update(dt, player, projManager, particles);
                projManager.Update(dt);
                particles.Update(dt);
                camera.Update(player.GetPosition(), input.mouseWorldPos, dt);
                break;
            }

            case GameState::HANGAR_LOADOUT: {
                if (input.menuUp) hangarFrame = (hangarFrame + 3) % 4;
                if (input.menuDown) hangarFrame = (hangarFrame + 1) % 4;
                if (input.menuLeft) hangarPrim = (hangarPrim + 3) % 4;
                if (input.menuRight) hangarPrim = (hangarPrim + 1) % 4;

                if (IsKeyPressed(KEY_Z)) hangarOrd = (hangarOrd + 2) % 3;
                if (IsKeyPressed(KEY_X)) hangarOrd = (hangarOrd + 1) % 3;

                if (input.menuBack) {
                    state = GameState::TITLE_SCREEN;
                } else if (input.menuConfirm) {
                    AudioSynth::Play(SoundID::UPGRADE_SELECT);
                    player.SetFrame(static_cast<MechFrame>(hangarFrame));
                    player.SetPrimaryWeapon(static_cast<WeaponType>(hangarPrim));
                    player.SetOrdnanceWeapon(static_cast<WeaponType>(hangarOrd + 4));
                    state = GameState::TITLE_SCREEN;
                }
                break;
            }

            case GameState::CODEX: {
                if (input.menuBack || input.menuConfirm) {
                    state = GameState::TITLE_SCREEN;
                }
                break;
            }

            case GameState::SECTOR_EXPEDITION: {
                if (input.pause) {
                    state = GameState::PAUSED;
                    break;
                }

                runTime += dt;

                // Adjust dynamic music intensity based on combat state
                if (boss.IsActive() && !boss.IsDefeated()) {
                    MusicEngine::SetIntensity(MusicIntensity::BOSS_BATTLE);
                } else if (enemies.GetActiveCount() > 5) {
                    MusicEngine::SetIntensity(MusicIntensity::COMBAT_HIGH);
                } else {
                    MusicEngine::SetIntensity(MusicIntensity::COMBAT_NORMAL);
                }

                // Update gameplay entities
                player.Update(dt, projManager, particles);
                drones.Update(dt, player.GetPosition(), enemies, projManager, particles);
                enemies.Update(dt, player, projManager, particles);
                boss.Update(dt, player, projManager, particles);
                sector.Update(dt, player, enemies, boss, projManager, particles);
                projManager.Update(dt);
                particles.Update(dt);
                starfield.Update(player.GetVelocity(), dt);

                // Update camera centered on player
                camera.Update(player.GetPosition(), input.mouseWorldPos, dt);

                // Check Sector Completion
                if (sector.IsSectorComplete()) {
                    totalWavesCleared += sector.GetMaxWaves();
                    SaveSystem::AddNanites(player.GetNaniteCores());
                    if (sector.GetSectorIndex() >= 3) {
                        SaveSystem::GetProfile().totalVictories++;
                        SaveSystem::Save();
                        state = GameState::VICTORY_SCREEN;
                    } else {
                        draftCards = UpgradeTree::GetDraftChoices(3);
                        draftSelection = 0;
                        state = GameState::UPGRADE_DRAFT;
                    }
                }

                // Check Player Death
                if (!player.IsAlive()) {
                    camera.AddTrauma(0.85f);
                    hitAberration = 1.0f;
                    SaveSystem::AddNanites(player.GetNaniteCores());
                    state = GameState::GAME_OVER;
                }
                break;
            }

            case GameState::UPGRADE_DRAFT: {
                MusicEngine::SetIntensity(MusicIntensity::AMBIENT);
                if (input.menuLeft) { draftSelection = (draftSelection + static_cast<int>(draftCards.size()) - 1) % static_cast<int>(draftCards.size()); AudioSynth::Play(SoundID::POWER_SHIFT); }
                if (input.menuRight) { draftSelection = (draftSelection + 1) % static_cast<int>(draftCards.size()); AudioSynth::Play(SoundID::POWER_SHIFT); }

                if (input.menuConfirm && !draftCards.empty()) {
                    const auto& card = draftCards[draftSelection];
                    if (player.GetNaniteCores() >= card.cost) {
                        player.AddNaniteCores(-card.cost);
                        UpgradeTree::ApplyUpgrade(card.id, player);
                        relics.AddRelic(card.id);
                        AudioSynth::Play(SoundID::UPGRADE_SELECT);
                    }
                    sector.ProceedToNextSector();
                    state = GameState::SECTOR_MAP;
                }
                break;
            }

            case GameState::PAUSED: {
                if (input.pause || input.menuConfirm) {
                    state = GameState::SECTOR_EXPEDITION;
                }
                break;
            }

            case GameState::GAME_OVER: {
                MusicEngine::SetIntensity(MusicIntensity::AMBIENT);
                if (input.menuConfirm) {
                    state = GameState::TITLE_SCREEN;
                }
                break;
            }

            case GameState::VICTORY_SCREEN: {
                MusicEngine::SetIntensity(MusicIntensity::AMBIENT);
                if (input.menuConfirm) {
                    state = GameState::TITLE_SCREEN;
                }
                break;
            }

            default:
                break;
        }

        // Render Frame into Post-Processing Buffer
        PostProcessing::Begin();
        ClearBackground({8, 12, 20, 255});

        if (state == GameState::SECTOR_EXPEDITION || state == GameState::PAUSED || state == GameState::GAME_OVER) {
            camera.BeginMode();

            // Background Parallax Starfield & Sector Objects
            starfield.Draw(camera.GetPosition());
            sector.Draw();
            projManager.Draw();
            enemies.Draw();
            boss.Draw();
            drones.Draw();
            player.Draw();
            particles.Draw();

            camera.EndMode();

            // Cockpit Futuristic HUD
            HUD::Draw(player, boss, sector);

            if (state == GameState::PAUSED) {
                Menu::DrawPause();
            } else if (state == GameState::GAME_OVER) {
                Menu::DrawGameOver(totalWavesCleared, player.GetNaniteCores());
            }
        } else if (state == GameState::FIRING_RANGE) {
            camera.BeginMode();
            starfield.Draw(camera.GetPosition());
            firingRange.Draw();
            projManager.Draw();
            player.Draw();
            particles.Draw();
            camera.EndMode();

            HUD::Draw(player, boss, sector);
        } else if (state == GameState::SECTOR_MAP) {
            sectorMap.Draw(mapSelectedNode);
        } else if (state == GameState::ANOMALY_EVENT) {
            const auto& ev = sectorMap.GetCurrentEvent();
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {8, 12, 22, 245});
            DrawText(ev.title.c_str(), 80, 80, 24, {100, 220, 255, 255});
            DrawText(ev.scenarioText.c_str(), 80, 130, 15, {180, 200, 230, 220});

            for (size_t c = 0; c < ev.choices.size(); ++c) {
                DrawText(ev.choices[c].text.c_str(), 80, 220 + static_cast<int>(c) * 45, 16, {255, 230, 80, 255});
            }
        } else if (state == GameState::TITLE_SCREEN) {
            Menu::DrawTitle(titleSelection, animTime);
        } else if (state == GameState::HANGAR_LOADOUT) {
            Menu::DrawHangar(player, hangarFrame, hangarPrim, hangarOrd);
        } else if (state == GameState::UPGRADE_DRAFT) {
            Menu::DrawUpgradeDraft(draftCards, draftSelection, player.GetNaniteCores());
        } else if (state == GameState::VICTORY_SCREEN) {
            Menu::DrawVictory(player.GetNaniteCores(), runTime);
        } else if (state == GameState::CODEX) {
            Menu::DrawCodex();
        }

        PostProcessing::End();

        // Output Post-Processing to Screen with CRT / chromatic aberration
        BeginDrawing();
        ClearBackground(BLACK);
        PostProcessing::Draw(hitAberration);
        EndDrawing();
    }

    // 5. Cleanup
    PostProcessing::Shutdown();
    MusicEngine::Shutdown();
    AudioSynth::Shutdown();
    CloseWindow();
    return 0;
}
