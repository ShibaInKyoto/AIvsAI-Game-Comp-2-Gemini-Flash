#pragma once

#include "core/ecs/Entity.h"
#include <vector>
#include <cassert>
#include <algorithm>

namespace Voidfall::ECS {

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
};

template <typename T>
class ComponentArray : public IComponentArray {
public:
    ComponentArray(size_t initialCapacity = 256) {
        m_dense.reserve(initialCapacity);
        m_denseToEntity.reserve(initialCapacity);
    }

    void Insert(Entity entity, T component) {
        assert(!Has(entity) && "Component already added to entity!");

        size_t denseIdx = m_dense.size();
        EnsureSparseCapacity(entity.id);
        m_sparse[entity.id] = static_cast<int32_t>(denseIdx);
        m_dense.push_back(std::move(component));
        m_denseToEntity.push_back(entity);
    }

    void Remove(Entity entity) {
        if (!Has(entity)) return;

        size_t denseIdx = static_cast<size_t>(m_sparse[entity.id]);
        size_t lastIdx = m_dense.size() - 1;

        if (denseIdx != lastIdx) {
            // Swap with last element for O(1) removal
            m_dense[denseIdx] = std::move(m_dense[lastIdx]);
            Entity lastEntity = m_denseToEntity[lastIdx];
            m_denseToEntity[denseIdx] = lastEntity;
            m_sparse[lastEntity.id] = static_cast<int32_t>(denseIdx);
        }

        m_dense.pop_back();
        m_denseToEntity.pop_back();
        m_sparse[entity.id] = -1;
    }

    [[nodiscard]] bool Has(Entity entity) const {
        if (entity.id >= m_sparse.size()) return false;
        return m_sparse[entity.id] != -1;
    }

    [[nodiscard]] T& Get(Entity entity) {
        assert(Has(entity) && "Retrieving non-existent component!");
        return m_dense[static_cast<size_t>(m_sparse[entity.id])];
    }

    [[nodiscard]] const T& Get(Entity entity) const {
        assert(Has(entity) && "Retrieving non-existent component!");
        return m_dense[static_cast<size_t>(m_sparse[entity.id])];
    }

    void EntityDestroyed(Entity entity) override {
        Remove(entity);
    }

    [[nodiscard]] size_t Size() const noexcept { return m_dense.size(); }
    [[nodiscard]] std::vector<T>& GetDenseArray() noexcept { return m_dense; }
    [[nodiscard]] const std::vector<T>& GetDenseArray() const noexcept { return m_dense; }
    [[nodiscard]] const std::vector<Entity>& GetDenseEntities() const noexcept { return m_denseToEntity; }

private:
    void EnsureSparseCapacity(uint32_t id) {
        if (id >= m_sparse.size()) {
            m_sparse.resize(id + 64, -1);
        }
    }

    std::vector<T> m_dense;
    std::vector<Entity> m_denseToEntity;
    std::vector<int32_t> m_sparse;
};

} // namespace Voidfall::ECS
