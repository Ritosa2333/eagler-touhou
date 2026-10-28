// Boss HUD 单测：0x4265a0/0x425170/0x413270/0x413a60 移植断言（证据行号见
// BossPresentation.hpp/SpellPresentation.hpp 头注释）。分两层：
//   合成资源层 —— 状态机（门控/警告/比值平滑/星位/邻近/位标/符卡结算）；
//   真实资源层 —— artifacts/reference/raw 的 ascii/effect.anm：计时数字
//   脚本 2/3、数字字模 sprite 270-279、血环色族脚本 81..93 的池装载与选取。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/BossPresentation.hpp"
#include "game/SpellPresentation.hpp"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

std::vector<u8> read_raw(const char* name) {
    std::ifstream file(name, std::ios::binary);
    if (!file) return {};
    return std::vector<u8>(std::istreambuf_iterator<char>(file),
                           std::istreambuf_iterator<char>());
}

struct SoundLog {
    std::vector<i32> ids;
    bool play(i32 id) {
        ids.push_back(id);
        return true;
    }
};

struct BossHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    BossPresentation boss{manager, {3, 2, 8}};
    BossHarness() {
        // ascii bank：数字 VM 脚本 2/3 + 字模 270-279。
        CHECK(load_bank(manager, 2, synthetic_anm(302, trivial_scripts(46))));
        // front bank：环/星脚本 240/241/242/243（front/lifebar，UC 取证）。
        CHECK(load_bank(manager, 3, synthetic_anm(16, trivial_scripts(244))));
    }
    AnmVm* vm(u32 handle) { return manager.registry.find(handle); }
};

i32 flags_of(AnmVm* vm) { return vm ? i32(vm->visual.flags) : -1; }
bool shown(AnmVm* vm) { return vm && (vm->visual.flags & 3u) == 3u; }

}  // namespace

TEST(timer_fraction_follows_live_digit_animation_position) {
    BossHarness h;
    CHECK(h.boss.initialize());
    auto* digit = h.vm(h.boss.digit_tens_handle());
    CHECK(digit);
    digit->variables.position = {208,51,0};
    CHECK_EQ(h.boss.timer_text_x(), 224.f);
    CHECK_EQ(h.boss.timer_text_y(), 44.f);
    digit->variables.position.y = 23;
    CHECK_EQ(h.boss.timer_text_y(), 16.f);
}

TEST(boss_name_uses_stage_table_script_and_retires_with_boss) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy; enemy.life = 1000;
    BossFrameInput input; input.boss = &enemy; input.name_script = 155;
    input.player_y = 400; input.boss_life = input.life_initial = 1000;
    CHECK(h.boss.update(input, {}));
    u32 name = 0;
    for (u32 i = 0; i < h.manager.registry.ordered_count(false); ++i) {
        const auto handle = h.manager.registry.ordered_handle(false, i);
        const auto* vm = h.manager.registry.find(handle);
        if (vm && vm->source_script == 155) name = handle;
    }
    CHECK(name);
    input.boss = nullptr;
    CHECK(h.boss.update(input, {}));
    CHECK(h.manager.update());
    CHECK(!h.manager.registry.find(name));
}

TEST(boss_timer_digits_hidden_until_gate_opens) {
    BossHarness h;
    CHECK(h.boss.initialize());
    CHECK(h.boss.digit_tens_handle() != 0);
    CHECK(h.boss.digit_ones_handle() != 0);
    CHECK(h.boss.ring_arc_handle() == 0);  // 未开门前不创建
    CHECK(!h.boss.timer_visible());
}

TEST(boss_gate_opens_digits_and_swaps_digit_sprites) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.life = 1000;
    enemy.position[0] = 0;
    enemy.position[1] = 128;
    BossFrameInput input;
    input.boss = &enemy;
    input.timer_seconds = 35;
    input.timer_frames = 30;
    input.boss_life = 700;
    input.life_initial = 1000;
    SoundLog sound;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.timer_visible());
    CHECK(h.boss.timer_seconds() == 35);
    CHECK(h.boss.timer_centiseconds() == 50);  // 30 帧 * 100/60
    // 数字精灵 = 270 + 35/10(=3) / 270 + 35%10(=5)（0x461a60 同式）。
    CHECK_EQ(h.vm(h.boss.digit_tens_handle())->visual.sprite, 273);
    CHECK_EQ(h.vm(h.boss.digit_ones_handle())->visual.sprite, 275);
    CHECK(shown(h.vm(h.boss.digit_tens_handle())));
    // 警告声只跨 <10/<5 档各一次：35 不触发。
    CHECK(sound.ids.empty());
}

