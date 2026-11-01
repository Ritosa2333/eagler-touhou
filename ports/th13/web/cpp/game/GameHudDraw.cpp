// GameHud 文本 pass：hi-score/score、残机/灵击碎片、灵力、最大得点、Graze。
// 坐标/颜色/字号按原版 0x427910 逐项对齐：
//   hi(620,44) ff808080 / score(620,66) ffffffff 右对齐（0x427910:155-176）
//   碎片(576±7,122) (576,160) scale 0.6（0x427910:178-201）
//   Power(540,216) 大字 + (560,223) 小数 + (574,216) "/N." + (606,223) "00"
//   色 ffff8030（0x427910:203-229）
//   maxPoint(620,238) ff40c0ff 右对齐（0x427910:230-238）
//   Graze(620,260) ff808080 右对齐（0x427910:239-246）
// 颜色语义（对 th13.exe 反汇编 + decomp 已核对）：原版每个字形顶点色 = AsciiInf 的当前文本色
// DAT_004c2160+0x19160（AsciiInf 构造 0x4038A0 默认 0xFFFFFFFF，0x4038F2；
// decomp 004038a0.c:18），经 0x403DF4/0x403DFA 复制到 text descriptor+0x10C，
// 0x404A55 再进 drawer+0x53C，0x467A80 写入四个顶点色 dword。0x427910 对这些
// 文本只写不透明常量（alpha=0xFF），不读任何动画 VM 的 alpha。
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

// 仅 stage-clear 奖励文本使用：原版 0x42797C mov dl,[VM+0x52b] -> 0x427993
// mov [AsciiInf+0x19163],dl 把 banner VM 的 alpha 字节写进文本色，随后
// 0x4279D5 恢复 0xFF（decomp 00427910.c:41-51）。普通 HUD 文本不走此路径。
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
    (void)context;
    // hi-score / score：右对齐，尾位为 continues / hi_score_extra。
    // 0x427D35 mov [edi+0x19160],0xff808080（decomp 00427910.c:142）。
    // 原版此处不读 life_icons[0]（front.anm script29）的 visual.color；该 VM 的
    // alpha 由 script29 t=0 op403 置 0、t=20 op409 才淡入，只属于残机图标自身，
    // 此前拿它做文本 alpha 掩码导致帧 0-15 整片 HUD 文本透明（web 帧 0 实测
    // 32 个 HUD 四边形 alpha=0x00）。
    style.color = 0xff808080;
    style.font = 4;
    style.alignment = 2;
    if (!text(620, 44, score_number(u32(progress.high_score), progress.high_score_extra)))
        return false;
    // 0x427D87 mov [eax+0x19160],0xffffffff；残机/灵击沿用该色（decomp 00427910.c:151）。
    style.color = 0xffffffff;
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
    // 0x427F2E mov [esi+0x19160],0xffff8030（decomp 00427910.c:197）。
    style.color = 0xffff8030;
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
    // 0x42808D mov [eax+0x19160],0xff40c0ff（decomp 00427910.c:230）。
    style.color = 0xff40c0ff;
    if (!text(620, 238,
              grouped_number(u32(progress.max_point / 100 - (progress.max_point / 100) % 10))))
        return false;
    // 0x428111 mov [edx+0x19160],0xff808080（decomp 00427910.c:241）。
    style.color = 0xff808080;
    if (!text(620, 260, grouped_number(u32(progress.graze)))) return false;
    // Boss 计时文本（0x427910:253-274）：'.' 与百分秒锚定数字 VM 位置
    //（ascii script2/3 dump：208/220,39）+16/-7；'.' 字体 4、百分秒 0.6 缩放。
    if (boss_hud.timer_visible()) {
        style.alignment = style.vertical_alignment = 0;
        // 0x4281C8 mov eax,[eax+0x528] -> 0x4281D4 mov [esi+0x19160],eax：
        // 原版把数字 VM 的完整 ARGB（含其自身 alpha）直接写入 AsciiInf 当前色
        //（decomp 00427910.c:258）。不要再叠加任何其它 VM 的 alpha。
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
