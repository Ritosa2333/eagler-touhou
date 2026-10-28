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

void check_hit(float ex, float ey, float ew, float eh,
               float sw, float sh, float angle, bool expected) {
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
    target->vel.pos[0] = ex; target->vel.pos[1] = ey;  // 0x419950：position := vel.pos+pos.pos，夹具坐标须写 vel 子位
    target->hurtbox[0] = ew;
    target->hurtbox[1] = eh;
    target->life = 100;
    target->tick_gate = false;  // 已过出生空转帧（0x4183f0:31 置位、0x418e00:20 清位）；本帧须干活，否则 0x419a90:275-308 的本体碰撞/伤害不执行
    StageEnemy* enemy = target.get();
    battle.sim.enemies.push_back(std::move(target));

    PlayerShot shot;
    shot.width = sw;
    shot.height = sh;
    shot.angle = angle;
    shot.damage = 15;
    shot.alive = true;
    shot.speed = 0;  // 静态几何探针：显式停用默认 12 的每帧位移（0x4464d0）
    battle.shots.push_back(shot);
    GameInput idle;
    CHECK(battle.step(idle));
    CHECK_EQ(score.score, expected ? 1 : 0);
    CHECK_EQ(enemy->life, expected ? 85.0f : 100.0f);
    CHECK_EQ(battle.frame_damage_total, expected ? 15.0f : 0.0f);
    CHECK_EQ(battle.shots.front().alive, !expected);
    if (expected) {
        enemy->tick_gate = false;  // 已过出生空转帧；几何夹具没有 ECL 线程，仅观察伤害是否重复。
        CHECK(battle.step(idle));
        CHECK_EQ(enemy->life, 85.0f);
        CHECK_EQ(battle.frame_damage_total, 0.0f);
    }
}

constexpr float kUp = -1.5707963705062866f;

}  // namespace

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
    // 离开 root(0,0) 的重叠区：root 先于本敌机被遍历，会抢先消耗命中弹。
    auto* enemy = battle.sim.spawn_enemy("main", 0, 100, 100, nullptr, false);
    CHECK(enemy != nullptr);
    if (!enemy) return;
    // 出生无敌 2（0x4183f0:44）被构造趟（:65→41a8a0，每 tick 递减）消耗 1：
    // spawn 返回时可观察值为 1。经验依据：th13_01.rpy 全程 RNG 对齐要求
    // 构造趟即扣（5641 GameOver 回归实证过相反安排）。
    CHECK_EQ(enemy->invulnerable, 1);
    enemy->tick_gate = false;  // 已过出生空转帧（0x4183f0:31 置位、0x418e00:20 清位），首步即干活
    enemy->invulnerable = 1;
    PlayerShot shot;
    shot.alive = true;
    shot.damage = 15;
    shot.x = 0; shot.y = 100;  // 静态探针：与被测敌机同位，避开 root
    shot.speed = 0;             // 停用默认 12 的每帧位移（0x4464d0）
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

int main() {
    return th13test::run_all("player shot geometry");
}
