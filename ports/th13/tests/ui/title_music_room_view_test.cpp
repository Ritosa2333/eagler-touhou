#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

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
    for (i32 i = 0; i < 11; ++i)
        text += "\x81\x48";
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
    bool play_track(i32 track) override {
        played = track;
        return true;
    }
    bool play_locked_response() override {
        ++locked;
        return true;
    }
};

struct Harness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    TitleState state;
    MusicHost host;
    TitleMusicRoomMenu menu{state, host};
    TitleMusicRoomView view{manager, 16, 0, 2};
    struct Text {
        u32 color;
        std::string text;
    };
    std::vector<Text> texts;
    std::map<AnmVm*, Text> last;
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
        return view.update(state, menu, host, [this](AnmVm& vm, u32 color, const std::string& text, AnmTextAlignment) {
            texts.push_back({color, text});
            last[&vm] = {color, text};
            return true;
        });
    }
    void advance(i32 frames, u32 pressed = 0, u32 repeated = 0) {
        for (i32 i = 0; i < frames; ++i)
            CHECK(frame(pressed, repeated));
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

}

TEST(music_room_view_rows_and_comments_use_cp932_path) {
    Harness h;
    h.advance(16);
    CHECK_EQ(h.state.substate, 2);
    CHECK(h.has_last(0xffffffu, "track0"));
    CHECK(h.has_last(0xffffffu, locked_title(1)));
    h.advance(15);
    CHECK_EQ(h.menu.revealed_lines(), 8);
    for (i32 line = 0; line < 8; ++line)
        CHECK(h.has_last(0xffffffu, "c0-" + std::to_string(line)));
}

TEST(music_room_view_locked_placeholder_then_playback) {
    Harness h;
    h.advance(16);
    h.frame(0x20);
    h.frame(0x80001);
    CHECK(h.menu.locked_armed());
    CHECK_EQ(h.menu.comment_track(), 1);
    h.advance(15);
    CHECK_EQ(h.menu.revealed_lines(), 8);
    CHECK(h.has_last(0x8080ffu, "\x81\x40"));
    const std::string expected1 = "\x81\x40\x81\x40\x81\x96\x81\x96\x91\x49\x91\xf0\x82\xb5\x82\xbd\x8b\xc8"
                                  "\x82\xcd\x82\xdc\x82\xbe\x83\x51\x81\x5b\x83\x80\x92\x86\x82\xc5\x8d\xc4"
                                  "\x90\xb6\x82\xb3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb9\x82\xf1\x81\x96"
                                  "\x81\x96";
    CHECK(h.has_last(0x8080ffu, expected1));
    CHECK(!h.has_last(0xffffffu, "c1-0"));
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
    h.frame(0x20);
    CHECK_EQ(h.menu.revealed_lines(), 8);
    CHECK_EQ(h.menu.comment_track(), 0);
    for (const auto& entry : h.texts)
        CHECK(entry.text.rfind("c1-", 0) != 0);
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
