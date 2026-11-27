#include "GraphicsDevice.hpp"

#include <SDL3/SDL.h>
#include <algorithm>
#include <cstring>

namespace th16::sdl {

namespace {
constexpr u32 kLogicalWidth = 640;
constexpr u32 kLogicalHeight = 480;
}

touhou::sdl::Surface GraphicsDevice::resolve(void* owner, u32 id) {
    auto& device = *static_cast<GraphicsDevice*>(owner);
    auto it = device.textures.find(id);
    if (it == device.textures.end()) return {};
    auto& t = it->second;
    const u32 bytes_per_pixel = t.format == touhou::graphics::PixelFormat::Argb4444 ? 2 : 4;
    return {id, t.width, t.height, t.format, t.width * bytes_per_pixel, t.pixels.data(),
            u32(t.pixels.size()), t.revision, t.renderScale};
}

bool GraphicsDevice::initialize() {
    using namespace touhou::graphics;
    for (u32 id : {screen, depth}) {
        auto& t = textures[id];
        t.renderScale = kRenderScale;
        t.identity = id == screen ? "device:screen" : "device:depth";
        t.width = kLogicalWidth;
        t.height = kLogicalHeight;
        t.format = id == screen ? PixelFormat::Bgra8 : PixelFormat::Depth16;
        if (id == screen) t.pixels.resize(size_t(t.width) * t.height * 4);
    }
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas");
    if (!backend.initialize()) {
        error = backend.error();
        return false;
    }
    SDL_SetWindowTitle(SDL_GL_GetCurrentWindow(), "Touhou 16");
    SDL_SetWindowSize(SDL_GL_GetCurrentWindow(), kLogicalWidth * kRenderScale, kLogicalHeight * kRenderScale);
    backend.state.target = screen;
    backend.state.depth = depth;
    // 0x45ba80：设备创建后的渲染状态起点（与 harness RecordingDevice 一致）
    apply_original_device_defaults(backend.pipeline());
    return true;
}

bool GraphicsDevice::preload(const AnmResource& file) {
    if (resources.count(&file)) return true;
    std::vector<u32> handles;
    handles.reserve(file.entries.size());
    for (u32 index = 0; index < file.entries.size(); ++index) {
        const auto& entry = file.entries[index];
        const bool render_target = entry.name.size() >= 2 && entry.name[0] == '@' && entry.name[1] == 'R';
        // 0x46cd30：其余无 THTX 的 '@' entry 按头部宽高建空纹理（0x459240 文字目标）
        const bool blank = !render_target && !entry.texture.present() && !entry.name.empty() && entry.name[0] == '@';
        if (!render_target && !blank && !entry.texture.present()) { handles.push_back(0); continue; }
        Texture t;
        if (blank) {
            t.width = entry.width;
            t.height = entry.height;
            // Text painters already produce A4R4G4B4. Keep the packed pixels so
            // the renderer can upload dirty rectangles instead of the full atlas.
            t.format = touhou::graphics::PixelFormat::Argb4444;
            t.pixels.assign(size_t(t.width) * t.height * sizeof(u16), 0);
        } else if (render_target) {
            // 0x46d3b0：'@R' 头部宽高改写为后备缓冲尺寸（DAT_004d9d2c/30），D3DXCreateTexture(USAGE_RENDERTARGET)；
            // 这里建一块空表面，内容全部来自之后的帧缓冲绘制
            t.width = animations != nullptr ? animations->render_target_width : kLogicalWidth;
            t.height = animations != nullptr ? animations->render_target_height : kLogicalHeight;
            t.renderScale = kRenderScale;
            t.pixels.assign(size_t(t.width) * t.height * 4, 0);
        } else {
            // 原版 D3DXCreateTexture 按 entry 声明尺寸建纹理（0x46cb60），THTX 内容贴在左上；
            // GetLevelDesc 回读的是声明尺寸，ANM UV 亦按此计算——这里把解码像素衬到声明尺寸以对齐采样。
            const u32 dst_w = entry.width ? entry.width : entry.texture.width;
            const u32 dst_h = entry.height ? entry.height : entry.texture.height;
            const u32 src_w = entry.texture.width, src_h = entry.texture.height;
            t.width = dst_w;
            t.height = dst_h;
            t.pixels.assign(size_t(dst_w) * dst_h * 4, 0);
            const u32 copy_w = src_w < dst_w ? src_w : dst_w;
            const u32 copy_h = src_h < dst_h ? src_h : dst_h;
            for (u32 y = 0; y < copy_h; ++y) {
                std::memcpy(t.pixels.data() + size_t(y) * dst_w * 4,
                            entry.texture.bgra.data() + size_t(y) * src_w * 4,
                            size_t(copy_w) * 4);
            }
        }
        t.identity = "anm:" + file.name + ":entry:" + std::to_string(index);
        const u32 id = next_handle++;
        textures.emplace(id, std::move(t));
        handles.push_back(id);
        backend.prepare(id);
    }
    resources.emplace(&file, std::move(handles));
    return true;
}

void GraphicsDevice::unload(const AnmResource& file) {
    if (recent_resource_ == &file) {
        recent_resource_ = nullptr;
        recent_handles_ = nullptr;
    }
    auto it = resources.find(&file);
    if (it == resources.end()) return;
    backend.flush();
    for (u32 id : it->second) if (id) { backend.release(id); textures.erase(id); }
    resources.erase(it);
}

u32 GraphicsDevice::texture(const AnmResource& file, u32 entry) {
    // Consecutive draws from the same bank share its handle table until unload.
    if (recent_resource_ != &file) {
        const auto it = resources.find(&file);
        if (it == resources.end()) return 0;
        recent_resource_ = &file;
        recent_handles_ = &it->second;
    }
    return entry < recent_handles_->size() ? (*recent_handles_)[entry] : 0;
}

u32 GraphicsDevice::handle_of(TextureRef ref) {
    if (!ref.valid() || animations == nullptr) return 0;
    const AnmFile* file = animations->file(ref.slot);
    if (file == nullptr || file->resource == nullptr || ref.entry < 0) return 0;
    return texture(*file->resource, u32(ref.entry));
}

void GraphicsDevice::select_target(TextureRef target) {
    const u32 handle = target.valid() ? handle_of(target) : screen;
    if (handle == 0) { error = "render target texture was not preloaded"; return; }
    backend.flush();
    backend.state.target = handle;
    // D3D SetRenderTarget 的隐含行为：视口复位为目标全幅
    const Texture& t = textures.at(handle);
    backend.viewport({0, 0, t.width, t.height, 0, 1});
}

void GraphicsDevice::clear(u32 flags, u32 argb, const ScreenRect* rect) {
    if (rect != nullptr) {
        const i32 box[4] = {rect->left, rect->top, rect->right, rect->bottom};
        backend.clear(flags, argb, 1.0f, 0, box, 1);
    } else {
        backend.clear(flags, argb, 1.0f, 0);
    }
}

void GraphicsDevice::set_viewport(const Viewport& viewport) {
    backend.viewport({u32(viewport.x), u32(viewport.y), u32(viewport.width), u32(viewport.height), 0, 1});
}

void GraphicsDevice::draw(Topology topology, VertexLayout layout, u32 primitive_count, const void* vertices, u32 stride) {
    backend.state.layout = touhou::graphics::attributes(layout);
    backend.draw(topology, primitive_count, vertices, stride);
}

void GraphicsDevice::write_texture_argb4444(u32 handle, i32 left, i32 top, const u16* pixels, i32 width, i32 height) {
    auto it = textures.find(handle);
    if (it == textures.end() || pixels == nullptr || width <= 0 || height <= 0) return;
    Texture& t = it->second;
    using touhou::graphics::PixelFormat;
    if (t.pixels.empty()) return;
    const i32 x0 = std::max(left, 0);
    const i32 y0 = std::max(top, 0);
    const i32 x1 = std::min(left + width, i32(t.width));
    const i32 y1 = std::min(top + height, i32(t.height));
    if (x0 >= x1 || y0 >= y1) return;
    if (t.format == PixelFormat::Argb4444) {
        for (i32 y = y0; y < y1; ++y) {
            const u16* src = pixels + size_t(y - top) * width + (x0 - left);
            u8* dst = t.pixels.data() + (size_t(y) * t.width + x0) * sizeof(u16);
            std::memcpy(dst, src, size_t(x1 - x0) * sizeof(u16));
        }
        backend.changed(handle, t.revision, {u32(x0), u32(y0), u32(x1), u32(y1)});
        ++t.revision;
        return;
    }
    if (t.format != PixelFormat::Bgra8) return;
    for (i32 y = y0; y < y1; ++y) {
        const u16* src = pixels + size_t(y - top) * width + (x0 - left);
        u8* dst = t.pixels.data() + (size_t(y) * t.width + x0) * 4;
        for (i32 x = x0; x < x1; ++x, ++src, dst += 4) {
            const u16 p = *src;
            dst[0] = u8((p & 15u) * 17u);
            dst[1] = u8(((p >> 4) & 15u) * 17u);
            dst[2] = u8(((p >> 8) & 15u) * 17u);
            dst[3] = u8(((p >> 12) & 15u) * 17u);
        }
    }
    ++t.revision;
}

}  // namespace th16::sdl
