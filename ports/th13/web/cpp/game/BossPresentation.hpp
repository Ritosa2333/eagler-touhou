// Boss HUD 表现层（Stage1 范围）：原版 0x4265a0（stage ctx 每帧 boss 段）+
// 0x425170（计时数字 VM 常备）+ 0x427910（计时文本）的行为移植。
// 状态字段按原版 stage ctx（DAT_004c2190，0x2b4 字节）偏移注释；VM 写入按
// 原版直写点（render_flags/rotation/offset）同式，经 AnmRegistry 池调度绘制。
//
// 原版证据索引（artifacts/decomp/jp/functions/）：
//   004265a0.c:98-107  boss 门控：mgr+0x5c 槽存活、mgr+0x7c bit0==0、
//                        ctx+0x198 对话==0、ctx+0x1a0 计时>=0
//   004265a0.c:109-145  玩家贴顶移动态机（ctx+0x180 bits9-10；y<128 移开/
//                        y>160 复原迟滞；0x42b530 中断 5 / 0x42b510 中断 4）
//   004265a0.c:147-177  计时秒数变化：<5/<10 警告（0x4126b0×2 + 声 0xc/0xb），
//                        数字 VM 精灵 = 0x10e+digit（ascii sprite 270-279，
//                        ascii.anm dump 实证 16×16 数字字模），复位时仅中断
//   004265a0.c:225-239  血环比值：target = 敌机 life(+0x5138)/初值(+0x513c)，
//                        displayed(+0x1ac) 每帧 +0.025 逼近、目标更小即直落
//   004265a0.c:241-266  血环 VM 组（ctx+0x1dc..+0x1f4 七槽）惰性创建；
//                        0042b1f0 常态重建 ctx+0xdc 环根
//   004265a0.c:417-447  环根 rotation = -displayed*2π（原版写 VM+0x48）、
//                        跟随 VM offset(+0x574..) = boss 世界位
//   004265a0.c:448-482  四枚阶段星：displayed<角度 显示、rotation(-π-a*2π)
//   004271b7-004272b5  玩家-boss 距离 <80 中断3淡出 / >=96 中断2复原（迟滞）
//   004265a0.c:630-739  底部敌位标（ctx+0xa0）：offset=(boss_x+224,480)、
//                        字节 +0x52b = '@'+round(|dx|*191/64)，|dx|>=64 或
//                        boss 横向出界时隐藏；计时 <700/400/200（trance 组
//                        2000/1000/400）阶段中断 7/8 + 声
//   00427910.c:253-274  计时文本：'.' 与百分秒锚定数字 VM 位置(+0x3c/+0x40)
//                        +16/-7（数字 VM ascii.anm script2/3 dump：208/220,39）
//
// Live state comes from ECL511/527/540 and4218f0 via RunScene. Remaining
// card stars (front68..77) and circular phase markers(front243) are distinct.
// Bottom Enemy marker is front111 (425d90), with phase-HP flash labels7..10.
// Independent original GPU pixel equivalence remains unverified.
//
// 【认证状态】环/星 VM 身份与写入语义已由原版运行时取证闭合（UC 池模板
// hash + 反汇编写入点，见 kRingArcScript 注与 r4/boss-hud/pool-map/）：
// front.anm 240/241/242（条带）与 243（星）经 AnmRegistry 池真实绘制。
// 仍待复核的残留（不阻塞呈现）：条带段几何的渲染端对拍（mode 602 条带
// 已在 AnmRenderer 实装，像素一致性属后端门）。
//
// 【本批审计结论】BossPresentation 保持零行为改动。独立反编译复核确认：
// 环门判据是 0x4265a0:180/225-226（独立于 :98-107 的两数字 VM 门），
// 早先把两者统一会令 boss 非符卡阶段血环消失，已撤回；跨关 retire 依据
// 0x42bfa0.c:104-139 亦被否证，已撤回。颜色/几何/坐标/layer/缩放不动。
// 仍 BLOCKED（待 D3D9 逐像素捕获）：蓝条位置/样式、「同屏 2 条」是残留
// 句柄还是原版固有双条。注：front 241/242 是 0x4265a0 池模板 ANM VM，
// 0x427910:107-136 是 FUN_0045efc0 直绘矩形，两者不是同一机制，早先的
// 对应关系属臆测，此处不再引用。
#pragma once