TEST(boss_timer_warnings_fire_each_decremented_second) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    BossFrameInput input;
    input.boss = &enemy;
    input.life_initial = 0;  // 环待命（分母缺口语义），只测计时。
    SoundLog sound;
    input.timer_seconds = 12;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(sound.ids.empty());
    input.timer_seconds = 9;  // <10：声 0xb（0x4265a0:154-158）
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(sound.ids.size(), 1u);
    CHECK_EQ(sound.ids[0], 0xb);
    input.timer_seconds = 8; // 426808 executes again at each decremented second.
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(sound.ids.size(), 2u);CHECK_EQ(sound.ids.back(),11);
    CHECK_EQ(h.vm(h.boss.digit_tens_handle())->pending_interrupt,8);
    input.timer_seconds = 4;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(sound.ids.size(), 3u);CHECK_EQ(sound.ids.back(),12);
    CHECK_EQ(h.vm(h.boss.digit_tens_handle())->pending_interrupt,9);
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(sound.ids.size(),3u); // same second, no extra sound
    input.timer_seconds = 20;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(sound.ids.size(),3u);CHECK_EQ(h.boss.warn_state(),0);
    CHECK_EQ(h.vm(h.boss.digit_tens_handle())->pending_interrupt,7);
    input.timer_seconds = 9;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(sound.ids.size(),4u);

}

TEST(boss_ring_is_independent_of_timer_and_empty_boss_retires_it) {
    BossHarness h;CHECK(h.boss.initialize());StageEnemy enemy;
    enemy.life=1000;enemy.position[1]=128;
    BossFrameInput input;input.boss=&enemy;input.player_y=400;
    input.boss_life=1000;input.life_initial=1000;input.timer_seconds=-1;
    CHECK(h.boss.update(input,{}));CHECK(!h.boss.timer_visible());
    CHECK(h.boss.ring_arc_handle()!=0);
    input.boss=nullptr;CHECK(h.boss.update(input,{}));
    CHECK_EQ(h.boss.ring_arc_handle(),0u);
    input.boss=&enemy;enemy.invulnerable=20;CHECK(h.boss.update(input,{}));
    CHECK_EQ(h.boss.ring_arc_handle(),0u);
    enemy.invulnerable=0;CHECK(h.boss.update(input,{}));
    CHECK(h.boss.ring_arc_handle()!=0);
}

TEST(remaining_card_stars_use_original_front_scripts_and_exit_interrupt) {
    BossHarness h;CHECK(h.boss.initialize());BossFrameInput input;
    input.remaining_stars=3;CHECK(h.boss.update(input,{}));
    std::vector<AnmVm*> stars;
    for(u32 i=0;i<h.manager.registry.ordered_count(false);++i)
        if(auto* vm=h.manager.registry.find(h.manager.registry.ordered_handle(false,i)))
            if(vm->resource==h.manager.resource(3)&&vm->source_script>=68&&vm->source_script<=77)stars.push_back(vm);
    CHECK_EQ(stars.size(),3u);
    input.remaining_stars=1;CHECK(h.boss.update(input,{}));
    for(auto* vm:stars)CHECK_EQ(vm->pending_interrupt,vm->source_script==68?0:1);
}

TEST(boss_ring_ratio_smooths_up_drops_instantly) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.position[1] = 128;
    BossFrameInput input;
    input.boss = &enemy;
    input.timer_seconds = 60;
    input.life_initial = 1000;
    input.boss_life = 1000;
    SoundLog sound;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    // 环组：front 240/241/242 三条带（UC 取证身份，0x426b06/0x426bad/0x426c66）。
    CHECK(h.boss.ring_arc_handle() != 0);
    CHECK(h.boss.ring_overlay_handle(0) != 0);
    CHECK(h.boss.ring_overlay_handle(1) != 0);
    CHECK(h.boss.displayed_ratio() == 0.025f);  // 首帧 +0.025（0x4265a0:230-239）
    // 逼近：32 帧到 0.8。
    for (int i = 0; i < 31; i++) CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_NEAR(h.boss.displayed_ratio(), 0.8, 1e-4);
    input.boss_life = 500;  // 目标 0.5 < displayed 0.8 → 直落
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.displayed_ratio() == 0.5f);
    input.boss_life = 1000;  // 回升仅按 0.025/帧逼近
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.displayed_ratio() > 0.5f);
    CHECK(h.boss.displayed_ratio() <= 0.525f);
    // 驱动写入（0x426fcc-0x426fd8）：rotation.x = -displayed*2π、
    // offset(+0x574..) = boss 世界位（0x426fdb-）。
    auto* arc = h.vm(h.boss.ring_arc_handle());
    CHECK(arc != nullptr);
    CHECK(arc->variables.rotation.x < 0.f);
    CHECK(arc->visual.translation.y == 128.f);
    auto* overlay = h.vm(h.boss.ring_overlay_handle(0));
    CHECK(overlay != nullptr);
    CHECK(overlay->visual.translation.y == 128.f);
}

