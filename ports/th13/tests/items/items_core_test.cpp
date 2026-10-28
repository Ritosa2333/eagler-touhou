// 道具/神霊/灵界定向单测（模块 I）：
// 覆盖 拾取/灵力/分数（原值×10 口径）/碎片阈值/灵界进入退出/死亡保护，
// 逐条对照 artifacts/decomp/jp/functions/{0042f020,0042e380,0042edc0,
// 0043d210,0043d2f0,0043d330,0043d440,0043d480,0043d530,00438ec0,00438730}.c。
// 全部纯逻辑（无资源装载）；RNG 用 GameRandom 孪生态核对抽取次序。
#include "../harness/test_framework.hpp"

#include "game/ItemManager.hpp"
#include "game/Rng.hpp"
#include "game/SessionState.hpp"

#include <cmath>
#include <cstdio>

namespace {

using namespace th13;

constexpr float kPi = 3.1415927410125732f;

// 便利装配：玩家 (x,y)（state 1 活跃），收集盒 ±box，吸附盒 ±auto。
struct Fixture {
    SessionState progress;
    HudPlayerState hud;
    HudScoreState score;
    ItemCollector collector;

    Fixture(float px = 0.0f, float py = 200.0f, float box = 5.0f,
            float auto_box = 0.0f, float auto_focus = 0.0f) {
        collector.x = px;
        collector.y = py;
        collector.state = 1;
        collector.focus = false;
        collector.dialog = false;
        collector.homing_speed = 3.0f;  // 原版 *(player+0x660 表+8) 取 3.0 定标
        collector.box = {px - box, py - box, px + box, py + box};
        if (auto_box > 0.0f)
            collector.box_auto = {px - auto_box, py - auto_box,
                                  px + auto_box, py + auto_box};
        if (auto_focus > 0.0f)
            collector.box_auto_focus = {px - auto_focus, py - auto_focus,
                                        px + auto_focus, py + auto_focus};
        collector.progress = &progress;
        collector.player = &hud;
        collector.score = &score;
        progress.difficulty = 1;  // DAT_004be7c4 < 5 门
        progress.character = 0;   // DAT_004be7b8（选人光标）
    }
};

i32 count_items(ItemManager& mgr, i32 type) {
    i32 n = 0;
    for (const Item& it : mgr.items())
        if (it.alive && it.type == type) ++n;
    return n;
}

i32 count_spirits(ItemManager& mgr, i32 kind) {
    i32 n = 0;
    for (const Spirit& s : mgr.spirits())
        if (s.alive && s.kind == kind) ++n;
    return n;
}

bool has_event(ItemManager& mgr, ItemEvent::Kind kind, i32 value) {
    for (const auto& e : mgr.events())
        if (e.kind == kind && (value == -1 || e.value == value)) return true;
    return false;
}

}  // namespace

// 413e50/42f020: ordinary slots start at599, cancel slots at2647.
// ---- 生成与钳制（0x42f020）----

TEST(item_spawn_state_and_clamp) {
    ItemManager mgr;
    mgr.spawn_item(300.0f, 200.0f, 0.0f, 1, -1.5707964f, 2.2f);
    const Item& it = mgr.items()[599];
    CHECK(it.alive);
    CHECK_EQ(it.state, 1);
    CHECK_EQ(it.type, 1);
    CHECK_EQ(it.type2, 0);
    CHECK_NEAR(it.x, 192.0f, 1e-6f);  // x 钳 [-192,192]
    CHECK_NEAR(it.vx, 0.0f, 1e-5f);   // polar(-π/2, 2.2)
    CHECK_NEAR(it.vy, -2.2f, 1e-5f);
    CHECK_EQ(it.anm_frame, 0);      // +0xB7C 出生 0
    CHECK_EQ(it.anm_frame_prev, -1);  // +0xB78 出生 -1
}

