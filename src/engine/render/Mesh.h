#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include "core/math/MathCore.h"

namespace Voidfall {

using namespace Math;

struct Vertex3D {
    Vec3 position;
    Vec3 normal;
    Vec2 texCoord;
    Vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
    Vec3 tangent{1.0f, 0.0f, 0.0f};
    Vec3 bitangent{0.0f, 1.0f, 0.0f};
};

struct Triangle3D {
    Vertex3D v[3];
};

struct Light3D {
    enum class Type { DIRECTIONAL, POINT, SPOT };
    Type type{Type::DIRECTIONAL};
    Vec3 position{0.0f, 10.0f, 0.0f};
    Vec3 direction{0.0f, -1.0f, 0.0f};
    Vec3 color{1.0f, 1.0f, 1.0f};
    float intensity{1.0f};
    float range{50.0f};
    float innerCutoff{0.9f}; // cos(angle)
    float outerCutoff{0.8f};
};

struct Material3D {
    std::string name{"Default"};
    Vec3 albedoColor{0.8f, 0.8f, 0.85f};
    float metallic{0.2f};
    float roughness{0.5f};
    float ambientOcclusion{1.0f};
    Vec3 emissiveColor{0.0f, 0.0f, 0.0f};
    float emissiveIntensity{0.0f};
};

class Mesh3D {
public:
    std::string name{"Mesh"};
    std::vector<Vertex3D> vertices;
    std::vector<uint32_t> indices;
    Material3D material;
    AABB3D bounds;

    void CalculateNormals();
    void CalculateTangents();
    void ComputeBounds();

    // Procedural Primitives
    static Mesh3D CreateCube(float size = 1.0f);
    static Mesh3D CreateSphere(float radius = 1.0f, int rings = 16, int slices = 16);
    static Mesh3D CreateCylinder(float radius = 1.0f, float height = 2.0f, int segments = 16);
    static Mesh3D CreateCone(float radius = 1.0f, float height = 2.0f, int segments = 16);
    static Mesh3D CreateTorus(float majorRadius = 1.0f, float minorRadius = 0.3f, int majorSegments = 16, int minorSegments = 8);
    static Mesh3D CreateCapsule(float radius = 0.5f, float height = 1.0f, int segments = 12);
    static Mesh3D CreatePyramid(float baseWidth = 1.0f, float height = 1.5f);
};

} // namespace Voidfall
