#include "core/math/MathCore.h"
#include "core/math/Splines.h"
#include "core/math/Noise.h"
#include "core/math/Geometry.h"
#include "core/ecs/Registry.h"
#include "engine/physics/Quadtree.h"
#include "engine/physics/RigidBody.h"
#include "engine/audio/DSPCore.h"
#include "engine/procgen/WeaponGenerator.h"
#include "core/SaveSystem.h"
#include <iostream>
#include <cassert>

using namespace Voidfall;

void RunMathCoreTests() {
    std::cout << "[TEST] Running MathCore Tests...\n";
    Math::Vec3 a{1.0f, 2.0f, 3.0f};
    Math::Vec3 b{4.0f, 5.0f, 6.0f};

    assert(Math::ApproxEqual(a.Dot(b), 32.0f));
    Math::Vec3 c = a.Cross(b);
    assert(Math::ApproxEqual(c.x, -3.0f));
    assert(Math::ApproxEqual(c.y, 6.0f));
    assert(Math::ApproxEqual(c.z, -3.0f));

    Math::Mat3 rot = Math::Mat3::Rotation(Math::HALF_PI);
    Math::Vec3 pt{1.0f, 0.0f, 1.0f};
    Math::Vec3 transformed = rot.TransformPoint(pt);
    assert(Math::ApproxEqual(transformed.x, 0.0f, 0.001f));
    assert(Math::ApproxEqual(transformed.y, 1.0f, 0.001f));
    std::cout << "  -> MathCore Tests Passed!\n";
}

void RunSplineTests() {
    std::cout << "[TEST] Running Splines Tests...\n";
    Vec2 p0{0.0f, 0.0f};
    Vec2 p1{10.0f, 20.0f};
    Vec2 p2{20.0f, 0.0f};

    Vec2 mid = Math::Spline2D::EvaluateBezierQuadratic(p0, p1, p2, 0.5f);
    assert(Math::ApproxEqual(mid.x, 10.0f));
    assert(Math::ApproxEqual(mid.y, 10.0f));
    std::cout << "  -> Splines Tests Passed!\n";
}

void RunNoiseTests() {
    std::cout << "[TEST] Running Noise Tests...\n";
    Math::NoiseGenerator noise(42);
    float n1 = noise.Perlin2D(12.5f, 34.2f);
    float n2 = noise.Perlin2D(12.5f, 34.2f);
    assert(n1 == n2); // Determinism
    assert(n1 >= -1.0f && n1 <= 1.0f);

    float fbm = noise.FBm2D(5.0f, 7.0f, 4);
    assert(fbm >= -1.0f && fbm <= 1.0f);
    std::cout << "  -> Noise Tests Passed!\n";
}

void RunGeometryTests() {
    std::cout << "[TEST] Running Geometry Tests...\n";
    Math::ConvexPolygon polyA;
    polyA.vertices = {{-10.0f, -10.0f}, {10.0f, -10.0f}, {10.0f, 10.0f}, {-10.0f, 10.0f}};

    Math::ConvexPolygon polyB;
    polyB.vertices = {{5.0f, 5.0f}, {25.0f, 5.0f}, {25.0f, 25.0f}, {5.0f, 25.0f}};

    Vec2 mtv;
    bool hit = Math::Geometry::IntersectsSAT(polyA, polyB, &mtv);
    assert(hit);
    assert(mtv.Length() > 0.0f);

    Math::ConvexPolygon polyC;
    polyC.vertices = {{100.0f, 100.0f}, {120.0f, 100.0f}, {120.0f, 120.0f}, {100.0f, 120.0f}};
    assert(!Math::Geometry::IntersectsSAT(polyA, polyC));
    std::cout << "  -> Geometry Tests Passed!\n";
}

