// th13 production submission boundary; portable/sdl stays shared with th15.
#pragma once
#include "../game/ZunGraphics.hpp"
#include "../game/TextureImage.hpp"
#include "../game/SubmissionTrace.hpp"
#include "../../../portable/sdl/Renderer.hpp"
#include <map>

namespace th13::sdl {
using RenderSubmission = BasicRenderSubmission<touhou::sdl::State>;
class GraphicsDevice final : public ZunGraphics {
    struct Texture {
        TextureImage image;
        u32 revision = 0, renderScale = 1;
        uint64_t content_revision = 0;
        bool gpu_content = false;
        std::string identity;
        SubmissionTrace::ContentHash content_hash;
    };
    std::map<u32, Texture> textures;
    std::map<const AnmResource*, std::vector<u32>> resources;
    u32 next_handle = 3, startup_credit = 0;
    static touhou::sdl::Surface resolve(void*, u32);
    static bool trace_resource(void*, u32, SubmissionTrace::Resource&);
    void submit(const RenderSubmission&);
    uint64_t capture_presents_ = 0;
public:
    touhou::sdl::Renderer backend{10, resolve, this};
    static constexpr u32 screen = 1, depth = 2;
    std::string error;
    u32 render_scale = 1;
    SubmissionTrace submission_trace;
    bool capture_begin(const char* path);
    bool capture_finish() noexcept;
    // Legacy layer metadata is not a device state and is not needed by v2.
    void capture_stale_layer(i32) noexcept {}
    bool capture_active() const noexcept { return submission_trace.active(); }
    struct PresentationStub {
        bool enabled = false;
        void begin() {} void finish() {} void reset() {}
    } presentation;
    void presentation_offset(Vec2) override {}
    void presentation_range(const AnmVm&, u32, u32, u32 = 0) override {}
    void presentation_camera(bool) override {}
    bool initialize();
    void draw_startup_branding(u32 tint);
    void release_startup_branding();
    bool preload(AnmResource&, bool low_color = false);
    // 将外部解码的 BGRA TextureImage 上传为设备纹理（ManualViewer 等）。
    // 契约：handle 入参 0 = 设备新分配；非 0 必须是本接口先前返回的句柄
    //（identity "surface:upload"），拒绝复用 ANM/屏幕/深度等其它来源句柄。
    // 设备持有像素副本（resolve() 在绘制时惰性读取设备侧存储），调用方可在
    // 上传后自由改写自己的 TextureImage；重复调用走 changed() 同款
    // flush+Update 提交与 revision 生命周期，不引入新上传框架。
    bool upload_surface(TextureImage& image, u32& handle);
    // 0x4418b0 → 0x45bf10/0x45d7d0：把本帧 screen 后备的 source 矩形（逻辑
    // 像素 x,y,w,h）点采样写入目标 ANM 纹理的 destination 矩形（纹理像素
    // x0,y0,x1,y1）。暂停黑幕：战场 (32,16,384,448) → text.anm entry1 Blank
    // 的 sprite76 (0,0,128,128)；目标 format 3 = RGB565。
    // 契约：source 为逻辑像素矩形（上下对称，GL 行序翻转不影响矩形本身）；
    // 目标必须是已 preload 的 RGB565 Blank 纹理，否则如实失败。
    bool capture_region_to_texture(const AnmResource& file, u32 texture_index,
                                   const i32 source[4], const i32 destination[4]);
    void unload(const AnmResource&);
    u32 texture(const AnmResource&, u32) override;
    TextureImage* pixels(u32 handle) {
        auto it = textures.find(handle);
        if (it == textures.end()) return nullptr;
        // Obtaining writable pixels invalidates cached CPU content even before
        // changed() advances the upload version. Callers must call changed().
        it->second.content_hash.invalidate();
        return &it->second.image;
    }
    void changed(u32 handle, std::array<u32, 4> region = {});
    void bind_texture(u32 handle) override { backend.state.texture = handle; }
    touhou::graphics::PipelineState& pipeline() override { return backend.state.pipeline; }
    void set_layout(touhou::graphics::VertexLayout layout) override {
        backend.state.layout = touhou::graphics::attributes(layout);
    }
    void set_matrix(touhou::graphics::MatrixKind kind, const Matrix4& matrix) override {
        std::memcpy(backend.state.matrix[u32(kind)].data(), matrix.m, 64);
    }
    void triangles(u32 count, const void* data, u32 stride) override;
    void primitives(touhou::graphics::Topology, u32 count, const void* data, u32 stride) override;
    bool select_target(const AnmResource*, u32) override;
    bool clear_target(u32, const GraphicsViewport*) override;
    bool clear_depth(const GraphicsViewport*) override;
    void set_viewport(const GraphicsViewport& v) override { viewport(v); }
    void viewport(const GraphicsViewport& v) {
        backend.state.viewport = {v.x, v.y, v.width, v.height, v.near_depth, v.far_depth};
    }
    void clear(u32 color, bool depth = true);
    void present();
    void flush() { submit({RenderSubmission::Kind::Flush, backend.state}); }
};
}
