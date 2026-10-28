#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include <memory>
using namespace th13;
namespace {
struct Host : StageBattleServices {
    int grazes = 0;
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32 sound) override { if (sound == 42) ++grazes; return true; }
};
struct Provider : EclResourceProvider {
    std::vector<u8> bytes;
    std::vector<std::string>* names = nullptr;
    Provider() {
        std::vector<u8> body;
        th13test::emit(body, 0, 23, 0, 255, 0, {1000});
        th13test::emit(body, 0, 10, 0, 255, 0, {});
        bytes = th13test::build_scpt({{"main", body}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string&, std::vector<u8>& out) override { out = bytes; return true; }
    bool animation(u32, const std::string& name) override { names->push_back(name); return true; }
};
struct Rig {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    Rig() {
        provider.names = &battle.sim.anim_list;
        CHECK(battle.load(provider, "test.ecl", 2, host, progress, player, score));
        battle.sim.enemies.clear();
        battle.set_player_pos(0, 200);
        battle.player_phase = StageBattle::PlayerPhase::Active;
        battle.invulnerable = 0;
        progress.max_point = 1000;
        progress.max_point_cap = 10000;
    }
    StageEnemy& enemy(float x, float y, u32 flags, float age) {
        auto enemy = std::make_unique<StageEnemy>();
        enemy->serial = 1;
        enemy->position[0] = enemy->vel.pos[0] = x;
        enemy->position[1] = enemy->vel.pos[1] = y;
        enemy->hitbox[0] = 24;
        enemy->hitbox[1] = 24;
        enemy->flags = flags;
        enemy->age = age;
        auto* result = enemy.get();
        battle.sim.enemies.push_back(std::move(enemy));
        return *result;
    }
};

TEST(circle_enemy_body_graze_uses_flag_and_six_frame_clock) {
    for (bool enabled : {false, true}) {
        Rig r;
        r.enemy(30, 200, enabled ? 0x200u : 0u, 6.f);
        CHECK(r.battle.step(GameInput{}));
        CHECK_EQ(r.progress.graze, enabled ? 1 : 0);
        CHECK_EQ(r.progress.max_point, enabled ? 1100 : 1000);
        CHECK_EQ(r.host.grazes, enabled ? 1 : 0);
        CHECK(r.battle.player_phase == StageBattle::PlayerPhase::Active);
        CHECK_EQ(r.battle.sim.anm_effects.size(), enabled ? size_t(1) : size_t(0));
        if (enabled && !r.battle.sim.anm_effects.empty()) {
            CHECK_EQ(r.battle.sim.anm_effects[0].x, 0.f);
            CHECK_EQ(r.battle.sim.anm_effects[0].y, 200.f);
        }
    }
}

TEST(enemy_body_graze_remains_live_during_player_invulnerability) {
    Rig r;
    r.enemy(30, 200, 0x200u, 12.f);
    r.battle.invulnerable = 30;
    CHECK(r.battle.step(GameInput{}));
    CHECK_EQ(r.progress.graze, 1);
    CHECK(r.battle.player_phase == StageBattle::PlayerPhase::Active);
}

TEST(enemy_body_protection_blocks_graze_until_its_clock_expires) {
    Rig r;
    auto& e = r.enemy(30, 200, 0x200u, 6.f);
    e.body_invulnerable = 1;
    e.body_invulnerable_clock.set(1);
    CHECK(r.battle.step(GameInput{}));
    CHECK_EQ(r.progress.graze, 0);
    CHECK_EQ(e.body_invulnerable, 0);
    e.age = 12;
    CHECK(r.battle.step(GameInput{}));
    CHECK_EQ(r.progress.graze, 1);
}

TEST(rotated_enemy_body_graze_uses_expanded_native_rectangle) {
    Rig r;
    auto& e = r.enemy(30, 200, 0x8200u, 6.f);
    e.hitbox[0] = 20;
    e.hitbox[1] = 40;
    e.body_angle = 0;
    CHECK(r.battle.step(GameInput{}));
    CHECK_EQ(r.progress.graze, 1);
    CHECK(r.battle.player_phase == StageBattle::PlayerPhase::Active);
}
}
int main() { return th13test::run_all("enemy body graze"); }
