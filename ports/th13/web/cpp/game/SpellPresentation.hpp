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
    };
    explicit SpellPresentation(AnmManager& animations) : SpellPresentation(animations, Banks{}) {}
    explicit SpellPresentation(AnmManager& animations, Banks banks)
        : animations_(animations), banks_(banks) {}

    // Production path: shared SpellState is the sole authority for capture,
    // timer and bonus. This consumes every actual begin/end instruction event;
    // it never adds score or writes a record.
    bool consume(const SpellEvent&, const float boss_position[3],
                 const std::function<bool(i32)>& sound);
    void sync(const SpellSnapshot&, const float* boss_position = nullptr);
    // 00413080 draw callback: read selected character/mode counters from the
    // host's live FileStore; queue text before the Trance capture mutation.
    bool draw(HudDrawServices&, i32 captures, i32 attempts);
    bool rasterize_name(DialogueTextureSink&, const NameRasterizer&);
    const SpellSnapshot& snapshot() const noexcept { return snapshot_; }
    u32 display_handle(i32 index) const noexcept {
        return index >= 0 && index < 3 ? displays_[index] : 0;
    }
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
    bool production_ = false;
    std::vector<u32> retiring_displays_;
    bool retire_production();
    bool fail(const std::string&);
};

}  // namespace th13
