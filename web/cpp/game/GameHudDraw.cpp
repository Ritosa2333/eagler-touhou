// GameHud 文本 pass：hi-score/score、残机/灵击碎片、灵力、最大得点、Graze。
// 坐标/颜色/字号按原版 0x427910 逐项对齐：
//   hi(620,44) ff808080 / score(620,66) ffffffff 右对齐（0x427910:155-176）
//   碎片(576±7,122) (576,160) scale 0.6（0x427910:178-201）
//   Power(540,216) 大字 + (560,223) 小数 + (574,216) "/N." + (606,223) "00"
//   色 ffff8030（0x427910:203-229）
//   maxPoint(620,238) ff40c0ff 右对齐（0x427910:230-238）
//   Graze(620,260) ff808080 右对齐（0x427910:239-246）
#include "GameHud.hpp"

#include <algorithm>
#include <cstdio>

namespace th13 {

namespace {
template <typename... T>
std::string format(const char* pattern, T... values) {
    char text[256];
    std::snprintf(text, sizeof(text), pattern, values...);
    return std::string(text);
}

u32 alpha(u32 color, u32 source) { return (color & 0xffffff) | (source & 0xff000000); }
}  // namespace

std::string grouped_number(u32 value) {
    if (value < 1000) return format("%d", signed_bits(value));
    if (value < 1000000) return format("%d,%.3d", value / 1000, value % 1000);
    if (value < 1000000000)
        return format("%d,%.3d,%.3d", value / 1000000, (value / 1000) % 1000, value % 1000);
    return format("%d,%.3d,%.3d,%.3d", value / 1000000000, (value / 1000000) % 1000,
                  (value / 1000) % 1000, value % 1000);
}

std::string score_number(u32 value, i32 last_digit) {
    if (value < 100) return format("%d", signed_bits(value * 10 + u32(last_digit)));
    if (value < 100000) return format("%d,%.2d%d", value / 100, value % 100, last_digit);
    if (value < 100000000)
        return format("%d,%.3d,%.2d%d", (value / 100000) % 1000, (value / 100) % 1000,
                      value % 100, last_digit);
    return format("%d,%.3d,%.3d,%.2d%d", (value / 100000000) % 1000, (value / 100000) % 1000,
                  (value / 100) % 1000, value % 100, last_digit);
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
        progress.run_mode_flags |= 4; // 0042a370 writes DAT_004be830, not a HUD-local word.
        progress.high_score_extra = progress.continues;
        player.mode_flags |= 4; // Retain the legacy HUD projection mirror.
        progress.high_score = displayed_score;
    }
}

bool GameHud::draw_trance_countdown(HudDrawServices& services) {
    if (!trance_active(progress)) return true;
    // 405d60 ->405940: screen-space ASCII font7 at(224,112), gauge/60 seconds.
    HudTextStyle style;
    style.font = 7;
    style.alignment = 0;
    const i32 gauge = progress.ending_points;
    const bool red = gauge < 30 ? (gauge & 1) != 0 : gauge < 90 && (gauge & 7) != 0;
    style.color = red ? 0xffff0000u : 0xffffffffu;
    if (services.hud_text({{224,112,0},style,format("%.2fs",double(gauge)/60.0)})) return true;
    error = "Trance countdown text service failed";
    return false;
}

bool GameHud::draw(const HudDrawContext& context, HudDrawServices& services) {
    // 427910 draws the clear bonus while HUD+110 resolves, including the
    // VM's label1 tail. Use that live VM's primary alpha (VM+52b), not the
    // clear flag or the independently timed normal HUD icon alpha.
    if (auto* banner = animations.registry.find(clear_banner)) {
        HudTextStyle clear_style;
        clear_style.font = 4;
        clear_style.alignment = clear_style.vertical_alignment = 0;
        clear_style.shadow = true;
        clear_style.color = alpha(0xffffffffu, banner->visual.color);
        if (!services.hud_text({{320,200,0}, clear_style,
                               grouped_number(u32(progress.stage_clear_bonus))})) {
            error = "Stage clear bonus text service failed";
            return false;
        }
    } else {
        clear_banner = 0;
    }
    // 灵界计值显示（0x42b030）随帧刷新；只消费 SessionState.ending_points
    //（DAT_004be808 灵界计，单一状态，不另建 HUD 私有计量）。
    if (!update_trance()) return false;
    HudTextStyle style;
    const auto text = [&](float x, float y, const std::string& value) {
        if (services.hud_text({{x, y, 0}, style, value})) return true;
        error = "HUD text draw service failed";
        return false;
    };
    const auto* root_vm = animations.registry.find(life_icons[0]);
    if (!root_vm) {
        error = "HUD life icon animation unavailable";
        return false;
    }
    (void)context;
    // hi-score / score：右对齐，尾位为 continues / hi_score_extra。
    style.color = alpha(0xff808080, root_vm->visual.color);
    style.font = 4;
    style.alignment = 2;
    if (!text(620, 44, score_number(u32(progress.high_score), progress.high_score_extra)))
        return false;
    style.color = alpha(0xffffffff, root_vm->visual.color);
    if (!text(620, 66, score_number(u32(displayed_score), progress.continues))) return false;
    // 残机/灵击碎片：0x427910:156-194。残机分母 = 0x4bb994[be7fc]
    //（首命 8，0x43d330 每命递增）；灵击分母固定 8（0x427f02 push $8）。
    // 哨兵 9999999 → "---/---"（0x427910:170/187），且残机 x 左移 7
    //（0x427910:156-162）。此前把 max_point≥999999 当哨兵是误接线。
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
    // 灵力：x.xx /4.00 两段字号（整数段大字号、小数段小字号），y=216/223。
    if (!player.power_step) {
        error = "HUD power step is zero";
        return false;
    }
    style.color = alpha(0xffff8030, root_vm->visual.color);
    style.scale = {1, 1};
    if (!text(540, 216, format("%d.", player.power / player.power_step))) return false;
    style.scale = {.6f, .6f};
    if (!text(560, 223,
              format("%.2d", wrapping_mul(player.power % player.power_step, 100) / player.power_step)))
        return false;
    style.scale = {1, 1};
    if (!text(574, 216, format("/%d.", player.max_power / player.power_step))) return false;
    style.scale = {.6f, .6f};
    if (!text(606, 223, "00")) return false;
    // 最大得点（0x4be7dc/100 的十位对齐显示）与 Graze。
    // 0x427910:227-228 在功率段后重设 0x19184=2（右对齐）；此前沿用碎片段的
    // 居中 1，Normal 开局 10,000 以 620 为中心溢出屏右缘（battle-f300 实证）。
    style.alignment = 2;
    style.scale = {1, 1};
    style.color = alpha(0xff40c0ff, root_vm->visual.color);
    if (!text(620, 238,
              grouped_number(u32(progress.max_point / 100 - (progress.max_point / 100) % 10))))
        return false;
    style.color = alpha(0xff808080, root_vm->visual.color);
    if (!text(620, 260, grouped_number(u32(progress.graze)))) return false;
    // Boss 计时文本（0x427910:253-274）：'.' 与百分秒锚定数字 VM 位置
    //（ascii script2/3 dump：208/220,39）+16/-7；'.' 字体 4、百分秒 0.6 缩放。
    // 原版 0x42814f/0x428155 写 +0x19184/+0x19188=1（左/上对齐），
    // 颜色整 dword 取数字 VM +0x528（含 alpha），不是 HUD 根 VM 的 alpha。
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

}  // namespace th13