#include "AnmManager.hpp"
#include "StageSim.hpp"
#include "Types.hpp"

#include <array>
#include <functional>
#include <string>

namespace th13 {

// 宿主装配的每帧 boss 视图（RunScene/测试填充；不复制 sim 状态）。
struct BossFrameInput {
    const StageEnemy* boss = nullptr;  // mgr+0x5c 槽 0 指向的实体（sim.boss()）
    bool boss_slot_present = false;    // mgr+0x5c 槽 0 占用（script_id>=0）。
                                       // 0x4265a0:98-107 计时门只看槽占用，
                                       // 不看实体存活：终符捕获（514→BossDead）
                                       // 后槽由 512(-1) 约 +60 帧才清，期间
                                       // 计时器保持显示（native-s3 trace：
                                       // boss 实体保留 61 帧）。
    bool boss_leaving = false;         // mgr+0x7c bit0
    bool dialog_active = false;        // ctx+0x198（对话进行中）
    float player_x = 0, player_y = 0;  // DAT_004c22c4+0x5b8/+0x5bc（世界位）
    i32 timer_seconds = -1;            // ctx+0x1a0；-1 = 无计时
    i32 timer_frames = 0;              // 帧内计数（百分秒 = frames*100/60）
    i32 timer_centiseconds=-1; // native4218f0 already converts remainder to hundredths
    i32 remaining_stars=0; // ECL540, front scripts68..77
    i32 name_script = -1; // stage table +74/+78 plus155, 0042b1f0
    i32 boss_life = 0;                 // 敌机+0x5138（int 口径）
    i32 life_initial = 0;              // 敌机+0x513c；<=0 时环隐藏
    struct PhaseMarker {
        i32 frame = 0;   // ctx+0x1c0+id*8, packed color (legacy field name)
        float ratio = 0; // ctx+0x1bc+id*8（ECL527）
    } markers[4];
    bool trance_mode = false;  // legacy name; native SPELL active bit0 selects HP thresholds
};

// Boss HUD 状态机 + VM 面。逻辑帧 update() 与文本面 draw 查询分离，
// 与 GameHud 的 Frame/Draw 分离同构。
class BossPresentation {
public:
    struct Banks {
        i32 front = 3, ascii = 2, effect = 8;  // Application kBanks/kFrontBank
    };
    // 【环 VM 身份（原版运行时 UC 取证定案，r4/boss-hud/pool-map/）】
    // stage mgr（ctx+0x2a8）池模板槽 240/241/242/243（0x426b06/0x426bad/
    // 0x426c66/0x426d13..0x426f1a 反汇编）→ front.anm 脚本 240/241/242/243
    //（entry10 front/lifebar.png；sha1 匹配 raw 资源）：
    //   240 弧驱动（sprite150 4×16、layer 17、ins_602 条带、10000=38 段）
    //   241/242 覆盖条（sprite151 2×16、10000=38 段、scale.y=58/54）
    //   243 阶段星（sprite152 3×6、mode 1 四元）×4 克隆
    // 写入点（th13.exe 0x426f9e-0x42719x）：驱动 VM+0x18|=4、
    // +0x48(rotation.x) = -displayed*2π（0x4ae850=2π）；三条带 +0x574.. =
    // boss 世界位；星 VM+0x18|=2（displayed<angle）、ratio==0 隐藏
    //（0x4270da-0x4270e5）、+0x50(rotation.z) = normalize(−π − angle*2π)
    //（0x4ae880=−π）、+0x574.. = boss + R(t)·(0,56,0) =
    // boss + (−56·sin t, 56·cos t, 0)（半径 0x4aef18=56，0x472fc0 标准旋转；
    // 调用点 push 后 ESP 偏移见 BossPresentation.cpp）。
    static constexpr i32 kRingArcScript = 240;
    static constexpr i32 kRingOverlayScripts[2] = {241, 242};
    static constexpr i32 kRingStarScript = 243;
    static constexpr float kStarOrbitRadius = 56.f;
    // 计时数字 VM：ascii.anm script2（十位 @208,39）/ script3（个位 @220,39）。
    static constexpr i32 kTimerTensScript = 2, kTimerOnesScript = 3;
    static constexpr i32 kTimerDigitSpriteBase = 270;  // ascii sprite 270-279

