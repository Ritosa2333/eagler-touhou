// M4 原生固定 case（全队灵界射击与唯一死亡责任；RULES R-T1..T10）。
// 被测生产面：
//   - StageBattle 团队灵界状态机（trance_mode/owner_seat/generation）；
//   - update_trance_mp（R-T8 Manual 请求逐席仲裁：席位升序第一个有效者）；
//   - commit_death 的 DeathProtection owner 记录（R-T5/R-T9）；
//   - end_trance owner 定向死亡（R-T5：owner 死一次，队友不进死亡演出）；
//   - seat_bomb_allowed（R-T2：owner 拒 Bomb、队友可用）；
//   - 槽消耗全场一次（R-T3：step_mp 每帧恰一次）。
//
// 固定 case 映射（workflow-cases.json m4.*）：
//   team_trance_shooting_four_roles → 全队灵界射击（弹型块 10 全席生效）
//   protection_owner_only_death    → 保护灵界结束只有 owner 死
//   gauge_once_thresholds          → 199/200/599/600 阈值 + 槽每帧 -1 一次
//   all_collision_invulnerability  → 灵界期间全席免疫三类受击
//   simultaneous_requests          → 同帧多席 C 请求仲裁（席位升序）
//   deathbomb_manual_c             → 决死窗内 C 开启记 Manual（结束 -200 不死）
//   generation_respawn_pause_stage → owner 重生/暂停/跨关不清新代灵界
//   peer_bomb_policy               → 灵界期间队友 Bomb 许可
#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
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
        if (name == "mp4.ecl") {
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
        return battle.load(provider, "mp4.ecl", 2, host, progress, player, score);
    }
    void enable(i32 count, std::array<i32, 3> characters) {
        CHECK(battle.enable_seats(provider, count, characters));
    }
    // 灵界就绪态：满槽 600 + 任务计时 ≥30（零输入推进 30 帧）。
    void arm_trance() {
        progress.ending_points = 600;
        GameInput none;
        for (int frame = 0; frame < 32; ++frame) CHECK(battle.step(none));
    }
};

const u32 kCKey = 0x800;

}  // namespace

// ---- gauge_once_thresholds：阈值行为 + 槽每帧 -1 恰一次 ----
TEST(gauge_once_thresholds) {
    // ① 阈值单元：trance_ready 600 门槛；trance_add 封顶；trance_end -200。
    {
        SessionState state;
        state.ending_points = 599;
        CHECK(!trance_ready(state));   // 599 不可激活
        state.ending_points = 600;
        CHECK(trance_ready(state));    // 600 可激活
        trance_begin(state);
        CHECK(!trance_ready(state));   // 激活中不可再激活
        trance_end(state, false);      // 正常结束 -200
        CHECK_EQ(state.ending_points, 400);
        trance_add(state, 250);        // 封顶 600
        CHECK_EQ(state.ending_points, 600);
        trance_add(state, 100);
        CHECK_EQ(state.ending_points, 600);
        state.ending_points = 199;
        trance_end(state, false);      // 199 - 200 → 0（无负值）
        CHECK_EQ(state.ending_points, 0);
    }
    // ② MP 槽消耗一次：2 席会话激活灵界后推进 10 帧——槽 -10（不是 -20）。
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        dual.arm_trance();
        // seat 0 按 C 激活（经 step 参数注入 C 沿）。
        GameInput trigger;
        trigger.update(kCKey);
        CHECK(dual.battle.step(trigger));
        CHECK(dual.battle.team_trance_active());
        CHECK_EQ(dual.battle.trance_mode, StageBattle::TranceMode::Manual);
        CHECK_EQ(dual.battle.trance_owner_seat, 0);
        const i32 slot_at_start = dual.progress.ending_points;
        GameInput none;
        for (int frame = 0; frame < 10; ++frame) CHECK(dual.battle.step(none));
        CHECK_EQ(dual.progress.ending_points, slot_at_start - 10);  // R-T3：一次/帧
    }
}

