// 对局场景单测：HUD 句柄建立、帧推进/分数逼近、暂停冻结、Esc/失焦请求、
// Stage1 战斗装载（合成 ECL/ANM）与操控步进。
#include "ui_test_fixtures.hpp"
#include "../ecl/ecl_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/RunScene.hpp"
#include "game/FrameScheduler.hpp"

#include <cstring>
#include <map>

using namespace th13;
using namespace th13::test;

namespace {

std::vector<u8> encrypt_text(const std::string& plain) {
    std::vector<u8> out;
    u32 key = 0x77, inc = 0x07;
    for (char ch : plain) {
        out.push_back(u8(u8(ch) ^ key));
        key = (key + inc) & 0xFF;
        inc = (inc + 0x10) & 0xFF;
    }
    out.push_back(u8(key));
    return out;
}

struct InstructionBuilder {
    std::vector<u8> bytes;
    void add(u16 time, u8 opcode, const std::vector<u8>& args = {}) {
        bytes.push_back(u8(time & 0xFF));
        bytes.push_back(u8(time >> 8));
        bytes.push_back(opcode);
        bytes.push_back(u8(args.size()));
        bytes.insert(bytes.end(), args.begin(), args.end());
    }
    void text(u16 time, const std::string& plain) { add(time, 0x11, encrypt_text(plain)); }
    void end(u16 time) {
        add(time, 0);
        bytes.insert(bytes.end(), 4, 0);
    }
};

void append_u32(std::vector<u8>& out, u32 value) {
    for (u32 i = 0; i < 4; i++) out.push_back(u8(value >> (i * 8)));
}

std::vector<u8> build_msg(const std::vector<std::vector<u8>>& entries) {
    std::vector<u8> out;
    append_u32(out, u32(entries.size()));
    u32 offset = 4 + u32(entries.size()) * 8;
    std::vector<u32> offsets;
    for (const auto& entry : entries) {
        offsets.push_back(offset);
        offset += u32(entry.size());
    }
    for (u32 value : offsets) {
        append_u32(out, value);
        append_u32(out, 0x100);
    }
    for (const auto& entry : entries) out.insert(out.end(), entry.begin(), entry.end());
    return out;
}
// 合成 stage ECL：main = wait(600) 后 return（root 结束 → 敌链空转，见
// StageSim::step_frame 空链门）；ANIM 名单两项供装载核验。
std::vector<u8> synthetic_stage_ecl() {
    std::vector<u8> main;
    th13test::emit(main, 0, 23, 0, 0xff, 0, {600});  // wait
    th13test::emit(main, 0, 10, 0, 0xff, 0, {});     // return
    return th13test::build_scpt({{"main", main}}, {"stage_a.anm", "stage_b.anm"});
}

// 合成 st01.std：1 对象（层 0）/1 图元（脚本 0）+ 1 实例 + 相机脚本
// （t=0 位置设置 + stop）——字段布局走查见 game/StageResource.hpp 文件头。
std::vector<u8> synthetic_std() {
    std::vector<u8> out(0x110, 0);
    auto put16 = [&](u32 at, u16 v) { std::memcpy(out.data() + at, &v, 2); };
    auto put32 = [&](u32 at, u32 v) { std::memcpy(out.data() + at, &v, 4); };
    auto putf = [&](u32 at, float v) { std::memcpy(out.data() + at, &v, 4); };
    put16(0, 1);   // 对象数
    put16(2, 1);   // 图元数
    put32(4, 0xd0);  // 实例区
    put32(8, 0xe4);  // 脚本区
    const char* name = "scenery.anm";
    std::memcpy(out.data() + 0x10, name, std::strlen(name));
    put32(0x90, 0x94);  // 对象目录
    // 对象头：id=0/layer=0/flags=1 + 位置 + 尺寸（28 字节）。
    put32(0x94, 0x00010000);
    putf(0x98, 0.f); putf(0x9c, 0.f); putf(0xa0, 0.f);
    putf(0xa4, 64.f); putf(0xa8, 64.f); putf(0xac, 64.f);
    // 图元：type=0/length=28/script=0/pad + 位置 + 宽高。
    put16(0xb0, 0);
    put16(0xb2, 28);
    put16(0xb4, 0);
    put16(0xb6, 0);
    putf(0xb8, 0.f); putf(0xbc, 0.f); putf(0xc0, 0.f);
    putf(0xc4, 64.f); putf(0xc8, 64.f);
    put32(0xcc, 0x0004ffff);  // 图元终止符
    // 实例：object=0/flags=0 + 位置；终止符。
    put32(0xd0, 0);
    putf(0xd4, 0.f); putf(0xd8, 0.f); putf(0xdc, 0.f);
    put32(0xe0, 0xffffffff);
    // 脚本：t=0 ins_2 位置设置（3 参）→ t=0 ins_0 stop → 0xffffffff 终止。
    put32(0xe4, 0); put16(0xe8, 2); put16(0xea, 20);
    putf(0xec, 10.f); putf(0xf0, 20.f); putf(0xf4, 30.f);
    put32(0xf8, 0); put16(0xfc, 0); put16(0xfe, 8);
    for (u32 at = 0x100; at < 0x110; at += 4) put32(at, 0xffffffff);
    return out;
}

struct RunHost final : RunSceneServices {
    AnmManager& manager;
    ZunGraphics* graphics = nullptr;
    int pauses = 0, game_overs = 0;
    std::vector<std::string> lines;
    std::vector<MsgLine> dialogue_lines;
    std::map<std::string, std::vector<u8>> assets;
    explicit RunHost(AnmManager& manager) : manager(manager) {
        assets["st01.ecl"] = synthetic_stage_ecl();
    }
    bool dialogue_line(i32 row, const MsgLine& line, float box_x, float box_y) override {
        (void)row; (void)box_x; (void)box_y;
        dialogue_lines.push_back(line);
        return true;
    }
    bool request_pause() override {
        ++pauses;
        return true;
    }
    bool request_game_over() override {
        ++game_overs;
        return true;
    }
    bool request_practice_end() override {
        return true;
    }
    bool hud_sound(i32) override { return true; }
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool camera(DrawCamera, bool) override { return true; }
    bool hud_text(const HudTextDraw& draw) override {
        lines.push_back(draw.text);
        return true;
    }
    bool read_asset(const std::string& name, std::vector<u8>& bytes,
                    bool required = true) override {
        const auto found = assets.find(name);
        if (found == assets.end()) return false;
        bytes = found->second;
        return true;
    }
    ZunGraphics& stage_graphics() override { return *graphics; }
    bool preload_scenery_animation(i32 bank, const std::string& name) override {
        if (manager.resource(bank)) return true;
        std::vector<u8> bytes;
        return read_asset(name, bytes) && manager.load(bank, bytes.data(), u32(bytes.size()));
    }
};

// ZunGraphics 纯虚桩：对局场景单测只驱动逻辑帧与文本 pass，不落 GPU。
struct NullGraphics final : ZunGraphics {
    touhou::graphics::PipelineState& pipeline() override { return pipeline_state; }
    u32 texture(const AnmResource&, u32) override { return 0; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
    void triangles(u32, const void*, u32) override {}
    void primitives(touhou::graphics::Topology, u32, const void*, u32) override {}
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}

private:
    touhou::graphics::PipelineState pipeline_state;
};

struct RunHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    FrameScheduler scheduler;
    RunHost host{manager};
    std::unique_ptr<RunScene> scene;

