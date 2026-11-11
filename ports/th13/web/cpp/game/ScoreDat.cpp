// scoreth13.dat 格式孪生实现。证据基线见 ScoreDat.hpp 头注与外层
// oracle/scorefmt.py（偏移表 + 校验和 + LCG 递推 + 写入点地址，[D]/[U]/[G] 标注）。
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
constexpr u32 kEncryptKey0 = 0xAC;  // 0x449e50:00449fe4
constexpr u32 kEncryptStep = 0x35;
constexpr u32 kEncryptBlock = 0x10;

constexpr u8 kEntryNameDefault[9] = {'-', '-', '-', '-', '-', '-', '-', '-', 0x00};
constexpr u8 kStNameDefault[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0x00};

// 0x4bbd30：练习记录 → 关卡 id（0x449760 movsx 读 127 字节；th13.exe 原字节，
// oracle/scorefmt.py PRACTICE_STAGE_TABLE 同源）。
constexpr u8 kPracticeStageTable[ScoreDat::kPracticeCount] = {
    0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 2, 3, 0, 1,
    2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 2, 3,
    0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 0,
    1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 0, 1,
    2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1,
    2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1,
    2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5,
};

u16 read_u16(const u8* at) {
    return u16(u16(at[0]) | u16(at[1]) << 8);
}
u32 read_u32(const u8* at) {
    return u32(at[0]) | u32(at[1]) << 8 | u32(at[2]) << 16 | u32(at[3]) << 24;
}
void put_u16(u8* at, u16 value) {
    at[0] = u8(value);
    at[1] = u8(value >> 8);
}
void put_u32(u8* at, u32 value) {
    for (int i = 0; i < 4; ++i) at[i] = u8(value >> (i * 8));
}

// 新装默认 CR 块（0x449760）：头 + 默认 6×10 高分行 + 练习记录号/关卡 id，
// 其余全 0（mid/progress/练习主体无默认写入者）。
void fill_default_cr_block(u8* block, u32 slot) {
    std::memset(block, 0, ScoreDat::kCRSize);
    put_u16(block + 0, kCRMagic);
    put_u16(block + 2, kBlockVersion);
    put_u32(block + 8, ScoreDat::kCRSize);
    put_u32(block + 0x0C, slot);
    for (int chara = 0; chara < ScoreDat::kCharaCount; ++chara) {
        for (int rank = 0; rank < ScoreDat::kRankCount; ++rank) {
            ScoreDat::Entry entry;
            entry.score = u32(100000 - rank * 10000);
            entry.stage = 1;
            std::memcpy(entry.name.data(), kEntryNameDefault, 9);
            ScoreDat::write_entry(block, chara, rank, entry);
        }
    }
    for (int i = 0; i < ScoreDat::kPracticeCount; ++i) {
        u8* record = block + ScoreDat::kPracticeOff + size_t(i) * ScoreDat::kPracticeSize;
        put_u32(record + 0x90, u32(i));
        put_u32(record + 0x94, u32(kPracticeStageTable[i]));
    }
}

// 新装默认 ST 块（0x449a30）：头 + 8 空格名 + 494 项 LCG 表。
void set_default_st_block(u8* st, u16 seed) {
    std::memset(st, 0, ScoreDat::kSTSize);
    put_u32(st + 0, u32(kSTMagic) | u32(kBlockVersion) << 16);  // 0x00015453（0x449a48）
    put_u32(st + 8, ScoreDat::kSTSize);                         // 块长（0x449a4e）
    std::memcpy(st + ScoreDat::kStNameOff, kStNameDefault, 9);  // 0x449a55-0x449a6f
    u16 state = seed;
    for (int i = 0; i < ScoreDat::kStTableLen; ++i) {
        put_u16(st + ScoreDat::kStTableOff + size_t(i) * 2, ScoreDat::lcg_next(state));
    }
}

}  // namespace

u16 ScoreDat::lcg_next(u16& state) {
    // 0x449a80-0x449ae5 逐指令孪生（Unicorn 读回一致 [U]）。
    const u32 x = u32(u16(state ^ 0x9630) - 0x6553) & 0xFFFF;
    state = u16((x << 2 | x >> 14) & 0xFFFF);  // 16 位循环左移 2
    return state;
}