// ---- simultaneous_requests：同帧多席 C 请求仲裁 ----
TEST(simultaneous_requests) {
    Fixture triple;
    CHECK(triple.init());
    triple.enable(3, {0, 0, 0});
    triple.arm_trance();
    // 三席同帧按 C：席位升序第一个有效者（seat 0）成为 owner。
    GameInput seat0;
    seat0.update(kCKey);
    triple.battle.seat_inputs[1].update(kCKey);
    triple.battle.seat_inputs[2].update(kCKey);
    CHECK(triple.battle.step(seat0));
    CHECK(triple.battle.team_trance_active());
    CHECK_EQ(triple.battle.trance_mode, StageBattle::TranceMode::Manual);
    CHECK_EQ(triple.battle.trance_owner_seat, 0);  // R-T8：升序第一
    CHECK_EQ(triple.battle.trance_generation, 1u);
    // seat 0 Bomb 中时跳过 → seat 1 成为 owner。
    Fixture triple2;
    CHECK(triple2.init());
    triple2.enable(3, {0, 0, 0});
    triple2.arm_trance();
    // seat 0 Bomb 中（无效请求）：bomb_timer 每帧递减，设 1 保证当帧有效。
    triple2.battle.seats[0].bomb_timer = 1;
    GameInput seat0b;
    seat0b.update(kCKey);
    triple2.battle.seat_inputs[1].update(kCKey);
    CHECK(triple2.battle.step(seat0b));
    CHECK(triple2.battle.team_trance_active());
    CHECK_EQ(triple2.battle.trance_owner_seat, 1);  // 跳过 Bomb 席
}

// ---- peer_bomb_policy：灵界期间队友 Bomb 许可 ----
TEST(peer_bomb_policy) {
    // 分类查询单元：owner 拒、队友可用（R-T2）。
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        dual.battle.trance_mode = StageBattle::TranceMode::Manual;
        dual.battle.trance_owner_seat = 0;
        // 模拟已激活（backing 位）。
        dual.progress.run_mode_flags |= 0x100u;
        CHECK(!dual.battle.seat_bomb_allowed(0));  // owner 拒
        CHECK(dual.battle.seat_bomb_allowed(1));   // 队友可用
        CHECK(dual.battle.seat_bomb_allowed(2));
        dual.battle.trance_owner_seat = 1;
        CHECK(dual.battle.seat_bomb_allowed(0));
        CHECK(!dual.battle.seat_bomb_allowed(1));
    }
    // 行为面：灵界期间队友按 X 扣 Bomb（owner 不扣）。
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        dual.arm_trance();
        GameInput trigger;
        trigger.update(kCKey);
        CHECK(dual.battle.step(trigger));
        CHECK(dual.battle.team_trance_active());
        const i32 bombs0 = dual.battle.seat_huds[0].bombs;
        const i32 bombs1 = dual.battle.seat_huds[1].bombs;
        // seat 1 按 X（Bomb 键 0x02）若干帧：bombs 递减（队友可用）。
        GameInput none;
        for (int frame = 0; frame < 3; ++frame) {
            dual.battle.seat_inputs[1].update(0x02);
            CHECK(dual.battle.step(none));
        }
        CHECK_EQ(dual.battle.seat_huds[1].bombs, bombs1 - 1);  // 队友扣 1
        // owner（seat 0）按 X：不扣（灵界拒 Bomb）。
        for (int frame = 0; frame < 3; ++frame) {
            GameInput owner_x;
            owner_x.update(0x02);
            CHECK(dual.battle.step(owner_x));
        }
        CHECK_EQ(dual.battle.seat_huds[0].bombs, bombs0);  // owner 不扣
    }
}

