// th16 ANM 绘制——原版 AnmManager 绘制侧（0x468490 族）的行为镜像：把一个 ANM VM 变成设备调用。
//
// 职责与生命周期：GameSession 持有一份，所有绘制链回调（Supervisor 合成、关卡背景、各管理器、层回调）
// 都经由它向 RenderDevice 下发；无设备（纯逻辑 harness）时所有绘制为空操作。
//
// 锚点：
//   0x468490  单 VM 绘制分派：按 render mode（flags_lo bit25..29）选路径；入口先关 Z 写（0x43c570）
//   0x465c40 / 0x4660b0  mode 0..3 四角（见 AnmGeometry）    0x465280  像素对齐、UV、顶点色、视口剔除、入批
//   0x467410  mode 8：世界矩阵（精灵尺寸/256 × scale × 旋转 × 平移）+ 纹理矩阵，以 ±128 单位四边形走固定管线
//   0x464f10  逐 VM 设备状态缓存：混合模式 / 采样过滤 / 寻址模式
//   0x469640 / 0x469a00 / 0x469bd0  mode 0x11 / 0x12 / 0x13：无贴图的正多边形（扇面 / 轮廓 / 环带），
//             边数 = $I0，半径 = 精灵宽 × scale，顶点格式 XYZRHW|DIFFUSE
//   0x468c70 / 0x469570 / 0x469330 / 0x468fc0  mode 0x10 / 0x14 / 0x15 / 0x16 / 0x1a / 0x1b：无贴图矩形（实心 /
//             描边 / 线段 / 线框），宽高 = ins_606 族的精灵尺寸 × scale，锚点取 ins_421
//   0x465b50  入批（每四边形 6 顶点 XYZRHW|DIFFUSE|TEX1）  0x465a80  出批 DrawPrimitiveUP
//   0x4681f0  宿主顶点条带：出批、绑精灵贴图、FVF XYZRHW|DIFFUSE|TEX1 + ARG2 = DIFFUSE、0x464f10 状态、
//             MODULATE，再按 TRIANGLESTRIP 直接 DrawPrimitiveUP（曲线激光 0x438750）
//   0x46e750  遍历一层绘制链
//   0x43c710  切相机：下发 view / projection，抖动偏移进绘制偏移
//   0x43c4b0 / 0x43c4f0 / 0x43c530 / 0x43c570  Supervisor 上的雾 / Z 写缓存（0x4c10d0 +0xa0c / +0xa10）
//
// 与原版一致的两层缓存：
//   设备状态缓存（AnmManager +0x184fbac..+0x184fbc4）——只在值变化时出批并下发；0x43d140 每帧开头复位
//   Supervisor 雾 / Z 写缓存——关卡背景与层回调按需切换
#pragma once
#include "AnmGeometry.hpp"
#include "AnmManager.hpp"
#include "Camera.hpp"
#include "RenderDevice.hpp"
#include "Types.hpp"

#include <string>
#include <vector>

