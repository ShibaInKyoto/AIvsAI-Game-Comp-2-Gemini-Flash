#include "core/ecs/Registry.h"

namespace Voidfall::ECS {

Registry::Registry(size_t maxEntities) {
    m_generations.resize(maxEntities, 0);
}

Entity Registry::CreateEntity() {
    uint32_t id = 0;
    if (!m_freeIndices.empty()) {
        id = m_freeIndices.front();
        m_freeIndices.pop();
    } else {
        id = m_nextId++;
        if (id >= m_generations.size()) {
            m_generations.resize(m_generations.size() * 2, 0);
        }
    }

    m_livingCount++;
    return Entity{id, m_generations[id]};
}

void Registry::DestroyEntity(Entity entity) {
    if (!IsValid(entity)) return;

    // Notify all component arrays to drop this entity's components
    for (auto& [type, compArray] : m_componentArrays) {
        compArray->EntityDestroyed(entity);
    }

    // Bump generation so stale entity handles cannot access new occupants
    m_generations[entity.id]++;
    m_freeIndices.push(entity.id);
    m_livingCount--;
}

bool Registry::IsValid(Entity entity) const {
    if (entity.id >= m_generations.size()) return false;
    return m_generations[entity.id] == entity.generation;
}

void Registry::Clear() {
    for (auto& [type, compArray] : m_componentArrays) {
        // Clearing is handled by dropping entities
    }
    m_componentArrays.clear();
    while (!m_freeIndices.empty()) m_freeIndices.pop();
    m_nextId = 0;
    m_livingCount = 0;
    std::fill(m_generations.begin(), m_generations.end(), 0);
}

} // namespace Voidfall::ECS
