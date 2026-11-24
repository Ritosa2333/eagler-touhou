#include "../harness/test_framework.hpp"

#include "game/ItemManager.hpp"
#include "game/Rng.hpp"
#include "game/SessionState.hpp"

#include <cmath>
#include <cstdio>

namespace {

using namespace th13;

constexpr float kPi = 3.1415927410125732f;

struct Fixture {
    SessionState progress;
    HudPlayerState hud;
    HudScoreState score;
    ItemCollector collector;

    Fixture(float px = 0.0f, float py = 200.0f, float box = 5.0f, float auto_box = 0.0f, float auto_focus = 0.0f) {
        collector.x = px;
        collector.y = py;
        collector.state = 1;
        collector.focus = false;
        collector.dialog = false;
        collector.homing_speed = 3.0f;
        collector.box = {px - box, py - box, px + box, py + box};
        if (auto_box > 0.0f) collector.box_auto = {px - auto_box, py - auto_box, px + auto_box, py + auto_box};
        if (auto_focus > 0.0f)
            collector.box_auto_focus = {px - auto_focus, py - auto_focus, px + auto_focus, py + auto_focus};
        collector.progress = &progress;
        collector.player = &hud;
        collector.score = &score;
        progress.difficulty = 1;
        progress.character = 0;
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

}

TEST(item_spawn_state_and_clamp) {
    ItemManager mgr;
    mgr.spawn_item(300.0f, 200.0f, 0.0f, 1, -1.5707964f, 2.2f);
    const Item& it = mgr.items()[599];
    CHECK(it.alive);
    CHECK_EQ(it.state, 1);
    CHECK_EQ(it.type, 1);
    CHECK_EQ(it.type2, 0);
    CHECK_NEAR(it.x, 192.0f, 1e-6f);
    CHECK_NEAR(it.vx, 0.0f, 1e-5f);
    CHECK_NEAR(it.vy, -2.2f, 1e-5f);
    CHECK_EQ(it.anm_frame, 0);
    CHECK_EQ(it.anm_frame_prev, -1);
}

TEST(item_cancel_spawn_state5_delay) {
    ItemManager mgr;
    mgr.spawn_item(0.0f, 300.0f, 0.0f, 9, -1.5707964f, 2.2f);
    const Item& first = mgr.items()[2647];
    CHECK_EQ(first.state, 5);
    CHECK_EQ(first.type, 9);
    CHECK_EQ(first.type2, 9);
    CHECK_EQ(first.sprite, 0);
    mgr.spawn_item(10.0f, 300.0f, 0.0f, 9, -1.5707964f, 2.2f);
    CHECK_EQ(mgr.items()[2646].sprite, 0);
}

TEST(item_falling_physics_old_velocity_first) {
    Fixture fix(0.0f, 400.0f);
    ItemManager mgr;
    mgr.spawn_item(0.0f, 300.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    const Item& it = mgr.items()[599];
    CHECK_EQ(it.state, 1);
    CHECK_NEAR(it.y, 300.0f - 2.2f, 1e-4f);
    CHECK_NEAR(it.vy, -2.2f + 0.03f, 1e-5f);
    mgr.update(fix.collector);
    CHECK_NEAR(it.y, 300.0f - 2.2f - 2.17f, 1e-4f);
}

TEST(item_falling_vy_nonnegative_zeroes_vx) {
    Fixture fix(0.0f, 400.0f);
    ItemManager mgr;
    mgr.spawn_item(0.0f, 300.0f, 0.0f, 1, 0.0f, 2.2f);
    mgr.update(fix.collector);
    const Item& it = mgr.items()[599];
    CHECK_NEAR(it.x, 2.2f, 1e-5f);
    mgr.update(fix.collector);
    CHECK_NEAR(it.x, 2.2f, 1e-5f);
    CHECK_NEAR(it.vy, 0.06f, 1e-5f);
}

TEST(item_bottom_recycle_no_reward) {
    Fixture fix(0.0f, 400.0f);
    ItemManager mgr;
    mgr.spawn_item(0.0f, 475.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK(!mgr.items()[599].alive);
    CHECK(mgr.events().empty());
}

TEST(item_pickup_line_homing) {
    Fixture fix(0.0f, 100.0f);
    ItemManager mgr;
    mgr.spawn_item(10.0f, 200.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    const Item& it = mgr.items()[599];
    CHECK_EQ(it.state, 3);
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
    mgr.update(fix.collector);
    CHECK_EQ(it.state, 2);
    CHECK_NEAR(it.y, 300.0f, 1e-5f);
    it.vy = -0.02f;
    mgr.update(fix.collector);
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
    dying.state = 4;
    mgr.update(dying);
    const Item& it = mgr.items()[599];
    CHECK_EQ(it.state, 1);
    CHECK_NEAR(it.vx, 0.0f, 1e-6f);
    CHECK_NEAR(it.vy, 0.0f, 1e-6f);
}

TEST(item_collect_small_power_and_level_cross) {
    Fixture fix;
    ItemManager mgr;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK(!mgr.items()[599].alive);
    CHECK_EQ(fix.hud.power, 101);
    CHECK(!has_event(mgr, ItemEvent::Kind::PowerLevelUp, -1));
    mgr.clear_events();
    fix.hud.power = 199;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.hud.power, 200);
    CHECK(has_event(mgr, ItemEvent::Kind::PowerLevelUp, -1));
}

TEST(item_full_power_small_p_score_raw) {
    Fixture fix;
    fix.hud.power = 400;
    ItemManager mgr;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.score.score, 1000);
    CHECK(has_event(mgr, ItemEvent::Kind::ScorePopup, 10000));
    CHECK_EQ(fix.hud.power, 400);
}

TEST(item_big_power_collect_and_full) {
    Fixture fix;
    ItemManager mgr;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 3, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.hud.power, 200);
    CHECK(has_event(mgr, ItemEvent::Kind::PowerLevelUp, -1));
    mgr.clear_events();
    fix.hud.power = 400;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 3, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.score.score, 2000);
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
    CHECK_EQ(fix.progress.max_point, 20000);
    CHECK(has_event(mgr, ItemEvent::Kind::ScorePopup, 100));
}

TEST(item_point_value_high_and_low) {
    {
        Fixture fix(0.0f, 100.0f);
        fix.progress.max_point = 10000;
        ItemManager mgr;
        mgr.spawn_item(0.0f, 100.0f, 0.0f, 2, -1.5707964f, 2.2f);
        mgr.update(fix.collector);
        CHECK_EQ(fix.score.score, 10);
        CHECK(has_event(mgr, ItemEvent::Kind::ScorePopup, 100));
    }
    {
        Fixture fix(0.0f, 200.0f);
        fix.progress.max_point = 10000;
        ItemManager mgr;
        mgr.spawn_item(0.0f, 200.0f, 0.0f, 2, -1.5707964f, 2.2f);
        mgr.update(fix.collector);
        CHECK_EQ(fix.score.score, 6);
        CHECK(has_event(mgr, ItemEvent::Kind::ScorePopup, 60));
    }
    {
        Fixture fix(0.0f, 200.0f);
        fix.progress.max_point = 10000;
        ItemManager mgr;
        mgr.spawn_item(0.0f, 250.0f, 0.0f, 2, -1.5707964f, 2.2f);
        Item& it = mgr.items()[599];
        it.state = 3;
        for (int i = 0; i < 100 && it.alive; ++i)
            mgr.update(fix.collector);
        CHECK(!it.alive);
        CHECK_EQ(fix.score.score, 10);
    }
    {
        Fixture fix(0.0f, 128.0f);
        fix.progress.max_point = 10000;
        ItemManager mgr;
        mgr.spawn_item(0.0f, 128.0f, 0.0f, 2, -1.5707964f, 2.2f);
        mgr.update(fix.collector);
        CHECK_EQ(fix.score.score, 7);
    }
    {
        Fixture fix(0.0f, 200.0f);
        fix.progress.max_point = 50;
        ItemManager mgr;
        mgr.spawn_item(0.0f, 200.0f, 0.0f, 2, -1.5707964f, 2.2f);
        mgr.update(fix.collector);
        CHECK_EQ(fix.score.score, 1);
    }
}

TEST(item_point_items_counter) {
    Fixture fix(0.0f, 100.0f);
    fix.progress.max_point = 10000;
    ItemManager mgr;
    mgr.spawn_item(0.0f, 100.0f, 0.0f, 2, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    mgr.spawn_item(0.0f, 100.0f, 0.0f, 2, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.score.score, 20);
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
    CHECK_EQ(fix.hud.extra_lives, 3);
    CHECK_EQ(fix.hud.life_pieces, 0);
    CHECK_EQ(fix.progress.life_piece_threshold_index, 1);
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
    fix.hud.bombs = 9;
    fix.hud.bomb_pieces = 7;
    ItemManager mgr;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 5, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.hud.bombs, 9);
    CHECK_EQ(fix.hud.bomb_pieces, 0);
    fix.hud.bombs = 2;
    for (int i = 0; i < 4; ++i) {
        mgr.spawn_item(0.0f, 200.0f, 0.0f, 5, -1.5707964f, 2.2f);
        mgr.update(fix.collector);
    }
    CHECK_EQ(fix.hud.bombs, 3);
    CHECK_EQ(fix.hud.bomb_pieces, 0);
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
    CHECK_EQ(fix.hud.extra_lives, 8);
    fix.hud.bombs = 8;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 7, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.hud.bombs, 9);
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 7, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(fix.hud.bombs, 9);
}