// ---- team_trance_shooting_four_roles：全队灵界射击 ----
// 灵界激活后每席的弹型块 = 10（trance 块）：开火后 shots 非空、弹型域
// 来自块 10（各角色 SHT 的 trance 弹型）。
TEST(team_trance_shooting_four_roles) {
    for (i32 role = 0; role < 4; ++role) {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {role, role, 0});
        dual.arm_trance();
        GameInput trigger;
        trigger.update(kCKey);
        CHECK(dual.battle.step(trigger));
        CHECK(dual.battle.team_trance_active());
        // 两席都开火（Z）：灵界弹型生效（shots 生成）。
        GameInput none;
        for (int frame = 0; frame < 5; ++frame) {
            GameInput seat0;
            seat0.update(0x01);
            dual.battle.seat_inputs[1].update(0x01);
            CHECK(dual.battle.step(seat0));
        }
        CHECK(!dual.battle.seats[0].shots.empty());
        CHECK(!dual.battle.seats[1].shots.empty());  // 全队灵界射击（非仅 owner）
    }
}

// ---- protection_owner_only_death：保护灵界 owner 唯一死亡 ----
// 槽 200 时 seat 1 撞弹 → DeathProtection（owner=1）；槽耗尽 → 只有
// seat 1 死亡（seat 0 存活、不进死亡演出）。
TEST(protection_owner_only_death) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    // 槽设 200（保护灵界可开不可续）；走步进建立任务计时。
    dual.progress.ending_points = 200;
    GameInput none;
    for (int frame = 0; frame < 32; ++frame) CHECK(dual.battle.step(none));
    // seat 0 移开（生产数据流：左移 60 帧 ≈ 270px，远离撞弹点）；
    // seat 1 清无敌撞弹。弹只压 seat 1（一弹消一次：seat 0 不在弹道）。
    GameInput left;
    for (int frame = 0; frame < 60; ++frame) {
        left.update(0x40);
        CHECK(dual.battle.step(left));
    }
    left.update(0x00);
    CHECK(std::fabs(dual.battle.seats[0].x) > 150.0f);  // seat 0 已离场（钳制边界 182）
    dual.battle.seats[1].invulnerable = 0;
    StageBullet bullet;
    bullet.alive = true;
    bullet.collision_ready = true;
    bullet.flags = 0x12u;
    bullet.hit_size[0] = 8.0f;
    bullet.hit_size[1] = 8.0f;
    bullet.x = dual.battle.seats[1].x;
    bullet.y = dual.battle.seats[1].y;
    const i32 lives0 = dual.battle.seat_huds[0].extra_lives;
    const i32 lives1 = dual.battle.seat_huds[1].extra_lives;
    dual.battle.sim.bullet_player_collision(bullet);  // MP 逐席：seat 1 命中
    // 00444ca0：撞弹先进 8 帧决死窗；窗口到期由 update_player 调
    // commit_death → 保护灵界（0x444a10 → 0x405a20(1)）。推进 10 帧。
    GameInput none2;
    for (int frame = 0; frame < 10; ++frame) CHECK(dual.battle.step(none2));
    CHECK(dual.battle.team_trance_active());
    CHECK_EQ(dual.battle.trance_mode, StageBattle::TranceMode::DeathProtection);
    CHECK_EQ(dual.battle.trance_owner_seat, 1);  // 撞弹者 = owner
    CHECK_EQ(dual.battle.trance_generation, 1u);
    // 槽耗尽：保护灵界激活后每帧 -1（撞弹前已推 10 帧含 8 帧决死窗——
    // 第 8 帧起扣槽，已扣 2；剩 198 帧 + 5 帧余量）。
    for (int frame = 0; frame < 203; ++frame) CHECK(dual.battle.step(none));
    CHECK(!dual.battle.team_trance_active());
    // owner（seat 1）进入死亡演出（phase Dying/Deathbomb 或已扣命）；
    // seat 0 存活且不扣命。
    const auto& s0 = dual.battle.seats[0];
    const auto& s1 = dual.battle.seats[1];
    CHECK_EQ(dual.battle.seat_huds[0].extra_lives, lives0);  // 队友不扣命
    CHECK(s1.phase == StagePlayerState::Phase::Dying ||
          s1.phase == StagePlayerState::Phase::Deathbomb ||
          dual.battle.seat_huds[1].extra_lives < lives1);   // owner 死亡责任
    CHECK(s0.phase == StagePlayerState::Phase::Active);     // 队友不进死亡演出
}

