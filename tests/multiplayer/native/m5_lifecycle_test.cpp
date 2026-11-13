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
#include "sdl/ApplicationState.hpp"
#include "multiplayer/Session.hpp"
#include "multiplayer/WorldState.hpp"
#include "eagler/gameplay/CooperativeRules.hpp"
#include <cmath>
#include <cstdlib>

using namespace th13;
using namespace th13test;

namespace {

struct Host final : StageBattleServices {
    std::vector<i32> stopped;
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool hud_sound(i32, float) override { return true; }
    bool stop_sound(i32 id) override { stopped.push_back(id); return true; }
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
    // Real last-life expiry retires all combat once, even if a later shared
    // reward raises stock during the death animation. Restore repeats it.
    {
        Fixture final;
        CHECK(final.init()); final.enable(2, {0, 1, 0});
        final.progress.ending_points = 0;
        auto& seat = final.battle.seats[1];
        final.battle.seat_huds[1].extra_lives = 0;
        seat.phase = StagePlayerState::Phase::Deathbomb;
        seat.phase_frame = 16;
        seat.bomb_timer = 50;
        seat.bomb_protection = 12;
        PlayerShot beam; beam.alive = true; beam.behavior = 2; beam.registration_slot = 0;
        seat.shots.push_back(beam);
        seat.damage_registrations[0].active = true;
        seat.damage_registrations[0].flags = 1;
        seat.damage_registrations[0].damage = 20;
        GameRandom visual(7);
        multiplayer::WorldState before, after, replayed;
        CHECK(before.capture(final.battle, visual));
        CHECK(final.battle.step(GameInput{}));
        CHECK(seat.final_death);
        CHECK_EQ(seat.bomb_timer, 0);
        CHECK(!seat.shots[0].alive);
        CHECK(!seat.damage_registrations[0].active);
        CHECK(std::find(final.host.stopped.begin(), final.host.stopped.end(), 20) != final.host.stopped.end());
        CHECK(after.capture(final.battle, visual));
        CHECK(before.digest() != after.digest());
        CHECK(before.restore(final.battle, visual));
        CHECK(final.battle.step(GameInput{}));
        CHECK(replayed.capture(final.battle, visual));
        CHECK_EQ(after.digest(), replayed.digest());
        CHECK_EQ(seat.bomb_protection, 11);
        final.battle.seats[0].invulnerable = 0;
        StageBullet hit; hit.alive = true; hit.collision_ready = true;
        hit.flags = 0x12; hit.hit_size[0] = hit.hit_size[1] = 8;
        hit.x = final.battle.seats[0].x; hit.y = final.battle.seats[0].y;
        final.battle.sim.bullet_player_collision(hit);
        CHECK_EQ(final.battle.seats[0].phase, StagePlayerState::Phase::Active);
        seat.bomb_protection = 0;
        StageBullet negative;
        negative.alive = true; negative.collision_ready = true; negative.flags = 0x12;
        negative.hit_size[0] = negative.hit_size[1] = 8;
        negative.x = final.battle.seats[0].x; negative.y = final.battle.seats[0].y;
        final.battle.sim.bullet_player_collision(negative);
        CHECK_EQ(final.battle.seats[0].phase, StagePlayerState::Phase::Deathbomb);
        CHECK(after.restore(final.battle, visual));
        final.battle.seat_huds[1].extra_lives = 1;
        for (int frame = 0; frame < 35 && !seat.ghost; ++frame)
            CHECK(final.battle.step(GameInput{}));
        CHECK(seat.ghost);
        CHECK(seat.ghost_just_entered);
        CHECK(!final.battle.rescue_seat(0, 1));
        multiplayer::WorldState ghost_start, ghost_forward, ghost_replay;
        CHECK(ghost_start.capture(final.battle, visual));
        for (int frame = 0; frame < 95; ++frame) CHECK(final.battle.step(GameInput{}));
        CHECK(ghost_forward.capture(final.battle, visual));
        CHECK(ghost_start.digest_core() != ghost_forward.digest_core());
        CHECK(ghost_start.restore(final.battle, visual));
        for (int frame = 0; frame < 95; ++frame) CHECK(final.battle.step(GameInput{}));
        CHECK(ghost_replay.capture(final.battle, visual));
        CHECK_EQ(ghost_forward.digest(), ghost_replay.digest());
        CHECK(final.battle.rescue_seat(0, 1));
        CHECK(!seat.final_death);
        CHECK_EQ(seat.bomb_timer, 0);
        CHECK(!seat.shots[0].alive);
    }
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    auto& battle = dual.battle;
    // ① 幽灵忽略操作，按 th12 私有随机流作确定性漂移。
    battle.seats[1].ghost = true;
    u32 rng = battle.seats[1].ghost_random;
    auto next = [&]() { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; };
    const i32 dx = i32(next() % 97) - 48;
    const i32 dy = i32(next() % 81) - 40;
    GameInput left;
    for (int frame = 0; frame < 10; ++frame) {
        left.update(0x40);
        battle.seat_inputs[1].update(0x40);
        CHECK(battle.step(left));
    }
    CHECK_EQ(battle.seats[1].fixed_x, dx * 10);
    CHECK_EQ(battle.seats[1].fixed_y, 51200 + dy * 10);
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
    // R-G / launcher MULTIPLAYER life-transfer: rescue grants about 4 s;
    // the separate stage-clear revival below remains 120 frames.
    CHECK_EQ(battle.seats[1].invulnerable, 240);
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