TEST(item_cancel_spawn_state5_delay) {
    ItemManager mgr;
    mgr.spawn_item(0.0f, 300.0f, 0.0f, 9, -1.5707964f, 2.2f);
    const Item& first = mgr.items()[2647];
    CHECK_EQ(first.state, 5);
    CHECK_EQ(first.type, 9);
    CHECK_EQ(first.type2, 9);
    CHECK_EQ(first.sprite, 0);  // 首件 total&3 = 0
    mgr.spawn_item(10.0f, 300.0f, 0.0f, 9, -1.5707964f, 2.2f);
    CHECK_EQ(mgr.items()[2646].sprite, 0);  // native42f020 does not increment the counter
}

// ---- 落下/追踪状态机（0x42e380）----

TEST(item_falling_physics_old_velocity_first) {
    Fixture fix(0.0f, 400.0f);  // y>=128 → 落下门
    ItemManager mgr;
    mgr.spawn_item(0.0f, 300.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    const Item& it = mgr.items()[599];
    CHECK_EQ(it.state, 1);
    CHECK_NEAR(it.y, 300.0f - 2.2f, 1e-4f);  // 旧速位移
    CHECK_NEAR(it.vy, -2.2f + 0.03f, 1e-5f);  // 后加减速
    mgr.update(fix.collector);
    CHECK_NEAR(it.y, 300.0f - 2.2f - 2.17f, 1e-4f);
}

TEST(item_falling_vy_nonnegative_zeroes_vx) {
    Fixture fix(0.0f, 400.0f);
    ItemManager mgr;
    mgr.spawn_item(0.0f, 300.0f, 0.0f, 1, 0.0f, 2.2f);  // 水平初速
    mgr.update(fix.collector);
    const Item& it = mgr.items()[599];
    CHECK_NEAR(it.x, 2.2f, 1e-5f);  // 首帧仍按旧 vx 位移
    mgr.update(fix.collector);
    CHECK_NEAR(it.x, 2.2f, 1e-5f);  // vy>=0 → vx 清零
    CHECK_NEAR(it.vy, 0.06f, 1e-5f);
}

TEST(item_bottom_recycle_no_reward) {
    Fixture fix(0.0f, 400.0f);
    ItemManager mgr;
    mgr.spawn_item(0.0f, 475.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK(!mgr.items()[599].alive);  // y>472 回收，无结算
    CHECK(mgr.events().empty());
}

TEST(item_pickup_line_homing) {
    Fixture fix(0.0f, 100.0f);  // y<128 → 追踪
    ItemManager mgr;
    mgr.spawn_item(10.0f, 200.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    const Item& it = mgr.items()[599];
    CHECK_EQ(it.state, 3);
    // 追踪步：位移恰 3.0（collector.homing_speed），初速 +0.2。
    const float dx = -10.0f, dy = -100.0f;
    const float len = std::sqrt(dx * dx + dy * dy);
    CHECK_NEAR(it.x, 10.0f + 3.0f * dx / len, 1e-4f);
    CHECK_NEAR(it.y, 200.0f + 3.0f * dy / len, 1e-4f);
    CHECK_NEAR(it.homing_speed, 3.2f, 1e-5f);
}

TEST(item_state2_to3_transition_frame_homes) {
    Fixture fix(0.0f, 100.0f);
    ItemManager mgr;
    mgr.spawn_item(0.0f, 300.0f, 0.0f, 9, -1.5707964f, 2.2f);
    Item& it = mgr.items()[2647];
    it.sprite = 0;
    mgr.update(fix.collector);  // state5 → 2（无位移）
    CHECK_EQ(it.state, 2);
    CHECK_NEAR(it.y, 300.0f, 1e-5f);
    it.vy = -0.02f;  // 下一帧 +0.03 后 >= 0 → 转 3
    mgr.update(fix.collector);
    // 转态帧：先旧速位移（300-0.02）再走追踪步（向下玩家 3.0）。
    CHECK_EQ(it.state, 3);
    CHECK_NEAR(it.y, 300.0f - 0.02f - 3.0f, 1e-4f);
}

TEST(item_deathbomb_reverts_homing) {
    Fixture fix(0.0f, 100.0f);
    ItemManager mgr;
    mgr.spawn_item(10.0f, 200.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(mgr.items()[599].state, 3);
    ItemCollector dying = fix.collector;
    dying.state = 4;  // 决死窗（player+0x65c == 4）
    mgr.update(dying);
    const Item& it = mgr.items()[599];
    CHECK_EQ(it.state, 1);  // LAB_0042e7d7：3/4 同门回落
    CHECK_NEAR(it.vx, 0.0f, 1e-6f);
    CHECK_NEAR(it.vy, 0.0f, 1e-6f);
}

// ---- 收集结算（0x42e380 case 表 + 各 0x43xxxx）----

TEST(item_collect_small_power_and_level_cross) {
    Fixture fix;
    ItemManager mgr;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK(!mgr.items()[599].alive);
    CHECK_EQ(fix.hud.power, 101);  // 100 → 101 无跨档
    CHECK(!has_event(mgr, ItemEvent::Kind::PowerLevelUp, -1));
    mgr.clear_events();
    fix.hud.power = 199;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.hud.power, 200);
    CHECK(has_event(mgr, ItemEvent::Kind::PowerLevelUp, -1));  // 199/100≠200/100
}

TEST(item_full_power_small_p_score_raw) {
    Fixture fix;
    fix.hud.power = 400;
    ItemManager mgr;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.score.score, 1000);  // 原值域 +1000
    CHECK(has_event(mgr, ItemEvent::Kind::ScorePopup, 10000));  // 显示值 10000
    CHECK_EQ(fix.hud.power, 400);
}

TEST(item_big_power_collect_and_full) {
    Fixture fix;
    ItemManager mgr;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 3, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.hud.power, 200);  // +power_step(100)
    CHECK(has_event(mgr, ItemEvent::Kind::PowerLevelUp, -1));
    mgr.clear_events();
    fix.hud.power = 400;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 3, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.score.score, 2000);  // 20000/10 原值
    CHECK(has_event(mgr, ItemEvent::Kind::ScorePopup, 20000));
    CHECK_EQ(fix.hud.power, 400);
}

