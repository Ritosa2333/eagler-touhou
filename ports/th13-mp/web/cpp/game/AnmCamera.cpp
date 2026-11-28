#include "AnmCamera.hpp"
#include "AnmTransforms.hpp"
#include <cmath>
namespace th13 {
Vec3 project_vertex(const Vec3& p, const Matrix4& world, const AnmCamera& camera,
                    const GraphicsViewport& viewport) noexcept {
    double a[16], b[16];
    for (u32 i = 0; i < 16; i++)
        a[i] = double(world.m[i]);
    for (u32 i = 0; i < 16; i++)
        b[i] = double(camera.view.m[i]);
    double wv[16];
    for (u32 r = 0; r < 4; r++)
        for (u32 c = 0; c < 4; c++)
            wv[r * 4 + c] =
                a[r * 4 + 0] * b[c] + a[r * 4 + 1] * b[4 + c] + a[r * 4 + 2] * b[8 + c] + a[r * 4 + 3] * b[12 + c];
    for (u32 i = 0; i < 16; i++)
        b[i] = double(camera.projection.m[i]);
    double full[16];
    for (u32 r = 0; r < 4; r++)
        for (u32 c = 0; c < 4; c++)
            full[r * 4 + c] =
                wv[r * 4 + 0] * b[c] + wv[r * 4 + 1] * b[4 + c] + wv[r * 4 + 2] * b[8 + c] + wv[r * 4 + 3] * b[12 + c];
    const double x = double(p.x), y = double(p.y), z = double(p.z);
    const double cx = x * full[0] + y * full[4] + z * full[8] + full[12];
    const double cy = x * full[1] + y * full[5] + z * full[9] + full[13];
    const double cz = x * full[2] + y * full[6] + z * full[10] + full[14];
    const double cw = x * full[3] + y * full[7] + z * full[11] + full[15];
    const double sx = double(viewport.x) + (1.0 + cx / cw) * double(viewport.width) / 2.0;
    const double sy = double(viewport.y) + (1.0 - cy / cw) * double(viewport.height) / 2.0;
    const double sz =
        double(viewport.near_depth) + (cz / cw) * (double(viewport.far_depth) - double(viewport.near_depth));
    return {float(sx), float(sy), float(sz)};
}
int anm_projected_quad(AnmVm& vm, const AnmCamera& camera, const GraphicsViewport& viewport, Vec3 (&out)[4]) noexcept {
    auto& v = vm.visual;
    const Vec3 rotation = vm.total_rotation();
    Matrix4 world;
    world.identity();
    world.m[12] = float(float(v.translation.x + vm.variables.position.x) + v.child_anchor.x);
    world.m[13] = float(float(v.translation.y + vm.variables.position.y) + v.child_anchor.y);
    world.m[14] = float(float(v.translation.z + vm.variables.position.z) + v.child_anchor.z);
    const Vec3 center = project_vertex({}, world, camera, viewport);
    if (center.z < 0 || center.z > 1) return -1;
    const Vec3 reference = project_vertex(camera.reference, world, camera, viewport);
    const float dx = float(reference.x - center.x), dy = float(reference.y - center.y),
                dz = float(reference.z - center.z);
    const float dist2 = float(double(dy) * double(dy) + double(dx) * double(dx) + double(dz) * double(dz));
    const float length = float(std::sqrt(double(dist2))), factor = float(length * .5f);
    const float width =
        float((double(double(v.sprite_size.x) * double(factor)) * double(v.scale.x)) * double(v.secondary_scale.x));
    const float height =
        float((double(double(v.sprite_size.y) * double(factor)) * double(v.scale.y)) * double(v.secondary_scale.y));
    const float sine = float(std::sin(double(rotation.z))), cosine = float(std::cos(double(rotation.z)));
    const u32 ax = v.flags >> 21 & 3, ay = v.flags >> 23 & 3;
    if (ax > 2 || ay > 2) return -2;
    constexpr float xs[3][4] = {{-.5f, .5f, -.5f, .5f}, {0, 1, 0, 1}, {-1, 0, -1, 0}},
                    ys[3][4] = {{-.5f, -.5f, .5f, .5f}, {0, 0, 1, 1}, {-1, -1, 0, 0}};
    for (u32 i = 0; i < 4; i++) {
        const double x = double(xs[ax][i]) * double(width), y = double(ys[ay][i]) * double(height);
        out[i] = {float(double(x) * double(cosine) - double(y) * double(sine) + double(center.x)),
                  float(double(y) * double(cosine) + double(x) * double(sine) + double(center.y)), center.z};
    }
    return 0;
}
}
