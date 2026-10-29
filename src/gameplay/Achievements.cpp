#include "gameplay/Achievements.h"
#include "engine/AudioSynth.h"
#include <algorithm>

namespace Voidfall {

std::vector<Achievement> AchievementSystem::s_achievements;
std::string AchievementSystem::s_recentNotification = "";
float AchievementSystem::s_notificationTimer = 0.0f;

void AchievementSystem::Initialize() {
    s_achievements = {
        {"FIRST_CONTACT", "First Contact", "Clear your first hostile drone wave.", false},
        {"FLASH_PURGE_EXPERT", "Flash Purge Expert", "Execute a perfect active thermal purge in the sweet spot.", false},
        {"TITAN_SLAYER", "Titan Slayer", "Destroy the Goliath-IV Dread Fortress.", false},
        {"ORBITAL_BREACH", "Orbital Breach", "Neutralize the Aethel-Core Orbital Carrier.", false},
        {"PROTOCOL_ZERO_EXTINCTION", "Protocol Zero Extinction", "Defeat the Sovereign Titan and complete the campaign.", false},
        {"NANITE_HOARDER", "Nanite Hoarder", "Accumulate over 300 Nanite Cores in a single expedition.", false},
        {"DEFLECTION_EXPERT", "Kinetic Deflector", "Parry 10 incoming projectiles with directional shields.", false},
        {"RESONANCE_CASCADE", "Resonance Cascade", "Form your first active relic synergy evolution.", false}
    };
}

void AchievementSystem::Unlock(const std::string& id) {
    for (auto& a : s_achievements) {
        if (a.id == id && !a.isUnlocked) {
            a.isUnlocked = true;
            s_recentNotification = "ACHIEVEMENT UNLOCKED: " + a.name;
            s_notificationTimer = 3.5f;
            AudioSynth::Play(SoundID::UPGRADE_SELECT, 1.3f, 1.25f);
            break;
        }
    }
}

bool AchievementSystem::IsUnlocked(const std::string& id) {
    for (const auto& a : s_achievements) {
        if (a.id == id) return a.isUnlocked;
    }
    return false;
}

void AchievementSystem::Update(float dt) {
    if (s_notificationTimer > 0.0f) {
        s_notificationTimer -= dt;
        if (s_notificationTimer <= 0.0f) {
            s_recentNotification = "";
        }
    }
}

} // namespace Voidfall
