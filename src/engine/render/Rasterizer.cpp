#include "Rasterizer.h"
#include <cmath>
#include <algorithm>
#include <cstring>

namespace Voidfall {

static inline uint32_t ColorToRGBA32(Color c) {
    return (static_cast<uint32_t>(c.a) << 24) |
           (static_cast<uint32_t>(c.b) << 16) |
           (static_cast<uint32_t>(c.g) << 8)  |
           (static_cast<uint32_t>(c.r));
}

Rasterizer::Rasterizer(int width, int height)
    : m_width(width), m_height(height),
      m_viewportX(0), m_viewportY(0),
      m_viewportW(width), m_viewportH(height)
{
    m_colorBuffer.resize(width * height, 0xFF000000);
    m_depthBuffer.resize(width * height, 1.0f);
    SetCamera(Vec3(0, 0, 5), Vec3(0, 0, 0), Vec3(0, 1, 0), 60.0f, 0.1f, 100.0f);
}

Rasterizer::~Rasterizer() = default;

void Rasterizer::Resize(int width, int height) {
    m_width = width;
    m_height = height;
    m_viewportX = 0;
    m_viewportY = 0;
    m_viewportW = width;
    m_viewportH = height;
    m_colorBuffer.resize(width * height);
    m_depthBuffer.resize(width * height);
}

void Rasterizer::SetViewport(int x, int y, int width, int height) {
    m_viewportX = x;
    m_viewportY = y;
    m_viewportW = width;
    m_viewportH = height;
}

void Rasterizer::Clear(Color clearColor, float depth) {
    uint32_t packed = ColorToRGBA32(clearColor);
    std::fill(m_colorBuffer.begin(), m_colorBuffer.end(), packed);
    std::fill(m_depthBuffer.begin(), m_depthBuffer.end(), depth);
}

void Rasterizer::SetCamera(const Vec3& eye, const Vec3& target, const Vec3& up, float fovDeg, float nearZ, float farZ) {
    m_cameraPos = eye;
    m_viewMatrix = Mat4::LookAt(eye, target, up);
    float aspect = (m_viewportH > 0) ? (static_cast<float>(m_viewportW) / static_cast<float>(m_viewportH)) : 1.333f;
    m_projMatrix = Mat4::Perspective(fovDeg * Math::DEG_TO_RAD, aspect, nearZ, farZ);
    m_viewProjMatrix = m_projMatrix * m_viewMatrix;
}

void Rasterizer::AddLight(const Light3D& light) {
    m_lights.push_back(light);
}

void Rasterizer::ClearLights() {
    m_lights.clear();
}

void Rasterizer::DrawMesh(const Mesh3D& mesh, const Mat4& modelMatrix, bool wireframe) {
    Mat4 mvp = m_viewProjMatrix * modelMatrix;

    for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
        const auto& origV0 = mesh.vertices[mesh.indices[i]];
        const auto& origV1 = mesh.vertices[mesh.indices[i + 1]];
        const auto& origV2 = mesh.vertices[mesh.indices[i + 2]];

        RasterVertex v0, v1, v2;

        v0.worldPos = modelMatrix.TransformPoint(origV0.position);
        v1.worldPos = modelMatrix.TransformPoint(origV1.position);
        v2.worldPos = modelMatrix.TransformPoint(origV2.position);

        v0.normal = modelMatrix.TransformVector(origV0.normal).Normalized();
        v1.normal = modelMatrix.TransformVector(origV1.normal).Normalized();
        v2.normal = modelMatrix.TransformVector(origV2.normal).Normalized();

        v0.texCoord = origV0.texCoord;
        v1.texCoord = origV1.texCoord;
        v2.texCoord = origV2.texCoord;

        v0.color = origV0.color;
        v1.color = origV1.color;
        v2.color = origV2.color;

        v0.clipPos = mvp.TransformVec4(Vec4(origV0.position.x, origV0.position.y, origV0.position.z, 1.0f));
        v1.clipPos = mvp.TransformVec4(Vec4(origV1.position.x, origV1.position.y, origV1.position.z, 1.0f));
        v2.clipPos = mvp.TransformVec4(Vec4(origV2.position.x, origV2.position.y, origV2.position.z, 1.0f));

        // Simple near clip check
        if (v0.clipPos.w <= 0.001f || v1.clipPos.w <= 0.001f || v2.clipPos.w <= 0.001f) {
            continue;
        }

        v0.invW = 1.0f / v0.clipPos.w;
        v1.invW = 1.0f / v1.clipPos.w;
        v2.invW = 1.0f / v2.clipPos.w;

        // Perspective divide -> NDC [-1, 1]
        Vec3 ndc0(v0.clipPos.x * v0.invW, v0.clipPos.y * v0.invW, v0.clipPos.z * v0.invW);
        Vec3 ndc1(v1.clipPos.x * v1.invW, v1.clipPos.y * v1.invW, v1.clipPos.z * v1.invW);
        Vec3 ndc2(v2.clipPos.x * v2.invW, v2.clipPos.y * v2.invW, v2.clipPos.z * v2.invW);

        // Viewport transform -> screen coords
        auto ndcToScreen = [&](const Vec3& ndc) -> Vec3 {
            return Vec3(
                m_viewportX + (ndc.x + 1.0f) * 0.5f * m_viewportW,
                m_viewportY + (1.0f - ndc.y) * 0.5f * m_viewportH,
                (ndc.z + 1.0f) * 0.5f // [0, 1] depth
            );
        };

        v0.screenPos = ndcToScreen(ndc0);
        v1.screenPos = ndcToScreen(ndc1);
        v2.screenPos = ndcToScreen(ndc2);

        // Backface culling in screen space (signed area)
        float signedArea = (v1.screenPos.x - v0.screenPos.x) * (v2.screenPos.y - v0.screenPos.y) -
                           (v1.screenPos.y - v0.screenPos.y) * (v2.screenPos.x - v0.screenPos.x);

        if (m_cullBackfaces && signedArea >= 0.0f) {
            continue; // Clockwise or degenerate
        }

        if (wireframe) {
            DrawLine2D(static_cast<int>(v0.screenPos.x), static_cast<int>(v0.screenPos.y), v0.screenPos.z,
                       static_cast<int>(v1.screenPos.x), static_cast<int>(v1.screenPos.y), v1.screenPos.z, m_wireframeColor);
            DrawLine2D(static_cast<int>(v1.screenPos.x), static_cast<int>(v1.screenPos.y), v1.screenPos.z,
                       static_cast<int>(v2.screenPos.x), static_cast<int>(v2.screenPos.y), v2.screenPos.z, m_wireframeColor);
            DrawLine2D(static_cast<int>(v2.screenPos.x), static_cast<int>(v2.screenPos.y), v2.screenPos.z,
                       static_cast<int>(v0.screenPos.x), static_cast<int>(v0.screenPos.y), v0.screenPos.z, m_wireframeColor);
        } else {
            RasterizeTriangle(v0, v1, v2, mesh.material);
        }
    }
}

