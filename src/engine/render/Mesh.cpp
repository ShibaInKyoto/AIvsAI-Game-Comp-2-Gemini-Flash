#include "Mesh.h"
#include <cmath>
#include <algorithm>

namespace Voidfall {

void Mesh3D::CalculateNormals() {
    for (auto& v : vertices) {
        v.normal = Vec3(0.0f, 0.0f, 0.0f);
    }

    for (size_t i = 0; i + 2 < indices.size(); i += 3) {
        uint32_t i0 = indices[i];
        uint32_t i1 = indices[i + 1];
        uint32_t i2 = indices[i + 2];

        Vec3 edge1 = vertices[i1].position - vertices[i0].position;
        Vec3 edge2 = vertices[i2].position - vertices[i0].position;
        Vec3 normal = edge1.Cross(edge2);

        vertices[i0].normal = vertices[i0].normal + normal;
        vertices[i1].normal = vertices[i1].normal + normal;
        vertices[i2].normal = vertices[i2].normal + normal;
    }

    for (auto& v : vertices) {
        float lenSq = v.normal.LengthSq();
        if (lenSq > 0.00001f) {
            v.normal = v.normal * (1.0f / std::sqrt(lenSq));
        } else {
            v.normal = Vec3(0.0f, 1.0f, 0.0f);
        }
    }
}

void Mesh3D::CalculateTangents() {
    for (auto& v : vertices) {
        v.tangent = Vec3(0.0f, 0.0f, 0.0f);
        v.bitangent = Vec3(0.0f, 0.0f, 0.0f);
    }

    for (size_t i = 0; i + 2 < indices.size(); i += 3) {
        uint32_t i0 = indices[i];
        uint32_t i1 = indices[i + 1];
        uint32_t i2 = indices[i + 2];

        auto& v0 = vertices[i0];
        auto& v1 = vertices[i1];
        auto& v2 = vertices[i2];

        Vec3 edge1 = v1.position - v0.position;
        Vec3 edge2 = v2.position - v0.position;

        Vec2 deltaUV1 = v1.texCoord - v0.texCoord;
        Vec2 deltaUV2 = v2.texCoord - v0.texCoord;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y + 0.000001f);

        Vec3 tangent(
            f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
            f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
            f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
        );

        Vec3 bitangent(
            f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
            f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
            f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z)
        );

        v0.tangent = v0.tangent + tangent;
        v1.tangent = v1.tangent + tangent;
        v2.tangent = v2.tangent + tangent;

        v0.bitangent = v0.bitangent + bitangent;
        v1.bitangent = v1.bitangent + bitangent;
        v2.bitangent = v2.bitangent + bitangent;
    }

    for (auto& v : vertices) {
        // Gram-Schmidt orthogonalize
        float dotNT = v.normal.Dot(v.tangent);
        v.tangent = (v.tangent - v.normal * dotNT).Normalized();
        float dotNB = v.normal.Dot(v.bitangent);
        v.bitangent = (v.bitangent - v.normal * dotNB).Normalized();
    }
}

void Mesh3D::ComputeBounds() {
    if (vertices.empty()) {
        bounds = AABB3D(Vec3(0,0,0), Vec3(0,0,0));
        return;
    }
    Vec3 minP = vertices[0].position;
    Vec3 maxP = vertices[0].position;
    for (const auto& v : vertices) {
        minP.x = std::min(minP.x, v.position.x);
        minP.y = std::min(minP.y, v.position.y);
        minP.z = std::min(minP.z, v.position.z);
        maxP.x = std::max(maxP.x, v.position.x);
        maxP.y = std::max(maxP.y, v.position.y);
        maxP.z = std::max(maxP.z, v.position.z);
    }
    bounds = AABB3D(minP, maxP);
}

