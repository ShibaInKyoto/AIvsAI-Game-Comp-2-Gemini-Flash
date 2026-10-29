#pragma once

#include "core/ecs/Entity.h"
#include "core/ecs/ComponentArray.h"
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <queue>

namespace Voidfall::ECS {

class Registry {
public:
    Registry(size_t maxEntities = 10000);

    Entity CreateEntity();
    void DestroyEntity(Entity entity);
    [[nodiscard]] bool IsValid(Entity entity) const;

    template <typename T, typename... Args>
    T& AddComponent(Entity entity, Args&&... args) {
        auto compArray = GetComponentArray<T>();
        compArray->Insert(entity, T(std::forward<Args>(args)...));
        return compArray->Get(entity);
    }

    template <typename T>
    void RemoveComponent(Entity entity) {
        GetComponentArray<T>()->Remove(entity);
    }

    template <typename T>
    [[nodiscard]] T& GetComponent(Entity entity) {
        return GetComponentArray<T>()->Get(entity);
    }

    template <typename T>
    [[nodiscard]] const T& GetComponent(Entity entity) const {
        return GetComponentArray<T>()->Get(entity);
    }

    template <typename T>
    [[nodiscard]] bool HasComponent(Entity entity) const {
        return GetComponentArray<T>()->Has(entity);
    }

    template <typename T>
    [[nodiscard]] std::shared_ptr<ComponentArray<T>> GetComponentArray() {
        std::type_index typeIdx(typeid(T));
        if (m_componentArrays.find(typeIdx) == m_componentArrays.end()) {
            m_componentArrays[typeIdx] = std::make_shared<ComponentArray<T>>();
        }
        return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[typeIdx]);
    }

    template <typename T>
    [[nodiscard]] std::shared_ptr<const ComponentArray<T>> GetComponentArray() const {
        std::type_index typeIdx(typeid(T));
        auto it = m_componentArrays.find(typeIdx);
        if (it == m_componentArrays.end()) {
            return nullptr;
        }
        return std::static_pointer_cast<const ComponentArray<T>>(it->second);
    }

    void Clear();

    [[nodiscard]] size_t GetLivingEntityCount() const noexcept { return m_livingCount; }

private:
    std::vector<uint32_t> m_generations;
    std::queue<uint32_t> m_freeIndices;
    uint32_t m_nextId{0};
    size_t m_livingCount{0};

    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> m_componentArrays;
};

} // namespace Voidfall::ECS