    explicit BossPresentation(AnmManager& animations) : BossPresentation(animations, Banks{}) {}
    explicit BossPresentation(AnmManager& animations, Banks banks)
        : animations_(animations), banks_(banks) {}

    struct RollbackState {
        bool gate_;
        float displayed_ratio_;
        float target_ratio_;
        i32 timer_seconds_;
        i32 timer_displayed_;
        i32 timer_frames_;
        i32 timer_centiseconds_;
        std::array<u32, 10> remaining_star_handles_;
        i32 warn_state_;
        u32 move_state_;
        i32 escalate_state_;
        bool proximity_hidden_;
        bool stars_created_;
        bool ring_created_;
        i32 marker_glyph_;
        u32 digit_tens_;
        u32 digit_ones_;
        u32 ring_arc_;
        std::array<u32, 2> ring_overlays_;
        std::array<u32, 4> star_handles_;
        u32 marker_handle_;
        u32 name_handle_;
    };
    RollbackState capture_rollback() const { return {gate_, displayed_ratio_, target_ratio_, timer_seconds_, timer_displayed_, timer_frames_, timer_centiseconds_, remaining_star_handles_, warn_state_, move_state_, escalate_state_, proximity_hidden_, stars_created_, ring_created_, marker_glyph_, digit_tens_, digit_ones_, ring_arc_, ring_overlays_, star_handles_, marker_handle_, name_handle_}; }
    void restore_rollback(const RollbackState& state) {
        gate_ = state.gate_;
        displayed_ratio_ = state.displayed_ratio_;
        target_ratio_ = state.target_ratio_;
        timer_seconds_ = state.timer_seconds_;
        timer_displayed_ = state.timer_displayed_;
        timer_frames_ = state.timer_frames_;
        timer_centiseconds_ = state.timer_centiseconds_;
        remaining_star_handles_ = state.remaining_star_handles_;
        warn_state_ = state.warn_state_;
        move_state_ = state.move_state_;
        escalate_state_ = state.escalate_state_;
        proximity_hidden_ = state.proximity_hidden_;
        stars_created_ = state.stars_created_;
        ring_created_ = state.ring_created_;
        marker_glyph_ = state.marker_glyph_;
        digit_tens_ = state.digit_tens_;
        digit_ones_ = state.digit_ones_;
        ring_arc_ = state.ring_arc_;
        ring_overlays_ = state.ring_overlays_;
        star_handles_ = state.star_handles_;
        marker_handle_ = state.marker_handle_;
        name_handle_ = state.name_handle_;
    }

    // 0x425170:181-287：数字 VM 常备、初始隐藏（flags bit1 清）。
    bool initialize();
    // 00424fd0：换关只复位 ctx+0x1a0/+0x1a8 的计时显示，保留数字计时 VM
    //（ctx+0x98/+0x9c，0x425170 常备）与 HUD 其它持久态。环/覆盖条/阶段星
    // 的回收由 0x4265a0:192-213 的空槽逐帧分支承担（7 句柄 + 创建旗 +
    // ctx+0xdc 环根），端口 update() 的 if(!ring_gate) 已实现同式，此处不再
    // 重复 retire。独立复核否证了早先「0x42bfa0.c:104-139 是 0xc 换关回收
    // 清单」的说法：那是 (be830&2)==0 的全量清理分支，全函数不含
    // 0x1dc/0x1e0/0x1e4/0x1e8/0x1f4/0x2a8/0x46fc30 任一引用；0xc 分支置
    // be830|=2 后走 :175-217 的「隐藏 + 清 bit1」，并不销毁环组 VM。
    void prepare_stage_assets() noexcept {
        timer_seconds_ = timer_displayed_ = -1;
    }
    // 场景卸载：环组/数字 VM 全量回收（0x4265a0 失活支路 0x46fc30×7 同义）。
    void release();

    // 每帧逻辑（0x4265a0 boss 段同序）。sound 回调 = 原版 0x460bc0(vm,id)。
    bool update(const BossFrameInput& input, const std::function<bool(i32)>& sound);

