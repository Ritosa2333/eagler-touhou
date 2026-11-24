#include "../harness/test_framework.hpp"

#include "game/MsgPlayer.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

using namespace th13;

namespace {

std::vector<u8> encrypt_text(const std::string& plain) {
    std::vector<u8> out;
    u32 key = 0x77, inc = 0x07;
    for (char ch : plain) {
        out.push_back(u8(u8(ch) ^ key));
        key = (key + inc) & 0xFF;
        inc = (inc + 0x10) & 0xFF;
    }
    out.push_back(u8(key));
    return out;
}

struct InstructionBuilder {
    std::vector<u8> bytes;
    void add(u16 time, u8 opcode, const std::vector<u8>& args = {}) {
        bytes.push_back(u8(time & 0xFF));
        bytes.push_back(u8(time >> 8));
        bytes.push_back(opcode);
        bytes.push_back(u8(args.size()));
        bytes.insert(bytes.end(), args.begin(), args.end());
    }
    void text(u16 time, const std::string& plain) { add(time, 0x11, encrypt_text(plain)); }
    void integer(u16 time, u8 opcode, i32 value) {
        std::vector<u8> args(4);
        std::memcpy(args.data(), &value, 4);
        add(time, opcode, args);
    }
    void floats(u16 time, u8 opcode, float a, float b) {
        std::vector<u8> args(8);
        std::memcpy(args.data(), &a, 4);
        std::memcpy(args.data() + 4, &b, 4);
        add(time, opcode, args);
    }
    void end(u16 time) {
        add(time, 0);
        bytes.insert(bytes.end(), 4, 0);
    }
};

void append_u32(std::vector<u8>& out, u32 value) {
    for (u32 i = 0; i < 4; i++)
        out.push_back(u8(value >> (i * 8)));
}

std::vector<u8> build_msg(const std::vector<std::vector<u8>>& entries) {
    std::vector<u8> out;
    append_u32(out, u32(entries.size()));
    u32 offset = 4 + u32(entries.size()) * 8;
    std::vector<u32> offsets;
    for (const auto& entry : entries) {
        offsets.push_back(offset);
        offset += u32(entry.size());
    }
    for (u32 value : offsets) {
        append_u32(out, value);
        append_u32(out, 0x100);
    }
    for (const auto& entry : entries)
        out.insert(out.end(), entry.begin(), entry.end());
    return out;
}

struct RecordHost final : MsgHost {
    std::vector<i32> bgm_calls, clears, boss_calls, interrupts, speakers, releases;
    std::vector<std::pair<i32, float>> music_fades;
    i32 portraits = 0, blips = 0;
    void msg_bgm(i32 opcode) override { bgm_calls.push_back(opcode); }
    void msg_portrait_load() override { portraits++; }
    void msg_boss_battle() override { boss_calls.push_back(1); }
    void msg_music_fade(i32 opcode) override { music_fades.emplace_back(opcode, -1.f); }
    void msg_music_fade(i32 opcode, float seconds) override { music_fades.emplace_back(opcode, seconds); }
    void msg_stage_clear() override { clears.push_back(1); }
    void msg_advance_blip() override { blips++; }
    void msg_release(i32 opcode) override { releases.push_back(opcode); }
    void msg_portrait_interrupt(i32 opcode, i32 label) override { interrupts.push_back(opcode * 1000 + label); }
    void msg_speaker_switch(i32 opcode) override { speakers.push_back(opcode); }
};

}

TEST(msg_header_and_entry_table) {
    InstructionBuilder first, second;
    first.add(0, 0x01);
    first.end(10);
    second.add(0, 0x02);
    second.end(20);
    const auto bytes = build_msg({first.bytes, second.bytes});
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    CHECK_EQ(file.entry_count(), 2u);
    CHECK_EQ(file.entry(0).offset, 4 + 2 * 8);
    CHECK_EQ(file.entry(0).id, 0x100u);
    CHECK_EQ(file.entry(1).offset, u32(4 + 2 * 8 + first.bytes.size()));
    CHECK_EQ(file.entry(1).id, 0x100u);
}

TEST(msg_header_rejects_truncated) {
    std::vector<u8> bad{2, 0, 0, 0, 0x40, 0, 0, 0};
    MsgFile file;
    std::string error;
    CHECK(!file.open(bad.data(), u32(bad.size()), error));
}

