#pragma once

#include "core/Types.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <any>

namespace Voidfall::AI {

enum class NodeStatus {
    SUCCESS,
    FAILURE,
    RUNNING
};

class Blackboard {
public:
    template <typename T>
    void Set(const std::string& key, const T& value) {
        m_data[key] = value;
    }

    template <typename T>
    [[nodiscard]] T Get(const std::string& key, const T& defaultValue = T()) const {
        auto it = m_data.find(key);
        if (it == m_data.end()) return defaultValue;
        try {
            return std::any_cast<T>(it->second);
        } catch (...) {
            return defaultValue;
        }
    }

    [[nodiscard]] bool Has(const std::string& key) const {
        return m_data.find(key) != m_data.end();
    }

    void Clear() { m_data.clear(); }

private:
    std::unordered_map<std::string, std::any> m_data;
};

class BTNode {
public:
    virtual ~BTNode() = default;
    virtual NodeStatus Tick(float dt, Blackboard& bb) = 0;
    virtual void Reset() {}
};

// Composites
class BTSelector : public BTNode {
public:
    void AddChild(std::shared_ptr<BTNode> child) { m_children.push_back(child); }
    NodeStatus Tick(float dt, Blackboard& bb) override;
    void Reset() override;

private:
    std::vector<std::shared_ptr<BTNode>> m_children;
    size_t m_currentChild{0};
};

class BTSequence : public BTNode {
public:
    void AddChild(std::shared_ptr<BTNode> child) { m_children.push_back(child); }
    NodeStatus Tick(float dt, Blackboard& bb) override;
    void Reset() override;

private:
    std::vector<std::shared_ptr<BTNode>> m_children;
    size_t m_currentChild{0};
};

class BTParallel : public BTNode {
public:
    enum class Policy { REQUIRE_ALL, REQUIRE_ONE };
    explicit BTParallel(Policy successPolicy = Policy::REQUIRE_ALL) : m_policy(successPolicy) {}

    void AddChild(std::shared_ptr<BTNode> child) { m_children.push_back(child); }
    NodeStatus Tick(float dt, Blackboard& bb) override;
    void Reset() override;

private:
    Policy m_policy;
    std::vector<std::shared_ptr<BTNode>> m_children;
};

// Decorators
class BTInverter : public BTNode {
public:
    explicit BTInverter(std::shared_ptr<BTNode> child) : m_child(std::move(child)) {}
    NodeStatus Tick(float dt, Blackboard& bb) override;
    void Reset() override { if (m_child) m_child->Reset(); }

private:
    std::shared_ptr<BTNode> m_child;
};

class BTCooldown : public BTNode {
public:
    BTCooldown(std::shared_ptr<BTNode> child, float cooldownTime)
        : m_child(std::move(child)), m_cooldown(cooldownTime) {}

    NodeStatus Tick(float dt, Blackboard& bb) override;
    void Reset() override { m_timer = 0.0f; if (m_child) m_child->Reset(); }

private:
    std::shared_ptr<BTNode> m_child;
    float m_cooldown{1.0f};
    float m_timer{0.0f};
};

} // namespace Voidfall::AI
