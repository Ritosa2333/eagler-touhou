#pragma once
#include "AnmCoordinates.hpp"
#include "ZunGraphics.hpp"
#include "AnmTransforms.hpp"
#include "AnmCamera.hpp"
namespace th15 {
class AnmRenderer {
    ZunGraphics& graphics;std::vector<AnmGeometryVertex> vertices;u32 bound_texture=~0u;
    struct ColorVertex {Vec3 position;float reciprocal_w;u32 color;};std::vector<ColorVertex> shapes;
    bool material(AnmVm&,bool textured=true);int quad(AnmVm&,bool pixel);int pack_quad(AnmVm&,Vec3 (&)[4],bool pixel,const u32* colors=nullptr);int projected_quad(AnmVm&,bool fog);int world_quad(AnmVm&);int mesh(AnmVm&);int shape(AnmVm&);int trail(AnmVm&);
    u32 tint_color(u32)const noexcept;int overlay(AnmVm&);
public:
    Vec2 offset{};GraphicsViewport viewport;AnmCamera camera;bool tint_enabled=false;u32 tint=0x80808080;
    std::string error;explicit AnmRenderer(ZunGraphics& graphics):graphics(graphics){vertices.reserve(6144);shapes.reserve(192);}
    void flush();void invalidate();void set_viewport(const GraphicsViewport&);void set_camera(const AnmCamera&);
    int draw(AnmVm&);int draw_glyph(AnmVm&);bool draw_layer(const std::vector<AnmVm*>&);
    int draw_screen_strip(AnmVm&,const AnmGeometryVertex*,u32 count);
    const std::vector<AnmGeometryVertex>& pending()const noexcept{return vertices;}
};
}
