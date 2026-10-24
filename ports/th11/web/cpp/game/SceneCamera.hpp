#pragma once
#include "AnmProjection.hpp"
#include "FogInterpolation.hpp"
namespace th11 {
// Native renderer camera, 0x118 bytes (42a810 / 42a970 / 42aae0).
struct SceneCamera {
    Vec3 position{},direction{},up{0,1,0},reserved24{},right{},eye_offset{};
    float fov=0.5235987901687622f;
    Matrix4 view{},projection{};
    GraphicsViewport viewport;
    u32 mode=0;
    Vec2 offset{};
    Vec3 animation_delta{};
    SceneFog fog;
    void perspective();
    void screen();
};
static_assert(sizeof(SceneCamera)==0x118);
// Presentation-only blend of two fixed-tick camera endpoints.
SceneCamera blend_camera(const SceneCamera&,const SceneCamera&,float);
// Authored discontinuity gate: a large position step or a view-direction cut
// must snap presentation instead of interpolating across scene rebases.
bool camera_discontinuous(const SceneCamera&,const SceneCamera&,float position_limit=64.f,float direction_dot=.9f);
}
