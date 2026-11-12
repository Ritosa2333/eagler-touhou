// Spell presentation consumes the effective SpellState at each ECL event.
// 00413270 creates ascii0/text74/ascii1 display roots, effect13 (children11/12
// receive duration in frames), effect20 and stage-specific background/decor.
// 00413a60 interrupts the display roots at label1, retires backgrounds and
// plays capture0x2e / timeout0x45 sounds. Score/persistence belong to the host.
// Numeric state is shared. Name rasterization updates the real text74 texture
// through a host font/device callback; 00413080 text uses the shared HUD queue.
// Original glyph edges and final pixels require independent rendering evidence.
#pragma once

#include "AnmManager.hpp"
#include "HudDrawData.hpp"
#include "DialogueText.hpp"
#include "SpellState.hpp"
#include "Types.hpp"

#include <functional>
#include <string>

namespace th13 {

class SpellPresentation {
public:
    // A real font rasterizer supplies white CP932 coverage at the original
    // style0 (32px/400). Layout, outline, scaling and texture submission stay
    // in this presentation path; synthetic tests provide an explicit mask.
    using NameRasterizer = std::function<bool(TextureImage&, i32 x, i32 y, u32 color,
                                               const std::string& cp932)>;
    // st01 合并 stage bank：enemy.anm（139 脚本）+ st01enm.anm（19 脚本）；
    // cardbg0 根 = st01enm 局部 11（spawn 12/13/14 子条带，dump 实证）。
    static constexpr i32 kStage1CardBgScript = 139 + 11;

    struct Banks {
        i32 stage = 4;  // Application kStageBank
        i32 stage_number = 1;
        i32 ascii = 2, text = 0, effects = 8;
        // front.anm（Application kFrontBank=3）：结算横幅 script59/60 的来源。
        // 0x42a440/0x42a879 用 stage+0x2a8（front bank）经 0x403800 取模板。
        i32 front = 3;
    };
    // 0x4bb4f0 + stage*0x7c 舞台表行 1..7（EXE 实读；下标 = 舞台号-1）。
    // 0x41381e-0x4139b6（capstone 直读，非 Ghidra 摘要）：符卡开始时
    // 章节 DAT_004be824 < 6 且 A 行非 -1 → cardbg 用 +0x34/+0x38、装饰用
    // +0x44/+0x48（槽 -1 不建装饰）；否则 B 行 +0x3c/+0x40 与 +0x4c/+0x50。
    // bank 列 = ECL ANIM 名单文件槽（0x4c2188+0x80：2=enemy.anm、
    // 3=stNXenm.anm、4=stNXmenm.anm/st06enm2）。stage3 A 行 = 小傘
    // st03menm（cdbg02）、B 行 = 芳香 st03enm（cdbg03）；stage5 A 行装饰 =
    // st05menm script9（中Boss）；stage7 A 行 = EX 中Boss st07menm。
    struct CardBgRow {
        i32 a_bank, a_script;               // +0x34/+0x38（章节<6 分支）
        i32 b_bank, b_script;               // +0x3c/+0x40
        i32 decor_a_bank, decor_a_script;   // +0x44/+0x48
        i32 decor_b_bank, decor_b_script;   // +0x4c/+0x50
    };
    static const CardBgRow kCardBgRows[7];
    explicit SpellPresentation(AnmManager& animations) : SpellPresentation(animations, Banks{}) {}
    explicit SpellPresentation(AnmManager& animations, Banks banks)
        : animations_(animations), banks_(banks) {}

    struct RollbackState {
        u32 flags_;
        i32 spell_id_;
        std::string name_;
        i32 bonus_base_;
        i32 bonus_cap_;
        i32 duration_;
        u32 background_;
        std::array<float, 3> anchor_;
        SpellSnapshot snapshot_;
        std::array<u32, 3> displays_;
        u32 stage_background_;
        u32 decoration_;
        u32 banner_;
        u32 notice_;
        u32 spirit_notice_;
        std::vector<u32> spirit_notices_;
        std::array<u32, 8> bonus_digits_;
        u32 bonus_million_;
        u32 bonus_thousand_;
        u32 bonus_capture_effect_;
        i32 capture_frames_;
        bool production_;
        std::vector<u32> retiring_displays_;
    };
    RollbackState capture_rollback() const { return {flags_, spell_id_, name_, bonus_base_, bonus_cap_, duration_, background_, anchor_, snapshot_, displays_, stage_background_, decoration_, banner_, notice_, spirit_notice_, spirit_notices_, bonus_digits_, bonus_million_, bonus_thousand_, bonus_capture_effect_, capture_frames_, production_, retiring_displays_}; }
    void restore_rollback(const RollbackState& state) {
        flags_ = state.flags_;
        spell_id_ = state.spell_id_;
        name_ = state.name_;
        bonus_base_ = state.bonus_base_;
        bonus_cap_ = state.bonus_cap_;
        duration_ = state.duration_;
        background_ = state.background_;
        anchor_ = state.anchor_;
        snapshot_ = state.snapshot_;
        displays_ = state.displays_;
        stage_background_ = state.stage_background_;
        decoration_ = state.decoration_;
        banner_ = state.banner_;
        notice_ = state.notice_;
        spirit_notice_ = state.spirit_notice_;
        spirit_notices_ = state.spirit_notices_;
        bonus_digits_ = state.bonus_digits_;
        bonus_million_ = state.bonus_million_;
        bonus_thousand_ = state.bonus_thousand_;
        bonus_capture_effect_ = state.bonus_capture_effect_;
        capture_frames_ = state.capture_frames_;
        production_ = state.production_;
        retiring_displays_ = state.retiring_displays_;
    }