static inline float EdgeFunction(const Vec3& a, const Vec3& b, const Vec2& c) {
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

void Rasterizer::RasterizeTriangle(const RasterVertex& v0, const RasterVertex& v1, const RasterVertex& v2, const Material3D& material) {
    // Screen bounding box clamped to viewport
    int minX = std::max(m_viewportX, static_cast<int>(std::floor(std::min({v0.screenPos.x, v1.screenPos.x, v2.screenPos.x}))));
    int maxX = std::min(m_viewportX + m_viewportW - 1, static_cast<int>(std::ceil(std::max({v0.screenPos.x, v1.screenPos.x, v2.screenPos.x}))));
    int minY = std::max(m_viewportY, static_cast<int>(std::floor(std::min({v0.screenPos.y, v1.screenPos.y, v2.screenPos.y}))));
    int maxY = std::min(m_viewportY + m_viewportH - 1, static_cast<int>(std::ceil(std::max({v0.screenPos.y, v1.screenPos.y, v2.screenPos.y}))));

    if (minX > maxX || minY > maxY) return;

    float area = EdgeFunction(v0.screenPos, v1.screenPos, Vec2(v2.screenPos.x, v2.screenPos.y));
    if (std::abs(area) < 0.00001f) return;
    float invArea = 1.0f / area;

    for (int y = minY; y <= maxY; ++y) {
        int rowOffset = y * m_width;
        for (int x = minX; x <= maxX; ++x) {
            Vec2 p(x + 0.5f, y + 0.5f);

            float w0 = EdgeFunction(v1.screenPos, v2.screenPos, p);
            float w1 = EdgeFunction(v2.screenPos, v0.screenPos, p);
            float w2 = EdgeFunction(v0.screenPos, v1.screenPos, p);

            // Top-left rule or inside test
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                w0 *= invArea;
                w1 *= invArea;
                w2 *= invArea;

                float depth = w0 * v0.screenPos.z + w1 * v1.screenPos.z + w2 * v2.screenPos.z;
                int pixelIdx = rowOffset + x;

                if (m_enableDepthTest && depth >= m_depthBuffer[pixelIdx]) {
                    continue; // Failed depth test
                }

                // Perspective-correct interpolation of attributes
                float interpolatedInvW = w0 * v0.invW + w1 * v1.invW + w2 * v2.invW;
                float wCorr = (interpolatedInvW > 0.000001f) ? (1.0f / interpolatedInvW) : 1.0f;

                Vec3 normal = (v0.normal * (w0 * v0.invW) +
                               v1.normal * (w1 * v1.invW) +
                               v2.normal * (w2 * v2.invW)) * wCorr;

                Vec3 worldPos = (v0.worldPos * (w0 * v0.invW) +
                                 v1.worldPos * (w1 * v1.invW) +
                                 v2.worldPos * (w2 * v2.invW)) * wCorr;

                Vec2 uv = (v0.texCoord * (w0 * v0.invW) +
                           v1.texCoord * (w1 * v1.invW) +
                           v2.texCoord * (w2 * v2.invW)) * wCorr;

                Vec4 color = (v0.color * (w0 * v0.invW) +
                              v1.color * (w1 * v1.invW) +
                              v2.color * (w2 * v2.invW)) * wCorr;

                Color litColor = ShadePixel(worldPos, normal.Normalized(), uv, color, material);

                m_depthBuffer[pixelIdx] = depth;
                m_colorBuffer[pixelIdx] = ColorToRGBA32(litColor);
            }
        }
    }
}

