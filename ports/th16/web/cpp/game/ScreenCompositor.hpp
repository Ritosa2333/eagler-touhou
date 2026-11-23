// th16 画面合成——Supervisor 绘制侧：场地经两张 '@R' 渲染目标乒乓合成后贴到屏幕，以及四台全局相机。
//
// 原版把场地画进 text.anm 的两张 640×480 渲染目标纹理（entry 2 = RT1、entry 3 = RT2，表面 DAT_004c127c /
// DAT_004c1280），每次"复制"都是用一个 text.anm 脚本 VM（DAT_004c128c..98）把上一张 RT 当纹理画到另一张上，
// 最后把 RT2 的场地区域贴到屏幕场地位置 (32,16)-(416,464)；暂停 / 续关的模糊、关卡切换的畸变等效果都由
// 这几段脚本承担。绘制链按优先级穿插在各管理器之间：
//
//   1     0x43d140  清后台缓冲 → 切 RT1 → 按渲染目标相机视口清场地区 → 设备状态缓存复位 → UI 相机
//   0xe   0x43d400  切 RT2、清场地区深度
//   0xf   0x43d2f0  把 RT1 以 VM0 画进 RT2（渲染目标相机），再画图层 0x22
//   0x19  0x43d5a0  切 RT1、清场地区深度
//   0x1a  0x43d4a0  把 RT2 以 VM1 画进 RT1
//   0x2b  0x43d6f0  切 RT2、清宽场地相机视口 (128,16,384,448) 深度
//   0x2c  0x43d640  把 RT1 以 VM2 画进 RT2
//   0x38  0x43d820  切回后台缓冲并清黑
//   0x39  0x43d7c0  把 RT2 以 VM3 画到屏幕场地位置
//   0x55  0x43d870  清画面抖动偏移（DAT_004c1788/8c、DAT_004c1530/34）
//
// 其它锚点：
//   0x43b520  建四个合成 VM（堆上 0x5fc，0x4093f0 构造）  0x43d970  text.anm 装载后取 RT 表面、按窗口宽度装脚本
//   0x42d7b0  GameManager 每帧推进四个合成 VM            0x42cb60  装载开头 VM0 中断 2；装载结束四个 VM 待中断 2
//   0x43cb10  四台相机初始化（见 Camera.hpp）
#pragma once
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "Camera.hpp"
#include "Chain.hpp"
#include "RenderDevice.hpp"
#include "ScreenEffect.hpp"
#include "Types.hpp"

namespace th16 {

class ScreenCompositor {
public:
    static constexpr int kTextSlot = 0;                // text.anm（DAT_004c17f8）
    static constexpr i32 kRenderTarget1Entry = 2;      // text.anm '@R' entry
    static constexpr i32 kRenderTarget2Entry = 3;
    static constexpr u32 kCompositeVmCount = 4;

    // ---- 全局相机（Supervisor +0x238 / +0x364 / +0x490 / +0x5bc）----
    Camera field_camera;            // 0x4c1308：(32,16,384,448)，kind 0
    Camera wide_field_camera;       // 0x4c1434：(128,16,384,448)，kind 1
    Camera ui_camera;               // 0x4c1560：(0,0,640,480)，kind 2
    // 0x4c168c：((w-408)/2,(h-472)/2,408,472)，kind 3。StageManager 每帧把关卡相机整体抄入同一全局（fov 随 STD 脚本），
    // 合成步骤 0x43d2f0 / 0x43d4a0 再 0x43c780 复位后用它——与 GameSession::stage_camera 必须是同一对象，
    // 由 GameSession 构造时绑定；init_cameras 只写初值
    Camera* render_target_camera = nullptr;

    u32 background_color = 0xff000000;   // DAT_004c1b0c：场地清屏色（STD ins 13 写；离开游戏复位）
    bool render_targets_ready = false;   // DAT_004c127c != 0：text.anm 已装载、'@R' 表面可用
    // DAT_004c1788/8c（关卡相机抖动）与 DAT_004c1530/34（宽场地相机 +0xfc 抖动）：0x43d870 每帧末清零
    ScreenShakeOffset* screen_shake = nullptr;

    // 0x43cb10：按窗口尺寸（640×480）初始化四台相机
    void init_cameras(float window_scale);
    // 0x43b520 + 0x43d970：合成 VM 装 text.anm 脚本（640 宽：0x3b / 0x41 / 0x3e / 0x44）并各跑一帧
    void create(AnmManager& manager, AnmFile& text);
    // 0x42d7b0：每帧推进四个合成 VM
    void tick(AnmManager& manager);
    // 0x42cb60 开头：VM0 中断 2 并立即推进（装载画面起始形态）
    void begin_loading(AnmManager& manager);
    // 0x42cb60 结尾（0x4173c0 ×4）：四个 VM 挂中断 2，下一帧回到普通复制形态
    void request_plain_copy();
    // 0x43edc0：暂停菜单给 VM2 上色（最高字节强制 0xff）
    void set_pause_tint(u32 color) { set_color(composite_vms_[2], color | 0xff000000u); }

    // 0x43ba40：把十个绘制链节点按原版优先级登记
    void register_draw_chains(ChainManager& chains, AnmRenderer& renderer);

private:
    AnmVm composite_vms_[kCompositeVmCount];   // DAT_004c128c / 1290 / 1294 / 1298

    static void set_color(AnmVm& vm, u32 argb);
    ScreenRect viewport_rect(const Camera& camera) const;

    ChainResult pass_begin_frame(AnmRenderer& renderer);          // 0x43d140
    ChainResult pass_select_rt2_clear_depth(AnmRenderer& renderer);   // 0x43d400
    ChainResult pass_copy_rt1_to_rt2(AnmRenderer& renderer);      // 0x43d2f0
    ChainResult pass_select_rt1_clear_depth(AnmRenderer& renderer);   // 0x43d5a0
    ChainResult pass_copy_rt2_to_rt1(AnmRenderer& renderer);      // 0x43d4a0
    ChainResult pass_select_rt2_clear_field_depth(AnmRenderer& renderer);   // 0x43d6f0
    ChainResult pass_copy_rt1_to_rt2_again(AnmRenderer& renderer);   // 0x43d640
    ChainResult pass_select_backbuffer(AnmRenderer& renderer);    // 0x43d820
    ChainResult pass_present_field(AnmRenderer& renderer);        // 0x43d7c0
    ChainResult pass_end_frame(AnmRenderer& renderer);            // 0x43d870

    // 0x43d2f0 / 0x43d4a0 / 0x43d640 公共形：BLENDOP ADD、关 alpha test、关 Z 写、（可选）渲染目标相机、
    // 画合成 VM、VM 色复位白、出批、开 alpha test
    void copy_with_vm(AnmRenderer& renderer, AnmVm& vm, bool use_render_target_camera, int extra_layer);
};

}  // namespace th16
