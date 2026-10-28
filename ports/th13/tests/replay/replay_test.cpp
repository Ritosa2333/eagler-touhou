// 录像解码与输入流单测：
// 1) XOR 双趟自逆（合成数据，含奇尾/小余尾分支）；
// 2) LZ77 字面量流、拷贝令牌与重叠回填（手工位流）；
// 3) 合成 rpy 全链路往返（字面量压缩 → 保存序加密 → 解码）；
// 4) 真机录像（games/replays/th13_udSr01.rpy）头/stage 链/USER 段；
// 5) 输入流关键帧：首帧、Boss 前关键帧（frame 2379，Boss 2380 前一帧，
//    2380 为 harness replay 注入实测的自机 0x21Sanae 一阶段 Boss 出现帧）、末帧；
// 6) 沿一致性：文件 pressed/released 与相邻帧差分全等（0x414f30 幂等重算）。
// 7) stage 原始 0xC4 头保留：头字段 = 原版偏移（0x449070 恢复源 = 头+0x14 起
//    0x58 字节、种子 = 头+0x02），解码器不得丢头或臆造字段名；恢复窗口
//    篡改负控（字节级传播对照）。
#include "../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/ReplayFile.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace {

using th13::u8;
using th13::u16;
using th13::u32;
using th13::i32;

bool read_file(const std::string& relative, std::vector<u8>& out) {
    static const char* bases[] = {".", "../../../..", "../../../../.."};
    for (const char* base : bases) {
        std::FILE* file = std::fopen((std::string(base) + "/" + relative).c_str(), "rb");
        if (!file) continue;
        std::fseek(file, 0, SEEK_END);
        const long size = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);
        out.resize(size_t(size < 0 ? 0 : size));
        const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
        std::fclose(file);
        if (got == out.size()) return true;
        return false;
    }
    return false;
}

bool load_replay(th13::ReplayFile& replay) {
    std::vector<u8> bytes;
    if (!read_file("games/replays/th13_udSr01.rpy", bytes)) return false;
    return replay.decode(bytes.data(), bytes.size());
}

// 伪随机字节（确定性）。
std::vector<u8> pattern_bytes(size_t size, u32 seed) {
    std::vector<u8> bytes(size);
    u32 state = seed;
    for (auto& byte : bytes) {
        state = state * 0x343FD + 0x269EC3;
        byte = u8(state >> 16);
    }
    return bytes;
}

TEST(xor_roundtrip_both_directions) {
    // 保存序加密 (0x7D,0x3A,0x100)+(0x5C,0xE1,0x400)，装载序解密反之；覆盖
    // 奇尺寸（末字节不动）、小余尾（<block/4 不变换）与大余尾（并入末块）分支。
    // 注：size=1 时原版变换为恒等（奇对齐+余尾合并后无变换区间），不在断言内。
    for (size_t size : {size_t(255), size_t(256), size_t(257), size_t(1025),
                        size_t(1026), size_t(2049), size_t(89681)}) {
        const std::vector<u8> original = pattern_bytes(size, u32(size) + 7);
        std::vector<u8> saved = original;
        th13::ReplayFile::xor_encrypt(saved.data(), saved.size(), 0x7D, 0x3A, 0x100);
        th13::ReplayFile::xor_encrypt(saved.data(), saved.size(), 0x5C, 0xE1, 0x400);
        CHECK(saved != original);  // 变换确实动了数据
        std::vector<u8> loaded = saved;
        th13::ReplayFile::xor_decrypt(loaded.data(), loaded.size(), 0x5C, 0xE1, 0x400);
        th13::ReplayFile::xor_decrypt(loaded.data(), loaded.size(), 0x7D, 0x3A, 0x100);
        CHECK_EQ(loaded.size(), original.size());
        CHECK(loaded == original);
    }
}

struct BitWriter {
    std::vector<u8> bytes;
    u32 bit = 0;  // MSB 先行
    void put(u32 value, u32 count) {
        bytes.resize(bytes.size() + (bit + count + 7) / 8 - bit / 8, 0);
        for (u32 i = count; i > 0; --i) {
            if (value >> (i - 1) & 1) bytes[bit >> 3] |= u8(0x80 >> (bit & 7));
            ++bit;
        }
    }
};

