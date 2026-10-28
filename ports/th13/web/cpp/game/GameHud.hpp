// 游戏 HUD（th15 GameHud 裁剪：残机/灵击/分数/灵力保留；boss 血环、符卡计时、
// pointdevice、chapter 通知延后；trance 槽仅留成员占位，docs/plans/ui-base.md §五）。
// HUD 只持动画句柄；文本/数字绘制是独立表现 pass，不持设备对象。
#pragma once

#include "AnmManager.hpp"
#include "BossPresentation.hpp"
#include "SessionState.hpp"
#include "SpellPresentation.hpp"
#include "HudFrameData.hpp"
#include "HudDrawData.hpp"

namespace th13 {

struct HudEntryContext {
    i32 scene_destination = 0;
    // Supervisor+590 (DAT_004dcc30):0 for destination0xc continuation,
    // 1 for fresh entries and retries (0043c1c0).
    bool new_run = true;
};

class GameHud {
    AnmManager& animations;
    SessionState& progress;
    HudPlayerState& player;
    HudScoreState& score;
    i32 front, ascii;

    bool check(bool);
    u32 create(i32 bank, i32 script, u32 ordering = 0);
    u32 widget(i32 script);
    bool direct_interrupt(u32, i32);

public:
    std::array<u32, 8> life_icons{}, bomb_icons{};
    u32 root = 0, difficulty_notice = 0, difficulty_label = 0;
    u32 flags = 0;
    Timer intro_age{0, 0, 0, 0, 0};
    Timer frame_age{0, 0, 0, 0, 0};
    // HUD+184..194 is independent from the loader-reset frame clock.
    Timer clear_age{0, 0, 0, 0, 0};
    u32 clear_banner = 0;
    i32 displayed_score = 0, score_increment = 0;
    std::string error;
    // th13 灵界计（agauge）：0x425170 FUN_0046f820(script 0xab=171) 建
    // 根 VM 存 stage ctx+0x11c，紧接着调 0x42b030 刷新；子 script172-175
    // 由 171 ins_500 spawn（front.anm 实 dump：ins_400(-196,448)）。
    // 值显示 0x42b030：172 标签 2(g>=200)/3、173 scale.x+uv_scale.x =
    // floor(g/200)/3、175 同式取 g%200/200 且 offset.x=frac*16；174 不写。
    // interrupt 标签：172: 2(亮)/3(灭)/4(满亮)/5(暗)、173-175: 4(亮)/5(暗)。
    // 显示层只持根句柄；172-175 子 VM 经 registry.find_child_script 定位，
    // 值/激活状态全部来自 SessionState（DAT_004be808 / be830 bit0x100），
    // 不在此另存计量。
    u32 gauge_main = 0;
    i32 trance_skull_label = 0;
    // Boss/符卡面（0x4265a0/0x413270 族；bank 常量 = Application kBanks/
    // kFrontBank：ascii=2、effect=8、stage=4）。宿主经 HudFrameContext.boss
    // 供给每帧视图；文本面并入 draw()。
    BossPresentation boss_hud;
    SpellPresentation spell_hud;

    GameHud(AnmManager& a, SessionState& p, HudPlayerState& s, HudScoreState& v, i32 front_bank,
            i32 ascii_bank)
        : animations(a), progress(p), player(s), score(v), front(front_bank), ascii(ascii_bank),
          boss_hud(a, {front_bank, ascii_bank, 8}), spell_hud(a, {4}) {
        frame_age.set(0);
    }

    bool initialize(const HudEntryContext&);
    bool life(i32 lives, i32 pieces);
    bool bombs(i32 bombs, i32 pieces);
    bool stage_logo(i32 scene_destination);
    bool update(const HudFrameContext&, HudFrameServices&);
    void update_score();
    bool draw(const HudDrawContext&, HudDrawServices&);
    bool draw_trance_countdown(HudDrawServices&);
    bool reset_for_retry();
    void prepare_stage_assets() noexcept {
        frame_age.set(0);
        displayed_score = score.score;
        boss_hud.prepare_stage_assets();
    }
    bool update_trance();  // 0x42b030 值→172/173/175 显示映射（无状态副作用）
    bool begin_stage_clear();  // 0042abf0: front115 and HUD+180 bit100
    bool finish_stage_clear(); // 0042ad40: label1, clear flag and clock
    void tick_stage_clear() noexcept {
        if (flags & 0x100u) clear_age.tick(&progress.rate);
    }
};

}  // namespace th13
