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

    // Production path: shared SpellState is the sole authority for capture,
    // timer and bonus. This consumes every actual begin/end instruction event;
    // it never adds score or writes a record. chapter = ECL 524 章节号
    //（原版 0x41381e 直读 DAT_004be824 同一变量；RunScene 传活值）。
    bool consume(const SpellEvent&, const float boss_position[3],
                 const std::function<bool(i32)>& sound, i32 chapter);
    void sync(const SpellSnapshot&);
    // 00413080 draw callback: read selected character/mode counters from the
    // host's live FileStore; queue text before the Trance capture mutation.
    bool draw(HudDrawServices&, i32 captures, i32 attempts);
    bool rasterize_name(DialogueTextureSink&, const NameRasterizer&);
    const SpellSnapshot& snapshot() const noexcept { return snapshot_; }
    u32 display_handle(i32 index) const noexcept {
        return index >= 0 && index < 3 ? displays_[index] : 0;
    }
    // 0x413a60 -> 0x42a430 case0/case1：front script59（Get Spell Card
    // Bonus!!）/ script60（Bonus Failed...）句柄（原版 stage+0xcc）。
    u32 banner_handle() const noexcept { return banner_; }
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
    float anchor_[3] = {0, 0, 0};         // +0xac.. 快照位
    SpellSnapshot snapshot_;
    u32 displays_[3] = {};
    u32 stage_background_ = 0, decoration_ = 0;
    u32 banner_ = 0;  // 0x42a430 写在 stage ctx+0xcc 的结算横幅
    // 0x42a4e2-0x42a876（case0 捕获分支）：结算数字 8 位（ascii script
    // 4..11，句柄组 stage+0xa4）+ 分隔符（≥10^6 script12 → +0xc4、
    // ≥10^3 script13 → +0xc8，sprite 0x11c=284）+ 捕获特效 front
    // script95（+0x118）。
    u32 bonus_digits_[8] = {};
    u32 bonus_million_ = 0, bonus_thousand_ = 0;
    u32 bonus_capture_effect_ = 0;
    bool production_ = false;
    std::vector<u32> retiring_displays_;
    bool retire_production();
    bool fail(const std::string&);
};

}  // namespace th13
