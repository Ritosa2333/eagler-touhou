#include "../ecl/ecl_test_fixtures.hpp"

#include "game/ItemManager.hpp"
#include "game/StageBattle.hpp"

#include <cmath>
#include <memory>

using namespace th13;

namespace {

class DummyProvider final : public EclResourceProvider {
public:
    DummyProvider() {
        std::vector<u8> main;
        th13test::emit(main, 0, 500, 0, 0xff, 0, {0x42200000u, 0x42480000u});
        th13test::emit(main, 0, 501, 0, 0xff, 0, {0x41800000u, 0x42000000u});
        th13test::emit(main, 0, 23, 0, 0xff, 0, {600});
        th13test::emit(main, 0, 10, 0, 0xff, 0, {});
        stage_ = th13test::build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
    }

    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name != "dummy.ecl") return false;
        out = stage_;
        return true;
    }
    bool animation(u32, const std::string& name) override {
        animations->push_back(name);
        return true;
    }

    std::vector<std::string>* animations = nullptr;

private:
    std::vector<u8> stage_;
};

struct DummyHost final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};

}

TEST(ecl_501_hitbox_independent_from_hurtbox) {
    DummyProvider provider;
    DummyHost host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    provider.animations = &battle.sim.anim_list;
    CHECK(battle.load(provider, "dummy.ecl", 2, host, progress, player, score));

    StageEnemy* enemy = battle.sim.spawn_enemy("main", 0.0f, 0.0f, 100.0f, nullptr, false);
    CHECK(enemy != nullptr);
    CHECK_EQ(enemy->hurtbox[0], 40.0f);
    CHECK_EQ(enemy->hurtbox[1], 50.0f);
    CHECK_EQ(enemy->hitbox[0], 16.0f);
    CHECK_EQ(enemy->hitbox[1], 32.0f);
}

TEST(enemy_circle_body_collision_hit_and_miss) {
    DummyProvider provider;
    DummyHost host;
    SessionState progress;
    progress.character = 0;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    provider.animations = &battle.sim.anim_list;
    CHECK(battle.load(provider, "dummy.ecl", 2, host, progress, player, score));

    battle.set_player_pos(0.0f, 400.0f);
    battle.invulnerable = 0;
    battle.player_phase = StageBattle::PlayerPhase::Active;

    auto target = std::make_unique<StageEnemy>();
    target->serial = 1;
    target->position[0] = 0.0f;
    target->vel.pos[0] = 0.0f;
    target->vel.pos[1] = 412.0f;
    target->hitbox[0] = 24.0f;
    target->hitbox[1] = 24.0f;
    target->hurtbox[0] = 100.0f;
    target->hurtbox[1] = 100.0f;
    target->position[1] = 412.0f;
    target->flags = 0;
    target->tick_gate = false;
    battle.sim.enemies.push_back(std::move(target));

    GameInput idle;
    CHECK(battle.step(idle));
    CHECK_EQ(int(battle.player_phase), int(StageBattle::PlayerPhase::Deathbomb));
}

TEST(enemy_circle_body_collision_gating) {
    DummyProvider provider;
    DummyHost host;
    SessionState progress;
    progress.character = 0;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    provider.animations = &battle.sim.anim_list;
    CHECK(battle.load(provider, "dummy.ecl", 2, host, progress, player, score));

    battle.set_player_pos(0.0f, 400.0f);
    battle.invulnerable = 0;
    battle.player_phase = StageBattle::PlayerPhase::Active;

    auto target = std::make_unique<StageEnemy>();
    target->serial = 1;
    target->position[0] = 0.0f;
    target->position[1] = 400.0f;
    target->vel.pos[0] = 0.0f;
    target->vel.pos[1] = 400.0f;
    target->hitbox[0] = 24.0f;
    target->hitbox[1] = 24.0f;
    target->flags = 0x20u;
    target->tick_gate = false;
    battle.sim.enemies.push_back(std::move(target));

    GameInput idle;
    CHECK(battle.step(idle));
    CHECK_EQ(int(battle.player_phase), int(StageBattle::PlayerPhase::Active));
}

TEST(enemy_rotated_obb_body_collision) {
    DummyProvider provider;
    DummyHost host;
    SessionState progress;
    progress.character = 0;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    provider.animations = &battle.sim.anim_list;
    CHECK(battle.load(provider, "dummy.ecl", 2, host, progress, player, score));

    battle.set_player_pos(0.0f, 400.0f);
    battle.invulnerable = 0;
    battle.player_phase = StageBattle::PlayerPhase::Active;

    auto target = std::make_unique<StageEnemy>();
    target->serial = 1;
    target->position[0] = 0.0f;
    target->position[1] = 400.0f;
    target->vel.pos[0] = 0.0f;
    target->vel.pos[1] = 400.0f;
    target->flags = 0x8000u;
    target->hitbox[0] = 20.0f;
    target->hitbox[1] = 40.0f;
    target->body_angle = 0.0f;
    target->tick_gate = false;
    battle.sim.enemies.push_back(std::move(target));

    GameInput idle;
    CHECK(battle.step(idle));
    CHECK_EQ(int(battle.player_phase), int(StageBattle::PlayerPhase::Deathbomb));
}

TEST(item_collector_score_wiring_smoke) {
    DummyProvider provider;
    DummyHost host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    provider.animations = &battle.sim.anim_list;
    CHECK(battle.load(provider, "dummy.ecl", 2, host, progress, player, score));

    battle.set_player_pos(0.0f, 400.0f);
    const float pos[3] = {0.0f, 400.0f, 0.0f};
    battle.item_manager.spawn_item(pos, 2, 0.0f, 0.0f);

    GameInput idle;
    CHECK(battle.step(idle));
    CHECK(score.score > 0);
}

int main() { return th13test::run_all("enemy body collision & item wiring"); }
