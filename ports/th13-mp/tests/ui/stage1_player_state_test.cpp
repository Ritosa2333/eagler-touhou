#include "../ecl/ecl_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/StageBattle.hpp"

#include <array>
#include <cstdio>
#include <string>
#include <vector>

using namespace th13;

namespace {

constexpr const char* kEventPath = "eagler-touhou/ports/th13/tests/ui/build/stage1_player_state.events.jsonl";

class Provider final : public EclResourceProvider {
public:
    bool read(const std::string& name, std::vector<u8>& data) override {
        return reader.read("artifacts/reference/raw/" + name, data);
    }
    bool animation(u32, const std::string& name) override {
        std::vector<u8> probe;
        if (!read(name, probe)) return false;
        if (anim_list) anim_list->push_back(name);
        return true;
    }

    th13test::TestFileReader reader;
    std::vector<std::string>* anim_list = nullptr;
};

struct Host final : StageBattleServices {
    int life_hud_calls = 0;
    int bomb_hud_calls = 0;
    bool life_hud(i32, i32) override {
        ++life_hud_calls;
        return true;
    }
    bool bomb_hud(i32, i32) override {
        ++bomb_hud_calls;
        return true;
    }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};

class Fingerprint {
public:
    void add(const void* data, size_t size) {
        const auto* bytes = static_cast<const u8*>(data);
        for (size_t i = 0; i < size; ++i) {
            hash_ ^= bytes[i];
            hash_ *= 1099511628211ull;
        }
    }
    template <class T> void field(const T& value) { add(&value, sizeof value); }
    void text(const std::string& value) {
        const u32 size = u32(value.size());
        field(size);
        add(value.data(), value.size());
    }
    u64 value() const { return hash_; }

private:
    u64 hash_ = 1469598103934665603ull;
};

template <class T> void hash_mover(Fingerprint& p, const T& mover) {
    for (float v : mover.pos)
        p.field(v);
    p.field(mover.speed);
    p.field(mover.angle);
    p.field(mover.radius);
    p.field(mover.growth);
    for (float v : mover.aux)
        p.field(v);
    p.field(mover.mode);
    p.field(mover.polar.from_a);
    p.field(mover.polar.from_b);
    p.field(mover.polar.to_a);
    p.field(mover.polar.to_b);
    p.field(mover.polar.duration);
    p.field(mover.polar.elapsed);
    p.field(mover.polar.mode);
    for (float v : mover.postime.start)
        p.field(v);
    for (float v : mover.postime.target)
        p.field(v);
    for (float v : mover.postime.p1)
        p.field(v);
    for (float v : mover.postime.p2)
        p.field(v);
    p.field(mover.postime.duration);
    p.field(mover.postime.elapsed);
    p.field(mover.postime.mode);
}

struct StateSections {
    u64 progress = 0, player = 0, score = 0, battle = 0, sim = 0;
};

StateSections production_state(const StageBattle& battle, const SessionState& progress, const HudPlayerState& player,
                               const HudScoreState& score) {
    StateSections out;

    Fingerprint p;
    p.field(progress.stage);
    p.field(progress.stage_frame);
    p.field(progress.character);
    p.field(progress.difficulty);
    p.field(progress.continues);
    p.field(progress.high_score);
    p.field(progress.high_score_extra);
    p.field(progress.configured_lives);
    p.field(progress.starting_stage);
    p.field(progress.rate);
    p.field(progress.scene_flags);
    p.field(progress.new_run);
    p.field(progress.run_mode_flags);
    p.field(progress.practice_mode);
    p.field(progress.ending_points);
    p.field(progress.good_endings);
    p.field(progress.stage_clear_bonus);
    p.field(progress.stage_cleared_bits);
    p.field(progress.max_point);
    p.field(progress.max_point_cap);
    p.field(progress.graze);
    p.field(progress.life_piece_threshold_index);
    out.progress = p.value();

    Fingerprint hp;
    hp.field(player.extra_lives);
    hp.field(player.life_pieces);
    hp.field(player.bombs);
    hp.field(player.bomb_pieces);
    hp.field(player.power);
    hp.field(player.max_power);
    hp.field(player.power_step);
    hp.field(player.deaths);
    hp.field(player.mode_flags);
    out.player = hp.value();

    Fingerprint hs;
    hs.field(score.score);
    hs.field(score.life_piece_tier);
    out.score = hs.value();

    Fingerprint b;
    b.field(battle.player_x);
    b.field(battle.player_y);
    b.field(battle.focus);
    b.field(battle.player_dead);
    b.field(battle.invulnerable);
    b.field(battle.bomb_timer);
    b.field(battle.pos_fx);
    b.field(battle.pos_fy);
    b.field(battle.speed_orth);
    b.field(battle.speed_orth_focus);
    b.field(battle.speed_diag);
    b.field(battle.speed_diag_focus);
    b.field(battle.input_state);
    b.field(battle.sht_shots_loaded);
    const u32 shot_count = u32(battle.shots.size());
    b.field(shot_count);
    for (const auto& shot : battle.shots) {
        b.field(shot.x);
        b.field(shot.y);
        b.field(shot.damage);
        b.field(shot.speed);
        b.field(shot.angle);
        b.field(shot.script);
        b.field(shot.alive);
    }
    out.battle = b.value();

    Fingerprint s;
    s.field(battle.sim.frame);
    s.field(battle.sim.boss_nonspell_frame);
    s.field(battle.sim.random.seed());
    s.field(battle.sim.random.index());
    for (i32 slot : battle.sim.boss_slot)
        s.field(slot);
    s.field(battle.sim.boss_main_serial);
    s.field(battle.sim.spawn_count());
    const u32 enemy_count = u32(battle.sim.enemies.size());
    s.field(enemy_count);
    for (const auto& enemy : battle.sim.enemies) {
        s.field(enemy->serial);
        s.field(enemy->script_id);
        s.field(enemy->trace_id);
        s.text(enemy->routine);
        s.field(enemy->life);
        hash_mover(s, enemy->vel);
        hash_mover(s, enemy->pos);
        for (float v : enemy->position)
            s.field(v);
        s.field(enemy->age);
        s.field(enemy->flags);
        for (float v : enemy->hurtbox)
            s.field(v);
        for (float v : enemy->limit_center)
            s.field(v);
        for (float v : enemy->limit_half)
            s.field(v);
        for (const auto& et : enemy->et) {
            s.field(et.sprite);
            s.field(et.subtype);
            for (float v : et.spawn)
                s.field(v);
            s.field(et.angle1);
            s.field(et.angle2);
            s.field(et.speed1);
            s.field(et.speed2);
            s.field(et.spin);
            s.field(et.count);
            s.field(et.ways);
            s.field(et.aim);
            for (float v : et.offset)
                s.field(v);
            for (float v : et.absolute)
                s.field(v);
            for (const auto& ex : et.ex) {
                s.field(ex.op);
                for (float v : ex.args)
                    s.field(v);
            }
        }
        for (i32 v : enemy->i_slots)
            s.field(v);
        for (float v : enemy->f_slots)
            s.field(v);
        s.field(enemy->dead);
        s.field(enemy->tick_gate);
        s.text(enemy->interrupt_label);
        s.field(enemy->anm_file);
        for (i32 v : enemy->anm_script)
            s.field(v);
        for (u32 v : enemy->anm_slot_handle)
            s.field(v);
        for (float v : enemy->anm_size_a)
            s.field(v);
        for (float v : enemy->anm_size_b)
            s.field(v);
        s.field(enemy->mirror_dir);
        s.field(enemy->mirror_prev_x);
        s.field(enemy->mirror_script_base);
    }
    const u32 bullet_count = u32(battle.sim.bullets.size());
    s.field(bullet_count);
    for (const auto& bullet : battle.sim.bullets) {
        s.field(bullet.x);
        s.field(bullet.y);
        s.field(bullet.z);
        s.field(bullet.angle);
        s.field(bullet.speed);
        for (float v : bullet.vel)
            s.field(v);
        for (float v : bullet.steer)
            s.field(v);
        s.field(bullet.ex4_duration);
        s.field(bullet.ex4_timer);
        s.field(bullet.ex8_speed_delta);
        s.field(bullet.ex8_angle_delta);
        s.field(bullet.ex8_duration);
        s.field(bullet.ex8_timer);
        s.field(bullet.sprite);
        s.field(bullet.subtype);
        for (float v : bullet.hit_size)
            s.field(v);
        s.field(bullet.flags);
        s.field(bullet.hit_scale);
        s.field(bullet.alive);
        s.field(bullet.grazed);
    }
    out.sim = s.value();
    return out;
}

bool same_state(const StateSections& a, const StateSections& b) {
    return a.progress == b.progress && a.player == b.player && a.score == b.score && a.battle == b.battle &&
           a.sim == b.sim;
}

struct Fixture {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    bool loaded = false;

