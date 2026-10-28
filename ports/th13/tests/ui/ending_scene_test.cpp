// Ending 场景单测（0x416570 家族）：结局索引公式各分支（好/坏/真结局变体/
// 续关）、落档位（0x1b26a 区）、滚屏解释器（op0/3/4/5/6/7/8/9/10/11/12/14/
// 15-17）、跳过输入语义（0x4e49f0/0x4e49fc/0x4e4b04 + 场景 bit0 门）、
// END → request 0x10 出口。宿主面用记录桩；msg 走合成脚本（格式与
// parse_ending_msg.py 同构；文本用 0x42a340 XOR 流加密）。
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

// ---- 0x42a340 加密（msg_decrypt_text 的逆；key=0x77、inc=0x07 起步）----
std::vector<u8> encrypt_text(const std::string& plain) {
    u32 key = 0x77, inc = 0x07;
    std::vector<u8> out;
    for (char c : plain) {
        out.push_back(u8(u8(c) ^ key));
        key = (key + inc) & 0xff;
        inc = (inc + 0x10) & 0xff;
    }
    out.push_back(u8(key));  // 解出 NUL 的终结字节
    return out;
}

struct Rec {
    u16 line;
    u8 op;
    std::vector<u8> payload;
};

// [u32 条目数=1][{u32 偏移=0xc, u32 id=0x100}][记录……]（e01/staff 头同式）。
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

// ---- 存档视图桩（0x1b26a..0x1b276 → 13 字节）----
struct MemorySave final : EndingSaveBytes {
    std::array<u8, 13> bytes{};
    u8 read(i32 index) const override {
        return index >= 0 && index < 13 ? bytes[u32(index)] : u8(0);
    }
    void write(i32 index, u8 value) override {
        if (index >= 0 && index < 13) bytes[u32(index)] = value;
    }
};