TEST(boss_ratio_crosses_phase_markers) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.position[1] = 128;
    BossFrameInput input;
    input.boss = &enemy;
    input.timer_seconds = 60;
    input.life_initial = 1000;
    input.boss_life = 1000;
    SoundLog sound;
    input.markers[0].ratio = 0.5f;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(!shown(h.vm(h.boss.star_handles()[0])));  // native: below threshold hides
    input.boss_life = 400;  // displayed 追向 0.4，仍 < 0.5
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(!shown(h.vm(h.boss.star_handles()[0])));
    input.boss_life = 1000;  // 追回 0.5+ 后越过星 0（0x4265a0:458-477 判据）。
    for (int i = 0; i < 40; i++) CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.displayed_ratio() >= 0.5f);
    CHECK(shown(h.vm(h.boss.star_handles()[0])));
    input.boss_life=500;CHECK(h.boss.update(input,{}));
    CHECK(!shown(h.vm(h.boss.star_handles()[0]))); // equality hides
}

TEST(boss_stars_created_and_hidden_when_passed) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.position[0] = 10;
    enemy.position[1] = 128;
    BossFrameInput input;
    input.boss = &enemy;
    input.timer_seconds = 60;
    input.life_initial = 1000;
    input.boss_life = 1000;
    input.markers[0].ratio = 0.25f;
    input.markers[1].ratio = 0.5f;
    SoundLog sound;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.displayed_ratio() == 0.025f);  // all thresholds hidden during ring growth
    CHECK(!shown(h.vm(h.boss.star_handles()[0])));
    CHECK(!shown(h.vm(h.boss.star_handles()[1])));
    input.boss_life = 500;  // 追至 0.5 → 越过星 0(0.25) 与星 1(0.5)
    for (int i = 0; i < 20; i++) CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.displayed_ratio() >= 0.5f);
    CHECK(shown(h.vm(h.boss.star_handles()[0])));
    CHECK(!shown(h.vm(h.boss.star_handles()[1])));
    // 星可见态复核 + 轨道半径 56（0x42713c-0x427193：boss + 56·(cos t, sin t)）。
    input.boss_life = 900;  // 目标 0.9 > 0.5 → displayed 0.525 起，星 0 仍越过。
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    auto* star = h.vm(h.boss.star_handles()[0]);
    CHECK(star != nullptr);
    CHECK(shown(star));  // remaining HP exceeds this threshold
    // 新会话重置后星 0 显示位：displayed 从 0 起时 t = -π-0.25*2π。
    BossHarness fresh;
    CHECK(fresh.boss.initialize());
    BossFrameInput fi;
    StageEnemy enemy2;
    enemy2.position[0] = 0;
    enemy2.position[1] = 128;
    fi.boss = &enemy2;
    fi.timer_seconds = 60;
    fi.life_initial = 1000;
    fi.boss_life = 1000;
    fi.markers[0].ratio = 0.25f;
    SoundLog fsound;
    for(int frame=0;frame<20;++frame)CHECK(fresh.boss.update(fi, [&](i32 id) { return fsound.play(id); }));
    auto* star0 = fresh.vm(fresh.boss.star_handles()[0]);
    CHECK(star0 != nullptr);
    CHECK(shown(star0));
    const float t = star0->variables.rotation.z;  // normalize(-π-0.25*2π) = +π/2
    CHECK_NEAR(double(t), 1.5707963, 1e-4);
    // 位置 = boss + 56·(cos t, sin t)：y = 128 + 56·sin(t)。
    CHECK_NEAR(double(star0->visual.translation.y), double(128.f + 56.f * std::sin(t)), 1e-3);
    CHECK_NEAR(double(star0->visual.translation.x), double(56.f * std::cos(t)), 1e-3);
}

TEST(boss_proximity_hides_ring_with_hysteresis) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.position[0] = 0;
    enemy.position[1] = 0;
    BossFrameInput input;
    input.boss = &enemy;
    input.timer_seconds = 60;
    input.life_initial = 1000;
    input.boss_life = 1000;
    input.player_x = 200;
    input.player_y = 300;  // 远离（>96）
    SoundLog sound;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.ring_arc_handle() != 0);
    CHECK(!h.boss.proximity_hidden());
    input.player_x = 40;
    input.player_y = 0;  // 距离 40 < 80 → 隐藏
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.proximity_hidden());
    CHECK(h.boss.ring_arc_handle() == 0);
    input.player_x = 90;  // 90 ∈ [80,96) → 迟滞保持隐藏
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.proximity_hidden());
    input.player_x = 100;  // ≥96 → 复原
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(!h.boss.proximity_hidden());
    CHECK(h.boss.ring_arc_handle() != 0);
}