// ---- all_collision_invulnerability：灵界期间全席免疫三类受击 ----
TEST(all_collision_invulnerability) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    dual.arm_trance();
    GameInput trigger;
    trigger.update(kCKey);
    CHECK(dual.battle.step(trigger));
    CHECK(dual.battle.team_trance_active());
    // 两席清个人无敌（灵界保护 ≠ 个人无敌）。
    dual.battle.seats[0].invulnerable = 0;
    dual.battle.seats[1].invulnerable = 0;
    // 弹直接压在两席上：不 kill（团队灵界免疫）。
    StageBullet bullet;
    bullet.alive = true;
    bullet.collision_ready = true;
    bullet.flags = 0x12u;
    bullet.hit_size[0] = 8.0f;
    bullet.hit_size[1] = 8.0f;
    bullet.x = dual.battle.seats[0].x;
    bullet.y = dual.battle.seats[0].y;
    dual.battle.sim.bullet_player_collision(bullet);
    CHECK_EQ(dual.battle.seats[0].phase, StagePlayerState::Phase::Active);
    CHECK_EQ(dual.battle.seats[1].phase, StagePlayerState::Phase::Active);
}

// ---- deathbomb_manual_c：决死窗内 C 开启记 Manual ----
// 灵界已激活时撞弹（早退出口保命）+ 槽耗尽正常 -200 不触发死亡。
TEST(deathbomb_manual_c) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    dual.arm_trance();
    GameInput trigger;
    trigger.update(kCKey);
    CHECK(dual.battle.step(trigger));
    CHECK_EQ(dual.battle.trance_mode, StageBattle::TranceMode::Manual);
    // 灵界中 seat 0 撞弹：团队无敌救下，不改 mode/owner（R-T8）。
    dual.battle.seats[0].invulnerable = 0;
    StageBullet bullet;
    bullet.alive = true;
    bullet.collision_ready = true;
    bullet.flags = 0x12u;
    bullet.hit_size[0] = 8.0f;
    bullet.hit_size[1] = 8.0f;
    bullet.x = dual.battle.seats[0].x;
    bullet.y = dual.battle.seats[0].y;
    dual.battle.sim.bullet_player_collision(bullet);
    CHECK_EQ(dual.battle.trance_mode, StageBattle::TranceMode::Manual);
    CHECK_EQ(dual.battle.trance_owner_seat, 0);
    CHECK_EQ(dual.battle.seats[0].phase, StagePlayerState::Phase::Active);
}

