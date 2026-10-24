#pragma once
#include "AnmCoordinates.hpp"
#include "ZunGraphics.hpp"
#include "AnmTransforms.hpp"
#include "AnmCamera.hpp"
#include <unordered_map>
namespace th15 {
class AnmRenderer {
    ZunGraphics& graphics;std::vector<AnmGeometryVertex> vertices;u32 bound_texture=~0u;
    struct ColorVertex {Vec3 position;float reciprocal_w;u32 color;};std::vector<ColorVertex> shapes;
#if TH15_MULTIPLAYER
    // Keep both headers and backing storage in the renderer's journaled heap,
    // just like vertices/shapes. Reuse them only after synchronous submission.
    std::vector<AnmGeometryVertex> opacity_screen_vertices;std::vector<AnmWorldVertex> opacity_world_vertices;
#endif
    int draw_body(AnmVm&,const AnmGeometry&,u32 opacity=255);
    bool material(AnmVm&,bool textured=true);int quad(AnmVm&,bool pixel);int pack_quad(AnmVm&,Vec3 (&)[4],bool pixel,const u32* colors=nullptr);int projected_quad(AnmVm&,bool fog);int world_quad(AnmVm&);int mesh(AnmVm&,const AnmGeometry&,u32 opacity);int shape(AnmVm&);int trail(AnmVm&,const AnmTrail&);
    u32 tint_color(u32)const noexcept;int overlay(AnmVm&);
public:
    u32 presentation_instance=0;
#if TH15_MULTIPLAYER
    std::vector<const AnmVm*> multiplayer_hidden_hud;
    // Bit 8 is a temporary stale marker while mp_prepare_presentation runs.
    // Completed preparation exposes only the original 0..255 opacity values.
    std::unordered_map<const AnmVm*,u16> multiplayer_player_opacity;
    std::vector<const AnmVm*> multiplayer_hidden_hitboxes;
    std::unordered_map<u32,unsigned> multiplayer_hitbox_owners;
#endif
    void submit(AnmVm&,touhou::graphics::Topology,u32,const void*,u32);
    Vec2 offset{};GraphicsViewport viewport;AnmCamera camera;bool tint_enabled=false;u32 tint=0x80808080;
    std::string error;explicit AnmRenderer(ZunGraphics& graphics):graphics(graphics){vertices.reserve(6144);shapes.reserve(192);}
    void flush();void invalidate();void set_viewport(const GraphicsViewport&);void set_camera(const AnmCamera&);
    int draw(AnmVm&);int draw_glyph(AnmVm&);bool draw_layer(const std::vector<AnmVm*>&);
    int draw_screen_strip(AnmVm&,const AnmGeometryVertex*,u32 count);
    const std::vector<AnmGeometryVertex>& pending()const noexcept{return vertices;}
};
}