Mesh3D Mesh3D::CreateCube(float size) {
    Mesh3D mesh;
    mesh.name = "Cube";
    float h = size * 0.5f;

    struct Face {
        Vec3 normal;
        Vec3 corners[4];
        Vec2 uvs[4];
    };

    Face faces[6] = {
        // Front (+Z)
        { Vec3(0, 0, 1), { Vec3(-h, -h, h), Vec3(h, -h, h), Vec3(h, h, h), Vec3(-h, h, h) },
          { Vec2(0,0), Vec2(1,0), Vec2(1,1), Vec2(0,1) } },
        // Back (-Z)
        { Vec3(0, 0, -1), { Vec3(h, -h, -h), Vec3(-h, -h, -h), Vec3(-h, h, -h), Vec3(h, h, -h) },
          { Vec2(0,0), Vec2(1,0), Vec2(1,1), Vec2(0,1) } },
        // Right (+X)
        { Vec3(1, 0, 0), { Vec3(h, -h, h), Vec3(h, -h, -h), Vec3(h, h, -h), Vec3(h, h, h) },
          { Vec2(0,0), Vec2(1,0), Vec2(1,1), Vec2(0,1) } },
        // Left (-X)
        { Vec3(-1, 0, 0), { Vec3(-h, -h, -h), Vec3(-h, -h, h), Vec3(-h, h, h), Vec3(-h, h, -h) },
          { Vec2(0,0), Vec2(1,0), Vec2(1,1), Vec2(0,1) } },
        // Top (+Y)
        { Vec3(0, 1, 0), { Vec3(-h, h, h), Vec3(h, h, h), Vec3(h, h, -h), Vec3(-h, h, -h) },
          { Vec2(0,0), Vec2(1,0), Vec2(1,1), Vec2(0,1) } },
        // Bottom (-Y)
        { Vec3(0, -1, 0), { Vec3(-h, -h, -h), Vec3(h, -h, -h), Vec3(h, -h, h), Vec3(-h, -h, h) },
          { Vec2(0,0), Vec2(1,0), Vec2(1,1), Vec2(0,1) } },
    };

    for (int f = 0; f < 6; ++f) {
        uint32_t baseIdx = static_cast<uint32_t>(mesh.vertices.size());
        for (int v = 0; v < 4; ++v) {
            Vertex3D vert;
            vert.position = faces[f].corners[v];
            vert.normal = faces[f].normal;
            vert.texCoord = faces[f].uvs[v];
            vert.color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
            mesh.vertices.push_back(vert);
        }
        mesh.indices.push_back(baseIdx + 0);
        mesh.indices.push_back(baseIdx + 1);
        mesh.indices.push_back(baseIdx + 2);
        mesh.indices.push_back(baseIdx + 0);
        mesh.indices.push_back(baseIdx + 2);
        mesh.indices.push_back(baseIdx + 3);
    }

    mesh.CalculateTangents();
    mesh.ComputeBounds();
    return mesh;
}

Mesh3D Mesh3D::CreateSphere(float radius, int rings, int slices) {
    Mesh3D mesh;
    mesh.name = "Sphere";

    for (int r = 0; r <= rings; ++r) {
        float phi = Math::PI * static_cast<float>(r) / static_cast<float>(rings);
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);

        for (int s = 0; s <= slices; ++s) {
            float theta = Math::TWO_PI * static_cast<float>(s) / static_cast<float>(slices);
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            Vec3 norm(sinPhi * cosTheta, cosPhi, sinPhi * sinTheta);
            Vertex3D v;
            v.position = norm * radius;
            v.normal = norm;
            v.texCoord = Vec2(static_cast<float>(s) / slices, static_cast<float>(r) / rings);
            v.color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
            mesh.vertices.push_back(v);
        }
    }

    for (int r = 0; r < rings; ++r) {
        for (int s = 0; s < slices; ++s) {
            uint32_t first = r * (slices + 1) + s;
            uint32_t second = first + slices + 1;

            mesh.indices.push_back(first);
            mesh.indices.push_back(second);
            mesh.indices.push_back(first + 1);

            mesh.indices.push_back(second);
            mesh.indices.push_back(second + 1);
            mesh.indices.push_back(first + 1);
        }
    }

    mesh.CalculateTangents();
    mesh.ComputeBounds();
    return mesh;
}

