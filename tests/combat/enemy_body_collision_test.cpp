#include "../ecl/ecl_test_fixtures.hpp"

#include "game/StageBattle.hpp"
#include "game/ItemManager.hpp"

#include <cmath>
#include <memory>

using namespace th13;

namespace {

class DummyProvider final : public EclResourceProvider {
public:
    DummyProvider() {
        std::vector<u8> main;
        // ECL 指令 500: setHurtbox(40.0, 50.0) -> bitcast to u32: 0x42200000, 0x42480000
        // ECL 指令 501: setHitbox(16.0, 32.0)  -> bitcast to u32: 0x41800000, 0x42000000
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

}  // namespace

// 测试 1: ECL 500 与 501 独立写点，hurtbox 与 hitbox 相互独立
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

// 测试 2: 敌机本体圆形碰撞（00444140 原版规则）
TEST(enemy_circle_body_collision_hit_and_miss) {
    DummyProvider provider;
    DummyHost host;
    SessionState progress;
    progress.character = 0; // Reimu, player_hit_size = 2.0
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    provider.animations = &battle.sim.anim_list;
    CHECK(battle.load(provider, "dummy.ecl", 2, host, progress, player, score));

    // 自机位置在 (0, 400)
    battle.set_player_pos(0.0f, 400.0f);
    battle.invulnerable = 0;
    battle.player_phase = StageBattle::PlayerPhase::Active;

    auto target = std::make_unique<StageEnemy>();
    target->serial = 1;
    target->position[0] = 0.0f;
    target->vel.pos[0] = 0.0f; target->vel.pos[1] = 412.0f;  // 0x419950：position := vel.pos+pos.pos，夹具坐标须写 vel 子位
    // hitbox[0] = 24.0f -> 半径 sx = 12.0f
    // p = 2.0f -> p^2 + sx^2 = 4 + 144 = 148. sqrt(148) ≈ 12.1655
    target->hitbox[0] = 24.0f;
    target->hitbox[1] = 24.0f;
    target->hurtbox[0] = 100.0f; // 很大，证明不影响本体碰撞
    target->hurtbox[1] = 100.0f;
    target->position[1] = 412.0f; // 12.0^2 = 144 < 148 -> 命中！
    target->flags = 0;
    target->tick_gate = false;  // 已过出生空转帧（0x4183f0:31 置位、0x418e00:20 清位）；本帧须干活，否则 0x419a90:275-308 的本体碰撞/伤害不执行
    battle.sim.enemies.push_back(std::move(target));

    GameInput idle;
    CHECK(battle.step(idle));
    // 命中自机，进入决死状态
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
    target->position[1] = 400.0f; // 就在自机脸上
    target->vel.pos[0] = 0.0f; target->vel.pos[1] = 400.0f;  // 同上：0x419950 累加语义
    target->hitbox[0] = 24.0f;
    target->hitbox[1] = 24.0f;
    // 门控 flags: 0x2 (无敌/不可伤), 0x20, 0x8000000
    target->flags = 0x20u; // 应该被门控跳过
    target->tick_gate = false;  // 已过出生空转帧（0x4183f0:31 置位、0x418e00:20 清位）；本帧须干活，否则 0x419a90:275-308 的本体碰撞/伤害不执行
    battle.sim.enemies.push_back(std::move(target));

    GameInput idle;
    CHECK(battle.step(idle));
    // 门控生效，玩家不受伤害
    CHECK_EQ(int(battle.player_phase), int(StageBattle::PlayerPhase::Active));
}

// 测试 3: 敌机本体旋转矩形碰撞（0x8000 与 00444260 规则）
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
    target->vel.pos[0] = 0.0f; target->vel.pos[1] = 400.0f;  // 同上：0x419950 累加语义
    target->flags = 0x8000u; // OBB 模式
    target->hitbox[0] = 20.0f; // 宽度 20 (half = 10)
    target->hitbox[1] = 40.0f; // 长度 40
    target->body_angle = 0.0f;
    target->tick_gate = false;  // 已过出生空转帧（0x4183f0:31 置位、0x418e00:20 清位）；本帧须干活，否则 0x419a90:275-308 的本体碰撞/伤害不执行
    battle.sim.enemies.push_back(std::move(target));

    GameInput idle;
    CHECK(battle.step(idle));
    // 命中自机，进入决死状态
    CHECK_EQ(int(battle.player_phase), int(StageBattle::PlayerPhase::Deathbomb));
}

// 测试 4: collector.score 接线验证，防止点道具/满P空指针写
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
    // 生成一个点道具在玩家收集盒内 (400, 400)
    const float pos[3] = {0.0f, 400.0f, 0.0f};
    battle.item_manager.spawn_item(pos, 2, 0.0f, 0.0f); // type 2 = Point

    GameInput idle;
    // step 中会执行 update_item_side()，此前 collector.score 未赋值会导致空指针崩溃
    CHECK(battle.step(idle));
    // 道具被收集，score 增加且无崩溃
    CHECK(score.score > 0);
}

int main() {
    return th13test::run_all("enemy body collision & item wiring");
}