    // Production path: shared SpellState is the sole authority for capture,
    // timer and bonus. This consumes every actual begin/end instruction event;
    // it never adds score or writes a record. chapter = ECL 524 章节号
    //（原版 0x41381e 直读 DAT_004be824 同一变量；RunScene 传活值）。
    bool consume(const SpellEvent&, const float boss_position[3],
                 const std::function<bool(i32)>& sound, i32 chapter);
    void sync(const SpellSnapshot&);
    // 00412dfe..00412eb1: Update26 smooths the effect13 root toward boss slot0.
    // State synchronization and draw callbacks never advance this position.
    void update_background(const float boss_position[3]);
    // 00413080 draw callback: read selected character/mode counters from the
    // host's live FileStore; queue text before the Trance capture mutation.
    bool draw(HudDrawServices&, i32 captures, i32 attempts);
    // 0x427910:56-106（实读反汇编 0x4279ea-0x427c03）捕获时间 HUD：
    // 门 = stage+0x120（0x42a4e2 case0 建 front95 后置 1）+ 该 VM(+0x118) 有效；
    // 第一对 = manager+0x94（00412ee0 在符卡结束把 +0x90 active_frames 抄入）
    // 的 sec/hh；第二对 = FUN_00424e20 解码 packed_time（DAT_004c2178+0xa8，
    // 0x412ee0 用 wall clock 打包；校验失败 → 999.99）。文本均 font4/space1/
    // 0.6 缩放，alpha 取特效 VM+0x52b，位置 (318,192)(362,198) 与
    // (318,208)(362,214)，格式 "%3d." / "%.2ds"。
    bool draw_capture_times(HudDrawServices&, i32 packed_time);
    bool rasterize_name(DialogueTextureSink&, const NameRasterizer&);
    const SpellSnapshot& snapshot() const noexcept { return snapshot_; }
    u32 display_handle(i32 index) const noexcept {
        return index >= 0 && index < 3 ? displays_[index] : 0;
    }
    // 0x413a60 -> 0x42a430 case0/case1：front script59（Get Spell Card
    // Bonus!!）/ script60（Bonus Failed...）句柄（原版 stage+0xcc）。
    u32 banner_handle() const noexcept { return banner_; }
    // 0x42a439 = jmp [eax*4+0x42abd4]（函数入口 0x42a430，jmp 在 +9）。
    // 全 EXE 可达 case 只有 0/1/2/4（0x413b05 / 0x413ba4 / 0x42ee28+0x43d23e /
    // 0x43d3a6+0x43d428）；case3(front62) 在不可达块、case5 空、case6(front64)
    // 无调用方，故本面只认 61/63。case2(0x42a9d4)/case4(0x42aa9f) 用
    // stage ctx+0xd0，case0/1/6 用 +0xcc（banner_）——两个独立槽，互不 retire。
    // front61/63（anm-list 7651/7681：sprite50/52）自带 ins_400(320,144,0)、
    // 层19；时序是 90 帧保持 + 10 帧 ins_409 淡出 + 20 帧 ins_1，故不手动设坐标。
    // 0x43d530：吸灵满槽，脚本号 = be808/200 + 0x6b → level1/2/3 =
    // front108/109/110（anm-list 8917/8935/8953：sprite78/79/80，**无 ins_400**）。
    // 原版由 FUN_0046f820 的 param_3 传自机世界坐标 (x, y-32, z)（0x43d583/
    // 0x43d589/0x43d5a4 取 DAT_004c22c4+0x5b8/+0x5bc/+0x5c0，y 减 32.0），
    // param_4=0.0→anm_arg、param_5=-1(layer 不覆盖)、param_6=0(flags，
    // 走 FUN_0046f3f0 主链、不加 +320/+16)。端口用 AnmManager::create 同式传入。
    // 0043d530 不保存返回句柄、也不回收上一级：快速跨 200/400/600 槽时三个
    // 提示动画各自按脚本 ins_6(96) 自然收尾。故本面保留全部 live 句柄
    //（spirit_notices_），getter 只给最近一次，release() 统一回收且对已自然
    // 结束句柄幂等（AnmRegistry generation 保证不误伤复用的新 VM）。
    // 非法 script / 非法 level 一律返回 true 且不改状态（纯改善前提）。
    bool show_notice(i32 front_script);
    bool show_spirit_notice(i32 level, float x, float y, float z);
    u32 notice_handle() const noexcept { return notice_; }
    u32 spirit_notice_handle() const noexcept { return spirit_notice_; }
    const std::vector<u32>& spirit_notice_handles() const noexcept {
        return spirit_notices_;
    }
    // 0x42a4e2-0x42a876（case0）：结算数字（stage+0xa4 数组）、分隔符
    //（+0xc4/+0xc8）与捕获特效（+0x118）句柄。
    u32 bonus_digit_handle(i32 index) const noexcept {
        return index >= 0 && index < 8 ? bonus_digits_[index] : 0;
    }
    u32 bonus_separator_million_handle() const noexcept { return bonus_million_; }
    u32 bonus_separator_thousand_handle() const noexcept { return bonus_thousand_; }
    u32 bonus_capture_effect_handle() const noexcept { return bonus_capture_effect_; }
    u32 stage_background_handle() const noexcept { return stage_background_; }
    u32 decoration_handle() const noexcept { return decoration_; }