    // 0x412890：符卡开始 → 两数字 VM 中断 2。
    void spell_started();
    // 0x412900（0x413a60 结束开头）：符卡结束 → 两数字 VM 中断 3（拉回 y23）。
    void spell_ended();
    // 0x4265a0 玩家贴顶态机的中断目标为数字 VM（原版经 0x42b510/0x42b530）。
    void interrupt_digits(i32 label);

    // ---- 文本面（0x427910:253-274；GameHudDraw 消费） ----
    bool timer_visible() const noexcept { return gate_ && timer_seconds_ >= 0; }
    i32 timer_seconds() const noexcept { return timer_seconds_; }
    i32 timer_centiseconds() const noexcept {
        return timer_visible() ? (timer_centiseconds_>=0?timer_centiseconds_:timer_frames_*100/60) : 0;
    }
    // 数字 VM 脚本位（208/220,39）+16/-7 锚（0x427910:255-257）。
    float timer_text_x() const noexcept;
    float timer_text_y() const noexcept;
    u32 timer_digit_color() const;  // 数字 VM 色（VM+0x528 → visual.color）

    // 诊断/测试面。
    float displayed_ratio() const noexcept { return displayed_ratio_; }
    u32 ring_arc_handle() const noexcept { return ring_arc_; }        // 槽 240
    u32 ring_overlay_handle(i32 i) const noexcept {  // 槽 241/242
        return ring_overlays_[u32(i < 0 || i > 1 ? 0 : i)];
    }
    const std::array<u32, 4>& star_handles() const noexcept { return star_handles_; }
    u32 marker_handle() const noexcept { return marker_handle_; }
    bool proximity_hidden() const noexcept { return proximity_hidden_; }
    i32 warn_state() const noexcept { return warn_state_; }  // 0 无/1 <10/2 <5
    i32 marker_glyph() const noexcept { return marker_glyph_; }  // +0x52b 字节
    bool marker_visible() const noexcept;
    // 计时数字 VM 句柄（ascii script2=十位/3=个位）。
    u32 digit_tens_handle() const noexcept { return digit_tens_; }
    u32 digit_ones_handle() const noexcept { return digit_ones_; }

    std::string error;

private:
    bool create_ring();                     // 槽 240/241/242 → front 240/241/242
    void retire_ring();                     // 0x46fc30 家族回收
    bool create_stars();                    // 槽 243×4 → front 243
    void write_star(u32 handle, const BossFrameInput& input, i32 index);
    bool ensure_marker();
    bool update_marker(const BossFrameInput&);                   // ctx+0xa0 常备（0x425170:447-508）
    void fail(const char* why) {
        if (error.empty()) error = why;
    }

    AnmManager& animations_;
    Banks banks_;

    // ---- 状态（stage ctx 偏移对照）----
    bool gate_ = false;              // 0x4265a0:98-107 总门
    float displayed_ratio_ = 0;      // +0x1ac
    float target_ratio_ = 0;         // +0x1b0
    i32 timer_seconds_ = -1;         // +0x1a0（输入镜像）
    i32 timer_displayed_ = -1;       // +0x1a8
    i32 timer_frames_ = 0;
    i32 timer_centiseconds_=-1;
    std::array<u32,10> remaining_star_handles_{};
    i32 warn_state_ = 0;             // 0x4265a0:147-163 警告档
    u32 move_state_ = 0;             // +0x180 bits9-10（0/1 移开迟滞）
    i32 escalate_state_ = 0;         // +0x180 bits1-2（<700/400/200 阶段）
    bool proximity_hidden_ = false;  // +0x1fc
    bool stars_created_ = false;     // (slot+6)*0x54 创建旗
    bool ring_created_ = false;      // 同上（三条带）
    i32 marker_glyph_ = 0xff;        // 位标字节（+0x52b 语义）

    // ---- VM 句柄（ctx 槽对照）----
    u32 digit_tens_ = 0, digit_ones_ = 0;  // +0x98/+0x9c
    u32 ring_arc_ = 0;                     // +0x1dc（front 240 弧驱动）
    std::array<u32, 2> ring_overlays_{};   // +0x1e0/+0x1e4（front 241/242）
    std::array<u32, 4> star_handles_{};    // +0x1e8..+0x1f4（front 243 ×4）
    u32 marker_handle_ = 0;                // +0xa0 底部敌位标
    u32 name_handle_ = 0;                  // +0xdc Boss nameplate
};

}  // namespace th13