    RunHarness() {
        CHECK(load_bank(manager, 3, synthetic_anm(8, trivial_scripts(176))));  // front 0..175（0x425170 灵界计主脚本 171 spawn 172-175）
        CHECK(load_bank(manager, 2, synthetic_anm(8, trivial_scripts(46))));  // ascii（boss 计时数字 2/3）
        CHECK(load_bank(manager, 4, synthetic_anm(8, trivial_scripts(4))));   // stage 合并位
        CHECK(load_bank(manager, 5, synthetic_anm(8, trivial_scripts(1))));   // bullet
        CHECK(load_bank(manager, 6, synthetic_anm(64, trivial_scripts(35))));  // plXX 0..34: options, Bomb roots/backgrounds and portraits.
        host.assets["stage_a.anm"] = synthetic_anm(8, trivial_scripts(1));
        host.assets["stage_b.anm"] = synthetic_anm(8, trivial_scripts(1));
        host.assets["st01.std"] = synthetic_std();
        host.assets["scenery.anm"] = synthetic_anm(8, trivial_scripts(1));  // STD 引用 ANM
        host.graphics = &graphics;
        StagePresentation::Banks banks{4, 5, 6};
        scene = std::make_unique<RunScene>(progress, player, score, manager, renderer,
                                           environment, host, scheduler, banks, 3, 2, 7,
                                           "st01.std");
        const bool initialized = scene->initialize();
        if (!initialized)
            std::printf("run_scene initialize error: %s\n", scene->error.c_str());
        CHECK(initialized);
    }
};
}  // namespace