namespace th16 {

// flags_lo bit5..8（ins_302）：混合模式 → 0x464f10 的 D3D 混合参数
enum class AnmBlendMode : u32 {
    Alpha = 0,            // SRCALPHA / INVSRCALPHA
    Additive = 1,         // SRCALPHA / ONE
    Subtractive = 2,      // ONE − SRCALPHA·src（REVSUBTRACT）
    Replace = 3,          // 关 alpha test，ONE / ZERO
    Screen = 4,           // INVDESTCOLOR / INVSRCCOLOR
    Multiply = 5,         // DESTCOLOR / ZERO
    InverseMultiply = 6,  // INVSRCCOLOR / INVSRCALPHA
    DestinationAlpha = 7, // DESTALPHA / INVDESTALPHA
    Darken = 8,           // MIN
    Lighten = 9,          // MAX
};

// ANM render mode（flags_lo bit25..29，ins_303 / ins_306 / ins_600 族）
enum class AnmRenderMode : u32 {
    PixelQuad = 0,         // 屏幕空间、像素对齐
    RotatedQuad = 1,       // 屏幕空间、绕 Z 旋转
    Quad = 2,              // 屏幕空间、不对齐
    RotatedQuad2 = 3,      // 同 1（历史别名）
    ProjectedQuad = 4,     // 0x466390：世界坐标投影到屏幕、按投影像素长缩放的旋转四边形（3D 粒子）
    Mode5 = 5, Mode6 = 6, Mode7 = 7,
    World = 8,             // 世界空间固定管线四边形（关卡背景、3D 粒子）
    VertexBuffer = 9,      // 0x4681f0：宿主自备顶点缓冲
    Mode11 = 0xb,
    VertexBuffer12 = 0xc, VertexBuffer13 = 0xd, VertexBuffer14 = 0xe,
    WorldFogToggle = 0xf,  // 开雾画 mode 8 再关雾
    Rect = 0x10,           // 0x468c70：单色实心矩形（ins_605 之后的旧式尺寸）
    PolygonFan = 0x11,     // 0x469640：中心色 color、边缘色 color2 的实心正多边形
    PolygonOutline = 0x12, // 0x469a00：线段带轮廓
    PolygonRing = 0x13,    // 0x469bd0：内外半径差 = 精灵高 × scale 的环带
    GradientRect = 0x14,   // 0x468c70：左 color → 右 color2 的实心矩形（ins_606）
    OutlinedRect = 0x15,   // 0x469570：先画大一像素、半透明的底，再画单色矩形（ins_607）
    OutlinedGradientRect = 0x16,  // 0x469570 的渐变版（ins_608）
    Strip = 0x18, Strip2 = 0x19,  // 0x4681f0 族：宿主顶点缓冲条带（ins_609 / 610）
    Line = 0x1a,           // 0x469330：沿旋转方向、长 = 精灵宽 的线段，两端 color / color2
    RectOutline = 0x1b,    // 0x468fc0：矩形线框（线段带 5 点）
};

// 绘制钩子表 DAT_00491b2c 中由其它子系统实现的项：AnmRenderer::draw_vm 在可见性检查之前按 vm.draw_callback 回调
struct AnmDrawHooks {
    virtual ~AnmDrawHooks() = default;
    // 钩子 5（0x42b5f0 → 0x42b480）：对话文字 VM 在绘制前按文字框子 VM 的当前屏幕位置重定 entity_offset
    virtual void dialogue_text_follow(AnmVm& vm) = 0;
};

class AnmRenderer {
public:
    using PipelineState = touhou::graphics::PipelineState;

    AnmRenderer(AnmManager& manager, const AnmGeometry& geometry) : manager_(manager), geometry_(geometry) {}

    RenderDevice* device = nullptr;   // 为空 = 无绘制（逻辑 harness）
    // 诊断（harness TH16_VM_DUMP=<帧>，与 game-oracle.py --vm-dump 同格式）：trace_frame == trace_current 的那一帧，
    // 每个进入 draw_vm 的 VM 向 stderr 打一行身份与绘制状态
    int trace_frame = -1;
    int trace_current = -1;
    AnmDrawHooks* draw_hooks = nullptr;   // Gui（钩子 5）
    std::string error;

    // ---- 相机（DAT_004c17b8 / DAT_004c17bc）----
    // 0x43c710 + SetViewport：出批、下发 view / projection、绘制偏移 = 相机抖动偏移、视口 = 相机视口
    void use_camera(const Camera& camera);
    const Camera* camera() const { return camera_; }
    const AnmGeometry& geometry() const { return geometry_; }

