#pragma once
#include "AnmResource.hpp"
#include "../../../portable/sdl/GraphicsState.hpp"
namespace th17 {
struct GraphicsViewport {u32 x=0,y=0,width=640,height=480;float near_depth=0,far_depth=1;};
// The shared backend receives semantic state, textures, matrices and vertices.
// Original Windows graphics objects and numeric device-state calls never cross
// this production boundary.
class ZunGraphics:public touhou::graphics::StateCommands {
public:
    virtual u32 texture(const AnmResource&,u32 index)=0;
    virtual void bind_texture(u32)=0;
    virtual void set_layout(touhou::graphics::VertexLayout)=0;
    virtual void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)=0;
    virtual void triangles(u32 count,const void*,u32 stride)=0;
    virtual bool supports_indexed_quads()const noexcept{return false;}
    virtual void quads(u32 count,const void* data,u32 stride){std::vector<u8> expanded(size_t(count)*6*stride);const auto* source=static_cast<const u8*>(data);const u32 order[]={0,1,2,1,2,3};for(u32 i=0;i<count;i++)for(u32 j=0;j<6;j++)std::memcpy(expanded.data()+size_t(i*6+j)*stride,source+size_t(i*4+order[j])*stride,stride);triangles(count*2,expanded.data(),stride);}
    virtual void primitives(touhou::graphics::Topology,u32 count,const void*,u32 stride)=0;
    virtual bool select_target(const AnmResource*,u32)=0;
    virtual bool clear_target(u32,const GraphicsViewport*)=0;
    virtual bool clear_depth(const GraphicsViewport*)=0;
    virtual void set_viewport(const GraphicsViewport&)=0;
};
}