void RunECSTests() {
    std::cout << "[TEST] Running ECS Tests...\n";
    struct TransformComp { Vec2 pos; };
    struct HealthComp { float hp; };

    ECS::Registry registry(100);
    ECS::Entity e1 = registry.CreateEntity();
    ECS::Entity e2 = registry.CreateEntity();

    registry.AddComponent<TransformComp>(e1, Vec2{10.0f, 20.0f});
    registry.AddComponent<HealthComp>(e1, 100.0f);
    registry.AddComponent<TransformComp>(e2, Vec2{50.0f, 60.0f});

    assert(registry.HasComponent<TransformComp>(e1));
    assert(registry.HasComponent<HealthComp>(e1));
    assert(registry.HasComponent<TransformComp>(e2));
    assert(!registry.HasComponent<HealthComp>(e2));

    assert(registry.GetComponent<TransformComp>(e1).pos.x == 10.0f);
    assert(registry.GetComponent<HealthComp>(e1).hp == 100.0f);

    registry.DestroyEntity(e1);
    assert(!registry.IsValid(e1));
    assert(!registry.HasComponent<TransformComp>(e1));
    std::cout << "  -> ECS Tests Passed!\n";
}

void RunPhysicsTests() {
    std::cout << "[TEST] Running Physics Tests...\n";
    Physics::Quadtree qtree(RectAABB{-1000.0f, -1000.0f, 2000.0f, 2000.0f});
    qtree.Insert(1, Circle{Vec2{10.0f, 10.0f}, 5.0f});
    qtree.Insert(2, Circle{Vec2{500.0f, 500.0f}, 5.0f});

    std::vector<uint32_t> results;
    qtree.Query(Circle{Vec2{12.0f, 12.0f}, 15.0f}, results);
    assert(results.size() == 1);
    assert(results[0] == 1);

    // RigidBody2D Collision Resolution
    Physics::RigidBody2D bodyA(Physics::BodyType::DYNAMIC, 10.0f, 15.0f);
    bodyA.SetPosition({0.0f, 0.0f});
    bodyA.SetVelocity({100.0f, 0.0f});

    Physics::RigidBody2D bodyB(Physics::BodyType::DYNAMIC, 10.0f, 15.0f);
    bodyB.SetPosition({25.0f, 0.0f});
    bodyB.SetVelocity({-100.0f, 0.0f});

    auto manifold = Physics::RigidBody2D::CheckCircleCircle(bodyA, bodyB);
    assert(manifold.hasCollision);
    assert(manifold.penetration > 0.0f);

    Physics::RigidBody2D::ResolveCollision(bodyA, bodyB, manifold);
    assert(bodyA.GetVelocity().x < 0.0f); // Bounced left
    assert(bodyB.GetVelocity().x > 0.0f); // Bounced right
    std::cout << "  -> Physics Tests Passed!\n";
}

void RunDSPAudioTests() {
    std::cout << "[TEST] Running DSP Audio Tests...\n";
    Audio::BiquadFilter filter;
    filter.Configure(Audio::FilterType::LOWPASS, 1000.0f, 0.707f, 44100.0f);

    float sample = filter.Process(1.0f);
    assert(!std::isnan(sample));
    assert(!std::isinf(sample));

    Audio::DelayLine delay(1000);
    delay.SetDelay(0.01f, 44100.0f);
    float dOut = delay.Process(0.5f);
    assert(!std::isnan(dOut));
    std::cout << "  -> DSP Audio Tests Passed!\n";
}

void RunHangarProgressionTests() {
    std::cout << "[TEST] Running Hangar Progression Tests...\n";
    auto& prof = SaveSystem::GetProfile();
    prof.totalNanitesBanked = 1000;
    prof.upgrades = HangarUpgrades();

    int cost0 = SaveSystem::GetUpgradeCost(0);
    assert(cost0 == 150);

    bool bought = SaveSystem::PurchaseUpgrade(0);
    assert(bought);
    assert(prof.upgrades.hullPlatingLevel == 1);
    assert(prof.totalNanitesBanked == 850);

    SaveSystem::ResetUpgrades();
    assert(prof.upgrades.hullPlatingLevel == 0);
    assert(prof.totalNanitesBanked == 1000); // 100% refunded
    std::cout << "  -> Hangar Progression Tests Passed!\n";
}

void RunExpandedEngineTests() {
    RunMathCoreTests();
    RunSplineTests();
    RunNoiseTests();
    RunGeometryTests();
    RunECSTests();
    RunPhysicsTests();
    RunDSPAudioTests();
    RunHangarProgressionTests();
}