    explicit Fixture(const char* event_path = nullptr, i32 bombs = 2, i32 power = 100, bool trance = false,
                     i32 extra_lives = 99, i32 power_step = 100, const char* sht_name = "pl02a.sht",
                     i32 character = 0) {
        player.bombs = bombs;
        player.power = power;
        player.power_step = power_step;
        player.extra_lives = extra_lives;
        progress.character = character;
        if (trance) {
            progress.run_mode_flags |= kTranceActiveFlag;
            progress.ending_points = 600;
        }
        provider.anim_list = &battle.sim.anim_list;
        battle.sim.anm_effects_noop = true;
        loaded = battle.load(provider, "st01.ecl", 1 << 3, host, progress, player, score, event_path, sht_name);
        if (!loaded) std::printf("load failed: %s\n", battle.error().c_str());
    }

    bool step(u32 held, u32 pressed) {
        GameInput input;
        input.held = held;
        input.pressed = pressed;
        const bool ok = battle.step(input);
        if (!ok) std::printf("step failed: %s\n", battle.error().c_str());
        return ok;
    }

    void place_bullet_at_player() {
        battle.invulnerable = 0;
        StageBullet bullet;
        bullet.x = battle.player_x;
        bullet.y = battle.player_y;
        bullet.flags = 0x12;
        bullet.hit_size[0] = bullet.hit_size[1] = 4.0f;
        battle.sim.bullets.push_back(bullet);
    }

