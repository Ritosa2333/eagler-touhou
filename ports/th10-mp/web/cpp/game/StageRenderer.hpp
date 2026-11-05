#pragma once
#include "Stage.hpp"
namespace th10 {
struct StageClearRect {i32 left,top,right,bottom;};
struct StageRenderEnvironment {
    Camera* world;
    Camera** active;
    u32* screen_space;
    u32* fog_enabled;
    float* rate;
    CameraEnvironment* camera;
    virtual void translation(Matrix4& matrix,const Vec3& offset)=0;
    virtual void project_points(Vec3* output,const Vec3* input,u32 count,const Camera& camera,const Matrix4& world)=0;
    virtual void draw_animation(AnmVm& vm)=0;
    virtual void draw_layer(u32 layer)=0;
    virtual void clear(u32 flags,u32 color,const StageClearRect* rectangle)=0;
    virtual void fade(i32 kind,i32 duration)=0;
    void activate_world(bool flat);
    void depth_mask(bool value);
    void depth_func(DepthFunc value);
    void fog_color(u32 value);
    void fog_range(float near_plane,float far_plane);
    void fog(bool enabled);
};
struct StageRenderer {
    Stage& stage;StageRenderEnvironment& environment;
    static i32 culled(const StageObject& object,const Vec3& instance,const Camera& camera,float distance_squared,StageRenderEnvironment& environment);
    i32 draw_objects(i32 layer);
    i32 draw_background();
    i32 draw_foreground();
    void copy_camera();
    void reset_tint();
    void apply_fog();
};
}
