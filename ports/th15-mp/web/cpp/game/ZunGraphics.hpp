#pragma once
#include "AnmResource.hpp"
#include "../../../portable/sdl/GraphicsState.hpp"
namespace th15 {
class AnmVm;
struct GraphicsViewport {u32 x=0,y=0,width=640,height=480;float near_depth=0,far_depth=1;};
// The shared backend receives semantic state, textures, matrices and vertices.
// Original Windows graphics objects and numeric device-state calls never cross
// this production boundary.
class ZunGraphics:public touhou::graphics::StateCommands {
public:
    virtual void presentation_offset(Vec2){}
    virtual void presentation_range(const AnmVm&,u32,u32,u32=0){}
    virtual void presentation_camera(bool){}
    virtual u32 texture(const AnmResource&,u32 index)=0;
    virtual void bind_texture(u32)=0;
    virtual void set_layout(touhou::graphics::VertexLayout)=0;
    virtual void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)=0;
    virtual void triangles(u32 count,const void*,u32 stride)=0;
    virtual void primitives(touhou::graphics::Topology,u32 count,const void*,u32 stride)=0;
    virtual bool select_target(const AnmResource*,u32)=0;
    virtual bool clear_target(u32,const GraphicsViewport*)=0;
    virtual bool clear_depth(const GraphicsViewport*)=0;
    virtual void set_viewport(const GraphicsViewport&)=0;
};
}