// ---- 平台记录桩 ----
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

    bool read_asset(const std::string& name, std::vector<u8>& bytes,
                    bool /*required*/) override {
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
    bool ending_page_line(i32 /*row*/, u32 color, const std::string& text) override {
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

}  // namespace

TEST(ending_index_good_bad_and_variant_branches) {
    // 0x416130:0x41624e-0x416297：base=char（th13 单机体 shot=0）。
    EndingHarness h;
    h.progress.character = 0;
    CHECK_EQ(ending_index_for(h.progress, h.save), 1);  // 好结局 e01
    h.progress.character = 3;
    CHECK_EQ(ending_index_for(h.progress, h.save), 7);  // e07
    // 续关 → base+9（坏结局 e09..e12）。
    h.progress.continues = 1;
    CHECK_EQ(ending_index_for(h.progress, h.save), 12);
    h.progress.continues = 3;
    h.progress.character = 0;
    CHECK_EQ(ending_index_for(h.progress, h.save), 9);
    // 真结局变体：难度>=1 && good_endings>=3 && 标准好结局已见 → +1。
    h.progress.continues = 0;
    h.progress.character = 2;
    h.progress.difficulty = 1;
    h.progress.good_endings = 3;
    CHECK_EQ(ending_index_for(h.progress, h.save), 5);  // 未见过 e05
    h.save.bytes[5] = 1;                                 // e05 已见
    CHECK_EQ(ending_index_for(h.progress, h.save), 6);   // e06 变体
    h.progress.good_endings = 2;
    CHECK_EQ(ending_index_for(h.progress, h.save), 5);   // 资格不足
    h.progress.good_endings = 3;
    h.progress.difficulty = 0;
    CHECK_EQ(ending_index_for(h.progress, h.save), 5);   // 难度不足
}

TEST(ending_initialize_writes_save_bits_and_scene_flags) {
    {
        EndingHarness h;
        h.progress.character = 2;
        h.progress.difficulty = 1;
        h.start("e05.msg", build_msg({}));
        CHECK_EQ(h.scene->ending_index(), 5);
        CHECK_EQ(h.save.bytes[5], 0x11);  // |= 1 |（diff>0）0x10
        CHECK_EQ(h.save.bytes[12], 1);    // idx<8 → 0x1b276 = 1
        CHECK_EQ(h.scene->scene_flags(), 3u);  // 首见 + 好结局全未见过
        CHECK_EQ(h.scene->message_name(), std::string("e05.msg"));
    }
    {
        // 复看（结局位与 0x1b276 已置）→ 场景旗 0；Easy（diff 0）不置 0x10。
        EndingHarness h;
        h.progress.character = 0;
        h.progress.difficulty = 0;
        h.save.bytes[1] = 1;
        h.save.bytes[12] = 1;
        h.start("e01.msg", build_msg({}));
        CHECK_EQ(h.save.bytes[1], 1);  // 仅 |=1（已含）
        CHECK_EQ(h.scene->scene_flags(), 0u);
    }
    {
        // 坏结局（续关）：idx = base+9 = 9（e09）→ 不写 0x1b276（idx>=8）。
        EndingHarness h;
        h.progress.character = 0;
        h.progress.continues = 1;
        h.progress.difficulty = 1;
        h.save.bytes[12] = 1;
        h.start("e09.msg", build_msg({}));
        CHECK_EQ(h.scene->ending_index(), 9);
        CHECK_EQ(h.save.bytes[9], 0x11);
        // e09 首见（bit0）+ 0x1b276 非空（bit1 清）。
        CHECK_EQ(h.scene->scene_flags(), 1u);
    }
}

TEST(ending_interpreter_runs_full_script_to_end_request) {
    EndingHarness h;
    h.progress.character = 0;
    h.progress.difficulty = 1;
    // e01 段：ANM 装载/VM cue/音乐/颜色/文本页 + 对话等待 + 淡出 + 接名单。
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
        {3, 5, u32_payload(0xffffffff)},  // 负参数 → 每帧重置 999（仅按下放行）
        {3, 14, u32_payload(0x3c)},
        {4, 0, {}},
    });
    // 帧 1：行 0 记录全执行；op5@1 在未来 → 行推进到 1。
    h.frame();
    CHECK_EQ(h.host.anm_loads.size(), 1u);
    CHECK_EQ(h.host.anm_loads[0].first, 20);  // 槽 0x14+idx
    CHECK_EQ(h.host.anm_loads[0].second, std::string("e01.anm"));
    CHECK_EQ(h.host.anm_cues.size(), 1u);
    CHECK_EQ(h.host.anm_cues[0].first, 20);
    CHECK_EQ(h.host.music.size(), 2u);  // th13_18 预载槽 0 + 播放
    CHECK_EQ(std::get<0>(h.host.music[0]), 1);
    CHECK_EQ(std::get<2>(h.host.music[0]), std::string("th13_18.wav"));
    CHECK_EQ(std::get<0>(h.host.music[1]), 2);
    CHECK_EQ(h.host.page_lines.size(), 1u);
    CHECK_EQ(h.host.page_lines[0].second, std::string("Hello ending"));
    CHECK_EQ(h.host.page_color, 0x00604040u);  // op9 颜色先行
    CHECK_EQ(h.scene->line(), 1);
    // 帧 2：op5 等待（无输入、首见封锁）。
    h.frame();
    CHECK(!h.scene->finished());
    CHECK_EQ(h.host.advance_sounds, 0);
    // 帧 3：Z 按下（0x4e49fc bit0）计声放行 → 第二行 + 淡出 + 接 staff.msg
    //（同帧继续解释新流：th13_19 起 + 名单 VM cue；行重置后推进）。
    h.frame(0, 0x1);
    CHECK_EQ(h.host.advance_sounds, 1);
    CHECK_EQ(h.host.page_lines.size(), 2u);
    CHECK_EQ(h.host.page_lines[1].second, std::string("Second line"));
    CHECK_EQ(h.host.fade_frames, 180);  // 0x43af70(3.0f)
    // op12@2 在下一帧行推进到 2 后执行：同帧接读 staff.msg 并解释新流
    //（th13_19 起 + 名单 VM cue），行重置后推进。
    h.frame();
    CHECK_EQ(h.host.music.size(), 4u);  // + th13_19 预载/播放
    CHECK_EQ(std::get<2>(h.host.music[2]), std::string("th13_19.wav"));
    CHECK_EQ(h.host.anm_cues.size(), 2u);
    CHECK_EQ(h.host.anm_cues[1].second, 5);
    // staff 尾段 op5 负参数：每帧重置 999（0x416bc2-0x416bcf 无 target 门），
    // 自然等待与 %6 持键节拍均不可达 → 仅 Z/Enter 按下放行 → op14 窗 → END。
    for (int i = 0; i < 120; i++) h.frame();
    CHECK(!h.scene->finished());
    h.frame(0, 0x1);  // 行 3 记录（op5 按下放行 + op14 窗）
    CHECK_EQ(h.host.windows.size(), 1u);
    CHECK_EQ(h.host.windows[0].first, 5);
    CHECK_EQ(h.host.windows[0].second, 0x3c);
    h.frame();  // 行 4：END
    CHECK(h.scene->finished());
    CHECK_EQ(h.host.requests.size(), 1u);
    CHECK_EQ(h.host.requests[0], 0x10);  // 0x416610 END 出口
    CHECK_EQ(h.scene->line(), 4);
}