TEST(msg_decrypt_roundtrip) {
    const auto encrypted = encrypt_text("Hello!");
    const auto decrypted = msg_decrypt_text(encrypted.data(), u32(encrypted.size()));
    CHECK_EQ(decrypted, std::string("Hello!\0", 7));
    const auto japanese = encrypt_text(std::string("\x97\x48\x81\x58\x8E\x71\x97\x6C", 8));
    const auto decoded = msg_decrypt_text(japanese.data(), u32(japanese.size()));
    CHECK_EQ(decoded, std::string("\x97\x48\x81\x58\x8E\x71\x97\x6C"
                                  "\0",
                                  9));
}

TEST(cp932_to_utf8_samples) {
    const u8 yuyuko[] = {0x97, 0x48, 0x81, 0x58, 0x8E, 0x71, 0x97, 0x6C};
    CHECK_EQ(cp932_to_utf8(yuyuko, sizeof yuyuko), std::string(u8"幽々子様"));
    const u8 half[] = {0xB1, 'A', 'z'};
    CHECK_EQ(cp932_to_utf8(half, sizeof half), std::string(u8"ｱAz"));
    const u8 first_line[] = {0x82, 0xB3, 0x81, 0x5B, 0x82, 0xC4, 0x81, 0x41, 0x97, 0x48, 0x97, 0xEC};
    CHECK_EQ(cp932_to_utf8(first_line, sizeof first_line), std::string(u8"さーて、幽霊"));
}

TEST(msg_opcode21_fires_at_scheduled_clock) {
    InstructionBuilder entry;
    entry.integer(0, 0x0a, 1);
    entry.add(0, 0x07);
    entry.floats(0, 0x1c, 112.f, 288.f);
    entry.text(0, "First line");
    entry.integer(10, 0x0b, 600);
    entry.add(30, 0x15);
    entry.add(31, 0x16);
    entry.integer(40, 0x0a, 0);
    entry.end(50);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    for (int frame = 0; frame < 20; frame++)
        CHECK(player.step(false));
    CHECK(player.step(true));
    CHECK_EQ(player.clock(), 11u);
    for (int frame = 0; frame < 19; frame++)
        CHECK(player.step(false));
    CHECK_EQ(player.clock(), 30u);
    CHECK_EQ(host.clears.size(), 0u);
    CHECK(player.step(false));
    CHECK_EQ(host.clears.size(), 1u);
    CHECK_EQ(host.music_fades.size(), 0u);
    CHECK(player.step(false));
    CHECK_EQ(host.music_fades.size(), 1u);
    if (host.music_fades.size() == 1u) {
        CHECK_EQ(host.music_fades[0].first, 0x16);
        CHECK_EQ(host.music_fades[0].second, 0.0f);
    }
    CHECK_EQ(host.boss_calls.size(), 0u);
    while (player.step(false)) {
    }
    CHECK(!player.active());
    CHECK(player.done());
    CHECK_EQ(host.clears.size(), 1u);
}

TEST(msg_music_fade_opcodes_22_and_27_do_not_start_boss_battle) {
    std::vector<u8> seconds(4);
    const float fade = 3.5f;
    std::memcpy(seconds.data(), &fade, sizeof fade);
    InstructionBuilder entry;
    entry.add(0, 0x16);
    entry.add(1, 0x1b, seconds);
    entry.end(2);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    CHECK(player.step(false));
    CHECK_EQ(host.music_fades.size(), 1u);
    if (host.music_fades.size() == 1u) {
        CHECK_EQ(host.music_fades[0].first, 0x16);
        CHECK_EQ(host.music_fades[0].second, 0.0f);
    }
    CHECK(player.step(false));
    CHECK_EQ(host.music_fades.size(), 2u);
    if (host.music_fades.size() == 2u) {
        CHECK_EQ(host.music_fades[1].first, 0x1b);
        CHECK_NEAR(host.music_fades[1].second, 3.5f, 1e-6);
    }
    CHECK(host.boss_calls.empty());
    CHECK(host.clears.empty());
}

