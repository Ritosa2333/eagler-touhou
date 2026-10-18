#pragma once
#include "AnmProjection.hpp"
namespace th10 {
struct CameraEnvironment;
struct CameraViewport {u32 x,y,width,height;float near_depth,far_depth;};
struct Camera {
    Vec3 position,target_offset,up;
    Vec3 reserved_024,right,eye_offset;
    float field_of_view;
    Matrix4 view,projection;
    CameraViewport viewport;
    u32 screen_space;
    Vec2 draw_offset;
    Vec3 animation_delta;
    AnmFog fog;
    static void initialize(Camera& world,Camera& ui) noexcept;
    void configure_flat(CameraEnvironment& environment);
    void configure_world(CameraEnvironment& environment);
};
static_assert(sizeof(Camera)==0x118);
static_assert(offsetof(Camera,view)==0x4c&&offsetof(Camera,viewport)==0xcc);
static_assert(offsetof(Camera,draw_offset)==0xe8&&offsetof(Camera,fog)==0xfc);
struct CameraEnvironment {
    AnmManager** animation_manager;
    AnmRenderEnvironment* render_environment;
    virtual void look_at(Matrix4& matrix,const Vec3& eye,const Vec3& target,const Vec3& up)=0;
    virtual void perspective(Matrix4& matrix,float field_of_view,float aspect,float near_plane,float far_plane)=0;
    virtual void normalize(Vec3& value)=0;
    virtual void set_viewport(const CameraViewport& viewport)=0;
    void flush();
    void copy_offset(const Camera& camera);
};
}
