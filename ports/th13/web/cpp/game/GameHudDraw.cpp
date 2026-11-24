#include "GameHud.hpp"

#include <algorithm>
#include <cstdio>

namespace th13 {

namespace {
template <typename... T> std::string format(const char* pattern, T... values) {
    char text[256];
    std::snprintf(text, sizeof(text), pattern, values...);
    return std::string(text);
}

u32 alpha(u32 color, u32 source) { return (color & 0xffffff) | (source & 0xff000000); }
}

std::string grouped_number(u32 value) {
    if (value < 1000) return format("%d", signed_bits(value));
    if (value < 1000000) return format("%d,%.3d", value / 1000, value % 1000);
    if (value < 1000000000) return format("%d,%.3d,%.3d", value / 1000000, (value / 1000) % 1000, value % 1000);
    return format("%d,%.3d,%.3d,%.3d", value / 1000000000, (value / 1000000) % 1000, (value / 1000) % 1000,
                  value % 1000);
}

std::string score_number(u32 value, i32 last_digit) {
    if (value < 100) return format("%d", signed_bits(value * 10 + u32(last_digit)));
    if (value < 100000) return format("%d,%.2d%d", value / 100, value % 100, last_digit);
    if (value < 100000000)
        return format("%d,%.3d,%.2d%d", (value / 100000) % 1000, (value / 100) % 1000, value % 100, last_digit);
    return format("%d,%.3d,%.3d,%.2d%d", (value / 100000000) % 1000, (value / 100000) % 1000, (value / 100) % 1000,
                  value % 100, last_digit);
}

void GameHud::update_score() {
    if (score.score != displayed_score) {
        u32 change = u32(wrapping_add(score.score, -displayed_score)) >> 5;
        change = std::min(578910u, std::max(1u, change));
        if (score_increment < i32(change)) score_increment = i32(change);
        const i32 difference = wrapping_add(score.score, -displayed_score);
        if (difference < score_increment) score_increment = difference;
        displayed_score = wrapping_add(displayed_score, score_increment);
        if (score.score <= displayed_score) score_increment = 0;
    }
    if (progress.high_score < displayed_score) {
        progress.run_mode_flags |= 4;
        progress.high_score_extra = progress.continues;
        player.mode_flags |= 4;
        progress.high_score = displayed_score;
    }
}

bool GameHud::draw_trance_countdown(HudDrawServices& services) {
    if (!trance_active(progress)) return true;
    HudTextStyle style;
    style.font = 7;
    style.alignment = 1;
    const i32 gauge = progress.ending_points;
    const bool red = gauge < 30 ? (gauge & 1) != 0 : gauge < 90 && (gauge & 7) != 0;
    style.color = red ? 0xffff0000u : 0xffffffffu;
    const auto text = format("%.2fs", double(gauge) / 60.0);
    const float x = 224.f - 5.f - 30.f * float(text.find('.'));
    if (services.hud_text({{x, 112, 0}, style, text})) return true;
    error = "Trance countdown text service failed";
    return false;
}

bool GameHud::draw(const HudDrawContext& context, HudDrawServices& services) {
    if (auto* banner = animations.registry.find(clear_banner)) {
        HudTextStyle clear_style;
        clear_style.font = 4;
        clear_style.coordinate_space = 1;
        clear_style.alignment = clear_style.vertical_alignment = 0;
        clear_style.shadow = true;
        clear_style.color = alpha(0xffffffffu, banner->visual.color);
        if (!services.hud_text({{320, 200, 0}, clear_style, grouped_number(u32(progress.stage_clear_bonus))})) {
            error = "Stage clear bonus text service failed";
            return false;
        }
    } else {
        clear_banner = 0;
    }
    if (!update_trance()) return false;
    HudTextStyle style;
    const auto text = [&](float x, float y, const std::string& value) {
        if (services.hud_text({{x, y, 0}, style, value})) return true;
        error = "HUD text draw service failed";
        return false;
    };
    (void)context;
    style.color = 0xff808080;
    style.font = 4;
    style.alignment = 2;
    if (!text(620, 44, score_number(u32(progress.high_score), progress.high_score_extra))) return false;
    style.color = 0xffffffff;
    if (!text(620, 66, score_number(u32(displayed_score), progress.continues))) return false;
    style.alignment = style.vertical_alignment = 1;
    style.scale = {.6f, .6f};
    const i32 life_threshold = life_piece_threshold(progress.life_piece_threshold_index);
    const float life_x = 576.f - (life_threshold >= 100 ? 7.f : 0.f);
    if (life_threshold >= 999999) {
        if (!text(life_x, 122, "---/---")) return false;
        if (!text(576, 160, "---/---")) return false;
    } else {
        if (!text(life_x, 122, format("%3d", player.life_pieces))) return false;
        if (!text(life_x + 21, 122, format("/%d", life_threshold))) return false;
        if (!text(576, 160, format("%3d", player.bomb_pieces))) return false;
        if (!text(597, 160, format("/%d", 8))) return false;
    }
    if (!player.power_step) {
        error = "HUD power step is zero";
        return false;
    }
    style.color = 0xffff8030;
    style.scale = {1, 1};
    if (!text(540, 216, format("%d.", player.power / player.power_step))) return false;
    style.scale = {.6f, .6f};
    if (!text(560, 223, format("%.2d", wrapping_mul(player.power % player.power_step, 100) / player.power_step)))
        return false;
    style.scale = {1, 1};
    if (!text(574, 216, format("/%d.", player.max_power / player.power_step))) return false;
    style.scale = {.6f, .6f};
    if (!text(606, 223, "00")) return false;
    style.alignment = 2;
    style.scale = {1, 1};
    style.color = 0xff40c0ff;
    if (!text(620, 238, grouped_number(u32(progress.max_point / 100 - (progress.max_point / 100) % 10)))) return false;
    style.color = 0xff808080;
    if (!text(620, 260, grouped_number(u32(progress.graze)))) return false;
    if (boss_hud.timer_visible()) {
        style.alignment = style.vertical_alignment = 1;
        style.color = boss_hud.timer_digit_color();
        style.font = 4;
        style.scale = {1, 1};
        if (!text(boss_hud.timer_text_x(), boss_hud.timer_text_y(), ".")) return false;
        style.scale = {.6f, .6f};
        if (!text(boss_hud.timer_text_x() + 8.f, boss_hud.timer_text_y() + 6.f,
                  format("%.2d", boss_hud.timer_centiseconds())))
            return false;
        style.scale = {1, 1};
        style.font = 0;
    }
    return true;
}

}