TEST(msg_wait_blocks_until_z_or_auto) {
    InstructionBuilder entry;
    entry.text(0, "Line");
    entry.integer(0, 0x0b, 600);
    entry.text(100, "After");
    entry.end(110);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    player.step(false);
    for (int frame = 0; frame < 60; frame++)
        player.step(false);
    CHECK(player.active());
    CHECK_EQ(player.state().lines.size(), 1u);
    CHECK(player.step(true));
    CHECK_EQ(player.state().lines.size(), 1u);
    for (int frame = 0; frame < 99; frame++)
        player.step(false);
    CHECK_EQ(player.clock(), 100u);
    CHECK_EQ(player.state().lines.size(), 1u);
    player.step(false);
    CHECK_EQ(player.state().lines.size(), 1u);
    CHECK_EQ(player.state().lines[0], std::string("After"));
}

TEST(msg_wait_release_paths_original_semantics) {
    InstructionBuilder entry;
    entry.integer(0, 0x0a, 1);
    entry.text(0, "Auto");
    entry.integer(0, 0x0b, 600);
    entry.text(1, "Next");
    entry.end(10);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    player.step(false);
    CHECK_EQ(player.state().lines.size(), 1u);
    for (int frame = 0; frame < 40; frame++)
        player.step(false);
    CHECK_EQ(player.state().lines.size(), 1u);
    CHECK_EQ(player.clock(), 0u);
    for (int frame = 0; frame < 19; frame++) {
        MsgAdvance input;
        input.held = 0x200;
        input.pressed = frame == 0 ? 0x200 : 0;
        input.ctrl_frames = u32(frame + 1);
        player.step(input);
    }
    CHECK_EQ(player.state().lines.size(), 1u);
    MsgAdvance skip;
    skip.held = 0x200;
    skip.ctrl_frames = 0x14;
    player.step(skip);
    CHECK_EQ(player.clock(), 1u);
    CHECK_EQ(player.state().lines.size(), 1u);
    player.step(skip);
    CHECK_EQ(player.state().lines.size(), 1u);
    CHECK_EQ(player.state().lines[0], std::string("Next"));
    CHECK_EQ(host.blips, 0);
}

TEST(msg_wait_skip_requires_auto_bit_and_keys) {
    InstructionBuilder entry;
    entry.integer(0, 0x0a, 0);
    entry.text(0, "Manual");
    entry.integer(0, 0x0b, 600);
    entry.text(1, "After");
    entry.end(10);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    player.step(false);
    for (int frame = 0; frame < 30; frame++) {
        MsgAdvance input;
        input.held = 0x200;
        input.ctrl_frames = 0x14;
        player.step(input);
    }
    CHECK_EQ(player.state().lines.size(), 1u);
    InstructionBuilder auto_entry;
    auto_entry.integer(0, 0x0a, 1);
    auto_entry.text(0, "A");
    auto_entry.integer(0, 0x0b, 600);
    auto_entry.text(1, "B");
    auto_entry.end(10);
    const auto auto_bytes = build_msg({auto_entry.bytes});
    MsgFile auto_file;
    CHECK(auto_file.open(auto_bytes.data(), u32(auto_bytes.size()), error));
    RecordHost auto_host;
    MsgPlayer auto_player;
    CHECK(auto_player.open(auto_file, 0, auto_host, error));
    auto_player.step(false);
    for (int frame = 0; frame < 30; frame++) {
        MsgAdvance input;
        input.ctrl_frames = 0x14;
        auto_player.step(input);
    }
    CHECK_EQ(auto_player.state().lines.size(), 1u);
    MsgAdvance z_skip;
    z_skip.pressed = 0x200;
    z_skip.held = 0x201;
    z_skip.z_frames = 0x14;
    auto_player.step(z_skip);
    z_skip.pressed = 0;
    auto_player.step(z_skip);
    CHECK_EQ(auto_player.state().lines[0], std::string("B"));
    CHECK_EQ(auto_host.blips, 0);
}