    // Legacy isolated diagnostic adapter. Production uses consume/sync.
    // The historical duration_seconds accessor is an alias for frame units.
    bool begin(i32 spell_id, const std::string& name, i32 duration_frames,
               i32 stage_number, i32 difficulty, const float boss_position[3],
               const std::function<bool(i32)>& sound);
    // 0x413a60：符卡结束。captured = bit1 仍置（未破坏）。score 服务把
    // bonus/10 计入对局分（0x413a60:33-45 的 +0x80/10 同式）。
    bool end(bool captured, bool last_spell, const std::function<bool(i32)>& sound,
             const std::function<void(i32)>& add_score);
    // Legacy diagnostic adapter; production capture state belongs to SpellState.
    void break_capture() { flags_ &= ~0x2u; }

    void release();  // 场景卸载全回收

    bool active() const noexcept { return (flags_ & 0x1u) != 0; }
    bool captured() const noexcept { return (flags_ & 0x2u) != 0; }
    i32 bonus_base() const noexcept { return bonus_base_; }    // +0x80
    i32 bonus_display() const noexcept { return bonus_cap_; }  // +0x84
    i32 duration_seconds() const noexcept { return duration_; } // legacy name, frames
    i32 duration_frames() const noexcept { return duration_; }
    i32 spell_id() const noexcept { return spell_id_; }
    const std::string& name() const noexcept { return name_; }
    u32 background_handle() const noexcept { return background_; }

    std::string error;

private:
    AnmManager& animations_;
    Banks banks_;
    u32 flags_ = 0;      // copied SpellState flags; bit0x20 = bomb protection
    i32 spell_id_ = -1;  // +0x78
    std::string name_;   // +0x38
    i32 bonus_base_ = 0, bonus_cap_ = 0;  // +0x80/+0x84
    i32 duration_ = 0;                    // +0x88
    u32 background_ = 0;                  // +0x20 背景枚 VM
    std::array<float, 3> anchor_{};         // +0xac.. 符卡环平滑跟随位置
    SpellSnapshot snapshot_;
    std::array<u32, 3> displays_{};
    u32 stage_background_ = 0, decoration_ = 0;
    u32 banner_ = 0;  // 0x42a430 写在 stage ctx+0xcc 的结算横幅
    // 0x42a430 case2/3/4：非符卡横幅句柄（原版 stage ctx+0xd0 槽，与
    // banner_ 的 +0xcc 独立，互不 retire）。
    u32 notice_ = 0;
    // 0x43d530：吸灵满槽提示句柄（front108/109/110），独立槽。原版不存句柄，
    // 这里保存全部 live 句柄以保证 release 不泄露；spirit_notice_ = 最近一次。
    u32 spirit_notice_ = 0;
    std::vector<u32> spirit_notices_;
    // 0x42a4e2-0x42a876（case0 捕获分支）：结算数字 8 位（ascii script
    // 4..11，句柄组 stage+0xa4）+ 分隔符（≥10^6 script12 → +0xc4、
    // ≥10^3 script13 → +0xc8，sprite 0x11c=284）+ 捕获特效 front
    // script95（+0x118）。
    std::array<u32, 8> bonus_digits_{};
    u32 bonus_million_ = 0, bonus_thousand_ = 0;
    u32 bonus_capture_effect_ = 0;
    i32 capture_frames_ = 0; // manager+0x94 survives the next spell's +0x90 reset
    bool production_ = false;
    std::vector<u32> retiring_displays_;
    bool retire_production();
    bool fail(const std::string&);
};

}  // namespace th13