    // ---- Supervisor 设备状态缓存 ----
    void set_fog(bool enabled);          // 0x43c4b0（开）/ 0x43c4f0（关）
    void set_depth_write(bool enabled);  // 0x43c530（开）/ 0x43c570（关）
    // ZFUNC（0x40a410 / 0x46e030 等直接下发，无缓存）
    void set_depth_compare(touhou::graphics::Compare compare);
    // 雾参数（0x409f90：FOGCOLOR / FOGSTART / FOGEND，每项前都先出批）
    void set_fog_parameters(u32 color, float near_, float far_);

    // ---- 色调（AnmManager +0x1c7fd88 / +0x1c7fd8c：关卡闪白 0x409f90 置位，绘制完背景后清回 0x80808080）----
    void set_tint(bool enabled, u32 color) { tint_enabled_ = enabled; tint_ = color; }

    // ---- 绘制偏移（AnmManager +0xd0 / +0xd4）----
    void set_draw_offset(float x, float y) { draw_offset_x_ = x; draw_offset_y_ = y; }

    // ---- 帧边界（0x45a8a0 主循环）----
    // BeginScene 后：丢弃残批（0x465a30）、雾缓存置未知后关雾（DAT_004c1adc = 0xff → 0x43c4f0）
    void begin_frame();
    // 绘制链之后：出批、SetTexture(0, NULL)
    void end_frame();

    // ---- 批与缓存 ----
    void flush();                 // 0x465a80
    void reset_state_cache();     // 0x43d140 中段：纹理 / 混合 / 过滤 / 寻址 / 顶点格式缓存全部置"未知"，色调关、偏移清零
    void reset_state_cache_after_device_defaults();  // 0x45ba80 尾
    // +0x184fbb4 = 10：外部直接改过 SRCBLEND / DESTBLEND 后，让下一个 VM 重新下发自己的混合模式（0x4073a0 过场清 alpha 后）
    void invalidate_blend_cache() { blend_cache_ = 0x0a; }
    bool batch_pending() const { return !batch_.empty(); }

    // ---- 绘制 ----
    // 0x468490：返回 0 已处理，-1 跳过（不可见 / 删除中 / 模式未实现）
    int draw_vm(AnmVm& vm);
    // 0x465c40 + 0x465280(…, 1)：不经 0x468490 分派、不看可见位，直接按 mode 0 像素对齐入批
    // （AsciiManager 字形 / Item 得分弹出的字形 VM 走这条路）
    int draw_screen_sprite(AnmVm& vm);
    // 0x4681f0：宿主自备的屏幕空间顶点条带（曲线激光 0x438750 等）。只取 vm 的可见位 / 顶点 alpha、贴图与
    // 混合 / 过滤 / 寻址状态，顶点由调用方算好（XYZRHW|DIFFUSE|TEX1）。返回 0 已绘制，-1 跳过
    int draw_host_strip(AnmVm& vm, const ScreenColorUvVertex* vertices, u32 vertex_count);
    // 0x46e750：遍历一层绘制链
    void draw_layer(int layer);
    // 0x45c690：ScreenEffect 淡入淡出的单色四边形。出批后以 SELECTARG1 / DIFFUSE、
    // DESTBLEND = INVSRCALPHA 画 XYZRHW|DIFFUSE 条带，再把贴图键 / 混合 / 顶点格式 / 纹理矩阵缓存置"未知"，
    // 并把组合器直接拨回 MODULATE / TEXTURE（不经缓存）；贴图与 DESTBLEND 不恢复，与原版一致
    void draw_screen_fill(float width, float height, u32 color);
    void draw_screen_fill(float x0, float y0, float x1, float y1, u32 color);
    // 未实现的 render mode 计数（按模式号），对拍时据此发现缺口
    u32 unimplemented_modes[32] = {};
    // 未实现的绘制钩子（DAT_00491b2c 表下标 1..6）计数
    u32 unimplemented_draw_callbacks[8] = {};

private:
    AnmManager& manager_;
    const AnmGeometry& geometry_;
    const Camera* camera_ = nullptr;

    // Supervisor +0xa0c / +0xa10：-1 未知、0 关、1 开
    i32 fog_cache_ = -1;
    i32 depth_write_cache_ = -1;

