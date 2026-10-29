#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "core/math/MathCore.h"

namespace Voidfall {

using namespace Math;

struct Joint {
    int id{0};
    std::string name;
    int parentId{-1};

    Vec3 localPosition{0.0f, 0.0f, 0.0f};
    Quat localRotation{Quat::Identity()};
    Vec3 localScale{1.0f, 1.0f, 1.0f};

    Mat4 localMatrix{Mat4::Identity()};
    Mat4 modelMatrix{Mat4::Identity()};
    Mat4 invBindPose{Mat4::Identity()};
};

struct VectorKeyframe {
    float time{0.0f};
    Vec3 value{0.0f, 0.0f, 0.0f};

    VectorKeyframe() = default;
    VectorKeyframe(float t, const Vec3& v) : time(t), value(v) {}
};

struct QuatKeyframe {
    float time{0.0f};
    Quat value{Quat::Identity()};

    QuatKeyframe() = default;
    QuatKeyframe(float t, const Quat& q) : time(t), value(q) {}
};

struct JointTrack {
    int jointId{-1};
    std::vector<VectorKeyframe> positionKeys;
    std::vector<QuatKeyframe> rotationKeys;
    std::vector<VectorKeyframe> scaleKeys;

    Vec3 SamplePosition(float time) const;
    Quat SampleRotation(float time) const;
    Vec3 SampleScale(float time) const;
};

struct AnimationClip {
    std::string name;
    float duration{1.0f};
    bool isLooping{true};
    std::vector<JointTrack> tracks;

    void AddTrack(const JointTrack& track);
    const JointTrack* FindTrack(int jointId) const;
};

class Skeleton {
public:
    Skeleton() = default;

    int AddJoint(const std::string& name, int parentId = -1, const Vec3& pos = Vec3(0,0,0), const Quat& rot = Quat::Identity());
    int FindJointIndex(const std::string& name) const;

    void SetBindPose();
    void UpdateHierarchy();

    Joint& GetJoint(int index) { return m_joints[index]; }
    const Joint& GetJoint(int index) const { return m_joints[index]; }
    size_t GetJointCount() const noexcept { return m_joints.size(); }
    const std::vector<Joint>& GetJoints() const noexcept { return m_joints; }

    // Two-Bone Analytic Inverse Kinematics (Legs / Knees / Arms)
    static bool SolveTwoBoneIK(
        Vec3& outRootRotAxis, float& outRootAngle,
        Vec3& outMidRotAxis, float& outMidAngle,
        const Vec3& rootPos, const Vec3& midPos, const Vec3& endPos,
        const Vec3& targetPos, const Vec3& poleVector
    );

    // Aim IK (Turrets, Cockpit Tracking, Weapon Hardpoints)
    static Quat SolveAimIK(
        const Vec3& currentAimDir,
        const Vec3& targetAimDir,
        const Vec3& upVector,
        float maxAngleRad = 3.14159265f
    );

private:
    std::vector<Joint> m_joints;
    std::unordered_map<std::string, int> m_nameToIndex;
};

class AnimationPlayer {
public:
    AnimationPlayer() = default;

    void Play(const AnimationClip* clip, float blendDuration = 0.2f);
    void Update(Skeleton& skeleton, float dt);
    void SetPlaybackSpeed(float speed) noexcept { m_speed = speed; }
    bool IsFinished() const noexcept { return m_isFinished; }
    float GetCurrentTime() const noexcept { return m_currentTime; }

private:
    const AnimationClip* m_currentClip{nullptr};
    const AnimationClip* m_prevClip{nullptr};
    float m_currentTime{0.0f};
    float m_prevTime{0.0f};
    float m_blendTimer{0.0f};
    float m_blendDuration{0.0f};
    float m_speed{1.0f};
    bool m_isFinished{false};
};

} // namespace Voidfall
