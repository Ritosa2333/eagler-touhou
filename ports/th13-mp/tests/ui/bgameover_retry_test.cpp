// 结算/Retry 专用窄测（新增；不改既有 tests/ui）：
// 1) 0x43f570 mode≠0 → 0x440960 态 0x18 的 BGameOver 三项菜单：标题/录像/
//    重试，默认光标 2（0x15030(2)），没有 Continue（不能 Esc/继续复活）；
// 2) mode0 普通 GameOver 的 continue_stock 门保持（be844<1 才禁用继续）；
// 3) 重试/回标题经 RunPausePlatform::restart_run/return_to_title；
// 4) RunScene 符卡超时（00428820 -2）只调用 host.request_game_over，不再预写
//    PostGameFlow 内部 BGameOver（宿主按 run_mode_flags 路由）。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/FrameScheduler.hpp"
#include "game/RunPause.hpp"
#include "game/RunScene.hpp"

using namespace th13;
using namespace th13::test;

namespace {

// ZunGraphics 纯虚桩：只构造场景对象，不落 GPU。
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

struct SettlementHost final : RunPausePlatform {
    int restarts = 0, titles = 0, continues = 0, slot_saves = 0;
    bool save_available = true;
    bool sound(i32) override { return true; }
    bool suspend_music() override { return true; }
    bool resume_music() override { return true; }
    bool restart_run() override {
        ++restarts;
        return true;
    }
    bool return_to_title() override {
        ++titles;
        return true;
    }
    bool continue_run() override {
        ++continues;
        return true;
    }
    bool can_save_replay() const override { return save_available; }
    bool save_replay_slot(i32, const std::string&) override {
        ++slot_saves;
        return true;
    }
};

struct SettlementHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    FrameScheduler scheduler;
    SettlementHost host;
    std::unique_ptr<RunPause> pause;

    SettlementHarness() {
        // front.anm 根 132/133/142/147 与行条/子项（155 脚本覆盖 137-141、
        // 120-128）；text.anm 底板 75。与生产 RunPause 数据面同源。
        CHECK(load_bank(manager, 0, synthetic_anm(8, trivial_scripts(76))));
        CHECK(load_bank(manager, 3, synthetic_anm(8, trivial_scripts(155))));
        progress.rate = 1;
        progress.continue_stock = 3;
        pause = std::make_unique<RunPause>(progress, manager, environment, host, scheduler, 3);
        CHECK(pause->prepare());
    }

    void frame(u32 pressed = 0) {
        pause->controls({pressed, 0, false});
        CHECK(scheduler.update() >= 0);
    }
    void advance(int frames, u32 pressed = 0) {
        for (int i = 0; i < frames; i++) frame(pressed);
    }
};

// 仅驱动 00428820 -2 的宿主路由；不装载资产，只构造场景对象。
struct TimeoutHost final : RunSceneServices {
    NullGraphics* graphics = nullptr;
    int game_overs = 0, practice_ends = 0, extra_results = 0, destinations = 0;
    bool hud_sound(i32) override { return true; }
    bool request_pause() override { return true; }
    bool request_game_over() override {
        ++game_overs;
        return true;
    }
    bool request_practice_end() override {
        ++practice_ends;
        return true;
    }
    bool request_extra_results() override {
        ++extra_results;
        return true;
    }
    bool request_destination(i32) override {
        ++destinations;
        return true;
    }
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool hud_text(const HudTextDraw&) override { return true; }
    bool camera(DrawCamera, bool) override { return true; }
    bool read_asset(const std::string&, std::vector<u8>&, bool = true) override { return false; }
    ZunGraphics& stage_graphics() override { return *graphics; }
    bool preload_scenery_animation(i32, const std::string&) override { return true; }
};

}  // namespace

TEST(bgameover_is_three_items_defaulting_to_retry_without_continue) {
    SettlementHarness harness;
    harness.progress.run_mode_flags = 0x20;  // 符卡练习（0x43f570 mode≠0）
    harness.progress.selected_spell_id = 7;
    CHECK(harness.pause->open(PauseEntrance::BGameOver));
    CHECK_EQ(int(harness.pause->state.screen), int(PauseScreen::BGameOver));
    harness.advance(12);  // 入场 10 帧后展开
    CHECK_EQ(harness.pause->state.phase, 6);
    CHECK_EQ(harness.pause->state.menu.count, 3);
    // 0x440960 态 0x18：0x15030(0) 后追加 0x15030(2)，默认重试。
    CHECK_EQ(harness.pause->state.menu.cursor, 2);
    // 续关点数存在也不给 Continue：BGameOver 菜单没有该项。
    CHECK(harness.pause->state.can_continue);
    harness.frame(1);     // 确认默认项 = 重试
    harness.advance(13);  // 收场 12 帧后执行
    CHECK(!harness.pause->active());
    CHECK_EQ(harness.host.restarts, 1);
    CHECK_EQ(harness.host.continues, 0);
    CHECK_EQ(harness.host.titles, 0);
    CHECK(harness.pause->error.empty());
}