void ScoreDat::set_defaults(u16 lcg_seed) {
    seed = lcg_seed;
    for (int slot = 0; slot < kCRCount; ++slot) {
        fill_default_cr_block(cr[size_t(slot)].data(), u32(slot));
        cr_present[size_t(slot)] = true;
    }
    set_default_st_block(st.data(), lcg_seed);
    error.clear();
}

void ScoreDat::set_default_cr_block(u32 slot) {
    if (slot >= u32(kCRCount)) return;
    fill_default_cr_block(cr[size_t(slot)].data(), slot);
    cr_present[slot] = true;
}

u32 ScoreDat::block_checksum(const u8* block, size_t size) {
    u32 sum = 0;
    for (size_t i = 8; i < size; ++i) sum += block[i];
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

    // 解密（0x449cd0:00449d2x → 0x472280，AL=0xAC/step=0x35/block=0x10 同保存方向参数）。
    std::vector<u8> plain(data + kHeaderSize, data + kHeaderSize + comp_size);
    ReplayFile::xor_decrypt(plain.data(), plain.size(), kEncryptKey0, kEncryptStep, kEncryptBlock);
    std::vector<u8> image;
    if (!ReplayFile::inflate(plain.data(), plain.size(), image, uncomp_size))
        return fail("score file inflate failed");

    const bool ok = parse_image(image.data(), image.size());
    if (!ok) error = "score file walk: " + error;
    return ok;
}

bool ScoreDat::parse_image(const u8* image, size_t size) {
    error.clear();
    for (int slot = 0; slot < kCRCount; ++slot) cr_present[size_t(slot)] = false;
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
            if (version != kBlockVersion || block_size != kCRSize ||
                checksum != block_checksum(block, kCRSize))
                return fail("CR block failed validation");
            const u32 slot = read_u32(block + 0x0C);
            if (slot >= u32(kCRCount)) return fail("CR slot out of range");
            std::memcpy(cr[slot].data(), block, kCRSize);
            cr_present[slot] = true;
        } else if (magic == kSTMagic) {
            if (version != kBlockVersion || block_size != kSTSize ||
                checksum != block_checksum(block, kSTSize))
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
    // 0x449e50:00449ec0 逐槽：'CR' 命中才收；+0xC=循环槽号（0x449ecb）、
    // +4=校验和（0x449ee0）；memcpy 恒 0x56DC（0x449f11），游标仅命中分支累加
    // （0x449f2f）——缺席块整块剔除，uncompSize 随之缩短（route_c 黄金实证）。
    size_t cursor = 0;
    for (int slot = 0; slot < kCRCount; ++slot) {
        if (!cr_present[size_t(slot)]) continue;
        u8* block = out + cursor;
        std::memcpy(block, cr[size_t(slot)].data(), kCRSize);
        put_u32(block + 0x0C, u32(slot));
        put_u32(block + 4, block_checksum(block, kCRSize));
        cursor += kCRSize;
    }
    // ST：+4=校验和（0x449f59）→ rep movsd 拷 0x42C（0x449fac）。
    std::memcpy(out + cursor, st.data(), kSTSize);
    put_u32(out + cursor + 4, block_checksum(out + cursor, kSTSize));
    cursor += kSTSize;
    return cursor;
}

bool ScoreDat::build_file(std::vector<u8>& out) {
    error.clear();
    // 0x449e50 尾段孪生：镜像 → 0x458c30 压缩（ReplayFile::deflate，
    // bit-exact Unicorn 向量 35/35）→ 0x4723a0 加密（AL=0xAC）→ 0x18 头。
    std::vector<u8> image(kImageSize);
    const size_t uncomp = build_image(image.data(), image.size());
    if (uncomp == 0) return fail("build_image: " + error);
    image.resize(uncomp);
    std::vector<u8> comp = ReplayFile::deflate(image.data(), uncomp);
    if (comp.empty()) return fail("deflate produced no output");
    ReplayFile::xor_encrypt(comp.data(), comp.size(), kEncryptKey0, kEncryptStep, kEncryptBlock);
    // 头（保存路径就地更新的模板）：+0x14=游标（0x449fb5）、+0x10=comp（LZ 回填
    // 0x449fc6）、+4=comp+0x18（0x449fd5）；+0x0C 恒 0x100、版本 4、+0x0A=0。
    out.assign(kHeaderSize + comp.size(), 0);
    std::memcpy(out.data(), kMagic, 4);
    put_u32(out.data() + 4, u32(comp.size() + kHeaderSize));
    put_u16(out.data() + 8, kFileVersion);
    put_u16(out.data() + 0xA, 0);
    put_u32(out.data() + 0xC, kHeaderFieldC);
    put_u32(out.data() + 0x10, u32(comp.size()));
    put_u32(out.data() + 0x14, u32(uncomp));
    std::memcpy(out.data() + kHeaderSize, comp.data(), comp.size());
    return true;
}

