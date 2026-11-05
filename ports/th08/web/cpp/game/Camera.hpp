#pragma once
#include "AnmLayout.hpp"
namespace th08 {
struct Viewport;
struct SceneCamera {
    Vec3 position{0,0,1000};
    Vec3 target_offset;
    Vec3 up{0,1,0};
    Vec3 unused24;
    Vec3 right;
    Vec3 eye_offset;
    float field_of_view=.5235987901687622f;
};
static_assert(sizeof(SceneCamera)==76);
struct Camera {
    static void screen(Matrix4& view,Matrix4& projection,const Viewport& viewport);
    static void scene(Matrix4& view,Matrix4& projection,const Viewport& viewport,SceneCamera& data);
};
}
