#pragma once

#include "core/Types.h"
#include <raylib.h>
#include <vector>

namespace Voidfall {

struct Projectile {
    Vec2 pos{0.0f, 0.0f};
    Vec2 vel{0.0f, 0.0f};
    float damage{10.0f};
    float radius{4.0f};
    float life{1.5f};
    float maxLife{1.5f};
    Color color{255, 255, 255, 255};
    bool isEnemy{false};
    int pierceCount{0};
    bool isHoming{false};
    Vec2 targetPos{0.0f, 0.0f};
    float homingStrength{0.0f};
    bool isExplosive{false};
    float explosionRadius{0.0f};
    bool isCluster{false};
    int submunitionCount{0};
    bool isDisruptor{false};
};

struct WeaponDefinition {
    WeaponType type;
    std::string name;
    std::string description;
    float baseDamage{10.0f};
    float fireRate{0.15f}; // seconds per shot
    float heatCost{5.0f};
    float projectileSpeed{700.0f};
    float projectileRadius{4.0f};
    Color projectileColor{255, 200, 50, 255};
    int projectilesPerShot{1};
    float spreadAngle{0.0f}; // radians
    int pierceCount{0};
    bool isHoming{false};
    bool isExplosive{false};
    float explosionRadius{0.0f};
    bool isCluster{false};
    int submunitionCount{0};
    bool isDisruptor{false};
};

class WeaponFactory {
public:
    static WeaponDefinition GetDefinition(WeaponType type);
};

class ProjectileManager {
public:
    ProjectileManager(size_t reserveCount = 1000);

    void Update(float dt);
    void Draw() const;
    void Clear();

    void SpawnProjectile(const Projectile& p);
    [[nodiscard]] std::vector<Projectile>& GetProjectiles() { return m_projectiles; }
    [[nodiscard]] const std::vector<Projectile>& GetProjectiles() const { return m_projectiles; }

private:
    std::vector<Projectile> m_projectiles;
};

} // namespace Voidfall
