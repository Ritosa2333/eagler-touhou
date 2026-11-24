#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/FrameScheduler.hpp"
#include "game/TitleMainMenu.hpp"
#include "game/TitleScene.hpp"

#include <memory>
#include <utility>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct OptionHost final : TitleScenePlatform {
    AnmManager& manager;
    i32 start_stage = -1;
    std::vector<std::pair<i32, i32>> applied;
    std::vector<std::pair<i32, i32>> persisted;
    bool persist_ok = true;
    explicit OptionHost(AnmManager& manager) : manager(manager) {}
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
    void apply_volumes(i32 bgm, i32 effect) override { applied.push_back({bgm, effect}); }
    bool persist_options(const TitleOptionVolumes& v) override {
        persisted.push_back({v.bgm, v.effect});
        return persist_ok;
    }
    bool open_key_config() override { return false; }
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
}

std::pair<i32, i32> last_of(const std::vector<std::pair<i32, i32>>& v) {
    return v.empty() ? std::make_pair(-1, -1) : v.back();
}

struct OptionHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings selection;
    FrameScheduler scheduler;
    OptionHost host{manager};
    std::array<FrameCallback, 2> animation_updates{};
    std::unique_ptr<TitleScene> scene;

    OptionHarness() {
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
        CHECK(scheduler.update() >= 0);
        CHECK(scheduler.draw() >= 0);
    }
    void advance(int frames, u32 pressed = 0, u32 repeated = 0) {
        for (int i = 0; i < frames; i++)
            frame(pressed, repeated);
    }
    void reach_main_menu() {
        advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::Main));
        CHECK_EQ(scene->state.substate, 2);
    }
};

TEST(option_entry_wires_screen_and_platform_roundtrip) {
    OptionHarness harness;
    harness.reach_main_menu();
    for (int i = 0; i < 6; i++)
        harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 7);
    harness.frame(0x01);
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Option));
    CHECK_EQ(int(harness.scene->state.screen), 3);
    harness.advance(8);
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK(harness.scene->state.handles[1] != 0);
    CHECK_EQ(last_of(harness.host.applied).first, 100);
    CHECK_EQ(last_of(harness.host.applied).second, 80);
    CHECK_EQ(harness.scene->state.menu.count, 5);
    CHECK_EQ(harness.scene->option_menu().row(), 0);
    harness.frame(0x20);
    CHECK_EQ(harness.scene->option_menu().row(), 1);
    harness.frame(0x80);
    CHECK_EQ(last_of(harness.host.applied).second, 85);
    CHECK_EQ(last_of(harness.host.applied).first, 100);
    harness.frame(0x100);
    CHECK_EQ(harness.scene->option_menu().row(), 4);
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 4);
    harness.advance(12);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.scene->state.menu.cursor, 7);
    CHECK_EQ(harness.scene->state.menu.count, kTitleMainEntries);
    CHECK_EQ(harness.host.persisted.size(), 1u);
    CHECK_EQ(last_of(harness.host.persisted).first, 100);
    CHECK_EQ(last_of(harness.host.persisted).second, 85);
    CHECK(harness.scene->error.empty());
}

TEST(option_leave_with_failed_persistence_reports_error) {
    OptionHarness harness;
    harness.host.persist_ok = false;
    harness.reach_main_menu();
    for (int i = 0; i < 6; i++)
        harness.frame(0x20);
    harness.frame(0x01);
    harness.advance(25);
    harness.advance(8);
    harness.frame(0x100);
    harness.frame(0x01);
    harness.advance(12);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.host.persisted.size(), 1u);
    CHECK(!harness.scene->option_menu().error.empty());
}

int main() { return th13test::run_all("Option 屏接线"); }
