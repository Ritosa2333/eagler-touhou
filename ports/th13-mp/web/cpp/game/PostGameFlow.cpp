#include "PostGameFlow.hpp"

#include "ScoreBoard.hpp"

namespace th13 {

void PostGameFlow::enter(OverlayState next) {
    state_ = next;
    age_ = 0;
    progress_.scene_flags |= 0x10u;
}

void PostGameFlow::apply_stage_clear_bonus() {
    const i32 bonus = stage_clear_bonus(progress_.stage);
    progress_.stage_clear_bonus = bonus;
    score_.score = i32(add_bonus_capped(u32(score_.score), bonus));
}

void PostGameFlow::on_stage_clear() {
    const u32 mode = (progress_.run_mode_flags >> 4) & 3;
    const bool spell_practice = (progress_.run_mode_flags & 0x30u) == 0x20u;
    if (!spell_practice) apply_stage_clear_bonus();
    const bool practice = progress_.practice_mode != 0;
    if (mode == 0) {
        if (progress_.stage == 6) {
            progress_.scene_flags |= 0x4000u;
            clear_sequence_ = true;
            clear_frame_ = 0;
            if (progress_.ending_points > 599) progress_.good_endings++;
            if (player_.bombs > 2) progress_.good_endings = 3;
            const i32 bonus = all_clear_bonus(progress_.difficulty, player_.extra_lives, player_.bombs);
            score_.score = i32(add_bonus_capped(u32(score_.score), bonus));
            progress_.stage_clear_bonus += bonus;
            if (practice) on_practice_end();
            return;
        }
        if (progress_.stage == 7) {
            const i32 bonus = extra_clear_bonus(player_.extra_lives, player_.bombs);
            score_.score = i32(add_bonus_capped(u32(score_.score), bonus));
            progress_.stage_clear_bonus += bonus;
            if (practice) {
                on_practice_end();
            } else {
                on_extra_end();
            }
            return;
        }
        if (!practice) {
            const u32 bit = u32(progress_.stage + progress_.difficulty * 8);
            if (bit < 32) progress_.stage_cleared_bits |= 1u << bit;
        }
        host_.request_destination(0xc);
        if (progress_.stage < 7) progress_.stage++;
        return;
    }
    if (practice) {
        on_practice_end();
    } else {
        on_extra_end();
    }
}

void PostGameFlow::on_game_over() {
    host_.bgm(7);
    const u32 mode = (progress_.run_mode_flags >> 4) & 3;
    if (mode == 0) {
        enter(OverlayState::GameOverShow);
        if (progress_.continue_stock < 0) progress_.continue_stock = 0;
    } else {
        enter(OverlayState::BGameOver);
    }
}

bool PostGameFlow::on_practice_end() {
    enter(OverlayState::PracticeEntry);
    if (!host_.request_practice_end()) {
        if (error.empty()) error = "Practice end request failed";
        return false;
    }
    return true;
}

void PostGameFlow::on_extra_end() {
    const u32 mode = (progress_.run_mode_flags >> 4) & 3;
    if (mode == 2) {
        enter(OverlayState::BSpellVariant);
    } else if (mode == 0) {
        enter(OverlayState::BShow);
    } else {
        enter(OverlayState::BClear);
    }
    if (!host_.request_extra_results()) error = "Extra results request failed";
}

void PostGameFlow::update() {
    age_++;
    if (clear_sequence_) {
        if (frozen()) return;
        clear_frame_++;
        if (clear_frame_ == 180) {
            if (!host_.request_stage6_fade()) {
                error = "Stage 6 clear fade request failed";
                return;
            }
        }
        if (clear_frame_ > 379) {
            clear_sequence_ = false;
            progress_.scene_flags &= ~0x4000u;
            if (progress_.practice_mode != 0) {
                on_practice_end();
            } else {
                host_.request_destination(0xf);
            }
        }
        return;
    }
}

}
