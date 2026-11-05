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
#ifdef TH_CAMERA_CACHE_TEST
// Standalone numeric-oracle counters; absent from both runtime build profiles.
struct CameraCacheStats {
    u64 screen_hits=0,screen_computations=0,scene_hits=0,scene_computations=0;
};
#endif
struct Camera {
    static void screen(Matrix4& view,Matrix4& projection,const Viewport& viewport);
    static void scene(Matrix4& view,Matrix4& projection,const Viewport& viewport,SceneCamera& data);
#ifdef TH_CAMERA_CACHE_TEST
    static CameraCacheStats cache_stats() noexcept;
#endif
};
}
