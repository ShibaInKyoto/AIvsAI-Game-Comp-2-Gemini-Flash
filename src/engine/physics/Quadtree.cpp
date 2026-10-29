#include "engine/physics/Quadtree.h"
#include <algorithm>

namespace Voidfall::Physics {

QuadtreeNode::QuadtreeNode(const RectAABB& boundary, int depth)
    : m_boundary(boundary), m_depth(depth) {
    m_elements.reserve(MAX_ELEMENTS);
}

void QuadtreeNode::Subdivide() {
    float halfW = m_boundary.width * 0.5f;
    float halfH = m_boundary.height * 0.5f;
    float x = m_boundary.x;
    float y = m_boundary.y;

    m_northWest = std::make_unique<QuadtreeNode>(RectAABB{x, y, halfW, halfH}, m_depth + 1);
    m_northEast = std::make_unique<QuadtreeNode>(RectAABB{x + halfW, y, halfW, halfH}, m_depth + 1);
    m_southWest = std::make_unique<QuadtreeNode>(RectAABB{x, y + halfH, halfW, halfH}, m_depth + 1);
    m_southEast = std::make_unique<QuadtreeNode>(RectAABB{x + halfW, y + halfH, halfW, halfH}, m_depth + 1);

    m_isDivided = true;

    // Distribute existing elements to children
    std::vector<QuadtreeElement> remaining;
    for (const auto& elem : m_elements) {
        bool inserted = m_northWest->Insert(elem) ||
                        m_northEast->Insert(elem) ||
                        m_southWest->Insert(elem) ||
                        m_southEast->Insert(elem);
        if (!inserted) {
            remaining.push_back(elem);
        }
    }
    m_elements = std::move(remaining);
}

bool QuadtreeNode::Insert(const QuadtreeElement& element) {
    // Check if element center is within boundary
    if (!m_boundary.Contains(element.bounds.center)) {
        return false;
    }

    if (m_elements.size() < MAX_ELEMENTS || m_depth >= MAX_DEPTH) {
        m_elements.push_back(element);
        return true;
    }

    if (!m_isDivided) {
        Subdivide();
    }

    if (m_northWest->Insert(element)) return true;
    if (m_northEast->Insert(element)) return true;
    if (m_southWest->Insert(element)) return true;
    if (m_southEast->Insert(element)) return true;

    // If it spans quadrants, keep in this node
    m_elements.push_back(element);
    return true;
}

void QuadtreeNode::QueryRange(const Circle& range, std::vector<uint32_t>& outResults) const {
    // Quick AABB intersection test with boundary
    RectAABB rangeBox{
        range.center.x - range.radius,
        range.center.y - range.radius,
        range.radius * 2.0f,
        range.radius * 2.0f
    };

    if (!m_boundary.Intersects(rangeBox)) {
        return;
    }

    for (const auto& elem : m_elements) {
        if (range.Intersects(elem.bounds)) {
            outResults.push_back(elem.id);
        }
    }

    if (m_isDivided) {
        m_northWest->QueryRange(range, outResults);
        m_northEast->QueryRange(range, outResults);
        m_southWest->QueryRange(range, outResults);
        m_southEast->QueryRange(range, outResults);
    }
}

void QuadtreeNode::QueryAABB(const RectAABB& aabb, std::vector<uint32_t>& outResults) const {
    if (!m_boundary.Intersects(aabb)) {
        return;
    }

    for (const auto& elem : m_elements) {
        if (aabb.Contains(elem.bounds.center)) {
            outResults.push_back(elem.id);
        }
    }

    if (m_isDivided) {
        m_northWest->QueryAABB(aabb, outResults);
        m_northEast->QueryAABB(aabb, outResults);
        m_southWest->QueryAABB(aabb, outResults);
        m_southEast->QueryAABB(aabb, outResults);
    }
}

void QuadtreeNode::Clear() {
    m_elements.clear();
    m_northWest.reset();
    m_northEast.reset();
    m_southWest.reset();
    m_southEast.reset();
    m_isDivided = false;
}

size_t QuadtreeNode::GetTotalElementCount() const {
    size_t count = m_elements.size();
    if (m_isDivided) {
        count += m_northWest->GetTotalElementCount();
        count += m_northEast->GetTotalElementCount();
        count += m_southWest->GetTotalElementCount();
        count += m_southEast->GetTotalElementCount();
    }
    return count;
}

Quadtree::Quadtree(const RectAABB& worldBounds)
    : m_worldBounds(worldBounds), m_root(worldBounds) {}

void Quadtree::Insert(uint32_t id, const Circle& bounds) {
    m_root.Insert(QuadtreeElement{id, bounds});
}

void Quadtree::Query(const Circle& range, std::vector<uint32_t>& outResults) const {
    m_root.QueryRange(range, outResults);
}

void Quadtree::Query(const RectAABB& aabb, std::vector<uint32_t>& outResults) const {
    m_root.QueryAABB(aabb, outResults);
}

void Quadtree::Clear() {
    m_root.Clear();
}

} // namespace Voidfall::Physics