Mesh3D Mesh3D::CreateCylinder(float radius, float height, int segments) {
    Mesh3D mesh;
    mesh.name = "Cylinder";
    float halfH = height * 0.5f;

    // Side vertices
    for (int i = 0; i <= segments; ++i) {
        float theta = Math::TWO_PI * static_cast<float>(i) / segments;
        float x = std::cos(theta);
        float z = std::sin(theta);

        Vertex3D topV;
        topV.position = Vec3(x * radius, halfH, z * radius);
        topV.normal = Vec3(x, 0.0f, z);
        topV.texCoord = Vec2(static_cast<float>(i) / segments, 1.0f);
        topV.color = Vec4(1, 1, 1, 1);

        Vertex3D botV;
        botV.position = Vec3(x * radius, -halfH, z * radius);
        botV.normal = Vec3(x, 0.0f, z);
        botV.texCoord = Vec2(static_cast<float>(i) / segments, 0.0f);
        botV.color = Vec4(1, 1, 1, 1);

        mesh.vertices.push_back(topV);
        mesh.vertices.push_back(botV);
    }

    for (int i = 0; i < segments; ++i) {
        uint32_t i0 = i * 2;
        uint32_t i1 = i * 2 + 1;
        uint32_t i2 = (i + 1) * 2;
        uint32_t i3 = (i + 1) * 2 + 1;

        mesh.indices.push_back(i0);
        mesh.indices.push_back(i1);
        mesh.indices.push_back(i2);

        mesh.indices.push_back(i2);
        mesh.indices.push_back(i1);
        mesh.indices.push_back(i3);
    }

    // Top cap
    uint32_t topCenterIdx = static_cast<uint32_t>(mesh.vertices.size());
    mesh.vertices.push_back({ Vec3(0, halfH, 0), Vec3(0, 1, 0), Vec2(0.5f, 0.5f), Vec4(1,1,1,1) });
    for (int i = 0; i <= segments; ++i) {
        float theta = Math::TWO_PI * static_cast<float>(i) / segments;
        mesh.vertices.push_back({ Vec3(std::cos(theta) * radius, halfH, std::sin(theta) * radius), Vec3(0, 1, 0), Vec2(0.5f + 0.5f * std::cos(theta), 0.5f + 0.5f * std::sin(theta)), Vec4(1,1,1,1) });
    }
    for (int i = 0; i < segments; ++i) {
        mesh.indices.push_back(topCenterIdx);
        mesh.indices.push_back(topCenterIdx + 1 + i);
        mesh.indices.push_back(topCenterIdx + 1 + i + 1);
    }

    // Bottom cap
    uint32_t botCenterIdx = static_cast<uint32_t>(mesh.vertices.size());
    mesh.vertices.push_back({ Vec3(0, -halfH, 0), Vec3(0, -1, 0), Vec2(0.5f, 0.5f), Vec4(1,1,1,1) });
    for (int i = 0; i <= segments; ++i) {
        float theta = Math::TWO_PI * static_cast<float>(i) / segments;
        mesh.vertices.push_back({ Vec3(std::cos(theta) * radius, -halfH, std::sin(theta) * radius), Vec3(0, -1, 0), Vec2(0.5f + 0.5f * std::cos(theta), 0.5f + 0.5f * std::sin(theta)), Vec4(1,1,1,1) });
    }
    for (int i = 0; i < segments; ++i) {
        mesh.indices.push_back(botCenterIdx);
        mesh.indices.push_back(botCenterIdx + 1 + i + 1);
        mesh.indices.push_back(botCenterIdx + 1 + i);
    }

    mesh.CalculateTangents();
    mesh.ComputeBounds();
    return mesh;
}

Mesh3D Mesh3D::CreateCone(float radius, float height, int segments) {
    Mesh3D mesh;
    mesh.name = "Cone";
    float halfH = height * 0.5f;

    Vertex3D tip;
    tip.position = Vec3(0.0f, halfH, 0.0f);
    tip.normal = Vec3(0.0f, 1.0f, 0.0f);
    tip.texCoord = Vec2(0.5f, 1.0f);
    tip.color = Vec4(1, 1, 1, 1);
    uint32_t tipIdx = 0;
    mesh.vertices.push_back(tip);

    for (int i = 0; i <= segments; ++i) {
        float theta = Math::TWO_PI * static_cast<float>(i) / segments;
        float x = std::cos(theta);
        float z = std::sin(theta);
        Vec3 norm = Vec3(x, radius / height, z).Normalized();

        Vertex3D v;
        v.position = Vec3(x * radius, -halfH, z * radius);
        v.normal = norm;
        v.texCoord = Vec2(static_cast<float>(i) / segments, 0.0f);
        v.color = Vec4(1, 1, 1, 1);
        mesh.vertices.push_back(v);
    }

    for (int i = 0; i < segments; ++i) {
        mesh.indices.push_back(tipIdx);
        mesh.indices.push_back(tipIdx + 1 + i);
        mesh.indices.push_back(tipIdx + 1 + i + 1);
    }

    // Base cap
    uint32_t baseCenterIdx = static_cast<uint32_t>(mesh.vertices.size());
    mesh.vertices.push_back({ Vec3(0, -halfH, 0), Vec3(0, -1, 0), Vec2(0.5f, 0.5f), Vec4(1,1,1,1) });
    for (int i = 0; i <= segments; ++i) {
        float theta = Math::TWO_PI * static_cast<float>(i) / segments;
        mesh.vertices.push_back({ Vec3(std::cos(theta) * radius, -halfH, std::sin(theta) * radius), Vec3(0, -1, 0), Vec2(0.5f, 0.5f), Vec4(1,1,1,1) });
    }
    for (int i = 0; i < segments; ++i) {
        mesh.indices.push_back(baseCenterIdx);
        mesh.indices.push_back(baseCenterIdx + 1 + i + 1);
        mesh.indices.push_back(baseCenterIdx + 1 + i);
    }

    mesh.CalculateTangents();
    mesh.ComputeBounds();
    return mesh;
}