TEST(boss_marker_glyph_tracks_horizontal_distance) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.position[0] = 30;
    BossFrameInput input;
    input.boss = &enemy;
    input.timer_seconds = 30;
    input.player_x = 0;
    SoundLog sound;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    // '@' + round(30*191/64) = 0x40 + 90（0x4265a0:720-727；30*2.984375=89.53）。
    CHECK_EQ(h.boss.marker_glyph(), 0x40 + 90);
    enemy.position[0] = 100;  // |dx| >= 64 → 0xff
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(h.boss.marker_glyph(), 0xff);
    enemy.position[0] = -200;  // 横向出界 → 0
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(h.boss.marker_glyph(), 0);
    // 位标位置 = (boss_x + 224, 480)。
    auto* marker = h.vm(h.boss.marker_handle());
    CHECK(marker != nullptr);
    CHECK(marker->visual.translation.x == 24.f);
    CHECK(marker->visual.translation.y == 480.f);
    CHECK_EQ(marker->source_script,111);
    CHECK_EQ(marker->visual.color>>24,0u);
}

TEST(boss_marker_flashes_by_remaining_phase_hp_and_resets_after_phase_change) {
    BossHarness h;CHECK(h.boss.initialize());StageEnemy enemy;
    enemy.phase_life_remaining=800;
    BossFrameInput input;input.boss=&enemy;input.dialog_active=true;
    SoundLog sound;auto update=[&]{CHECK(h.boss.update(input,[&](i32 id){return sound.play(id);}));};
    update();auto* marker=h.vm(h.boss.marker_handle());CHECK(marker);if(!marker)return;
    CHECK(shown(marker));CHECK_EQ(marker->pending_interrupt,0);
    enemy.phase_life_remaining=699;update();CHECK_EQ(marker->pending_interrupt,7);
    enemy.phase_life_remaining=399;update();CHECK_EQ(marker->pending_interrupt,8);
    enemy.phase_life_remaining=199;update();CHECK_EQ(marker->pending_interrupt,9);
    enemy.phase_life_remaining=1500;update();CHECK_EQ(marker->pending_interrupt,10);
    CHECK(sound.ids.empty()); // native marker does not play a capture sound
    input.boss=nullptr;update();CHECK(!shown(marker));
}

TEST(spell_card_lifecycle_bonus_and_sounds) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    CHECK(load_bank(manager, 4, synthetic_anm(8, trivial_scripts(151))));  // cardbg 根 150
    SpellPresentation spell{manager, {4}};
    SoundLog sound;
    i32 awarded = 0;
    const float pos[3] = {0, 128, 0};
    CHECK(spell.begin(3, "符卡", 60, 1, 1, pos, [&](i32 id) { return sound.play(id); }));
    CHECK(spell.active());
    CHECK(spell.captured());  // 0x413270:42 bit1 开场即置
    // bonus = (stage+diff)*1000000、显示封顶 99999999。
    CHECK_EQ(spell.bonus_base(), 2000000);
    CHECK_EQ(spell.bonus_display(), 2000000);
    CHECK_EQ(spell.duration_seconds(), 60);
    CHECK_EQ(sound.ids.size(), 1u);
    CHECK_EQ(sound.ids[0], 0x21);  // 开场声
    CHECK(spell.background_handle() != 0);
    CHECK(spell.end(true, false, [&](i32 id) { return sound.play(id); },
                    [&](i32 amount) { awarded += amount; }));
    CHECK_EQ(awarded, 200000);  // 取得：bonus/10（0x413a60:34-36）
    CHECK_EQ(sound.ids.size(), 2u);
    CHECK_EQ(sound.ids[1], 0x2e);
    CHECK(!spell.active());
    // 最终符：0x45 追加。
    CHECK(spell.begin(4, "终符", 99, 6, 4, pos, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(spell.bonus_base(), 10000000);
    CHECK(spell.end(true, true, [&](i32 id) { return sound.play(id); },
                    [&](i32 amount) { awarded += amount; }));
    CHECK_EQ(awarded, 1200000);
    CHECK_EQ(sound.ids.size(), 5u);
    CHECK_EQ(sound.ids[2], 0x21);  // 终符开场声（每次 begin 均响）
    CHECK_EQ(sound.ids[3], 0x2e);
    CHECK_EQ(sound.ids[4], 0x45);
}

TEST(spell_break_capture_blocks_award) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    CHECK(load_bank(manager, 4, synthetic_anm(8, trivial_scripts(151))));
    SpellPresentation spell{manager, {4}};
    SoundLog sound;
    i32 awarded = 0;
    const float pos[3] = {0, 0, 0};
    CHECK(spell.begin(0, "卡", 30, 1, 0, pos, [&](i32 id) { return sound.play(id); }));
    spell.break_capture();  // 死亡/炸弹路径（宿主通知）
    CHECK(!spell.captured());
    CHECK(spell.end(true, false, [&](i32 id) { return sound.play(id); },
                    [&](i32 amount) { awarded += amount; }));
    CHECK_EQ(awarded, 0);
    CHECK(sound.ids.size() <= 1u);  // 无取得声
    // 未结束再开新卡 = 显式失败。
    const float again[3] = {0, 0, 0};
    CHECK(spell.begin(1, "乙", 30, 1, 0, again, [&](i32 id) { return sound.play(id); }));
    CHECK(!spell.begin(2, "丙", 30, 1, 0, again, [&](i32 id) { return sound.play(id); }));
}

