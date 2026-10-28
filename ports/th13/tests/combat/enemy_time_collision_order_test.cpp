#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"

using namespace th13;

namespace {
// H-RNG-SPAWN-330: 00419f3c reads TIME before 0041aab9 advances it.
// At age 60 the original emits one death spirit; at age 61 it emits none.
class TimeProvider final : public EclResourceProvider {
public:
    TimeProvider() {
        std::vector<u8> root, target, watcher;
        th13test::emit(root, 0, 502, 0, 0xff, 0, {0x20});
        th13test::emit(root, 0, 23, 0, 0xff, 0, {600});
        th13test::emit(root, 0, 10, 0, 0xff, 0, {});
        th13test::emit(target, 0, 515, 0, 0xff, 0, {0});
        th13test::emit(target, 0, 42, 1, 0xff, 0, {u32(-9988)});
        th13test::emit(target, 0, 43, 1, 0xff, 0, {u32(-9985)});
        th13test::emit(target, 0, 23, 0, 0xff, 0, {600});
        th13test::emit(target, 0, 10, 0, 0xff, 0, {});
        th13test::emit(watcher, 1, 42, 1, 0xff, 0, {u32(-10000)});
        th13test::emit(watcher, 1, 43, 1, 0xff, 0, {u32(-9985)});
        th13test::emit(watcher, 1, 23, 0, 0xff, 0, {600});
        th13test::emit(watcher, 1, 10, 0, 0xff, 0, {});
        stage = th13test::build_scpt({{"Target", target}, {"Watcher", watcher}, {"main", root}},
                                    {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name != "time.ecl") return false;
        out = stage;
        return true;
    }
    bool animation(u32, const std::string& name) override {
        animations->push_back(name);
        return true;
    }
    std::vector<std::string>* animations = nullptr;
    std::vector<u8> stage;
};

struct TimeHost final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};

struct TimeFixture {
    TimeProvider provider;
    TimeHost host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    GameInput idle;
    bool init() {
        provider.animations = &battle.sim.anim_list;
        return battle.load(provider, "time.ecl", 2, host, progress, player, score);
    }
    StageEnemy* target() {
        return battle.sim.spawn_enemy("Target", 0, 200, 40, nullptr, false);
    }
    void lethal_shot() {
        PlayerShot shot;
        shot.alive = true;
        shot.y = 200;
        shot.angle = 0;
        shot.speed = 0;
        shot.damage = 80;
        battle.shots.push_back(shot);
    }
    int spirits() {
        int count = 0;
        for (const auto& spirit : battle.item_manager.spirits()) count += spirit.alive;
        return count;
    }
};
}  // namespace

TEST(constructor_ecl_observes_zero_time_and_returns_after_timer_tick) {
    TimeFixture fixture;
    CHECK(fixture.init());
    auto* enemy = fixture.target();
    CHECK(enemy != nullptr);
    if (!enemy) return;
    CHECK_EQ(enemy->i_slots[0], 0);
    CHECK_EQ(enemy->age, 1.0f);
    CHECK_EQ(enemy->life, 40.0f);
}

TEST(lethal_collision_at_age_sixty_spawns_before_time_advances) {
    for (i32 age : {60, 61}) {
        TimeFixture fixture;
        CHECK(fixture.init());
        auto* enemy = fixture.target();
        CHECK(enemy != nullptr);
        if (!enemy) continue;
        enemy->age = float(age);
        enemy->invulnerable = 0;
        enemy->tick_gate = false;
        float retired_age = -1;
        const u32 serial = enemy->serial;
        fixture.battle.sim.retire_sink = [enemy, serial, &retired_age](u32 retired) {
            if (retired == serial) retired_age = enemy->age;
        };
        fixture.lethal_shot();
        fixture.battle.sim.random.reseed_restore(60189);
        GameRandom expected(60189);
        if (age == 60) {
            expected.signed_unit();  // 00439170: death spirit angle
            expected.unit();         // 004391a4: death spirit speed
        }
        expected.signed_unit();     // 00417b2a: scatter-table initial angle
        CHECK(fixture.battle.step(fixture.idle));
        CHECK_EQ(retired_age, float(age));  // dead update returns before 0041aab9
        for (const auto& alive : fixture.battle.sim.enemies) CHECK(alive->serial != serial);
        CHECK_EQ(fixture.spirits(), age == 60 ? 1 : 0);
        CHECK_EQ(fixture.battle.sim.random.index(), expected.index());
        CHECK_EQ(fixture.battle.sim.random.seed(), expected.seed());
    }
}

TEST(surviving_linked_enemy_advances_time_after_collision_pass) {
    TimeFixture fixture;
    CHECK(fixture.init());
    auto* enemy = fixture.target();
    CHECK(enemy != nullptr);
    if (!enemy) return;
    enemy->age = 60;
    enemy->tick_gate = false;
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(!enemy->dead);
    CHECK_EQ(enemy->age, 61.0f);
    CHECK_EQ(enemy->life, 40.0f);
}

TEST(constructor_resolves_lethal_shot_before_return_and_time_tick) {
    TimeFixture fixture;
    CHECK(fixture.init());
    fixture.lethal_shot();
    auto* enemy = fixture.target();
    CHECK(enemy != nullptr);
    if (!enemy) return;
    CHECK(enemy->dead);
    CHECK_EQ(enemy->age, 0.0f);
    CHECK_EQ(fixture.spirits(), 2);  // 00419f3c age0: (60-0)/60+1
    CHECK(!fixture.battle.shots.front().alive);
}

TEST(earlier_enemy_death_rng_is_visible_to_later_enemy_ecl) {
    TimeFixture fixture;
    CHECK(fixture.init());
    auto* target = fixture.target();
    auto* watcher = fixture.battle.sim.spawn_enemy("Watcher", 100, 200, 40, nullptr, false);
    CHECK(target != nullptr);
    CHECK(watcher != nullptr);
    if (!target || !watcher) return;
    target->age = 60;
    target->tick_gate = watcher->tick_gate = false;
    fixture.lethal_shot();
    fixture.battle.sim.random.reseed_restore(60189);
    GameRandom expected(60189);
    expected.signed_unit();  // target's death spirit angle
    expected.unit();         // target's death spirit speed
    expected.signed_unit();  // target's scatter table
    const i32 observed = i32(expected.next() & 0x7fffffffu);
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(watcher->i_slots[0], observed);
    CHECK_EQ(fixture.battle.sim.random.index(), expected.index());
    CHECK_EQ(fixture.battle.sim.random.seed(), expected.seed());
}

int main() { return th13test::run_all("enemy TIME and collision order"); }
