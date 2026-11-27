// th16 Gui——HUD 部分（原版 DAT_004a6dcc 的 VM 字段；对话部分见 Gui.cpp）。
//
//   0x426d70  关卡首帧建 HUD VM、刷新残机 / 炸弹 / 季节量表显示          → Gui::init_hud
//   0x42c920  残机 / 炸弹格：UI 链表 VM + 指针出参                       → AnmManager::spawn_ui(out_vm)
//   0x42c280  残机显示刷新                                               → Gui::refresh_lives_display
//   0x42c390  炸弹显示刷新                                               → Gui::refresh_bombs_display
//   0x42c600  季节量表显示刷新（0x43df20 进度）                          → Gui::refresh_season_gauge
//   0x42bcf0  弹出通知（符卡奖励 / 取得失败 / 满火力 / extend）          → Gui::show_notice
//   0x427cf0  每帧 HUD 段：文字底存活、面板让位、Boss 血条、符卡星、Boss 方位标、炸弹可用标
//   0x42c8f0 / 0x42c8c0 / 0x42c890 / 0x418440 / 0x418470  计时数字移位 → Gui::move_timer_digit
//   0x42c580 / 0x42c5c0  HUD 根 VM 隐藏 / 显示（+0x12c 在 th16 无创建路径）→ Gui::hide_hud / show_hud
//   0x428e70  绘制链 0x30：HUD 文字入队 AsciiManager（0x426840 解码符卡用时）  → Gui::draw_hud_text
//
// 原版 Gui+0x1ac bit11 / bit12（符卡奖励滚动显示，0x42c4f0 收尾）在 th16 没有任何置位方，属前作遗留的死代码，不移植。
#include "Gui.hpp"

#include "AnmManager.hpp"
#include "Ascii.hpp"
#include "Bomb.hpp"
#include "Enemy.hpp"
#include "GameGlobals.hpp"
#include "Motion.hpp"
#include "Player.hpp"
#include "Spellcard.hpp"