TEST(run_scene_initializes_hud_handles) {
    RunHarness harness;
    CHECK(harness.scene->hud.root != 0);
    for (u32 i = 0; i < 8; i++) {
        CHECK(harness.scene->hud.life_icons[i] != 0);
        CHECK(harness.scene->hud.bomb_icons[i] != 0);
    }
    CHECK(harness.scene->error.empty());
}

TEST(run_scene_loads_stage_battle) {
    RunHarness harness;
    // ANIM 名单核验（ECL 头声明 → provider 读名）。
    CHECK_EQ(harness.scene->battle.sim.anim_list.size(), 2u);
    CHECK_EQ(harness.scene->battle.sim.anim_list[0], std::string("stage_a.anm"));
    CHECK(harness.scene->error.empty());
}

TEST(run_scene_frame_and_score_convergence) {
    RunHarness harness;
    harness.score.score = 40000;
    const i32 before = harness.progress.stage_frame;
    for (int frame = 0; frame < 200; frame++) {
        const int stepped = harness.scheduler.update();
        if (stepped < 0)
            std::printf("scheduler error at %d: scene=[%s]\n", frame,
                        harness.scene->error.c_str());
        CHECK(stepped >= 0);
    }
    CHECK(harness.progress.stage_frame > before + 100);   // 帧钟推进
    CHECK_EQ(harness.scene->hud.displayed_score, 40000);  // 分数收敛
    CHECK(harness.scene->error.empty());
}

TEST(run_scene_player_controls_and_shots) {
    RunHarness harness;
    // Z 按住 + 左移 60 帧：自机左移、自机弹生成。唯一 GameInput 实例逐帧
    // update（与 BrowserRuntime 同序：先推输入再推帧），边沿只在首帧。
    GameInput in;
    const float start_x = harness.scene->battle.player_x;
    for (int frame = 0; frame < 60; frame++) {
        in.update(0x40 | 0x01);
        harness.scene->controls(in);
        CHECK(harness.scheduler.update() >= 0);
    }
    CHECK(harness.scene->battle.player_x < start_x - 100.0f);
    CHECK(!harness.scene->battle.shots.empty());
    CHECK(harness.scene->error.empty());
}