TEST(item_big_power_variant_full_adds_two_max_point) {
    Fixture fix;
    fix.progress.max_point = 10000;
    fix.progress.max_point_cap = 10000000;
    fix.hud.power = 400;
    ItemManager mgr;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 8, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
        CHECK_EQ(fix.progress.max_point, 20000);  // 0x42f510: fmul 100.0 @4ae868 → 100×100
    CHECK(has_event(mgr, ItemEvent::Kind::ScorePopup, 100));
}

TEST(item_point_value_high_and_low) {
    {
        Fixture fix(0.0f, 100.0f);  // y=100 < 128 → 高位
        fix.progress.max_point = 10000;
        ItemManager mgr;
        mgr.spawn_item(0.0f, 100.0f, 0.0f, 2, -1.5707964f, 2.2f);
        mgr.update(fix.collector);
        CHECK_EQ(fix.score.score, 10);  // value 100 显示，原值 +100/10
        CHECK(has_event(mgr, ItemEvent::Kind::ScorePopup, 100));
    }
    {
        Fixture fix(0.0f, 200.0f);  // y>=128 非 state3 → 低位
        fix.progress.max_point = 10000;
        ItemManager mgr;
        mgr.spawn_item(0.0f, 200.0f, 0.0f, 2, -1.5707964f, 2.2f);
        mgr.update(fix.collector);
        // floor4(-300)=-75、-75*72/450=-12、+floor4(300)=75 → 63 → 60。
        CHECK_EQ(fix.score.score, 6);
        CHECK(has_event(mgr, ItemEvent::Kind::ScorePopup, 60));
    }
    {
        Fixture fix(0.0f, 200.0f);
        fix.progress.max_point = 10000;
        ItemManager mgr;
        mgr.spawn_item(0.0f, 250.0f, 0.0f, 2, -1.5707964f, 2.2f);
        Item& it = mgr.items()[599];
        it.state = 3;  // 追踪态 → 高位公式
        for (int i = 0; i < 100 && it.alive; ++i) mgr.update(fix.collector);
        CHECK(!it.alive);
        CHECK_EQ(fix.score.score, 10);
    }
    {
        Fixture fix(0.0f, 128.0f);  // 恰在线上：y>=128 → 低位
        fix.progress.max_point = 10000;
        ItemManager mgr;
        mgr.spawn_item(0.0f, 128.0f, 0.0f, 2, -1.5707964f, 2.2f);
        mgr.update(fix.collector);
        // h=0 → pre=75 → value 70。
        CHECK_EQ(fix.score.score, 7);
    }
    {
        Fixture fix(0.0f, 200.0f);
        fix.progress.max_point = 50;  // A = 0
        ItemManager mgr;
        mgr.spawn_item(0.0f, 200.0f, 0.0f, 2, -1.5707964f, 2.2f);
        mgr.update(fix.collector);
        CHECK_EQ(fix.score.score, 1);  // value 钳 10 → +1 原值
    }
}

