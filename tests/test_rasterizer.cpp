#include "engine/render/Mesh.h"
#include "engine/render/Rasterizer.h"
#include "engine/animation/Skeleton.h"
#include "engine/render/ProceduralMech3D.h"
#include <cassert>
#include <iostream>
#include <cmath>

namespace Voidfall {

using namespace Math;

void RunRasterizerTests() {
    std::cout << "[TEST] Running 3D Rasterizer & Mesh Tests...\n";

    // 1. Primitive Generation
    Mesh3D cube = Mesh3D::CreateCube(2.0f);
    assert(cube.vertices.size() == 24);
    assert(cube.indices.size() == 36);
    assert(std::abs(cube.bounds.min.x - (-1.0f)) < 0.001f);
    assert(std::abs(cube.bounds.max.x - (1.0f)) < 0.001f);

    Mesh3D sphere = Mesh3D::CreateSphere(1.0f, 8, 8);
    assert(!sphere.vertices.empty());
    assert(!sphere.indices.empty());

    Mesh3D cylinder = Mesh3D::CreateCylinder(1.0f, 2.0f, 8);
    assert(!cylinder.vertices.empty());

    // 2. Rasterizer initialization & clear
    Rasterizer raster(320, 240);
    assert(raster.GetWidth() == 320);
    assert(raster.GetHeight() == 240);

    Color clearCol = Color{20, 25, 35, 255};
    raster.Clear(clearCol, 1.0f);

    const uint32_t* colorBuf = raster.GetColorBuffer();
    const float* depthBuf = raster.GetDepthBuffer();
    assert(depthBuf[0] == 1.0f);
    assert((colorBuf[0] & 0xFF) == 20);

    // 3. Lighting & Mesh Rasterization
    Light3D dirLight;
    dirLight.type = Light3D::Type::DIRECTIONAL;
    dirLight.direction = Vec3(0, -1, -1).Normalized();
    dirLight.color = Vec3(1, 1, 1);
    dirLight.intensity = 1.0f;
    raster.AddLight(dirLight);

    raster.SetCamera(Vec3(0, 0, 4), Vec3(0, 0, 0), Vec3(0, 1, 0), 60.0f, 0.1f, 50.0f);
    Mat4 cubeModel = Mat4::RotationY(0.4f) * Mat4::RotationX(0.2f);
    raster.DrawMesh(cube, cubeModel, false);

    // Check that some depth values were written closer than 1.0f
    bool depthWritten = false;
    for (int i = 0; i < 320 * 240; ++i) {
        if (depthBuf[i] < 0.999f) {
            depthWritten = true;
            break;
        }
    }
    assert(depthWritten);

    // 4. Skeleton & IK Solving
    Skeleton skel;
    int root = skel.AddJoint("Root", -1, Vec3(0, 0, 0));
    int hip = skel.AddJoint("Hip", root, Vec3(0, 1.5f, 0));
    int knee = skel.AddJoint("Knee", hip, Vec3(0, -0.8f, 0));
    int foot = skel.AddJoint("Foot", knee, Vec3(0, -0.8f, 0));
    skel.UpdateHierarchy();

    assert(skel.GetJointCount() == 4);
    assert(skel.FindJointIndex("Knee") == knee);

    // Test Two-Bone IK
    Vec3 rAxis, mAxis;
    float rAng = 0.0f, mAng = 0.0f;
    bool ikSolved = Skeleton::SolveTwoBoneIK(
        rAxis, rAng, mAxis, mAng,
        Vec3(0, 1.6f, 0), Vec3(0, 0.8f, 0), Vec3(0, 0, 0),
        Vec3(0.2f, 0.1f, 0.3f), Vec3(0, 0, 1)
    );
    assert(ikSolved);

    // 5. Procedural Mech 3D
    ProceduralMech3D mech(MechChassisClass::VANGUARD_STRIKER);
    mech.WalkCycle(1.0f, 0.016f);
    mech.AimTorso(Vec3(10, 2, 10), Mat4::Identity(), 0.016f);
    raster.Clear();
    mech.Render(raster, Mat4::Identity(), false);

    std::cout << "[PASS] 3D Rasterizer & Mesh Tests passed!\n";
}

} // namespace Voidfall