std::vector<u8> literal_compress(const std::vector<u8>& plain) {
    BitWriter writer;
    for (u8 byte : plain) {
        writer.put(1, 1);
        writer.put(byte, 8);
    }
    writer.put(0, 1);   // 拷贝令牌
    writer.put(0, 13);  // 地址 0 = 终止符
    return std::move(writer.bytes);
}

TEST(lz77_literal_stream) {
    const std::vector<u8> plain = pattern_bytes(1000, 99);
    const std::vector<u8> packed = literal_compress(plain);
    std::vector<u8> out;
    CHECK(th13::ReplayFile::inflate(packed.data(), packed.size(), out, plain.size()));
    CHECK(out == plain);
    // 尺寸不符（limit 过小）应报解码失败。
    std::vector<u8> short_out;
    CHECK(!th13::ReplayFile::inflate(packed.data(), packed.size(), short_out, plain.size() - 1));
}

TEST(lz77_copy_token_and_overlap) {
    // 两个字面量 'a','b'（环位 1、2）后跟拷贝令牌：地址 1、长 n=3 → 6 字节，
    // 读环 (1+i) 自回填 ⇒ 输出 "abababab"（重叠复制语义）。
    BitWriter writer;
    writer.put(1, 1);
    writer.put('a', 8);
    writer.put(1, 1);
    writer.put('b', 8);
    writer.put(0, 1);
    writer.put(1, 13);  // 环地址 1
    writer.put(3, 4);   // 长 = 3 + 3 = 6
    writer.put(0, 1);
    writer.put(0, 13);
    std::vector<u8> out;
    CHECK(th13::ReplayFile::inflate(writer.bytes.data(), writer.bytes.size(), out, 8));
    CHECK_EQ(out.size(), 8u);
    CHECK(std::memcmp(out.data(), "abababab", 8) == 0);
}