TEST(run_scene_score_display_precedes_this_frames_battle_awards) {
    // Native42c550 calls42a370 before enemy damage and spell awards. A new
    // score is first visible to convergence on the following update.
    RunHarness harness;
    for (int n = 0; n < 3; ++n) CHECK(harness.scheduler.update() >= 0);
    harness.score.score = harness.scene->hud.displayed_score = 100000;
    harness.progress.high_score = 100000;
    harness.scene->battle.sim.enemy_hit = [&](StageEnemy&) {
        harness.score.score = 200000;
    };
    CHECK(harness.scheduler.update() >= 0);
    CHECK_EQ(harness.score.score,200000);
    CHECK_EQ(harness.scene->hud.displayed_score,100000);
    CHECK_EQ(harness.progress.high_score,100000);
    CHECK(harness.scheduler.update() >= 0);
    CHECK_EQ(harness.scene->hud.displayed_score,103125);
    CHECK_EQ(harness.progress.high_score,103125);
    harness.progress.scene_flags |= 0x10;
    CHECK(harness.scheduler.update() >= 0);
    CHECK(harness.scene->hud.displayed_score > 103125);
}

TEST(run_scene_bomb_consumes_stock_and_clears) {
    RunHarness harness;
    const i32 bombs = harness.player.bombs;
    GameInput in;
    in.update(0x02);  // X 边沿（唯一实例首帧 0→0x02，同 BrowserRuntime 序）
    harness.scene->controls(in);
    CHECK(harness.scheduler.update() >= 0);
    CHECK_EQ(harness.player.bombs, bombs - 1);
    CHECK(harness.scene->battle.bomb_timer > 0);
    CHECK(harness.scene->error.empty());
}

TEST(run_scene_draws_hud_text) {
    RunHarness harness;
    harness.scene->hud.displayed_score = 0;
    CHECK(harness.scheduler.update() >= 0);  // 更新 pass
    // 文本 pass 由 Draw 回调驱动：直接调用一次绘制入口收集字符串。
    harness.host.lines.clear();
    CHECK(harness.scene->draw());
    CHECK(harness.host.lines.size() >= 6);  // hi-score/score/碎片/灵力族
    CHECK(harness.scene->error.empty());
}

TEST(run_scene_draws_dialogue_japanese_and_ruby) {
    InstructionBuilder entry;
    entry.add(0, 12); // Native MSG12 releases ECL519 for the following stage update.
    entry.text(0, "|4,14,\x82\xb1\x82\xb1"); // |4,14,ここ
    entry.text(0, "\x8d\x87\x90\xac\x91\xe5\x8f\xaa"); // 合成大賞
    entry.add(0, 0x0b, {0x58, 0x02, 0, 0}); // opcode 11 的 i32 超时参数 600
    entry.end(10);
    const auto msg_data = build_msg({entry.bytes});

    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    FrameScheduler scheduler;
    RunHost host{manager};

    CHECK(load_bank(manager, 3, synthetic_anm(8, trivial_scripts(176))));
    CHECK(load_bank(manager, 2, synthetic_anm(8, trivial_scripts(46))));
    CHECK(load_bank(manager, 4, synthetic_anm(8, trivial_scripts(4))));
    CHECK(load_bank(manager, 5, synthetic_anm(8, trivial_scripts(1))));
    CHECK(load_bank(manager, 6, synthetic_anm(64, trivial_scripts(22))));
    host.assets["stage_a.anm"] = synthetic_anm(8, trivial_scripts(1));
    host.assets["stage_b.anm"] = synthetic_anm(8, trivial_scripts(1));
    host.assets["st01.std"] = synthetic_std();
    host.assets["scenery.anm"] = synthetic_anm(8, trivial_scripts(1));
    host.assets["st01a.msg"] = msg_data; // 在 initialize 前装入
    host.graphics = &graphics;

    StagePresentation::Banks banks{4, 5, 6};
    progress.stage = 1;
    progress.character = 0;
    auto scene = std::make_unique<RunScene>(progress, player, score, manager, renderer,
                                           environment, host, scheduler, banks, 3, 2, 7,
                                           "st01.std");
    CHECK(scene->initialize());

    // 428820 opens entry then405b80 ends any active Trance.
    progress.ending_points = 277;
    trance_begin(progress);
    scene->open_dialog(0);
    CHECK(scene->dialog_active());
    CHECK(!trance_active(progress));
    CHECK_EQ(progress.ending_points,77);
    CHECK_EQ(scene->battle.invulnerable,70);

    // 原版 0x428ae0 在 update 内写 text.anm；提交必须先于层 26 绘制。
    host.lines.clear();
    host.dialogue_lines.clear();
    GameInput in;
    scene->controls(in);
    CHECK(scheduler.update() >= 0);
    CHECK(scene->dialog_active());
    CHECK(scene->dialog_wait_released());
    // 0041e407 -> 00418090 must observe +0x174 while the dialogue still exists.
    auto& sim = scene->battle.sim;
    sim.current = sim.enemies.front().get();
    EclContext context;
    CHECK_EQ(sim.commands.execute(context, 519), 0);
    CHECK(scene->draw());
    // 应该通过 dialogue_line 服务接收到保留 CP932 原文与 ruby 注音的 MsgLine
    bool found_japanese = false;
    bool found_ruby = false;
    for (const auto& line : host.dialogue_lines) {
        if (line.text_cp932.find("\x8d\x87\x90\xac\x91\xe5\x8f\xaa") != std::string::npos) found_japanese = true;
        if (line.has_ruby && line.ruby_cp932.find("\x82\xb1\x82\xb1") != std::string::npos) found_ruby = true;
    }
    CHECK(found_japanese);
    CHECK(found_ruby);
    CHECK(scheduler.update() >= 0); // MSG interpreter decrements +0x174 on reentry.
    CHECK(scene->dialog_active());
    CHECK(!scene->dialog_wait_released());
    sim.current = sim.enemies.front().get();
    CHECK_EQ(sim.commands.execute(context, 519), 1);
    sim.current = nullptr;
}

