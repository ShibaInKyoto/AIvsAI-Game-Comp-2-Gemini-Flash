#pragma once

#include "core/Types.h"
#include <vector>
#include <memory>

namespace Voidfall::Physics {

struct QuadtreeElement {
    uint32_t id{0};
    Circle bounds;
};

class QuadtreeNode {
public:
    static constexpr size_t MAX_ELEMENTS = 8;
    static constexpr int MAX_DEPTH = 6;

    QuadtreeNode(const RectAABB& boundary, int depth = 0);

    bool Insert(const QuadtreeElement& element);
    void Subdivide();
    void QueryRange(const Circle& range, std::vector<uint32_t>& outResults) const;
    void QueryAABB(const RectAABB& aabb, std::vector<uint32_t>& outResults) const;
    void Clear();

    [[nodiscard]] size_t GetTotalElementCount() const;

private:
    RectAABB m_boundary;
    int m_depth{0};
    bool m_isDivided{false};

    std::vector<QuadtreeElement> m_elements;

    std::unique_ptr<QuadtreeNode> m_northWest;
    std::unique_ptr<QuadtreeNode> m_northEast;
    std::unique_ptr<QuadtreeNode> m_southWest;
    std::unique_ptr<QuadtreeNode> m_southEast;
};

class Quadtree {
public:
    explicit Quadtree(const RectAABB& worldBounds);

    void Insert(uint32_t id, const Circle& bounds);
    void Query(const Circle& range, std::vector<uint32_t>& outResults) const;
    void Query(const RectAABB& aabb, std::vector<uint32_t>& outResults) const;
    void Clear();

private:
    RectAABB m_worldBounds;
    QuadtreeNode m_root;
};

} // namespace Voidfall::Physics
