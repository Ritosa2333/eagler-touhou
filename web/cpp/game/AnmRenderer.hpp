#pragma once
#include "AnmCoordinates.hpp"
#include "ZunGraphics.hpp"
#include "AnmTransforms.hpp"
#include "AnmCamera.hpp"
namespace th17 {
class AnmRenderer {
    ZunGraphics& graphics;touhou::graphics::PipelineState& render_state;const bool indexed_quads;
    std::vector<AnmGeometryVertex> vertices;u32 vertex_count=0,bound_texture=~0u;bool quad_batch=false;
    struct ColorVertex {Vec3 position;float reciprocal_w;u32 color;};std::vector<ColorVertex> shapes;
    bool material(AnmVm&,bool textured=true);int quad(AnmVm&,bool pixel);int pack_quad(AnmVm&,Vec3 (&)[4],bool pixel,const u32* colors=nullptr);int projected_quad(AnmVm&,bool fog);int world_quad(AnmVm&);int mesh(AnmVm&);int shape(AnmVm&);int trail(AnmVm&);
    u32 tint_color(u32)const noexcept;int overlay(AnmVm&);
public:
    Vec2 offset{};GraphicsViewport viewport;AnmCamera camera;bool tint_enabled=false;u32 tint=0x80808080;
    // A triangle strip can leave 6141 vertices before a six-vertex sprite.
    // Retain the original post-append flush threshold, including that tail.
    // Both graphics implementations keep their pipeline object and quad
    // capability for their lifetime. Reuse that reference, not a state copy.
    std::string error;explicit AnmRenderer(ZunGraphics& graphics):graphics(graphics),render_state(graphics.pipeline()),indexed_quads(graphics.supports_indexed_quads()),vertices(6150){shapes.reserve(192);}
    void flush();void invalidate();void set_viewport(const GraphicsViewport&);void set_camera(const AnmCamera&);
    int draw(AnmVm&);int draw_glyph(AnmVm& vm){return quad(vm,true);}bool draw_layer(const std::vector<AnmVm*>&);
    int draw_screen_strip(AnmVm&,const AnmGeometryVertex*,u32 count);
};
}
