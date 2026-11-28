#include "../harness/test_framework.hpp"

#include "game/EndingScene.hpp"

#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

using namespace th13;

namespace {

std::vector<u8> encrypt_text(const std::string& plain) {
    u32 key = 0x77, inc = 0x07;
    std::vector<u8> out;
    for (char c : plain) {
        out.push_back(u8(u8(c) ^ key));
        key = (key + inc) & 0xff;
        inc = (inc + 0x10) & 0xff;
    }
    out.push_back(u8(key));
    return out;
}

struct Rec {
    u16 line;
    u8 op;
    std::vector<u8> payload;
};

std::vector<u8> build_msg(const std::vector<Rec>& records) {
    std::vector<u8> out{1, 0, 0, 0, 0x0c, 0, 0, 0, 0x00, 0x01, 0, 0};
    for (const auto& r : records) {
        const u8 line[2] = {u8(r.line & 0xff), u8(r.line >> 8)};
        out.insert(out.end(), line, line + 2);
        out.push_back(r.op);
        out.push_back(u8(r.payload.size()));
        out.insert(out.end(), r.payload.begin(), r.payload.end());
    }
    out.push_back(0);
    out.push_back(0);
    out.push_back(0);
    out.push_back(0);
    return out;
}

std::vector<u8> op7_payload(u32 index, const std::string& name) {
    std::vector<u8> p;
    p.insert(p.end(), {u8(index), 0, 0, 0});
    p.insert(p.end(), name.begin(), name.end());
    p.push_back(0);
    return p;
}

std::vector<u8> op8_payload(u32 vm, u32 bank, u32 script) {
    std::vector<u8> p(12);
    std::memcpy(p.data(), &vm, 4);
    std::memcpy(p.data() + 4, &bank, 4);
    std::memcpy(p.data() + 8, &script, 4);
    return p;
}

std::vector<u8> u32_payload(u32 value) {
    std::vector<u8> p(4);
    std::memcpy(p.data(), &value, 4);
    return p;
}

std::vector<u8> string_payload(const std::string& text) {
    std::vector<u8> p(text.begin(), text.end());
    p.push_back(0);
    return p;
}

struct MemorySave final : EndingSaveBytes {
    std::array<u8, 13> bytes{};
    u8 read(i32 index) const override { return index >= 0 && index < 13 ? bytes[u32(index)] : u8(0); }
    void write(i32 index, u8 value) override {
        if (index >= 0 && index < 13) bytes[u32(index)] = value;
    }
};

struct RecordPlatform final : EndingPlatform {
    MemorySave* save = nullptr;
    std::map<std::string, std::vector<u8>> assets;
    std::vector<i32> requests;
    std::vector<std::tuple<i32, i32, std::string>> music;
    i32 fade_frames = -1;
    int advance_sounds = 0;
    std::vector<std::pair<i32, std::string>> anm_loads;
    std::vector<std::pair<i32, i32>> anm_cues;
    std::vector<std::pair<i32, i32>> windows;
    int page_resets = 0, page_clears = 0;
    std::vector<std::pair<i32, std::string>> page_lines;
    u32 page_color = 0;

    bool read_asset(const std::string& name, std::vector<u8>& bytes, bool) override {
        const auto found = assets.find(name);
        if (found == assets.end()) return false;
        bytes = found->second;
        return true;
    }
    EndingSaveBytes* save_bytes() override { return save; }
    bool request_destination(i32 raw) override {
        requests.push_back(raw);
        return true;
    }
    bool queue_music(i32 code, i32 value, const std::string& name) override {
        music.emplace_back(code, value, name);
        return true;
    }
    bool fade_out_music(i32 frames) override {
        fade_frames = frames;
        return true;
    }
    bool advance_sound() override {
        ++advance_sounds;
        return true;
    }
    bool load_ending_anm(i32 slot, const std::string& name) override {
        anm_loads.emplace_back(slot, name);
        return true;
    }
    bool cue_ending_anm(i32 bank_slot, i32 script) override {
        anm_cues.emplace_back(bank_slot, script);
        return true;
    }
    bool open_ending_window(i32 kind, i32 param) override {
        windows.emplace_back(kind, param);
        return true;
    }
    bool ending_page_reset() override {
        ++page_resets;
        return true;
    }
    bool ending_page_line(i32, u32 color, const std::string& text) override {
        page_lines.emplace_back(0, text);
        page_color = color;
        return true;
    }
    bool ending_page_clear() override {
        ++page_clears;
        return true;
    }
};

struct EndingHarness {
    SessionState progress;
    MemorySave save;
    RecordPlatform host;
    std::unique_ptr<EndingScene> scene;