TEST(item_cancel_spirit_collect) {
    Fixture fix;
    fix.progress.max_point = 10000;
    fix.progress.max_point_cap = 10000000;
    ItemManager mgr;
    mgr.spawn_item(0.0f, 200.0f, 0.0f, 9, -1.5707964f, 2.2f);
    Item& it = mgr.items()[2647];
    it.state = 2;
    mgr.update(fix.collector);
    CHECK(!it.alive);
    CHECK_EQ(fix.score.score, 1);
    CHECK_EQ(fix.progress.max_point, 10010);
}

TEST(item_auto_collect_box_to_state4) {
    Fixture fix(0.0f, 100.0f, 5.0f, 40.0f);
    ItemManager mgr;
    mgr.spawn_item(30.0f, 100.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    CHECK_EQ(mgr.items()[599].state, 3);
}

TEST(item_auto_collect_box_while_falling) {
    Fixture fix(0.0f, 300.0f, 5.0f, 40.0f);
    ItemManager mgr;
    mgr.spawn_item(30.0f, 300.0f, 0.0f, 1, -1.5707964f, 2.2f);
    mgr.update(fix.collector);
    const Item& it = mgr.items()[599];
    CHECK_EQ(it.state, 4);
    CHECK_NEAR(it.homing_speed, 1.0f, 1e-5f);
    CHECK(it.alive);
}

TEST(item_dialog_blocks_magnet_only) {
    Fixture fix(0.0f, 300.0f);
    ItemManager mgr;
    mgr.spawn_item(30.0f, 300.0f, 0.0f, 1, -1.5707964f, 2.2f);
    ItemCollector dialog = fix.collector;
    dialog.dialog = true;
    mgr.update(dialog);
    CHECK_EQ(mgr.items()[599].state, 3);
}

TEST(spirit_spawn_two_rng_draws_order) {
    GameRandom random(0x1234);
    ItemManager mgr(&random);
    GameRandom twin(0x1234);
    mgr.spawn_value_item(0, 0.0f, 200.0f, 0.0f, 0.5f);
    const Spirit& s = mgr.spirits()[0];
    CHECK(s.alive);
    CHECK_EQ(s.state, 1);
    CHECK_EQ(s.kind, 0);
    const float su = twin.signed_unit();
    const float u = twin.unit();
    CHECK_EQ(s.flags & 1u, 0u);
    CHECK_EQ(s.flags & 2u, 2u);
    CHECK_NEAR(s.angle, su * kPi * 0.25f + 0.7853982f, 1e-6f);
    CHECK_NEAR(s.move_speed, u * 3.5f + 0.5f, 1e-6f);
    CHECK_NEAR(s.speed, 0.0f, 1e-9f);
    CHECK_EQ(s.frame, -1);
    CHECK_EQ(twin.index(), random.index());
}

TEST(spirit_kind4_resolves_to_one) {
    GameRandom random(7);
    ItemManager mgr(&random);
    mgr.spawn_value_item(4, 0.0f, 200.0f, 0.0f, 0.5f);
    CHECK_EQ(mgr.spirits()[0].kind, 1);
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
    CHECK_EQ(count_spirits(mgr, 1), 10);
    CHECK_EQ(count_spirits(mgr, 3), 1);
    mgr.spawn_value_item(1, 0.0f, 200.0f, 0.0f, 0.5f);
    CHECK_EQ(count_spirits(mgr, 3), 2);
}

TEST(spirit_pool_capacity_256) {
    GameRandom random(5);
    ItemManager mgr(&random);
    for (int i = 0; i < 400; ++i)
        mgr.spawn_value_item(3, 0.0f, 200.0f, 0.0f, 0.5f);
    i32 total = 0;
    for (const Spirit& s : mgr.spirits())
        if (s.alive) ++total;
    CHECK_EQ(total, 256);
}

TEST(spirit_trance_flag_from_session) {
    GameRandom random(11);
    SessionState session;
    ItemManager mgr(&random);
    trance_begin(session);
    mgr.bind_session(session);
    mgr.spawn_value_item(0, 0.0f, 200.0f, 0.0f, 0.5f);
    CHECK_EQ(mgr.spirits()[0].flags & 1u, 1u);
}

TEST(spirit_free_to_tracking) {
    GameRandom random(3);
    ItemManager mgr(&random);
    Fixture fix(0.0f, 200.0f, 5.0f);
    mgr.spawn_value_item(0, 0.0f, 120.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    s.x = 0.0f;
    s.y = 120.0f;
    mgr.update_spirits(fix.collector);
    CHECK_EQ(s.state, 2);
    CHECK_NEAR(s.speed, 0.0f, 1e-6f);
    mgr.update_spirits(fix.collector);
    CHECK_NEAR(s.speed, 0.3f, 1e-6f);
    mgr.update_spirits(fix.collector);
    mgr.update_spirits(fix.collector);
    CHECK_NEAR(s.speed, 0.9f, 1e-5f);
}

TEST(spirit_motion_quantizes_after_float_velocity_like_original) {
    struct Case {
        float x, y, speed, angle, expected_x, expected_y;
    };
    for (const auto value : {Case{-10.123f, 100.007f, 1.0f, 0.0f, -9.13f, 100.0f},
                             Case{-10.123f, 100.007f, 1.0f, -1.5707963705062866f, -10.13f, 99.0f},
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

TEST(spirit_right_edge_reflects_when_other_axis_is_outside) {
    GameRandom random(3);
    ItemManager mgr(&random);
    Fixture fix(0.f, 300.f);
    mgr.spawn_value_item(3, 0.f, 0.f, 0.f, 0.5f);
    Spirit& spirit = mgr.spirits()[0];
    spirit.x = 192.f;
    spirit.y = -0.59f;
    spirit.angle = 0.f;
    spirit.move_speed = 0.1f;
    mgr.update_spirits(fix.collector);
    CHECK_EQ(spirit.x, 191.9f);

    spirit.x = 192.f;
    spirit.y = 100.f;
    spirit.angle = 0.f;
    spirit.move_speed = 0.1f;
    mgr.update_spirits(fix.collector);
    CHECK_EQ(spirit.x, 192.1f);
}

TEST(spirit_collect_kind0_common_trance_and_piece) {
    GameRandom random(3);
    ItemManager mgr(&random);
    Fixture fix(0.0f, 200.0f, 20.0f);
    mgr.spawn_value_item(0, 0.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    s.x = 5.0f;
    s.y = 205.0f;
    mgr.update_spirits(fix.collector);
    CHECK(!s.alive);
    CHECK_EQ(fix.progress.ending_points, 2);
    CHECK_EQ(fix.hud.life_pieces, 1);
    CHECK(has_event(mgr, ItemEvent::Kind::Collected, 10));
}

TEST(trance_born_life_spirit_adds_two_pieces_and_crosses_threshold_once) {
    GameRandom random(3);
    SessionState session;
    trance_begin(session);
    ItemManager mgr(&random);
    mgr.bind_session(session);
    Fixture fix(0, 200, 20);
    fix.hud.extra_lives = 2;
    fix.hud.life_pieces = 7;
    fix.score.score = 1234;
    mgr.spawn_value_item(0, 0, 200, 0, .5f);
    auto& spirit = mgr.spirits()[0];
    CHECK(spirit.flags & 1u);
    spirit.x = 5;
    spirit.y = 205;
    trance_end(session, false);
    mgr.update_spirits(fix.collector);
    CHECK(!spirit.alive);
    CHECK_EQ(fix.hud.extra_lives, 3);
    CHECK_EQ(fix.hud.life_pieces, 1);
    CHECK_EQ(fix.progress.life_piece_threshold_index, 1);
    CHECK_EQ(fix.progress.ending_points, 0);
    CHECK_EQ(fix.score.score, 1234);
}

TEST(spirit_collect_kind3_score_and_trance) {
    GameRandom random(3);
    ItemManager mgr(&random);
    Fixture fix(0.0f, 200.0f, 20.0f);
    fix.progress.max_point = 10000;
    mgr.spawn_value_item(3, 0.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    s.x = 5.0f;
    s.y = 205.0f;
    mgr.update_spirits(fix.collector);
    CHECK(!s.alive);
    CHECK_EQ(fix.progress.ending_points, 3);
    CHECK_EQ(fix.score.score, 10);
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
    s.x = 5.0f;
    s.y = 205.0f;
    mgr.update_spirits(fix.collector);
    CHECK(!s.alive);
    CHECK_EQ(fix.progress.ending_points, 0);
    CHECK_EQ(fix.hud.bomb_pieces, 2);
}

TEST(spirit_collect_kind1_max_point_and_char2_trance) {
    GameRandom random(3);
    ItemManager mgr(&random);
    Fixture fix(0.0f, 200.0f, 20.0f);
    fix.progress.max_point = 10000;
    fix.progress.max_point_cap = 10000000;
    fix.progress.character = 2;
    mgr.spawn_value_item(1, 0.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    s.x = 5.0f;
    s.y = 205.0f;
    mgr.update_spirits(fix.collector);
    CHECK_EQ(fix.progress.max_point, 11000);
    CHECK_EQ(fix.progress.ending_points, 3);
}

TEST(spirit_dying_player_no_collect_slow_home) {
    GameRandom random(3);
    SessionState session;
    trance_begin(session);
    ItemManager mgr(&random);
    mgr.bind_session(session);
    Fixture fix(0.0f, 200.0f, 20.0f);
    mgr.spawn_value_item(0, 0.0f, 60.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    ItemCollector dying = fix.collector;
    dying.state = 2;
    mgr.update_spirits(dying);
    CHECK(s.alive);
    CHECK_EQ(s.state, 4);
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
    s.x = 230.0f;
    s.y = 200.0f;
    mgr.update_spirits(fix.collector);
    CHECK(!s.alive);
    GameRandom random2(3);
    SessionState session;
    trance_begin(session);
    ItemManager mgr2(&random2);
    mgr2.bind_session(session);
    Fixture fix2(0.0f, 200.0f, 5.0f);
    mgr2.spawn_value_item(0, 230.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s2 = mgr2.spirits()[0];
    s2.x = 230.0f;
    s2.y = 200.0f;
    mgr2.update_spirits(fix2.collector);
    CHECK(s2.alive);
}

TEST(spirit_frame_480_closes_bounce_gate) {
    GameRandom random(3);
    ItemManager mgr(&random);
    Fixture fix(0.0f, 200.0f, 5.0f);
    fix.collector.x = -400.0f;
    mgr.spawn_value_item(0, 0.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    for (int i = 0; i < 481; ++i)
        mgr.update_spirits(fix.collector);
    CHECK_EQ(s.flags & 2u, 0u);
    CHECK(s.alive);
}

TEST(spirit_trance_reset_keeps_lifetime_clock_aligned) {
    GameRandom random(3);
    ItemManager mgr(&random);
    Fixture fix(0.0f, 200.0f, 5.0f);
    fix.collector.x = -400.0f;
    mgr.spawn_value_item(0, 0.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    for (int i = 0; i < 10; ++i)
        mgr.update_spirits(fix.collector);
    CHECK_EQ(s.frame, 9);
    mgr.set_spirit_trance(false);
    CHECK_EQ(s.frame, -1);
    CHECK_EQ(s.flags & 1u, 0u);
    mgr.update_spirits(fix.collector);
    CHECK_EQ(s.frame, 0);
    for (int i = 0; i < 479; ++i)
        mgr.update_spirits(fix.collector);
    CHECK_EQ(s.frame, 479);
    CHECK_EQ(s.flags & 2u, 2u);
    mgr.update_spirits(fix.collector);
    CHECK_EQ(s.frame, 480);
    CHECK_EQ(s.flags & 2u, 0u);
    CHECK(s.alive);
}

TEST(trance_lifecycle_helpers) {
    SessionState s;
    CHECK_EQ(trance_value(s), 0);
    CHECK(!trance_ready(s));
    trance_add(s, 250);
    CHECK_EQ(trance_value(s), 250);
    trance_add(s, 400);
    CHECK_EQ(trance_value(s), 600);
    CHECK(trance_ready(s));
    trance_begin(s);
    CHECK(trance_active(s));
    CHECK(!trance_ready(s));
    trance_end(s, false);
    CHECK(!trance_active(s));
    CHECK_EQ(trance_value(s), 400);
    trance_begin(s);
    trance_end(s, true);
    CHECK_EQ(trance_value(s), 400);
    trance_add(s, -1000);
    CHECK_EQ(trance_value(s), -600);
}

TEST(trance_collect_events_cross_segment) {
    GameRandom random(3);
    ItemManager mgr(&random);
    Fixture fix(0.0f, 200.0f, 20.0f);
    fix.progress.ending_points = 199;
    mgr.spawn_value_item(3, 0.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s = mgr.spirits()[0];
    s.x = 5.0f;
    s.y = 205.0f;
    mgr.update_spirits(fix.collector);
    CHECK_EQ(fix.progress.ending_points, 202);
    CHECK(has_event(mgr, ItemEvent::Kind::SpiritGained, -1));
}

int main() { return th13test::run_all("items") ? 1 : 0; }
