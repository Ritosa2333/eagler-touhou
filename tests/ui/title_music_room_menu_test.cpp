// 音楽室状態機単測（FUN_004554f0 移植対合）：
//   case0 age≥10 → case1（隔帧显影）→ >4 帧 case2；case2 隔帧继续显影至 8；
//   ↑↓ 环绕音10；Z 未听曲首次 = 锁定响应 +0x754=1，二次 Z = 播放 + 落档
//   score[0x1b27a]=1；导航分支仅在 +0x74c>=8 时清 +0x754（非 8 行 timeout）；
//   X 音9 → case3 >9 帧 → resume_title_music + 回 Main；空表可退出不假成功。
//   锁定曲名 = "No.%2d " + 11 CP932 全角？白字；锁定注释 = 0x4c0da0 占位串
//   0x8080ff。
#include "../harness/test_framework.hpp"

#include "game/TitleMusicRoomMenu.hpp"

#include <cstdio>
#include <string>
#include <vector>

using namespace th13;

namespace {

std::string locked_title(i32 track) {
    char prefix[16];
    std::snprintf(prefix, sizeof prefix, "No.%2d ", int(track) + 1);
    std::string text = prefix;
    for (i32 i = 0; i < 11; ++i) text += "\x81\x48";
    return text;
}

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
        sink.entries.clear();  // 每帧只看当前绘制队列
        const bool ok = menu.update(pressed, repeated);
        menu.draw_texts(sink);
        return ok;
    }
    void advance(i32 n, u32 pressed = 0, u32 repeated = 0) {
        for (i32 i = 0; i < n; i++) CHECK(frame(pressed, repeated));
    }
};

bool has_text(const TextSink& sink, const std::string& text) {
    for (const auto& entry : sink.entries)
        if (entry.text == text) return true;
    return false;
}

const TextSink::Entry* find_text(const TextSink& sink, const std::string& text) {
    for (const auto& entry : sink.entries)
        if (entry.text == text) return &entry;
    return nullptr;
}

}  // namespace

TEST(music_room_entry_and_reveal) {
    Harness h;
    h.advance(9);
    CHECK_EQ(h.state.substate, 0);  // age≥10 才进
    CHECK(h.frame());
    CHECK_EQ(h.state.substate, 1);
    CHECK_EQ(h.menu.revealed_lines(), 0);  // case0 转 substate1（本帧不进 case1）
    h.frame();
    CHECK_EQ(h.menu.revealed_lines(), 0);  // case1 第一帧（odd）不显影
    h.frame();
    CHECK_EQ(h.menu.revealed_lines(), 1);  // case1 第二帧（even）显 1 行
    h.advance(3);  // frames 3..5：再显 1 行后 >4 进 case2（case1 共 2 行）
    CHECK_EQ(h.state.substate, 2);
    CHECK_EQ(h.menu.revealed_lines(), 2);
    // 列表 10 行窗口（已听白字，选中 x+4；未听 = CP932 占位白字）。
    const auto* selected = find_text(h.sink, "track0");
    CHECK(selected != nullptr);
    CHECK_NEAR(selected->x, 62.f, 1e-5);
    CHECK_EQ(selected->color, 0xffffffffu);
    const auto* locked = find_text(h.sink, locked_title(1));
    CHECK(locked != nullptr);
    CHECK_NEAR(locked->x, 58.f, 1e-5);
    CHECK_EQ(locked->color, 0xffffffffu);
    // case2 继续隔帧显影：2 → 8（6 个偶数帧）。
    h.advance(12);
    CHECK_EQ(h.menu.revealed_lines(), 8);
    CHECK(has_text(h.sink, "c0-7"));
}

TEST(music_room_navigation_wrap_and_window) {
    Harness h;
    h.advance(16);  // 进 case2
    CHECK_EQ(h.state.substate, 2);
    h.frame(0x20);  // ↓ 0→1
    CHECK_EQ(h.menu.row(), 1);
    CHECK(!h.sounds.empty() && h.sounds.back() == 10);
    h.frame(0, 0x10);      // ↑ 回 0
    CHECK_EQ(h.menu.row(), 0);
    h.advance(1, 0x10);    // ↑ 环绕 0→11
    CHECK_EQ(h.menu.row(), 11);
    // 窗口跟随：末页（top=10）显示 track11（已听）；track10 未听 → 占位白字。
    CHECK(has_text(h.sink, "track11"));
    CHECK(has_text(h.sink, locked_title(10)));
    // 导航不清显影计数（原版 +0x74c 只在 Z 重置）。
    const i32 revealed = h.menu.revealed_lines();
    h.frame(0x10);
    CHECK_EQ(h.menu.revealed_lines(), revealed);
}

TEST(music_room_locked_two_z_play_and_unlock) {
    Harness h;
    h.advance(16);
    h.frame(0x20);  // → 曲1（未听）
    CHECK_EQ(h.menu.row(), 1);
    // 第一次 Z：锁定响应 +0x754=1；不播放、不落档。
    CHECK(h.frame(0x80001));
    CHECK_EQ(h.host.locked_plays, 1);
    CHECK_EQ(h.host.played, -1);
    CHECK_EQ(h.host.marked, -1);
    CHECK(h.menu.locked_armed());
    CHECK_EQ(h.menu.comment_track(), 1);
    CHECK_EQ(h.menu.revealed_lines(), 0);  // Z 重置注释显影
    CHECK(!h.host.heard[1]);
    // 第二次 Z（显影未满 8 行）：播放该曲 + 解锁 0 号曲，清 +0x754。
    CHECK(h.frame(0x80001));
    CHECK_EQ(h.host.locked_plays, 1);
    CHECK_EQ(h.host.played, 1);
    CHECK_EQ(h.host.marked, 0);  // 原版任意播曲解锁 0 号曲
    CHECK(!h.menu.locked_armed());
    CHECK(!h.host.heard[1]);     // 当前曲本身不置位（只置 score[0]）
}