namespace th16 {

namespace {

constexpr float kTwoPi = 6.2831855f;
constexpr float kPi = 3.1415927f;
constexpr float kBossBarMarkerRadius = 112.0f;
constexpr float kBossBarApproachStep = 0.025f;
// 自机与 Boss 的距离平方：小于 80² 时血条整套淡出，大于等于 96² 时恢复（滞回）
constexpr float kBossBarFadeDistanceSq = 6400.0f;
constexpr float kBossBarRestoreDistanceSq = 9216.0f;

}  // namespace

u32 Gui::spawn_world(AnmFile& file, i32 script) {
    return animations->spawn(file, script, -1);
}

u32 Gui::spawn_hidden_world(AnmFile& file, i32 script, AnmVm** out_vm) {
    // 0x426d70 计时数字：世界链表 VM、creation_flags 0；建好后整树隐藏并清 flags_hi 的 0xc0000
    const u32 id = animations->spawn(file, script, -1, out_vm);
    AnmVm& vm = **out_vm;
    AnmManager::hide_tree(vm);
    vm.flags_hi &= ~0xc0000u;
    return id;
}

AnmVm* Gui::panel_child(i32 script) {
    AnmVm* panel = find_or_clear(panel_vm);
    return panel != nullptr ? AnmManager::find_child(*panel, script, 0) : nullptr;
}

// 0x426d70
void Gui::init_hud(AnmFile* ascii) {
    if (on_hud_init) on_hud_init();
    if (animations == nullptr || front == nullptr || globals == nullptr) return;
    if (hud_frame_vm == 0) hud_frame_vm = animations->spawn_ui(*front, kHudFrame);
    if (life_vms[0] == nullptr) {
        for (int i = 0; i < kResourceSlots; ++i) life_vm_ids[i] = animations->spawn_ui(*front, kLifeSlot0 + i, &life_vms[i]);
        for (int i = 0; i < kResourceSlots; ++i) bomb_vm_ids[i] = animations->spawn_ui(*front, kBombSlot0 + i, &bomb_vms[i]);
        if (ascii != nullptr) {
            for (int i = 0; i < 2; ++i) boss_timer_vm_ids[i] = spawn_hidden_world(*ascii, 2 + i, &boss_timer_vms[i]);
        }
    }
    refresh_lives_display(globals->lives, globals->life_fragments);
    refresh_bombs_display(globals->bombs, globals->bomb_fragments);

    // 关卡标题（logo.anm 脚本 1）：符卡练习与演示回放不出；id 不保存，由脚本自行结束
    const bool demo = (globals->flags & 0x40u) != 0;
    if (globals->scene_request != 8 && !demo && (globals->flags & GameGlobals::kModeMask) != 0x20 && logo != nullptr) {
        spawn_world(*logo, 1);
    }
    if (demo) spawn_world(*front, kDemoBanner);
    if (boss_indicator_vm == 0) boss_indicator_vm = spawn_world(*front, kBossIndicator);
    // 第 1 面、非回放、未续关：操作提示；魔理沙的提示位置更低
    if (globals->stage == 1 && !replay_playback && globals->continues == 0) {
        AnmVm* hint = nullptr;
        animations->spawn(*front, kStageOneHint, -1, &hint);
        hint->entity_offset = Vec3{0.0f, globals->shot == 3 ? 148.0f : 128.0f, 0.0f};
    }
    if (globals->new_game) {
        difficulty_label_vm = spawn_world(*front, kDifficultyLabel0 + globals->difficulty);
        animations->interrupt(difficulty_label_vm, 3);
    }
    difficulty_badge_vm = spawn_world(*front, kDifficultyBadge0 + globals->difficulty);
    // 名牌与其子 VM 再挂一次中断 3（原版此处重复，保留）
    if (AnmVm* label = animations->find(difficulty_label_vm)) {
        animations->request_interrupt(*label, 3);
        for (AnmListNode* n = label->children.next; n != nullptr; n = n->next) {
            if (n->owner != nullptr) animations->request_interrupt(*n->owner, 3);
        }
    }
    spell_star_count = 0;
    boss_bars[0].created = 0;
    boss_bars[1].created = 0;
    if (globals->new_game) {
        bomb_ready_shown = 0;
        panel_vm = spawn_world(*front, kHudPanel);
        if (AnmVm* icon = panel_child(kSeasonIcon)) {
            if (AnmFile* file = animations->file(icon->file_slot)) anm_set_sprite(*file, *icon, globals->season + 0x52);
        }
    }
    refresh_season_gauge();
    if (vm_110 != 0) {
        animations->interrupt(vm_110, 1);
        vm_110 = 0;
    }
}

// 0x42c280
void Gui::refresh_lives_display(i32 lives, i32 fragments) {
    if (life_vms[0] == nullptr || animations == nullptr || globals == nullptr) return;
    i32 lit = 0;
    if (lives > 0) {
        // 原版按指针自 +0x54 连续走 lives 格、不设上限：练习 9 残机（0x42cb60 DAT_004a5bf8 == 0）时第 9 格落在 +0x74 =
        // bomb_vms[0]（随后 0x42c390 立即改写）。这里显式按同一内存次序写，不越过数组
        for (; lit < lives && lit < kResourceSlots; ++lit) animations->request_interrupt(*life_vms[lit], 2);
        for (i32 over = kResourceSlots; over < lives && over - kResourceSlots < kResourceSlots; ++over) {
            if (AnmVm* vm = bomb_vms[over - kResourceSlots]) animations->request_interrupt(*vm, 2);
        }
        if (lives > 7) return;
    }
    // 第 lives 格按碎片数分段点亮：中断 7 + 碎片×5 / 本级 extend 所需碎片
    const u32 needed = globals->extend_threshold();
    const i32 fragment_level = i32(i16(u32(fragments * 5) / needed));
    animations->request_interrupt(*life_vms[lit], fragment_level + 7);
    for (++lit; lit < kResourceSlots; ++lit) animations->request_interrupt(*life_vms[lit], 3);
}

// 0x42c390
void Gui::refresh_bombs_display(i32 bombs, i32 fragments) {
    if (bomb_vms[0] == nullptr || animations == nullptr) return;
    i32 lit = 0;
    if (bombs > 0) {
        for (; lit < bombs; ++lit) animations->request_interrupt(*bomb_vms[lit], 2);
        if (bombs > 7) return;
    }
    animations->request_interrupt(*bomb_vms[lit], i32(i16(fragments)) + 7);
    for (++lit; lit < kResourceSlots; ++lit) animations->request_interrupt(*bomb_vms[lit], 3);
}

// 0x42c600
void Gui::refresh_season_gauge() {
    if (animations == nullptr || globals == nullptr) return;
    AnmVm* bar = panel_child(kGaugeBar);
    AnmVm* level_vm = panel_child(kGaugeLevel);
    if (bar == nullptr || level_vm == nullptr) return;   // 原版不判空：面板只在新开局建，此后一直存在
    const i32 level = globals->season_option_count();
    if (level == 0) {
        bar->sprite_size.x = globals->season_gauge_fraction() * 100.0f;
        bar->flags_lo |= 8u;
        AnmManager::hide_tree(*level_vm);
        if (gauge_level_shown == 1) animations->interrupt_now(*bar, 3);
        gauge_level_shown = 0;
        return;
    }
    bar->sprite_size.x = globals->season_gauge < globals->season_gauge_max ? globals->season_gauge_fraction() * 100.0f : 100.0f;
    bar->flags_lo |= 8u;
    AnmManager::show_tree(*level_vm);
    animations->request_interrupt(*level_vm, level + 7);
    if (gauge_level_shown == 0) animations->interrupt_now(*bar, 2);
    gauge_level_shown = 1;
}

// 0x42bcf0
void Gui::show_notice(Notice kind, i32 value) {
    if (animations == nullptr || front == nullptr) return;
    AnmFile* ascii = animations->file(AnmManager::kAsciiSlot);
    switch (kind) {
    case Notice::SpellBonus: {
        kill_vm(notice_vm);
        notice_vm = spawn_world(*front, kNoticeSpellBonus);
        // 8 位数字（ascii.anm 4..0xb，精灵 0xef + 数字），前导零整树隐藏
        i32 remaining = value;
        i32 divisor = 10000000;
        bool significant = false;
        for (int i = 0; i < kNoticeDigits; ++i) {
            kill_vm(notice_digit_vms[i]);
            if (ascii == nullptr) continue;
            notice_digit_vms[i] = spawn_world(*ascii, 4 + i);
            const i32 digit = remaining / divisor;
            remaining %= divisor;
            divisor /= 10;
            if (digit != 0) significant = true;
            if (AnmVm* vm = animations->find(notice_digit_vms[i])) {
                if (AnmFile* file = animations->file(vm->file_slot)) anm_set_sprite(*file, *vm, digit + 0xef);
                if (significant) AnmManager::show_tree(*vm);
                else AnmManager::hide_tree(*vm);
            }
        }
        // 百万位 / 千位逗号（精灵 0xfd）
        for (int i = 0; i < 2; ++i) {
            kill_vm(notice_comma_vms[i]);
            if (ascii == nullptr || value < (i == 0 ? 1000000 : 1000)) continue;
            notice_comma_vms[i] = spawn_world(*ascii, 0xc + i);
            if (AnmVm* vm = animations->find(notice_comma_vms[i])) {
                if (AnmFile* file = animations->file(vm->file_slot)) anm_set_sprite(*file, *vm, 0xfd);
            }
        }
        notice_label_active = 1;
        notice_label_vm = spawn_world(*front, kNoticeLabel);
        return;
    }
    case Notice::SpellFailed:
        kill_vm(notice_vm);
        notice_vm = spawn_world(*front, kNoticeSpellFailed);
        notice_label_active = 1;
        notice_label_vm = spawn_world(*front, kNoticeLabel);
        return;
    case Notice::FullPower:
    case Notice::Kind3:
    case Notice::Extend:
        kill_vm(notice_small_vm);
        notice_small_vm = spawn_world(*front, kNoticeFullPower + (i32(kind) - i32(Notice::FullPower)));
        return;
    case Notice::Kind6:
        kill_vm(notice_vm);
        notice_vm = spawn_world(*front, kNoticeKind6);
        return;
    case Notice::None:
        return;
    }
}

void Gui::celebrate_extend() {
    if (globals != nullptr) refresh_lives_display(globals->lives, globals->life_fragments);
    play_sfx(0x11);
    show_notice(Notice::Extend, 0);
}

void Gui::dismiss_notice() {
    if (animations != nullptr) animations->interrupt(notice_vm, 1);
}

void Gui::set_boss_bar_marker(i32 slot, i32 index, float ratio, i32 flag) {
    if (slot < 0 || slot >= kBossBarSlots || index < 0 || index >= kBossBarMarkers) return;
    boss_bars[slot].markers[index] = ratio;
    boss_bars[slot].marker_flags[index] = flag;
}

void Gui::set_boss_timer_style(TimerStyle style) {
    if (animations == nullptr) return;
    for (AnmVm* vm : boss_timer_vms) {
        if (vm != nullptr) animations->interrupt_now(*vm, i32(style));
    }
}

// 0x42c8f0（5 让位）/ 0x42c8c0（4 归位）只挂中断；0x418440 / 0x418470 + 0x42c890 立即执行（符卡中 2、否则 3，再 4）
void Gui::move_timer_digit(AnmVm* vm, TimerDigitMove move) {
    if (vm == nullptr || animations == nullptr) return;
    switch (move) {
    case TimerDigitMove::Aside: animations->request_interrupt(*vm, 5); return;
    case TimerDigitMove::Back: animations->request_interrupt(*vm, 4); return;
    case TimerDigitMove::Reset: {
        const bool spell_active = spellcard_flags != nullptr && (*spellcard_flags & Spellcard::kActive) != 0;
        animations->interrupt_now(*vm, spell_active ? 2 : 3);
        animations->interrupt_now(*vm, 4);
        return;
    }
    }
}

// 0x42c580 / 0x42c5c0：目标是 +0x12c 的 HUD 根 VM，th16 从不创建它
void Gui::hide_hud() {}
void Gui::show_hud() {}

// 0x427cf0 段 (c)
void Gui::update_notice_label() {
    if (notice_label_active == 0) return;
    if (animations->find(notice_label_vm) == nullptr) {
        notice_label_vm = 0;
        notice_label_active = 0;
    }
}

// 0x427cf0 段 (d)：自机进到面板下方区域（y > 400 且 x < -64）时面板中断 5 让位，离开后中断 4 归位
void Gui::slide_hud_for_player() {
    if (player == nullptr) return;
    const bool near_panel = player->pos.y > 400.0f && player->pos.x < -64.0f;
    if ((flags_1ac & 1u) == 0) {
        if (near_panel) {
            animations->interrupt(panel_vm, 5);
            flags_1ac |= 1u;
        }
    } else if (player->pos.y < 384.0f || player->pos.x > -64.0f) {
        animations->interrupt(panel_vm, 4);
        flags_1ac &= ~1u;
    }
}

void Gui::retire_boss_bar(BossBar& bar) {
    if (bar.created == 0) return;
    for (u32& id : bar.vm_ids) {
        animations->kill(id);
        id = 0;
    }
    bar.created = 0;
}

// 0x427cf0 段 (f)
void Gui::update_boss_bars() {
    if (enemies == nullptr || enemies->hud_hidden) return;
    for (int slot = 0; slot < kBossBarSlots; ++slot) {
        BossBar& bar = boss_bars[slot];
        Enemy* boss = enemies->boss_slot(slot);
        if (boss == nullptr) {
            bar.shown = 0.0f;
            for (float& marker : bar.markers) marker = 0.0f;
            retire_boss_bar(bar);
            if (slot == 0) kill_vm(boss_name_vm);
            continue;
        }
        const bool visible = boss->life < 100000 && (boss->flags & 0x31u) == 0 && boss->invuln.current < 1 && dialogue == nullptr;
        if (!visible) {
            retire_boss_bar(bar);
            continue;
        }
        bar.life = boss->life;
        const float target = float(boss->life) / float(boss->life_max);
        bar.target = target;
        if (bar.shown < target) bar.shown = bar.shown + kBossBarApproachStep;
        if (target < bar.shown) bar.shown = target;
        if (bar.created == 0) {
            static constexpr i32 kScripts[kBossBarVms] = {kBossRing, kBossRingBase, kBossRingFace,
                                                          kBossRingMarker, kBossRingMarker, kBossRingMarker, kBossRingMarker};
            for (int i = 0; i < kBossBarVms; ++i) bar.vm_ids[i] = spawn_world(*front, kScripts[i]);
            bar.created = 1;
        }
        ensure_boss_name();
        // 环 / 底 / 面跟随 Boss（场地坐标 ×2 = 渲染目标像素），环按显示比例转动
        const Vec3 center{boss->pos.x * 2.0f, boss->pos.y * 2.0f, boss->pos.z};
        if (AnmVm* ring = find_or_clear(bar.vm_ids[0])) {
            ring->flags_lo |= 4u;
            ring->rotation.x = bar.shown * -kTwoPi;
            ring->entity_offset = center;
        }
        for (int i = 1; i < 3; ++i) {
            if (AnmVm* vm = find_or_clear(bar.vm_ids[i])) vm->entity_offset = center;
        }
        // 刻度：比例为 0 或已被显示比例越过的隐藏；其余放到环上对应角度（-π 起顺时针）
        float marker_z = 0.0f;
        for (int i = 0; i < kBossBarMarkers; ++i) {
            AnmVm* marker = find_or_clear(bar.vm_ids[3 + i]);
            if (marker == nullptr) continue;
            const float ratio = bar.markers[i];
            if (ratio == 0.0f || bar.shown <= ratio) {
                AnmManager::hide_tree(*marker);
                continue;
            }
            AnmManager::show_tree(*marker);
            const float angle = wrap_angle(-kPi - ratio * kTwoPi);
            marker->flags_lo |= 4u;
            marker->rotation.z = angle;
            const float s = sin_f(angle);
            const float c = cos_f(angle);
            marker_z = boss->pos.z + marker_z;
            // 0x42865b：x = cos·0 − sin·112 + cx，y = cos·112 + sin·0 + cy（Ghidra 把两个 CRT 返回值标反了）
            marker->entity_offset = Vec3{(c * 0.0f - s * kBossBarMarkerRadius) + center.x,
                                         (c * kBossBarMarkerRadius + s * 0.0f) + center.y, marker_z};
        }
        // 自机贴近 Boss：整套中断 3（淡出）；离开到 96 以外：中断 2（恢复）
        if (player != nullptr) {
            const Enemy* slot_boss = enemies->boss_slot(slot);
            const float dx = slot_boss->pos.x - player->pos.x;
            const float dy = slot_boss->pos.y - player->pos.y;
            const float dist_sq = dx * dx + dy * dy;
            if (bar.near_player == 0) {
                if (dist_sq < kBossBarFadeDistanceSq) {
                    for (u32 id : bar.vm_ids) animations->interrupt(id, 3);
                    bar.near_player = 1;
                }
            } else if (dist_sq >= kBossBarRestoreDistanceSq) {
                for (u32 id : bar.vm_ids) animations->interrupt(id, 2);
                bar.near_player = 0;
            }
        }
    }
}

// 0x427cf0 段 (g)
void Gui::update_spell_stars() {
    for (int i = 0; i < kSpellStarSlots; ++i) {
        u32& id = spell_star_vms[i];
        if (i < spell_star_count) {
            if (id == 0) id = spawn_world(*front, kSpellStar0 + i);
        } else if (id != 0) {
            animations->interrupt(id, 1);
            id = 0;
        }
    }
}

// 0x427cf0 段 (i)：Boss 槽 0 存活且未隐藏（flags bit5）、未标记 bit0 时显示方位标
void Gui::update_boss_indicator() {
    if (enemies == nullptr) return;
    Enemy* boss = enemies->boss_slot(0);
    if (boss == nullptr || (boss->flags & 0x20u) != 0 || (boss->flags & 0x1u) != 0) {
        if (AnmVm* vm = animations->find(boss_indicator_vm)) AnmManager::hide_tree(*vm);
        return;
    }
    AnmVm* vm = find_or_clear(boss_indicator_vm);
    if (vm == nullptr) return;   // 原版不判空（方位标 VM 由 init_hud 保证存在）
    AnmManager::show_tree(*vm);
    // 血量阶段：非符卡 700 / 400 / 200 / 200，符卡 2000 / 1000 / 400 / 400；flags_1ac bit1-2 记录已到的阶段
    const bool spell_active = spellcard_flags != nullptr && (*spellcard_flags & Spellcard::kActive) != 0;
    const i32 life = boss->life_display;
    const u32 phase = flags_1ac & 6u;
    if (!spell_active) {
        if (phase == 0) {
            if (life < 700) { animations->request_interrupt(*vm, 7); flags_1ac = (flags_1ac & ~4u) | 2u; }
        } else if (phase == 2) {
            if (life < 400) { animations->request_interrupt(*vm, 8); flags_1ac = (flags_1ac & ~2u) | 4u; }
        } else if (phase == 4) {
            if (life < 200) { animations->request_interrupt(*vm, 9); flags_1ac |= 6u; }
        } else if (phase == 6) {
            if (life > 200) { animations->request_interrupt(*vm, 10); flags_1ac &= ~6u; }
        }
    } else {
        if (phase == 0) {
            if (life < 2000) { animations->request_interrupt(*vm, 7); flags_1ac = (flags_1ac & ~4u) | 2u; }
        } else if (phase == 2) {
            if (life < 1000) { animations->request_interrupt(*vm, 8); flags_1ac = (flags_1ac & ~2u) | 4u; }
        } else if (phase == 4) {
            if (life < 400) { animations->request_interrupt(*vm, 9); flags_1ac |= 6u; }
        } else if (phase == 6) {
            if (life > 400) { animations->request_interrupt(*vm, 10); flags_1ac &= ~6u; }
        }
    }
    // 位置：场地 x 换算到渲染目标像素（(x + 224) × 2），y 固定 960；与自机横向距离 < 64 时按距离变淡；
    // Boss 出了场地横向范围则完全透明
    vm->entity_offset.y = 960.0f;
    const float screen_x = boss->pos.x + 32.0f + 192.0f;
    vm->entity_offset.x = screen_x + screen_x;
    float dx = 0.0f;
    if (player != nullptr) dx = boss->pos.x - player->pos.x;
    if (dx < 0.0f) dx = -dx;
    vm->color[3] = dx < 64.0f ? u8(i32(dx * 191.0f * 0.015625f) + 0x40) : u8(0xff);
    if (boss->pos.x < -192.0f || boss->pos.x > 192.0f) vm->color[3] = 0;
}

// 0x427cf0 段 (j)
void Gui::update_bomb_ready_mark() {
    if (season_release != nullptr && season_release->usable()) {
        if (bomb_ready_shown == 0) {
            AnmVm* mark = panel_child(kBombReadyMark);
            animations->interrupt_now(mark != nullptr ? mark->id : 0u, 2);
        }
        bomb_ready_shown = 1;
        return;
    }
    if (bomb_ready_shown == 1) {
        AnmVm* mark = panel_child(kBombReadyMark);
        animations->interrupt_now(mark != nullptr ? mark->id : 0u, 3);
    }
    bomb_ready_shown = 0;
}

// ---------------------------------------------------------------------------------------------
// HUD 文字（绘制链 0x30）

namespace {

// 0x426840：Spellcard+0xa4 的取得用时编码 → (秒, 百分秒)。编码 = ((秒 + 22 + 百分秒) × 1000 + (秒 + 66) % 1000) × 100
// + (百分秒 + 33) % 100；校验不符（含从未计时的 0）时显示 999.99
void decode_spell_time(i32 record, i32& seconds, i32& hundredths) {
    const i32 s = ((record / 100) % 1000 + 0x3a6) % 1000;
    const i32 h = (record % 100 + 0x43) % 100;
    if (record / 100000 - 0x16 != s + h) {
        seconds = 999;
        hundredths = 99;
        return;
    }
    seconds = s;
    hundredths = h;
}

u32 vm_color(const AnmVm& vm) {
    return u32(vm.color[0]) | (u32(vm.color[1]) << 8) | (u32(vm.color[2]) << 16) | (u32(vm.color[3]) << 24);
}

// 场地底部季节等级数字的颜色（0..6 级）
constexpr u32 kSeasonLevelColors[7] = {
    0x60606060, 0xa0b0b080, 0xb0b8b880, 0xc0c0c080, 0xd0d0d080, 0xe0e0e080, 0xffffff30,
};

}  // namespace

// 0x428e70
void Gui::draw_hud_text(AsciiManager& text) {
    using Font = AsciiManager::Font;
    using Group = AsciiManager::Group;
    using HAlign = AsciiManager::HAlign;
    using VAlign = AsciiManager::VAlign;
    if (globals == nullptr || animations == nullptr) return;
    AsciiManager::Style& style = text.style;

    // 过关奖励分：跟着 front.anm 0x78 奖励 VM 的透明度，场地中央大字
    if (AnmVm* bonus = find_or_clear(clear_bonus_vm)) {
        text.set_alpha(bonus->color[3]);
        style.group = Group::FieldLate;
        style.font = Font::Large;
        style.h_align = HAlign::Center;
        style.v_align = VAlign::Middle;
        text.print_grouped({224.0f, 200.0f, 0.0f}, u32(clear_bonus));
        text.set_alpha(0xff);
        style.font = Font::Default;
        style.group = Group::Ui;
        style.h_align = HAlign::Left;
        style.v_align = VAlign::Top;
    }
    // Gui+0x12c HUD 根 VM 上的符卡奖励明细：th16 从不创建该 VM，整段不可达

    // 符卡用时：本次 "秒.百分秒s" 与解码出的记录各一行，跟着奖励文字底 VM 的透明度
    if (notice_label_active != 0) {
        AnmVm* label = find_or_clear(notice_label_vm);
        if (spellcard == nullptr) {
            notice_label_active = 0;
            animations->kill(notice_label_vm);
            notice_label_vm = 0;
        } else if (label == nullptr) {
            notice_label_active = 0;
        } else {
            text.set_alpha(label->color[3]);
            style.group = Group::FieldLate;
            style.font = Font::Large;
            const i32 frames = spellcard->timed_frames;
            i32 seconds = frames / 60;
            if (seconds >= 1000) seconds = 999;
            text.printf({224.0f, 144.0f, 0.0f}, "%3d.", seconds);
            style.scale = {0.6f, 0.6f};
            text.printf({268.0f, 150.0f, 0.0f}, "%.2ds", ((frames % 60) * 100) / 60);
            style.color = 0xff808080;
            text.set_alpha(label->color[3]);
            style.scale = {1.0f, 1.0f};
            i32 record_seconds, record_hundredths;
            decode_spell_time(spellcard->time_record, record_seconds, record_hundredths);
            text.printf({224.0f, 160.0f, 0.0f}, "%3d.", record_seconds);
            style.scale = {0.6f, 0.6f};
            text.printf({268.0f, 166.0f, 0.0f}, "%.2ds", record_hundredths);
            text.set_alpha(0xff);
            style.scale = {1.0f, 1.0f};
            style.font = Font::Default;
            style.group = Group::Ui;
            style.color = 0xffffffff;
        }
    }

    // 右栏：透明度跟第一个残机格（自机靠近面板时整栏变淡）
    const u8 panel_alpha = life_vms[0] != nullptr ? life_vms[0]->color[3] : 0xff;
    style.color = 0xff808080;
    style.font = Font::Large;
    text.set_alpha(panel_alpha);
    style.h_align = HAlign::Right;
    style.v_align = VAlign::Top;
    text.print_score({620.0f, 42.0f, 0.0f}, u32(globals->hiscore), u32(globals->hiscore_continues));
    style.color = 0xffffffff;
    text.set_alpha(panel_alpha);
    text.print_score({620.0f, 64.0f, 0.0f}, u32(displayed_score), u32(globals->continues));
    // 下一 extend 分（表尾 1e9 哨兵不画）
    style.h_align = HAlign::Left;
    style.v_align = VAlign::Top;
    style.color = 0xff80c0f0;
    style.h_align = HAlign::Right;
    style.v_align = VAlign::Top;
    text.set_alpha(panel_alpha);
    style.scale = {0.6f, 0.6f};
    const u32 extend_threshold = globals->extend_threshold();
    if (extend_threshold < 900000000u) text.print_grouped({618.0f, 118.0f, 0.0f}, extend_threshold * 10);
    // 炸弹碎片 n/5
    style.color = 0xffffffff;
    style.h_align = HAlign::Left;
    style.v_align = VAlign::Top;
    text.set_alpha(panel_alpha);
    style.scale = {0.6f, 0.6f};
    text.printf({576.0f, 158.0f, 0.0f}, "%3d", globals->bomb_fragments);
    text.printf({597.0f, 158.0f, 0.0f}, "/%d", 5);
    // 火力 x.xx / y.00
    style.color = 0xffff8030;
    style.scale = {1.0f, 1.0f};
    text.set_alpha(panel_alpha);
    const i32 per_level = globals->power_per_level != 0 ? globals->power_per_level : 1;
    text.printf({540.0f, 182.0f, 0.0f}, "%d.", globals->power / per_level);
    style.scale = {0.6f, 0.6f};
    text.printf({560.0f, 189.0f, 0.0f}, "%.2d", ((globals->power % per_level) * 100) / per_level);
    style.scale = {1.0f, 1.0f};
    text.printf({574.0f, 182.0f, 0.0f}, "/%d.", globals->power_max / per_level);
    style.scale = {0.6f, 0.6f};
    text.printf({606.0f, 189.0f, 0.0f}, "00");
    // 得点值（舍到十位）与擦弹数
    style.color = 0xff40c0ff;
    style.scale = {1.0f, 1.0f};
    style.h_align = HAlign::Right;
    style.v_align = VAlign::Top;
    text.set_alpha(panel_alpha);
    const i32 point_value = globals->point_value / 100;
    text.print_grouped({620.0f, 204.0f, 0.0f}, u32(point_value - point_value % 10));
    style.color = 0xffffffff;
    text.set_alpha(panel_alpha);
    text.print_grouped({620.0f, 226.0f, 0.0f}, u32(globals->graze));
    style.color = 0xffffffff;
    style.h_align = HAlign::Left;
    style.v_align = VAlign::Top;
    style.scale = {1.0f, 1.0f};
    text.set_alpha(0xff);
    style.font = Font::Default;
    style.group = Group::Ui;

    // Boss 计时的小数点与百分秒：挂在第一位计时数字 VM 旁，颜色跟它
    const bool boss_timer_shown = enemies != nullptr && boss_timer_display >= 0 && enemies->boss_slot(0) != nullptr &&
                                  !enemies->hud_hidden && dialogue == nullptr &&
                                  (game_flags == nullptr || (*game_flags & 0x10000u) == 0);
    if (boss_timer_shown && boss_timer_vms[0] != nullptr) {
        const AnmVm& digit = *boss_timer_vms[0];
        const float x = digit.pos.x + 16.0f;
        const float y = digit.pos.y - 7.0f;
        style.color = vm_color(digit);
        style.font = Font::Large;
        style.group = Group::FieldLate;
        text.printf({x, y, 0.0f}, ".");
        style.scale = {0.6f, 0.6f};
        text.printf({x + 8.0f, y + 6.0f, 0.0f}, "%.2d", boss_timer_fraction);
        style.scale = {1.0f, 1.0f};
        style.color = 0xffffffff;
        style.group = Group::Ui;
        style.font = Font::Default;
    }

    // 场地底部的季节等级数字（场地相机组，坐标以场地左上为原点）
    const i32 level = globals->season_option_count();
    style.color = kSeasonLevelColors[level];
    if ((flags_1ac & 1u) != 0) text.set_alpha(0x40);
    style.group = Group::Field;
    style.font = Font::Medium;
    style.h_align = HAlign::Center;
    style.v_align = VAlign::Bottom;
    text.print_grouped({-132.0f, 446.0f, 0.0f}, u32(level));
    style.color = 0xffffffff;
    text.set_alpha(0xff);
    style.font = Font::Default;
    style.group = Group::Ui;
    style.h_align = HAlign::Left;
    style.v_align = VAlign::Top;
}

}  // namespace th16
