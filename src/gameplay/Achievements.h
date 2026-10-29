#pragma once

#include <string>
#include <vector>

namespace Voidfall {

struct Achievement {
    std::string id;
    std::string name;
    std::string description;
    bool isUnlocked{false};
};

class AchievementSystem {
public:
    static void Initialize();
    static void Unlock(const std::string& id);
    static bool IsUnlocked(const std::string& id);
    static const std::vector<Achievement>& GetAchievements() { return s_achievements; }
    static const std::string& GetRecentNotification() { return s_recentNotification; }
    static float GetNotificationTimer() { return s_notificationTimer; }
    static void Update(float dt);

private:
    static std::vector<Achievement> s_achievements;
    static std::string s_recentNotification;
    static float s_notificationTimer;
};

} // namespace Voidfall