TEST(synthetic_replay_roundtrip) {
    // 顶层头 + 单 stage 块 + USER 段，走保存路径（字面量压缩 → 双趟加密）。
    const u32 frames = 40;
    std::vector<u8> input;
    for (u32 i = 0; i < frames; ++i) {
        const u16 state = u16(0x19 * i + 1), pressed = u16(1 << (i % 8)), released = 0;
        input.push_back(u8(state));
        input.push_back(u8(state >> 8));
        input.push_back(u8(pressed));
        input.push_back(u8(pressed >> 8));
        input.push_back(0);
        input.push_back(0);
    }
    std::vector<u8> events((frames + 29) / 30, 60);
    events[1] = 55;
    std::vector<u8> plain(0x74 + 0xC4);
    plain[0] = 'T'; plain[1] = 'E'; plain[2] = 'S'; plain[3] = 'T';
    // 顶层头：+0x14 分数、+0x54 慢速率、+0x58 关卡数、+0x5C/60/64 角色/机体/难度。
    const u32 score = 1234567;
    std::memcpy(plain.data() + 0x14, &score, 4);
    const float slow = 1.5f;
    std::memcpy(plain.data() + 0x54, &slow, 4);
    plain[0x58] = 1;
    const i32 character = 2, shot = 1, rank = 3;
    std::memcpy(plain.data() + 0x5C, &character, 4);
    std::memcpy(plain.data() + 0x60, &shot, 4);
    std::memcpy(plain.data() + 0x64, &rank, 4);
    const i32 practice = -1;
    std::memcpy(plain.data() + 0x70, &practice, 4);
    // stage 头：+0 关卡号、+4 帧数、+8 数据长、+0x1C 起始分。
    const u16 number = 1;
    const u32 blob = u32(input.size() + events.size());
    std::memcpy(plain.data() + 0x74, &number, 2);
    std::memcpy(plain.data() + 0x74 + 4, &frames, 4);
    std::memcpy(plain.data() + 0x74 + 8, &blob, 4);
    const u32 stage_score = 777;
    std::memcpy(plain.data() + 0x74 + 0x1C, &stage_score, 4);
    plain.insert(plain.end(), input.begin(), input.end());
    plain.insert(plain.end(), events.begin(), events.end());

    std::vector<u8> body = literal_compress(plain);
    th13::ReplayFile::xor_encrypt(body.data(), body.size(), 0x7D, 0x3A, 0x100);
    th13::ReplayFile::xor_encrypt(body.data(), body.size(), 0x5C, 0xE1, 0x400);
    std::vector<u8> file;
    file.insert(file.end(), {'t', '1', '3', 'r'});
    const u16 version = 2;
    const u32 user_offset = u32(0x24 + body.size()), hundred = 0x100;
    file.insert(file.end(), {0, 0, 0, 0});
    std::memcpy(file.data() + 4, &version, 2);
    file.insert(file.end(), 4, 0);                                   // +0x08
    file.insert(file.end(), (const u8*)&user_offset, (const u8*)&user_offset + 4);  // +0x0C
    file.insert(file.end(), (const u8*)&hundred, (const u8*)&hundred + 4);          // +0x10
    file.insert(file.end(), 8, 0);                                   // +0x14/+0x18
    const u32 comp = u32(body.size());
    file.insert(file.end(), (const u8*)&comp, (const u8*)&comp + 4);                // +0x1C
    const u32 uncomp = u32(plain.size());
    file.insert(file.end(), (const u8*)&uncomp, (const u8*)&uncomp + 4);            // +0x20
    file.insert(file.end(), body.begin(), body.end());
    const std::string text = "Version 1.00c\r\nName TEST\r\n";
    const u32 block = u32(12 + text.size() + 1);
    file.insert(file.end(), {'U', 'S', 'E', 'R'});
    file.insert(file.end(), (const u8*)&block, (const u8*)&block + 4);
    file.insert(file.end(), {0, 0, 0, 0});
    file.insert(file.end(), text.begin(), text.end());
    file.push_back(0);

    th13::ReplayFile replay;
    CHECK(replay.decode(file.data(), file.size()));
    CHECK_EQ(replay.version, 2u);
    CHECK_EQ(replay.comp_size, comp);
    CHECK_EQ(replay.uncomp_size, uncomp);
    CHECK_EQ(replay.player_name, std::string("TEST"));
    CHECK_EQ(replay.score, 1234567);
    CHECK_EQ(replay.rank, 3);
    CHECK_EQ(replay.character, 2);
    CHECK_EQ(replay.shot_type, 1);
    CHECK(replay.stage_count == 1);
    CHECK_EQ(replay.stages.size(), 1u);
    const auto& stage = replay.stages[0];
    CHECK_EQ(stage.number, 1u);
    CHECK_EQ(stage.frame_count, frames);
    CHECK_EQ(stage.blob_size, blob);
    CHECK_EQ(stage.score, 777);
    CHECK(stage.input == input);
    CHECK(stage.events == events);
    CHECK_EQ(stage.slow_rate(0), 60);
    CHECK_EQ(stage.slow_rate(30), 55);
    CHECK_EQ(stage.slow_rate(59), 55);
    CHECK_EQ(replay.user_blocks.size(), 1u);
    CHECK(replay.user_blocks[0].find("Name TEST") != std::string::npos);
}

TEST(real_replay_decode) {
    th13::ReplayFile replay;
    if (!load_replay(replay)) {
        CHECK(false);  // 原版录像缺失（容器内应存在 games/replays/th13_udSr01.rpy）
        return;
    }
    CHECK_EQ(replay.version, 2u);
    CHECK_EQ(replay.comp_size, 89681u);
    CHECK_EQ(replay.uncomp_size, 637666u);
    CHECK_EQ(replay.user_offset, 89717u);
    CHECK_EQ(replay.player_name, std::string("Kaz"));
    CHECK_EQ(replay.rank, 1);        // Normal
    CHECK_EQ(replay.character, 2);   // Sanae
    CHECK_EQ(replay.score, 24898353);
    CHECK_EQ(replay.stage_count, 6u);
    CHECK_EQ(replay.stages.size(), 6u);
    const u32 expected_frames[6] = {11064, 15501, 17448, 17892, 19813, 23758};
    const u32 expected_scores[6] = {0, 829835, 2696226, 4802191, 8074159, 13320266};
    for (u32 i = 0; i < 6; ++i) {
        CHECK_EQ(replay.stages[i].number, i + 1);
        CHECK_EQ(replay.stages[i].frame_count, expected_frames[i]);
        CHECK_EQ(replay.stages[i].events.size(), (expected_frames[i] + 29) / 30);
        CHECK_EQ(u32(replay.stages[i].input.size()), expected_frames[i] * 6);
        CHECK_EQ(replay.stages[i].score, expected_scores[i]);
    }
    CHECK_EQ(replay.user_blocks.size(), 2u);  // 摘要 + 备注
    CHECK(replay.user_blocks[0].find("Rank Normal") != std::string::npos);
    CHECK(replay.user_blocks[0].find("Stage All Clear") != std::string::npos);
    CHECK(replay.user_blocks[0].find("Score 24898353") != std::string::npos);
}

