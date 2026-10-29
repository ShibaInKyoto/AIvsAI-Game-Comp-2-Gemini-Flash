#pragma once

#include <vector>
#include <memory>
#include "Mesh.h"
#include "Rasterizer.h"
#include "engine/animation/Skeleton.h"

namespace Voidfall {

using namespace Math;

enum class MechChassisClass {
    VANGUARD_STRIKER,
    TITAN_DREADNOUGHT,
    GHOST_INFILTRATOR
};

struct MechPart3D {
    std::string name;
    int jointId{-1};
    Mesh3D mesh;
    Mat4 offsetTransform{Mat4::Identity()};
};

class ProceduralMech3D {
public:
    ProceduralMech3D(MechChassisClass chassisClass = MechChassisClass::VANGUARD_STRIKER);

    void Generate(MechChassisClass chassisClass);
    void Update(float dt);
    void Render(Rasterizer& rasterizer, const Mat4& worldTransform, bool wireframe = false);

    Skeleton& GetSkeleton() noexcept { return m_skeleton; }
    const Skeleton& GetSkeleton() const noexcept { return m_skeleton; }
    AnimationPlayer& GetAnimPlayer() noexcept { return m_animPlayer; }

    void AimTorso(const Vec3& targetWorldPos, const Mat4& mechWorldTransform, float dt);
    void WalkCycle(float speed, float dt);

private:
    void BuildSkeleton();
    void BuildMeshes(MechChassisClass chassisClass);
    void BuildWalkAnimation();

    MechChassisClass m_class{MechChassisClass::VANGUARD_STRIKER};
    Skeleton m_skeleton;
    AnimationPlayer m_animPlayer;
    std::vector<MechPart3D> m_parts;
    std::unique_ptr<AnimationClip> m_walkClip;

    int m_rootJoint{-1};
    int m_pelvisJoint{-1};
    int m_torsoJoint{-1};
    int m_cockpitJoint{-1};
    int m_leftShoulderJoint{-1};
    int m_rightShoulderJoint{-1};
    int m_leftUpperLegJoint{-1};
    int m_leftLowerLegJoint{-1};
    int m_leftFootJoint{-1};
    int m_rightUpperLegJoint{-1};
    int m_rightLowerLegJoint{-1};
    int m_rightFootJoint{-1};

    float m_walkTimer{0.0f};
};

} // namespace Voidfall
