// 音楽室状态机单测（FUN_004554f0 移植对合）：
//   case0 age≥10 → case1（隔帧显 8 行）→ >4 帧 case2；↑↓ 环绕音10；Z 已听曲
//   → play_track + mark_heard(0)；Z 未听曲 → play_locked_response 不解锁；
//   X 音9 → case3 >9 帧 → resume_title_music + 回 Main；空表可退出不假成功。
#include "../harness/test_framework.hpp"

#include "game/TitleMusicRoomMenu.hpp"

#include <string>
#include <vector>

using namespace th13;

namespace {

struct MusicHost final : TitleMusicRoomPlatform {
    i32 tracks = 12;
    std::vector<bool> heard;          // 0/2/11 号已听
    i32 played = -1, locked_plays = 0, resumes = 0, marked = -1;
    MusicHost() : heard(tracks, false) { heard[0] = heard[2] = heard[11] = true; }
    i32 track_count() const override { return tracks; }
    std::string track_title(i32 t) override { return "track" + std::to_string(t); }
    std::string track_comment(i32 t, i32 line) override {
        return "c" + std::to_string(t) + "-" + std::to_string(line);
    }
    bool track_heard(i32 t) const override { return heard[t]; }
    void mark_heard(i32 t) override { marked = t; }
    bool play_track(i32 t) override { played = t; return true; }
    bool play_locked_response() override { ++locked_plays; return true; }
    bool resume_title_music() override { ++resumes; return true; }
};

struct TextSink final : HudDrawServices {
    struct Entry { float x, y; u32 color; std::string text; };
    std::vector<Entry> entries;
    bool hud_text(const HudTextDraw& draw) override {
        entries.push_back({draw.position.x, draw.position.y, draw.style.color, draw.text});
        return true;
    }
};

struct Harness {
    TitleState state;
    MusicHost host;
    TextSink sink;
    TitleMusicRoomMenu menu{state, host};
    std::vector<i32> sounds;
    Harness() {
        state.screen = TitleScreen(kTitleScreenMusicRoom);
        menu.play_sound = [this](i32 id) { sounds.push_back(id); return true; };
    }
    bool frame(u32 pressed = 0, u32 repeated = 0) {
        const bool ok = menu.update(pressed, repeated);
        menu.draw_texts(sink);
        return ok;
    }
    void advance(i32 n, u32 pressed = 0, u32 repeated = 0) {
        for (i32 i = 0; i < n; i++) CHECK(frame(pressed, repeated));
    }
};

}  // namespace

TEST(music_room_entry_and_reveal) {
    Harness h;
    h.advance(9);
    CHECK_EQ(h.state.substate, 0);  // age≥10 才进
    CHECK(h.frame());
    CHECK_EQ(h.state.substate, 1);
    h.frame();
    CHECK_EQ(h.sink.entries.size(), 0u);  // 隔帧：奇数帧未显
    h.frame();
    CHECK_EQ(h.sink.entries.size(), 1u);  // 偶数帧显 1 行
    h.advance(4);
    CHECK_EQ(h.state.substate, 2);  // >4 帧进交互
    // 首行 = 0 号曲（已听，白色）；选中 x+4。
    bool found = false;
    for (const auto& e : h.sink.entries)
        if (e.text == "track0" && e.x > 58.f) found = true;
    CHECK(found);
}

TEST(music_room_navigation_wrap_and_window) {
    Harness h;
    h.advance(16);  // 进 case2
    CHECK_EQ(h.state.substate, 2);
    CHECK(h.frame(0x20));  // ↓ 0→1
    CHECK_EQ(h.menu.row(), 1);
    CHECK(!h.sounds.empty() && h.sounds.back() == 10);
    h.frame(0, 0x10);      // ↑ 回 0
    CHECK_EQ(h.menu.row(), 0);
    h.advance(1, 0x10);    // ↑ 环绕 0→11
    CHECK_EQ(h.menu.row(), 11);
    // 窗口跟随：末页（top=10）显示 track11（已听）；track10 未听 → "???"
    // 占位色 0x8080ff 族。
    bool saw_locked = false, saw_page_top = false;
    for (const auto& e : h.sink.entries) {
        if (e.text == "???" && (e.color & 0xffffff) == 0x8080ff) saw_locked = true;
        if (e.text == "track11") saw_page_top = true;
    }
    CHECK(saw_locked);
    CHECK(saw_page_top);
}

TEST(music_room_play_paths) {
    Harness h;
    h.advance(16);
    h.frame(0x20); h.frame(0x20);  // → 曲2（已听）
    CHECK(h.frame(0x80001));
    CHECK_EQ(h.host.played, 2);
    CHECK_EQ(h.host.marked, 0);  // 原版任意播曲解锁 0 号曲
    h.frame(0x10);               // ↑ 回 1（未听）
    CHECK(h.frame(0x80001));
    CHECK_EQ(h.host.played, 2);           // 未再播
    CHECK_EQ(h.host.locked_plays, 1);     // 锁定响应
    CHECK(!h.host.heard[1]);              // 不解锁
}

TEST(music_room_exit_restores_title_music) {
    Harness h;
    h.advance(16);
    CHECK(h.frame(0x102));  // X：音9 → case3
    CHECK_EQ(h.state.substate, 3);
    CHECK(!h.sounds.empty() && h.sounds.back() == 9);
    CHECK_EQ(h.host.resumes, 0);
    h.advance(9);
    CHECK_EQ(h.host.resumes, 0);  // >9 帧才离屏
    CHECK(h.frame());
    CHECK_EQ(h.host.resumes, 1);
    CHECK_EQ(int(h.state.screen), int(TitleScreen::Main));
    CHECK(h.menu.error.empty());
}

TEST(music_room_empty_catalog_exits_cleanly) {
    Harness h;
    h.host.tracks = 0;
    h.host.heard.clear();
    h.advance(16);
    CHECK_EQ(h.state.substate, 2);
    CHECK(h.frame(0x80001));  // Z：空表无曲目可播，不崩溃
    CHECK_EQ(h.host.played, -1);
    CHECK(h.frame(0x102));
    h.advance(10);
    CHECK_EQ(int(h.state.screen), int(TitleScreen::Main));
}

int main() {
    std::printf("# title_music_room_menu_test\n");
    return th13test::run_all("title_music_room_menu_test");
}