    // AnmManager 设备状态缓存
    enum class VertexFormat : u8 { Unknown = 0xff, ScreenQuads = 1, WorldQuad = 2, HostBuffer = 3 };
    i32 texture_key_cache_ = -1;        // +0x184fbb0：精灵表 key（slot<<8 | entry）
    u8 blend_cache_ = 0x0a;             // +0x184fbb4
    u8 filter_cache_ = 0xff;            // +0x184fbba
    u8 combiner_args_set_ = 0xff;       // +0x184fbbb：1 = ALPHAOP/COLOROP 已设为 MODULATE
    u8 address_u_cache_ = 0xff;         // +0x184fbbc
    u8 address_v_cache_ = 0xff;         // +0x184fbbd
    VertexFormat vertex_format_cache_ = VertexFormat::Unknown;  // +0x184fbb6
    const AnmRuntimeSprite* texture_matrix_sprite_ = nullptr;   // +0x184fbc0
    u32 texture_factor_cache_ = 0;      // +0x184fbac
    bool tint_enabled_ = false;         // +0x1c7fd8c
    u32 tint_ = 0x80808080;             // +0x1c7fd88
    float draw_offset_x_ = 0, draw_offset_y_ = 0;   // +0xd0 / +0xd4

    std::vector<ScreenColorUvVertex> batch_;   // +0x184fc1c 起的四边形批（每四边形 6 顶点）

    const AnmRuntimeSprite* sprite_of(const AnmVm& vm) const;
    // 0x465280：mode 0..3 四角 → 像素对齐 / UV / 顶点色 / 视口剔除 / 入批
    int submit_screen_quad(AnmVm& vm, const AnmRuntimeSprite& sprite, float corners[4][2], bool pixel_align);
    // 0x465280：四角已含位置；加绘制偏移、可选像素对齐、UV、视口剔除、顶点色后入批
    int submit_screen_corners(AnmVm& vm, const AnmRuntimeSprite& sprite, float corners[4][2], float z, bool pixel_align);
    // 0x467410
    int draw_world_quad(AnmVm& vm, const AnmRuntimeSprite& sprite);
    // 0x468490 mode 0x11..0x13 分支 + 0x469640 / 0x469a00 / 0x469bd0
    int draw_polygon(AnmVm& vm, AnmRenderMode mode);
    // 0x468490 mode 0x10 / 0x14..0x16 / 0x1a / 0x1b 分支 + 0x468c70 / 0x469570 / 0x469330 / 0x468fc0
    int draw_rectangle(AnmVm& vm, AnmRenderMode mode);
    // 0x468c70：矩形四角（左上 c1、右上 c2、左下 c1、右下 c2 的三角条带）
    void submit_rectangle(float x, float y, float width, float height, float rotation, u32 c1, u32 c2,
                          u32 anchor_x, u32 anchor_y);
    // 多边形顶点提交前的共同状态：ALPHAOP / COLOROP = SELECTARG2、ARG2 = DIFFUSE、FVF XYZRHW|DIFFUSE
    void prepare_untextured_draw();
    // 0x464f10
    void apply_vm_state(const AnmVm& vm);
    // 0x406860 + 0x469890（绘制钩子 3）：爆散粒子折线——VM 世界位置 + 各光点偏移、顶点色，XYZRHW|DIFFUSE 线带
    void draw_burst_lines(AnmVm& vm);
    // 0x465280 / 0x467410 共用：纹理 key 变化时出批并绑定
    void bind_sprite_texture(const AnmRuntimeSprite& sprite);
    // 0x465a80 内：ALPHAOP / COLOROP = MODULATE（只设一次）
    void ensure_modulate_combiner();
    // 顶点色 × 色调（每通道 >>7 并钳 255）
    u32 apply_tint(u32 color) const;
    bool in_camera_viewport(const float corners[4][2]) const;
};

}  // namespace th16
