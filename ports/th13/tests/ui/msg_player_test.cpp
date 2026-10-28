// msg 对话系统单测（S3 第一阶段）：MSG13 头/表解析、0x42a340 文本解密、
// cp932→UTF-8、指令推进/时间门、opcode 21 终局回调、等输入与两行配对语义；
// 尾部对真实 st01a.msg 的字节级用例（资产缺席时跳过）。
#include "../harness/test_framework.hpp"

#include "game/MsgPlayer.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

using namespace th13;

namespace {

// 0x42a340 的逆：明文（含 NUL 终止）→ 密文字节。
std::vector<u8> encrypt_text(const std::string& plain) {
    std::vector<u8> out;
    u32 key = 0x77, inc = 0x07;
    for (char ch : plain) {
        out.push_back(u8(u8(ch) ^ key));
        key = (key + inc) & 0xFF;
        inc = (inc + 0x10) & 0xFF;
    }
    out.push_back(u8(key));  // NUL 的密文位
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
        bytes.insert(bytes.end(), 4, 0);  // entry 尾 4 零字节填充（staff.msg 实证）
    }
};

void append_u32(std::vector<u8>& out, u32 value) {
    for (u32 i = 0; i < 4; i++) out.push_back(u8(value >> (i * 8)));
}

// 头（u32 count）+ 每项 8 字节表 {offset,id=0x100} + 指令流。
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
    for (const auto& entry : entries) out.insert(out.end(), entry.begin(), entry.end());
    return out;
}

struct RecordHost final : MsgHost {
    std::vector<i32> bgm_calls, clears, boss_calls, interrupts, speakers, releases;
    i32 portraits = 0, blips = 0;
    void msg_bgm(i32 opcode) override { bgm_calls.push_back(opcode); }
    void msg_portrait_load() override { portraits++; }
    void msg_boss_battle() override { boss_calls.push_back(1); }
    void msg_stage_clear() override { clears.push_back(1); }
    void msg_advance_blip() override { blips++; }
    void msg_release(i32 opcode) override { releases.push_back(opcode); }
    void msg_portrait_interrupt(i32 opcode, i32 label) override {
        interrupts.push_back(opcode * 1000 + label);
    }
    void msg_speaker_switch(i32 opcode) override { speakers.push_back(opcode); }
};

}  // namespace

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
    CHECK_EQ(file.entry(0).offset, 4 + 2 * 8);  // 指令流起点 = 4+count*8
    CHECK_EQ(file.entry(0).id, 0x100u);
    CHECK_EQ(file.entry(1).offset, u32(4 + 2 * 8 + first.bytes.size()));
    CHECK_EQ(file.entry(1).id, 0x100u);
}

TEST(msg_header_rejects_truncated) {
    std::vector<u8> bad{2, 0, 0, 0, 0x40, 0, 0, 0};  // 声称 2 项但表不完整
    MsgFile file;
    std::string error;
    CHECK(!file.open(bad.data(), u32(bad.size()), error));
}

TEST(msg_decrypt_roundtrip) {
    // 0x42a340：key=0x77 起步、key+=inc、inc+=0x10（u8 环绕）。
    const auto encrypted = encrypt_text("Hello!");
    const auto decrypted = msg_decrypt_text(encrypted.data(), u32(encrypted.size()));
    CHECK_EQ(decrypted, std::string("Hello!\0", 7));  // 含 NUL 终止
    const auto japanese = encrypt_text(std::string("\x97\x48\x81\x58\x8E\x71\x97\x6C", 8));
    const auto decoded = msg_decrypt_text(japanese.data(), u32(japanese.size()));
    CHECK_EQ(decoded, std::string("\x97\x48\x81\x58\x8E\x71\x97\x6C"
                                  "\0",
                                  9));
}

TEST(cp932_to_utf8_samples) {
    // 幽々子様（cp932 97 48 81 58 8E 71 97 6C——st01a 对话数据实证用字）
    const u8 yuyuko[] = {0x97, 0x48, 0x81, 0x58, 0x8E, 0x71, 0x97, 0x6C};
    CHECK_EQ(cp932_to_utf8(yuyuko, sizeof yuyuko), std::string(u8"幽々子様"));
    // 半角片假名 ｱ→U+FF71、ASCII 直通
    const u8 half[] = {0xB1, 'A', 'z'};
    CHECK_EQ(cp932_to_utf8(half, sizeof half), std::string(u8"ｱAz"));
    // さーて、…（st01a entry0 首句）
    const u8 first_line[] = {0x82, 0xB3, 0x81, 0x5B, 0x82, 0xC4, 0x81, 0x41,
                             0x97, 0x48, 0x97, 0xEC};
    CHECK_EQ(cp932_to_utf8(first_line, sizeof first_line), std::string(u8"さーて、幽霊"));
}

