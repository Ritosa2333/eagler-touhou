// TH20 semantic graphics device on top of the shared touhou::sdl::Renderer.
// Textures and the backbuffer keep CPU pixel stores; the renderer uploads
// them on revision change and can blit/read back through FBOs. This is the
// TH_SDL3 implementation of th20_web/cpp/platform/Graphics.hpp — no D3D9
// objects exist in this build.
#include "../platform/Graphics.hpp"
#include "Renderer.hpp"
#include "StartupBranding.hpp"
#include <deque>
#include <array>
#include <map>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace touhou::sdl;
using th20::web::Texture;
using th20::web::TextureLock;

namespace {
struct Resource {
    Surface s{};
    std::vector<u8> bytes;
    bool scalable=false;
    std::array<std::int32_t,4> lock_rect{};
    bool locked=false;
};
struct Host {
    std::map<u32, Resource> resources;
    u32 next = 1, startup_credit = 0;
    u32 density = 1;
    std::deque<Texture> textures;       // deque: Texture* stays stable for the game
    std::unique_ptr<Renderer> gpu;
    u32 back = 0, depth = 0, target = 0; // target 0 means the backbuffer
    const Texture* bound = nullptr;
    Texture back_token{};
    std::vector<u8> corners;
    u32 corner_stride = 20;
    Resource& get(u32 id) {
        auto it = resources.find(id);
        if (it == resources.end()) { std::fprintf(stderr, "SDL TH20 invalid surface %u\n", id); std::abort(); }
        return it->second;
    }
    u32 image(u32 w, u32 h, PixelFormat f, bool renderTarget=false) {
        const u32 id = next++;
        auto& r = resources[id];
        r.s.handle = id; r.s.width = w; r.s.height = h; r.s.format = f;
        r.scalable=renderTarget;
        r.s.pixelScale = renderTarget ? density : 1;
        r.s.pitch = w * pixel_bytes(f);
        r.s.size = r.s.pitch * h;
        // Render targets live on the GPU. Allocate a CPU copy only if a
        // caller explicitly locks one for readback or editing.
        if (!renderTarget) {
            r.bytes.assign(r.s.size, 0);
            r.s.data = r.bytes.data();
        }
        return id;
    }
    void erase(u32 id) { gpu->release(id); resources.erase(id); }
    static Surface resolve(void* p, u32 id) {
        // Shared ImGui overlay pass asks for the canonical depth handle.
        if (id == 0xffffffff) return {id, static_cast<Host*>(p)->back_token.width, static_cast<Host*>(p)->back_token.height, PixelFormat::Depth16, 0, nullptr, 0, 0, static_cast<Host*>(p)->density};
        return static_cast<Host*>(p)->get(id).s;
    }
    u32 current_target() const { return target ? target : back; }
} host;
}

