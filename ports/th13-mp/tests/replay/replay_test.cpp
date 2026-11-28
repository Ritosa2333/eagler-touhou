#include "../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/ReplayFile.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace {

using th13::i32;
using th13::u16;
using th13::u32;
using th13::u8;

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
    for (size_t size :
         {size_t(255), size_t(256), size_t(257), size_t(1025), size_t(1026), size_t(2049), size_t(89681)}) {
        const std::vector<u8> original = pattern_bytes(size, u32(size) + 7);
        std::vector<u8> saved = original;
        th13::ReplayFile::xor_encrypt(saved.data(), saved.size(), 0x7D, 0x3A, 0x100);
        th13::ReplayFile::xor_encrypt(saved.data(), saved.size(), 0x5C, 0xE1, 0x400);
        CHECK(saved != original);
        std::vector<u8> loaded = saved;
        th13::ReplayFile::xor_decrypt(loaded.data(), loaded.size(), 0x5C, 0xE1, 0x400);
        th13::ReplayFile::xor_decrypt(loaded.data(), loaded.size(), 0x7D, 0x3A, 0x100);
        CHECK_EQ(loaded.size(), original.size());
        CHECK(loaded == original);
    }
}

struct BitWriter {
    std::vector<u8> bytes;
    u32 bit = 0;
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
    writer.put(0, 1);
    writer.put(0, 13);
    return std::move(writer.bytes);
}

TEST(lz77_literal_stream) {
    const std::vector<u8> plain = pattern_bytes(1000, 99);
    const std::vector<u8> packed = literal_compress(plain);
    std::vector<u8> out;
    CHECK(th13::ReplayFile::inflate(packed.data(), packed.size(), out, plain.size()));
    CHECK(out == plain);
    std::vector<u8> short_out;
    CHECK(!th13::ReplayFile::inflate(packed.data(), packed.size(), short_out, plain.size() - 1));
}

TEST(lz77_copy_token_and_overlap) {
    BitWriter writer;
    writer.put(1, 1);
    writer.put('a', 8);
    writer.put(1, 1);
    writer.put('b', 8);
    writer.put(0, 1);
    writer.put(1, 13);
    writer.put(3, 4);
    writer.put(0, 1);
    writer.put(0, 13);
    std::vector<u8> out;
    CHECK(th13::ReplayFile::inflate(writer.bytes.data(), writer.bytes.size(), out, 8));
    CHECK_EQ(out.size(), 8u);
    CHECK(std::memcmp(out.data(), "abababab", 8) == 0);
}

TEST(synthetic_replay_roundtrip) {
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
    plain[0] = 'T';
    plain[1] = 'E';
    plain[2] = 'S';
    plain[3] = 'T';
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
    file.insert(file.end(), 4, 0);
    file.insert(file.end(), (const u8*)&user_offset, (const u8*)&user_offset + 4);
    file.insert(file.end(), (const u8*)&hundred, (const u8*)&hundred + 4);
    file.insert(file.end(), 8, 0);
    const u32 comp = u32(body.size());
    file.insert(file.end(), (const u8*)&comp, (const u8*)&comp + 4);
    const u32 uncomp = u32(plain.size());
    file.insert(file.end(), (const u8*)&uncomp, (const u8*)&uncomp + 4);
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
        CHECK(false);
        return;
    }
    CHECK_EQ(replay.version, 2u);
    CHECK_EQ(replay.comp_size, 89681u);
    CHECK_EQ(replay.uncomp_size, 637666u);
    CHECK_EQ(replay.user_offset, 89717u);
    CHECK_EQ(replay.player_name, std::string("Kaz"));
    CHECK_EQ(replay.rank, 1);
    CHECK_EQ(replay.character, 2);
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
    CHECK_EQ(replay.user_blocks.size(), 2u);
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
    const th13::ReplayFrame first = stage1.frame(0);
    CHECK_EQ(first.state, 0u);
    CHECK_EQ(first.pressed, 0u);
    CHECK_EQ(first.released, 0u);
    const th13::ReplayFrame move = stage1.frame(73);
    CHECK_EQ(move.state, 0x40u);
    CHECK_EQ(move.pressed, 0x40u);
    CHECK_EQ(move.released, 0u);
    const th13::ReplayFrame pre_boss = stage1.frame(2379);
    CHECK_EQ(pre_boss.state, 0xA1u);
    CHECK_EQ(pre_boss.pressed, 0u);
    CHECK_EQ(pre_boss.released, 0u);
    const th13::ReplayFrame boss = stage1.frame(2380);
    CHECK_EQ(boss.state, 0x21u);
    CHECK_EQ(boss.released, 0x80u);
    const th13::ReplayFrame last = stage6.frame(stage6.frame_count - 1);
    CHECK_EQ(last.state, 0u);
    CHECK_EQ(last.pressed, 0u);
    CHECK_EQ(last.released, 0u);
    CHECK_EQ(stage1.slow_rate(0), 60);
    CHECK_EQ(stage1.slow_rate(30), 55);
    CHECK_EQ(stage1.slow_rate(60), 55);
    CHECK_EQ(stage1.slow_rate(90), 60);
}

TEST(input_stream_edge_consistency) {
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
    CHECK(cursor.next().state == 0);
    CHECK(cursor.next().eof() == false);
    th13::ReplayInputCursor rate(stage);
    CHECK_EQ(rate.current_slow_rate(), 60);
    for (u32 i = 0; i < 30; ++i)
        rate.next();
    CHECK_EQ(rate.current_slow_rate(), 55);
}

TEST(stage_header_preserved_raw) {
    th13::ReplayFile replay;
    if (!load_replay(replay)) {
        CHECK(false);
        return;
    }
    u16 max_number = 0;
    for (u32 i = 0; i < replay.stage_count; ++i) {
        const auto& stage = replay.stages[i];
        CHECK_EQ(stage.header.size(), size_t(0xC4));
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
        CHECK(number != 0 && number <= 8);
        CHECK(u32(number) > u32(max_number));
        max_number = number;
    }
    CHECK_EQ(replay.stages[0].stamp, u16(61738));
}

TEST(tampered_header_changes_the_restored_state_bytes) {
    th13::ReplayFile replay;
    if (!load_replay(replay)) {
        CHECK(false);
        return;
    }
    const auto& stage = replay.stages[0];
    std::vector<u8> tampered(stage.header);
    tampered[0x20] = u8(tampered[0x20] ^ 1);
    bool restored_differs = false;
    for (u32 i = 0; i < 0x58; ++i) {
        if (tampered[0x14 + i] != stage.header[0x14 + i]) restored_differs = true;
    }
    CHECK(restored_differs);
    const auto& untampered = stage.header;
    u32 same = 0;
    for (u32 i = 0; i < 0x58; ++i) {
        if (tampered[0x14 + i] == untampered[0x14 + i]) ++same;
    }
    CHECK(same < 0x58);
}

}

int main() { return th13test::run_all("录像解码与输入流"); }