TEST(msg_opcode21_fires_at_scheduled_clock) {
    InstructionBuilder entry;
    entry.integer(0, 0x0a, 1);  // 10;1 自动推进
    entry.add(0, 0x07);         // 左侧说话人
    entry.floats(0, 0x1c, 112.f, 288.f);
    entry.text(0, "First line");
    entry.integer(10, 0x0b, 600);  // 11;600 等输入
    entry.add(30, 0x15);           // 21 ★关终了
    entry.add(31, 0x16);           // 22 Boss 战设置
    entry.integer(40, 0x0a, 0);
    entry.end(50);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    // 原版 428ae0：等待期间脚本钟停在 10。第 21 帧按 Z 后继续走时间门，
    // 另需 19 帧到 clock=30；下一次更新才执行 Stage clear。
    for (int frame = 0; frame < 20; frame++) CHECK(player.step(false));
    CHECK(player.step(true));  // Z 边沿放行 opcode 11
    CHECK_EQ(player.clock(), 11u);
    for (int frame = 0; frame < 19; frame++) CHECK(player.step(false));
    CHECK_EQ(player.clock(), 30u);
    CHECK_EQ(host.clears.size(), 0u);
    CHECK(player.step(false));
    CHECK_EQ(host.clears.size(), 1u);
    CHECK_EQ(host.boss_calls.size(), 0u);  // 22 在 time=31，尚未到
    CHECK(player.step(false));
    CHECK_EQ(host.boss_calls.size(), 1u);
    // 终止指令（time=50）后 done 且不再活跃。
    while (player.step(false)) {
    }
    CHECK(!player.active());
    CHECK(player.done());
    CHECK_EQ(host.clears.size(), 1u);
}

TEST(msg_wait_blocks_until_z_or_auto) {
    InstructionBuilder entry;
    entry.text(0, "Line");
    entry.integer(0, 0x0b, 600);  // 手动模式：等 Z
    entry.text(100, "After");
    entry.end(110);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file;
    std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    player.step(false);  // 首句入块，停在 11
    for (int frame = 0; frame < 60; frame++) player.step(false);
    CHECK(player.active());                  // 无 Z：仍阻塞
    CHECK_EQ(player.state().lines.size(), 1u);
    CHECK(player.step(true));                // Z 边沿放行：指针前进
    // 0x428ae0:378-379/393-394 只清 +0x5f/+0x60，不擦已写入的 text VM；
    // 旧行保持到下一块首行（RunScene 用 lines_meta 维持纹理）。
    CHECK_EQ(player.state().lines.size(), 1u);
    for (int frame = 0; frame < 99; frame++) player.step(false);
    CHECK_EQ(player.clock(), 100u);
    CHECK_EQ(player.state().lines.size(), 1u);  // 下一句执行前仍保留旧行
    player.step(false);  // time=100 的次句执行
    CHECK_EQ(player.state().lines.size(), 1u);  // time=100 的次句入块（旧块被替换）
    CHECK_EQ(player.state().lines[0], std::string("After"));
}

TEST(msg_wait_release_paths_original_semantics) {
    // 原版 0x4298fc-0x429969（r4 逐指令核）：自动位 bit0 + 快进位 bit0x40 +
    // Ctrl(9)/Z(0) 持续 ≥0x14 → 静默放行；被动等待不放行（无计时放行路径）。
    InstructionBuilder entry;
    entry.integer(0, 0x0a, 1);  // 10;1 → 自动推进
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
    player.step(false);  // 开场帧：行入块 + 停在 11
    CHECK_EQ(player.state().lines.size(), 1u);
    // 被动等待：远超 0x14 帧仍阻塞（429938/429940 缺快进位即回阻塞）。
    for (int frame = 0; frame < 40; frame++) player.step(false);
    CHECK_EQ(player.state().lines.size(), 1u);
    CHECK_EQ(player.clock(), 0u);  // 原版等待不推进脚本钟
    // Ctrl 持续但未满 0x14：仍阻塞。
    for (int frame = 0; frame < 19; frame++) {
        MsgAdvance input;
        input.held = 0x200;
        input.pressed = frame == 0 ? 0x200 : 0;  // 新按下才能锁存快进请求
        input.ctrl_frames = u32(frame + 1);
        player.step(input);
    }
    CHECK_EQ(player.state().lines.size(), 1u);
    // 第 20 帧静默放行；下条 time=1 的时间门仍需本次 +1。
    MsgAdvance skip;
    skip.held = 0x200;
    skip.ctrl_frames = 0x14;
    player.step(skip);
    CHECK_EQ(player.clock(), 1u);
    // 放行不擦旧文字（原版只清 +0x5f/+0x60）；下一句执行时才换块。
    CHECK_EQ(player.state().lines.size(), 1u);
    player.step(skip);
    CHECK_EQ(player.state().lines.size(), 1u);
    CHECK_EQ(player.state().lines[0], std::string("Next"));
    CHECK_EQ(host.blips, 0);  // 42996f 路径无 0x460bc0 调用
}