TEST(run_scene_ecl_alpha_sees_same_turn_slot_four_birth) {
    RunHarness h;auto& sim=h.scene->battle.sim;auto& enemy=*sim.enemies.front();
    auto* saved=sim.current;sim.current=&enemy;EclContext context;
    const auto op=[&](u16 id,std::initializer_list<u32> args){
        std::vector<u8> code;th13test::emit(code,0,id,0,255,0,args);
        context.instruction=reinterpret_cast<const EclInstruction*>(code.data());
        return sim.commands.execute(context,id);
    };
    CHECK_EQ(op(302,{2}),0);CHECK_EQ(op(303,{4,0}),0);
    CHECK_EQ(op(328,{4,30,0,64}),0);
    auto* vm=h.manager.registry.find(enemy.anm_slot_handle[4]);CHECK(vm);
    if(vm){CHECK_EQ(vm->interpolators.alpha.duration,30);CHECK_EQ(vm->interpolators.alpha.end[0],64);}
    CHECK_EQ(op(329,{4,float_to_bits(-200),float_to_bits(800)}),0);
    if(vm){CHECK_EQ(vm->visual.scale.x,-200.0f);CHECK_EQ(vm->visual.scale.y,800.0f);}
    CHECK(sim.anm_effects.empty());sim.current=saved;
}

TEST(run_scene_freezes_when_paused) {
    RunHarness harness;
    for (int i = 0; i < 10; i++) CHECK(harness.scheduler.update() >= 0);
    const i32 frozen = harness.progress.stage_frame;
    harness.environment.paused = true;  // 暂停覆盖层置位（ApplicationState 同机制）
    for (int i = 0; i < 10; i++) CHECK(harness.scheduler.update() >= 0);
    CHECK_EQ(harness.progress.stage_frame, frozen);
    harness.environment.paused = false;
    CHECK(harness.scheduler.update() >= 0);
    CHECK(harness.progress.stage_frame > frozen);
}

