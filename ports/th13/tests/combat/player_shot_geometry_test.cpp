#include "../ecl/ecl_test_fixtures.hpp"

#include "game/StageBattle.hpp"

#include <cmath>
#include <memory>

using namespace th13;

namespace {

class Provider final : public EclResourceProvider {
public:
    Provider() {
        std::vector<u8> main;
        th13test::emit(main, 0, 23, 0, 0xff, 0, {600});
        th13test::emit(main, 0, 10, 0, 0xff, 0, {});
        stage_ = th13test::build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
    }

    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name != "geometry.ecl") return false;
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

struct Host final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};

void check_hit(float ex, float ey, float ew, float eh, float sw, float sh, float angle, bool expected) {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    provider.animations = &battle.sim.anim_list;
    CHECK(battle.load(provider, "geometry.ecl", 2, host, progress, player, score));
    if (!battle.error().empty()) return;

    auto target = std::make_unique<StageEnemy>();
    target->serial = 1;
    target->position[0] = ex;
    target->position[1] = ey;
    target->vel.pos[0] = ex;
    target->vel.pos[1] = ey;
    target->hurtbox[0] = ew;
    target->hurtbox[1] = eh;
    target->life = 100;
    target->tick_gate = false;
    StageEnemy* enemy = target.get();
    battle.sim.enemies.push_back(std::move(target));

    PlayerShot shot;
    shot.width = sw;
    shot.height = sh;
    shot.angle = angle;
    shot.damage = 15;
    shot.alive = true;
    shot.speed = 0;
    battle.shots.push_back(shot);
    GameInput idle;
    CHECK(battle.step(idle));
    CHECK_EQ(score.score, expected ? 1 : 0);
    CHECK_EQ(enemy->life, expected ? 85.0f : 100.0f);
    CHECK_EQ(battle.frame_damage_total, expected ? 15.0f : 0.0f);
    CHECK_EQ(battle.shots.front().alive, true);
    CHECK_EQ(battle.shots.front().state, expected ? 2 : 1);
    if (expected) {
        CHECK_EQ(battle.shots.front().registration_slot, -1);
        enemy->tick_gate = false;
        CHECK(battle.step(idle));
        CHECK_EQ(enemy->life, 85.0f);
        CHECK_EQ(battle.frame_damage_total, 0.0f);
        battle.shots.front().animation_finished = true;
        CHECK(battle.step(idle));
        CHECK(battle.shots.empty());
    }
}

constexpr float kUp = -1.5707963705062866f;

}

TEST(vertical_shot_uses_local_width_along_world_y) {
    check_hit(25.5f, 3.25f, 24, 24, 48, 16, kUp, false);
    check_hit(10.5f, -15.5f, 24, 24, 48, 16, kUp, true);
    check_hit(30.5f, -15.5f, 24, 24, 48, 16, kUp, false);
    check_hit(-4.25f, -34.75f, 24, 24, 48, 16, kUp, true);
    check_hit(21, 0, 24, 24, 48, 16, kUp, false);
    check_hit(0, 37, 24, 24, 48, 16, kUp, false);
}

TEST(rotated_enemy_size_not_shot_size_is_transformed) {
    check_hit(25, 0, 40, 10, 48, 16, kUp, true);
    check_hit(29, 0, 40, 10, 48, 16, kUp, false);
    check_hit(0, 28, 40, 10, 48, 16, kUp, true);
    check_hit(0, 30, 40, 10, 48, 16, kUp, false);
}

TEST(zero_angle_includes_contact_on_each_axis) {
    check_hit(36, 0, 24, 24, 48, 16, 0, true);
    check_hit(-36, 0, 24, 24, 48, 16, 0, true);
    check_hit(0, 20, 24, 24, 48, 16, 0, true);
    check_hit(0, -20, 24, 24, 48, 16, 0, true);
    check_hit(36.25f, 0, 24, 24, 48, 16, 0, false);
    check_hit(0, 20.25f, 24, 24, 48, 16, 0, false);
}

TEST(enemy_invulnerability_expires_after_the_collision_pass) {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    provider.animations = &battle.sim.anim_list;
    CHECK(battle.load(provider, "geometry.ecl", 2, host, progress, player, score));
    auto* enemy = battle.sim.spawn_enemy("main", 0, 100, 100, nullptr, false);
    CHECK(enemy != nullptr);
    if (!enemy) return;
    CHECK_EQ(enemy->invulnerable, 1);
    enemy->tick_gate = false;
    enemy->invulnerable = 1;
    PlayerShot shot;
    shot.alive = true;
    shot.damage = 15;
    shot.x = 0;
    shot.y = 100;
    shot.speed = 0;
    battle.shots.push_back(shot);
    GameInput idle;
    CHECK(battle.step(idle));
    CHECK_EQ(score.score, 1);
    CHECK_EQ(enemy->life, 100.0f);
    CHECK_EQ(enemy->invulnerable, 0);
    battle.shots.push_back(shot);
    CHECK(battle.step(idle));
    CHECK_EQ(score.score, 2);
    CHECK_EQ(enemy->life, 85.0f);
}

int main() { return th13test::run_all("player shot geometry"); }
