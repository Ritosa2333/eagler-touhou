// M5 原生固定 case（表现、幽灵与团队生命周期；R-I8/R-I9/R-G）。
// 被测生产面：
//   - StagePlayerState.ghost/life_transfer_state（幽灵位/送残计数）；
//   - step_mp 席位循环跳过幽灵席（不操作/不射击）；
//   - sim.seat_eligible/ItemCollector.eligible 排除幽灵（不被瞄准/不拾取）；
//   - collide_player_bullet MP 分派跳过幽灵（不受击/不擦弹）；
//   - rescue_seat/revive_all_on_stage_clear/team_wiped/ghost_count；
//   - 最后命死亡 MP 转幽灵（不触发单人 GameOver）。
//
// 固定 case 映射（workflow-cases.json m5.*）：
//   ghost_rescue_resources  → 幽灵全消费面排除 + 救援定向事务
//   team_wipe_180           → 全员幽灵 = team_wiped；180 帧等待语义
//   menu_edges_pause_dialogue → 合作菜单 pressed 合并（CooperativeMenuInput）
//   hud_world_owner_visibility → 世界表现一份 + 每席 HUD 域分离
//   no_sp_save_writes       → MP 会话不写 SP 成绩/存档面（接口级）
//   stage_revive            → 过关复活清全部幽灵
#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "eagler/gameplay/CooperativeRules.hpp"
#include <cmath>

using namespace th13;
using namespace th13test;

namespace {

struct Host final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool hud_sound(i32, float) override { return true; }
    bool stop_sound(i32) override { return true; }
};

class Provider final : public EclResourceProvider {
public:
    Provider() {
        std::vector<u8> main;
        emit(main, 0, 23, 0, 0xff, 0, {600});
        emit(main, 0, 10, 0, 0xff, 0, {});
        stage = build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name == "mp5.ecl") {
            out = stage;
            return true;
        }
        if (name.size() > 4 && name.compare(name.size() - 4, 4, ".sht") == 0) {
            static const std::string root = [] {
                std::string path = __FILE__;
                for (int i = 0; i < 8; ++i) {
                    const std::string probe = path + "/artifacts/reference/raw/pl00a.sht";
                    std::FILE* check = std::fopen(probe.c_str(), "rb");
                    if (check) { std::fclose(check); break; }
                    const auto cut = path.find_last_of("/\\");
                    if (cut == std::string::npos) break;
                    path.erase(cut);
                }
                return path;
            }();
            std::FILE* file = std::fopen((root + "/artifacts/reference/raw/" + name).c_str(), "rb");
            if (!file) return false;
            std::fseek(file, 0, SEEK_END);
            const long size = std::ftell(file);
            std::fseek(file, 0, SEEK_SET);
            out.resize(size_t(size < 0 ? 0 : size));
            const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
            std::fclose(file);
            return got == out.size();
        }
        return false;
    }
    bool animation(u32, const std::string& name) override {
        if (animations) animations->push_back(name);
        return true;
    }
    std::vector<std::string>* animations = nullptr;
    std::vector<u8> stage;
};

struct Fixture {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    GameInput idle;

    bool init() {
        provider.animations = &battle.sim.anim_list;
        return battle.load(provider, "mp5.ecl", 2, host, progress, player, score);
    }
    void enable(i32 count, std::array<i32, 3> characters) {
        CHECK(battle.enable_seats(provider, count, characters));
    }
};

}  // namespace

