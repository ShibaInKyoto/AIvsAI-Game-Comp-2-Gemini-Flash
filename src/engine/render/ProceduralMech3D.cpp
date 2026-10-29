#include "ProceduralMech3D.h"
#include <cmath>

namespace Voidfall {

using namespace Math;

ProceduralMech3D::ProceduralMech3D(MechChassisClass chassisClass) {
    Generate(chassisClass);
}

void ProceduralMech3D::Generate(MechChassisClass chassisClass) {
    m_class = chassisClass;
    m_parts.clear();
    BuildSkeleton();
    BuildMeshes(chassisClass);
    BuildWalkAnimation();
}

void ProceduralMech3D::BuildSkeleton() {
    m_skeleton = Skeleton();

    // Joint tree
    m_rootJoint = m_skeleton.AddJoint("Root", -1, Vec3(0, 0, 0));
    m_pelvisJoint = m_skeleton.AddJoint("Pelvis", m_rootJoint, Vec3(0, 1.8f, 0));
    m_torsoJoint = m_skeleton.AddJoint("Torso", m_pelvisJoint, Vec3(0, 0.6f, 0));
    m_cockpitJoint = m_skeleton.AddJoint("Cockpit", m_torsoJoint, Vec3(0, 0.4f, 0.3f));
    m_leftShoulderJoint = m_skeleton.AddJoint("LeftShoulder", m_torsoJoint, Vec3(-1.4f, 0.3f, 0));
    m_rightShoulderJoint = m_skeleton.AddJoint("RightShoulder", m_torsoJoint, Vec3(1.4f, 0.3f, 0));

    // Left leg
    m_leftUpperLegJoint = m_skeleton.AddJoint("LeftUpperLeg", m_pelvisJoint, Vec3(-0.7f, -0.3f, 0));
    m_leftLowerLegJoint = m_skeleton.AddJoint("LeftLowerLeg", m_leftUpperLegJoint, Vec3(0, -1.0f, -0.1f));
    m_leftFootJoint = m_skeleton.AddJoint("LeftFoot", m_leftLowerLegJoint, Vec3(0, -1.0f, 0.2f));

    // Right leg
    m_rightUpperLegJoint = m_skeleton.AddJoint("RightUpperLeg", m_pelvisJoint, Vec3(0.7f, -0.3f, 0));
    m_rightLowerLegJoint = m_skeleton.AddJoint("RightLowerLeg", m_rightUpperLegJoint, Vec3(0, -1.0f, -0.1f));
    m_rightFootJoint = m_skeleton.AddJoint("RightFoot", m_rightLowerLegJoint, Vec3(0, -1.0f, 0.2f));

    m_skeleton.SetBindPose();
}

void ProceduralMech3D::BuildMeshes(MechChassisClass chassisClass) {
    Material3D armorMat;
    armorMat.metallic = 0.7f;
    armorMat.roughness = 0.35f;

    Material3D frameMat;
    frameMat.albedoColor = Vec3(0.2f, 0.22f, 0.25f);
    frameMat.metallic = 0.9f;
    frameMat.roughness = 0.2f;

    Material3D cockpitVisorMat;
    cockpitVisorMat.albedoColor = Vec3(0.0f, 0.8f, 1.0f);
    cockpitVisorMat.emissiveColor = Vec3(0.0f, 0.9f, 1.0f);
    cockpitVisorMat.emissiveIntensity = 1.8f;

    Material3D reactorCoreMat;
    reactorCoreMat.albedoColor = Vec3(1.0f, 0.4f, 0.0f);
    reactorCoreMat.emissiveColor = Vec3(1.0f, 0.3f, 0.0f);
    reactorCoreMat.emissiveIntensity = 2.5f;

    float scaleMult = 1.0f;
    if (chassisClass == MechChassisClass::TITAN_DREADNOUGHT) {
        armorMat.albedoColor = Vec3(0.6f, 0.15f, 0.15f); // Heavy Crimson
        scaleMult = 1.4f;
    } else if (chassisClass == MechChassisClass::GHOST_INFILTRATOR) {
        armorMat.albedoColor = Vec3(0.15f, 0.15f, 0.2f); // Stealth Matte Black
        scaleMult = 0.8f;
    } else {
        armorMat.albedoColor = Vec3(0.25f, 0.45f, 0.65f); // Cobalt Blue
        scaleMult = 1.0f;
    }

    // 1. Pelvis
    {
        MechPart3D part;
        part.name = "Pelvis";
        part.jointId = m_pelvisJoint;
        part.mesh = Mesh3D::CreateCube(0.8f * scaleMult);
        part.mesh.material = frameMat;
        m_parts.push_back(part);
    }

    // 2. Torso
    {
        MechPart3D part;
        part.name = "Torso";
        part.jointId = m_torsoJoint;
        part.mesh = Mesh3D::CreateCube(1.5f * scaleMult);
        part.mesh.material = armorMat;
        m_parts.push_back(part);

        // Reactor Vent on back
        MechPart3D reactorVent;
        reactorVent.name = "ReactorCore";
        reactorVent.jointId = m_torsoJoint;
        reactorVent.mesh = Mesh3D::CreateCylinder(0.35f * scaleMult, 0.3f * scaleMult, 12);
        reactorVent.mesh.material = reactorCoreMat;
        reactorVent.offsetTransform = Mat4::Translation(0.0f, 0.0f, -0.8f * scaleMult) * Mat4::RotationX(1.5707963f);
        m_parts.push_back(reactorVent);
    }

    // 3. Cockpit Visor
    {
        MechPart3D part;
        part.name = "Cockpit";
        part.jointId = m_cockpitJoint;
        part.mesh = Mesh3D::CreatePyramid(0.6f * scaleMult, 0.5f * scaleMult);
        part.mesh.material = cockpitVisorMat;
        part.offsetTransform = Mat4::RotationX(1.5707963f);
        m_parts.push_back(part);
    }

    // 4. Shoulders and Weapon Cannons
    auto createWeaponArm = [&](int jointId, float side) {
        // Shoulder Pauldron
        MechPart3D pauldron;
        pauldron.name = (side < 0) ? "LeftPauldron" : "RightPauldron";
        pauldron.jointId = jointId;
        pauldron.mesh = Mesh3D::CreateCube(0.7f * scaleMult);
        pauldron.mesh.material = armorMat;
        m_parts.push_back(pauldron);

        // Autocannon Barrel
        MechPart3D barrel;
        barrel.name = (side < 0) ? "LeftCannon" : "RightCannon";
        barrel.jointId = jointId;
        barrel.mesh = Mesh3D::CreateCylinder(0.12f * scaleMult, 1.4f * scaleMult, 10);
        barrel.mesh.material = frameMat;
        barrel.offsetTransform = Mat4::Translation(0.0f, -0.2f * scaleMult, 0.7f * scaleMult) * Mat4::RotationX(1.5707963f);
        m_parts.push_back(barrel);
    };

    createWeaponArm(m_leftShoulderJoint, -1.0f);
    createWeaponArm(m_rightShoulderJoint, 1.0f);

    // 5. Legs
    auto createLeg = [&](int hipJoint, int kneeJoint, int footJoint) {
        // Upper Leg
        MechPart3D thigh;
        thigh.name = "Thigh";
        thigh.jointId = hipJoint;
        thigh.mesh = Mesh3D::CreateCylinder(0.2f * scaleMult, 0.9f * scaleMult, 8);
        thigh.mesh.material = frameMat;
        thigh.offsetTransform = Mat4::Translation(0.0f, -0.45f * scaleMult, 0.0f);
        m_parts.push_back(thigh);

        // Lower Leg Shin Armor
        MechPart3D shin;
        shin.name = "Shin";
        shin.jointId = kneeJoint;
        shin.mesh = Mesh3D::CreateCube(0.4f * scaleMult);
        shin.mesh.material = armorMat;
        shin.offsetTransform = Mat4::Translation(0.0f, -0.45f * scaleMult, 0.0f);
        m_parts.push_back(shin);

        // Foot Tread
        MechPart3D foot;
        foot.name = "Foot";
        foot.jointId = footJoint;
        foot.mesh = Mesh3D::CreateCube(0.5f * scaleMult);
        foot.mesh.material = frameMat;
        foot.offsetTransform = Mat4::Translation(0.0f, 0.0f, 0.2f * scaleMult) * Mat4::Scale(1.0f, 0.4f, 1.8f);
        m_parts.push_back(foot);
    };

    createLeg(m_leftUpperLegJoint, m_leftLowerLegJoint, m_leftFootJoint);
    createLeg(m_rightUpperLegJoint, m_rightLowerLegJoint, m_rightFootJoint);
}

void ProceduralMech3D::BuildWalkAnimation() {
    m_walkClip = std::make_unique<AnimationClip>();
    m_walkClip->name = "Walk";
    m_walkClip->duration = 1.0f;
    m_walkClip->isLooping = true;

    // Left Upper Leg Pitch Track
    JointTrack leftThighTrack;
    leftThighTrack.jointId = m_leftUpperLegJoint;
    leftThighTrack.rotationKeys.push_back({ 0.0f, Quat::FromAxisAngle(Vec3(1, 0, 0),  0.35f) });
    leftThighTrack.rotationKeys.push_back({ 0.25f, Quat::FromAxisAngle(Vec3(1, 0, 0), 0.0f) });
    leftThighTrack.rotationKeys.push_back({ 0.5f, Quat::FromAxisAngle(Vec3(1, 0, 0), -0.35f) });
    leftThighTrack.rotationKeys.push_back({ 0.75f, Quat::FromAxisAngle(Vec3(1, 0, 0), 0.0f) });
    leftThighTrack.rotationKeys.push_back({ 1.0f, Quat::FromAxisAngle(Vec3(1, 0, 0),  0.35f) });
    m_walkClip->AddTrack(leftThighTrack);

    // Right Upper Leg Pitch Track (180 deg out of phase)
    JointTrack rightThighTrack;
    rightThighTrack.jointId = m_rightUpperLegJoint;
    rightThighTrack.rotationKeys.push_back({ 0.0f, Quat::FromAxisAngle(Vec3(1, 0, 0), -0.35f) });
    rightThighTrack.rotationKeys.push_back({ 0.25f, Quat::FromAxisAngle(Vec3(1, 0, 0), 0.0f) });
    rightThighTrack.rotationKeys.push_back({ 0.5f, Quat::FromAxisAngle(Vec3(1, 0, 0),  0.35f) });
    rightThighTrack.rotationKeys.push_back({ 0.75f, Quat::FromAxisAngle(Vec3(1, 0, 0), 0.0f) });
    rightThighTrack.rotationKeys.push_back({ 1.0f, Quat::FromAxisAngle(Vec3(1, 0, 0), -0.35f) });
    m_walkClip->AddTrack(rightThighTrack);

    // Pelvis Bobbing Track
    JointTrack pelvisTrack;
    pelvisTrack.jointId = m_pelvisJoint;
    pelvisTrack.positionKeys.push_back({ 0.0f, Vec3(0, 1.8f, 0) });
    pelvisTrack.positionKeys.push_back({ 0.25f, Vec3(0, 1.7f, 0) });
    pelvisTrack.positionKeys.push_back({ 0.5f, Vec3(0, 1.8f, 0) });
    pelvisTrack.positionKeys.push_back({ 0.75f, Vec3(0, 1.7f, 0) });
    pelvisTrack.positionKeys.push_back({ 1.0f, Vec3(0, 1.8f, 0) });
    m_walkClip->AddTrack(pelvisTrack);

    m_animPlayer.Play(m_walkClip.get());
}

void ProceduralMech3D::WalkCycle(float speed, float dt) {
    if (std::abs(speed) > 0.01f) {
        m_animPlayer.SetPlaybackSpeed(speed);
        m_animPlayer.Update(m_skeleton, dt);
    }
}

void ProceduralMech3D::AimTorso(const Vec3& targetWorldPos, const Mat4& mechWorldTransform, float) {
    // Transform target into local mech coordinates
    Vec3 torsoPosWorld = mechWorldTransform.TransformPoint(m_skeleton.GetJoint(m_torsoJoint).localPosition);
    Vec3 aimDir = (targetWorldPos - torsoPosWorld).Normalized();

    // Yaw rotation calculation
    float targetAngle = std::atan2(aimDir.x, aimDir.z);
    m_skeleton.GetJoint(m_torsoJoint).localRotation = Quat::FromAxisAngle(Vec3(0, 1, 0), targetAngle);
    m_skeleton.UpdateHierarchy();
}

void ProceduralMech3D::Update(float dt) {
    m_skeleton.UpdateHierarchy();
}

void ProceduralMech3D::Render(Rasterizer& rasterizer, const Mat4& worldTransform, bool wireframe) {
    for (const auto& part : m_parts) {
        if (part.jointId >= 0 && part.jointId < static_cast<int>(m_skeleton.GetJointCount())) {
            const auto& joint = m_skeleton.GetJoint(part.jointId);
            Mat4 partModelMatrix = worldTransform * joint.modelMatrix * part.offsetTransform;
            rasterizer.DrawMesh(part.mesh, partModelMatrix, wireframe);
        }
    }
}

} // namespace Voidfall
