#pragma once

#include "AnmManager.hpp"
#include "DialogueText.hpp"
#include "HudDrawData.hpp"
#include "SpellState.hpp"
#include "Types.hpp"

#include <functional>
#include <string>

namespace th13 {

class SpellPresentation {
public:
    using NameRasterizer = std::function<bool(TextureImage&, i32 x, i32 y, u32 color, const std::string& cp932)>;
    static constexpr i32 kStage1CardBgScript = 139 + 11;

    struct Banks {
        i32 stage = 4;
        i32 stage_number = 1;
        i32 ascii = 2, text = 0, effects = 8;
        i32 front = 3;
    };
    struct CardBgRow {
        i32 a_bank, a_script;
        i32 b_bank, b_script;
        i32 decor_a_bank, decor_a_script;
        i32 decor_b_bank, decor_b_script;
    };
    static const CardBgRow kCardBgRows[7];
    explicit SpellPresentation(AnmManager& animations) : SpellPresentation(animations, Banks{}) {}
    explicit SpellPresentation(AnmManager& animations, Banks banks) : animations_(animations), banks_(banks) {}

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
    RollbackState capture_rollback() const {
        return {flags_,
                spell_id_,
                name_,
                bonus_base_,
                bonus_cap_,
                duration_,
                background_,
                anchor_,
                snapshot_,
                displays_,
                stage_background_,
                decoration_,
                banner_,
                notice_,
                spirit_notice_,
                spirit_notices_,
                bonus_digits_,
                bonus_million_,
                bonus_thousand_,
                bonus_capture_effect_,
                capture_frames_,
                production_,
                retiring_displays_};
    }
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

    bool consume(const SpellEvent&, const float boss_position[3], const std::function<bool(i32)>& sound, i32 chapter);
    void sync(const SpellSnapshot&);
    void update_background(const float boss_position[3]);
    bool draw(HudDrawServices&, i32 captures, i32 attempts);
    bool draw_capture_times(HudDrawServices&, i32 packed_time);
    bool rasterize_name(DialogueTextureSink&, const NameRasterizer&);
    const SpellSnapshot& snapshot() const noexcept { return snapshot_; }
    u32 display_handle(i32 index) const noexcept { return index >= 0 && index < 3 ? displays_[index] : 0; }
    u32 banner_handle() const noexcept { return banner_; }
    bool show_notice(i32 front_script);
    bool show_spirit_notice(i32 level, float x, float y, float z);
    u32 notice_handle() const noexcept { return notice_; }
    u32 spirit_notice_handle() const noexcept { return spirit_notice_; }
    const std::vector<u32>& spirit_notice_handles() const noexcept { return spirit_notices_; }
    u32 bonus_digit_handle(i32 index) const noexcept { return index >= 0 && index < 8 ? bonus_digits_[index] : 0; }
    u32 bonus_separator_million_handle() const noexcept { return bonus_million_; }
    u32 bonus_separator_thousand_handle() const noexcept { return bonus_thousand_; }
    u32 bonus_capture_effect_handle() const noexcept { return bonus_capture_effect_; }
    u32 stage_background_handle() const noexcept { return stage_background_; }
    u32 decoration_handle() const noexcept { return decoration_; }

    bool begin(i32 spell_id, const std::string& name, i32 duration_frames, i32 stage_number, i32 difficulty,
               const float boss_position[3], const std::function<bool(i32)>& sound);
    bool end(bool captured, bool last_spell, const std::function<bool(i32)>& sound,
             const std::function<void(i32)>& add_score);
    void break_capture() { flags_ &= ~0x2u; }

    void release();

    bool active() const noexcept { return (flags_ & 0x1u) != 0; }
    bool captured() const noexcept { return (flags_ & 0x2u) != 0; }
    i32 bonus_base() const noexcept { return bonus_base_; }
    i32 bonus_display() const noexcept { return bonus_cap_; }
    i32 duration_seconds() const noexcept { return duration_; }
    i32 duration_frames() const noexcept { return duration_; }
    i32 spell_id() const noexcept { return spell_id_; }
    const std::string& name() const noexcept { return name_; }
    u32 background_handle() const noexcept { return background_; }

    std::string error;

private:
    AnmManager& animations_;
    Banks banks_;
    u32 flags_ = 0;
    i32 spell_id_ = -1;
    std::string name_;
    i32 bonus_base_ = 0, bonus_cap_ = 0;
    i32 duration_ = 0;
    u32 background_ = 0;
    std::array<float, 3> anchor_{};
    SpellSnapshot snapshot_;
    std::array<u32, 3> displays_{};
    u32 stage_background_ = 0, decoration_ = 0;
    u32 banner_ = 0;
    u32 notice_ = 0;
    u32 spirit_notice_ = 0;
    std::vector<u32> spirit_notices_;
    std::array<u32, 8> bonus_digits_{};
    u32 bonus_million_ = 0, bonus_thousand_ = 0;
    u32 bonus_capture_effect_ = 0;
    i32 capture_frames_ = 0;
    bool production_ = false;
    std::vector<u32> retiring_displays_;
    bool retire_production();
    bool fail(const std::string&);
};

}