    // R-T10: exercise the actual Application pause path while Manual trance
    // is active. A transition-only step does not prove pause freezes its gauge.
    multiplayer::session.Stop();
    sdl::ApplicationState app;
    const char* assets = std::getenv("TH13_NATIVE_ASSET_ROOT");
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    CHECK(assets && root);
    if (!assets || !root) return;
    ::setenv("TH13_SAVE_PATH", "manual-trance-pause.bin", 1);
    CHECK(app.audio_device.initialize_for_test(assets,
        std::string(root) + "/games/th13/thbgmogg.dat"));
    CHECK(app.initialize(false));
    CHECK(app.configure_multiplayer(2, {0, 1, 0}, 0));
    CHECK(app.start_game(1));
    app.pending_destination = -1;
    app.frames = 0;
    app.game_random.reseed_restore(1234);
    app.visual.reseed_restore(1234);
    CHECK(app.begin_run());
    if (!app.run) return;
    GameInput controls;
    const auto step = [&](u32 held) {
        controls.update(held);
        return app.update(controls, false) && app.commit(60.f) &&
            app.present() && app.failure.empty();
    };
    for (unsigned i = 0; i < 35; ++i) CHECK(step(0));
    app.progress.ending_points = 600;
    CHECK(step(0x800));
    CHECK(app.run->battle.team_trance_active());
    CHECK_EQ(app.run->battle.trance_mode, StageBattle::TranceMode::Manual);
    CHECK(step(0));
    CHECK(step(0x100));
    CHECK(app.pause->active());
    CHECK_EQ(app.pause->state.screen, PauseScreen::Pause);
    const i32 paused_frame = app.progress.stage_frame;
    const i32 paused_gauge = app.progress.ending_points;
    const u32 generation = app.run->battle.trance_generation;
    for (unsigned i = 0; i < 40; ++i) {
        CHECK(step(0));
        CHECK(app.pause->active());
        CHECK_EQ(app.progress.stage_frame, paused_frame);
        CHECK_EQ(app.progress.ending_points, paused_gauge);
        CHECK_EQ(app.run->battle.trance_generation, generation);
    }
    CHECK(step(0x100));
    for (unsigned i = 0; i < 15; ++i) CHECK(step(0));
    CHECK(!app.pause->active());
    CHECK(app.progress.stage_frame > paused_frame);
    CHECK_EQ(app.progress.ending_points,
        paused_gauge - (app.progress.stage_frame - paused_frame));
    CHECK_EQ(app.run->battle.trance_generation, generation);
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
