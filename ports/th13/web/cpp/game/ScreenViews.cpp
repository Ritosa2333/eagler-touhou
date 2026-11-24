#include "ScreenViews.hpp"
#include <cmath>
#include <cstdio>
#include <cstring>
namespace th13 {
ScreenView screen_view(const GraphicsViewport& viewport, float fov, Vec2 offset) noexcept {
    ScreenView out;
    out.viewport = viewport;
    out.offset = offset;
    out.camera.projection = scene_perspective(fov, 1.3333334f, 1.f, 10000.f);
    const float tan_half = float(std::tan(double(fov * 0.5f)));
    const float distance = float(240.0 / tan_half);
    out.camera.view = scene_look_at({320, 240, distance}, {320, 240, 0}, {0, -1, 0});
    return out;
}
void ScreenViews::configure(const AnmEnvironment& environment) {
    this->environment = &environment;
    const float scale = environment.resolution_scale;
    const auto scaled = [&](float value) { return u32(truncate_int(float(value * scale))); };
    views[2] = screen_view({0, 0, environment.screen_width, environment.screen_height, 0, 1}, fov);
    views[0] = screen_view({scaled(32), scaled(16), scaled(384), scaled(448), 0, 1}, fov);
    views[1] = screen_view({scaled(32), scaled(16), scaled(384), scaled(448), 0, 1}, fov);
    views[3] =
        screen_view({u32(truncate_int(float(float(float(environment.screen_width) - 408.f) * .5f))),
                     u32(truncate_int(float(float(float(environment.screen_height) - 476.f) * .5f))), 408, 476, 0, 1},
                    fov);
}
bool ScreenViews::loading_camera() {
    const u32 width = environment ? u32(environment->screen_width) : 640u;
    const u32 height = environment ? u32(environment->screen_height) : 480u;
    ScreenView view = screen_view({0, 0, width, height, 0, 1}, fov);
    renderer.set_viewport(view.viewport);
    renderer.set_camera(view.camera);
    renderer.offset = {};
    return true;
}
namespace {
bool same_viewport(const GraphicsViewport& a, const GraphicsViewport& b) noexcept {
    return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height && a.near_depth == b.near_depth &&
           a.far_depth == b.far_depth;
}
bool same_camera(const AnmCamera& a, const AnmCamera& b) noexcept {
    return a.eye.x == b.eye.x && a.eye.y == b.eye.y && a.eye.z == b.eye.z && a.reference.x == b.reference.x &&
           a.reference.y == b.reference.y && a.reference.z == b.reference.z &&
           std::memcmp(a.view.m, b.view.m, sizeof a.view.m) == 0 &&
           std::memcmp(a.projection.m, b.projection.m, sizeof a.projection.m) == 0 && a.fog_near == b.fog_near &&
           a.fog_far == b.fog_far && a.fog_rgb.x == b.fog_rgb.x && a.fog_rgb.y == b.fog_rgb.y &&
           a.fog_rgb.z == b.fog_rgb.z && a.fog_color == b.fog_color;
}
}

bool ScreenViews::camera_if_changed(DrawCamera index, bool refresh) {
    auto& view = views[u32(index)];
    if (refresh) view = screen_view(view.viewport, fov, view.offset);
    if (same_viewport(view.viewport, renderer.viewport) && same_camera(view.camera, renderer.camera)) {
        const Vec2 shake = environment ? environment->screen_shake_offset : Vec2{};
        renderer.offset = {float(view.offset.x + shake.x), float(view.offset.y + shake.y)};
        return true;
    }
    return camera(index, false);
}
bool ScreenViews::camera(DrawCamera index, bool refresh) {
    auto& view = views[u32(index)];
    if (refresh) view = screen_view(view.viewport, fov, view.offset);
    renderer.set_viewport(view.viewport);
    renderer.set_camera(view.camera);
    const Vec2 shake = environment ? environment->screen_shake_offset : Vec2{};
    renderer.offset = {float(view.offset.x + shake.x), float(view.offset.y + shake.y)};
    return true;
}
}