TEST(msg_wait_timeout_blips_and_enter_press) {
    InstructionBuilder entry;
    entry.text(0, "T");
    entry.integer(0, 0x0b, 5);
    entry.text(1, "Done");
    entry.end(10);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    player.step(false);
    for (int frame = 0; frame < 3; frame++)
        player.step(false);
    CHECK_EQ(player.state().lines.size(), 1u);
    player.step(false);
    CHECK_EQ(host.blips, 1);
    CHECK_EQ(player.state().lines.size(), 1u);
    player.step(false);
    CHECK_EQ(player.state().lines[0], std::string("Done"));
    InstructionBuilder second;
    second.text(0, "E");
    second.integer(0, 0x0b, 600);
    second.text(1, "Go");
    second.end(10);
    const auto second_bytes = build_msg({second.bytes});
    MsgFile second_file;
    CHECK(second_file.open(second_bytes.data(), u32(second_bytes.size()), error));
    RecordHost second_host;
    MsgPlayer second_player;
    CHECK(second_player.open(second_file, 0, second_host, error));
    second_player.step(false);
    MsgAdvance enter;
    enter.held = 0x80000;
    second_player.step(enter);
    CHECK_EQ(second_host.blips, 0);
    enter.pressed = 0x80000;
    second_player.step(enter);
    CHECK_EQ(second_host.blips, 1);
    CHECK_EQ(second_player.state().lines.size(), 1u);
    enter.pressed = 0;
    second_player.step(enter);
    CHECK_EQ(second_player.state().lines[0], std::string("Go"));
}

TEST(msg_future_end_and_opcode12_do_not_bypass_time_gate) {
    InstructionBuilder entry;
    entry.add(0, 12);
    entry.add(6, 12);
    entry.end(8);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    MsgPlayer player;
    RecordHost host;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    CHECK(player.open(file, 0, host, error));
    for (u32 update = 0; update < 8; ++update) {
        CHECK(player.step(false));
        CHECK_EQ(player.clock(), update + 1);
    }
    CHECK(!player.step(false));
    CHECK_EQ(player.clock(), 8u);
}

TEST(msg_held_before_dialogue_does_not_arm_skip_and_zero_timeout_passes) {
    InstructionBuilder entry;
    entry.integer(0, 10, 1);
    entry.integer(0, 11, 40);
    entry.end(5);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    MsgPlayer player;
    RecordHost host;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    CHECK(player.open(file, 0, host, error));
    MsgAdvance input;
    input.held = 1;
    input.z_frames = 100;
    for (u32 frame = 0; frame < 30; ++frame)
        CHECK(player.step(input));
    CHECK(!player.state().skip_request);
    CHECK_EQ(player.clock(), 0u);
    CHECK_EQ(host.blips, 0);
    InstructionBuilder immediate;
    immediate.integer(0, 11, 0);
    immediate.end(0);
    const auto zero_bytes = build_msg({immediate.bytes});
    MsgFile zero;
    CHECK(zero.open(zero_bytes.data(), u32(zero_bytes.size()), error));
    CHECK(player.open(zero, 0, host, error));
    CHECK(!player.step(false));
    CHECK_EQ(host.blips, 1);
}

TEST(msg_expression_interrupt_labels_and_speaker_switch) {
    InstructionBuilder entry;
    entry.add(0, 0x07);
    entry.integer(0, 0x0d, 3);
    entry.integer(0, 0x0e, 0);
    entry.add(0, 0x08);
    entry.end(5);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    player.step(false);
    CHECK_EQ(host.interrupts.size(), 2u);
    CHECK_EQ(host.interrupts[0], 13 * 1000 + 20);
    CHECK_EQ(host.interrupts[1], 14 * 1000 + 17);
    CHECK_EQ(host.speakers.size(), 2u);
    CHECK_EQ(player.state().side, 1);
}

TEST(cp932_font_mapping_matches_runtime_raster_contract) {
    std::vector<u8> mapping;
    CHECK(build_cp932_font_mapping(mapping));
    CHECK_EQ(mapping.size(), 131072u);
    auto at = [&mapping](u32 index) { return u16(mapping[index * 2]) | (u16(mapping[index * 2 + 1]) << 8); };
    CHECK_EQ(at('A'), 0x41u);
    CHECK_EQ(at(0xB1u), 0xFF71u);
    CHECK_EQ(at(0x82B3u), 0x3055u);
    CHECK_EQ(at(0x9748u), 0x5E7Du);
    CHECK_EQ(at(0x8140u), 0x3000u);
    CHECK_EQ(at(0x80u), 0x30FBu);
    CHECK_EQ(at(0xA141u), 0x30FBu);
    CHECK_EQ(at(0x8100u), 0x30FBu);
    CHECK_EQ(at(0xFD00u), 0x30FBu);
    const u8 yuyuko[] = {0x97, 0x48, 0x81, 0x58, 0x8E, 0x71, 0x97, 0x6C};
    const auto utf8 = cp932_to_utf8(yuyuko, sizeof yuyuko);
    CHECK_EQ(utf8, std::string(u8"幽々子様"));
    CHECK_EQ(at(0x8158u), 0x3005u);
    CHECK_EQ(at(0x8E71u), 0x5B50u);
    CHECK_EQ(at(0x976Cu), 0x69D8u);
}