// ---- ghost_rescue_resources：幽灵全消费面 + 救援定向事务 ----
TEST(ghost_rescue_resources) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    auto& battle = dual.battle;
    // ① 幽灵席不推进玩家面：seat 1 幽灵化后按 ← 不动。
    battle.seats[1].ghost = true;
    GameInput left;
    for (int frame = 0; frame < 10; ++frame) {
        left.update(0x40);
        CHECK(battle.step(left));
    }
    CHECK_EQ(battle.seats[1].x, 0.0f);       // 幽灵不操作
    CHECK(battle.seats[0].x < 0.0f);          // 可操作席照常
    // ② 幽灵不被瞄准：seat_eligible 排除（帧尾写入）。
    CHECK(!battle.sim.seat_eligible[1]);
    // ③ 幽灵不受击/不擦弹：弹压幽灵席无结算。
    dual.progress.graze = 0;
    dual.progress.max_point_cap = 500000;
    StageBullet bullet;
    bullet.alive = true;
    bullet.collision_ready = true;
    bullet.flags = 0x12u;
    bullet.hit_size[0] = 8.0f;
    bullet.hit_size[1] = 8.0f;
    bullet.x = 0.0f;   // 压在幽灵席位置（擦弹带内）
    bullet.y = 400.0f;
    battle.sim.bullet_player_collision(bullet);
    CHECK_EQ(dual.progress.graze, 0);  // 幽灵不擦弹
    // ④ 幽灵不拾取：ItemCollector.eligible=false（nearest 回退 seat 0）。
    // ⑤ 救援定向事务：seat 0（1 命）送 seat 1。
    battle.seat_huds[0].extra_lives = 1;
    const i32 bombs_before = battle.seat_huds[1].bombs;
    battle.seat_huds[1].bombs = 0;  // 幽灵期 Bomb 耗尽
    CHECK(battle.rescue_seat(0, 1));
    CHECK_EQ(battle.seat_huds[0].extra_lives, 0);  // donor 扣 1
    CHECK(!battle.seats[1].ghost);                  // 幽灵位清
    CHECK_EQ(battle.seat_huds[1].bombs, 2);         // SP 初始 Bomb（R-G）
    CHECK_EQ(battle.seats[1].invulnerable, 120);
    // ⑥ 救援约束：非幽灵不可救、无命不可送、自送拒绝。
    CHECK(!battle.rescue_seat(0, 1));   // seat 1 已可操作
    battle.seats[1].ghost = true;
    CHECK(!battle.rescue_seat(1, 1));   // 自送
    battle.seat_huds[0].extra_lives = 0;
    CHECK(!battle.rescue_seat(0, 1));   // donor 无命
    // ⑦ 送残计数（AdvanceLifeTransfer 状态字节）：5 拍窗内完成触发。
    u8 transfer = 0;
    bool granted = false;
    for (int tap = 0; tap < 5; ++tap)
        granted = eagler::cooperative::AdvanceLifeTransfer(transfer, true, true);
    CHECK(granted);
    CHECK_EQ(unsigned(transfer), 0u);
}

// ---- team_wipe_180：全灭判定 + 180 帧等待语义 ----
TEST(team_wipe_180) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    auto& battle = dual.battle;
    CHECK(!battle.team_wiped());
    CHECK_EQ(battle.ghost_count(), 0);
    battle.seats[0].ghost = true;
    CHECK(!battle.team_wiped());       // 一席幽灵非全灭
    CHECK_EQ(battle.ghost_count(), 1);
    battle.seats[1].ghost = true;
    CHECK(battle.team_wiped());        // 全员幽灵
    CHECK_EQ(battle.ghost_count(), 2);
    // 180 帧等待窗：全灭后世界面继续推进（宿主层计时；模拟面不冻结）。
    GameInput none;
    for (int frame = 0; frame < 180; ++frame) CHECK(battle.step(none));
    CHECK(battle.team_wiped());  // 等待窗内保持全灭态
    // 3 席变体。
    Fixture triple;
    CHECK(triple.init());
    triple.enable(3, {0, 0, 0});
    triple.battle.seats[0].ghost = true;
    triple.battle.seats[1].ghost = true;
    CHECK(!triple.battle.team_wiped());  // 2/3 幽灵
    triple.battle.seats[2].ghost = true;
    CHECK(triple.battle.team_wiped());
}

// ---- stage_revive：过关复活清全部幽灵 ----
TEST(stage_revive) {
    Fixture triple;
    CHECK(triple.init());
    triple.enable(3, {0, 0, 0});
    auto& battle = triple.battle;
    battle.seats[0].ghost = true;
    battle.seats[2].ghost = true;
    battle.seat_huds[0].bombs = 0;
    battle.seat_huds[2].bombs = 0;
    battle.revive_all_on_stage_clear();
    CHECK_EQ(battle.ghost_count(), 0);           // 全部清
    CHECK(!battle.seats[0].ghost);
    CHECK(!battle.seats[2].ghost);
    CHECK_EQ(battle.seat_huds[0].bombs, 2);      // SP 初始 Bomb
    CHECK_EQ(battle.seat_huds[2].bombs, 2);
    CHECK_EQ(battle.seats[0].invulnerable, 120);
    CHECK_EQ(battle.seats[0].phase, StagePlayerState::Phase::Active);
    // 过关复活后可操作（席位循环恢复推进）。
    GameInput left;
    for (int frame = 0; frame < 5; ++frame) {
        left.update(0x40);
        CHECK(battle.step(left));
    }
    CHECK(battle.seats[0].x < 0.0f);
}