TEST(msg_wait_skip_requires_auto_bit_and_keys) {
    // 非自动模式：Ctrl 持续 ≥0x14 不放行（429936 test al,0x1 缺即阻塞）。
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
    CHECK_EQ(player.state().lines.size(), 1u);  // 仍停在首句
    // 自动位置位但无按下边沿：不放行。
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
        MsgAdvance input;  // pressed=0：skip_request 不置位
        input.ctrl_frames = 0x14;  // 持续计数即使非零也无快进位
        auto_player.step(input);
    }
    CHECK_EQ(auto_player.state().lines.size(), 1u);
    // Ctrl 边沿锁存请求，已持续 20 帧的 Z 走静默路径。
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
    // arg 倒计时耗尽、Z/Enter 按下均计声放行；开场前已按住不触发。
    InstructionBuilder entry;
    entry.text(0, "T");
    entry.integer(0, 0x0b, 5);  // 短超时
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
    for (int frame = 0; frame < 3; frame++) player.step(false);
    CHECK_EQ(player.state().lines.size(), 1u);  // 倒计时未尽
    player.step(false);                          // 耗尽帧：计声放行
    CHECK_EQ(host.blips, 1);
    CHECK_EQ(player.state().lines.size(), 1u);   // 旧行保留到下一句写入
    player.step(false);                          // time=1
    CHECK_EQ(player.state().lines[0], std::string("Done"));
    // Enter 按下位（0x80000）立即计声放行。
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
    CHECK_EQ(second_player.state().lines.size(), 1u);  // 旧行保留到下一句写入
    enter.pressed = 0;
    second_player.step(enter);
    CHECK_EQ(second_player.state().lines[0], std::string("Go"));
}

TEST(msg_future_end_and_opcode12_do_not_bypass_time_gate) {
    // Native 428ae0 synthetic stream: 12@0 continues to 12@6 in update 0;
    // end@8 executes on update 8. Opcode 12 never forces an extra update.
    InstructionBuilder entry;
    entry.add(0, 12);
    entry.add(6, 12);
    entry.end(8);
    const auto bytes = build_msg({entry.bytes});
    MsgFile file; MsgPlayer player; RecordHost host; std::string error;
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
    MsgFile file; MsgPlayer player; RecordHost host; std::string error;
    CHECK(file.open(bytes.data(), u32(bytes.size()), error));
    CHECK(player.open(file, 0, host, error));
    MsgAdvance input;
    input.held = 1; input.z_frames = 100;
    for (u32 frame = 0; frame < 30; ++frame) CHECK(player.step(input));
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
    // 13/14（0x4298d4/0x4298e8）：label = arg+0x11；7/8（0x429641/0x4296ee）。
    InstructionBuilder entry;
    entry.add(0, 0x07);
    entry.integer(0, 0x0d, 3);   // 13;3 → 左 label 3+17=20
    entry.integer(0, 0x0e, 0);   // 14;0 → 右 label 0+17=17
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
    CHECK_EQ(player.state().side, 1);  // 8 = 右侧
}

