// scoreth13.dat 的原始块容器与字段视图。
// 文件 = 0x18 明文头 + xor_encrypt(LZ 压缩(镜像体), key0=0xAC, step=0x35,
// block=0x10)。镜像体 = 5 个 CR 记录列（4 名角色 + 合计，各 0x56DC）+ ST 块。
// 每个 CR 列包含 6 组难度榜表，每组 10 条；界面使用前 5 个难度槽。
//
// parse/build 保留块内未解释字节。保存时只收录魔数为 'CR' 的列，并按列号
// 重写 +0x0C 与校验和；缺席列不进入文件，uncompSize 随之缩短。
#pragma once

#include "../game/Types.hpp"

#include <array>
#include <string>
#include <vector>

namespace th13 {

class ScoreDat {
public:
    static constexpr int kColumnCount = 5;
    static constexpr int kDifficultySlots = 6;
    static constexpr int kRankCount = 10;
    static constexpr int kPracticeCount = 127;
    static constexpr size_t kCRSize = 0x56DC;
    static constexpr size_t kSTSize = 0x42C;
    static constexpr size_t kImageSize = size_t(kColumnCount) * kCRSize + kSTSize;
    static constexpr size_t kEntrySize = 0x1C;
    static constexpr size_t kRankTableStride = kRankCount * kEntrySize;
    static constexpr size_t kEntriesOff = 0x10;
    static constexpr size_t kMidOff = 0x6A0;
    static constexpr size_t kPlayCountOff = 0x7B8;
    static constexpr size_t kPlayFramesOff = 0x7BC;
    static constexpr size_t kClearCountOff = 0x7C0;
    static constexpr size_t kNoContinueClearCountOff = 0x7DC;
    static constexpr size_t kProgressRowsOff = 0x7F4;
    static constexpr size_t kPracticeOff = 0x978;
    static constexpr size_t kPracticeSize = 0x9C;
    static constexpr size_t kPracticeNameSize = 0x80;
    static constexpr size_t kPracticeCapturesOff = 0x80;
    static constexpr size_t kPracticeAttemptsOff = 0x88;
    static constexpr size_t kPracticeIdOff = 0x90;
    static constexpr size_t kPracticeStageOff = 0x94;
    static constexpr size_t kPracticeBestOff = 0x98;
    static constexpr size_t kStNameOff = 0x0C;
    static constexpr size_t kStTableOff = 0x50;
    static constexpr int kStTableLen = 0x1EE;
    static constexpr u32 kHeaderFieldC = 0x100;
    static constexpr u16 kFileVersion = 4;
    static constexpr size_t kHeaderSize = 0x18;
    static constexpr size_t kStEndingOff = 0x16;
    static constexpr size_t kEndingBytes = 13;

    static_assert(kEntriesOff + kDifficultySlots * kRankTableStride == kMidOff);
    static_assert(kNoContinueClearCountOff + kDifficultySlots * 4 == kProgressRowsOff);
    static_assert(kPracticeOff + kPracticeCount * kPracticeSize == kCRSize);

    struct Entry {
        u32 score = 0;
        u8 stage = 0;
        u8 continues = 0;
        std::array<u8, 9> name{};
        u32 time_lo = 0;
        u32 time_hi = 0;
        u32 rate_bits = 0;
    };

    std::array<std::array<u8, kCRSize>, kColumnCount> columns{};
    std::array<bool, kColumnCount> column_present{};
    std::array<u8, kSTSize> st{};
    u16 seed = 0;
    std::string error;

    static u16 lcg_next(u16& state);

    void set_defaults(u16 lcg_seed);
    void set_default_column(u32 column);

    bool parse_file(const u8* data, size_t size);
    bool parse_image(const u8* image, size_t size);

    size_t build_image(u8* out, size_t capacity);
    bool build_file(std::vector<u8>& out);
    static void wrap_file(std::vector<u8> comp, size_t uncomp, std::vector<u8>& out);

    static Entry read_entry(const u8* column, int difficulty, int rank);
    static void write_entry(u8* column, int difficulty, int rank, const Entry& entry);

    static u32 play_count(const u8* column);
    static void set_play_count(u8* column, u32 value);
    static u32 play_frames(const u8* column);
    static void set_play_frames(u8* column, u32 value);
    static u32 clear_count(const u8* column, int difficulty);
    static void set_clear_count(u8* column, int difficulty, u32 value);
    static u32 no_continue_clear_count(const u8* column, int difficulty);
    static void set_no_continue_clear_count(u8* column, int difficulty, u32 value);

    static u32 progress_row_score(const u8* column, int difficulty, int stage);
    static void set_progress_row_score(u8* column, int difficulty, int stage, u32 value);
    static u8 progress_row_flag(const u8* column, int difficulty, int stage, int which);
    static void set_progress_row_flag(u8* column, int difficulty, int stage, int which, u8 value);

    static std::array<u8, kPracticeNameSize> practice_name(const u8* column, int i);
    static void set_practice_name(u8* column, int i,
                                  const std::array<u8, kPracticeNameSize>& name);
    static u32 practice_u32(const u8* column, int i, size_t field_offset);
    static void set_practice_u32(u8* column, int i, size_t field_offset, u32 value);

    static std::array<u8, 9> st_name(const u8* st);
    static void set_st_name(u8* st, const std::array<u8, 9>& name);

    static void read_ending_bytes(const u8* st, u8 (&out)[kEndingBytes]);
    static void write_ending_bytes(u8* st, const u8 (&in)[kEndingBytes]);

    static u32 block_checksum(const u8* block, size_t size);
    u64 max_entry_timestamp() const;

private:
    bool fail(const std::string& message) {
        error = message;
        return false;
    }
};

}  // namespace th13
