#include "ScoreDat.hpp"

#include "ReplayFile.hpp"

#include <algorithm>
#include <cstring>

namespace th13 {
namespace {

constexpr char kMagic[4] = {'T', 'H', '3', '1'};
constexpr u16 kCRMagic = 0x5243;
constexpr u16 kSTMagic = 0x5453;
constexpr u16 kBlockVersion = 1;
constexpr u32 kEncryptKey0 = 0xAC;
constexpr u32 kEncryptStep = 0x35;
constexpr u32 kEncryptBlock = 0x10;

constexpr u8 kEntryNameDefault[9] = {'-', '-', '-', '-', '-', '-', '-', '-', 0x00};
constexpr u8 kStNameDefault[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0x00};

constexpr u8 kPracticeStageTable[ScoreDat::kPracticeCount] = {
    0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 2, 3,
    0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 0, 1,
    2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1,
    2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5,
};

u16 read_u16(const u8* at) { return u16(u16(at[0]) | u16(at[1]) << 8); }
u32 read_u32(const u8* at) { return u32(at[0]) | u32(at[1]) << 8 | u32(at[2]) << 16 | u32(at[3]) << 24; }
void put_u16(u8* at, u16 value) {
    at[0] = u8(value);
    at[1] = u8(value >> 8);
}
void put_u32(u8* at, u32 value) {
    for (int i = 0; i < 4; ++i)
        at[i] = u8(value >> (i * 8));
}

void fill_default_column(u8* column, u32 column_index) {
    std::memset(column, 0, ScoreDat::kCRSize);
    put_u16(column + 0, kCRMagic);
    put_u16(column + 2, kBlockVersion);
    put_u32(column + 8, ScoreDat::kCRSize);
    put_u32(column + 0x0C, column_index);
    ScoreDat::Entry entry;
    entry.stage = 1;
    std::memcpy(entry.name.data(), kEntryNameDefault, entry.name.size());
    for (int difficulty = 0; difficulty < ScoreDat::kDifficultySlots; ++difficulty) {
        for (int rank = 0; rank < ScoreDat::kRankCount; ++rank) {
            entry.score = u32(100000 - rank * 10000);
            ScoreDat::write_entry(column, difficulty, rank, entry);
        }
    }
    for (int i = 0; i < ScoreDat::kPracticeCount; ++i) {
        u8* record = column + ScoreDat::kPracticeOff + size_t(i) * ScoreDat::kPracticeSize;
        put_u32(record + ScoreDat::kPracticeIdOff, u32(i));
        put_u32(record + ScoreDat::kPracticeStageOff, u32(kPracticeStageTable[i]));
    }
}

void set_default_st_block(u8* st, u16 seed) {
    std::memset(st, 0, ScoreDat::kSTSize);
    put_u32(st + 0, u32(kSTMagic) | u32(kBlockVersion) << 16);
    put_u32(st + 8, ScoreDat::kSTSize);
    std::memcpy(st + ScoreDat::kStNameOff, kStNameDefault, 9);
    u16 state = seed;
    for (int i = 0; i < ScoreDat::kStTableLen; ++i) {
        put_u16(st + ScoreDat::kStTableOff + size_t(i) * 2, ScoreDat::lcg_next(state));
    }
}

}

u16 ScoreDat::lcg_next(u16& state) {
    const u32 x = u32(u16(state ^ 0x9630) - 0x6553) & 0xFFFF;
    state = u16((x << 2 | x >> 14) & 0xFFFF);
    return state;
}

void ScoreDat::set_defaults(u16 lcg_seed) {
    seed = lcg_seed;
    for (int column = 0; column < kColumnCount; ++column) {
        fill_default_column(columns[size_t(column)].data(), u32(column));
        column_present[size_t(column)] = true;
    }
    set_default_st_block(st.data(), lcg_seed);
    error.clear();
}

void ScoreDat::set_default_column(u32 column) {
    if (column >= u32(kColumnCount)) return;
    fill_default_column(columns[column].data(), column);
    column_present[column] = true;
}

u32 ScoreDat::block_checksum(const u8* block, size_t size) {
    u32 sum = 0;
    for (size_t i = 8; i < size; ++i)
        sum += block[i];
    return sum;
}

bool ScoreDat::parse_file(const u8* data, size_t size) {
    error.clear();
    if (size < kHeaderSize) return fail("score file shorter than header");
    if (std::memcmp(data, kMagic, 4) != 0) return fail("score file bad magic (0x449cd0:00449ce2)");
    const u16 version = read_u16(data + 8);
    if (version != kFileVersion) return fail("score file bad version");
    const u32 comp_size = read_u32(data + 0x10);
    const u32 uncomp_size = read_u32(data + 0x14);
    if (comp_size > size - kHeaderSize) return fail("score file shorter than comp_size");
    if (uncomp_size == 0 || uncomp_size > kImageSize) return fail("score file bad uncomp_size");

    std::vector<u8> plain(data + kHeaderSize, data + kHeaderSize + comp_size);
    ReplayFile::xor_decrypt(plain.data(), plain.size(), kEncryptKey0, kEncryptStep, kEncryptBlock);
    std::vector<u8> image;
    if (!ReplayFile::inflate(plain.data(), plain.size(), image, uncomp_size)) return fail("score file inflate failed");

    const bool ok = parse_image(image.data(), image.size());
    if (!ok) error = "score file walk: " + error;
    return ok;
}

bool ScoreDat::parse_image(const u8* image, size_t size) {
    error.clear();
    for (int column = 0; column < kColumnCount; ++column)
        column_present[size_t(column)] = false;
    bool st_present = false;
    size_t pos = 0;
    while (pos < size) {
        if (size - pos < 8) return fail("trailing garbage");
        const u16 magic = read_u16(image + pos);
        const u16 version = read_u16(image + pos + 2);
        const u32 checksum = read_u32(image + pos + 4);
        const u32 block_size = read_u32(image + pos + 8);
        if (block_size == 0 || pos + block_size > size) return fail("block overruns image");
        const u8* block = image + pos;
        if (magic == kCRMagic) {
            if (version != kBlockVersion || block_size != kCRSize || checksum != block_checksum(block, kCRSize))
                return fail("CR block failed validation");
            const u32 column = read_u32(block + 0x0C);
            if (column >= u32(kColumnCount)) return fail("CR column out of range");
            std::memcpy(columns[column].data(), block, kCRSize);
            column_present[column] = true;
        } else if (magic == kSTMagic) {
            if (version != kBlockVersion || block_size != kSTSize || checksum != block_checksum(block, kSTSize))
                return fail("ST block failed validation");
            std::memcpy(st.data(), block, kSTSize);
            st_present = true;
        } else {
            return fail("unknown block magic (game: abort walk)");
        }
        pos += block_size;
    }
    if (!st_present) return fail("ST block missing (game: keeps boot-regenerated table)");
    return true;
}

size_t ScoreDat::build_image(u8* out, size_t capacity) {
    if (capacity < kImageSize) {
        fail("build_image capacity too small");
        return 0;
    }
    error.clear();
    size_t cursor = 0;
    for (int column = 0; column < kColumnCount; ++column) {
        if (!column_present[size_t(column)]) continue;
        u8* block = out + cursor;
        std::memcpy(block, columns[size_t(column)].data(), kCRSize);
        put_u32(block + 0x0C, u32(column));
        put_u32(block + 4, block_checksum(block, kCRSize));
        cursor += kCRSize;
    }
    std::memcpy(out + cursor, st.data(), kSTSize);
    put_u32(out + cursor + 4, block_checksum(out + cursor, kSTSize));
    cursor += kSTSize;
    return cursor;
}

bool ScoreDat::build_file(std::vector<u8>& out) {
    error.clear();
    std::vector<u8> image(kImageSize);
    const size_t uncomp = build_image(image.data(), image.size());
    if (uncomp == 0) return fail("build_image: " + error);
    image.resize(uncomp);
    std::vector<u8> comp = ReplayFile::deflate(image.data(), uncomp);
    if (comp.empty()) return fail("deflate produced no output");
    wrap_file(std::move(comp), uncomp, out);
    return true;
}

void ScoreDat::wrap_file(std::vector<u8> comp, size_t uncomp, std::vector<u8>& out) {
    ReplayFile::xor_encrypt(comp.data(), comp.size(), kEncryptKey0, kEncryptStep, kEncryptBlock);
    out.assign(kHeaderSize + comp.size(), 0);
    std::memcpy(out.data(), kMagic, 4);
    put_u32(out.data() + 4, u32(comp.size() + kHeaderSize));
    put_u16(out.data() + 8, kFileVersion);
    put_u16(out.data() + 0xA, 0);
    put_u32(out.data() + 0xC, kHeaderFieldC);
    put_u32(out.data() + 0x10, u32(comp.size()));
    put_u32(out.data() + 0x14, u32(uncomp));
    std::memcpy(out.data() + kHeaderSize, comp.data(), comp.size());
}

ScoreDat::Entry ScoreDat::read_entry(const u8* column, int difficulty, int rank) {
    Entry entry;
    const u8* at = column + kEntriesOff + (size_t(difficulty) * kRankCount + size_t(rank)) * kEntrySize;
    entry.score = read_u32(at);
    entry.stage = at[4];
    entry.continues = at[5];
    std::memcpy(entry.name.data(), at + 6, entry.name.size());
    entry.time_lo = read_u32(at + 0x10);
    entry.time_hi = read_u32(at + 0x14);
    entry.rate_bits = read_u32(at + 0x18);
    return entry;
}

void ScoreDat::write_entry(u8* column, int difficulty, int rank, const Entry& entry) {
    u8* at = column + kEntriesOff + (size_t(difficulty) * kRankCount + size_t(rank)) * kEntrySize;
    put_u32(at, entry.score);
    at[4] = entry.stage;
    at[5] = entry.continues;
    std::memcpy(at + 6, entry.name.data(), entry.name.size());
    at[15] = 0;
    put_u32(at + 0x10, entry.time_lo);
    put_u32(at + 0x14, entry.time_hi);
    put_u32(at + 0x18, entry.rate_bits);
}

u32 ScoreDat::play_count(const u8* column) { return read_u32(column + kPlayCountOff); }

void ScoreDat::set_play_count(u8* column, u32 value) { put_u32(column + kPlayCountOff, value); }

u32 ScoreDat::play_frames(const u8* column) { return read_u32(column + kPlayFramesOff); }

void ScoreDat::set_play_frames(u8* column, u32 value) { put_u32(column + kPlayFramesOff, value); }

u32 ScoreDat::clear_count(const u8* column, int difficulty) {
    return read_u32(column + kClearCountOff + size_t(difficulty) * 4);
}

void ScoreDat::set_clear_count(u8* column, int difficulty, u32 value) {
    put_u32(column + kClearCountOff + size_t(difficulty) * 4, value);
}

u32 ScoreDat::no_continue_clear_count(const u8* column, int difficulty) {
    return read_u32(column + kNoContinueClearCountOff + size_t(difficulty) * 4);
}

void ScoreDat::set_no_continue_clear_count(u8* column, int difficulty, u32 value) {
    put_u32(column + kNoContinueClearCountOff + size_t(difficulty) * 4, value);
}

u32 ScoreDat::progress_row_score(const u8* column, int difficulty, int stage) {
    return read_u32(column + kProgressRowsOff + size_t(difficulty * 8 + stage) * 8);
}

void ScoreDat::set_progress_row_score(u8* column, int difficulty, int stage, u32 value) {
    put_u32(column + kProgressRowsOff + size_t(difficulty * 8 + stage) * 8, value);
}

u8 ScoreDat::progress_row_flag(const u8* column, int difficulty, int stage, int which) {
    return column[kProgressRowsOff + size_t(difficulty * 8 + stage) * 8 + 4 + size_t(which)];
}

void ScoreDat::set_progress_row_flag(u8* column, int difficulty, int stage, int which, u8 value) {
    column[kProgressRowsOff + size_t(difficulty * 8 + stage) * 8 + 4 + size_t(which)] = value;
}

std::array<u8, ScoreDat::kPracticeNameSize> ScoreDat::practice_name(const u8* column, int i) {
    std::array<u8, kPracticeNameSize> name{};
    std::memcpy(name.data(), column + kPracticeOff + size_t(i) * kPracticeSize, name.size());
    return name;
}

void ScoreDat::set_practice_name(u8* column, int i, const std::array<u8, kPracticeNameSize>& name) {
    std::memcpy(column + kPracticeOff + size_t(i) * kPracticeSize, name.data(), name.size());
}

u32 ScoreDat::practice_u32(const u8* column, int i, size_t field_offset) {
    return read_u32(column + kPracticeOff + size_t(i) * kPracticeSize + field_offset);
}

void ScoreDat::set_practice_u32(u8* column, int i, size_t field_offset, u32 value) {
    put_u32(column + kPracticeOff + size_t(i) * kPracticeSize + field_offset, value);
}

u64 ScoreDat::max_entry_timestamp() const {
    u64 newest = 0;
    for (int column = 0; column < kColumnCount; ++column) {
        if (!column_present[size_t(column)]) continue;
        const u8* block = columns[size_t(column)].data();
        for (int difficulty = 0; difficulty < kDifficultySlots; ++difficulty) {
            for (int rank = 0; rank < kRankCount; ++rank) {
                const Entry entry = read_entry(block, difficulty, rank);
                newest = std::max(newest, u64(entry.time_lo) | u64(entry.time_hi) << 32);
            }
        }
    }
    return newest;
}

std::array<u8, 9> ScoreDat::st_name(const u8* st) {
    std::array<u8, 9> name{};
    std::memcpy(name.data(), st + kStNameOff, name.size());
    return name;
}

void ScoreDat::set_st_name(u8* st, const std::array<u8, 9>& name) {
    std::memcpy(st + kStNameOff, name.data(), name.size());
}

void ScoreDat::read_ending_bytes(const u8* st, u8 (&out)[kEndingBytes]) {
    std::memcpy(out, st + kStEndingOff, kEndingBytes);
}

void ScoreDat::write_ending_bytes(u8* st, const u8 (&in)[kEndingBytes]) {
    std::memcpy(st + kStEndingOff, in, kEndingBytes);
}

}
