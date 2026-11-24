#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
using namespace th13;
namespace {
struct Provider : EclResourceProvider {
    std::vector<u8> data;
    std::vector<std::string>* names = nullptr;
    Provider() {
        std::vector<u8> bomb800;
        th13test::emit(bomb800, 0, 502, 0, 255, 0, {0x821});
        th13test::emit(bomb800, 0, 23, 0, 255, 0, {1000});
        th13test::emit(bomb800, 0, 10, 0, 255, 0, {});
        std::vector<u8> del;
        th13test::emit(del, 0, 1, 0, 255, 0, {});
        std::vector<u8> delkeep;
        th13test::emit(delkeep, 0, 502, 0, 255, 0, {0x4000000u});
        th13test::emit(delkeep, 0, 23, 0, 255, 0, {1000});
        th13test::emit(delkeep, 0, 10, 0, 255, 0, {});
        std::vector<u8> code;
        th13test::emit(code, 0, 502, 0, 255, 0, {0x20});
        th13test::emit(code, 0, 23, 0, 255, 0, {1000});
        th13test::emit(code, 0, 10, 0, 255, 0, {});
        data = th13test::build_scpt({{"bomb800", bomb800}, {"del", del}, {"delkeep", delkeep}, {"main", code}},
                                    {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string&, std::vector<u8>& out) override {
        out = data;
        return true;
    }
    bool animation(u32, const std::string& name) override {
        names->push_back(name);
        return true;
    }
};
struct Host : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};
struct Rig {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StagePlayerState player_state;
    StageBattle battle{nullptr, &player_state};
    StageEnemy ship;
    int death_sounds = 0, death_effects = 0;
    bool init() {
        provider.names = &battle.sim.anim_list;
        if (!battle.load(provider, "ship.ecl", 2, host, progress, player, score)) return false;
        battle.sim.enemy_position_sound = [&](i32, float) {
            ++death_sounds;
            return true;
        };
        battle.sim.enemy_death_effect = [&](i32, i32, const Vec3&, float) {
            ++death_effects;
            return true;
        };
        ship.life = 200000;
        ship.position[1] = 200;
        ship.hurtbox[0] = ship.hurtbox[1] = 0;
        ship.damage_function = 0;
        ship.death_sound = 36;
        ship.death_effect = 5;
        ship.death_effect_file = 1;
        return true;
    }
    DamageRegistration& registration(float damage, u32 flags) {
        auto& reg = battle.damage_registrations[0];
        reg = DamageRegistration{};
        reg.active = true;
        reg.flags = flags;
        reg.owner = DamageRegistration::Owner::Area;
        reg.center_x = 0;
        reg.center_y = 200;
        reg.width = 64;
        reg.height = 64;
        reg.angle = 0;
        reg.radius = 32;
        reg.damage = damage;
        reg.interval = 1;
        reg.clock.set(4);
        return reg;
    }
    void shot(float x, float y, float damage) {
        PlayerShot s;
        s.alive = true;
        s.x = x;
        s.y = y;
        s.width = s.height = 2;
        s.angle = 0;
        s.damage = damage;
        battle.shots.push_back(s);
    }
};
TEST(bomb_cancel_flag800_rect_registration_kills_enemy_without_side_effects) {
    Rig r;
    CHECK(r.init());
    r.ship.flags |= 0x800u;
    r.registration(30, 0x1u | 0x4u);
    r.shot(0, 200, 13);
    r.battle.sim.enemy_hit(r.ship);
    CHECK(r.ship.dead);
    CHECK_EQ(r.death_sounds, 1);
    CHECK_EQ(r.death_effects, 1);
    CHECK_EQ(r.score.score, 0);
    CHECK_EQ(r.battle.shots[0].state, 1);
    CHECK(r.battle.shots[0].alive);
    CHECK_EQ(r.battle.damage_registrations[0].accumulated_damage, 0);
    CHECK_EQ(r.ship.last_hit_position[0], 0.f);
    CHECK_EQ(r.ship.last_hit_position[1], 0.f);
    CHECK_EQ(r.ship.life, 200000.f);
}
TEST(bomb_cancel_circle_registration_kills_enemy) {
    Rig r;
    CHECK(r.init());
    r.ship.flags |= 0x800u;
    r.registration(30, 0x1u | 0x2u | 0x4u);
    r.battle.sim.enemy_hit(r.ship);
    CHECK(r.ship.dead);
    CHECK_EQ(r.death_sounds, 1);
    CHECK_EQ(r.death_effects, 1);
    CHECK_EQ(r.score.score, 0);
    CHECK_EQ(r.battle.damage_registrations[0].accumulated_damage, 0);
    CHECK_EQ(r.ship.last_hit_position[0], 0.f);
}
TEST(bomb_cancel_rotated_rect_registration_kills_enemy) {
    Rig r;
    CHECK(r.init());
    r.ship.flags |= 0x800u;
    auto& reg = r.registration(30, 0x1u | 0x4u);
    reg.angle = 0.7f;
    r.battle.sim.enemy_hit(r.ship);
    CHECK(r.ship.dead);
    CHECK_EQ(r.death_sounds, 1);
    CHECK_EQ(r.death_effects, 1);
    CHECK_EQ(r.score.score, 0);
}
TEST(bomb_cancel_precedes_0x21_damage_gate) {
    Rig r;
    CHECK(r.init());
    r.ship.flags |= 0x800u | 0x21u;
    r.registration(30, 0x1u | 0x4u);
    r.battle.sim.enemy_hit(r.ship);
    CHECK(r.ship.dead);
    CHECK_EQ(r.death_sounds, 1);
    CHECK_EQ(r.score.score, 0);
    CHECK_EQ(r.ship.life, 200000.f);
}
TEST(no_flag800_same_registration_does_not_cancel) {
    Rig r;
    CHECK(r.init());
    r.registration(30, 0x1u | 0x4u);
    r.shot(0, 200, 13);
    r.battle.sim.enemy_hit(r.ship);
    CHECK(!r.ship.dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.score.score, 0);
    CHECK_EQ(r.battle.shots[0].state, 1);
    CHECK_EQ(r.battle.damage_registrations[0].accumulated_damage, 0);
    CHECK_EQ(r.ship.last_hit_position[0], 0.f);
}
TEST(registration_without_bomb_marker_bit_does_not_cancel) {
    Rig r;
    CHECK(r.init());
    r.ship.flags |= 0x800u;
    r.registration(30, 0x1u);
    r.battle.sim.enemy_hit(r.ship);
    CHECK(!r.ship.dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.score.score, 0);
    CHECK_EQ(r.battle.damage_registrations[0].accumulated_damage, 0);
    CHECK_EQ(r.ship.last_hit_position[0], 0.f);
}
TEST(bomb_registration_missing_geometry_does_not_cancel) {
    Rig r;
    CHECK(r.init());
    r.ship.flags |= 0x800u;
    auto& reg = r.registration(30, 0x1u | 0x4u);
    reg.center_x = 150;
    reg.center_y = -100;
    r.battle.sim.enemy_hit(r.ship);
    CHECK(!r.ship.dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.score.score, 0);
}
TEST(player_phase_clock_not_advanced_blocks_bomb_query) {
    Rig r;
    CHECK(r.init());
    r.player_state.phase_clock.previous = r.player_state.phase_clock.current;
    r.ship.flags |= 0x800u;
    r.registration(30, 0x1u | 0x4u);
    r.battle.sim.enemy_hit(r.ship);
    CHECK(!r.ship.dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.score.score, 0);
}
TEST(registration_clock_not_advanced_skips_bomb_query) {
    Rig r;
    CHECK(r.init());
    r.ship.flags |= 0x800u;
    auto& reg = r.registration(30, 0x1u | 0x4u);
    reg.clock.previous = reg.clock.current;
    r.battle.sim.enemy_hit(r.ship);
    CHECK(!r.ship.dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.score.score, 0);
}
TEST(zero_damage_bomb_registration_does_not_cancel) {
    Rig r;
    CHECK(r.init());
    r.ship.flags |= 0x800u;
    r.registration(0, 0x1u | 0x4u);
    r.battle.sim.enemy_hit(r.ship);
    CHECK(!r.ship.dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.score.score, 0);
}
TEST(plain_player_shot_alone_never_cancels_flag800_enemy) {
    Rig r;
    CHECK(r.init());
    r.ship.flags |= 0x800u;
    r.shot(0, 200, 13);
    r.battle.sim.enemy_hit(r.ship);
    CHECK(!r.ship.dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.score.score, 0);
    CHECK_EQ(r.battle.shots[0].state, 1);
    CHECK(r.battle.shots[0].alive);
    CHECK_EQ(r.ship.last_hit_position[0], 0.f);
    CHECK_EQ(r.ship.last_hit_position[1], 0.f);
}
TEST(spawn_bomb_query_kill_effects_happen_but_enemy_stays_linked_not_dead) {
    Rig r;
    CHECK(r.init());
    auto& reg = r.registration(30, 0x1u | 0x2u | 0x4u);
    reg.center_x = 0;
    reg.center_y = 0;
    reg.radius = 600;
    StageEnemy* e = r.battle.sim.spawn_enemy("bomb800", 0.f, 200.f, 1000.f, nullptr, false);
    CHECK(e != nullptr);
    CHECK(!e->dead);
    CHECK_EQ(r.death_sounds, 1);
    CHECK_EQ(r.death_effects, 1);
    CHECK_EQ(e->life, 1000.f);
    CHECK_EQ(e->age, 0.f);
    CHECK(e->tick_gate);
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));
    CHECK(r.battle.sim.enemies[1].get() == e);
    CHECK((e->flags & 0x821u) == 0x821u);
    CHECK(r.battle.sim.step_frame());
    CHECK(!e->dead);
    CHECK(!e->tick_gate);
    CHECK_EQ(e->age, 0.f);
    CHECK_EQ(r.death_sounds, 1);
    CHECK_EQ(r.death_effects, 1);
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));
    const u32 serial = e->serial;
    const size_t linked = r.battle.sim.enemies.size();
    CHECK(r.battle.sim.step_frame());
    CHECK_EQ(r.death_sounds, 2);
    CHECK_EQ(r.death_effects, 2);
    CHECK_EQ(r.battle.sim.enemies.size(), linked - 1);
    bool present = false;
    for (const auto& entry : r.battle.sim.enemies)
        present |= entry.get() == e;
    CHECK(!present);
}
TEST(spawn_without_bomb_registration_hit_constructs_normally) {
    Rig r;
    CHECK(r.init());
    StageEnemy* e = r.battle.sim.spawn_enemy("bomb800", 0.f, 200.f, 1000.f, nullptr, false);
    CHECK(e != nullptr);
    CHECK(!e->dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK(e->age > 0.f);
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));
    CHECK(r.battle.sim.step_frame());
    CHECK(!e->dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));
    CHECK(r.battle.sim.step_frame());
    CHECK(!e->dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));
    CHECK(r.battle.sim.enemies[1].get() == e);
}
TEST(spawn_ecl_delete_constructor_ignored_return_stays_linked_not_dead) {
    Rig r;
    CHECK(r.init());
    StageEnemy* e = r.battle.sim.spawn_enemy("del", 0.f, 200.f, 1000.f, nullptr, false);
    CHECK(e != nullptr);
    CHECK(!e->dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));
    CHECK(r.battle.sim.enemies[1].get() == e);
    CHECK(r.battle.sim.step_frame());
    CHECK(!e->dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));
    const size_t linked = r.battle.sim.enemies.size();
    CHECK(r.battle.sim.step_frame());
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.battle.sim.enemies.size(), linked - 1);
    bool present = false;
    for (const auto& entry : r.battle.sim.enemies)
        present |= entry.get() == e;
    CHECK(!present);
}
TEST(spawn_explicit_0x4000000_flag_keeps_dead_true_and_unlinks_next_pass) {
    Rig r;
    CHECK(r.init());
    StageEnemy* e = r.battle.sim.spawn_enemy("delkeep", 0.f, 200.f, 1000.f, nullptr, false);
    CHECK(e != nullptr);
    CHECK(e->dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));
    CHECK(r.battle.sim.enemies[1].get() == e);
    const size_t linked = r.battle.sim.enemies.size();
    CHECK(r.battle.sim.step_frame());
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.battle.sim.enemies.size(), linked - 1);
    bool present = false;
    for (const auto& entry : r.battle.sim.enemies)
        present |= entry.get() == e;
    CHECK(!present);
}
}
int main() { return th13test::run_all("enemy bomb cancel (0x419a90 0x800)"); }