TEST(cp932_font_mapping_matches_runtime_raster_contract) {
    // RuntimeFontRaster::initialize 契约：131072 字节 u16le；th15
    // prepare-fonts.mjs 同语义（单字节/双字节解码，无效 → 0x30FB）。
    std::vector<u8> mapping;
    CHECK(build_cp932_font_mapping(mapping));
    CHECK_EQ(mapping.size(), 131072u);
    auto at = [&mapping](u32 index) {
        return u16(mapping[index * 2]) | (u16(mapping[index * 2 + 1]) << 8);
    };
    CHECK_EQ(at('A'), 0x41u);            // ASCII 直通
    CHECK_EQ(at(0xB1u), 0xFF71u);        // 半角 ｱ
    CHECK_EQ(at(0x82B3u), 0x3055u);      // さ
    CHECK_EQ(at(0x9748u), 0x5E7Du);      // 幽
    CHECK_EQ(at(0x8140u), 0x3000u);      // 全角空格（表首项）
    CHECK_EQ(at(0x80u), 0x30FBu);        // 无效单字节 → 中点
    CHECK_EQ(at(0xA141u), 0x30FBu);      // 半角区不当双字节 lead → 中点
    CHECK_EQ(at(0x8100u), 0x30FBu);      // 次字节 0x00 出列 → 中点
    CHECK_EQ(at(0xFD00u), 0x30FBu);      // lead 出 0x81..0xEF → 中点
    // 与 cp932_to_utf8 同源一致性：表内有效格全等（抽查对角）。
    const u8 yuyuko[] = {0x97, 0x48, 0x81, 0x58, 0x8E, 0x71, 0x97, 0x6C};
    const auto utf8 = cp932_to_utf8(yuyuko, sizeof yuyuko);
    CHECK_EQ(utf8, std::string(u8"幽々子様"));
    CHECK_EQ(at(0x8158u), 0x3005u);  // 々
    CHECK_EQ(at(0x8E71u), 0x5B50u);  // 子
    CHECK_EQ(at(0x976Cu), 0x69D8u);  // 様
}

TEST(msg_line_pairing_two_per_block) {
    InstructionBuilder entry;
    entry.add(0, 0x07);
    entry.text(0, "one");
    entry.text(0, "two");
    entry.text(0, "three");  // 无 11 间隔：第三行重开块（0x5f==0 && 0x60==0 清）
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
    entry.add(0, 0x08);  // 右侧说话人
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
    // 右侧文字位置 = box_x-(bubble_width+30)（0x428ae0:499）；气泡宽度
    // ((len-1)*8)&~0xf-28，len=5 → 4。
    CHECK_EQ(player.state().lines_meta[0].side, 1);
    CHECK_EQ(player.state().lines_meta[0].bubble_width, 4.0f);
}

TEST(msg_bubble_width_excludes_nul_terminator) {
    // 0x428ae0:474：len = pcVar5-(pcVar3+1)（NUL 前字节数）。
    // 3 个 CP932 汉字 = 6 字节 → ((6-1)*8)&~0xf-28 = 4。
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
    CHECK_EQ(player.state().lines_meta[0].bubble_width, 4.0f);
}

TEST(msg_real_st01a_bytes) {
    // 真实资产字节级用例：repo 根相对 ../artifacts/reference/raw/st01a.msg
    //（主仓库布局；隔离子仓验证可用 TH13_RAW_DIR 覆盖）。缺席即跳过。
    std::vector<std::string> candidates;
    if (const char* override_dir = std::getenv("TH13_RAW_DIR"))
        candidates.push_back(std::string(override_dir) + "/st01a.msg");
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
    CHECK_EQ(file.entry_count(), 2u);           // entry0=Boss 前 / entry1=Boss 后
    CHECK_EQ(file.entry(1).offset, 0x334u);     // 头表字节实证
    CHECK_EQ(file.entry(0).offset, 0x14u);
    // 逐 entry 扫 opcode：只有 entry 1 带 21（全 28 个 st0N[a-d] 各恰一处）。
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
                    first_text = cp932_to_utf8(
                        reinterpret_cast<const u8*>(msg_decrypt_text(cursor + 4, arg_bytes).data()),
                        u32(msg_decrypt_text(cursor + 4, arg_bytes).size()));
            }
            cursor += 4 + arg_bytes;
        }
    }
    CHECK_EQ(clears[0], 0);
    CHECK_EQ(clears[1], 1);
    CHECK(texts_entry0 >= 10);
    CHECK_EQ(first_text, std::string(u8"さーて、幽霊の親玉を倒さないとね！"));
    // 同句 34 CP932 字节（17 字）→ ((34-1)*8)&~0xf-28 = 228。
    RecordHost host;
    MsgPlayer player;
    CHECK(player.open(file, 0, host, error));
    for (int frame = 0; frame < 40 && player.state().lines_meta.empty(); frame++)
        player.step(false);
    CHECK_EQ(player.state().lines_meta.size(), 1u);
    CHECK_EQ(player.state().lines_meta[0].bubble_width, 228.0f);
}

