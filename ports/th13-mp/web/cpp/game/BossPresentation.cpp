#include "BossPresentation.hpp"

#include <algorithm>
#include <cmath>

namespace th13 {

namespace {
constexpr u32 kFlagVisible = 0x2;
constexpr u32 kFlagSelected = 0x1;
constexpr float kPi = 3.1415927410125732421875f;
constexpr float kTwoPi = 6.283185482025146484375f;
}

bool BossPresentation::initialize() {
    if (digit_tens_ && digit_ones_) return true;
    digit_tens_ = animations_.create(banks_.ascii, kTimerTensScript, -1, 0);
    digit_ones_ = animations_.create(banks_.ascii, kTimerOnesScript, -1, 0);
    if (!digit_tens_ || !digit_ones_) {
        fail("Boss timer digit animation unavailable");
        return false;
    }
    for (u32 handle : {digit_tens_, digit_ones_}) {
        if (auto* vm = animations_.registry.find(handle)) {
            vm->visual.flags &= ~kFlagVisible;
            vm->visual.render_flags &= ~0x40000u;
        }
    }
    return true;
}

void BossPresentation::release() {
    retire_ring();
    if (name_handle_) animations_.retire(name_handle_);
    for (auto& handle : remaining_star_handles_)
        if (handle) {
            animations_.retire(handle);
            handle = 0;
        }
    for (auto& handle : star_handles_) {
        if (handle && !animations_.retire(handle)) fail("Boss star retire failed");
        handle = 0;
    }
    stars_created_ = false;
    for (u32 handle : {digit_tens_, digit_ones_, marker_handle_}) {
        if (handle && !animations_.retire(handle)) fail("Boss VM retire failed");
    }
    digit_tens_ = digit_ones_ = marker_handle_ = 0;
    gate_ = false;
    displayed_ratio_ = target_ratio_ = 0;
    timer_displayed_ = -1;
    warn_state_ = 0;
    escalate_state_ = 0;
}

bool BossPresentation::create_ring() {
    if (ring_created_) return true;
    ring_arc_ = animations_.create(banks_.front, kRingArcScript, -1, 0);
    if (!ring_arc_) {
        fail("Boss ring arc animation unavailable");
        return false;
    }
    for (i32 i = 0; i < 2; i++) {
        ring_overlays_[u32(i)] = animations_.create(banks_.front, kRingOverlayScripts[i], -1, 0);
        if (!ring_overlays_[u32(i)]) {
            fail("Boss ring overlay animation unavailable");
            return false;
        }
    }
    for (u32 handle : {ring_arc_, ring_overlays_[0], ring_overlays_[1]})
        if (auto* vm = animations_.registry.find(handle)) vm->visual.render_flags &= ~0xc000u;
    ring_created_ = true;
    return true;
}

void BossPresentation::retire_ring() {
    for (u32 handle : {ring_arc_, ring_overlays_[0], ring_overlays_[1]}) {
        if (handle && !animations_.retire(handle)) fail("Boss ring retire failed");
    }
    ring_arc_ = ring_overlays_[0] = ring_overlays_[1] = 0;
    ring_created_ = false;
}

bool BossPresentation::create_stars() {
    if (stars_created_) return true;
    for (auto& handle : star_handles_) {
        handle = animations_.create(banks_.front, kRingStarScript, -1, 0);
        if (!handle) {
            fail("Boss star animation unavailable");
            return false;
        }
        if (auto* vm = animations_.registry.find(handle)) vm->visual.render_flags &= ~0xc000u;
    }
    stars_created_ = true;
    return true;
}

bool BossPresentation::ensure_marker() {
    if (marker_handle_) return true;
    marker_handle_ = animations_.create(banks_.front, 111, -1, 0);
    if (!marker_handle_) {
        fail("Boss marker animation unavailable");
        return false;
    }

    return true;
}

void BossPresentation::spell_started() {
    for (u32 handle : {digit_tens_, digit_ones_})
        if (!animations_.interrupt(handle, 2, true)) fail("Boss timer spell interrupt failed");
}

void BossPresentation::spell_ended() {
    for (u32 handle : {digit_tens_, digit_ones_})
        if (!animations_.interrupt(handle, 3, true)) fail("Boss timer spell-end interrupt failed");
}

float BossPresentation::timer_text_x() const noexcept {
    const auto* vm = animations_.registry.find(digit_tens_);
    return (vm ? vm->variables.position.x : 208.f) + 16.f;
}

float BossPresentation::timer_text_y() const noexcept {
    const auto* vm = animations_.registry.find(digit_tens_);
    return (vm ? vm->variables.position.y : 39.f) - 7.f;
}

void BossPresentation::interrupt_digits(i32 label) {
    for (u32 handle : {digit_tens_, digit_ones_}) {
        if (auto* vm = animations_.registry.find(handle)) vm->pending_interrupt = label;
    }
}

u32 BossPresentation::timer_digit_color() const {
    auto* vm = const_cast<AnmManager&>(animations_).registry.find(digit_tens_);
    return vm ? vm->visual.color : 0xffffffffu;
}

bool BossPresentation::marker_visible() const noexcept {
    const auto* vm = animations_.registry.find(marker_handle_);
    return vm && (vm->visual.flags & 3u) == 3u;
}

bool BossPresentation::update(const BossFrameInput& input, const std::function<bool(i32)>& sound) {
    const StageEnemy* boss = input.boss;
    if (!boss && name_handle_ && !animations_.retire(name_handle_)) {
        fail("Boss name retirement failed");
        return false;
    }
    const bool gate =
        input.boss_slot_present && !input.boss_leaving && !input.dialog_active && input.timer_seconds >= 0;
    if (gate != gate_) {
        gate_ = gate;
        for (u32 handle : {digit_tens_, digit_ones_}) {
            if (auto* vm = animations_.registry.find(handle)) {
                if (gate)
                    vm->visual.flags |= kFlagVisible | kFlagSelected;
                else
                    vm->visual.flags &= ~kFlagVisible;
            }
        }
    }
    for (size_t i = 0; i < remaining_star_handles_.size(); ++i) {
        auto& handle = remaining_star_handles_[i];
        if (i < size_t(std::max(0, input.remaining_stars))) {
            if (!handle) handle = animations_.create(banks_.front, 68 + i32(i), -1, 0);
            if (!handle) {
                fail("Boss remaining-star animation unavailable");
                return false;
            }
        } else if (handle) {
            if (!animations_.interrupt(handle, 1, false)) {
                fail("Boss remaining-star exit failed");
                return false;
            }
            handle = 0;
        }
    }
    if (gate) {
        timer_seconds_ = input.timer_seconds;
        timer_frames_ = input.timer_frames;
        timer_centiseconds_ = input.timer_centiseconds;
        const i32 seconds = input.timer_seconds;
        if (seconds != timer_displayed_) {
            if (seconds < timer_displayed_) {
                if (seconds < 10) {
                    warn_state_ = seconds < 5 ? 2 : 1;
                    interrupt_digits(seconds < 5 ? 9 : 8);
                    if (sound && !sound(seconds < 5 ? 12 : 11)) fail("Boss timer sound service failed");
                }
            } else {
                interrupt_digits(7);
                warn_state_ = 0;
            }
            if (auto* vm = animations_.registry.find(digit_tens_))
                if (!vm->select_sprite(kTimerDigitSpriteBase + seconds / 10))
                    fail("Boss timer tens sprite unavailable");
            if (auto* vm = animations_.registry.find(digit_ones_))
                if (!vm->select_sprite(kTimerDigitSpriteBase + seconds % 10))
                    fail("Boss timer ones sprite unavailable");
            timer_displayed_ = seconds;
        }

        const u32 move = (move_state_ >> 9) & 3;
        if (move == 0) {
            if (input.player_y < 128.f) {
                move_state_ = (move_state_ & ~0x400u) | 0x200u;
                interrupt_digits(5);
            }
        } else if (move == 1) {
            if (input.player_y > 160.f) {
                interrupt_digits(4);
                move_state_ &= ~0x600u;
            }
        }
    }
    if (!update_marker(input)) return false;
    const bool ring_gate = boss && !input.boss_leaving && !input.dialog_active && boss->life < 100000.f &&
                           !(boss->flags & 0x31u) && boss->invulnerable < 1;
    if (!ring_gate) {
        retire_ring();
        for (auto& handle : star_handles_)
            if (handle) {
                animations_.retire(handle);
                handle = 0;
            }
        stars_created_ = false;
        if (!boss) {
            displayed_ratio_ = target_ratio_ = 0;
        }
        return error.empty();
    }
    if (input.name_script >= 0 && !name_handle_) {
        name_handle_ = animations_.create(banks_.front, input.name_script);
        if (!name_handle_) {
            fail("Boss name animation unavailable");
            return false;
        }
    }
    if (input.life_initial > 0 && input.boss_life >= 0) {
        target_ratio_ = float(input.boss_life) / float(input.life_initial);
        if (target_ratio_ > 1.f) target_ratio_ = 1.f;
    }
    if (displayed_ratio_ > target_ratio_)
        displayed_ratio_ = target_ratio_;
    else
        displayed_ratio_ = float(displayed_ratio_ + 0.025f) > target_ratio_ ? target_ratio_ : displayed_ratio_ + 0.025f;

    const bool life_ratio_ready = input.life_initial > 0;
    if (life_ratio_ready) {
        if (!create_ring()) return false;
        if (auto* vm = animations_.registry.find(ring_arc_)) {
            vm->visual.flags |= 0x4u;
            vm->variables.rotation.x = -displayed_ratio_ * kTwoPi;
            vm->visual.translation = {boss->position[0], boss->position[1], boss->position[2]};
        }
        for (u32 handle : ring_overlays_)
            if (auto* vm = animations_.registry.find(handle))
                vm->visual.translation = {boss->position[0], boss->position[1], boss->position[2]};
    } else if (ring_created_) {
        retire_ring();
    }

    if (life_ratio_ready) {
        if (!stars_created_ && !create_stars()) return false;
        for (i32 i = 0; i < 4; i++)
            write_star(star_handles_[u32(i)], input, i);
    } else if (stars_created_) {
        for (auto& handle : star_handles_) {
            if (handle && !animations_.retire(handle)) fail("Boss star retire failed");
            handle = 0;
        }
        stars_created_ = false;
    }

    if (life_ratio_ready) {
        const float dx = boss->position[0] - input.player_x;
        const float dy = boss->position[1] - input.player_y;
        const float distance_squared = dx * dx + dy * dy;
        const i32 label = !proximity_hidden_ && distance_squared < 6400.f   ? 3
                          : proximity_hidden_ && distance_squared >= 9216.f ? 2
                                                                            : 0;
        if (label) {
            for (u32 handle : {ring_arc_, ring_overlays_[0], ring_overlays_[1], star_handles_[0], star_handles_[1],
                               star_handles_[2], star_handles_[3]})
                if (!animations_.interrupt(handle, label, false)) {
                    fail("Boss proximity interrupt failed");
                    return false;
                }
            proximity_hidden_ = label == 3;
        }
    }

    return true;
}

bool BossPresentation::update_marker(const BossFrameInput& input) {
    const auto* boss = input.boss;
    if (!boss || (boss->flags & 0x21u)) {
        if (auto* vm = animations_.registry.find(marker_handle_)) vm->visual.flags &= ~kFlagVisible;
        return true;
    }
    if (!ensure_marker()) return false;
    if (auto* vm = animations_.registry.find(marker_handle_)) {
        vm->visual.flags |= kFlagVisible | kFlagSelected;
        vm->visual.translation = {boss->position[0] + 32.f + 192.f, 480.f, 0.f};
        const float dx = std::fabs(boss->position[0] - input.player_x);
        i32 glyph = 0xff;
        if (dx < 64.f) glyph = '@' + i32(std::nearbyint(dx * 191.f * 0.015625f));
        if (boss->position[0] < -192.f || boss->position[0] > 192.f) glyph = 0;
        marker_glyph_ = glyph;
        vm->visual.color = (vm->visual.color & 0xffffffu) | (u32(glyph) << 24);
        const i32 life_left = boss->phase_life_remaining;
        const i32 t1 = input.trance_mode ? 2000 : 700;
        const i32 t2 = input.trance_mode ? 1000 : 400;
        const i32 t3 = input.trance_mode ? 400 : 200;
        if (escalate_state_ == 0 && life_left < t1) {
            vm->pending_interrupt = 7;
            escalate_state_ = 1;
        } else if (escalate_state_ == 1 && life_left < t2) {
            vm->pending_interrupt = 8;
            escalate_state_ = 2;
        } else if (escalate_state_ == 2 && life_left < t3) {
            vm->pending_interrupt = 9;
            escalate_state_ = 3;
        } else if (escalate_state_ == 3 && life_left > t3) {
            vm->pending_interrupt = 10;
            escalate_state_ = 0;
        }
    }
    return true;
}

void BossPresentation::write_star(u32 handle, const BossFrameInput& input, i32 index) {
    auto* vm = animations_.registry.find(handle);
    if (!vm) return;
    const float angle = input.markers[u32(index)].ratio;
    if (angle == 0.f || angle == displayed_ratio_ || displayed_ratio_ < angle) {
        vm->visual.flags &= ~kFlagVisible;
        return;
    }
    vm->visual.flags |= kFlagVisible | kFlagSelected;
    const float t = normalize_angle(float(-double(kPi) - double(angle) * double(kTwoPi)));
    vm->variables.rotation.z = t;
    const StageEnemy* boss = input.boss;
    const float sine = std::sin(t);
    const float cosine = std::cos(t);
    vm->visual.translation = {float(boss->position[0] - kStarOrbitRadius * sine),
                              float(boss->position[1] + kStarOrbitRadius * cosine), boss->position[2]};
}

}