TEST(input_stream_key_frames) {
    th13::ReplayFile replay;
    if (!load_replay(replay)) {
        CHECK(false);
        return;
    }
    const auto& stage1 = replay.stages[0];
    const auto& stage6 = replay.stages[5];
    // 首帧：全零（关卡开场无输入）。
    const th13::ReplayFrame first = stage1.frame(0);
    CHECK_EQ(first.state, 0u);
    CHECK_EQ(first.pressed, 0u);
    CHECK_EQ(first.released, 0u);
    // 首个输入帧 73：按左（0x40）带按下沿。
    const th13::ReplayFrame move = stage1.frame(73);
    CHECK_EQ(move.state, 0x40u);
    CHECK_EQ(move.pressed, 0x40u);
    CHECK_EQ(move.released, 0u);
    // Boss 前关键帧 2379：Boss 出现帧 2380 的前一帧，Z+下+右（0xA1）贴脸走位。
    const th13::ReplayFrame pre_boss = stage1.frame(2379);
    CHECK_EQ(pre_boss.state, 0xA1u);
    CHECK_EQ(pre_boss.pressed, 0u);
    CHECK_EQ(pre_boss.released, 0u);
    // Boss 出现帧 2380：右键释放（0x80）走位收束。
    const th13::ReplayFrame boss = stage1.frame(2380);
    CHECK_EQ(boss.state, 0x21u);
    CHECK_EQ(boss.released, 0x80u);
    // 末帧：末关卡最后一帧全零。
    const th13::ReplayFrame last = stage6.frame(stage6.frame_count - 1);
    CHECK_EQ(last.state, 0u);
    CHECK_EQ(last.pressed, 0u);
    CHECK_EQ(last.released, 0u);
    // 慢速率节拍：tick0=60、tick1/2=55、tick3=60。
    CHECK_EQ(stage1.slow_rate(0), 60);
    CHECK_EQ(stage1.slow_rate(30), 55);
    CHECK_EQ(stage1.slow_rate(60), 55);
    CHECK_EQ(stage1.slow_rate(90), 60);
}

TEST(input_stream_edge_consistency) {
    // 全 stage1 11064 帧：文件 pressed/released 与相邻帧差分全等——录制侧
    // 0x4480a0 落盘的沿与 0x414f30 的幂等重算一致（回放侧等价注入依据）。
    th13::ReplayFile replay;
    if (!load_replay(replay)) {
        CHECK(false);
        return;
    }
    const auto& stage = replay.stages[0];
    u16 previous = 0;
    u32 mismatches = 0;
    for (u32 i = 0; i < stage.frame_count; ++i) {
        const th13::ReplayFrame frame = stage.frame(i);
        if (i > 0) {
            if (frame.pressed != (frame.state & ~previous & 0xFFFF)) ++mismatches;
            if (frame.released != (previous & ~frame.state & 0xFFFF)) ++mismatches;
        }
        previous = frame.state;
    }
    CHECK_EQ(mismatches, 0u);
    // GameInput::update 注入路径：沿推导与文件沿全等。
    th13::GameInput input;
    u32 input_mismatches = 0;
    for (u32 i = 0; i < stage.frame_count; ++i) {
        const th13::ReplayFrame frame = stage.frame(i);
        input.update(frame.state);
        if (i > 0) {
            if (u16(input.pressed) != frame.pressed) ++input_mismatches;
            if (u16(input.released) != frame.released) ++input_mismatches;
        }
    }
    CHECK_EQ(input_mismatches, 0u);
}