TEST(msg_line_pairing_two_per_block) {
    InstructionBuilder entry;
    entry.add(0, 0x07);
    entry.text(0, "one");
    entry.text(0, "two");
    entry.text(0, "three");
    entry.end(5);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    player.step(false);
    CHECK_EQ(player.state().lines.size(), 1u);
    CHECK_EQ(player.state().lines[0], std::string("three"));
    CHECK_EQ(player.state().side, 0);
}

TEST(msg_side_and_box_position) {
    InstructionBuilder entry;
    entry.add(0, 0x08);
    entry.floats(0, 0x1c, 336.f, 272.f);
    entry.text(0, "right");
    entry.end(5);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    player.step(false);
    CHECK_EQ(player.state().side, 1);
    CHECK_EQ(player.state().box_x, 336.f);
    CHECK_EQ(player.state().box_y, 272.f);
    CHECK_EQ(player.state().lines_meta.size(), 1u);
    CHECK_EQ(player.state().lines_meta[0].side, 1);
    CHECK_EQ(player.state().lines_meta[0].bubble_width, 4.0f);
}

TEST(msg_bubble_width_excludes_nul_terminator) {
    InstructionBuilder entry;
    entry.text(0, "\x82\xa0\x82\xa2\x82\xa4");
    entry.end(5);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    player.step(false);
    CHECK_EQ(player.state().lines_meta.size(), 1u);
    CHECK_EQ(player.state().lines_meta[0].bubble_width, 20.0f);
}

TEST(msg_real_st01a_bytes) {
    std::vector<std::string> candidates;
    if (const char* override_dir = std::getenv("TH13_RAW_DIR"))
        candidates.push_back(std::string(override_dir) + "/st01a.msg");
    candidates.push_back("artifacts/reference/raw/st01a.msg");
    candidates.push_back("../artifacts/reference/raw/st01a.msg");
    candidates.push_back("../../../artifacts/reference/raw/st01a.msg");
    FILE* asset = nullptr;
    for (const auto& path : candidates) {
        asset = std::fopen(path.c_str(), "rb");
        if (asset) break;
    }
    if (!asset) {
        std::printf("# 跳过：st01a.msg 不在本机（相对 repo 根）\n");
        return;
    }
    std::vector<u8> bytes;
    u8 buffer[4096];
    size_t got;
    while ((got = std::fread(buffer, 1, sizeof buffer, asset)) > 0)
        bytes.insert(bytes.end(), buffer, buffer + got);
    std::fclose(asset);
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    CHECK_EQ(file.entry_count(), 2u);
    CHECK_EQ(file.entry(1).offset, 0x334u);
    CHECK_EQ(file.entry(0).offset, 0x14u);
    i32 clears[2] = {0, 0};
    i32 texts_entry0 = 0;
    std::string first_text;
    for (u32 index = 0; index < 2; index++) {
        const u8* cursor = file.bytes() + file.entry(index).offset;
        while (cursor + 4 <= file.bytes() + file.size()) {
            u16 time = 0;
            std::memcpy(&time, cursor, 2);
            const u8 opcode = cursor[2];
            const u8 arg_bytes = cursor[3];
            if (opcode == 0) break;
            if (opcode == 0x15) clears[index]++;
            if (opcode == 0x11 && index == 0) {
                texts_entry0++;
                if (first_text.empty())
                    first_text =
                        cp932_to_utf8(reinterpret_cast<const u8*>(msg_decrypt_text(cursor + 4, arg_bytes).data()),
                                      u32(msg_decrypt_text(cursor + 4, arg_bytes).size()));
            }
            cursor += 4 + arg_bytes;
        }
    }
    CHECK_EQ(clears[0], 0);
    CHECK_EQ(clears[1], 1);
    CHECK(texts_entry0 >= 10);
    CHECK_EQ(first_text, std::string(u8"さーて、幽霊の親玉を倒さないとね！"));
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    for (int frame = 0; frame < 40 && player.state().lines_meta.empty(); frame++)
        player.step(false);
    CHECK_EQ(player.state().lines_meta.size(), 1u);
    CHECK_EQ(player.state().lines_meta[0].bubble_width, 244.0f);
}

