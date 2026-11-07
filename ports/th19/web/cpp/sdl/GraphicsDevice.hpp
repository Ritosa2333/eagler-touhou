// th19 SDL3 graphics device: th19's own flat 2D renderer interface backed by
// the shared portable/ SDL3+GLES3 renderer. Not a copy of th09's ZunGraphics —
// th19 draws 2D sprites only, so this is trimmed to what the title screen,
// menus and versus fields need.
#pragma once
#include "../../portable/sdl/GraphicsState.hpp"
#include "../../portable/sdl/Renderer.hpp"
#include "../game/Types.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>
namespace th19::sdl {
using namespace touhou::graphics;

// CPU vertex for screen-space textured quads (matches VertexLayout::ScreenColorUv).
struct SpriteVertex { th19::Vec3 position; float reciprocal_w=1; u32 color=0xffffffff; th19::Vec2 uv; u32 specular=0xffffffff; };
static_assert(sizeof(SpriteVertex)==32, "SpriteVertex must be 32 bytes");

struct Viewport { u32 x=0, y=0, width=1280, height=960; float near_depth=0, far_depth=1; };
struct TextureAllocation { u32 handle=0; u32 width=0, height=0; };

// Flat 2D graphics device. The game sees named state + CPU vertices; the device
// owns GPU storage through the shared portable renderer.
class GraphicsDevice {
public:
    GraphicsDevice();
    ~GraphicsDevice();

    bool initialize();
    TextureAllocation create_texture(const u8* bytes, u32 count);   // PNG/JPEG
    TextureAllocation create_raw(u32 width, u32 height, const u8* rgba);
    void destroy(u32 handle);
    void viewport(const Viewport&);
    void clear(u32 rgba);
    void draw_sprite(u32 texture, const PipelineState& state,
                     const SpriteVertex* quad, u32 vertices);
    void draw_line_strip(u32 texture,const PipelineState& state,const SpriteVertex* vertices,u32 count);
    void capture_screen(u32 texture);
    void present();

    const char* error() const { return error_.c_str(); }
    touhou::sdl::Surface surface(u32 id);   // resolve callback bridge

    // shared portable renderer (public so Application can set window size etc.)
    touhou::sdl::Renderer backend{0, resolve, this};
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    std::string error_;
    static touhou::sdl::Surface resolve(void*, u32);
};
}