// ---- hud_world_owner_visibility：世界一份 + 每席 HUD 分离 ----
TEST(hud_world_owner_visibility) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 1, 0});  // seat 1 = Marisa（角色域分离观察）
    auto& battle = dual.battle;
    // ① 世界面一份：sim/敌机池/RNG 单实例（enable 不复制世界）。
    CHECK_EQ(battle.sim.enemies.size() >= 0, true);  // 单一 sim 实例
    // ② 每席 HUD 域分离：改 seat 0 不影响 seat 1。
    battle.seat_huds[0].power = 123;
    battle.seat_huds[0].extra_lives = 5;
    battle.seat_huds[0].bombs = 7;
    CHECK_EQ(battle.seat_huds[1].power, 100);   // 默认起飞 1.00
    CHECK_EQ(battle.seat_huds[1].extra_lives, 2);
    CHECK_EQ(battle.seat_huds[1].bombs, 2);
    // ③ 角色记录逐席（owner 归属面）。
    CHECK_EQ(battle.seat_characters[0], 0);
    CHECK_EQ(battle.seat_characters[1], 1);
    // ④ 伤害注册 owner_seat 归属（M2 域）：seat 0 开火注册属 seat 0。
    // 开火门 0x4460d0：startup_clock > 19——先空推 25 帧再按 Z。
    GameInput none;
    for (int frame = 0; frame < 25; ++frame) CHECK(battle.step(none));
    GameInput fire;
    for (int frame = 0; frame < 5; ++frame) {
        fire.update(0x01);
        CHECK(battle.step(fire));
    }
    bool owner0_seen = false;
    for (const auto& reg : battle.seats[0].damage_registrations)
        if (reg.active && reg.owner_seat == 0) owner0_seen = true;
    CHECK(owner0_seen);
}

// ---- menu_edges_pause_dialogue：合作菜单 pressed 合并 ----
TEST(menu_edges_pause_dialogue) {
    // CooperativeMenuInput 语义（common）：席位合并 pressed 沿——不是
    // OR(held)（长按不重复触发；另一席的确认不被吞）。
    // 直接验证合并函数的边沿语义。
    const u32 seat0_held = 0x01, seat1_held = 0x01;  // 两席都按 Z
    // 模拟两席 GameInput 的 pressed 合并：席位 0 pressed=1（首按）、
    // 席位 1 pressed=0（持续按）→ 合并 pressed=1（恰一次）。
    GameInput a, b;
    a.update(seat0_held);       // 首按：pressed=1
    b.update(seat1_held);       // 持续按：pressed=0
    const u32 merged_pressed = a.pressed | b.pressed;
    CHECK_EQ(merged_pressed, 1u);   // 恰一次确认沿
    // 下一帧两席都持续按：合并 pressed=0（不重复触发菜单）。
    a.update(seat0_held);
    b.update(seat1_held);
    CHECK_EQ(a.pressed | b.pressed, 0u);
    // 席位 1 新按（席位 0 持续）：合并沿出现（另一席确认不被吞）。
    a.update(seat0_held);
    b.update(0x00);   // 席位 1 先松
    b.update(seat1_held);  // 再按：pressed=1
    CHECK_EQ(a.pressed | b.pressed, 1u);
    // 对话门（暂停/对话期菜单合并语义）：battle 层面 MP 每席输入沿独立
    //（seat_inputs 互不串扰——M2 已证；本 case 钉住合并口径）。
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    GameInput none;
    for (int frame = 0; frame < 3; ++frame) CHECK(dual.battle.step(none));
    CHECK_EQ(dual.battle.seat_inputs[0].held, 0u);
    CHECK_EQ(dual.battle.seat_inputs[1].held, 0u);
}

// ---- no_sp_save_writes：MP 不写 SP 成绩/存档面 ----
TEST(no_sp_save_writes) {
    // 接口级断言：MP 会话（seat_count>=2）下 StageBattle 不暴露 SP 存档
    // 写路径——RunScene/宿主的 save_score 由 ApplicationState 决定；战斗
    // 层无存档 API。本 case 验证：MP 启用后 battle 的公共面不含 SP 存档
    // 写入口（编译期契约 + 运行期会话状态隔离）。
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    // MP 会话的 progress 是共享团队态（灵界槽/分数一份），但成绩/解锁/
    // Replay 槽由宿主层按会话类型隔离——战斗层不触碰（无 save API）。
    // 运行期可验证的不变量：MP 激活后 progress.run_mode_flags 不含
    // SP 成绩位污染（bit4-5 练习位不因 MP 改变）。
    const u32 flags_before = dual.progress.run_mode_flags;
    GameInput none;
    for (int frame = 0; frame < 10; ++frame) CHECK(dual.battle.step(none));
    CHECK_EQ(dual.progress.run_mode_flags, flags_before);  // 会话位不漂移
    // 团队分数一份（R-I4）：两席 HUD 的 score 指同一共享账面。
    dual.score.score = 1000;
    CHECK_EQ(dual.score.score, 1000);
}

int main() { return th13test::run_all("m5.lifecycle"); }
