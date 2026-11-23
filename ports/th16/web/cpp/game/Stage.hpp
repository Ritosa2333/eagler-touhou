// th16 StageManager——原版 DAT_004a6d9c / DAT_004a6da0（0x66f0 字节，0x409db0 创建）。
//
// 锚点：
//   0x4097c0  构造：装 STD（0x40ac30）、相机取全局 0x4c168c 默认值、注册更新链 0x11 / 绘制链 3 与 6
//   0x40add0  GameManager 首帧：对象 flags=1，按遍历顺序给每个四边形建面 VM（0x407b20 + 0x45f980）
//   0x409e50  更新：清抖动位移、facing 归一化、0x40aed0 面 VM 推进、0x40b3b0 脚本、相机写回全局、层精灵 VM 推进
//   0x40b3b0  STD 脚本解释（ins 0..20），之后推进四组插值与 rockingMode 摄像机晃动
//   0x409f90  绘制（优先级 3）：相机 → 雾 → 清场地矩形 → 逐层 0x40af70（层精灵 0x40c280 + 实例四边形）
//   0x40a410  绘制（优先级 6）：以关卡相机绘制 ANM 层 0x20 / 0x21；尾部推进结束计时 +0x66c8（换关：0x40c0d0 给上一关
//             置 bit1 + 0x1e 帧，0x40c210 给新关置 bit2 + 0x3c 帧），归零时 bit1 → bit3（0x42d7b0 据此析构上一关）
//
// 相机结构见 Camera.hpp（StageCamera = Camera，与全局 0x4c168c 同布局）。
//
// 原版把关卡画进 640×480 的场地渲染目标（渲染目标相机视口 (116,4,408,472)，中心 (320,240)），再由
// Supervisor 的合成 VM（ScreenCompositor）把渲染目标贴到屏幕场地 (32,16,384,448)。
#pragma once
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "AnmVm.hpp"
#include "Camera.hpp"
#include "Chain.hpp"
#include "Distortion.hpp"
#include "ScreenEffect.hpp"
#include "Std.hpp"
#include "Types.hpp"

#include <functional>
#include <string>
#include <vector>

namespace th16 {

class StageManager {
public:
    static constexpr int kLayerSprites = 8;
    static constexpr float kNearZ = 30.0f;    // 0x43c940 PerspectiveFovLH zn=0x41f00000
    static constexpr float kFarZ = 8000.0f;   // zf=0x45fa0000
    static constexpr float kFieldShiftX = -96.0f;  // 场地 RT 中心 (320,240) → 屏幕场地中心 (224,240)

    StdFile std;
    StageCamera camera;
    // DAT_004c168c：0x409e50 每帧脚本与插值之后整体抄出的全局相机副本（ANM 变量 10016..10021 读它）。
    // 由 GameSession 持有：换关期间上一关与新关先后写入，新关在 bit2 且计时 ≥ 0x3c 的首帧不更新，全局仍为上一关的值
    StageCamera* camera_global = nullptr;
    AnmFile* anm = nullptr;      // +0x66b4
    i32 stage_number = 0;        // +0x66dc

    // 脚本状态（this+0xc 起）
    Timer timer;                 // +0x0c
    i32 ins_index = 0;           // +0x20（指令下标；原版为字节偏移）
    i32 rocking_mode = 0;        // +0x24
    Timer rocking_timer;         // +0x28
    Interp<Vec3> facing_interp;  // +0x3c
    Interp<Vec3> pos_interp;     // +0x94
    Interp<Vec3> up_interp;      // +0xec
    Interp<StageFog> fog_interp; // +0x144

    std::vector<AnmVm> face_vms;            // +0x669c（面 VM 数 × 0x5fc）
    AnmVm layer_vms[kLayerSprites];         // +0x31c
    i32 layer_ids[kLayerSprites] = {};      // +0x32fc
    float cull_distance_sq = 0;             // +0x331c（ins 20 绘制距离的平方；0x40a7d0 先按实例中心到相机的距离剔除）
    // ins 17 关卡畸变（0x40c4a0 每帧更新；th16 数据只有第 1 关用种类 1：场地底部 384 × 128 的 7 行波浪网格）
    std::unique_ptr<DistortionGrid> distortion;   // +0x3314
    float distortion_radius_min = 112.0f;   // +0x3318（种类 2：半径下限）
    float distortion_radius = 192.0f;       // +0x331c（种类 2：每帧 −2 至下限）
    i32 distortion_unused = -1;             // +0x3320
    Timer distortion_timer;                 // +0x3324（种类 1：前 60 帧振幅从 0 升到 6）
    float distortion_angle_a = 0;           // +0x3338
    float distortion_angle_b = 0;           // +0x333c
    i32 distortion_kind = 0;                // +0x3340（ins 17 arg0）
    const bool* render_targets_ready = nullptr;   // DAT_004c127c：ins 17 只在渲染目标可用时建 VM
    const u32* spellcard_flags = nullptr;         // DAT_004a6db0+0x78：符卡进行中（bit0）时种类 1 不更新网格

    u32 flags = 1;               // +0x66c4：bit0 绘制启用  bit1 结束中  bit2 结束淡出  bit3 隐藏
    Timer end_timer;             // +0x66c8
    u32 tint = 0x808080;         // +0x3350：0x409e50 每帧写 0x808080；最高字节（+0x3353）为绘制侧闪白标志
    i32 frame = 0;               // +0x66e0
    i32 drawn_instances = 0;     // +0x66b8
    i32 culled_instances = 0;    // +0x66bc
    i32 drawn_quads = 0;         // +0x66c0
    u32 bg_color = 0xff000000;   // DAT_004c1b0c（ins 13）本关最后写入的值
    u32* screen_clear_color = nullptr;   // DAT_004c1b0c 本体（ScreenCompositor::background_color），ins 13 同步写入
    std::string error;

