// 音楽室 View（production 路径，record_menu_views=true）单测：
//   行标题 CP932 占位白字；注释按 +0x750（comment_track）显影；未听且
//   +0x754!=0 时用 0x4c0da0 占位串 0x8080ff；二次 Z 后真实注释白字；
//   导航不重置显影计数；release 回收 VM。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/TitleMusicRoomView.hpp"

#include <cstdio>
#include <map>
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

struct MusicHost final : TitleMusicRoomPlatform {
    std::vector<bool> heard{true, false, false, false};
    i32 played = -1, locked = 0, marked = -1;
    i32 track_count() const override { return i32(heard.size()); }
    std::string track_title(i32 track) override { return "track" + std::to_string(track); }
    std::string track_comment(i32 track, i32 line) override {
        return "c" + std::to_string(track) + "-" + std::to_string(line);
    }
    bool track_heard(i32 track) const override { return heard[u32(track)]; }
    void mark_heard(i32 track) override { marked = track; }
    bool play_track(i32 track) override { played = track; return true; }
    bool play_locked_response() override { ++locked; return true; }
};

struct Harness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    TitleState state;
    MusicHost host;
    TitleMusicRoomMenu menu{state, host};
    TitleMusicRoomView view{manager, 16, 0, 2};
    struct Text { u32 color; std::string text; };
    std::vector<Text> texts;              // 本帧 rasterize 调用
    std::map<AnmVm*, Text> last;          // 每个 VM 最近一次写入内容
    Harness() {
        state.screen = TitleScreen(kTitleScreenMusicRoom);
        menu.play_sound = [](i32) { return true; };
        CHECK(load_bank(manager, 16, synthetic_anm(1, trivial_scripts(200))));
        CHECK(load_bank(manager, 0, synthetic_anm(1, trivial_scripts(64))));
        CHECK(load_bank(manager, 2, synthetic_anm(1, trivial_scripts(24))));
    }
    bool frame(u32 pressed = 0, u32 repeated = 0) {
        texts.clear();
        if (!menu.update(pressed, repeated)) return false;
        return view.update(state, menu, host, [this](AnmVm& vm, u32 color, const std::string& text,
                                                     AnmTextAlignment) {
            texts.push_back({color, text});
            last[&vm] = {color, text};
            return true;
        });
    }
    void advance(i32 frames, u32 pressed = 0, u32 repeated = 0) {
        for (i32 i = 0; i < frames; ++i) CHECK(frame(pressed, repeated));
    }
    bool has(u32 color, const std::string& text) const {
        for (const auto& entry : texts)
            if (entry.color == color && entry.text == text) return true;
        return false;
    }
    bool has_last(u32 color, const std::string& text) const {
        for (const auto& entry : last)
            if (entry.second.color == color && entry.second.text == text) return true;
        return false;
    }
};

}  // namespace

TEST(music_room_view_rows_and_comments_use_cp932_path) {
    Harness h;
    h.advance(16);  // case0/1 → case2
    CHECK_EQ(h.state.substate, 2);
    CHECK(h.has_last(0xffffffu, "track0"));
    CHECK(h.has_last(0xffffffu, locked_title(1)));
    // 显影到 8 行：注释走真实曲目文本（comment_track=0，已听）。
    h.advance(15);
    CHECK_EQ(h.menu.revealed_lines(), 8);
    for (i32 line = 0; line < 8; ++line)
        CHECK(h.has_last(0xffffffu, "c0-" + std::to_string(line)));
}

TEST(music_room_view_locked_placeholder_then_playback) {
    Harness h;
    h.advance(16);
    h.frame(0x20);      // 曲1 未听
    h.frame(0x80001);   // 首次 Z：锁定响应
    CHECK(h.menu.locked_armed());
    CHECK_EQ(h.menu.comment_track(), 1);
    h.advance(15);      // 显影 8 行
    CHECK_EQ(h.menu.revealed_lines(), 8);
    CHECK(h.has_last(0x8080ffu, "\x81\x40"));
    const std::string expected1 =
        "\x81\x40\x81\x40\x81\x96\x81\x96\x91\x49\x91\xf0\x82\xb5\x82\xbd\x8b\xc8"
        "\x82\xcd\x82\xdc\x82\xbe\x83\x51\x81\x5b\x83\x80\x92\x86\x82\xc5\x8d\xc4"
        "\x90\xb6\x82\xb3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb9\x82\xf1\x81\x96"
        "\x81\x96";
    CHECK(h.has_last(0x8080ffu, expected1));
    CHECK(!h.has_last(0xffffffu, "c1-0"));
    // 二次 Z（显影已满 8 行但仍在同一帧直接 Z；armed 未在导航分支被清）→ 播放。
    h.frame(0x80001);
    CHECK_EQ(h.host.played, 1);
    CHECK_EQ(h.host.marked, 0);
    CHECK(!h.menu.locked_armed());
    h.advance(15);
    CHECK(h.has_last(0xffffffu, "c1-0"));
    CHECK(!h.has_last(0x8080ffu, expected1));
}

TEST(music_room_view_navigation_does_not_reset_reveal) {
    Harness h;
    h.advance(16);
    h.advance(15);
    CHECK_EQ(h.menu.revealed_lines(), 8);
    h.frame(0x20);  // 曲1；导航不清计数
    CHECK_EQ(h.menu.revealed_lines(), 8);
    CHECK_EQ(h.menu.comment_track(), 0);  // 注释块仍为 +0x750（未 Z）
    // 导航帧不应重挂/重绘注释（真实注释 VM 保持已写入内容）。
    for (const auto& entry : h.texts) CHECK(entry.text.rfind("c1-", 0) != 0);
}

TEST(music_room_view_release_reclaims_vms) {
    Harness h;
    const u32 baseline = h.manager.registry.count();
    h.advance(16);
    CHECK(h.manager.registry.count() > baseline);
    h.view.release();
    CHECK(h.manager.update());
    CHECK_EQ(h.manager.registry.count(), baseline);
}

int main() {
    std::printf("# title_music_room_view_test\n");
    return th13test::run_all("title_music_room_view_test");
}