TEST(music_room_locked_comments_use_cp932_placeholders) {
    Harness h;
    h.advance(16);
    h.frame(0x20);      // 曲1 未听
    h.frame(0x80001);   // 锁定响应
    h.advance(4);       // 显影 2 行
    CHECK_EQ(h.menu.revealed_lines(), 2);
    // 占位串 0x4c0da0[0..7]，色 0x8080ff；不是真实曲目注释。
    const TextSink::Entry* first = find_text(h.sink, "\x81\x40");
    CHECK(first != nullptr);
    CHECK_EQ(first->color, 0xff8080ffu);
    const std::string expected1 =
        "\x81\x40\x81\x40\x81\x96\x81\x96\x91\x49\x91\xf0\x82\xb5\x82\xbd\x8b\xc8"
        "\x82\xcd\x82\xdc\x82\xbe\x83\x51\x81\x5b\x83\x80\x92\x86\x82\xc5\x8d\xc4"
        "\x90\xb6\x82\xb3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb9\x82\xf1\x81\x96"
        "\x81\x96";
    const TextSink::Entry* second = find_text(h.sink, expected1);
    CHECK(second != nullptr);
    CHECK_EQ(second->color, 0xff8080ffu);
    CHECK(!has_text(h.sink, "c1-0"));  // 真实注释不显示
    // 二次 Z 播放后，重新显影使用真实注释（白字）。
    h.frame(0x80001);
    h.advance(4);
    const TextSink::Entry* real = find_text(h.sink, "c1-0");
    CHECK(real != nullptr);
    CHECK_EQ(real->color, 0xffffffffu);
}

TEST(music_room_navigation_clears_armed_only_after_eight_lines) {
    Harness h;
    h.advance(16);
    h.frame(0x20);      // 曲1 未听
    h.frame(0x80001);   // 锁定响应，armed=1，reveal=0
    // 显影满 8 行（14 帧：偶数帧 1..8）。
    h.advance(15);
    CHECK_EQ(h.menu.revealed_lines(), 8);
    CHECK(h.menu.locked_armed());
    // 导航分支：+0x74c>=8 → 清 +0x754（0x455d76-0x455d86）。
    h.frame(0x20);      // 曲2（已听）
    CHECK(!h.menu.locked_armed());
    CHECK_EQ(h.menu.revealed_lines(), 8);  // 导航不清显影计数
    // 回到未听曲：armed 已清 → 再次锁定响应，而不是播放。
    h.frame(0x10);      // 曲1
    CHECK(h.frame(0x80001));
    CHECK_EQ(h.host.locked_plays, 2);
    CHECK_EQ(h.host.played, -1);
    CHECK(h.menu.locked_armed());
}

TEST(music_room_navigation_before_eight_lines_keeps_armed) {
    Harness h;
    h.advance(16);
    h.frame(0x20);      // 曲1 未听
    h.frame(0x80001);   // 锁定响应，armed=1，reveal=0
    h.advance(3);       // reveal=1（未满 8）
    CHECK(h.menu.locked_armed());
    h.frame(0x20);      // 曲2（已听）：导航分支不清 +0x754
    CHECK(h.menu.locked_armed());
    h.frame(0x10);      // 曲1
    // 二次 Z：armed 仍在 → 播放 + 解锁。
    CHECK(h.frame(0x80001));
    CHECK_EQ(h.host.played, 1);
    CHECK_EQ(h.host.marked, 0);
    CHECK(!h.menu.locked_armed());
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

TEST(music_room_repeated_z_and_x_do_not_retrigger) {
    Harness h;
    h.advance(16);
    h.frame(0x20);      // 曲1 未听
    h.frame(0x80001);   // 首次 Z（pressed）：锁定响应
    CHECK_EQ(h.host.locked_plays, 1);
    // 仅 repeated 的 Z 不触发播放/二次锁定（原版 0x4554f0 只读 DAT_004e49fc）。
    CHECK(h.frame(0, 0x80001));
    CHECK_EQ(h.host.locked_plays, 1);
    CHECK_EQ(h.host.played, -1);
    CHECK(h.menu.locked_armed());
    // 仅 repeated 的 X 不退出。
    CHECK(h.frame(0, 0x102));
    CHECK_EQ(h.state.substate, 2);
}

TEST(music_room_failure_paths_are_honest) {
    Harness h;
    h.host.tracks = 1;
    h.host.heard.assign(1, false);
    h.advance(16);
    h.menu.play_sound = [](i32) { return false; };
    CHECK(!h.frame(0x102));  // 退出音 9 失败：不静默成功
    CHECK(!h.menu.error.empty());
}

int main() {
    std::printf("# title_music_room_menu_test\n");
    return th13test::run_all("title_music_room_menu_test");
}
