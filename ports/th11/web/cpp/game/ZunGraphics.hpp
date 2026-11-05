#pragma once
#include "AnmVm.hpp"
#include "GraphicsMath.hpp"
#include "../../../portable/sdl/GraphicsState.hpp"
namespace th11 {
// The game speaks in textures, matrices, blend factors and vertex layouts.
// This contract has no Windows device object or numeric D3D state dispatch.
class ZunGraphics:public touhou::graphics::StateCommands {
public:
    // 0x447270: stage-zero texture combiners and fixed-function defaults.
    void initialize_game_pipeline(){configure_game(1);}
    virtual u32 texture(const AnmResource& file,u32 index)=0;
    virtual void bind_texture(u32 handle)=0;
    virtual void set_layout(touhou::graphics::VertexLayout layout)=0;
    virtual void set_matrix(touhou::graphics::MatrixKind kind,const Matrix4& matrix)=0;
    virtual void triangles(u32 count,const void* vertices,u32 stride)=0;
    virtual void primitives(touhou::graphics::Topology topology,u32 count,const void* vertices,u32 stride)=0;
    virtual bool select_target(const AnmResource*,u32){return false;}
    virtual bool clear_target(u32,const GraphicsViewport*){return false;}
    virtual void set_viewport(const GraphicsViewport&){}
};
}