    StageEnemy* enemy(u32 serial) {
        for (auto& e : battle.sim.enemies)
            if (e->serial == serial) return e.get();
        return nullptr;
    }

    StageEnemy* vulnerable_enemy() {
        for (int frame = 0; frame < 120; ++frame) {
            for (auto& e : battle.sim.enemies)
                if (!e->dead && e->life > StageBattle::kBombDamagePerFrame && (e->flags & 0x21u) == 0) return e.get();
            if (!step(0, 0)) {
                CHECK(false);
                return nullptr;
            }
        }
        return nullptr;
    }
};

i32 death_power(i32 start, bool capture, i32 step = 100, StateSections* state = nullptr) {
    Fixture f(capture ? kEventPath : nullptr, 2, start, false, 99, step);
    CHECK(f.loaded);
    if (!f.loaded) return -1;
    f.place_bullet_at_player();
    CHECK(f.step(0, 0));
    for (int frame = 0; frame < 12; ++frame)
        CHECK(f.step(0, 0));
    if (state) *state = production_state(f.battle, f.progress, f.player, f.score);
    CHECK_EQ(f.player.deaths, 1);
    return f.player.power;
}

}

TEST(death_power_deducts_step) {
    CHECK_EQ(death_power(300, false), 200);
    CHECK_EQ(death_power(201, false), 101);
    CHECK_EQ(death_power(120, false, 50), 70);
}

TEST(death_power_clamps_at_step) {
    const i32 starts[] = {100, 108, 150, 199, 200};
    for (i32 start : starts)
        CHECK_EQ(death_power(start, false), 100);
    CHECK_EQ(death_power(40, false, 50), 50);
}

TEST(death_power_same_with_and_without_events) {
    const std::array<std::array<i32, 2>, 4> cases = {{{100, 100}, {201, 100}, {300, 100}, {49, 50}}};
    for (const auto& item : cases) {
        StateSections off, on;
        CHECK_EQ(death_power(item[0], false, item[1], &off), death_power(item[0], true, item[1], &on));
        if (!same_state(off, on)) {
            std::printf("state differs start=%d off={%016llx,%016llx,%016llx,%016llx,%016llx} "
                        "on={%016llx,%016llx,%016llx,%016llx,%016llx}\n",
                        item[0], static_cast<unsigned long long>(off.progress),
                        static_cast<unsigned long long>(off.player), static_cast<unsigned long long>(off.score),
                        static_cast<unsigned long long>(off.battle), static_cast<unsigned long long>(off.sim),
                        static_cast<unsigned long long>(on.progress), static_cast<unsigned long long>(on.player),
                        static_cast<unsigned long long>(on.score), static_cast<unsigned long long>(on.battle),
                        static_cast<unsigned long long>(on.sim));
        }
        CHECK(same_state(off, on));
    }
}

