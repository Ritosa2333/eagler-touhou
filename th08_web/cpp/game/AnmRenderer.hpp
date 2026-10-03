#pragma once
#include "AnmLayout.hpp"
#include "Camera.hpp"
#include <vector>
#include "../../../portable/sdl/RenderCommands.hpp"

namespace th08 {
#ifdef TH_MULTIPLAYER_FIXTURES
bool fixture_projection_reuse(bool enabled);
#endif
using namespace touhou::graphics;
using touhou::graphics::DepthFunc;
using touhou::graphics::ColorOp;
using touhou::graphics::TextureArg;
using touhou::graphics::BlendMode;
struct SpriteVertex {
    Vec3 pos;
    float reciprocal_w = 1;
    u32 color = 0xffffffff;
    Vec2 uv;
};
static_assert(sizeof(SpriteVertex) == 28);
struct UntexturedVertex { Vec3 pos; float reciprocal_w=1; u32 color=0xffffffff; };
struct WorldVertex { Vec3 pos; Vec2 uv; };
struct ColoredWorldVertex { Vec3 pos; u32 color=0xffffffff; Vec2 uv; };
static_assert(sizeof(UntexturedVertex)==20 && sizeof(WorldVertex)==20 && sizeof(ColoredWorldVertex)==24);
#ifdef TH_NATIVE_PLATFORM
using Primitive=Topology;
enum class VertexFormat : u32 { World, ColoredWorld, Screen, Untextured };
#else
enum class Primitive : u32 { Triangles=4, Strip=5, Fan=6 };
enum class VertexFormat : u32 { World=0x102, ColoredWorld=0x142, Screen=0x144, Untextured=0x44 };
#endif

struct Viewport {
    u32 x = 0, y = 0, width = 640, height = 480;
    float min_z = 0, max_z = 1;
};

// The game submits vertices and state through this interface. The browser
// implementation owns graphics API objects; game code uses texture handles.
struct SpriteBackend
#ifdef TH_NATIVE_PLATFORM
 : StateCommands
#endif
{
    virtual ~SpriteBackend() = default;
    virtual void bind_texture(u32 texture) = 0;
    virtual void destination_blend(BlendParameter blend) = 0;
    virtual void write_depth(bool enabled) = 0;
    virtual void triangles(const SpriteVertex* vertices, u32 count) = 0;
    virtual void transform(MatrixParameter kind,const Matrix4& matrix) = 0;
    virtual void set_viewport(const Viewport& viewport) = 0;
    virtual void texture_factor(u32 color) = 0;
#ifndef TH_NATIVE_PLATFORM
    virtual void stage_state(u32 state,u32 value) = 0;
    virtual void render_state(u32 state,u32 value) = 0;
#endif
    virtual void vertex_format(VertexFormat format) = 0;
    virtual void draw(Primitive primitive,VertexFormat format,const void* vertices,u32 count) = 0;
    virtual void clear_target(u32 flags,u32 color,float depth,u32 stencil) = 0;
};

class AnmRenderer {
public:
    using ProjectionCallback = void (*)(AnmVm& vm, Vec3& projected, void* context);
    explicit AnmRenderer(SpriteBackend& backend);
    SpriteVertex quad[4];
    Viewport viewport;
    Matrix4 view_matrix, projection_matrix, last_world_matrix;
    Vec3 world_vertices[4]{{-128,-128,0},{128,-128,0},{-128,128,0},{128,128,0}};
    SceneCamera scene_camera;
    WorldVertex world_quad[4];
    ColoredWorldVertex colored_world_quad[4];
    Vec2 shake;
    u32 mix_color = 0x80808080;
    bool mix_enabled = false, depth_test_disabled = false;
    bool vertex_buffer_disabled=false, color_compositing_disabled=false;
    bool visual_geometry_suppressed=false;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    bool rollback_visual_geometry_suppressed=false;
#endif
    bool fog_disabled=false,fog_enabled=false;
    u32 current_texture = 0;
    u8 current_blend = 3, current_shader = 0xff, disable_z_write = 0xff;
    u8 camera_mode=0xff, diffuse_source=0, current_color_op=0xff;
    u32 current_factor=1;
    const AnmLoadedSprite* current_sprite=nullptr;
    u32 state_changes = 0, flushes = 0;

    i32 draw_no_rotation(AnmVm& vm, bool round = true);
    i32 draw_2d(AnmVm& vm, bool no_round = false);
    Matrix4 world_matrix(AnmVm& vm);
    void transform_world(AnmVm& vm);
    i32 draw_world(AnmVm& vm);
    i32 transform_facing_camera(AnmVm& vm, ProjectionCallback callback=nullptr, void* context=nullptr);
    i32 draw_facing_camera(AnmVm& vm, ProjectionCallback callback=nullptr, void* context=nullptr);
    void draw_player_bullet(AnmVm& vm);
    void set_3d_state(AnmVm& vm);
    i32 draw_3d(AnmVm& vm);
    i32 draw_vertices(AnmVm& vm,const SpriteVertex* vertices,i32 count);
    i32 draw_quad(AnmVm& vm,const SpriteVertex* vertices);
    i32 draw_fan(AnmVm& vm,const UntexturedVertex* vertices,i32 count);
    static i32 texture_strip(AnmVm& vm,SpriteVertex* vertices,i32 count,bool vertical);
    i32 draw_inner(AnmVm& vm, u32 flags);
    void set_state(AnmVm& vm);
    void add_quad(const SpriteVertex* vertices);
    void flush();
    void set_viewport(const Viewport& value) { flush(); viewport=value; backend.set_viewport(viewport); }
#ifndef TH_NATIVE_PLATFORM
    void set_render_state(u32 state,u32 value) { flush(); backend.render_state(state,value); }
#endif
    void set_depth_func(DepthFunc value){flush();RenderCommands(backend).SetDepthFunc(value);}
    void set_fog_color(u32 value){flush();RenderCommands(backend).SetFogColor(value);}
    void set_fog_range(float near_plane,float far_plane){flush();RenderCommands(backend).SetFogRange(near_plane,far_plane);}
    void set_fog_state(bool enabled){flush();RenderCommands(backend).SetFogEnabled(enabled);}
    void set_fog(bool enabled) { flush(); if(fog_enabled!=enabled){fog_enabled=enabled;RenderCommands(backend).SetFogEnabled(enabled);} }
    void background_camera(SceneCamera& value);
    void screen_camera();
    void begin_background();
    void clear_target(u32 flags,u32 color,float depth=1,u32 stencil=0){backend.clear_target(flags,color,depth,stencil);}
    void draw_rectangle(float left,float top,float right,float bottom,const u32* colors,bool shaded=false);
    void draw_dialogue_background(float left,float top,float right,float bottom);
    void draw_gui_strip(const UntexturedVertex* vertices);
    void draw_depth_mask(const UntexturedVertex* vertices,u32 count);
    void clear();
    u32 pending_sprites() const noexcept { return pending.size() / 6; }
    const std::vector<SpriteVertex>& pending_vertices() const noexcept { return pending; }

    static void translate_rotation(SpriteVertex& vertex, float x, float y,
                                   float sine, float cosine, float offset_x, float offset_y);
private:
    SpriteBackend& backend;
    std::vector<SpriteVertex> pending;
    bool drawable(const AnmVm& vm) const noexcept;
    u32 vm_color(const AnmVm& vm) const noexcept;
    void unrotated_vertices(AnmVm& vm, bool write_z);
};
}