Mesh3D Mesh3D::CreateTorus(float majorRadius, float minorRadius, int majorSegments, int minorSegments) {
    Mesh3D mesh;
    mesh.name = "Torus";

    for (int i = 0; i <= majorSegments; ++i) {
        float u = Math::TWO_PI * static_cast<float>(i) / majorSegments;
        float cosU = std::cos(u);
        float sinU = std::sin(u);

        for (int j = 0; j <= minorSegments; ++j) {
            float v = Math::TWO_PI * static_cast<float>(j) / minorSegments;
            float cosV = std::cos(v);
            float sinV = std::sin(v);

            Vec3 pos(
                (majorRadius + minorRadius * cosV) * cosU,
                minorRadius * sinV,
                (majorRadius + minorRadius * cosV) * sinU
            );

            Vec3 norm(cosV * cosU, sinV, cosV * sinU);

            Vertex3D vert;
            vert.position = pos;
            vert.normal = norm;
            vert.texCoord = Vec2(static_cast<float>(i) / majorSegments, static_cast<float>(j) / minorSegments);
            vert.color = Vec4(1, 1, 1, 1);
            mesh.vertices.push_back(vert);
        }
    }

    for (int i = 0; i < majorSegments; ++i) {
        for (int j = 0; j < minorSegments; ++j) {
            uint32_t first = i * (minorSegments + 1) + j;
            uint32_t second = first + minorSegments + 1;

            mesh.indices.push_back(first);
            mesh.indices.push_back(second);
            mesh.indices.push_back(first + 1);

            mesh.indices.push_back(second);
            mesh.indices.push_back(second + 1);
            mesh.indices.push_back(first + 1);
        }
    }

    mesh.CalculateTangents();
    mesh.ComputeBounds();
    return mesh;
}

Mesh3D Mesh3D::CreateCapsule(float radius, float height, int segments) {
    Mesh3D mesh = CreateCylinder(radius, height, segments);
    mesh.name = "Capsule";
    mesh.ComputeBounds();
    return mesh;
}

Mesh3D Mesh3D::CreatePyramid(float baseWidth, float height) {
    Mesh3D mesh;
    mesh.name = "Pyramid";
    float hBase = baseWidth * 0.5f;

    Vec3 apex(0.0f, height * 0.5f, 0.0f);
    Vec3 p0(-hBase, -height * 0.5f,  hBase);
    Vec3 p1( hBase, -height * 0.5f,  hBase);
    Vec3 p2( hBase, -height * 0.5f, -hBase);
    Vec3 p3(-hBase, -height * 0.5f, -hBase);

    // 4 triangle faces
    auto addTri = [&](const Vec3& a, const Vec3& b, const Vec3& c) {
        uint32_t base = static_cast<uint32_t>(mesh.vertices.size());
        Vec3 norm = (b - a).Cross(c - a).Normalized();
        mesh.vertices.push_back({ a, norm, Vec2(0.5f, 1.0f), Vec4(1,1,1,1) });
        mesh.vertices.push_back({ b, norm, Vec2(0.0f, 0.0f), Vec4(1,1,1,1) });
        mesh.vertices.push_back({ c, norm, Vec2(1.0f, 0.0f), Vec4(1,1,1,1) });
        mesh.indices.push_back(base);
        mesh.indices.push_back(base + 1);
        mesh.indices.push_back(base + 2);
    };

    addTri(apex, p0, p1);
    addTri(apex, p1, p2);
    addTri(apex, p2, p3);
    addTri(apex, p3, p0);

    // Base quad
    uint32_t base = static_cast<uint32_t>(mesh.vertices.size());
    Vec3 down(0, -1, 0);
    mesh.vertices.push_back({ p0, down, Vec2(0,0), Vec4(1,1,1,1) });
    mesh.vertices.push_back({ p3, down, Vec2(0,1), Vec4(1,1,1,1) });
    mesh.vertices.push_back({ p2, down, Vec2(1,1), Vec4(1,1,1,1) });
    mesh.vertices.push_back({ p1, down, Vec2(1,0), Vec4(1,1,1,1) });
    mesh.indices.push_back(base + 0);
    mesh.indices.push_back(base + 1);
    mesh.indices.push_back(base + 2);
    mesh.indices.push_back(base + 0);
    mesh.indices.push_back(base + 2);
    mesh.indices.push_back(base + 3);

    mesh.CalculateTangents();
    mesh.ComputeBounds();
    return mesh;
}

} // namespace Voidfall
