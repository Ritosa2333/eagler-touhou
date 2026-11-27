// th16 设备层：RenderDevice 的真机实现——ANM 纹理句柄 → 共享语义 GLES3 后端（portable/sdl/Renderer，与 th13/th15 同源不分叉）。
//
// 原版 0x46cdd0 族把 THTX 交给 D3D9 纹理；这里 CPU 解码为 BGRA8 后由 Renderer::surface() 惰性上传。
// text.anm 的 '@R' 渲染目标 entry（0x46d3b0 改写为后备缓冲尺寸）对应一块空 BGRA8 表面，由帧缓冲纹理承接；
// 所有目标共用同一块深度缓冲（D3D 的自动深度 / 模板面不随 SetRenderTarget 切换）。
//
// 纹理身份：游戏侧用 TextureRef{ANM 槽, entry}（与 harness RecordingDevice 同口径），这里经 AnmManager 槽表
// 解析到 AnmResource，再查本设备为该资源建的句柄表。
#pragma once
#include "../game/AnmManager.hpp"
#include "../game/AnmResource.hpp"
#include "../game/RenderDevice.hpp"
#include "../game/Types.hpp"
#include "../../../portable/sdl/Renderer.hpp"

#include <map>
#include <string>
#include <vector>

namespace th16::sdl {

class GraphicsDevice final : public RenderDevice {
    struct Texture {
        u32 width = 0, height = 0, revision = 0, renderScale = 1;
        touhou::graphics::PixelFormat format = touhou::graphics::PixelFormat::Bgra8;
        std::vector<u8> pixels;
        std::string identity;
    };
    std::map<u32, Texture> textures;
    std::map<const AnmResource*, std::vector<u32>> resources;
    const AnmResource* recent_resource_ = nullptr;
    const std::vector<u32>* recent_handles_ = nullptr;
    u32 next_handle = 3;
    static touhou::sdl::Surface resolve(void*, u32);
    u32 handle_of(TextureRef ref);

public:
    static constexpr u32 screen = 1, depth = 2;
    static constexpr u32 kRenderScale = 2;
    touhou::sdl::Renderer backend{10, resolve, this};
    std::string error;
    // TextureRef 槽号 → AnmFile → AnmResource 的解析来源（GameSession::animations）
    const AnmManager* animations = nullptr;

    bool initialize();
    // 为 AnmResource 的每个 entry 建纹理句柄；无贴图且非 '@R' 的 entry 句柄为 0
    bool preload(const AnmResource&);
    void unload(const AnmResource&);
    u32 texture(const AnmResource&, u32 entry);

    // ---- RenderDevice ----
    touhou::graphics::PipelineState& pipeline() override { return backend.pipeline(); }
    void select_target(TextureRef target) override;
    void clear(u32 flags, u32 argb, const ScreenRect* rect) override;
    void set_viewport(const Viewport& viewport) override;
    void set_matrix(MatrixKind kind, const float matrix[16]) override { backend.transform(kind, matrix); }
    void bind_texture(TextureRef texture) override { backend.state.texture = handle_of(texture); }
    void draw(Topology topology, VertexLayout layout, u32 primitive_count, const void* vertices, u32 stride) override;

    // 0x45a8a0 EndScene / Present：把后备缓冲表面送上窗口
    void present() { backend.present(screen); }
    void flush() { backend.flush(); }

    // 0x459240：写入 A4R4G4B4 文字矩形并抬 revision。
    void write_texture_argb4444(u32 handle, i32 left, i32 top, const u16* pixels, i32 width, i32 height);
};

}  // namespace th16::sdl