TEST(run_scene_replay_input_is_consumed_only_by_unfrozen_gameplay) {
    RunHarness harness;
    GameInput recorded;
    int reads = 0;
    bool ended = false;
    harness.scene->playback_input = [&]() -> const GameInput* {
        ++reads;
        if (ended) return nullptr;
        recorded.update(0);
        return &recorded;
    };
    const i32 start = harness.progress.stage_frame;
    CHECK(harness.scheduler.update() >= 0);
    CHECK_EQ(reads, 1);
    CHECK_EQ(harness.progress.stage_frame, start + 1);
    harness.environment.paused = true;
    for (int n = 0; n < 10; ++n) CHECK(harness.scheduler.update() >= 0);
    CHECK_EQ(reads, 1);
    harness.environment.paused = false;
    harness.progress.scene_flags |= 0x10;
    CHECK(harness.scheduler.update() >= 0);
    CHECK_EQ(reads, 1);
    harness.progress.scene_flags &= ~0x10u;
    CHECK(harness.scheduler.update() >= 0);
    CHECK_EQ(reads, 2);
    CHECK_EQ(harness.progress.stage_frame, start + 2);
    ended = true;
    CHECK(harness.scheduler.update() >= 0);
    CHECK_EQ(reads, 3);
    CHECK_EQ(harness.progress.stage_frame, start + 2);
}

TEST(run_scene_requests_pause_and_game_over) {
    RunHarness harness;
    harness.scene->request_pause();
    harness.scene->request_game_over();
    CHECK_EQ(harness.host.pauses, 1);
    CHECK_EQ(harness.host.game_overs, 1);
}

TEST(ecl_shake_uses_scheduled_anm_random_and_preserves_overlap_order) {
    RunHarness h;
    auto& sim = h.scene->battle.sim;
    StageEnemy enemy;
    sim.current = &enemy;
    EclContext context;
    std::vector<u8> code;
    th13test::emit(code,0,517,0,255,0,{30,12,0});
    context.instruction = reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(context,517),0);
    sim.current = nullptr;
    h.random.reseed_restore(12345);
    const auto game_index = sim.random.index();
    CHECK_EQ(h.random.index(),0u);
    CHECK(h.scheduler.update()>=0);
    // Independent original45f7c0 sample: duration30/start12/end0, seed12345.
    CHECK_EQ(float_to_bits(h.environment.screen_shake_offset.x),3241777562u);
    CHECK_EQ(float_to_bits(h.environment.screen_shake_offset.y),1094293914u);
    CHECK_EQ(h.random.index(),4u);
    CHECK_EQ(sim.random.index(),game_index);
    h.environment.paused = true;
    const Vec2 frozen = h.environment.screen_shake_offset;
    CHECK(h.scheduler.update()>=0);
    CHECK_EQ(h.random.index(),4u);
    CHECK_EQ(h.environment.screen_shake_offset.x,frozen.x);
    h.environment.paused = false;
    h.progress.scene_flags |= 0x20;
    CHECK(h.scheduler.update()>=0);
    CHECK_EQ(h.random.index(),4u);
    h.progress.scene_flags &= ~0x20u;
    // 470990 inserts new equal-priority callbacks before older ones.
    CHECK(sim.screen_shake(10,100,0));
    GameRandom expected = h.random;
    ScreenShake newer(10,100,0), older(30,12,0);
    older.age.set(1);
    Vec2 offset{};
    CHECK(newer.advance(expected,1,offset));
    CHECK(older.advance(expected,1,offset));
    CHECK(h.scheduler.update()>=0);
    CHECK_EQ(h.random.index(),expected.index());
    CHECK_EQ(h.environment.screen_shake_offset.x,offset.x);
    CHECK_EQ(h.environment.screen_shake_offset.y,offset.y);
    h.scene.reset();
    CHECK(h.scheduler.update()>=0);
    CHECK_EQ(h.random.index(),expected.index());
}

TEST(ecl_shake_completion_keeps_last_offset_without_extra_rng) {
    RunHarness h;
    CHECK(h.scene->battle.sim.screen_shake(3,12,0));
    h.random.reseed_restore(12345);
    CHECK(h.scheduler.update()>=0);
    CHECK(h.scheduler.update()>=0);
    const Vec2 last = h.environment.screen_shake_offset;
    const auto index = h.random.index();
    CHECK(h.scheduler.update()>=0);
    CHECK(h.scheduler.update()>=0);
    CHECK_EQ(h.random.index(),index);
    CHECK_EQ(h.environment.screen_shake_offset.x,last.x);
    CHECK_EQ(h.environment.screen_shake_offset.y,last.y);
}

