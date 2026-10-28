// 语义绘制清单捕获设备（draw-contract.md v1 移植侧导出器）。
//
// 定位：ZunGraphics 的无头实现——AnmRenderer/关卡表现层看到的全部语义状态
// （矩阵/管线的混合与 TFACTOR/顶点布局/视口/纹理身份）在这里落影子，每当
// 一批精灵提交（triangles()/primitives()）即按契约序列化一个 op。输出格式
// 与 oracle/render_oracle.py 逐字段同构：JSONL、每帧一行、键序固定、浮点
// %.9g（binary32 往返无损）、viewport 的 X/Y/W/H 按原版 D3DVIEWPORT9 捕获
// 口径（DWORD 位型经 float 视图序列化）。
//
// 激活：仅 TH13_DRAW_CAPTURE 构建（build-harness.mjs）下由 AnmRenderer 的
// 提交钩子（draw_capture_sprite）喂数；web 构建编译本文件但全局指针恒空。
// 确定性：不打印任何指针/地址/时间；未登记纹理以 "port:" 前缀可见标记。
#pragma once
#include "AnmResource.hpp"
#include "ZunGraphics.hpp"

#include <cstdio>
#include <map>
#include <string>
#include <vector>

namespace th13 {

class DrawCapture final : public ZunGraphics {
public:
    // 纹理身份登记（槽号 = 原版 ANM mgr 26 槽表；draw-contract §0 最小装载集：
    // bullet=7、effect=8、st01 anim 名单 10/11）。file 内 [first, first+count)
    // 号纹理 → (slot, entry=文件内序号, name)。
    void register_texture(const AnmResource* file, u32 first, u32 count, i32 slot);

    bool open_output(const char* path);   // false 时 error() 有值
    bool close_output() noexcept;
    bool legacy_harness() const noexcept {
#ifdef __EMSCRIPTEN__
        return false;
#else
        return true;
#endif
    }
    void begin_frame(u32 frame);          // 帧界：viewport 保留（同 oracle 影子）
    bool end_frame();                     // 落一行 JSONL（含末尾 \n）
    // 帧尾 SetTexture(0,0)（0x45d2f0:49 的移植侧镜像）：设备侧纹理清空
    // （契约 §2.1：此后绘制 tex=null），但精灵侧缓存（原版 mgr+0xb78188）
    // 不失效——次帧同纹理精灵经缓存判定跳过 SetTexture，DrawPrimitive 落
    // 在 NULL 纹理上（oracle f1-8 tex=None 的来源，需逐帧复现）。
    void clear_device_texture() noexcept {
        bound_set_ = false;
        bound_ = Identity{};
    }
    // AnmRenderer 提交钩子语境：层号 + 九宫格锚点组（原版 startVertex =
    // ((flags>>23&3)*3+(flags>>21&3))*4，render-chain.md §2.1 第 10 步）。
    void sprite(i32 layer, u32 flags) noexcept;
    // UP 批提交时的层口径：原版 UP op 的 layer = 0x46f380 层 watch 的残值
    // （层循环走完 = 56，flush 发生在其后）——层循环后、flush 前调用。
    void set_stale_layer(i32 layer) noexcept { layer_ = layer; }

    u64 frames_out() const noexcept { return frames_out_; }
    u64 quads() const noexcept { return quads_; }
    u64 ups() const noexcept { return ups_; }
    u32 zero_op_frames() const noexcept { return zero_frames_; }
    const std::string& error() const noexcept { return error_; }
    // 已写输出字节偏移（web 影子分块导出：MEMFS 文件按偏移切片拉走）。
    long output_position() const noexcept { return out_ ? std::ftell(out_) : 0; }

    // ---- ZunGraphics（语义状态影子；目标/清屏类不产 op，静默成功）----
    touhou::graphics::PipelineState& pipeline() override { return pipeline_; }
    u32 texture(const AnmResource& file, u32 index) override;
    void bind_texture(u32 handle) override;
    void set_layout(touhou::graphics::VertexLayout layout) override;
    void set_matrix(touhou::graphics::MatrixKind kind, const Matrix4& matrix) override;
    void triangles(u32 count, const void* data, u32 stride) override;
    void primitives(touhou::graphics::Topology kind, u32 count, const void* data,
                    u32 stride) override;
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport& value) override {
        viewport_ = value;
        viewport_set_ = true;
    }
    void presentation_offset(Vec2) override {}
    void presentation_range(const AnmVm&, u32, u32, u32 = 0) override {}
    void presentation_camera(bool) override {}

private:
    struct Identity {
        i32 slot = -1, entry = -1;
        std::string name;
    };
    // 提交语境（钩子维护）
    i32 layer_ = -100;
    u32 anchor_ = 0;
    // 状态影子（管线初始 textureFactor=0 = 真机 D3D 设备初始 TFACTOR，
    // 蓝图 §8.8，open_output 落定；共享 GraphicsState 缺省 0xffffffff
    // 非 th13 语义——与 GraphicsDevice::initialize 同一分叉点）。
    touhou::graphics::PipelineState pipeline_{};
    touhou::graphics::VertexLayout layout_ = touhou::graphics::VertexLayout::ScreenColorUv;
    Matrix4 matrices_[4]{};  // World/View/Projection/Texture（MatrixKind 序）
    Identity bound_{};
    bool bound_set_ = false;  // 首次 bind_texture 前 = 原版 SetTexture(0,0) 态（tex null）
    GraphicsViewport viewport_{};
    bool viewport_set_ = false;
    // 纹理登记：句柄（1 起）→ 身份；(资源指针, 文件内序号) → 句柄。
    std::vector<Identity> identities_;
    std::map<std::pair<const AnmResource*, u32>, u32> handles_;
    // 输出
    std::FILE* out_ = nullptr;
    std::string line_;
    u32 frame_ = 0, seq_ = 0, zero_frames_ = 0;
    u64 frames_out_ = 0, quads_ = 0, ups_ = 0;
    std::string error_;

    void fail(const char* why) {
        if (error_.empty()) error_ = why;
    }
    // 栈纪律（V8 wasm 原生栈远小于宿主，蓝图 §8.7/§8.9）：emit 路径禁止被
    // -O2 跨函数内联成巨帧（emit_quad 内联 4×append_matrix×16 append_float
    // 曾在窗B 转场期粒子全量绘制时溢出 V8 栈并越过栈检查）。定点 noinline
    // 取代全局 -fno-inline-functions 旗语（emit_* 系互不内联，帧尺寸有界）。
    __attribute__((noinline)) void emit_quad(u32 prim, u32 count);
    __attribute__((noinline)) void emit_up(u32 prim, u32 count, u32 stride, const void* data);
    __attribute__((noinline)) void append_tex(const Identity& identity);
    __attribute__((noinline)) void append_blend();
    __attribute__((noinline)) void append_matrix(const Matrix4& matrix);
    __attribute__((noinline)) static void append_float(std::string& out, float value);
};

// AnmRenderer 提交钩子的目标（定义于 DrawCapture.cpp；默认 nullptr = 闲置）。
extern DrawCapture* draw_capture;

}  // namespace th13
