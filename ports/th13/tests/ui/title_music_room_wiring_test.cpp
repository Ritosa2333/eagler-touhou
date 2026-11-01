// 音楽室主接线单测（Title owner 接线，非 MenuExtras 屏内逻辑——那由
// title_music_room_menu_test 5/5 覆盖）：
//   TitleScreen::MusicRoom=13 枚举、TitleMainMenu entry 6 Z 切屏（光标 push/
//   回 Main 还原）、TitleFrame case 13 + Draw 文本队列、TitleScenePlatform
//   聚合 TitleMusicRoomPlatform（track/play/heard/resume 经宿主回调）。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/TitleScene.hpp"
#include "game/TitleMainMenu.hpp"
#include "game/FrameScheduler.hpp"

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
    for (i32 i = 0; i < 11; ++i) text += "\x81\x48";
    return text;
}

struct MusicHost final : TitleScenePlatform {
    AnmManager& manager;
    i32 start_stage = -1;
    // 曲目表：12 曲；仅 0 号听过（新档全 false 语义保留一处 true 供播放路径）。
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
    // TitleMusicRoomPlatform
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
    for (auto& script : scripts) script = {wait(), stop()};
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
}  // namespace

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
        animation_updates[0].run = [](void* p) {
            return static_cast<AnmManager*>(p)->update(true) ? 1 : 5;
        };
        animation_updates[1].run = [](void* p) {
            return static_cast<AnmManager*>(p)->update(false) ? 1 : 5;
        };
        CHECK(scheduler.add(animation_updates[0], FramePass::Update, 9) >= 0);
        CHECK(scheduler.add(animation_updates[1], FramePass::Update, 34) >= 0);
        CHECK(load_bank(manager, 16, title_anm_fixture()));
        CHECK(load_bank(manager, 17, synthetic_anm(1, trivial_scripts(1))));
        CHECK(load_bank(manager, 2, synthetic_anm(8, trivial_scripts(20))));
        CHECK(load_bank(manager, 19, synthetic_anm(4, trivial_scripts(4))));
        CHECK(load_bank(manager, 8, synthetic_anm(4, trivial_scripts(4))));
        scene = std::make_unique<TitleScene>(progress, selection, manager, host, scheduler,
                                             TitleSceneResources{});
        CHECK(scene->error.empty());
        scene->state.return_reason = 0;
    }

    void frame(u32 pressed = 0, u32 repeated = 0) {
        scene->controls({0, pressed, repeated, 0});
        CHECK(scheduler.update() >= 0);
        CHECK(scheduler.draw() >= 0);
    }
    void advance(int frames, u32 pressed = 0, u32 repeated = 0) {
        for (int i = 0; i < frames; i++) frame(pressed, repeated);
    }
    void reach_main_menu() {
        advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::Main));
        CHECK_EQ(scene->state.substate, 2);
    }
};

// entry 6（音楽室）Z → 屏 13 → case0/1 进列表 → 10 行窗口 + 注释队列文本 →
// 未听曲首次 Z = 锁定响应（不播曲），二次 Z = play_track + mark_heard(0)；
// 已听曲 Z = play_track + mark_heard(0)；X → case3 >9 帧：resume_title_music +
// 回 Main + 光标还原 6；返回主菜单按 0x44cdb9 重建背景/LOGO 根（H01-1）。
TEST(music_room_entry_platform_and_return_roundtrip) {
    MusicHarness harness;
    harness.reach_main_menu();
    const u32 background_before = harness.scene->state.handles[89];
    CHECK(background_before != 0);
    for (int i = 0; i < 5; i++) harness.frame(0x20);  // 0→2→3→4→5→6
    CHECK_EQ(harness.scene->state.menu.cursor, 6);
    harness.frame(0x01);   // Z：主菜单确认序
    harness.advance(25);   // age>19 → enter(6) → TitleScreen::MusicRoom
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::MusicRoom));
    CHECK_EQ(int(harness.scene->state.screen), 13);  // 0x44c570 case d 屏值
    harness.advance(20);  // case0 age≥10 → case1 >4 → case2
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK_EQ(harness.scene->music_room_menu().row(), 0);
    CHECK_EQ(harness.scene->state.menu.count, 12);  // 曲数入光标计数
    // 列表文本（Draw 队列通道）：行0 已听 = "Track 00" 白字选中 x=62；
    // 行1 未听 = 0x4ac930 占位白字 x=58；origin (58,80) 行距 20。
    harness.host.texts.clear();
    harness.frame();
    CHECK(harness.host.texts.size() >= 10u);
    CHECK_EQ(harness.host.texts[0].text, std::string("Track 00"));
    CHECK_EQ(harness.host.texts[0].color, 0xffffffffu);
    CHECK_NEAR(harness.host.texts[0].position.x, 62.f, 1e-5);  // 选中 +4
    CHECK_NEAR(harness.host.texts[0].position.y, 80.f, 1e-5);
    CHECK_EQ(harness.host.texts[1].text, locked_title(1));
    CHECK_EQ(harness.host.texts[1].color, 0xffffffffu);
    CHECK_NEAR(harness.host.texts[1].position.x, 58.f, 1e-5);
    CHECK_NEAR(harness.host.texts[1].position.y, 100.f, 1e-5);
    // ↓ 到行1（未听）→ 首次 Z：锁定响应，不播曲、不落档；二次 Z 播放+落档。
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
        if (text.color == 0xff8080ffu && text.text == std::string("\x81\x40"))
            saw_placeholder = true;
    CHECK(saw_placeholder);
    harness.frame(0x01);   // 二次 Z（显影未满 8 行）：播放 + 解锁 0 号曲
    CHECK_EQ(harness.host.played.size(), 1u);
    CHECK_EQ(harness.host.played[0], 1);
    CHECK_EQ(harness.host.marked.size(), 1u);
    CHECK_EQ(harness.host.marked[0], 0);
    // ↑ 回行0（已听）→ Z：play_track(0) + mark_heard(0)。
    harness.frame(0x10);
    CHECK_EQ(harness.scene->music_room_menu().row(), 0);
    harness.frame(0x01);
    CHECK_EQ(harness.host.played.size(), 2u);
    CHECK_EQ(harness.host.played[1], 0);
    CHECK_EQ(harness.host.marked.size(), 2u);
    CHECK_EQ(harness.host.marked[1], 0);
    CHECK(harness.scene->error.empty());
    // X → case3 → >9 帧：恢复标题曲 + 回主菜单 + 光标栈还原 6/10。
    harness.frame(0x100);
    CHECK_EQ(harness.scene->state.substate, 3);
    harness.advance(11);
    CHECK_EQ(harness.host.resumed.size(), 1u);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.scene->state.menu.cursor, 6);
    CHECK_EQ(harness.scene->state.menu.count, kTitleMainEntries);
    // 0x44cdb9：返回路径重建背景 89 与 LOGO 94（旧树已停机，label2 不可恢复）。
    CHECK(harness.scene->state.handles[89] != 0);
    CHECK(harness.scene->state.handles[89] != background_before);
    CHECK(harness.scene->state.handles[94] != 0);
    CHECK_EQ(harness.scene->state.handles[95], 0u);
    auto* background = harness.manager.registry.find(harness.scene->state.handles[89]);
    CHECK(background != nullptr);
    CHECK(harness.manager.registry.find_child_script(*background, 90, 0) != nullptr);
    CHECK(harness.manager.registry.find_child_script(*background, 91, 0) != nullptr);
    CHECK(harness.scene->error.empty());
}

int main() { return th13test::run_all("音楽室接线"); }