TEST(item_point_items_counter) {
    Fixture fix(0.0f, 100.0f);  // 高位（y<128）
    fix.progress.max_point = 10000;
    ItemManager mgr;
    mgr.spawn_item(0.0f, 100.0f, 0.0f, 2, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    mgr.spawn_item(0.0f, 100.0f, 0.0f, 2, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.score.score, 20);  // 2×（100/10 原值；0x4be7d8 计数侧无对外差）
}

TEST(item_life_piece_thresholds) {
    Fixture fix;
    fix.hud.extra_lives = 2;
    ItemManager mgr;
    for (int i = 0; i < 8; ++i) {
        mgr.spawn_item(0.0f, 200.0f, 0.0f, 4, -1.5707964f, 2.2f);
        mgr.update(fix.collector);
        mgr.clear_events();
    }
    CHECK_EQ(fix.hud.extra_lives, 3);  // 8 碎片 → +1 命
    CHECK_EQ(fix.hud.life_pieces, 0);
    CHECK_EQ(fix.progress.life_piece_threshold_index, 1);  // 0x4bb994 下标推进
    // 第二档 10 碎片。
    for (int i = 0; i < 9; ++i) {
        mgr.spawn_item(0.0f, 200.0f, 0.0f, 4, -1.5707964f, 2.2f);
        mgr.update(fix.collector);
    }
    CHECK_EQ(fix.hud.extra_lives, 3);
    CHECK_EQ(fix.hud.life_pieces, 9);
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 4, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.hud.extra_lives, 4);
    CHECK_EQ(fix.progress.life_piece_threshold_index, 2);
    // 命满 8：碎片照扣、不加命、下标照走（第三档阈值 12）。
    fix.hud.extra_lives = 8;
    for (int i = 0; i < 12; ++i) {
        mgr.spawn_item(0.0f, 200.0f, 0.0f, 4, -1.5707964f, 2.2f);
        mgr.update(fix.collector);
    }
    CHECK_EQ(fix.hud.extra_lives, 8);
    CHECK_EQ(fix.hud.life_pieces, 0);
    CHECK_EQ(fix.progress.life_piece_threshold_index, 3);
}

TEST(item_bomb_piece_overflow_caps_nine) {
    Fixture fix;
    fix.hud.bombs = 9;      // 已满：+1 后按 min(9+1,9) 钳 9
    fix.hud.bomb_pieces = 7;
    ItemManager mgr;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 5, -1.5707964f, 2.2f);  // +2 → 9 > 7
    mgr.update(fix.collector);
    CHECK_EQ(fix.hud.bombs, 9);  // 0043d480：先 +1 后 ≥10 钳 9（非 10）
    CHECK_EQ(fix.hud.bomb_pieces, 0);
    // 常规：2 炸弹 0 碎片，四次 +2 → 8 → +1 炸弹。
    fix.hud.bombs = 2;
    for (int i = 0; i < 4; ++i) {
        mgr.spawn_item(0.0f, 200.0f, 0.0f, 5, -1.5707964f, 2.2f);
        mgr.update(fix.collector);
    }
    CHECK_EQ(fix.hud.bombs, 3);
    CHECK_EQ(fix.hud.bomb_pieces, 0);
    // 灵击超上限（>8）：碎片清零直收。
    fix.hud.bombs = 10;
    fix.hud.bomb_pieces = 5;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 5, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.hud.bombs, 10);
    CHECK_EQ(fix.hud.bomb_pieces, 0);
}

