#include "GraphicsDevice.hpp"

#include "../../../portable/sdl/StartupBranding.hpp"

#include <SDL3/SDL.h>

namespace th13::sdl {

touhou::sdl::Surface GraphicsDevice::resolve(void* owner, u32 id) {
    auto& device = *static_cast<GraphicsDevice*>(owner);
    auto it = device.textures.find(id);
    if (it == device.textures.end()) return {};
    auto& t = it->second;
    auto& image = t.image;
    return {id,          image.width,     image.height,       image.format,
            image.pitch, image.pixels.data(), u32(image.pixels.size()), t.revision,
            t.renderScale};
}

void GraphicsDevice::draw_startup_branding(u32 tint) {
    if (!startup_credit) {
        auto pixels = touhou::sdl::startup_branding::load();
        if (pixels.empty()) return;
        startup_credit = next_handle++;
        auto& texture = textures[startup_credit];
        texture.identity = "shell:startup-credit";
        auto& image = texture.image;
        image.width = 1280;
        image.height = 960;
        image.pitch = 1280 * 4;
        image.format = touhou::graphics::PixelFormat::Bgra8;
        image.pixels = std::move(pixels);
    }
    if (!startup_credit || !(tint >> 24)) return;
    flush();
    const auto saved = backend.state;
    auto& s = backend.state;
    s.target = screen; s.depth = 0; s.texture = startup_credit;
    s.viewport = {0, 0, 640, 480, 0, 1};
    s.layout = touhou::graphics::attributes(touhou::graphics::VertexLayout::ScreenColorUv);
    s.pipeline = {};
    auto& p = s.pipeline;
    using namespace touhou::graphics;
    p.depthWrite = false; p.blend = true;
    p.sourceBlend = BlendFactor::SourceAlpha; p.destinationBlend = BlendFactor::InverseSourceAlpha;
    p.color.operation = p.alpha.operation = ColorOperation::Multiply;
    p.color.second = p.alpha.second = {ArgumentSource::Diffuse};
    p.addressU = p.addressV = Address::Clamp;
    struct Vertex { float x,y,z,w; u32 color; float u,v; };
    const Vertex quad[] = {{0,0,0,1,tint,0,0},{640,0,0,1,tint,1,0},
                           {0,480,0,1,tint,0,1},{640,480,0,1,tint,1,1}};
    primitives(Topology::Strip, 2, quad, sizeof(Vertex));
    flush(); backend.state = saved;
}

void GraphicsDevice::release_startup_branding() {
    if (startup_credit) {
        RenderSubmission c{RenderSubmission::Kind::Release, backend.state};
        c.resource = startup_credit;
        submit(c);
        textures.erase(startup_credit);
        startup_credit = 0;
    }
}

bool GraphicsDevice::initialize() {
    using namespace touhou::graphics;
    for (u32 id : {screen, depth}) {
        textures[id].renderScale = render_scale;
        textures[id].identity = id == screen ? "device:screen" : "device:depth";
        textures[id].gpu_content = true;
        auto& image = textures[id].image;
        image.width = 640;
        image.height = 480;
        image.pitch = 640 * (id == screen ? 4 : 2);
        image.format = id == screen ? PixelFormat::Bgra8 : PixelFormat::Depth16;
        if (id == screen) image.pixels.resize(image.pitch * image.height);
    }
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas");
    // COLOR-01: match the independently measured original Wine/Mesa UNORM8
    // source-term rounding for the original X8 opaque display only. Render
    // textures (whose alpha can be consumed) and other games retain GLES.
    // Initialize-only: Renderer bakes this choice into its shader programs.
    if (!backend.configure_unorm_blend_profile(
            touhou::sdl::UnormBlendProfile::SeparateRgbTerms, screen)) {
        error = "unorm blend profile must be configured before renderer initialization";
        return false;
    }
    if (!backend.initialize()) {
        error = backend.error();
        return false;
    }
    SDL_SetWindowTitle(SDL_GL_GetCurrentWindow(), "Touhou 13");
    SDL_SetWindowSize(SDL_GL_GetCurrentWindow(), 640 * render_scale, 480 * render_scale);
    backend.state.target = screen;
    backend.state.depth = depth;
    // th13 设备态分叉点（与 th15 共享件归一后，本作语义落在此处）：
    // textureFactor 初始 0 = 真机 D3D 设备初始 TFACTOR（oracle 标题窗 idle 期
    // 全 op tfactor=0 实证；共享缺省 0xffffffff 是 th15 始终显式配置的惰性缺
    // 省，th13 不经 configure_game 路径，须在设备初装即写真机初始态）。
    // TFACTOR 为跨帧持续设备态（蓝图 §8.8）；Factor 模式由 AnmRenderer
    // world_quad 族按 VM 色显式设置（真机 0x4692d0 同位）——configure_game 的
    // Factor 第二参数在本作不成立（曾致 web 全像素着黑，视觉全灭根因）。
    backend.pipeline().textureFactor = 0;
    return true;
}

bool GraphicsDevice::preload(AnmResource& file, bool low_color) {
    if (resources.find(&file) != resources.end()) return true;
    std::vector<u32> handles;
    handles.reserve(file.textures.size());
    for (u32 index = 0; index < file.textures.size(); index++) {
        const auto& source = file.textures[index];
        Texture texture;
        texture.renderScale = source.kind == AnmTexture::Kind::RenderTarget ? render_scale : 1;
        texture.identity = "anm:" + source.name + ":entry:" + std::to_string(index);
        texture.gpu_content = source.kind == AnmTexture::Kind::RenderTarget;
        if (!texture.image.load(source, low_color)) {
            error = "Unable to prepare ANM texture: " + source.name;
            for (const auto id : handles) {
                RenderSubmission c{RenderSubmission::Kind::Release, backend.state};
                c.resource = id;
                submit(c);
                textures.erase(id);
            }
            return false;
        }
        const auto id = next_handle++;
        textures.emplace(id, std::move(texture));
        handles.push_back(id);
        RenderSubmission c{RenderSubmission::Kind::Prepare, backend.state};
        c.resource = id;
        submit(c);
    }
    resources.emplace(&file, std::move(handles));
    return true;
}

bool GraphicsDevice::capture_region_to_texture(const AnmResource& file, u32 texture_index,
                                               const i32 source[4], const i32 destination[4]) {
    // 0x45bf10/0x45d7d0 的 D3DXLoadSurfaceFromSurface 等价：Present 前把
    // 本帧 screen 后备点采样进 ANM Blank 纹理。暂停激活时 0x4418b0 入队
    // {src=战场(32,16,384,448), dst=text.anm entry1 sprite76 (0,0,128,128)}；
    // script76-79 之后以加色混合淡入该纹理（0x43e3f0 黑幕通路）。
    flush();
    backend.read(screen);  // GL 后备 → screen TextureImage（BGRA8，逻辑像素）
    const auto screen_it = textures.find(screen);
    const u32 target_handle = texture(file, texture_index);
    if (screen_it == textures.end() || !target_handle) {
        if (error.empty()) error = "Animation capture source unavailable";
        return false;
    }
    TextureImage* target = pixels(target_handle);
    if (!target) {
        error = "Animation capture target unavailable";
        return false;
    }
    const auto& origin = screen_it->second.image;
    if (origin.format != touhou::graphics::PixelFormat::Bgra8 || !origin.width ||
        !origin.height || origin.pitch < origin.width * 4) {
        error = "Animation capture source format unavailable";
        return false;
    }
    if (target->format != touhou::graphics::PixelFormat::Rgb565 ||
        target->pitch != target->width * 2) {
        error = "Animation capture target is not an RGB565 blank texture";
        return false;
    }
    const i32 source_width = source[2], source_height = source[3];
    const i32 x0 = destination[0], y0 = destination[1];
    const i32 x1 = destination[2], y1 = destination[3];
    if (source_width <= 0 || source_height <= 0 || x1 <= x0 || y1 <= y0 || x0 < 0 ||
        y0 < 0 || u32(x1) > target->width || u32(y1) > target->height) {
        error = "Animation capture rectangle outside texture";
        return false;
    }
    const i32 target_width = x1 - x0, target_height = y1 - y0;
    for (i32 y = y0; y < y1; ++y) {
        // 目标像素中心反投影（最近邻）：sx = src.x + (dx+0.5)*src.w/dst.w。
        const i32 sy = source[1] + i32((i64(2) * (y - y0) + 1) * source_height /
                                       (2 * target_height));
        const i32 cy = sy < 0 ? 0 : sy >= i32(origin.height) ? i32(origin.height) - 1 : sy;
        const u8* row = origin.pixels.data() + size_t(cy) * origin.pitch;
        auto* out_row = target->pixels.data() + size_t(y) * target->pitch;
        for (i32 x = x0; x < x1; ++x) {
            const i32 sx = source[0] + i32((i64(2) * (x - x0) + 1) * source_width /
                                           (2 * target_width));
            const i32 cx = sx < 0 ? 0 : sx >= i32(origin.width) ? i32(origin.width) - 1 : sx;
            const u8* pixel = row + size_t(cx) * 4;  // BGRA8
            const u16 value = u16(((u32(pixel[2]) >> 3) << 11) |
                                  ((u32(pixel[1]) >> 2) << 5) | (u32(pixel[0]) >> 3));
            std::memcpy(out_row + size_t(x) * 2, &value, 2);
        }
    }
    changed(target_handle, {u32(x0), u32(y0), u32(x1), u32(y1)});
    return error.empty();
}

void GraphicsDevice::unload(const AnmResource& file) {
    presentation.reset();
    auto it = resources.find(&file);
    if (it == resources.end()) return;
    flush();
    for (u32 id : it->second) {
        RenderSubmission c{RenderSubmission::Kind::Release, backend.state};
        c.resource = id;
        submit(c);
        textures.erase(id);
        if (backend.state.texture == id) backend.state.texture = 0;
        if (backend.state.target == id) backend.state.target = screen;
        if (backend.state.depth == id) backend.state.depth = depth;
    }
    resources.erase(it);
}

u32 GraphicsDevice::texture(const AnmResource& file, u32 index) {
    const auto it = resources.find(&file);
    if (it == resources.end() || index >= it->second.size()) {
        error = "ANM texture was not preloaded";
        return 0;
    }
    return it->second[index];
}

bool GraphicsDevice::trace_resource(void* owner, u32 id, SubmissionTrace::Resource& r) {
    auto& device = *static_cast<GraphicsDevice*>(owner);
    auto it = device.textures.find(id);
    if (it == device.textures.end()) return false;
    auto& t = it->second;
    const auto s = resolve(owner, id);
    r.surface = {s.width,s.height,u32(s.format),s.pitch,s.renderScale,s.version,s.data,s.size};
    r.identity = t.identity;
    r.gpu_content = t.gpu_content;
    r.revision = t.content_revision;
    if (!r.gpu_content && s.data && s.size) r.content_hash = t.content_hash.get(r.surface, r.revision);
    return true;
}

void GraphicsDevice::submit(const RenderSubmission& c) {
    if (!c.dispatch(backend, submission_trace, trace_resource, this)) error = submission_trace.error();
    using K = RenderSubmission::Kind;
    // Record GPU writes after observing input resources. Never label stale CPU
    // backing pixels as the content of a rendered target.
    auto written = [&](u32 id) {
        auto it = textures.find(id);
        if (it != textures.end()) {
            ++it->second.content_revision;
            it->second.gpu_content = true;
            it->second.content_hash.invalidate();
        }
    };
    if (c.kind == K::Draw || (c.kind == K::Clear && (c.flags & 1))) written(c.state.target);
    if ((c.kind == K::Draw && c.state.pipeline.depthWrite) || (c.kind == K::Clear && (c.flags & 2))) written(c.state.depth);
    if (c.kind == K::Present && submission_trace.active()) ++capture_presents_;
}

void GraphicsDevice::triangles(u32 count, const void* data, u32 stride) {
    backend.state.stride = stride;
    RenderSubmission c{RenderSubmission::Kind::Draw, backend.state};
    c.count = count; c.vertices = data; c.stride = stride; c.direct = true;
    submit(c);
}

void GraphicsDevice::primitives(touhou::graphics::Topology topology, u32 count, const void* data, u32 stride) {
    backend.state.stride = stride;
    RenderSubmission c{RenderSubmission::Kind::Draw, backend.state};
    c.topology = topology; c.count = count; c.vertices = data; c.stride = stride;
    submit(c);
}

void GraphicsDevice::changed(u32 handle, std::array<u32,4> region) {
    auto it = textures.find(handle);
    if (it == textures.end()) { error = "unknown texture update"; return; }
    flush(); // Finish pending GPU reads before advancing the upload version.
    auto& t = it->second;
    RenderSubmission c{RenderSubmission::Kind::Update, backend.state};
    c.resource = handle; c.previous = t.revision; c.region = region;
    ++t.revision; ++t.content_revision; t.content_hash.invalidate();
    // A partial CPU upload cannot qualify previously GPU-written content.
    if (!region[2] && !region[3]) t.gpu_content = false;
    submit(c);
}

bool GraphicsDevice::upload_surface(TextureImage& image, u32& handle) {
    // 无既有等价面：preload() 只收 AnmResource，pixels() 只暴露已存在句柄。
    // 这里复用 changed() 的 flush+Update 提交与 revision 生命周期；GPU 侧由
    // Renderer 经 resolve() 按版本惰性上传（Renderer.cpp surface() 的
    // glTexImage2D/glTexSubImage2D 路径），不新增上传机制。
    if (!image.width || !image.height || image.pixels.empty()) {
        error = "uploaded surface is empty";
        return false;
    }
    if (handle == 0) {
        handle = next_handle++;
        auto& t = textures[handle];
        t.identity = "surface:upload";
        t.renderScale = 1;
        t.gpu_content = false;
    } else {
        auto it = textures.find(handle);
        // 只接受本接口先前返回的句柄；屏幕/深度/ANM/startup 句柄一律拒绝，
        // 防止把外部像素写进其它来源的纹理。
        if (it == textures.end() || it->second.identity != "surface:upload") {
            error = "uploaded surface handle is not surface-owned";
            return false;
        }
    }
    // 设备持有副本：resolve() 在绘制时把设备侧 pixels 交给后端，调用方后续
    // 改写自己的 TextureImage 不会移动已提交内容。
    textures[handle].image = image;
    changed(handle);
    return error.empty();
}

bool GraphicsDevice::capture_begin(const char* path) {
    if (submission_trace.active()) { error = "trace already open"; return false; }
    capture_presents_ = 0;
    if (!submission_trace.open(path)) {
        error = submission_trace.error();
        submission_trace.close();
        return false;
    }
    return true;
}

bool GraphicsDevice::capture_finish() noexcept {
    if (!submission_trace.active()) { error = "trace not open"; return false; }
    const bool ok = submission_trace.close(capture_presents_);
    if (!ok) error = submission_trace.error();
    return ok;
}

bool GraphicsDevice::select_target(const AnmResource* file, u32 index) {
    const u32 handle = file ? texture(*file, index) : screen;
    if (!handle) return false;
    const auto& image = textures.at(handle).image;
    RenderSubmission c{RenderSubmission::Kind::Target, backend.state};
    c.state.target = handle;
    c.state.viewport = {0, 0, image.width, image.height, 0, 1};
    submit(c);
    return true;
}

bool GraphicsDevice::clear_target(u32 color, const GraphicsViewport* rect) {
    if (rect) {
        const i32 box[] = {i32(rect->x), i32(rect->y), i32(rect->x + rect->width),
                           i32(rect->y + rect->height)};
        RenderSubmission c{RenderSubmission::Kind::Clear, backend.state};
        c.flags = 3; c.color = color; c.rect = box; submit(c);
    } else {
        clear(color);
    }
    return true;
}

bool GraphicsDevice::clear_depth(const GraphicsViewport* rect) {
    if (rect) {
        const i32 box[] = {i32(rect->x), i32(rect->y), i32(rect->x + rect->width),
                           i32(rect->y + rect->height)};
        RenderSubmission c{RenderSubmission::Kind::Clear, backend.state};
        c.flags = 2; c.rect = box; submit(c);
    } else {
        RenderSubmission c{RenderSubmission::Kind::Clear, backend.state};
        c.flags = 2; submit(c);
    }
    return true;
}

void GraphicsDevice::clear(u32 color, bool depth) {
    RenderSubmission c{RenderSubmission::Kind::Clear, backend.state};
    c.flags = depth ? 3 : 1; c.color = color;
    submit(c);
}

void GraphicsDevice::present() {
    RenderSubmission c{RenderSubmission::Kind::Present, backend.state};
    c.resource = screen;
    submit(c);
}

}  // namespace th13::sdl