    // 0x4097c0 建（禁用）、0x40add0 启用：更新链 0x11、绘制链 3（0x409f90）/ 6（0x40a410）。GameSession 登记与摘除
    ChainEntry* update_chain = nullptr;
    ChainEntry* background_draw_chain = nullptr;
    ChainEntry* foreground_draw_chain = nullptr;

    // 0x4097c0 + 0x40ac30：解析 STD；anm 由调用方装入槽 3 + (stage & 1) 后传入
    // 0x4097c0：解析 STD；相机先整份抄自全局 0x4c168c（上一关留下的雾色 / 雾距 / 视口 / 法向等随之继承，
    // 直到本关 STD 自己设定），再覆盖 pos / facing / up 并清 pos_offset / facing_offset
    bool load(const u8* bytes, u32 size, i32 stage, const Camera& global_camera = Camera{});
    // 0x40add0：建面 VM 并重置脚本
    void start(AnmManager& manager);
    // 0x409e50
    void update(AnmManager& manager, float game_speed, const Rng* visual_rng = nullptr);
    // 0x40c4a0：ins 17 畸变网格每帧更新（种类 1 波浪 / 种类 2 透镜），末尾推进畸变计时
    void update_distortion(AnmManager& manager, float game_speed);

    // ---- 绘制链 ----
    // 0x45d150(3, 0x1e, 0xffffffff)：新关淡入白闪（0x409f90 在结束计时 < 0x1e 时建；更新链 0x13 + 绘制链 10）
    std::function<void(i32 kind, i32 duration, u32 color)> on_screen_fade;
    // 0x409f90（优先级 3）：关卡相机 → 雾参数 → 清场地矩形 → 淡入逻辑 → 层 0..7。renderer 无设备时只执行逻辑部分
    void draw_background(AnmManager& manager, AnmRenderer& renderer, Camera& global_camera, const ScreenShakeOffset& shake);
    // 0x40a410（优先级 6）：关卡相机下的 ANM 层 0x20 / 0x21，层 8..11，结束计时倒数
    void draw_foreground(AnmManager& manager, AnmRenderer& renderer, Camera& global_camera, const ScreenShakeOffset& shake,
                         float game_speed);
    // 不跑绘制链（无头对拍）时补上两条绘制链里改动逻辑状态的部分：
    // 0x409f90 的淡入 / 启用位逻辑；0x40a410 尾部的结束计时倒数（0x40d490），归零时 bit1 → bit3 并清 bit1 / bit2
    void transition_step();
    void end_timer_step(float game_speed);
    // 0x40c0d0 后半：上一关结束——bit1、计时 0x1e；0x40c210：新关淡入——bit2、计时 0x3c
    void begin_end(i32 frames, u32 flag);
    // 0x40b2f0 / ins 19：对全部面 VM 与层精灵 VM 挂中断并立即推进
    void interrupt(AnmManager& manager, i32 value);
    // 0x40c040 / ECL 630：从脚本开头找第一条 ins 16（标签）且参数 == label 的指令，把脚本指针与计时跳到它
    //（计时取该指令的 time，与 ins 1 跳转相同的初始化口径）；找不到则不动。st06bs Boss2..6 用它切换背景段落
    void jump_to_label(i32 label);
    // 0x43c940 的 eye / at：eye = pos + pos_offset，at = eye + facing_norm
    Vec3 eye() const;
    Vec3 at() const;

private:
    // 0x40af70：画第 layer 层——先 0x40c280 的层精灵（无雾、Z 关），再逐实例 × 四边形的面 VM
    //（写入 entity_offset / 尺寸缩放后立即绘制：同一面 VM 被多个实例复用）；实例先过 0x40a7d0 视锥剔除
    void draw_layer(int layer, AnmManager& manager, AnmRenderer& renderer, Camera& global_camera, const ScreenShakeOffset& shake);
    void draw_layer_sprites(int layer, AnmManager& manager, AnmRenderer& renderer, Camera& global_camera, const ScreenShakeOffset& shake);
    // 0x40a7d0：实例中心超过绘制距离，或包围盒 16 个采样点投影后与场地矩形 [0,384]×[0,448] 不相交 → 剔除
    bool instance_visible(const StdObject& object, const Vec3& instance_pos, const Camera& camera) const;
    // 0x409f90 / 0x40a410 开头共用：抖动偏移写进相机、整体抄到全局、0x43c940 建矩阵并切换
    void look_through_stage_camera(AnmRenderer& renderer, Camera& global_camera, const ScreenShakeOffset& shake);
    // 0x409f90 中段：bit2（新关淡入）下的启用位与白闪
    void step_fade_in();
    void run_script(AnmManager& manager, float game_speed);   // 0x40b3b0 前半
    void apply_rocking(float game_speed);                       // 0x40b3b0 后半
    void step_faces(AnmManager& manager);                       // 0x40aed0
    void set_fog_time(i32 time, i32 mode, const StageFog& goal); // 0x409490
    StageFog step_fog(float game_speed);                        // 0x40cd10
};

}  // namespace th16