TEST(game_clock_advances_without_capture) {
    Fixture f;
    CHECK(f.loaded);
    if (!f.loaded) return;
    CHECK_EQ(f.battle.sim.frame, 0u);
    for (u32 frame = 1; frame <= 16; ++frame) {
        CHECK(f.step(0, 0));
        CHECK_EQ(f.battle.sim.frame, frame);
    }
}

TEST(game_clock_same_with_and_without_events) {
    Fixture off;
    Fixture on(kEventPath);
    CHECK(off.loaded);
    CHECK(on.loaded);
    if (!off.loaded || !on.loaded) return;
    for (u32 frame = 1; frame <= 16; ++frame) {
        const u32 held = frame <= 8 ? 0x01u : 0u;
        CHECK(off.step(held, frame == 1 ? 0x01u : 0u));
        CHECK(on.step(held, frame == 1 ? 0x01u : 0u));
        CHECK_EQ(off.battle.sim.frame, frame);
        CHECK_EQ(on.battle.sim.frame, frame);
        CHECK(same_state(production_state(off.battle, off.progress, off.player, off.score),
                         production_state(on.battle, on.progress, on.player, on.score)));
    }
}

TEST(shot_table_fields_match_original_layout) {
    const char* files[] = {"pl00a.sht", "pl01a.sht", "pl02a.sht", "pl03a.sht"};
    for (const char* name : files) {
        Fixture f(nullptr, 2, 100, false, 99, 100, name);
        CHECK(f.loaded);
        if (!f.loaded) continue;
        CHECK(!f.battle.sht_shots[1].empty());
        if (f.battle.sht_shots[1].empty()) continue;
        const auto& shot = f.battle.sht_shots[1][0];
        CHECK_EQ(shot.damage, 15.0f);
        CHECK_EQ(shot.speed, 24.0f);
        CHECK_EQ(shot.script, 10);
    }
}

TEST(main_shot_uses_record_speed_damage_and_animation) {
    const char* files[] = {"pl00a.sht", "pl01a.sht", "pl02a.sht", "pl03a.sht"};
    for (i32 character = 0; character < 4; ++character) {
        Fixture f(nullptr, 2, 100, false, 99, 100, files[character], character);
        CHECK(f.loaded);
        if (!f.loaded) continue;
        for (u32 frame = 0; frame < 65; ++frame)
            CHECK(f.step(0, 0));
        for (u32 frame = 1; frame <= 2; ++frame)
            CHECK(f.step(0x01u, frame == 1 ? 0x01u : 0u));
        bool found = false;
        for (const auto& shot : f.battle.shots) {
            if (shot.script != 10 || shot.x != f.battle.player_x - 10.01f) continue;
            found = true;
            CHECK_EQ(shot.damage, 15.0f);
            CHECK_EQ(shot.speed, 24.0f);
            CHECK_EQ(shot.y, f.battle.player_y - 8.0f - 24.0f);
        }
        CHECK(found);
    }
}

TEST(bomb_rejected_while_trance_active_stock_unchanged) {
    Fixture f(nullptr, 2, 100, true);
    CHECK(f.loaded);
    if (!f.loaded) return;
    CHECK(f.step(0x02, 0x02));
    CHECK_EQ(f.player.bombs, 2);
}

TEST(bomb_rejected_while_trance_active_no_hud_or_timer) {
    Fixture f(nullptr, 2, 100, true);
    CHECK(f.loaded);
    if (!f.loaded) return;
    CHECK(f.step(0x02, 0x02));
    CHECK_EQ(f.host.bomb_hud_calls, 0);
    CHECK_EQ(f.battle.bomb_timer, 0);
}

TEST(bomb_normal_edge_still_consumes_one_stock) {
    Fixture f(nullptr, 2);
    CHECK(f.loaded);
    if (!f.loaded) return;
    CHECK(f.step(0x02, 0x02));
    CHECK_EQ(f.player.bombs, 1);
    CHECK_EQ(f.host.bomb_hud_calls, 1);
    CHECK(f.battle.bomb_timer > 0);
}