Color Rasterizer::ShadePixel(const Vec3& worldPos, const Vec3& normal, const Vec2&, const Vec4& vertexColor, const Material3D& mat) {
    Vec3 albedo = mat.albedoColor * Vec3(vertexColor.x, vertexColor.y, vertexColor.z);
    Vec3 viewDir = (m_cameraPos - worldPos).Normalized();

    // Base ambient light
    Vec3 ambientLight = albedo * 0.15f * mat.ambientOcclusion;
    Vec3 diffuseTotal(0.0f, 0.0f, 0.0f);
    Vec3 specTotal(0.0f, 0.0f, 0.0f);

    for (const auto& light : m_lights) {
        Vec3 lightDir;
        float attenuation = 1.0f;

        if (light.type == Light3D::Type::DIRECTIONAL) {
            lightDir = (light.direction * -1.0f).Normalized();
        } else {
            Vec3 lightVec = light.position - worldPos;
            float dist = lightVec.Length();
            if (dist > light.range) continue;
            lightDir = lightVec * (1.0f / (dist + 0.0001f));
            attenuation = 1.0f / (1.0f + 0.09f * dist + 0.032f * dist * dist);
        }

        // Lambert diffuse
        float NdotL = std::max(0.0f, normal.Dot(lightDir));
        diffuseTotal = diffuseTotal + light.color * (NdotL * light.intensity * attenuation);

        // Blinn-Phong specular with metallic modulation
        Vec3 halfDir = (lightDir + viewDir).Normalized();
        float NdotH = std::max(0.0f, normal.Dot(halfDir));
        float shininess = std::max(2.0f, (1.0f - mat.roughness) * 64.0f);
        float specFactor = std::pow(NdotH, shininess);

        Vec3 specColor = Math::Lerp(Vec3(0.04f, 0.04f, 0.04f), albedo, mat.metallic);
        specTotal = specTotal + specColor * (specFactor * light.intensity * attenuation);
    }

    Vec3 finalColor = ambientLight + albedo * diffuseTotal + specTotal;
    // Emissive contribution
    if (mat.emissiveIntensity > 0.0f) {
        finalColor = finalColor + mat.emissiveColor * mat.emissiveIntensity;
    }

    // Tone map & clamp
    auto saturate = [](float val) { return std::min(255, std::max(0, static_cast<int>(val * 255.0f))); };

    return Color{
        static_cast<unsigned char>(saturate(finalColor.x)),
        static_cast<unsigned char>(saturate(finalColor.y)),
        static_cast<unsigned char>(saturate(finalColor.z)),
        255
    };
}

