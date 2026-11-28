#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/FrameScheduler.hpp"
#include "game/TitleMainMenu.hpp"
#include "game/TitleScene.hpp"

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

std::string locked_title(i32 track) {
    char prefix[16];
    std::snprintf(prefix, sizeof prefix, "No.%2d ", int(track) + 1);
    std::string text = prefix;
    for (i32 i = 0; i < 11; ++i)
        text += "\x81\x48";
    return text;
}

struct MusicHost final : TitleScenePlatform {
    AnmManager& manager;
    i32 start_stage = -1;
    i32 heard = 1 << 0;
    std::vector<i32> played, locked_responses, marked, resumed;
    struct Text {
        Vec3 position;
        u32 color;
        std::string text;
    };
    std::vector<Text> texts;
    explicit MusicHost(AnmManager& manager) : manager(manager) {}
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
    bool hud_text(const HudTextDraw& text) override {
        texts.push_back({text.position, text.style.color, text.text});
        return true;
    }
    bool title_exit(i32) override { return true; }
    i32 track_count() const override { return 12; }
    std::string track_title(i32 track) override {
        char buffer[32];
        std::snprintf(buffer, sizeof buffer, "Track %02d", track);
        return buffer;
    }
    std::string track_comment(i32 track, i32 line) override {
        char buffer[48];
        std::snprintf(buffer, sizeof buffer, "cmt %02d-%d", track, line);
        return line < 3 ? buffer : std::string();
    }
    bool track_heard(i32 track) const override { return (heard >> track & 1) != 0; }
    void mark_heard(i32 track) override { marked.push_back(track); }
    bool play_track(i32 track) override {
        played.push_back(track);
        return true;
    }
    bool play_locked_response() override {
        locked_responses.push_back(1);
        return true;
    }
    bool resume_title_music() override {
        resumed.push_back(1);
        return true;
    }
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

struct MusicHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings selection;
    FrameScheduler scheduler;
    MusicHost host{manager};
    std::array<FrameCallback, 2> animation_updates{};
    std::unique_ptr<TitleScene> scene;

    MusicHarness() {
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

TEST(music_room_entry_platform_and_return_roundtrip) {
    MusicHarness harness;
    harness.reach_main_menu();
    const u32 background_before = harness.scene->state.handles[89];
    CHECK(background_before != 0);
    for (int i = 0; i < 5; i++)
        harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 6);
    harness.frame(0x01);
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::MusicRoom));
    CHECK_EQ(int(harness.scene->state.screen), 13);
    harness.advance(20);
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK_EQ(harness.scene->music_room_menu().row(), 0);
    CHECK_EQ(harness.scene->state.menu.count, 12);
    harness.host.texts.clear();
    harness.frame();
    CHECK(harness.host.texts.size() >= 10u);
    CHECK_EQ(harness.host.texts[0].text, std::string("Track 00"));
    CHECK_EQ(harness.host.texts[0].color, 0xffffffffu);
    CHECK_NEAR(harness.host.texts[0].position.x, 62.f, 1e-5);
    CHECK_NEAR(harness.host.texts[0].position.y, 80.f, 1e-5);
    CHECK_EQ(harness.host.texts[1].text, locked_title(1));
    CHECK_EQ(harness.host.texts[1].color, 0xffffffffu);
    CHECK_NEAR(harness.host.texts[1].position.x, 58.f, 1e-5);
    CHECK_NEAR(harness.host.texts[1].position.y, 100.f, 1e-5);
    harness.frame(0x20);
    CHECK_EQ(harness.scene->music_room_menu().row(), 1);
    harness.frame(0x01);
    CHECK_EQ(harness.host.locked_responses.size(), 1u);
    CHECK(harness.host.played.empty());
    CHECK(harness.host.marked.empty());
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::MusicRoom));
    harness.host.texts.clear();
    harness.frame();
    bool saw_placeholder = false;
    for (const auto& text : harness.host.texts)
        if (text.color == 0xff8080ffu && text.text == std::string("\x81\x40")) saw_placeholder = true;
    CHECK(saw_placeholder);
    harness.frame(0x01);
    CHECK_EQ(harness.host.played.size(), 1u);
    CHECK_EQ(harness.host.played[0], 1);
    CHECK_EQ(harness.host.marked.size(), 1u);
    CHECK_EQ(harness.host.marked[0], 0);
    harness.frame(0x10);
    CHECK_EQ(harness.scene->music_room_menu().row(), 0);
    harness.frame(0x01);
    CHECK_EQ(harness.host.played.size(), 2u);
    CHECK_EQ(harness.host.played[1], 0);
    CHECK_EQ(harness.host.marked.size(), 2u);
    CHECK_EQ(harness.host.marked[1], 0);
    CHECK(harness.scene->error.empty());
    harness.frame(0x100);
    CHECK_EQ(harness.scene->state.substate, 3);
    harness.advance(11);
    CHECK_EQ(harness.host.resumed.size(), 1u);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.scene->state.menu.cursor, 6);
    CHECK_EQ(harness.scene->state.menu.count, kTitleMainEntries);
    CHECK(harness.scene->state.handles[89] != 0);
    CHECK_EQ(harness.scene->state.handles[89], background_before);
    CHECK_EQ(harness.scene->state.handles[94], 0u);
    CHECK(harness.scene->state.handles[95] != 0);
    auto* background = harness.manager.registry.find(harness.scene->state.handles[89]);
    CHECK(background != nullptr);
    CHECK(harness.manager.registry.find_child_script(*background, 90, 0) != nullptr);
    CHECK(harness.manager.registry.find_child_script(*background, 91, 0) != nullptr);
    CHECK(harness.scene->error.empty());
}

int main() { return th13test::run_all("音楽室接线"); }
