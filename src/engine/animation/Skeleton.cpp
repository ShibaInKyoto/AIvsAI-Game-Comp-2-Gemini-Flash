#include "Skeleton.h"
#include <cmath>
#include <algorithm>

namespace Voidfall {

Vec3 JointTrack::SamplePosition(float time) const {
    if (positionKeys.empty()) return Vec3(0, 0, 0);
    if (positionKeys.size() == 1 || time <= positionKeys.front().time) return positionKeys.front().value;
    if (time >= positionKeys.back().time) return positionKeys.back().value;

    for (size_t i = 0; i + 1 < positionKeys.size(); ++i) {
        if (time >= positionKeys[i].time && time <= positionKeys[i + 1].time) {
            float dt = positionKeys[i + 1].time - positionKeys[i].time;
            float t = (dt > 0.0001f) ? (time - positionKeys[i].time) / dt : 0.0f;
            return Math::Lerp(positionKeys[i].value, positionKeys[i + 1].value, t);
        }
    }
    return positionKeys.back().value;
}

Quat JointTrack::SampleRotation(float time) const {
    if (rotationKeys.empty()) return Quat::Identity();
    if (rotationKeys.size() == 1 || time <= rotationKeys.front().time) return rotationKeys.front().value;
    if (time >= rotationKeys.back().time) return rotationKeys.back().value;

    for (size_t i = 0; i + 1 < rotationKeys.size(); ++i) {
        if (time >= rotationKeys[i].time && time <= rotationKeys[i + 1].time) {
            float dt = rotationKeys[i + 1].time - rotationKeys[i].time;
            float t = (dt > 0.0001f) ? (time - rotationKeys[i].time) / dt : 0.0f;
            return Quat::Slerp(rotationKeys[i].value, rotationKeys[i + 1].value, t);
        }
    }
    return rotationKeys.back().value;
}

Vec3 JointTrack::SampleScale(float time) const {
    if (scaleKeys.empty()) return Vec3(1, 1, 1);
    if (scaleKeys.size() == 1 || time <= scaleKeys.front().time) return scaleKeys.front().value;
    if (time >= scaleKeys.back().time) return scaleKeys.back().value;

    for (size_t i = 0; i + 1 < scaleKeys.size(); ++i) {
        if (time >= scaleKeys[i].time && time <= scaleKeys[i + 1].time) {
            float dt = scaleKeys[i + 1].time - scaleKeys[i].time;
            float t = (dt > 0.0001f) ? (time - scaleKeys[i].time) / dt : 0.0f;
            return Math::Lerp(scaleKeys[i].value, scaleKeys[i + 1].value, t);
        }
    }
    return scaleKeys.back().value;
}

void AnimationClip::AddTrack(const JointTrack& track) {
    tracks.push_back(track);
}

const JointTrack* AnimationClip::FindTrack(int jointId) const {
    for (const auto& track : tracks) {
        if (track.jointId == jointId) return &track;
    }
    return nullptr;
}

int Skeleton::AddJoint(const std::string& name, int parentId, const Vec3& pos, const Quat& rot) {
    int id = static_cast<int>(m_joints.size());
    Joint j;
    j.id = id;
    j.name = name;
    j.parentId = parentId;
    j.localPosition = pos;
    j.localRotation = rot;
    j.localScale = Vec3(1, 1, 1);
    m_joints.push_back(j);
    m_nameToIndex[name] = id;
    return id;
}

int Skeleton::FindJointIndex(const std::string& name) const {
    auto it = m_nameToIndex.find(name);
    return (it != m_nameToIndex.end()) ? it->second : -1;
}

void Skeleton::UpdateHierarchy() {
    for (auto& joint : m_joints) {
        Mat4 t = Mat4::Translation(joint.localPosition.x, joint.localPosition.y, joint.localPosition.z);
        Mat4 r = joint.localRotation.ToMat4();
        Mat4 s = Mat4::Scale(joint.localScale.x, joint.localScale.y, joint.localScale.z);
        joint.localMatrix = t * r * s;

        if (joint.parentId >= 0 && joint.parentId < static_cast<int>(m_joints.size())) {
            joint.modelMatrix = m_joints[joint.parentId].modelMatrix * joint.localMatrix;
        } else {
            joint.modelMatrix = joint.localMatrix;
        }
    }
}

void Skeleton::SetBindPose() {
    UpdateHierarchy();
    // Invert model matrix for bind pose
    for (auto& joint : m_joints) {
        joint.invBindPose = joint.modelMatrix; // Identity bind approximation or inverse
    }
}

bool Skeleton::SolveTwoBoneIK(
    Vec3& outRootRotAxis, float& outRootAngle,
    Vec3& outMidRotAxis, float& outMidAngle,
    const Vec3& rootPos, const Vec3& midPos, const Vec3& endPos,
    const Vec3& targetPos, const Vec3& poleVector
) {
    Vec3 rToM = midPos - rootPos;
    Vec3 mToE = endPos - midPos;
    Vec3 rToE = endPos - rootPos;
    Vec3 rToT = targetPos - rootPos;

    float l1 = rToM.Length();
    float l2 = mToE.Length();
    float d = rToT.Length();

    if (l1 <= 0.0001f || l2 <= 0.0001f || d <= 0.0001f) {
        return false;
    }

    // Clamp distance within reachable envelope
    float maxDist = (l1 + l2) * 0.999f;
    float minDist = std::max(0.01f, std::abs(l1 - l2) * 1.001f);
    d = std::clamp(d, minDist, maxDist);

    // Law of cosines for current angle at mid
    float cosCurrentMid = (l1 * l1 + l2 * l2 - rToE.LengthSq()) / (2.0f * l1 * l2);
    cosCurrentMid = std::clamp(cosCurrentMid, -1.0f, 1.0f);

    // Law of cosines for desired angle at mid
    float cosDesiredMid = (l1 * l1 + l2 * l2 - d * d) / (2.0f * l1 * l2);
    cosDesiredMid = std::clamp(cosDesiredMid, -1.0f, 1.0f);

    // Bend plane normal from pole vector
    Vec3 bendNormal = rToT.Cross(poleVector).Normalized();
    if (bendNormal.LengthSq() < 0.0001f) {
        bendNormal = Vec3(0, 0, 1);
    }

    outMidRotAxis = bendNormal;
    outMidAngle = std::acos(cosDesiredMid) - std::acos(cosCurrentMid);

    // Desired angle at root
    float cosDesiredRoot = (l1 * l1 + d * d - l2 * l2) / (2.0f * l1 * d);
    cosDesiredRoot = std::clamp(cosDesiredRoot, -1.0f, 1.0f);

    Vec3 dirToTarget = rToT.Normalized();
    Vec3 dirToMid = rToM.Normalized();

    outRootRotAxis = dirToMid.Cross(dirToTarget).Normalized();
    if (outRootRotAxis.LengthSq() < 0.0001f) {
        outRootRotAxis = bendNormal;
    }
    outRootAngle = std::acos(cosDesiredRoot);

    return true;
}

Quat Skeleton::SolveAimIK(
    const Vec3& currentAimDir,
    const Vec3& targetAimDir,
    const Vec3& upVector,
    float maxAngleRad
) {
    Vec3 cur = currentAimDir.Normalized();
    Vec3 tgt = targetAimDir.Normalized();

    float dot = std::clamp(cur.Dot(tgt), -1.0f, 1.0f);
    float angle = std::acos(dot);

    if (angle < 0.0001f) {
        return Quat::Identity();
    }

    angle = std::min(angle, maxAngleRad);
    Vec3 axis = cur.Cross(tgt).Normalized();
    if (axis.LengthSq() < 0.0001f) {
        axis = upVector;
    }

    return Quat::FromAxisAngle(axis, angle);
}

void AnimationPlayer::Play(const AnimationClip* clip, float blendDuration) {
    if (m_currentClip != clip) {
        m_prevClip = m_currentClip;
        m_prevTime = m_currentTime;
        m_currentClip = clip;
        m_currentTime = 0.0f;
        m_blendDuration = blendDuration;
        m_blendTimer = 0.0f;
        m_isFinished = false;
    }
}

void AnimationPlayer::Update(Skeleton& skeleton, float dt) {
    if (!m_currentClip) return;

    m_currentTime += dt * m_speed;
    if (m_currentTime > m_currentClip->duration) {
        if (m_currentClip->isLooping) {
            m_currentTime = std::fmod(m_currentTime, m_currentClip->duration);
        } else {
            m_currentTime = m_currentClip->duration;
            m_isFinished = true;
        }
    }

    float blendWeight = 1.0f;
    if (m_blendDuration > 0.0001f && m_blendTimer < m_blendDuration && m_prevClip) {
        m_blendTimer += dt;
        blendWeight = std::min(1.0f, m_blendTimer / m_blendDuration);
    } else {
        m_prevClip = nullptr;
    }

    for (size_t i = 0; i < skeleton.GetJointCount(); ++i) {
        Joint& joint = skeleton.GetJoint(static_cast<int>(i));
        const JointTrack* curTrack = m_currentClip->FindTrack(joint.id);

        if (curTrack) {
            Vec3 pos = curTrack->SamplePosition(m_currentTime);
            Quat rot = curTrack->SampleRotation(m_currentTime);
            Vec3 sca = curTrack->SampleScale(m_currentTime);

            if (m_prevClip) {
                const JointTrack* prevTrack = m_prevClip->FindTrack(joint.id);
                if (prevTrack) {
                    Vec3 prevPos = prevTrack->SamplePosition(m_prevTime);
                    Quat prevRot = prevTrack->SampleRotation(m_prevTime);
                    Vec3 prevSca = prevTrack->SampleScale(m_prevTime);

                    pos = Math::Lerp(prevPos, pos, blendWeight);
                    rot = Quat::Slerp(prevRot, rot, blendWeight);
                    sca = Math::Lerp(prevSca, sca, blendWeight);
                }
            }

            joint.localPosition = pos;
            joint.localRotation = rot;
            joint.localScale = sca;
        }
    }

    skeleton.UpdateHierarchy();
}

} // namespace Voidfall