extern "C" void sdl_startup_branding_draw(unsigned tint){
 if(!host.startup_credit){auto pixels=startup_branding::load();if(pixels.empty())return;
  host.startup_credit=host.image(1280,960,PixelFormat::Bgra8);auto& resource=host.get(host.startup_credit);
  resource.bytes=std::move(pixels);resource.s.data=resource.bytes.data();}
 startup_branding::draw(*host.gpu,host.startup_credit,host.back,tint);
}
namespace th20::web {
PipelineState& GraphicsDevice::pipeline() { return host.gpu->pipeline(); }

bool GraphicsDevice::create(u32 width, u32 height, PixelFormat back_format, u32 /*present_interval*/) {
    if (!host.gpu) {
        host.gpu = std::make_unique<Renderer>(20, Host::resolve, &host);
        if (!host.gpu->initialize()) {
            // Report the real failure instead of letting the next backbuffer
            // candidate "succeed" against a renderer that was never created:
            // that produced a device with no GL context and a game that threw
            // on its first texture. No caller can recover, so fail here.
            std::fprintf(stderr, "SDL TH20 graphics device: WebGL initialization failed: %s\n", host.gpu->error());
            host.gpu.reset();
            return false;
        }
    }
    host.density=host.gpu->refresh_output_size();
    if (host.back) { host.erase(host.back); host.erase(host.depth); }
    host.back = host.image(width, height, back_format, true);
    host.depth = host.image(width, height, PixelFormat::Depth16, true);
    host.back_token = {host.back, width, height, back_format, true};
    host.target = 0;
    host.gpu->state.target = host.back;
    host.gpu->state.depth = host.depth;
    return true;
}

bool GraphicsDevice::reset_backbuffer(u32 width, u32 height, PixelFormat back_format, u32 interval) {
    if (!host.gpu) return create(width, height, back_format, interval);
    host.gpu->flush();
    host.erase(host.back);
    host.erase(host.depth);
    host.back = host.image(width, height, back_format, true);
    host.depth = host.image(width, height, PixelFormat::Depth16, true);
    host.back_token = {host.back, width, height, back_format, true};
    host.target = 0;
    host.gpu->state.target = host.back;
    host.gpu->state.depth = host.depth;
    return true;
}

bool GraphicsDevice::ready() const { return host.gpu != nullptr; }

void GraphicsDevice::clear(u32 flags, u32 color, float depth, u32 stencil, const std::int32_t* rects, u32 count) {
    host.gpu->state.target = host.current_target();
    host.gpu->state.depth = host.depth;
    host.gpu->clear(flags, color, depth, stencil, rects, count);
}

void GraphicsDevice::present() {
    host.gpu->flush();
    const u32 density=host.gpu->refresh_output_size();
    if(density!=host.density){
        const u32 previous=host.density;host.density=density;
        for(auto& entry:host.resources)if(entry.second.scalable){
            entry.second.s.pixelScale=density;
            host.gpu->resize_surface(entry.first,previous);
        }
        host.gpu->resize_surface(0xffffffff,previous);
    }
    host.gpu->present(host.back);
}

void GraphicsDevice::vertex_format(VertexLayout layout) { host.gpu->state.layout = attributes(layout); }

void GraphicsDevice::bind_texture(const Texture* texture) {
    host.bound = texture;
    host.gpu->state.texture = texture ? texture->id : 0;
}

void GraphicsDevice::transform(MatrixKind kind, const void* matrix16) { host.gpu->transform(kind, matrix16); }

void GraphicsDevice::set_viewport(u32 x, u32 y, u32 w, u32 h, float zmin, float zmax) {
    host.gpu->viewport({x, y, w, h, zmin, zmax});
}

void GraphicsDevice::draw(Topology primitive, u32 count, const void* vertices, u32 stride) {
    host.gpu->state.target = host.current_target();
    host.gpu->state.depth = host.depth;
    host.gpu->state.texture = host.bound ? host.bound->id : 0;
    host.gpu->draw(primitive, count, vertices, stride);
}

void GraphicsDevice::draw_triangle_batch(u32 count, const void* vertices, u32 stride) {
    host.gpu->state.target = host.current_target();
    host.gpu->state.depth = host.depth;
    host.gpu->state.texture = host.bound ? host.bound->id : 0;
    host.gpu->draw_batch(count, vertices, stride);
}

void GraphicsDevice::initialize_corner_buffer(const void* vertices, u32 bytes, u32 stride) {
    host.corners.assign(static_cast<const u8*>(vertices), static_cast<const u8*>(vertices) + bytes);
    host.corner_stride = stride;
}

void GraphicsDevice::draw_corner_strip(u32 first, u32 count) {
    draw(Topology::Strip, count, host.corners.data() + first * host.corner_stride, host.corner_stride);
}

Texture* GraphicsDevice::create_texture(u32 width, u32 height, PixelFormat format, bool render_target) {
    const u32 id = host.image(width, height, format, render_target);
    for (auto& slot : host.textures)
        if (!slot.id) {
            slot = {id, width, height, format, render_target};
            return &slot;
        }
    host.textures.push_back({id, width, height, format, render_target});
    return &host.textures.back();
}

void GraphicsDevice::release_texture(Texture*& texture) {
    if (!texture) return;
    if (host.bound == texture) bind_texture(nullptr);
    if (host.target == texture->id) set_render_target(nullptr);
    if (texture->id) host.erase(texture->id);
    texture->id = 0;
    texture = nullptr;
}

TextureLock GraphicsDevice::lock(Texture* texture, const std::int32_t* rect) {
    if (!texture || !texture->id) return {};
    auto& resource = host.get(texture->id);
    const bool missing_cpu_copy = resource.bytes.empty();
    if (missing_cpu_copy) {
        resource.bytes.assign(resource.s.size, 0);
        resource.s.data = resource.bytes.data();
    }
    host.gpu->read(texture->id, missing_cpu_copy); // restore discarded atlases or read modified targets
    auto& s = resource.s;
    // If the GPU already consumed the previous CPU revision, this lock starts
    // a new dirty region. Otherwise accumulate with earlier not-yet-uploaded
    // locks so no write is lost before the next draw.
    if(host.gpu->revision(texture->id)==s.version)s.dirtyW=s.dirtyH=0;
    const std::int32_t left=rect?rect[0]:0,top=rect?rect[1]:0;
    const std::int32_t right=rect?rect[2]:static_cast<std::int32_t>(s.width);
    const std::int32_t bottom=rect?rect[3]:static_cast<std::int32_t>(s.height);
    resource.lock_rect={left,top,right,bottom};resource.locked=true;
    if(right>left&&bottom>top){
        if(!s.dirtyW||!s.dirtyH){s.dirtyX=left;s.dirtyY=top;s.dirtyW=right-left;s.dirtyH=bottom-top;}
        else {const u32 r=std::max(s.dirtyX+s.dirtyW,static_cast<u32>(right)),b=std::max(s.dirtyY+s.dirtyH,static_cast<u32>(bottom));
            s.dirtyX=std::min(s.dirtyX,static_cast<u32>(left));s.dirtyY=std::min(s.dirtyY,static_cast<u32>(top));s.dirtyW=r-s.dirtyX;s.dirtyH=b-s.dirtyY;}
    }
    u8* data = s.data;
    if (rect) data += rect[1] * s.pitch + rect[0] * pixel_bytes(s.format);
    return {data, static_cast<std::int32_t>(s.pitch)};
}

void GraphicsDevice::unlock(Texture* texture) {
    if (texture && texture->id) {auto& r=host.get(texture->id);r.locked=false;r.s.version++;}
}

void GraphicsDevice::add_dirty_rect(Texture* texture) {
    // copy_surface() is a GPU blit and advances both renderer and logical
    // revisions already. D3D9's AddDirtyRect is only a managed-texture hint;
    // incrementing the CPU revision here would upload a stale CPU copy back
    // over the freshly blitted GPU pixels.
    (void)texture;
}

void GraphicsDevice::preload(Texture* texture) {
    if (texture && texture->id) host.gpu->prepare(texture->id);
}

void GraphicsDevice::discard_cpu_copy(Texture* texture) {
    if (!texture || !texture->id || texture->render_target) return;
    auto& resource = host.get(texture->id);
    if (resource.bytes.empty()) return;
    preload(texture);
    resource.s.data = nullptr;
    std::vector<u8>().swap(resource.bytes);
}

u32 GraphicsDevice::texture_width(const Texture* texture) const { return texture ? texture->width : 0; }
u32 GraphicsDevice::texture_height(const Texture* texture) const { return texture ? texture->height : 0; }

void GraphicsDevice::set_render_target(Texture* texture) {
    host.gpu->flush();
    host.target = texture ? texture->id : 0;
    host.gpu->state.target = host.current_target();
}

Texture* GraphicsDevice::back_buffer() { return host.back ? &host.back_token : nullptr; }

bool GraphicsDevice::copy_surface(Texture* destination, const std::int32_t* destination_rect,
                                  Texture* source, const std::int32_t* source_rect) {
    if (!destination || !source || !destination->id || !source->id) return false;
    auto& from = host.get(source->id).s;
    auto& to = host.get(destination->id).s;
    if (from.format != to.format) return false;
    std::int32_t box[]{0, 0, static_cast<std::int32_t>(from.width), static_cast<std::int32_t>(from.height)};
    if (!source_rect) source_rect = box;
    std::int32_t full_destination[]{source_rect[0], source_rect[1], source_rect[0] + source_rect[2] - source_rect[0], source_rect[1] + source_rect[3] - source_rect[1]};
    if (!destination_rect) destination_rect = full_destination;
    if (source_rect[2] <= source_rect[0] || source_rect[3] <= source_rect[1] ||
        destination_rect[2] <= destination_rect[0] || destination_rect[3] <= destination_rect[1]) return false;
    if (source_rect[0] < 0 || source_rect[1] < 0 || source_rect[2] > (std::int32_t)from.width ||
        source_rect[3] > (std::int32_t)from.height || destination_rect[0] < 0 || destination_rect[1] < 0 ||
        destination_rect[2] > (std::int32_t)to.width || destination_rect[3] > (std::int32_t)to.height) return false;
    host.gpu->blit(source->id, source_rect, destination->id, destination_rect);
    to.version++; // GPU revision advanced by the blit; match the CPU revision
    return true;
}

bool GraphicsDevice::supports_texture_format(PixelFormat format) const { return format == PixelFormat::Bgra8; }
u32 GraphicsDevice::max_texture_width() const { return 4096; }

GraphicsDevice& shared_device() {
    static GraphicsDevice device;
    return device;
}
}
