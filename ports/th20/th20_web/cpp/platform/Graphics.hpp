// TH20 semantic graphics boundary for the Emscripten/SDL3 build.
//
// The reconstructed game code calls this typed interface at every point the
// original called IDirect3DDevice9/IDirect3DTexture9/IDirect3DSurface9. The
// shape is deliberately semantic (textures as CPU-backed pixel surfaces with
// lock/unlock, one render target slot, fixed-function pipeline state), mapped
// onto the shared touhou::sdl::Renderer by th20_web/cpp/sdl/GraphicsHost.cpp.
// There is no D3D9 emulation layer: game-visible state lives in
// touhou::graphics::PipelineState and matrices are plain float[16].
#pragma once

#include "GraphicsState.hpp"
#include "AssetPixelFormat.hpp"
#include <cstdint>

namespace th20::web {
using namespace touhou::graphics;

// Host-owned texture. Game code holds Texture* opaquely; pixel access happens
// through lock()/unlock() only (the D3D LockRect/UnlockRect shape).
struct Texture {
    std::uint32_t id = 0;          // renderer surface handle
    std::uint32_t width = 0, height = 0;
    PixelFormat format = PixelFormat::Bgra8;
    bool render_target = false;
};

struct TextureLock {
    void* pixels = nullptr;
    std::int32_t pitch = 0;
};

class GraphicsDevice : public StateCommands {
public:
    PipelineState& pipeline() override;

    // Device/backbuffer lifecycle (host side; TH_SDL3 replaces CreateDevice/Reset).
    bool create(std::uint32_t width, std::uint32_t height, PixelFormat back_format, std::uint32_t present_interval);
    bool reset_backbuffer(std::uint32_t width, std::uint32_t height, PixelFormat back_format, std::uint32_t present_interval);
    bool ready() const;

    // Frame boundary.
    void clear(std::uint32_t flags, std::uint32_t color, float depth, std::uint32_t stencil,
               const std::int32_t* rectangles = nullptr, std::uint32_t count = 0);
    void present();

    // Fixed-function state beyond PipelineState.
    void vertex_format(VertexLayout);
    void bind_texture(const Texture*);
    void transform(MatrixKind, const void* matrix16);
    void set_viewport(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, float zmin, float zmax);

    // Drawing. Vertices are game-owned CPU batches (DrawPrimitiveUP shape).
    void draw(Topology, std::uint32_t primitive_count, const void* vertices, std::uint32_t stride);
    void draw_triangle_batch(std::uint32_t triangle_count, const void* vertices, std::uint32_t stride);
    // The one true vertex buffer in the original: 9 precomputed corner quads
    // (TexturedCorner20). initialize_corner_buffer takes the locked contents.
    void initialize_corner_buffer(const void* vertices, std::uint32_t bytes, std::uint32_t stride);
    void draw_corner_strip(std::uint32_t first_vertex, std::uint32_t primitive_count);

    // Textures (CreateTexture/GetSurfaceLevel/GetLevelDesc/LockRect/UnlockRect).
    Texture* create_texture(std::uint32_t width, std::uint32_t height, PixelFormat, bool render_target);
    void release_texture(Texture*&);
    TextureLock lock(Texture*, const std::int32_t* rectangle = nullptr);
    void unlock(Texture*);
    void add_dirty_rect(Texture*);
    // PreLoad: upload to the GPU now instead of on first use.
    void preload(Texture*);
    // Immutable atlases no longer need a second decoded copy in WASM memory.
    void discard_cpu_copy(Texture*);
    std::uint32_t texture_width(const Texture*) const;
    std::uint32_t texture_height(const Texture*) const;

    // Render targets (SetRenderTarget; nullptr restores the backbuffer) and
    // the lockable backbuffer surface (GetBackBuffer + LockRect readbacks).
    void set_render_target(Texture*);
    Texture* back_buffer();
    // Surface-to-surface rectangle copy (D3DXLoadSurfaceFromSurface shape;
    // source may be back_buffer()). Blits on the GPU and keeps the CPU/GPU
    // revisions consistent.
    bool copy_surface(Texture* destination, const std::int32_t* destination_rect,
                      Texture* source, const std::int32_t* source_rect);

    // Device capabilities the game queries (CheckDeviceFormat/GetDeviceCaps).
    bool supports_texture_format(PixelFormat) const;
    std::uint32_t max_texture_width() const;
};

// The single shared device, created by the SDL host before the game starts.
GraphicsDevice& shared_device();
}