TEST(ending_first_viewing_blocks_hold_skip) {
    // 场景 bit0（该结局首见）封锁持键快进（0x416bed）；单键按下仍放行。
    EndingHarness h;
    h.start("e01.msg", build_msg({
        {0, 5, u32_payload(6000000)},
        {1, 0, {}},
    }));
    for (int i = 0; i < 60; i++) h.frame(0x1, 0, 30);  // Z 持续、重复 ≥0x14
    CHECK(!h.scene->finished());
    CHECK_EQ(h.host.advance_sounds, 0);
    h.frame(0, 0x1);  // 按下沿
    CHECK_EQ(h.host.advance_sounds, 1);
    h.frame();        // 行推进到 1 后 END
    CHECK(h.scene->finished());
    CHECK_EQ(h.host.requests.size(), 1u);
}

TEST(ending_repeat_viewing_hold_skip_advances_every_six) {
    // 复看（bit0 清）：Z 持续 + 重复 ≥0x14 + %6 节拍放行；Ctrl（0x200）免重复门槛。
    EndingHarness h;
    h.save.bytes[1] = 1;  // e01 已见
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
    CHECK(frames <= 20);  // 两次等待 × ≤6 帧 + 行推进帧
    // Ctrl 路径：无 Z、重复计数 0。
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
    // op12 目标 msg 缺席 → 0x416000 返空 → 解释器返 -1（END 同款 0x10 出口）。
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
    // op6 页断（+0x78 清零 → 下一 op3 重建页）；op4 清文本（页计数不动）。
    EndingHarness h;
    h.progress.continues = 1;  // 坏结局脚本不接 staff，直接排到 END
    h.start("e09.msg", build_msg({
        {0, 3, encrypt_text("a")},
        {0, 3, encrypt_text("b")},
        {0, 4, {}},
        {0, 3, encrypt_text("c")},  // op4 后页计数仍 2 → 行 2
        {1, 6, u32_payload(6000000)},
        {2, 3, encrypt_text("d")},  // op6 后重建页 → 行 0 + reset
        {3, 0, {}},
    }));
    h.frame();  // 行 0 全部 op 执行（op3×3 + op4）
    CHECK_EQ(h.host.page_resets, 1);
    CHECK_EQ(h.host.page_lines.size(), 3u);
    CHECK_EQ(h.host.page_clears, 1);
    h.frame(0, 0x1);  // 行 1：op6 按下放行（页断）
    h.frame();        // 行 2：op3 重建页
    CHECK_EQ(h.host.page_resets, 2);
    CHECK_EQ(h.host.page_lines.size(), 4u);
    CHECK_EQ(h.host.page_lines[3].second, std::string("d"));
    h.frame();  // 行 3：END
    CHECK(h.scene->finished());
}

TEST(ending_difficulty_gated_cues) {
    // op15/16/17：DAT_004be7c4 == 1/2/3 时同 op8（0x416da9/0x416e43/0x416edd）。
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
            CHECK_EQ(h.host.anm_cues[0].second, difficulty);  // 脚本号 1/2/3
        }
    }
}

TEST(ending_negative_wait_requires_press) {
    // 负参数 op5（staff.msg 尾段 0xffffffff）：0x416bc2-0x416bcf 的 999 置位
    // 每帧执行（无 target<=0 门）→ 计数永不下降、%6 持键节拍 999%6!=0
    // 也不可达 → 仅 Z/Enter 按下放行（0x4e49fc & 0x80001）。
    // 注：staff.msg 表项 0 不会成为首段（索引公式值域 1..12），op12 专用。
    EndingHarness h;
    h.save.bytes[1] = 1;
    h.save.bytes[12] = 1;
    h.start("e01.msg", build_msg({
        {0, 5, u32_payload(0xffffffff)},
        {1, 0, {}},
    }));
    for (int i = 0; i < 1100; i++) h.frame();  // 长时无输入不放行
    CHECK(!h.scene->finished());
    CHECK_EQ(h.host.advance_sounds, 0);
    for (int i = 0; i < 60; i++) h.frame(0x1, 0, 30);  // 持键也不放行（999%6）
    CHECK(!h.scene->finished());
    h.frame(0, 0x1);  // 按下沿放行
    CHECK_EQ(h.host.advance_sounds, 1);
    h.frame();
    CHECK(h.scene->finished());
    CHECK_EQ(h.host.requests.size(), 1u);
}

int main() { return th13test::run_all("Ending 场景"); }