TEST(item_extra_life_and_bomb_caps) {
    Fixture fix;
    ItemManager mgr;
    fix.hud.extra_lives = 7;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 6, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.hud.extra_lives, 8);
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 6, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.hud.extra_lives, 8);  // 1UP 上限 8
    fix.hud.bombs = 8;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 7, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.hud.bombs, 9);
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 7, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.hud.bombs, 9);  // 灵击上限 9
}

TEST(item_cancel_spirit_collect) {
    Fixture fix;
    fix.progress.max_point = 10000;
    fix.progress.max_point_cap = 10000000;
    ItemManager mgr;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 9, -1.5707964f, 2.2f);
    Item& it = mgr.items()[2647];
    it.state = 2;  // 越过 state5 计时直入上升态
    mgr.update(fix.collector);
    CHECK(!it.alive);
    CHECK_EQ(fix.score.score, 1);          // 10/10 原值
    CHECK_EQ(fix.progress.max_point, 10010);  // 0x42f510: fmul 100.0 → 0.1×100
}

// ---- 吸附盒（0x42e380:305-315）----

TEST(item_auto_collect_box_to_state4) {
    Fixture fix(0.0f, 100.0f, 5.0f, 40.0f);  // 线上（y<128 → 追踪门关）
    ItemManager mgr;
    mgr.spawn_item(30.0f, 100.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    // y<128 且玩家活跃 → state3 追踪而非吸附。吸附盒仅在「未追踪」时触发。
    CHECK_EQ(mgr.items()[599].state, 3);
}

TEST(item_auto_collect_box_while_falling) {
    // 玩家 y>=128（线下）→ 磁门 = 落下；近身吸附盒 → state4、初速 = 表速/3。
    Fixture fix(0.0f, 300.0f, 5.0f, 40.0f);
    ItemManager mgr;
    mgr.spawn_item(30.0f, 300.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    const Item& it = mgr.items()[599];
    CHECK_EQ(it.state, 4);
    CHECK_NEAR(it.homing_speed, 1.0f, 1e-5f);  // 3.0/3
    CHECK(it.alive);  // 未收集（在收集盒外）
}

TEST(item_dialog_blocks_magnet_only) {
    Fixture fix(0.0f, 300.0f);  // y>=128
    ItemManager mgr;
    mgr.spawn_item(30.0f, 300.0f, 0.0f, 1, -1.5707964f, 2.2f);
    ItemCollector dialog = fix.collector;
    dialog.dialog = true;
    mgr.update(dialog);
    CHECK_EQ(mgr.items()[599].state, 3);  // 对话 → 磁门关 → 追踪
}

// ---- 神霊生成（0x438ec0）----

TEST(spirit_spawn_two_rng_draws_order) {
    GameRandom random(0x1234);
    ItemManager mgr(&random);
    GameRandom twin(0x1234);
    mgr.spawn_value_item(0, 0.0f, 200.0f, 0.0f, 0.5f);
    const Spirit& s = mgr.spirits()[0];
    CHECK(s.alive);
    CHECK_EQ(s.state, 1);
    CHECK_EQ(s.kind, 0);
    // 每件两次抽取，先 signed_unit（角）后 unit（速）。
    const float su = twin.signed_unit();
    const float u = twin.unit();
    CHECK_EQ(s.flags & 1u, 0u);  // 未绑会话 → bit0 = 0
    CHECK_EQ(s.flags & 2u, 2u);  // bit1 反弹门
    CHECK_NEAR(s.angle, su * kPi * 0.25f + 0.7853982f, 1e-6f);  // id=1 → 象限1
    CHECK_NEAR(s.move_speed, u * 3.5f + 0.5f, 1e-6f);
    CHECK_NEAR(s.speed, 0.0f, 1e-9f);
    CHECK_EQ(s.frame, -1);
    CHECK_EQ(twin.index(), random.index());  // 抽次一致
}

TEST(spirit_kind4_resolves_to_one) {
    GameRandom random(7);
    ItemManager mgr(&random);
    mgr.spawn_value_item(4, 0.0f, 200.0f, 0.0f, 0.5f);
    CHECK_EQ(mgr.spirits()[0].kind, 1);  // 0x438ec0:18
}

TEST(spirit_pos_clamp) {
    GameRandom random(7);
    ItemManager mgr(&random);
    mgr.spawn_value_item(1, 300.0f, 500.0f, 0.0f, 0.5f);
    const Spirit& s = mgr.spirits()[0];
    CHECK_NEAR(s.x, 192.0f, 1e-6f);
    CHECK_NEAR(s.y, 384.0f, 1e-6f);
}

TEST(spirit_overflow_spawns_kind3) {
    GameRandom random(99);
    ItemManager mgr(&random);
    for (int i = 0; i < 10; ++i)
        mgr.spawn_value_item(1, 0.0f, 200.0f, 0.0f, 0.5f);
    // 第 10 件（计数 10 > 9）起同位补 kind3。
    CHECK_EQ(count_spirits(mgr, 1), 10);
    CHECK_EQ(count_spirits(mgr, 3), 1);
    mgr.spawn_value_item(1, 0.0f, 200.0f, 0.0f, 0.5f);
    CHECK_EQ(count_spirits(mgr, 3), 2);  // 之后每件都补
}

TEST(spirit_pool_capacity_256) {
    GameRandom random(5);
    ItemManager mgr(&random);
    for (int i = 0; i < 400; ++i)
        mgr.spawn_value_item(3, 0.0f, 200.0f, 0.0f, 0.5f);  // kind3 不计溢出
    i32 total = 0;
    for (const Spirit& s : mgr.spirits())
        if (s.alive) ++total;
    CHECK_EQ(total, 256);  // 0x438ec0 扫描 0x100 槽，满即静默丢弃
}

TEST(spirit_trance_flag_from_session) {
    GameRandom random(11);
    SessionState session;
    ItemManager mgr(&random);
    trance_begin(session);  // be830 bit8
    mgr.bind_session(session);
    mgr.spawn_value_item(0, 0.0f, 200.0f, 0.0f, 0.5f);
    CHECK_EQ(mgr.spirits()[0].flags & 1u, 1u);
}

// ---- 神霊步进与收集（0x438730）----

TEST(spirit_free_to_tracking) {
    GameRandom random(3);
    ItemManager mgr(&random);
    Fixture fix(0.0f, 200.0f, 5.0f);
    mgr.spawn_value_item(0, 0.0f, 120.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    s.x = 0.0f; s.y = 120.0f;  // dist=80：6400 < 9216（入追踪）且 >= 400（不收集）
    mgr.update_spirits(fix.collector);
    // 1→2 转态帧：case1 只改态，尾段仍按出生 move_speed（衰减后）位移。
    CHECK_EQ(s.state, 2);
    CHECK_NEAR(s.speed, 0.0f, 1e-6f);
    mgr.update_spirits(fix.collector);
    // case2：move_speed = speed（0）→ 无位移；speed → 0.3。
    CHECK_NEAR(s.speed, 0.3f, 1e-6f);
    mgr.update_spirits(fix.collector);
    mgr.update_spirits(fix.collector);
    CHECK_NEAR(s.speed, 0.9f, 1e-5f);  // 每帧 +0.3
}

TEST(spirit_motion_quantizes_after_float_velocity_like_original) {
    struct Case { float x, y, speed, angle, expected_x, expected_y; };
    for (const auto value : {Case{-10.123f, 100.007f, 1.0f, 0.0f, -9.13f, 100.0f},
                             Case{-10.123f, 100.007f, 1.0f, -1.5707963705062866f,
                                  -10.13f, 99.0f},
                             Case{0.0f, 0.0f, 3.125f, 0.7853981852531433f, 2.2f, 2.2f}}) {
        GameRandom random(3);
        ItemManager mgr(&random);
        Fixture fix(180.0f, 400.0f);
        mgr.spawn_value_item(1, value.x, value.y, 0, 0.5f);
        Spirit& spirit = mgr.spirits()[0];
        spirit.state = 2;
        spirit.x = value.x;
        spirit.y = value.y;
        spirit.angle = value.angle;
        spirit.speed = value.speed;
        fix.collector.x = value.x + std::cos(value.angle) * 1000.0f;
        fix.collector.y = value.y + std::sin(value.angle) * 1000.0f;
        mgr.update_spirits(fix.collector);
        CHECK_EQ(spirit.x, value.expected_x);
        CHECK_EQ(spirit.y, value.expected_y);
        CHECK(spirit.alive);
    }
}

TEST(spirit_collect_kind0_common_trance_and_piece) {
    GameRandom random(3);
    ItemManager mgr(&random);
    Fixture fix(0.0f, 200.0f, 20.0f);
    mgr.spawn_value_item(0, 0.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    s.x = 5.0f; s.y = 205.0f;  // dist² = 50 < 400
    mgr.update_spirits(fix.collector);
    CHECK(!s.alive);
    CHECK_EQ(fix.progress.ending_points, 2);   // 公共 trance +2
    CHECK_EQ(fix.hud.life_pieces, 1);          // kind0 → 残机碎片
    CHECK(has_event(mgr, ItemEvent::Kind::Collected, 10));
}

TEST(spirit_collect_kind3_score_and_trance) {
    GameRandom random(3);
    ItemManager mgr(&random);
    Fixture fix(0.0f, 200.0f, 20.0f);
    fix.progress.max_point = 10000;
    mgr.spawn_value_item(3, 0.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    s.x = 5.0f; s.y = 205.0f;
    mgr.update_spirits(fix.collector);
    CHECK(!s.alive);
    CHECK_EQ(fix.progress.ending_points, 3);  // +2 公共 +1 kind3
    CHECK_EQ(fix.score.score, 10);            // (100-0)/10 原值
    CHECK(has_event(mgr, ItemEvent::Kind::ScorePopup, 100));
}

TEST(spirit_collect_bit0_path_no_common_trance) {
    GameRandom random(3);
    SessionState session;
    trance_begin(session);
    ItemManager mgr(&random);
    mgr.bind_session(session);
    Fixture fix(0.0f, 200.0f, 20.0f);
    mgr.spawn_value_item(2, 0.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    s.x = 5.0f; s.y = 205.0f;
    mgr.update_spirits(fix.collector);
    CHECK(!s.alive);
    CHECK_EQ(fix.progress.ending_points, 0);  // bit0=1：无公共 +2
    CHECK_EQ(fix.hud.bomb_pieces, 2);         // kind2 → 0x43d480(2)
}

TEST(spirit_collect_kind1_max_point_and_char2_trance) {
    GameRandom random(3);
    ItemManager mgr(&random);
    Fixture fix(0.0f, 200.0f, 20.0f);
    fix.progress.max_point = 10000;
    fix.progress.max_point_cap = 10000000;
    fix.progress.character = 2;  // DAT_004be7b8 == 2 → 再 +1 trance
    mgr.spawn_value_item(1, 0.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    s.x = 5.0f; s.y = 205.0f;
    mgr.update_spirits(fix.collector);
    // 0x438baf: fld 10.0 → 0x42f510 fmul 100.0 → +1000
    CHECK_EQ(fix.progress.max_point, 11000);
    CHECK_EQ(fix.progress.ending_points, 3);  // +2 公共 +1 角色门
}

TEST(spirit_dying_player_no_collect_slow_home) {
    GameRandom random(3);
    SessionState session;
    trance_begin(session);
    ItemManager mgr(&random);
    mgr.bind_session(session);
    Fixture fix(0.0f, 200.0f, 20.0f);
    mgr.spawn_value_item(0, 0.0f, 60.0f, 0.0f, 0.5f);  // 远（>96）
    Spirit& s = mgr.spirits()[0];
    ItemCollector dying = fix.collector;
    dying.state = 2;  // player+0x65c dying
    mgr.update_spirits(dying);
    CHECK(s.alive);      // 不收集
    CHECK_EQ(s.state, 4);  // 角色门（be7b8==0 且 bit0）
    // 角色 1：同条件下保持游离。
    GameRandom random2(3);
    SessionState session2;
    trance_begin(session2);
    ItemManager mgr2(&random2);
    mgr2.bind_session(session2);
    Fixture fix2(0.0f, 200.0f, 20.0f);
    fix2.progress.character = 1;
    mgr2.spawn_value_item(0, 0.0f, 60.0f, 0.0f, 0.5f);
    ItemCollector dying2 = fix2.collector;
    dying2.state = 2;
    mgr2.update_spirits(dying2);
    CHECK_EQ(mgr2.spirits()[0].state, 1);
}

TEST(spirit_cull_box_and_bit1_exemption) {
    GameRandom random(3);
    ItemManager mgr(&random);
    Fixture fix(0.0f, 200.0f, 5.0f);
    mgr.spawn_value_item(0, 230.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    s.x = 230.0f; s.y = 200.0f;
    mgr.update_spirits(fix.collector);
    CHECK(!s.alive);  // x>224（0x40bb70(0,224,448,512) 外）→ 回收
    // trance 产件（bit0=1）无回收路径。
    GameRandom random2(3);
    SessionState session;
    trance_begin(session);
    ItemManager mgr2(&random2);
    mgr2.bind_session(session);
    Fixture fix2(0.0f, 200.0f, 5.0f);
    mgr2.spawn_value_item(0, 230.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s2 = mgr2.spirits()[0];
    s2.x = 230.0f; s2.y = 200.0f;
    mgr2.update_spirits(fix2.collector);
    CHECK(s2.alive);
}

TEST(spirit_frame_480_closes_bounce_gate) {
    GameRandom random(3);
    ItemManager mgr(&random);
    Fixture fix(0.0f, 200.0f, 5.0f);
    fix.collector.x = -400.0f;  // 远离：永不追踪/收集
    mgr.spawn_value_item(0, 0.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    for (int i = 0; i < 481; ++i) mgr.update_spirits(fix.collector);
    CHECK_EQ(s.flags & 2u, 0u);  // [0x18]==480 → 清 bit1
    CHECK(s.alive);
}

// ---- 灵界（SessionState 单一真值 + 0x43d530/0x405a20/0x405b80）----

TEST(trance_lifecycle_helpers) {
    SessionState s;
    CHECK_EQ(trance_value(s), 0);
    CHECK(!trance_ready(s));
    trance_add(s, 250);
    CHECK_EQ(trance_value(s), 250);
    trance_add(s, 400);
    CHECK_EQ(trance_value(s), 600);  // 封顶（0x43d530）
    CHECK(trance_ready(s));
    trance_begin(s);
    CHECK(trance_active(s));
    CHECK(!trance_ready(s));
    trance_end(s, false);
    CHECK(!trance_active(s));
    CHECK_EQ(trance_value(s), 400);  // 正常结束 -200
    trance_begin(s);
    trance_end(s, true);
    CHECK_EQ(trance_value(s), 400);  // 死亡保护不扣
    trance_add(s, -1000);
    CHECK_EQ(trance_value(s), -600);  // 0x43d530 无下限（下限钳在 0x405b80 扣减路径）
}

TEST(trance_collect_events_cross_segment) {
    GameRandom random(3);
    ItemManager mgr(&random);
    Fixture fix(0.0f, 200.0f, 20.0f);
    fix.progress.ending_points = 199;
    mgr.spawn_value_item(3, 0.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    s.x = 5.0f; s.y = 205.0f;
    mgr.update_spirits(fix.collector);
    // +2 公共跨 200 段、+1 kind3 → 202；段位演出事件存在。
    CHECK_EQ(fix.progress.ending_points, 202);
    CHECK(has_event(mgr, ItemEvent::Kind::SpiritGained, -1));
}

int main() {
    return th13test::run_all("items") ? 1 : 0;
}
