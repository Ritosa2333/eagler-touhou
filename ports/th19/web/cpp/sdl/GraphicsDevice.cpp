// th19 SDL3 graphics device implementation: decodes PNG/JPEG textures with
// stb_image, keeps CPU-side pixels, and hands them to the shared portable
// SDL3+GLES3 renderer via its Surface resolve callback.
#include "GraphicsDevice.hpp"
#include <cstring>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STBI_NO_STDIO
#include "../../portable/sdl/third_party/stb_image.h"

namespace th19::sdl {
namespace {
constexpr u32 SCREEN = 1, DEPTH = 2;
struct Texture { u32 width=0, height=0, revision=0; std::vector<u8> pixels; };
}

struct GraphicsDevice::Impl {
    std::map<u32, Texture> textures;
    u32 next = 3;
};

touhou::sdl::Surface GraphicsDevice::resolve(void* opaque, u32 id) {
    return static_cast<GraphicsDevice*>(opaque)->surface(id);
}

GraphicsDevice::GraphicsDevice() : impl_(new Impl) {
    backend.window_size(1280, 960);   // th19 renders at 1280x960
}
GraphicsDevice::~GraphicsDevice() = default;

touhou::sdl::Surface GraphicsDevice::surface(u32 id) {
    auto it = impl_->textures.find(id);
    if (it == impl_->textures.end()) return {};
    auto& t = it->second;
    return { id, t.width, t.height, id==DEPTH?PixelFormat::Depth24Stencil8:PixelFormat::Bgra8, t.width * 4,
             t.pixels.data(), u32(t.pixels.size()), t.revision };
}

bool GraphicsDevice::initialize() {
    for (u32 id : {SCREEN, DEPTH}) {
        auto& t = impl_->textures[id];
        t.width = 1280; t.height = 960;
        if (id == SCREEN) t.pixels.resize(1280 * 960 * 4);
    }
    if (!backend.initialize()) { error_ = backend.error(); return false; }
    backend.state.target = SCREEN; backend.state.depth = DEPTH;
    return true;
}

TextureAllocation GraphicsDevice::create_texture(const u8* bytes, u32 count) {
    int w = 0, h = 0, ch = 0;
    u8* raw = stbi_load_from_memory(bytes, int(count), &w, &h, &ch, 4);
    if (!raw) { error_ = stbi_failure_reason(); return {}; }
    Texture t; t.width = u32(w); t.height = u32(h); t.pixels.resize(size_t(w) * h * 4);
    for (size_t p = 0; p < t.pixels.size(); p += 4) {   // RGBA -> Bgra8
        t.pixels[p]   = raw[p + 2];
        t.pixels[p+1] = raw[p + 1];
        t.pixels[p+2] = raw[p];
        t.pixels[p+3] = raw[p + 3];
    }
    stbi_image_free(raw);
    const u32 id = impl_->next++;
    impl_->textures.emplace(id, std::move(t));
    backend.prepare(id);
    // prepare() has uploaded the image, and the renderer uploads again only
    // when the revision changes, which never happens to a decoded image (it
    // is never a render or capture target). Drop the CPU copy so the image
    // is held once, by the GPU. A swap, not shrink_to_fit: built with
    // -fno-exceptions, libstdc++ makes shrink_to_fit a no-op.
    std::vector<u8>().swap(impl_->textures.at(id).pixels);
    return { id, u32(w), u32(h) };
}

TextureAllocation GraphicsDevice::create_raw(u32 width, u32 height, const u8* rgba) {
    Texture t; t.width = width; t.height = height; t.pixels.resize(size_t(width) * height * 4);
    for (size_t p = 0; p < t.pixels.size(); p += 4) {
        t.pixels[p]   = rgba[p + 2];
        t.pixels[p+1] = rgba[p + 1];
        t.pixels[p+2] = rgba[p];
        t.pixels[p+3] = rgba[p + 3];
    }
    const u32 id = impl_->next++;
    impl_->textures.emplace(id, std::move(t));
    backend.prepare(id);
    return { id, width, height };
}

void GraphicsDevice::destroy(u32 handle) {
    if (handle < 3) return;
    backend.flush(); backend.release(handle); impl_->textures.erase(handle);
}

void GraphicsDevice::viewport(const Viewport& v) {
    backend.viewport({ v.x, v.y, v.width, v.height, v.near_depth, v.far_depth });
}

void GraphicsDevice::clear(u32 rgba) {
    backend.clear(3, rgba, 1, 0);
}

void GraphicsDevice::draw_sprite(u32 texture, const PipelineState& state,
                                 const SpriteVertex* quad, u32 vertices) {
    if (vertices < 3) return;
    backend.state.pipeline = state;
    backend.state.texture = texture;
    backend.state.layout = attributes(VertexLayout::ScreenColorUv);
    backend.state.layout.specular=offsetof(SpriteVertex,specular);
    if (vertices == 4) {
        // Expand a 4-vertex quad (TL,TR,BL,BR) into 2 triangles.
        // draw_batch takes triangle count; a quad is 2 triangles (6 verts).
        const SpriteVertex tri[6] = { quad[0], quad[1], quad[2],
                                      quad[2], quad[1], quad[3] };
        backend.draw(Topology::Triangles,2,tri,sizeof(SpriteVertex));
        return;
    }
    // The renderer owns a copy until flush and only merges adjacent draws
    // with identical texture, pipeline, target and transforms. Captures,
    // clears, line strips and present flush it, preserving compositing order.
    backend.draw(Topology::Triangles,vertices/3,quad,sizeof(SpriteVertex));
}

void GraphicsDevice::draw_line_strip(u32 texture,const PipelineState& state,const SpriteVertex* vertices,u32 count){
    if(count<2)return;
    backend.state.pipeline=state;backend.state.texture=texture;
    backend.state.layout=attributes(VertexLayout::ScreenColorUv);
    backend.state.layout.specular=offsetof(SpriteVertex,specular);
    backend.draw(Topology::LineStrip,count-1,vertices,sizeof(SpriteVertex));
}
void GraphicsDevice::capture_screen(u32 texture) {
    const i32 rect[]{0,0,1280,960}, point[]{0,0};
    backend.copy(SCREEN,rect,texture,point);
    // copy publishes GPU revision + 1; keep the resolver in sync so sampling
    // does not overwrite the captured frame with the empty CPU allocation.
    ++impl_->textures.at(texture).revision;
}

void GraphicsDevice::present() { backend.present(SCREEN); }
}
