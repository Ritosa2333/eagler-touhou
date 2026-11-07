// 敌机 Bomb 消除定向测试（0x419a90 最前 0x800 支路）。
// 原版依据：0x419a90:90-98 在 0x21 伤害门之前先测敌机 flags bit 0x800，命中
// 即调 0x419b31..0x419b74 的 446870(param5=1) query-only 扫描，命中则走
// 41aad0（死亡演出/掉落，= death_effects_and_drops）并解除敌机。446870
// param5=1 不触发弹回调、不累加 registration.accumulated_damage、不计分、
// 不写 last-hit（enemy+0x27C/0x280）；Bomb 标记独立于伤害，仅来自
// registration flags bit2（Bomb vtable+0xc 四角色全返回 0）。446870 顶部
// 时钟门 = 玩家 +668 vs +664（player_phase_clock_，反汇编已核）。
#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
using namespace th13;
namespace {
struct Provider : EclResourceProvider {
    std::vector<u8> data; std::vector<std::string>* names = nullptr;
    Provider() {
        // 两条 routine 均以 ret(10) 收尾：WaitInteger(23) 靠下一条指令的
        // time 门挂起线程，缺 ret 会在同一步冲到例程尾（step 返回 -1 →
        // 构造趟即 dead）。第 4 参 param_mask 必须为 0：参数是内联字面量，
        // 非零位会被当作 ECL 表达式弹栈解析（空栈 → 0）。rank(第 5 参)=255
        // 对任意难度可执行。build_scpt 顺序 = 字典序（bomb800 < main）：
        // EclProgram 首文件按原序建 definitions，find_index 二分要求有序，
        // 乱序查名失败（0x4797d0 → 主线程立即结束）。
        std::vector<u8> bomb800;
        th13test::emit(bomb800, 0, 502, 0, 255, 0, {0x821});
        th13test::emit(bomb800, 0, 23, 0, 255, 0, {1000});
        th13test::emit(bomb800, 0, 10, 0, 255, 0, {});
        // del：构造趟内 ECL Delete（op1）线程即终（-1）；delkeep：502 显式
        // 置 0x4000000 销毁标记后线程存活。字典序 bomb800 < del < delkeep < main。
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
        data = th13test::build_scpt(
            {{"bomb800", bomb800}, {"del", del}, {"delkeep", delkeep}, {"main", code}},
            {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string&, std::vector<u8>& out) override { out = data; return true; }
    bool animation(u32, const std::string& name) override { names->push_back(name); return true; }
};
struct Host : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};
struct Rig {
    Provider provider; Host host; SessionState progress; HudPlayerState player; HudScoreState score;
    StagePlayerState player_state;                     // 共享自机状态：时钟可直接冻结
    StageBattle battle{nullptr, &player_state};
    StageEnemy ship;
    int death_sounds = 0, death_effects = 0;           // 41aad0 主调用记录
    bool init() {
        provider.names = &battle.sim.anim_list;
        if (!battle.load(provider, "ship.ecl", 2, host, progress, player, score)) return false;
        battle.sim.enemy_position_sound = [&](i32, float) { ++death_sounds; return true; };
        battle.sim.enemy_death_effect =
            [&](i32, i32, const Vec3&, float) { ++death_effects; return true; };
        ship.life = 200000; ship.position[1] = 200;
        ship.hurtbox[0] = ship.hurtbox[1] = 0;
        ship.damage_function = 0;   // 无 +0x40f0 回调：隔离 0x800 支路
        ship.death_sound = 36; ship.death_effect = 5; ship.death_effect_file = 1;
        return true;
    }
    // 手工安装一个 4457e0 damage registration。owner=Area 避开普通扫描的
    // generation-shot 解引用；本套件 normal 伤害路径因 hurtbox 0 +
    // damage_function 0 完全不触发，registration 只被 0x800 query 消费。
    DamageRegistration& registration(float damage, u32 flags) {
        auto& reg = battle.damage_registrations[0];
        reg = DamageRegistration{};
        reg.active = true;
        reg.flags = flags;          // bit0 enabled, bit1 circular, bit2 bomb
        reg.owner = DamageRegistration::Owner::Area;
        reg.center_x = 0; reg.center_y = 200;   // 敌机中心
        reg.width = 64; reg.height = 64; reg.angle = 0;
        reg.radius = 32;
        reg.damage = damage;
        reg.interval = 1;
        reg.clock.set(4);           // current=4 previous=3：已推进且 4%1==0
        return reg;
    }
    void shot(float x, float y, float damage) {
        PlayerShot s; s.alive = true; s.x = x; s.y = y; s.width = s.height = 2;
        s.angle = 0; s.damage = damage;
        battle.shots.push_back(s);
    }
};
// 正控：bit0x800 + 命中矩形 registration（bit2 bomb 标记、时钟已推进）→
// 消除成立：41aad0 恰好主调一次，敌机死亡；param5=1 侧无任何副作用：
// 不计分、不消费在场自机弹、不累加 accumulated_damage、不写 last-hit。
TEST(bomb_cancel_flag800_rect_registration_kills_enemy_without_side_effects) {
    Rig r; CHECK(r.init());
    r.ship.flags |= 0x800u;
    r.registration(30, 0x1u | 0x4u);
    r.shot(0, 200, 13);   // 在场自机弹：0x800 支路不得消费
    r.battle.sim.enemy_hit(r.ship);
    CHECK(r.ship.dead);
    CHECK_EQ(r.death_sounds, 1);
    CHECK_EQ(r.death_effects, 1);
    CHECK_EQ(r.score.score, 0);                                // 不计分
    CHECK_EQ(r.battle.shots[0].state, 1); CHECK(r.battle.shots[0].alive);
    CHECK_EQ(r.battle.damage_registrations[0].accumulated_damage, 0);
    CHECK_EQ(r.ship.last_hit_position[0], 0.f);                // +0x27C 不写
    CHECK_EQ(r.ship.last_hit_position[1], 0.f);
    CHECK_EQ(r.ship.life, 200000.f);                           // query 不入命
}
// 正控：圆形 registration（bit1）同样可消，同一无副作用契约。
TEST(bomb_cancel_circle_registration_kills_enemy) {
    Rig r; CHECK(r.init());
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
// 正控：旋转矩形（angle≠0，敌机居箱心）同样可消。
TEST(bomb_cancel_rotated_rect_registration_kills_enemy) {
    Rig r; CHECK(r.init());
    r.ship.flags |= 0x800u;
    auto& reg = r.registration(30, 0x1u | 0x4u);
    reg.angle = 0.7f;
    r.battle.sim.enemy_hit(r.ship);
    CHECK(r.ship.dead);
    CHECK_EQ(r.death_sounds, 1);
    CHECK_EQ(r.death_effects, 1);
    CHECK_EQ(r.score.score, 0);
}
// 正控：0x419a90:90-98 的 0x800 测试先于 0x21 伤害门，flags 0x821 敌机
// 照样可消（可消性不被 bit0/bit0x20 抑制）。
TEST(bomb_cancel_precedes_0x21_damage_gate) {
    Rig r; CHECK(r.init());
    r.ship.flags |= 0x800u | 0x21u;
    r.registration(30, 0x1u | 0x4u);
    r.battle.sim.enemy_hit(r.ship);
    CHECK(r.ship.dead);
    CHECK_EQ(r.death_sounds, 1);
    CHECK_EQ(r.score.score, 0);
    CHECK_EQ(r.ship.life, 200000.f);
}
// 负控：敌机无 bit0x800 → 同一 registration 不触发消除；normal 路径因
// hurtbox 0 / damage_function 0 完全惰性，任何 side effect 都不应出现。
TEST(no_flag800_same_registration_does_not_cancel) {
    Rig r; CHECK(r.init());               // ship.flags 默认 0
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
// 负控：registration 缺 bit2 bomb 标记（伤害>0 也只算普通 registration，
// Bomb vtable+0xc 不提供标记）→ 不消除。
TEST(registration_without_bomb_marker_bit_does_not_cancel) {
    Rig r; CHECK(r.init());
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
// 负控：几何不命中（registration 在远处）→ 不消除。
TEST(bomb_registration_missing_geometry_does_not_cancel) {
    Rig r; CHECK(r.init());
    r.ship.flags |= 0x800u;
    auto& reg = r.registration(30, 0x1u | 0x4u);
    reg.center_x = 150; reg.center_y = -100;
    r.battle.sim.enemy_hit(r.ship);
    CHECK(!r.ship.dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.score.score, 0);
}
// 负控：玩家 +668 vs +664 时钟未推进（446870 顶部 gate）→ 整个 query
// 直接返回 false，即使 registration 完全有效。
TEST(player_phase_clock_not_advanced_blocks_bomb_query) {
    Rig r; CHECK(r.init());
    r.player_state.phase_clock.previous = r.player_state.phase_clock.current;  // 冻结
    r.ship.flags |= 0x800u;
    r.registration(30, 0x1u | 0x4u);
    r.battle.sim.enemy_hit(r.ship);
    CHECK(!r.ship.dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.score.score, 0);
}
// 负控：registration 自身时钟未推进（current==previous，逐 registration
// gate）→ 该 registration 被跳过，不消除。
TEST(registration_clock_not_advanced_skips_bomb_query) {
    Rig r; CHECK(r.init());
    r.ship.flags |= 0x800u;
    auto& reg = r.registration(30, 0x1u | 0x4u);
    reg.clock.previous = reg.clock.current;
    r.battle.sim.enemy_hit(r.ship);
    CHECK(!r.ship.dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.score.score, 0);
}
// 负控：零伤害 registration（即使命中且带 bomb 位，sum==0）→ 不消除。
TEST(zero_damage_bomb_registration_does_not_cancel) {
    Rig r; CHECK(r.init());
    r.ship.flags |= 0x800u;
    r.registration(0, 0x1u | 0x4u);
    r.battle.sim.enemy_hit(r.ship);
    CHECK(!r.ship.dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.score.score, 0);
}
// 负控：纯自机弹（无任何 registration，弹压在敌机中心）永不触发 0x800
// 消除；弹不被消费、不计分、不写 last-hit。
TEST(plain_player_shot_alone_never_cancels_flag800_enemy) {
    Rig r; CHECK(r.init());
    r.ship.flags |= 0x800u;
    r.shot(0, 200, 13);
    r.battle.sim.enemy_hit(r.ship);
    CHECK(!r.ship.dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.score.score, 0);
    CHECK_EQ(r.battle.shots[0].state, 1); CHECK(r.battle.shots[0].alive);
    CHECK_EQ(r.ship.last_hit_position[0], 0.f);
    CHECK_EQ(r.ship.last_hit_position[1], 0.f);
}
// 正控（native serial2141：帧34260出生即被 query kill、4183f0:65 忽略
// 41a8a0 返回值仍挂链；Web after-tick 统一 raw->dead = (flags & 0x4000000)
// != 0，query kill 不置该位 → 构造后非 dead）：
// 生产 spawn_enemy 构造趟带 bit800 命中有效 Bomb 注册 → 死亡效果发生
// （41aad0 一次），但返回对象非 dead、TIME 未推进（41aab9 尾随死亡门被
// 跳过）、仍挂链且 tick_gate 置位；下一管理器趟新生儿空转（418e00:20 清
// 0x80000 门，无第二次效果）；再下一趟命中才真正致死并当场摘链（418e00
// 销毁支路，第二次 41aad0）。
TEST(spawn_bomb_query_kill_effects_happen_but_enemy_stays_linked_not_dead) {
    Rig r; CHECK(r.init());
    auto& reg = r.registration(30, 0x1u | 0x2u | 0x4u);
    reg.center_x = 0; reg.center_y = 0; reg.radius = 600;  // 圆形大半径：构造点必命中
    StageEnemy* e =
        r.battle.sim.spawn_enemy("bomb800", 0.f, 200.f, 1000.f, nullptr, false);
    CHECK(e != nullptr);
    CHECK(!e->dead);                            // 4183f0 忽略返回：after-tick 重置
    CHECK_EQ(r.death_sounds, 1);                // 死亡效果在构造趟已发生
    CHECK_EQ(r.death_effects, 1);
    CHECK_EQ(e->life, 1000.f);                  // query 不入命
    CHECK_EQ(e->age, 0.f);                      // TIME 未推进（tail 被跳过）
    CHECK(e->tick_gate);                        // 41a8a0:31：待 MGR_UPDATE 清
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));  // root + 新敌机仍挂链
    CHECK(r.battle.sim.enemies[1].get() == e);
    CHECK((e->flags & 0x821u) == 0x821u);       // ECL502 已置 800+21
    // 下一管理器趟：新生儿空转清门，无第二次死亡效果。
    CHECK(r.battle.sim.step_frame());
    CHECK(!e->dead);
    CHECK(!e->tick_gate);                       // 418e00:20 门已清
    CHECK_EQ(e->age, 0.f);                      // 空转不推进
    CHECK_EQ(r.death_sounds, 1);
    CHECK_EQ(r.death_effects, 1);
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));
    // 再下一趟：命中才摘链；对象销毁后只断言链与效果计数。
    const u32 serial = e->serial;
    const size_t linked = r.battle.sim.enemies.size();
    CHECK(r.battle.sim.step_frame());
    CHECK_EQ(r.death_sounds, 2);                // 第二次 41aad0
    CHECK_EQ(r.death_effects, 2);
    CHECK_EQ(r.battle.sim.enemies.size(), linked - 1);  // 当场摘链
    bool present = false;
    for (const auto& entry : r.battle.sim.enemies) present |= entry.get() == e;
    CHECK(!present);
}
// 负控：同一 routine（bit800+0x21）但构造期无有效 Bomb 注册（注册表全空）
// → 构造趟无消除：无死亡效果、tail 完整执行（TIME 推进）、正常挂链；
// 后续管理器趟既不致死也不摘链，敌机常驻。
TEST(spawn_without_bomb_registration_hit_constructs_normally) {
    Rig r; CHECK(r.init());                     // 不安装任何 registration
    StageEnemy* e =
        r.battle.sim.spawn_enemy("bomb800", 0.f, 200.f, 1000.f, nullptr, false);
    CHECK(e != nullptr);
    CHECK(!e->dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK(e->age > 0.f);                        // 构造趟 tail 完整：TIME 推进
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));
    CHECK(r.battle.sim.step_frame());           // 新生儿空转趟（同 root 首趟）
    CHECK(!e->dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));
    CHECK(r.battle.sim.step_frame());           // 工作趟：无注册不消除、不摘链
    CHECK(!e->dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));
    CHECK(r.battle.sim.enemies[1].get() == e);
}
// 正控（native serial176：构造内 ECL Delete 终结主线程，41a8a0 返回死亡，
// 4183f0:65 同样忽略返回仍挂链；after-tick 统一规则下 flags 无 0x4000000
// → 构造后非 dead）：下一管理器趟空转清门，再下一趟主线程已终 → dead →
// 当场摘链（418e00 销毁支路，全程无 41aad0 效果）。
TEST(spawn_ecl_delete_constructor_ignored_return_stays_linked_not_dead) {
    Rig r; CHECK(r.init());
    StageEnemy* e =
        r.battle.sim.spawn_enemy("del", 0.f, 200.f, 1000.f, nullptr, false);
    CHECK(e != nullptr);
    CHECK(!e->dead);                            // Delete 不置 0x4000000 → 重置
    CHECK_EQ(r.death_sounds, 0);                // 无 41aad0：线程终≠死亡演出
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));
    CHECK(r.battle.sim.enemies[1].get() == e);
    CHECK(r.battle.sim.step_frame());           // 新生儿空转趟
    CHECK(!e->dead);
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));
    const size_t linked = r.battle.sim.enemies.size();
    CHECK(r.battle.sim.step_frame());           // 主线程已终 → dead → 摘链
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.battle.sim.enemies.size(), linked - 1);
    bool present = false;
    for (const auto& entry : r.battle.sim.enemies) present |= entry.get() == e;
    CHECK(!present);
}
// 负控（after-tick 规则的另一支）：构造内 ECL 502 显式置 0x4000000 销毁
// 标记 → dead 保留为 true（4183f0 仍挂链），下一管理器趟按 dead 预检当场
// 摘链，无 41aad0 效果。
TEST(spawn_explicit_0x4000000_flag_keeps_dead_true_and_unlinks_next_pass) {
    Rig r; CHECK(r.init());
    StageEnemy* e =
        r.battle.sim.spawn_enemy("delkeep", 0.f, 200.f, 1000.f, nullptr, false);
    CHECK(e != nullptr);
    CHECK(e->dead);                             // 0x4000000：after-tick 保留 dead
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.battle.sim.enemies.size(), size_t(2));
    CHECK(r.battle.sim.enemies[1].get() == e);
    const size_t linked = r.battle.sim.enemies.size();
    CHECK(r.battle.sim.step_frame());           // dead 预检 → 当场摘链（不空转）
    CHECK_EQ(r.death_sounds, 0);
    CHECK_EQ(r.death_effects, 0);
    CHECK_EQ(r.battle.sim.enemies.size(), linked - 1);
    bool present = false;
    for (const auto& entry : r.battle.sim.enemies) present |= entry.get() == e;
    CHECK(!present);
}
}
int main() { return th13test::run_all("enemy bomb cancel (0x419a90 0x800)"); }
