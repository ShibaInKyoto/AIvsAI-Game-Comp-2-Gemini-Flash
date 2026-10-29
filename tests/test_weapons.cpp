#include "gameplay/Weapons.h"
#include <cassert>
#include <iostream>

using namespace Voidfall;

void RunWeaponTests() {
    std::cout << "[TEST] Running Weapons & Projectiles Tests..." << std::endl;

    auto vulcan = WeaponFactory::GetDefinition(WeaponType::KINETIC_VULCAN);
    assert(vulcan.baseDamage > 0.0f);
    assert(vulcan.projectileSpeed > 500.0f);

    auto lance = WeaponFactory::GetDefinition(WeaponType::PARTICLE_LANCE);
    assert(lance.pierceCount >= 2);

    auto missiles = WeaponFactory::GetDefinition(WeaponType::SWARM_MISSILE);
    assert(missiles.isHoming == true);
    assert(missiles.isExplosive == true);

    // Projectile Manager update
    ProjectileManager pm;
    Projectile p;
    p.pos = {0.0f, 0.0f};
    p.vel = {100.0f, 0.0f};
    p.life = 1.0f;
    pm.SpawnProjectile(p);

    assert(pm.GetProjectiles().size() == 1);
    pm.Update(0.1f);
    assert(pm.GetProjectiles()[0].pos.x == 10.0f);

    std::cout << " -> Weapons & Projectiles Tests Passed!" << std::endl;
}