TEST(real_resources_match_ring_and_digit_script_map) {
    // 真实资源层：ascii.anm（数字字模 sprite 270-279 + 脚本 2/3）与
    // effect.anm（血环色族 81..93，ins_604(256,36)）。
    auto ascii = read_raw("artifacts/reference/raw/ascii.anm");
    CHECK(!ascii.empty());
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    CHECK(manager.load(2, ascii.data(), u32(ascii.size())));
    // 生产同构：ascii bank 兼任负值精灵回落（Application.cpp:266 语义；
    // effect 脚本族 ins_300(-1) 消费回落字模 301）。
    CHECK(manager.sprite_fallback(2));
    auto front = read_raw("artifacts/reference/raw/front.anm");
    CHECK(!front.empty());
    CHECK(manager.load(3,front.data(),u32(front.size())));
    BossPresentation boss{manager, {3, 2, 8}};
    CHECK(boss.initialize());
    StageEnemy enemy;
    enemy.position[1] = 128;
    BossFrameInput input;
    input.boss = &enemy;
    input.timer_seconds = 42;
    input.timer_frames = 0;
    input.life_initial = 0;
    SoundLog sound;
    CHECK(boss.update(input, [&](i32 id) { return sound.play(id); }));
    auto* tens = manager.registry.find(boss.digit_tens_handle());
    auto* ones = manager.registry.find(boss.digit_ones_handle());
    CHECK(tens != nullptr && ones != nullptr);
    CHECK_EQ(tens->visual.sprite, 274);  // 270 + 42/10
    CHECK_EQ(ones->visual.sprite, 272);  // 270 + 42%10
    // 数字 VM 由 ascii script2/3 出生：脚本位 (208,39)/(220,39)。
    CHECK(tens->variables.position.x == 208.f);
    CHECK(tens->variables.position.y == 39.f);
    CHECK(ones->variables.position.x == 220.f);

    // 环/星真实资源断言（UC 取证：front.anm 240-243 = lifebar entry）。
    input.life_initial = 1000;
    input.boss_life = 1000;
    input.markers[0].ratio = 0.5f;
    CHECK(boss.update(input, [&](i32 id) { return sound.play(id); }));
    auto* arc = manager.registry.find(boss.ring_arc_handle());
    CHECK(arc != nullptr);
    CHECK_EQ(arc->source_script, 240);
    CHECK_EQ(arc->visual.sprite, 150);       // ins_300(sprite150)（4×16 条带）
    CHECK_EQ(arc->visual.layer, 17);         // ins_304(17)
    CHECK_EQ(arc->variables.integers[0], 38);  // ins_100(10000,38) 段数
    auto* overlay = manager.registry.find(boss.ring_overlay_handle(0));
    CHECK(overlay != nullptr);
    CHECK_EQ(overlay->source_script, 241);
    CHECK_EQ(overlay->visual.sprite, 151);
    CHECK_EQ(overlay->variables.integers[0], 38);      // 10000=38（段数同弧）
    CHECK_NEAR(double(overlay->visual.scale.y), 58.0, 1e-4);  // ins_402(1,58)
    auto* star = manager.registry.find(boss.star_handles()[0]);
    CHECK(star != nullptr);
    CHECK_EQ(star->source_script, 243);
    CHECK_EQ(star->visual.sprite, 152);      // 3×6 星点
}

int main() { return th13test::run_all("Boss HUD"); }
