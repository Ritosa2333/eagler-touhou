#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/FrameScheduler.hpp"
#include "game/TitleMainMenu.hpp"
#include "game/TitleScene.hpp"

#include <memory>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct ManualHost final : TitleScenePlatform {
    AnmManager& manager;
    i32 start_stage = -1;
    int opened = 0, closed = 0;
    bool open_ok = true, finished = false;
    std::vector<std::string> page_names;
    explicit ManualHost(AnmManager& manager) : manager(manager) {}
    bool clear_title_overlay() override { return true; }
    bool queue_title_music(i32, i32, const std::string&) override { return true; }
    bool clear_current_wave() override { return true; }
    bool fade_out_title_music() override { return true; }
    bool sound(i32) override { return true; }
    bool begin_transition(u32& animation) override {
        animation = manager.create(8, 0, -1, 0);
        return animation != 0;
    }
    bool start_game(i32 stage) override {
        start_stage = stage;
        return true;
    }
    bool hud_text(const HudTextDraw&) override { return true; }
    bool title_exit(i32) override { return true; }
    bool open_manual() override {
        ++opened;
        for (i32 page = 0; page < 3; page++)
            page_names.push_back(help_page_name(page));
        return open_ok;
    }
    bool manual_finished() const override { return finished; }
    void close_manual() override { ++closed; }
};

std::vector<u8> title_anm_fixture() {
    std::vector<std::vector<AnmIns>> scripts(243);
    for (auto& script : scripts)
        script = {wait(), stop()};
    auto spawn_range = [&](i32 root, i32 first, i32 last) {
        scripts[u32(root)].clear();
        for (i32 child = first; child <= last; child++)
            scripts[u32(root)].push_back(spawn(child));
        scripts[u32(root)].push_back(wait());
        scripts[u32(root)].push_back(stop());
    };
    spawn_range(0, 3, 22);
    for (i32 item = 0; item < 10; item++) {
        scripts[u32(3 + item)].clear();
        scripts[u32(3 + item)].push_back(label(7 + item));
        scripts[u32(3 + item)].push_back(wait());
        scripts[u32(3 + item)].push_back(stop());
    }
    spawn_range(1, 23, 48);
    spawn_range(2, 49, 82);
    scripts[89] = {spawn(92), spawn(93), spawn(91), spawn(90), wait(), stop()};
    scripts[94] = {spawn(97), spawn(96), spawn(100), wait(), stop()};
    scripts[95] = {spawn(99), spawn(96), spawn(100), wait(), stop()};
    scripts[123].clear();
    for (i32 child : {83, 84, 118, 119, 120, 121, 113, 114, 115, 116})
        scripts[123].push_back(spawn(child));
    scripts[123].push_back(wait());
    scripts[123].push_back(stop());
    scripts[129].clear();
    for (i32 child : {125, 126, 127, 128, 135, 136, 137, 138, 131, 132, 133, 134})
        scripts[129].push_back(spawn(child));
    scripts[129].push_back(wait());
    scripts[129].push_back(stop());
    return synthetic_anm(72, scripts);
}

struct ManualHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings selection;
    FrameScheduler scheduler;
    ManualHost host{manager};
    std::array<FrameCallback, 2> animation_updates{};
    std::unique_ptr<TitleScene> scene;
    bool expect_frame_error = false;

    ManualHarness() {
        animation_updates[0].owner = animation_updates[1].owner = &manager;
        animation_updates[0].enabled = animation_updates[1].enabled = true;
        animation_updates[0].run = [](void* p) { return static_cast<AnmManager*>(p)->update(true) ? 1 : 5; };
        animation_updates[1].run = [](void* p) { return static_cast<AnmManager*>(p)->update(false) ? 1 : 5; };
        CHECK(scheduler.add(animation_updates[0], FramePass::Update, 9) >= 0);
        CHECK(scheduler.add(animation_updates[1], FramePass::Update, 34) >= 0);
        CHECK(load_bank(manager, 16, title_anm_fixture()));
        CHECK(load_bank(manager, 17, synthetic_anm(1, trivial_scripts(1))));
        CHECK(load_bank(manager, 2, synthetic_anm(8, trivial_scripts(20))));
        CHECK(load_bank(manager, 19, synthetic_anm(4, trivial_scripts(4))));
        CHECK(load_bank(manager, 8, synthetic_anm(4, trivial_scripts(4))));
        scene = std::make_unique<TitleScene>(progress, selection, manager, host, scheduler, TitleSceneResources{});
        CHECK(scene->error.empty());
        scene->state.return_reason = 0;
    }

    void frame(u32 pressed = 0, u32 repeated = 0) {
        scene->controls({0, pressed, repeated, 0});
        const i32 result = scheduler.update();
        if (expect_frame_error)
            CHECK(result < 0);
        else
            CHECK(result >= 0);
        CHECK(scheduler.draw() >= 0);
    }
    void advance(int frames, u32 pressed = 0, u32 repeated = 0) {
        for (int i = 0; i < frames; i++)
            frame(pressed, repeated);
    }
    void enter_manual() {
        advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
        for (int i = 0; i < 7; i++)
            frame(0x20);
        CHECK_EQ(scene->state.menu.cursor, 8);
        frame(0x01);
        advance(25);
    }
};
}

TEST(manual_entry_routes_and_finishes_via_platform) {
    ManualHarness harness;
    harness.advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
    const u32 background_before = harness.scene->state.handles[89];
    CHECK(background_before != 0);
    for (int i = 0; i < 7; i++)
        harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 8);
    harness.frame(0x01);
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Manual));
    CHECK_EQ(int(harness.scene->state.screen), 16);
    CHECK_EQ(harness.scene->state.substate, 1);
    CHECK_EQ(harness.host.opened, 1);
    CHECK_EQ(harness.host.page_names.size(), 3u);
    CHECK_EQ(harness.host.page_names[0], std::string("help_01.png"));
    CHECK_EQ(harness.host.page_names[2], std::string("help_03.png"));
    CHECK_EQ(harness.host.closed, 0);
    harness.host.finished = true;
    harness.advance(2);
    CHECK_EQ(harness.host.closed, 1);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.scene->state.menu.cursor, 8);
    CHECK_EQ(harness.scene->state.menu.count, kTitleMainEntries);
    CHECK(harness.scene->state.handles[89] != 0);
    CHECK_EQ(harness.scene->state.handles[89], background_before);
    CHECK_EQ(harness.scene->state.handles[94], 0u);
    CHECK(harness.scene->state.handles[95] != 0);
    harness.advance(kTitleMenuReadyDelay + 1);
    CHECK_EQ(harness.scene->state.substate, 2);
    harness.host.finished = false;
    harness.frame(0x01);
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Manual));
    CHECK_EQ(harness.host.opened, 2);
    harness.host.finished = true;
    harness.advance(2);
    CHECK_EQ(harness.host.closed, 2);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK(harness.scene->error.empty());
}

TEST(manual_open_failure_reports_real_error) {
    ManualHarness harness;
    harness.host.open_ok = false;
    harness.advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
    for (int i = 0; i < 7; i++)
        harness.frame(0x20);
    harness.frame(0x01);
    bool errored = false;
    for (int i = 0; i < 30 && !errored; i++) {
        harness.scene->controls({0, 0, 0, 0});
        errored = harness.scheduler.update() < 0;
    }
    CHECK(errored);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Manual));
    CHECK_EQ(harness.host.opened, 1);
    CHECK(!harness.scene->error.empty());
    CHECK_EQ(harness.host.closed, 0);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Manual));
}

int main() { return th13test::run_all("マニュアル接线"); }