void Rasterizer::DrawLine2D(int x0, int y0, float z0, int x1, int y1, float z1, Color color) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    int steps = std::max(dx, dy);
    float stepZ = (steps > 0) ? (z1 - z0) / steps : 0.0f;
    float currentZ = z0;

    uint32_t packed = ColorToRGBA32(color);

    int maxIterations = std::max(4000, steps * 2 + 10);
    while (maxIterations-- > 0) {
        if (x0 >= m_viewportX && x0 < m_viewportX + m_viewportW &&
            y0 >= m_viewportY && y0 < m_viewportY + m_viewportH) {
            int idx = y0 * m_width + x0;
            if (!m_enableDepthTest || currentZ < m_depthBuffer[idx]) {
                m_depthBuffer[idx] = currentZ;
                m_colorBuffer[idx] = packed;
            }
        }

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
        currentZ += stepZ;
    }
}

void Rasterizer::DrawLine3D(const Vec3& p0, const Vec3& p1, Color color) {
    Vec4 c0 = m_viewProjMatrix.TransformVec4(Vec4(p0.x, p0.y, p0.z, 1.0f));
    Vec4 c1 = m_viewProjMatrix.TransformVec4(Vec4(p1.x, p1.y, p1.z, 1.0f));

    if (c0.w <= 0.001f || c1.w <= 0.001f) return;

    Vec3 ndc0(c0.x / c0.w, c0.y / c0.w, c0.z / c0.w);
    Vec3 ndc1(c1.x / c1.w, c1.y / c1.w, c1.z / c1.w);

    int sX0 = static_cast<int>(m_viewportX + (ndc0.x + 1.0f) * 0.5f * m_viewportW);
    int sY0 = static_cast<int>(m_viewportY + (1.0f - ndc0.y) * 0.5f * m_viewportH);
    int sX1 = static_cast<int>(m_viewportX + (ndc1.x + 1.0f) * 0.5f * m_viewportW);
    int sY1 = static_cast<int>(m_viewportY + (1.0f - ndc1.y) * 0.5f * m_viewportH);

    DrawLine2D(sX0, sY0, (ndc0.z + 1.0f) * 0.5f, sX1, sY1, (ndc1.z + 1.0f) * 0.5f, color);
}

void Rasterizer::DrawPoint3D(const Vec3& p, Color color, float size) {
    Vec4 clip = m_viewProjMatrix.TransformVec4(Vec4(p.x, p.y, p.z, 1.0f));
    if (clip.w <= 0.001f) return;
    Vec3 ndc(clip.x / clip.w, clip.y / clip.w, clip.z / clip.w);
    int sx = static_cast<int>(m_viewportX + (ndc.x + 1.0f) * 0.5f * m_viewportW);
    int sy = static_cast<int>(m_viewportY + (1.0f - ndc.y) * 0.5f * m_viewportH);
    float depth = (ndc.z + 1.0f) * 0.5f;

    int half = static_cast<int>(size * 0.5f);
    uint32_t packed = ColorToRGBA32(color);

    for (int y = sy - half; y <= sy + half; ++y) {
        if (y < m_viewportY || y >= m_viewportY + m_viewportH) continue;
        for (int x = sx - half; x <= sx + half; ++x) {
            if (x < m_viewportX || x >= m_viewportX + m_viewportW) continue;
            int idx = y * m_width + x;
            if (!m_enableDepthTest || depth < m_depthBuffer[idx]) {
                m_depthBuffer[idx] = depth;
                m_colorBuffer[idx] = packed;
            }
        }
    }
}

void Rasterizer::UpdateTexture(Texture2D& texture) {
    ::UpdateTexture(texture, m_colorBuffer.data());
}

Image Rasterizer::CreateRaylibImage() const {
    Image img;
    img.width = m_width;
    img.height = m_height;
    img.mipmaps = 1;
    img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    img.data = RL_MALLOC(m_width * m_height * 4);
    std::memcpy(img.data, m_colorBuffer.data(), m_width * m_height * 4);
    return img;
}

} // namespace Voidfall
