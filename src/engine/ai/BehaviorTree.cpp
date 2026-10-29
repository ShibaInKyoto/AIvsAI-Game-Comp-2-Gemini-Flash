#include "engine/ai/BehaviorTree.h"

namespace Voidfall::AI {

NodeStatus BTSelector::Tick(float dt, Blackboard& bb) {
    while (m_currentChild < m_children.size()) {
        NodeStatus status = m_children[m_currentChild]->Tick(dt, bb);
        if (status == NodeStatus::RUNNING) {
            return NodeStatus::RUNNING;
        }
        if (status == NodeStatus::SUCCESS) {
            Reset();
            return NodeStatus::SUCCESS;
        }
        m_currentChild++;
    }
    Reset();
    return NodeStatus::FAILURE;
}

void BTSelector::Reset() {
    m_currentChild = 0;
    for (auto& child : m_children) child->Reset();
}

NodeStatus BTSequence::Tick(float dt, Blackboard& bb) {
    while (m_currentChild < m_children.size()) {
        NodeStatus status = m_children[m_currentChild]->Tick(dt, bb);
        if (status == NodeStatus::RUNNING) {
            return NodeStatus::RUNNING;
        }
        if (status == NodeStatus::FAILURE) {
            Reset();
            return NodeStatus::FAILURE;
        }
        m_currentChild++;
    }
    Reset();
    return NodeStatus::SUCCESS;
}

void BTSequence::Reset() {
    m_currentChild = 0;
    for (auto& child : m_children) child->Reset();
}

NodeStatus BTParallel::Tick(float dt, Blackboard& bb) {
    size_t successCount = 0;
    size_t failureCount = 0;

    for (auto& child : m_children) {
        NodeStatus status = child->Tick(dt, bb);
        if (status == NodeStatus::SUCCESS) successCount++;
        if (status == NodeStatus::FAILURE) failureCount++;
    }

    if (m_policy == Policy::REQUIRE_ALL) {
        if (failureCount > 0) return NodeStatus::FAILURE;
        if (successCount == m_children.size()) return NodeStatus::SUCCESS;
    } else {
        if (successCount > 0) return NodeStatus::SUCCESS;
        if (failureCount == m_children.size()) return NodeStatus::FAILURE;
    }

    return NodeStatus::RUNNING;
}

void BTParallel::Reset() {
    for (auto& child : m_children) child->Reset();
}

NodeStatus BTInverter::Tick(float dt, Blackboard& bb) {
    if (!m_child) return NodeStatus::FAILURE;
    NodeStatus status = m_child->Tick(dt, bb);
    if (status == NodeStatus::SUCCESS) return NodeStatus::FAILURE;
    if (status == NodeStatus::FAILURE) return NodeStatus::SUCCESS;
    return NodeStatus::RUNNING;
}

NodeStatus BTCooldown::Tick(float dt, Blackboard& bb) {
    if (m_timer > 0.0f) {
        m_timer -= dt;
        return NodeStatus::FAILURE;
    }

    if (!m_child) return NodeStatus::FAILURE;
    NodeStatus status = m_child->Tick(dt, bb);
    if (status == NodeStatus::SUCCESS) {
        m_timer = m_cooldown;
    }
    return status;
}

} // namespace Voidfall::AI