TEST(msg_ruby_and_cp932_metadata) {
    InstructionBuilder entry;
    // 构造伴随 ruby 的对话行：|x,spacing,ruby 前缀 + 正文
    entry.text(0, "|4,14,\x82\xb1\x82\xb1"); // |4,14,ここ (CP932)
    entry.text(0, "\x96\xbb\x8a\x45\x82\xc9\x97\x88\x82\xbd\x82\xcc\x82\xcd\x8c\xa9\x93\x96\x88\xe1\x82\xa2\x82\xc1\x82\xc4\x8e\x96\x81\x48"); // 冥界に来たのは見当違いって事？
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
    // 0x428ae0:464 → 0x46e8f0 第 5 参 → 0x45aef0 param_9：第二数是逐字
    // 步进（不是 y）；真实 st01a `|4,14,ここ` 即 x=4, spacing=14。
    CHECK_EQ(meta.ruby_spacing, 14);
    CHECK_EQ(meta.ruby_utf8, std::string(u8"ここ"));
    CHECK_EQ(meta.text_utf8, std::string(u8"冥界に来たのは見当違いって事？"));
    CHECK(meta.bubble_width > 0.0f);
    CHECK_EQ(meta.text_attr, 0u);
}

TEST(msg_release_ops_keep_lines_for_vm_fade) {
    // 原版 opcode 6/18 不清 +0x5f/+0x60，也不清已写入文字；文本 VM 仅
    // 由 0x46fc30 label1/label3 淡出。RunScene 通过 msg_release 接 fade。
    for (u8 opcode : {u8(0x06), u8(0x12)}) {
        InstructionBuilder entry;
        entry.text(0, "Line");
        entry.integer(0, 0x0b, 1);  // 1 帧超时放行
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
        // 行数据保留：RunScene 仍可提交同一行；纹理由 DialogueText::fade
        // 保留像素，不会触发 blank_all。
        CHECK_EQ(player.state().lines.size(), 1u);
        CHECK_EQ(player.state().lines_meta.size(), 1u);
        CHECK_EQ(player.state().lines_meta[0].text_utf8, std::string("Line"));
    }
}

TEST(msg_text_attr_style_bits_follow_original_ops) {
    // 0x428ae0：op26(+0x1a) → +0x5e |= 2（样式 5）；op7/8/9 → +0x5e &= ~2
    // （样式 4）；op29(+0x1d) 只改 bit2-5，不碰样式位 bit1。
    InstructionBuilder entry;
    entry.integer(0, 0x1d, 1);  // 29;1 → bit2
    entry.text(0, "A");
    entry.integer(5, 0x0b, 600);
    entry.integer(10, 0x1a, 0);  // 26 → bit1（样式 5）
    entry.text(10, "B");
    entry.integer(15, 0x0b, 600);
    entry.add(20, 0x07);         // 7 → 清 bit1（样式 4）
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
    player.step(false);  // 29 + text A + time5 gate
    CHECK_EQ(player.state().lines_meta.size(), 1u);
    CHECK_EQ(player.state().lines_meta[0].text_attr, 4u);   // bit2
    for (int frame = 0; frame < 5; frame++) player.step(false);  // clock 5；op11 阻塞
    player.step(true);   // op11 放行；time10 门
    for (int frame = 0; frame < 5; frame++) player.step(false);  // 26 + text B
    CHECK_EQ(player.state().lines_meta.size(), 1u);
    CHECK_EQ(player.state().lines_meta[0].text_attr, 6u);   // bit1|bit2
    CHECK_EQ(((player.state().lines_meta[0].text_attr >> 1) & 1u) + 4u, 5u);
    for (int frame = 0; frame < 5; frame++) player.step(false);  // clock 15；op11 阻塞
    player.step(true);   // op11 放行；time20 门
    for (int frame = 0; frame < 5; frame++) player.step(false);  // 7 + text C
    CHECK_EQ(player.state().lines_meta.size(), 1u);
    CHECK_EQ(player.state().lines_meta[0].text_attr, 4u);   // bit1 cleared
    // 正文字体样式 = ((text_attr>>1)&1)+4：4 = 40px/400，5 = 40px/600。
    CHECK_EQ(((player.state().lines_meta[0].text_attr >> 1) & 1u) + 4u, 4u);
}

int main() { return th13test::run_all("msg 对话系统"); }