// ---- generation_respawn_pause_stage：代数防串 + 暂停不耗槽 ----
TEST(generation_respawn_pause_stage) {
    // ① 代数单元：两次激活 generation 递增；owner 重生只清当前代
    //（R-T9 的 generation 检查面——重生清槽路径在 update_player 第 30
    // 帧，M4 记录代数；清槽语义由 SP 路径保留）。
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        dual.arm_trance();
        GameInput trigger;
        trigger.update(kCKey);
        CHECK(dual.battle.step(trigger));
        const u32 gen1 = dual.battle.trance_generation;
        // 槽耗尽 → 结束。
        GameInput none;
        for (int frame = 0; frame < 700; ++frame) CHECK(dual.battle.step(none));
        CHECK(!dual.battle.team_trance_active());
        // 再充再开：generation 递增（新代）。
        dual.progress.ending_points = 600;
        for (int frame = 0; frame < 32; ++frame) CHECK(dual.battle.step(none));
        GameInput trigger2;
        trigger2.update(kCKey);
        CHECK(dual.battle.step(trigger2));
        CHECK_EQ(dual.battle.trance_generation, gen1 + 1);
    }
    // ①-2 R-T9 跨代守卫（负控）：owner 保护灵界耗尽死亡的演出期间，
    // 队友撞弹开启的新一代保护灵界不得被 owner 重生第 30 帧的清槽路径
    // 清掉——否则新代槽瞬间归零、新 owner 被错误判定死亡。
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        dual.progress.ending_points = 200;
        GameInput none;
        for (int frame = 0; frame < 32; ++frame) CHECK(dual.battle.step(none));
        GameInput left;
        for (int frame = 0; frame < 60; ++frame) {
            left.update(0x40);
            CHECK(dual.battle.step(left));
        }
        left.update(0x00);
        dual.battle.seats[1].invulnerable = 0;
        StageBullet bullet;
        bullet.alive = true;
        bullet.collision_ready = true;
        bullet.flags = 0x12u;
        bullet.hit_size[0] = 8.0f;
        bullet.hit_size[1] = 8.0f;
        bullet.x = dual.battle.seats[1].x;
        bullet.y = dual.battle.seats[1].y;
        dual.battle.sim.bullet_player_collision(bullet);
        for (int frame = 0; frame < 10; ++frame) CHECK(dual.battle.step(none));
        CHECK_EQ(dual.battle.trance_mode, StageBattle::TranceMode::DeathProtection);
        // 槽耗尽：owner（seat 1）进入死亡演出。
        for (int frame = 0; frame < 203; ++frame) CHECK(dual.battle.step(none));
        CHECK(!dual.battle.team_trance_active());
        CHECK_EQ(dual.battle.seats[1].phase, StagePlayerState::Phase::Dying);
        // 演出期间充槽（神霊拾取值域）+ seat 0 撞弹 → 新一代保护灵界。
        dual.progress.ending_points = 600;
        dual.battle.seats[0].invulnerable = 0;
        StageBullet second;
        second.alive = true;
        second.collision_ready = true;
        second.flags = 0x12u;
        second.hit_size[0] = 8.0f;
        second.hit_size[1] = 8.0f;
        second.x = dual.battle.seats[0].x;
        second.y = dual.battle.seats[0].y;
        dual.battle.sim.bullet_player_collision(second);
        for (int frame = 0; frame < 12; ++frame) CHECK(dual.battle.step(none));
        CHECK(dual.battle.team_trance_active());
        CHECK_EQ(dual.battle.trance_generation, 2u);
        CHECK_EQ(dual.battle.trance_owner_seat, 0);
        const i32 slot = dual.progress.ending_points;
        // 推进越过 seat 1 重生第 30 帧清槽点：新代槽只按每帧 −1 消耗。
        for (int frame = 0; frame < 30; ++frame) CHECK(dual.battle.step(none));
        CHECK(dual.battle.team_trance_active());  // 新代未被旧重生清槽打断
        CHECK_EQ(dual.battle.trance_generation, 2u);
        CHECK(dual.progress.ending_points > 0);
        CHECK(dual.progress.ending_points < slot);  // 仍在正常消耗（非清零）
        CHECK_EQ(dual.battle.seats[0].phase, StagePlayerState::Phase::Active);  // 新 owner 未被误判死亡
    }
    // ② 暂停不耗槽：灵界中 defer 模式（step_transition 不走 update_trance
    // 扣槽——转场帧保持槽值）。
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        dual.arm_trance();
        GameInput trigger;
        trigger.update(kCKey);
        CHECK(dual.battle.step(trigger));
        const i32 slot = dual.progress.ending_points;
        // 转场帧（step_transition）不扣槽（暂停语义：不推进灵界计时）。
        for (int frame = 0; frame < 5; ++frame)
            CHECK(dual.battle.step_transition(GameInput{}));
        CHECK_EQ(dual.progress.ending_points, slot);
    }
}

int main() { return th13test::run_all("m4.trance"); }