    EndingHarness() { host.save = &save; }

    void start(const std::string& name, const std::vector<u8>& bytes) {
        host.assets[name] = bytes;
        scene = std::make_unique<EndingScene>(progress, host);
        CHECK(scene->initialize());
        CHECK(scene->error.empty());
    }

    GameInput frame(u32 held = 0, u32 pressed = 0, u32 z_repeat_age = 0) {
        GameInput input;
        input.held = held;
        input.pressed = pressed;
        input.repeat_age[0] = z_repeat_age;
        CHECK(scene->update(input));
        return input;
    }

    void run_to_finish(int limit = 4000) {
        int frames = 0;
        while (!scene->finished() && frames < limit) {
            frame();
            ++frames;
        }
    }
};

}

TEST(ending_index_good_bad_and_variant_branches) {
    EndingHarness h;
    h.progress.character = 0;
    CHECK_EQ(ending_index_for(h.progress, h.save), 1);
    h.progress.character = 3;
    CHECK_EQ(ending_index_for(h.progress, h.save), 7);
    h.progress.continues = 1;
    CHECK_EQ(ending_index_for(h.progress, h.save), 12);
    h.progress.continues = 3;
    h.progress.character = 0;
    CHECK_EQ(ending_index_for(h.progress, h.save), 9);
    h.progress.continues = 0;
    h.progress.character = 2;
    h.progress.difficulty = 1;
    h.progress.good_endings = 3;
    CHECK_EQ(ending_index_for(h.progress, h.save), 5);
    h.save.bytes[5] = 1;
    CHECK_EQ(ending_index_for(h.progress, h.save), 6);
    h.progress.good_endings = 2;
    CHECK_EQ(ending_index_for(h.progress, h.save), 5);
    h.progress.good_endings = 3;
    h.progress.difficulty = 0;
    CHECK_EQ(ending_index_for(h.progress, h.save), 5);
}

TEST(ending_initialize_writes_save_bits_and_scene_flags) {
    {
        EndingHarness h;
        h.progress.character = 2;
        h.progress.difficulty = 1;
        h.start("e05.msg", build_msg({}));
        CHECK_EQ(h.scene->ending_index(), 5);
        CHECK_EQ(h.save.bytes[5], 0x11);
        CHECK_EQ(h.save.bytes[12], 1);
        CHECK_EQ(h.scene->scene_flags(), 3u);
        CHECK_EQ(h.scene->message_name(), std::string("e05.msg"));
    }
    {
        EndingHarness h;
        h.progress.character = 0;
        h.progress.difficulty = 0;
        h.save.bytes[1] = 1;
        h.save.bytes[12] = 1;
        h.start("e01.msg", build_msg({}));
        CHECK_EQ(h.save.bytes[1], 1);
        CHECK_EQ(h.scene->scene_flags(), 0u);
    }
    {
        EndingHarness h;
        h.progress.character = 0;
        h.progress.continues = 1;
        h.progress.difficulty = 1;
        h.save.bytes[12] = 1;
        h.start("e09.msg", build_msg({}));
        CHECK_EQ(h.scene->ending_index(), 9);
        CHECK_EQ(h.save.bytes[9], 0x11);
        CHECK_EQ(h.scene->scene_flags(), 1u);
    }
}

