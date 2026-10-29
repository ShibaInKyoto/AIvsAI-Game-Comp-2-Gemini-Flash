#pragma once

#include <vector>
#include <cstdint>
#include "core/math/MathCore.h"
#include "engine/render/Mesh.h"
#include "raylib.h"

#ifdef PI
#undef PI
#endif

namespace Voidfall {

using namespace Math;

struct RasterVertex {
    Vec4 clipPos;      // Clip space
    Vec3 screenPos;    // Viewport coordinates (x, y, z_depth in [0, 1])
    float invW{1.0f};  // 1.0f / clipPos.w
    Vec3 worldPos;
    Vec3 normal;
    Vec2 texCoord;
    Vec4 color;
};

class Rasterizer {
public:
    Rasterizer(int width, int height);
    ~Rasterizer();

    void Resize(int width, int height);
    void Clear(Color clearColor = Color{10, 12, 18, 255}, float depth = 1.0f);

    void SetCamera(const Vec3& eye, const Vec3& target, const Vec3& up, float fovDeg = 60.0f, float nearZ = 0.1f, float farZ = 100.0f);
    void SetViewport(int x, int y, int width, int height);

    void AddLight(const Light3D& light);
    void ClearLights();

    void DrawMesh(const Mesh3D& mesh, const Mat4& modelMatrix, bool wireframe = false);
    void DrawLine3D(const Vec3& p0, const Vec3& p1, Color color);
    void DrawPoint3D(const Vec3& p, Color color, float size = 2.0f);

    // Blitting & Raylib GPU Texture updates
    void UpdateTexture(Texture2D& texture);
    Image CreateRaylibImage() const;

    int GetWidth() const noexcept { return m_width; }
    int GetHeight() const noexcept { return m_height; }
    const uint32_t* GetColorBuffer() const noexcept { return m_colorBuffer.data(); }
    const float* GetDepthBuffer() const noexcept { return m_depthBuffer.data(); }

    void SetDepthTest(bool enable) noexcept { m_enableDepthTest = enable; }
    void SetCullBackfaces(bool enable) noexcept { m_cullBackfaces = enable; }
    void SetWireframeColor(Color col) noexcept { m_wireframeColor = col; }

private:
    void RasterizeTriangle(const RasterVertex& v0, const RasterVertex& v1, const RasterVertex& v2, const Material3D& material);
    void DrawLine2D(int x0, int y0, float z0, int x1, int y1, float z1, Color color);
    Color ShadePixel(const Vec3& worldPos, const Vec3& normal, const Vec2& uv, const Vec4& vertexColor, const Material3D& mat);

    int m_width;
    int m_height;
    int m_viewportX{0};
    int m_viewportY{0};
    int m_viewportW{0};
    int m_viewportH{0};

    std::vector<uint32_t> m_colorBuffer;
    std::vector<float> m_depthBuffer;

    Mat4 m_viewMatrix;
    Mat4 m_projMatrix;
    Mat4 m_viewProjMatrix;
    Vec3 m_cameraPos{0.0f, 0.0f, 5.0f};

    std::vector<Light3D> m_lights;
    bool m_enableDepthTest{true};
    bool m_cullBackfaces{true};
    Color m_wireframeColor{0, 255, 200, 255};
};

} // namespace Voidfall