TEST(marisa_bomb_slow_multiplier_starts_on_the_frame_after_the_press) {
    Fixture f(nullptr, 2, 100, false, 99, 100, "pl02a.sht", 1);
    CHECK(f.loaded);
    if (!f.loaded) return;
    for (int i = 0; i < 70; ++i)
        CHECK(f.step(0, 0));
    const i32 before_press = f.battle.pos_fx;
    CHECK(f.step(0x80u, 0x02u));
    const i32 press_frame = f.battle.pos_fx - before_press;
    CHECK_EQ(press_frame, f.battle.speed_orth);
    const i32 before_slow = f.battle.pos_fx;
    CHECK(f.step(0x80u, 0));
    const i32 slow_frame = f.battle.pos_fx - before_slow;
    CHECK_EQ(slow_frame, i32(float(f.battle.speed_orth) * 0.2f));
}

TEST(reimu_bomb_keeps_full_speed_on_the_frame_after_the_press) {
    Fixture f(nullptr, 2, 100, false, 99, 100, "pl02a.sht", 0);
    CHECK(f.loaded);
    if (!f.loaded) return;
    for (int i = 0; i < 70; ++i)
        CHECK(f.step(0, 0));
    const i32 before_press = f.battle.pos_fx;
    CHECK(f.step(0x80u, 0x02u));
    CHECK_EQ(f.battle.pos_fx - before_press, f.battle.speed_orth);
    const i32 before_next = f.battle.pos_fx;
    CHECK(f.step(0x80u, 0));
    CHECK_EQ(f.battle.pos_fx - before_next, f.battle.speed_orth);
}

TEST(bomb_zero_stock_rejected) {
    Fixture f(nullptr, 0);
    CHECK(f.loaded);
    if (!f.loaded) return;
    CHECK(f.step(0x02, 0x02));
    CHECK_EQ(f.player.bombs, 0);
    CHECK_EQ(f.host.bomb_hud_calls, 0);
    CHECK_EQ(f.battle.bomb_timer, 0);
}

TEST(bomb_preserves_stage_controller) {
    Fixture f;
    CHECK(f.loaded);
    if (!f.loaded) return;
    auto* root = f.enemy(1);
    CHECK(root != nullptr);
    if (!root) return;
    CHECK_EQ(root->routine, std::string("main"));
    CHECK_EQ(root->life, 0.0f);
    CHECK(f.step(0x02, 0x02));
    root = f.enemy(1);
    CHECK(root != nullptr);
    if (!root) return;
    CHECK(!root->dead);
    CHECK_EQ(root->life, 0.0f);
    CHECK_EQ(f.player.bombs, 1);
}

TEST(bomb_respects_enemy_damage_gate) {
    Fixture f;
    CHECK(f.loaded);
    if (!f.loaded) return;
    auto* target = f.vulnerable_enemy();
    CHECK(target != nullptr);
    if (!target) return;
    const u32 serial = target->serial;
    const float life = target->life;
    target->flags |= 1u;
    CHECK(f.step(0x02, 0x02));
    target = f.enemy(serial);
    CHECK(target != nullptr);
    if (!target) return;
    CHECK(!target->dead);
    CHECK_EQ(target->life, life);
}

TEST(bomb_still_damages_vulnerable_enemy) {
    Fixture f(nullptr, 2, 100, false, 99, 100, "pl00a.sht", 0);
    CHECK(f.loaded);
    if (!f.loaded) return;
    auto* target = f.vulnerable_enemy();
    CHECK(target != nullptr);
    if (!target) return;
    CHECK(f.step(0, 0));
    const u32 serial = target->serial;
    target->life = 1000.0f;
    target->invulnerable = 0;
    target->position[0] = f.battle.player_x;
    target->position[1] = f.battle.player_y - 30.0f;
    target->vel.pos[0] = target->position[0];
    target->vel.pos[1] = target->position[1];
    const float life = target->life;
    CHECK(f.step(0x02, 0x02));
    target = f.enemy(serial);
    CHECK(target != nullptr);
    if (!target) return;
    CHECK_EQ(target->life, life - 80.0f);
    CHECK(f.step(0, 0));
    target = f.enemy(serial);
    CHECK(target != nullptr);
    if (!target) return;
    CHECK(!target->dead);
    CHECK_EQ(target->life, life - 80.0f);
}

TEST(load_failure_is_a_real_failure) {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    CHECK(!battle.load(provider, "missing.ecl", 1 << 3, host, progress, player, score));
    CHECK(!battle.error().empty());
}

int main() { return th13test::run_all("Stage1 玩家生产状态"); }
