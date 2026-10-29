#pragma once

#include <cstdint>

namespace Voidfall::ECS {

struct Entity {
    uint32_t id{0};
    uint32_t generation{0};

    [[nodiscard]] constexpr bool operator==(const Entity& o) const noexcept {
        return id == o.id && generation == o.generation;
    }

    [[nodiscard]] constexpr bool operator!=(const Entity& o) const noexcept {
        return !(*this == o);
    }

    [[nodiscard]] constexpr uint64_t ToHandle() const noexcept {
        return (static_cast<uint64_t>(generation) << 32) | static_cast<uint64_t>(id);
    }

    static constexpr Entity Null() noexcept {
        return {0xFFFFFFFF, 0xFFFFFFFF};
    }

    [[nodiscard]] constexpr bool IsNull() const noexcept {
        return id == 0xFFFFFFFF && generation == 0xFFFFFFFF;
    }
};

} // namespace Voidfall::ECS