TEST(ending_interpreter_runs_full_script_to_end_request) {
    EndingHarness h;
    h.progress.character = 0;
    h.progress.difficulty = 1;
    h.start("e01.msg", build_msg({
                           {0, 7, op7_payload(0, "e01.anm")},
                           {0, 8, op8_payload(0, 0, 0)},
                           {0, 10, string_payload("bgm/th13_18")},
                           {0, 9, u32_payload(0x00604040)},
                           {0, 3, encrypt_text("Hello ending")},
                           {1, 5, u32_payload(6000000)},
                           {1, 3, encrypt_text("Second line")},
                           {1, 11, {}},
                           {2, 12, string_payload("staff.msg")},
                       }));
    h.host.assets["staff.msg"] = build_msg({
        {0, 10, string_payload("bgm/th13_19")},
        {0, 8, op8_payload(0, 0, 5)},
        {3, 5, u32_payload(0xffffffff)},
        {3, 14, u32_payload(0x3c)},
        {4, 0, {}},
    });
    h.frame();
    CHECK_EQ(h.host.anm_loads.size(), 1u);
    if (h.host.anm_loads.size() == 1u) {
        CHECK_EQ(h.host.anm_loads[0].first, 20);
        CHECK_EQ(h.host.anm_loads[0].second, std::string("e01.anm"));
    }
    CHECK(h.host.anm_cues.empty());
    CHECK(h.host.music.empty());
    CHECK(h.host.page_lines.empty());
    CHECK_EQ(h.scene->line(), 0);
    h.frame();
    CHECK_EQ(h.host.anm_cues.size(), 1u);
    if (h.host.anm_cues.size() == 1u) CHECK_EQ(h.host.anm_cues[0].first, 20);
    CHECK_EQ(h.host.music.size(), 2u);
    if (h.host.music.size() == 2u) {
        CHECK_EQ(std::get<0>(h.host.music[0]), 1);
        CHECK_EQ(std::get<2>(h.host.music[0]), std::string("th13_18.wav"));
        CHECK_EQ(std::get<0>(h.host.music[1]), 2);
    }
    CHECK_EQ(h.host.page_lines.size(), 1u);
    if (h.host.page_lines.size() == 1u) CHECK_EQ(h.host.page_lines[0].second, std::string("Hello ending"));
    CHECK_EQ(h.host.page_color, 0x00604040u);
    CHECK_EQ(h.scene->line(), 1);
    h.frame();
    CHECK(!h.scene->finished());
    CHECK_EQ(h.host.advance_sounds, 0);
    h.frame(0, 0x1);
    CHECK_EQ(h.host.advance_sounds, 1);
    CHECK_EQ(h.host.page_lines.size(), 2u);
    if (h.host.page_lines.size() == 2u) CHECK_EQ(h.host.page_lines[1].second, std::string("Second line"));
    CHECK_EQ(h.host.fade_frames, 180);
    h.frame();
    CHECK_EQ(h.host.music.size(), 4u);
    if (h.host.music.size() == 4u) CHECK_EQ(std::get<2>(h.host.music[2]), std::string("th13_19.wav"));
    CHECK_EQ(h.host.anm_cues.size(), 2u);
    if (h.host.anm_cues.size() == 2u) CHECK_EQ(h.host.anm_cues[1].second, 5);
    for (int i = 0; i < 120; i++)
        h.frame();
    CHECK(!h.scene->finished());
    h.frame(0, 0x1);
    CHECK_EQ(h.host.windows.size(), 1u);
    if (h.host.windows.size() == 1u) {
        CHECK_EQ(h.host.windows[0].first, 5);
        CHECK_EQ(h.host.windows[0].second, 0x3c);
    }
    h.frame();
    CHECK(h.scene->finished());
    CHECK_EQ(h.host.requests.size(), 1u);
    if (h.host.requests.size() == 1u) CHECK_EQ(h.host.requests[0], 0x10);
    CHECK_EQ(h.scene->line(), 4);
}

TEST(ending_first_viewing_blocks_hold_skip) {
    EndingHarness h;
    h.start("e01.msg", build_msg({
                           {0, 5, u32_payload(6000000)},
                           {1, 0, {}},
                       }));
    for (int i = 0; i < 60; i++)
        h.frame(0x1, 0, 30);
    CHECK(!h.scene->finished());
    CHECK_EQ(h.host.advance_sounds, 0);
    h.frame(0, 0x1);
    CHECK_EQ(h.host.advance_sounds, 1);
    h.frame();
    CHECK(h.scene->finished());
    CHECK_EQ(h.host.requests.size(), 1u);
}