TEST(bgameover_retry_survives_escape_and_cursor_zero_returns_to_title) {
    SettlementHarness harness;
    harness.progress.run_mode_flags = 0x20;
    CHECK(harness.pause->open(PauseEntrance::BGameOver));
    harness.advance(12);
    // Esc 不能像普通暂停一样恢复对局（结束局无 Continue）。
    harness.frame(0x100);
    CHECK(harness.pause->active());
    CHECK_EQ(harness.pause->state.menu.cursor, 2);
    CHECK_EQ(harness.host.continues, 0);
    // ↑ 到录像保存项，确认后进入选槽（不是直接重试）。
    harness.frame(0x10);
    CHECK_EQ(harness.pause->state.menu.cursor, 1);
    harness.frame(1);
    harness.advance(13);
    CHECK_EQ(harness.pause->state.phase, 17);
    CHECK_EQ(harness.host.restarts, 0);
    // 选槽界面 X 返回菜单；三项菜单重新配置后仍默认重试。
    harness.advance(10);
    harness.frame(0x102);
    CHECK_EQ(harness.pause->state.phase, 6);
    CHECK_EQ(harness.pause->state.menu.cursor, 2);
    // ↑↑ 到标题项并确认。
    harness.frame(0x10);
    harness.frame(0x10);
    CHECK_EQ(harness.pause->state.menu.cursor, 0);
    harness.frame(1);
    harness.advance(13);
    CHECK_EQ(harness.host.titles, 1);
    CHECK_EQ(harness.host.restarts, 0);
    CHECK(harness.pause->error.empty());
}

TEST(ordinary_gameover_keeps_continue_stock_gate) {
    {
        SettlementHarness harness;
        harness.progress.continue_stock = 0;  // be844 < 1：隐藏继续项
        CHECK(harness.pause->open(PauseEntrance::GameOver));
        harness.advance(12);
        CHECK_EQ(harness.pause->state.menu.count, 5);
        CHECK_EQ(harness.pause->state.menu.cursor, 1);
        // 43fa90 态 0xf 无 Yes/No 确认：Z 直接进态 0x15 收场，光标 1 → 回标题。
        harness.frame(1);
        harness.advance(13);
        CHECK_EQ(harness.host.titles, 1);
        CHECK_EQ(harness.host.continues, 0);
    }
    {
        SettlementHarness harness;
        harness.progress.continue_stock = 3;  // be844 >= 1：继续可选
        CHECK(harness.pause->open(PauseEntrance::GameOver));
        harness.advance(12);
        CHECK_EQ(harness.pause->state.menu.cursor, 0);
        harness.frame(1);
        harness.advance(13);
        CHECK_EQ(harness.host.continues, 1);
        CHECK_EQ(harness.host.titles, 0);
        CHECK_EQ(harness.host.restarts, 0);
        CHECK(!harness.pause->active());
        CHECK_EQ(harness.progress.scene_flags & 0x10u, 0u);
    }
}

TEST(spell_timeout_requests_game_over_only) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    SessionState progress;
    progress.stage = 1;
    HudPlayerState player;
    HudScoreState score;
    FrameScheduler scheduler;
    TimeoutHost host;
    host.graphics = &graphics;
    RunScene scene(progress, player, score, manager, renderer, environment, host, scheduler,
                   StagePresentation::Banks{4, 5, 6}, 3, 2, 7, "st01.std");
    CHECK(scene.error.empty());
    // 00428820 -2 的 timeout 分支 = 0x43f570；场景只向宿主发请求。
    scene.battle.sim.spell.phase_timeout(false);
    scene.open_dialog(-2);
    CHECK_EQ(host.game_overs, 1);
    CHECK_EQ(host.practice_ends, 0);
    CHECK_EQ(host.extra_results, 0);
    CHECK_EQ(host.destinations, 0);
    CHECK(scene.error.empty());
}

int main() { return th13test::run_all("结算/Retry"); }