TEST(msg_ruby_and_cp932_metadata) {
    InstructionBuilder entry;
    entry.text(0, "|4,14,\x82\xb1\x82\xb1");
    entry.text(0, "\x96\xbb\x8a\x45\x82\xc9\x97\x88\x82\xbd\x82\xcc\x82\xcd\x8c\xa9\x93\x96\x88\xe1\x82\xa2\x82\xc1\x82"
                  "\xc4\x8e\x96\x81\x48");
    entry.end(10);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    player.step(false);

    CHECK_EQ(player.state().lines.size(), 1u);
    CHECK_EQ(player.state().lines_meta.size(), 1u);
    const auto& meta = player.state().lines_meta[0];
    CHECK(meta.has_ruby);
    CHECK_EQ(meta.ruby_x, 4);
    CHECK_EQ(meta.ruby_spacing, 14);
    CHECK_EQ(meta.ruby_utf8, std::string(u8"ここ"));
    CHECK_EQ(meta.text_utf8, std::string(u8"冥界に来たのは見当違いって事？"));
    CHECK(meta.bubble_width > 0.0f);
    CHECK_EQ(meta.text_attr, 0u);
}

TEST(msg_release_ops_keep_lines_for_vm_fade) {
    for (u8 opcode : {u8(0x06), u8(0x12)}) {
        InstructionBuilder entry;
        entry.text(0, "Line");
        entry.integer(0, 0x0b, 1);
        entry.add(0, opcode);
        entry.end(5);
        const auto bytes = build_msg({entry.bytes});
        MsgFile file;
        std::string error;
        CHECK(file.open(bytes.data(), u32(bytes.size()), error));
        RecordHost host;
        MsgPlayer player;
        CHECK(player.open(file, 0, host, error));
        player.step(false);
        CHECK_EQ(host.releases.size(), 1u);
        CHECK_EQ(host.releases[0], i32(opcode));
        CHECK_EQ(player.state().lines.size(), 1u);
        CHECK_EQ(player.state().lines_meta.size(), 1u);
        CHECK_EQ(player.state().lines_meta[0].text_utf8, std::string("Line"));
    }
}

TEST(msg_text_attr_style_bits_follow_original_ops) {
    InstructionBuilder entry;
    entry.integer(0, 0x1d, 1);
    entry.text(0, "A");
    entry.integer(5, 0x0b, 600);
    entry.integer(10, 0x1a, 0);
    entry.text(10, "B");
    entry.integer(15, 0x0b, 600);
    entry.add(20, 0x07);
    entry.text(20, "C");
    entry.integer(25, 0x0b, 600);
    entry.end(30);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    player.step(false);
    CHECK_EQ(player.state().lines_meta.size(), 1u);
    CHECK_EQ(player.state().lines_meta[0].text_attr, 4u);
    for (int frame = 0; frame < 5; frame++)
        player.step(false);
    player.step(true);
    for (int frame = 0; frame < 5; frame++)
        player.step(false);
    CHECK_EQ(player.state().lines_meta.size(), 1u);
    CHECK_EQ(player.state().lines_meta[0].text_attr, 6u);
    CHECK_EQ(((player.state().lines_meta[0].text_attr >> 1) & 1u) + 4u, 5u);
    for (int frame = 0; frame < 5; frame++)
        player.step(false);
    player.step(true);
    for (int frame = 0; frame < 5; frame++)
        player.step(false);
    CHECK_EQ(player.state().lines_meta.size(), 1u);
    CHECK_EQ(player.state().lines_meta[0].text_attr, 4u);
    CHECK_EQ(((player.state().lines_meta[0].text_attr >> 1) & 1u) + 4u, 4u);
}

int main() { return th13test::run_all("msg 对话系统"); }