ScoreDat::Entry ScoreDat::read_entry(const u8* block, int chara, int rank) {
    Entry entry;
    const u8* at = block + kEntriesOff + size_t(chara * kRankCount + rank) * kEntrySize;
    entry.score = read_u32(at);
    entry.stage = at[4];
    entry.clear = at[5];
    std::memcpy(entry.name.data(), at + 6, 9);
    entry.time_lo = read_u32(at + 0x10);
    entry.time_hi = read_u32(at + 0x14);
    entry.rate_bits = read_u32(at + 0x18);
    return entry;
}

void ScoreDat::write_entry(u8* block, int chara, int rank, const Entry& entry) {
    u8* at = block + kEntriesOff + size_t(chara * kRankCount + rank) * kEntrySize;
    put_u32(at, entry.score);
    at[4] = entry.stage;
    at[5] = entry.clear;
    std::memcpy(at + 6, entry.name.data(), 9);
    at[15] = 0;  // +0x0F 垫：memset 0、无写入者，恒写 0
    put_u32(at + 0x10, entry.time_lo);
    put_u32(at + 0x14, entry.time_hi);
    put_u32(at + 0x18, entry.rate_bits);
}

u32 ScoreDat::progress_u32(const u8* block, size_t offset, int chara) {
    return read_u32(block + offset + size_t(chara) * 4);
}

void ScoreDat::set_progress_u32(u8* block, size_t offset, int chara, u32 value) {
    put_u32(block + offset + size_t(chara) * 4, value);
}

u32 ScoreDat::progress_row_score(const u8* block, int chara, int stage) {
    return read_u32(block + 0x7F0 + size_t(chara * 8 + stage) * 8);
}

void ScoreDat::set_progress_row_score(u8* block, int chara, int stage, u32 value) {
    put_u32(block + 0x7F0 + size_t(chara * 8 + stage) * 8, value);
}

u8 ScoreDat::progress_row_flag(const u8* block, int chara, int stage, int which) {
    return block[0x7F0 + size_t(chara * 8 + stage) * 8 + 4 + size_t(which)];
}

void ScoreDat::set_progress_row_flag(u8* block, int chara, int stage, int which, u8 value) {
    block[0x7F0 + size_t(chara * 8 + stage) * 8 + 4 + size_t(which)] = value;
}

u32 ScoreDat::practice_u32(const u8* block, int i, size_t field_offset) {
    return read_u32(block + kPracticeOff + size_t(i) * kPracticeSize + field_offset);
}

void ScoreDat::set_practice_u32(u8* block, int i, size_t field_offset, u32 value) {
    put_u32(block + kPracticeOff + size_t(i) * kPracticeSize + field_offset, value);
}

u64 ScoreDat::max_entry_timestamp() const {
    u64 newest = 0;
    for (int slot = 0; slot < kCRCount; ++slot) {
        if (!cr_present[size_t(slot)]) continue;
        const u8* block = cr[size_t(slot)].data();
        for (int chara = 0; chara < kCharaCount; ++chara) {
            for (int rank = 0; rank < kRankCount; ++rank) {
                const Entry entry = read_entry(block, chara, rank);
                newest = std::max(newest,
                                  u64(entry.time_lo) | u64(entry.time_hi) << 32);
            }
        }
    }
    return newest;
}

std::array<u8, 9> ScoreDat::st_name(const u8* st) {
    std::array<u8, 9> name{};
    std::memcpy(name.data(), st + kStNameOff, 9);
    return name;
}

void ScoreDat::set_st_name(u8* st, const std::array<u8, 9>& name) {
    std::memcpy(st + kStNameOff, name.data(), 9);
}

void ScoreDat::read_ending_bytes(const u8* st, u8 (&out)[kEndingBytes]) {
    std::memcpy(out, st + kStEndingOff, kEndingBytes);
}

void ScoreDat::write_ending_bytes(u8* st, const u8 (&in)[kEndingBytes]) {
    std::memcpy(st + kStEndingOff, in, kEndingBytes);
}

}  // namespace th13
