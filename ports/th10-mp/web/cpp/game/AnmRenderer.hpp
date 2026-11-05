#pragma once
#include "../../../portable/sdl/RenderCommands.hpp"
#include "AnmManager.hpp"
namespace th10 {
using namespace touhou::graphics;
using touhou::graphics::DepthFunc;
using touhou::graphics::ColorOp;
using touhou::graphics::TextureArg;
using touhou::graphics::BlendMode;
struct RenderViewport {u32 x,y,width,height;};
struct AnmRenderEnvironment
#ifdef TH_NATIVE_PLATFORM
 : StateCommands
#endif
{
    AnmVertex* quad;
    const RenderViewport* viewport;
    AnmManager* global_manager=nullptr;
#ifndef TH_NATIVE_PLATFORM
    virtual void texture_stage(u32 stage,u32 state,u32 value)=0;
    void stage_state(u32 state,u32 value){texture_stage(0,state,value);}
    virtual void sampler_state(u32 stage,u32 state,u32 value)=0;
    virtual void render_state(u32 state,u32 value)=0;
#endif
    void texture_filter(bool point){
#ifdef TH_NATIVE_PLATFORM
        set_texture_filter(point?Filter::Nearest:Filter::Linear,point?Filter::Nearest:Filter::Linear);
#else
        sampler_state(0,5,point?1:2);sampler_state(0,6,point?1:2);
#endif
    }
    virtual void set_texture(void* texture)=0;
    virtual void vertex_format(LayoutParameter format)=0;
    virtual void draw_triangles(TopologyParameter primitive,u32 count,const void* vertices,u32 stride)=0;
    virtual void set_transform(MatrixParameter kind,const Matrix4& matrix)=0;
    virtual void stream_source(void* buffer,u32 stride)=0;
    virtual void draw_buffer(TopologyParameter primitive,u32 first,u32 count)=0;
    virtual i32 special_draw(AnmManager& manager,AnmVm& vm,u32 mode)=0;
};
struct AnmRenderer {
    AnmManager& manager;AnmRenderEnvironment& environment;
    static u32 axis_geometry(const AnmVm& vm,AnmVertex* quad,bool pixel_aligned) noexcept;
    static u32 rotated_geometry(const AnmVm& vm,AnmVertex* quad) noexcept;
    static void flipped_geometry(const AnmVm& vm,AnmVertex* quad) noexcept;
    static u32 modulate_channel(u32 color,u32 tint) noexcept;
    void flush();
    void apply_state(const AnmVm& vm);
    void apply_material(const AnmVm& vm);
    i32 append(const AnmVertex* quad) noexcept;
    void begin_frame() noexcept;
    i32 submit(const AnmVm& vm,u32 flags,bool flip_u=false);
    i32 draw_flipped(const AnmVm& vm);
    i32 submit_prebuilt(const AnmVm& vm,const AnmVertex* quad);
    i32 draw_color_mesh(const AnmVm& vm,const void* vertices,u32 count,bool fan);
    i32 draw_textured_fan(const AnmVm& vm,const void* vertices,u32 count);
    static i32 strip_uv(const AnmVm& vm,AnmVertex* vertices,i32 count,bool vertical) noexcept;
    i32 draw(AnmVm& vm);
};
}