TEST(replay_input_cursor) {
    th13::ReplayFile replay;
    if (!load_replay(replay)) {
        CHECK(false);
        return;
    }
    const auto& stage = replay.stages[0];
    th13::ReplayInputCursor cursor(stage);
    CHECK(cursor.valid());
    CHECK_EQ(cursor.frame_index(), 0u);
    u32 count = 0;
    while (cursor.valid()) {
        const th13::ReplayFrame frame = cursor.next();
        CHECK_EQ(cursor.frame_index(), count + 1);
        if (count == 0) CHECK(frame.state == 0);
        if (count == 73) CHECK(frame.state == 0x40);
        ++count;
    }
    CHECK_EQ(count, stage.frame_count);
    CHECK(!cursor.valid());
    // 越界续读：零帧（回放侧 0x448200 越界清零语义）。
    CHECK(cursor.next().state == 0);
    CHECK(cursor.next().eof() == false);
    // 慢速率随游标推进。
    th13::ReplayInputCursor rate(stage);
    CHECK_EQ(rate.current_slow_rate(), 60);
    for (u32 i = 0; i < 30; ++i) rate.next();
    CHECK_EQ(rate.current_slow_rate(), 55);
}

TEST(stage_header_preserved_raw) {
    // stage 原始 0xC4 头保留：真实初值唯一源（0x449070 的恢复面 =
    // 头+0x14 起 0x58 字节 → 0x4be7b8、头+0x02 u16 种子 → 0x4dc324/0x4dc31c，
    // artifacts/decomp/jp/functions/00449070.c:40-49）。头字段与派生量
    // 必须逐位出自原样字节，不臆造字段名。
    th13::ReplayFile replay;
    if (!load_replay(replay)) {
        CHECK(false);
        return;
    }
    u16 max_number = 0;
    for (u32 i = 0; i < replay.stage_count; ++i) {
        const auto& stage = replay.stages[i];
        CHECK_EQ(stage.header.size(), size_t(0xC4));
        // 派生字段 = 头内偏移。
        const u16 number = u16(stage.header[0]) | u16(stage.header[1]) << 8;
        const u16 stamp = u16(stage.header[2]) | u16(stage.header[3]) << 8;
        u32 frame_count = 0, blob_size = 0;
        for (u32 b = 0; b < 4; ++b) {
            frame_count |= u32(stage.header[4 + b]) << (8 * b);
            blob_size |= u32(stage.header[8 + b]) << (8 * b);
        }
        CHECK_EQ(stage.number, number);
        CHECK_EQ(stage.stamp, stamp);
        CHECK_EQ(stage.frame_count, frame_count);
        CHECK_EQ(stage.blob_size, blob_size);
        CHECK_EQ(u32(stage.input.size()), frame_count * 6);
        // 关卡号唯一且 ≤ 关卡总数（全通关录像单调递增；单关/练习档
        // 只含一个关卡——不误杀合法文件：仅查唯一性与上界）。
        CHECK(number != 0 && number <= 8);
        CHECK(u32(number) > u32(max_number));
        max_number = number;
    }
    // udSr01 stage1 戳 = 61738（原版 0x449070 种子恢复的已知值）。
    CHECK_EQ(replay.stages[0].stamp, u16(61738));
}

TEST(tampered_header_changes_the_restored_state_bytes) {
    // 负控（有检出力）：恢复窗口（头+0x14..0x6c）的原始字节经字节级
    // 重解析后，篡改版与原版必然导出不同的恢复字节序列——模拟
    // 0x449070:43-49 的 0x58 字节拷贝（该拷贝是逐位复制，篡改即传播）。
    th13::ReplayFile replay;
    if (!load_replay(replay)) {
        CHECK(false);
        return;
    }
    const auto& stage = replay.stages[0];
    std::vector<u8> tampered(stage.header);
    tampered[0x20] = u8(tampered[0x20] ^ 1);   // 恢复区内单字节翻转
    bool restored_differs = false;
    for (u32 i = 0; i < 0x58; ++i) {
        if (tampered[0x14 + i] != stage.header[0x14 + i]) restored_differs = true;
    }
    CHECK(restored_differs);   // 篡改必然落入恢复字节序列（检出力）
    // 与已知恢复值对照：篡改前恢复序列 = 头[0x14,0x6c) 原样字节。
    const auto& untampered = stage.header;
    u32 same = 0;
    for (u32 i = 0; i < 0x58; ++i) {
        if (tampered[0x14 + i] == untampered[0x14 + i]) ++same;
    }
    CHECK(same < 0x58);   // 篡改后逐位对照不再全等
}

}  // namespace

int main() {
    return th13test::run_all("录像解码与输入流");
}
