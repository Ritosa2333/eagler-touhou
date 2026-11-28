#pragma once

#include "AnmManager.hpp"
#include "StageSim.hpp"
#include "Types.hpp"

#include <array>
#include <functional>
#include <string>

namespace th13 {

struct BossFrameInput {
    const StageEnemy* boss = nullptr;
    bool boss_slot_present = false;
    bool boss_leaving = false;
    bool dialog_active = false;
    float player_x = 0, player_y = 0;
    i32 timer_seconds = -1;
    i32 timer_frames = 0;
    i32 timer_centiseconds = -1;
    i32 remaining_stars = 0;
    i32 name_script = -1;
    i32 boss_life = 0;
    i32 life_initial = 0;
    struct PhaseMarker {
        i32 frame = 0;
        float ratio = 0;
    } markers[4];
    bool trance_mode = false;
};

class BossPresentation {
public:
    struct Banks {
        i32 front = 3, ascii = 2, effect = 8;
    };
    static constexpr i32 kRingArcScript = 240;
    static constexpr i32 kRingOverlayScripts[2] = {241, 242};
    static constexpr i32 kRingStarScript = 243;
    static constexpr float kStarOrbitRadius = 56.f;
    static constexpr i32 kTimerTensScript = 2, kTimerOnesScript = 3;
    static constexpr i32 kTimerDigitSpriteBase = 270;

    explicit BossPresentation(AnmManager& animations) : BossPresentation(animations, Banks{}) {}
    explicit BossPresentation(AnmManager& animations, Banks banks) : animations_(animations), banks_(banks) {}

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
    RollbackState capture_rollback() const {
        return {gate_,
                displayed_ratio_,
                target_ratio_,
                timer_seconds_,
                timer_displayed_,
                timer_frames_,
                timer_centiseconds_,
                remaining_star_handles_,
                warn_state_,
                move_state_,
                escalate_state_,
                proximity_hidden_,
                stars_created_,
                ring_created_,
                marker_glyph_,
                digit_tens_,
                digit_ones_,
                ring_arc_,
                ring_overlays_,
                star_handles_,
                marker_handle_,
                name_handle_};
    }
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

    bool initialize();
    void prepare_stage_assets() noexcept { timer_seconds_ = timer_displayed_ = -1; }
    void release();

    bool update(const BossFrameInput& input, const std::function<bool(i32)>& sound);

    void spell_started();
    void spell_ended();
    void interrupt_digits(i32 label);

    bool timer_visible() const noexcept { return gate_ && timer_seconds_ >= 0; }
    i32 timer_seconds() const noexcept { return timer_seconds_; }
    i32 timer_centiseconds() const noexcept {
        return timer_visible() ? (timer_centiseconds_ >= 0 ? timer_centiseconds_ : timer_frames_ * 100 / 60) : 0;
    }
    float timer_text_x() const noexcept;
    float timer_text_y() const noexcept;
    u32 timer_digit_color() const;

    float displayed_ratio() const noexcept { return displayed_ratio_; }
    u32 ring_arc_handle() const noexcept { return ring_arc_; }
    u32 ring_overlay_handle(i32 i) const noexcept { return ring_overlays_[u32(i < 0 || i > 1 ? 0 : i)]; }
    const std::array<u32, 4>& star_handles() const noexcept { return star_handles_; }
    u32 marker_handle() const noexcept { return marker_handle_; }
    bool proximity_hidden() const noexcept { return proximity_hidden_; }
    i32 warn_state() const noexcept { return warn_state_; }
    i32 marker_glyph() const noexcept { return marker_glyph_; }
    bool marker_visible() const noexcept;
    u32 digit_tens_handle() const noexcept { return digit_tens_; }
    u32 digit_ones_handle() const noexcept { return digit_ones_; }

    std::string error;

private:
    bool create_ring();
    void retire_ring();
    bool create_stars();
    void write_star(u32 handle, const BossFrameInput& input, i32 index);
    bool ensure_marker();
    bool update_marker(const BossFrameInput&);
    void fail(const char* why) {
        if (error.empty()) error = why;
    }

    AnmManager& animations_;
    Banks banks_;

    bool gate_ = false;
    float displayed_ratio_ = 0;
    float target_ratio_ = 0;
    i32 timer_seconds_ = -1;
    i32 timer_displayed_ = -1;
    i32 timer_frames_ = 0;
    i32 timer_centiseconds_ = -1;
    std::array<u32, 10> remaining_star_handles_{};
    i32 warn_state_ = 0;
    u32 move_state_ = 0;
    i32 escalate_state_ = 0;
    bool proximity_hidden_ = false;
    bool stars_created_ = false;
    bool ring_created_ = false;
    i32 marker_glyph_ = 0xff;

    u32 digit_tens_ = 0, digit_ones_ = 0;
    u32 ring_arc_ = 0;
    std::array<u32, 2> ring_overlays_{};
    std::array<u32, 4> star_handles_{};
    u32 marker_handle_ = 0;
    u32 name_handle_ = 0;
};

}