TEST(run_scene_global_rate_reaches_player_shots_and_animation_without_skipping_inputs) {
    RunHarness h;
    GameInput input;input.held=0x80; // right, fixed-point movement
    h.scene->controls(input);
    const float x=h.scene->battle.player_x;
    const auto frame=h.progress.stage_frame;
    h.scene->battle.sim.set_time_rate(.5f);
    CHECK_EQ(h.progress.rate,.5f);CHECK_EQ(h.manager.rate,.5f);
    PlayerShot shot;shot.x=0;shot.y=200;shot.angle=0;shot.speed=4;
    shot.script=-1;shot.alive=true;
    h.scene->battle.shots.push_back(shot);
    CHECK(h.scene->update());
    CHECK_EQ(h.progress.stage_frame,frame+1);
    CHECK_EQ(h.scene->battle.player_x,float(x+float(h.scene->battle.speed_orth/2)/128));
    CHECK_EQ(h.scene->battle.shots[0].x,2.f);
    CHECK_EQ(h.scene->battle.shots[0].age,0);
    CHECK_EQ(h.scene->battle.shots[0].age_clock.fractional,.5f);
    CHECK(h.scene->update());
    CHECK_EQ(h.progress.stage_frame,frame+2);
    CHECK_EQ(h.scene->battle.shots[0].x,4.f);
    CHECK_EQ(h.scene->battle.shots[0].age,1);
}

TEST(ecl317_flushes_pending_birth_and_defers_interrupt_for_root_and_children) {
    RunHarness h;auto& sim=h.scene->battle.sim;
    const auto data=synthetic_anm(8,{
        {spawn(1),wait(),label(10),AnmIns{402,0,{i32(float_to_bits(3.f)),i32(float_to_bits(3.f))}},wait(),stop()},
        {wait(),label(10),AnmIns{402,0,{i32(float_to_bits(9.f)),i32(float_to_bits(9.f))}},wait(),stop()}
    });
    h.manager.unload(4);CHECK(load_bank(h.manager,4,data));
    StageEnemy* enemy=sim.enemies.front().get();sim.current=enemy;
    enemy->anm_file=2;
    EclContext context;
    const auto op=[&](u16 id,std::initializer_list<u32> args){
        std::vector<u8> bytes;th13test::emit(bytes,0,id,0,255,0,args);
        context.instruction=reinterpret_cast<const EclInstruction*>(bytes.data());
        return sim.commands.execute(context,id);
    };
    CHECK_EQ(op(303,{0,0}),0);CHECK_EQ(op(317,{0,65546}),0);
    const u32 handle=enemy->anm_slot_handle[0];CHECK(handle!=0);
    auto* vm=h.manager.registry.find(handle);CHECK(vm);if(!vm)return;
    auto children=h.manager.registry.children(*vm);CHECK_EQ(children.size(),size_t(1));
    CHECK_EQ(vm->pending_interrupt,10);CHECK_EQ(vm->visual.scale.x,1.f);
    if(children.empty())return;
    CHECK_EQ(children[0]->pending_interrupt,10);CHECK_EQ(children[0]->visual.scale.x,1.f);
    CHECK(h.manager.update());CHECK_EQ(vm->visual.scale.x,3.f);CHECK_EQ(children[0]->visual.scale.x,9.f);
    CHECK_EQ(enemy->anm_slot_handle[0],handle); // no replacement identity
    CHECK_EQ(op(317,{0,65535}),0);CHECK_EQ(vm->pending_interrupt,-1);
    CHECK_EQ(op(317,{u32(StageEnemy::kAnimationSlots-1),10}),0); // no VM is a no-op
    sim.current=nullptr;
}

int main() { return th13test::run_all("对局场景"); }
