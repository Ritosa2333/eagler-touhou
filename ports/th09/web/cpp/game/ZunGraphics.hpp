#pragma once
#include "AnmLayout.hpp"
#include "../../../portable/sdl/GraphicsState.hpp"
namespace th09 {
using namespace touhou::graphics;
struct SpriteVertex {Vec3 position;float reciprocal_w=1;u32 color=0xffffffff;Vec2 uv;};
struct ColorVertex {Vec3 position;float reciprocal_w=1;u32 color=0xffffffff;};
struct WorldVertex {Vec3 position;Vec2 uv;};
struct Viewport {u32 x=0,y=0,width=640,height=480;float near_depth=0,far_depth=1;};
static_assert(sizeof(SpriteVertex)==28&&sizeof(ColorVertex)==20&&sizeof(WorldVertex)==20);
// All game graphics use named state, textures, matrices and CPU vertices.
// Platform implementations own API objects and GPU buffer storage.
struct ZunGraphics {
    virtual ~ZunGraphics()=default;
    virtual void viewport(const Viewport&)=0;
    virtual void transform(MatrixKind,const Matrix4&)=0;
    virtual void clear(bool color,bool depth,u32 rgba,float z=1)=0;
    virtual void draw(const PipelineState&,u32 texture,Topology,VertexLayout,const void*,u32 vertices)=0;
    // A backend may directly consume the game's contiguous batch buffer.
    virtual void triangles(const PipelineState& state,u32 texture,const SpriteVertex* p,u32 vertices){draw(state,texture,Topology::Triangles,VertexLayout::ScreenColorUv,p,vertices);}
};
}