TEST(ending_repeat_viewing_hold_skip_advances_every_six) {
    EndingHarness h;
    h.save.bytes[1] = 1;
    h.start("e01.msg", build_msg({
                           {0, 5, u32_payload(6000000)},
                           {1, 5, u32_payload(6000000)},
                           {2, 0, {}},
                       }));
    int frames = 0;
    while (!h.scene->finished() && frames < 40) {
        h.frame(0x1, 0, 30);
        frames++;
    }
    CHECK(h.scene->finished());
    CHECK(frames <= 20);
    EndingHarness ctrl;
    ctrl.save.bytes[1] = 1;
    ctrl.start("e01.msg", build_msg({
                              {0, 5, u32_payload(6000000)},
                              {1, 0, {}},
                          }));
    int ctrl_frames = 0;
    while (!ctrl.scene->finished() && ctrl_frames < 40) {
        ctrl.frame(0x200, 0, 0);
        ctrl_frames++;
    }
    CHECK(ctrl.scene->finished());
}

TEST(ending_reload_failure_exits_like_end) {
    EndingHarness h;
    h.start("e01.msg", build_msg({
                           {0, 12, string_payload("staff.msg")},
                       }));
    h.frame();
    CHECK(h.scene->finished());
    CHECK_EQ(h.host.requests.size(), 1u);
    CHECK_EQ(h.host.requests[0], 0x10);
}

TEST(ending_page_break_and_clear_state) {
    EndingHarness h;
    h.progress.continues = 1;
    h.start("e09.msg", build_msg({
                           {0, 3, encrypt_text("a")},
                           {0, 3, encrypt_text("b")},
                           {0, 4, {}},
                           {0, 3, encrypt_text("c")},
                           {1, 6, u32_payload(6000000)},
                           {2, 3, encrypt_text("d")},
                           {3, 0, {}},
                       }));
    h.frame();
    CHECK_EQ(h.host.page_resets, 1);
    CHECK_EQ(h.host.page_lines.size(), 3u);
    CHECK_EQ(h.host.page_clears, 1);
    h.frame(0, 0x1);
    h.frame();
    CHECK_EQ(h.host.page_resets, 2);
    CHECK_EQ(h.host.page_lines.size(), 4u);
    CHECK_EQ(h.host.page_lines[3].second, std::string("d"));
    h.frame();
    CHECK(h.scene->finished());
}

TEST(ending_difficulty_gated_cues) {
    for (i32 difficulty = 0; difficulty <= 3; difficulty++) {
        EndingHarness h;
        h.progress.difficulty = difficulty;
        h.start("e01.msg", build_msg({
                               {0, 15, op8_payload(0, 0, 1)},
                               {0, 16, op8_payload(1, 0, 2)},
                               {0, 17, op8_payload(2, 0, 3)},
                               {1, 0, {}},
                           }));
        h.frame();
        h.frame();
        CHECK(h.scene->finished());
        if (difficulty == 0) {
            CHECK(h.host.anm_cues.empty());
        } else {
            CHECK_EQ(h.host.anm_cues.size(), 1u);
            if (h.host.anm_cues.size() == 1u) CHECK_EQ(h.host.anm_cues[0].second, difficulty);
        }
    }
}

TEST(ending_negative_wait_requires_press) {
    EndingHarness h;
    h.save.bytes[1] = 1;
    h.save.bytes[12] = 1;
    h.start("e01.msg", build_msg({
                           {0, 5, u32_payload(0xffffffff)},
                           {1, 0, {}},
                       }));
    for (int i = 0; i < 1100; i++)
        h.frame();
    CHECK(!h.scene->finished());
    CHECK_EQ(h.host.advance_sounds, 0);
    for (int i = 0; i < 60; i++)
        h.frame(0x1, 0, 30);
    CHECK(!h.scene->finished());
    h.frame(0, 0x1);
    CHECK_EQ(h.host.advance_sounds, 1);
    h.frame();
    CHECK(h.scene->finished());
    CHECK_EQ(h.host.requests.size(), 1u);
}

int main() { return th13test::run_all("Ending 场景"); }
