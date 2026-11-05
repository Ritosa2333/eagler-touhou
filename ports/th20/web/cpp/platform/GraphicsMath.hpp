// Float matrix/vector math replacing the D3DX9 calls the original made
// through d3dx9_43.dll (LoadLibrary/GetProcAddress). D3DX computes these in
// single precision; the implementations below keep the same float operation
// order (and the build uses -ffp-contract=off, so no FMA contraction).
// Matrices are row-major float[16] with the D3D row-vector convention, layout
// compatible with D3DMATRIX.m and the game's Matrix4.elements.
#pragma once

#include <cstdint>
#include <cmath>
#include <cstring>

namespace th20::web::math {

struct Vec3 { float x, y, z; };
struct Vec4 { float x, y, z, w; };
struct Viewport { std::uint32_t x, y, width, height; float min_z, max_z; };

inline float sine(float a) { return std::sin(a); }
inline float cosine(float a) { return std::cos(a); }

inline void identity(float* m) {
    std::memset(m, 0, 64);
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

// D3DXMatrixMultiply: out = a * b, float accumulation in ascending k order.
inline void multiply(float* out, const float* a, const float* b) {
    float r[16];
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            float sum = a[i * 4] * b[j];
            sum += a[i * 4 + 1] * b[4 + j];
            sum += a[i * 4 + 2] * b[8 + j];
            sum += a[i * 4 + 3] * b[12 + j];
            r[i * 4 + j] = sum;
        }
    std::memcpy(out, r, 64);
}

inline void rotation_x(float* m, float angle) {
    identity(m);
    const float s = sine(angle), c = cosine(angle);
    m[5] = c; m[6] = s; m[9] = -s; m[10] = c;
}
inline void rotation_y(float* m, float angle) {
    identity(m);
    const float s = sine(angle), c = cosine(angle);
    m[0] = c; m[2] = -s; m[8] = s; m[10] = c;
}
inline void rotation_z(float* m, float angle) {
    identity(m);
    const float s = sine(angle), c = cosine(angle);
    m[0] = c; m[1] = s; m[4] = -s; m[5] = c;
}
inline void rotation_axis(float* m, unsigned axis, float angle) {
    if (axis == 0) rotation_x(m, angle);
    else if (axis == 1) rotation_y(m, angle);
    else rotation_z(m, angle);
}

inline void translation(float* m, float x, float y, float z) {
    identity(m);
    m[12] = x; m[13] = y; m[14] = z;
}

// D3DXVec3Transform: v * m with implicit w=1, returns x,y,z,w.
inline Vec4 transform(const Vec3& v, const float* m) {
    const float x = v.x * m[0] + v.y * m[4] + v.z * m[8] + m[12];
    const float y = v.x * m[1] + v.y * m[5] + v.z * m[9] + m[13];
    const float z = v.x * m[2] + v.y * m[6] + v.z * m[10] + m[14];
    const float w = v.x * m[3] + v.y * m[7] + v.z * m[11] + m[15];
    return {x, y, z, w};
}

// D3DXVec3TransformCoord: transform then divide by w.
inline Vec3 transform_coord(const Vec3& v, const float* m) {
    const Vec4 t = transform(v, m);
    return {t.x / t.w, t.y / t.w, t.z / t.w};
}

// D3DXVec3Project: world -> view -> projection, homogeneous divide, viewport map.
inline Vec3 project(const Vec3& v, const Viewport& vp,
                    const float* projection, const float* view, const float* world) {
    float m[16];
    multiply(m, world, view);
    multiply(m, m, projection);
    const Vec4 t = transform(v, m);
    const float iw = 1.0f / t.w;
    Vec3 out;
    out.x = (t.x * iw + 1.0f) * (vp.width * 0.5f) + vp.x;
    out.y = (1.0f - t.y * iw) * (vp.height * 0.5f) + vp.y;
    out.z = t.z * iw * (vp.max_z - vp.min_z) + vp.min_z;
    return out;
}

// D3DXVec3ProjectArray: project n vectors with independent strides.
inline void project_array(Vec3* out, std::uint32_t out_stride, const Vec3* in, std::uint32_t in_stride,
                          std::uint32_t n, const Viewport& vp,
                          const float* projection, const float* view, const float* world) {
    float m[16];
    multiply(m, world, view);
    multiply(m, m, projection);
    auto* dst = reinterpret_cast<std::uint8_t*>(out);
    auto* src = reinterpret_cast<const std::uint8_t*>(in);
    for (std::uint32_t i = 0; i < n; ++i, dst += out_stride, src += in_stride) {
        const Vec4 t = transform(*reinterpret_cast<const Vec3*>(src), m);
        const float iw = 1.0f / t.w;
        Vec3 r;
        r.x = (t.x * iw + 1.0f) * (vp.width * 0.5f) + vp.x;
        r.y = (1.0f - t.y * iw) * (vp.height * 0.5f) + vp.y;
        r.z = t.z * iw * (vp.max_z - vp.min_z) + vp.min_z;
        std::memcpy(dst, &r, sizeof(Vec3));
    }
}

// D3DXMatrixLookAtLH.
inline void look_at(float* m, const Vec3& eye, const Vec3& target, const Vec3& up) {
    Vec3 z{target.x - eye.x, target.y - eye.y, target.z - eye.z};
    const float zl = std::sqrt(z.x * z.x + z.y * z.y + z.z * z.z);
    z.x /= zl; z.y /= zl; z.z /= zl;
    Vec3 x{up.y * z.z - up.z * z.y, up.z * z.x - up.x * z.z, up.x * z.y - up.y * z.x};
    const float xl = std::sqrt(x.x * x.x + x.y * x.y + x.z * x.z);
    x.x /= xl; x.y /= xl; x.z /= xl;
    const Vec3 y{z.y * x.z - z.z * x.y, z.z * x.x - z.x * x.z, z.x * x.y - z.y * x.x};
    m[0] = x.x; m[1] = y.x; m[2] = z.x; m[3] = 0.0f;
    m[4] = x.y; m[5] = y.y; m[6] = z.y; m[7] = 0.0f;
    m[8] = x.z; m[9] = y.z; m[10] = z.z; m[11] = 0.0f;
    m[12] = -(x.x * eye.x + x.y * eye.y + x.z * eye.z);
    m[13] = -(y.x * eye.x + y.y * eye.y + y.z * eye.z);
    m[14] = -(z.x * eye.x + z.y * eye.y + z.z * eye.z);
    m[15] = 1.0f;
}

// D3DXMatrixPerspectiveFovLH.
inline void perspective_fov_lh(float* m, float fov, float aspect, float near_plane, float far_plane) {
    std::memset(m, 0, 64);
    const float y_scale = 1.0f / std::tan(fov * 0.5f);
    m[0] = y_scale / aspect;
    m[5] = y_scale;
    m[10] = far_plane / (far_plane - near_plane);
    m[11] = 1.0f;
    m[14] = -near_plane * far_plane / (far_plane - near_plane);
}
}
