// th16 成绩数据——原版 DAT_004a6f0c（0x4497e0 构造，0x1a3ac 字节）。
//
// 内存布局与 scoreth16.dat 解压后的载荷逐字节相同：
//   +0x00      文件头指针 / +0x04 解压缓冲指针（宿主不需要，不存）
//   +0x08      5 个机体块 "CR" ver 1（0x4493c0，每块 0x5318；块 4 为全机体合计）
//   +0x19f80   公共块 "ST" ver 1（0x449720，0x42c）
// 读盘 0x402440 → 0x449880：头 "TH61" ver 2，载荷 0x402220(0xac, '5', 0x10) 后 0x457f00 LZSS，
//   再按块魔数 / 版本 / 校验和（0x449350：+8 起逐字节和）/ 尺寸覆盖默认值；任何一项不符就保留默认。
// 写盘 0x449a00：各块重算校验和、CR 块写序号，LZSS 0x457b20 → 0x402330 → 头 0x18 + 载荷。
#pragma once
#include <cstddef>
#include <cstring>
#include <vector>

#include "Types.hpp"

namespace th16 {

struct ScoreEntry {
    u32 score = 0;          // +0x00
    u8 stage = 0;           // +0x04
    u8 continues = 0;       // +0x05
    char name[9] = {};      // +0x06（8 字 + NUL）
    i64 time = 0;           // +0x10（__time64）
    u32 slowdown = 0;       // +0x18：100 − 实际帧率百分比
    i32 season = 0;         // +0x1c
};
static_assert(sizeof(ScoreEntry) == 0x20, "ScoreEntry must be 0x20 bytes");

struct PracticeRecord {
    u32 score = 0;          // +0x00（文件 +0x5198）
    u8 cleared = 0;         // +0x04（文件 +0x519c：0x42e150 过关置 1）
    u8 reached = 0;         // +0x05（文件 +0x519d：0x42cb60 非回放装载该关置 1；练习选关 / 分数表据此解锁）
    u8 pad_06[2] = {};
};
static_assert(sizeof(PracticeRecord) == 8, "PracticeRecord must be 8 bytes");

struct SpellRecord {
    char name[0x80] = {};   // +0x00
    u32 captures[2] = {};   // +0x80：[0] 正常 / [1] 符卡练习
    u32 attempts[2] = {};   // +0x88
    i32 id = 0;             // +0x90：0x4493c0 写入序号
    i32 difficulty = 0;     // +0x94：0x4493c0 抄 DAT_00491700
    u32 high_score = 0;     // +0x98：符卡练习最高分（0x42e150 写入；SCORE `%8d0`）
};
static_assert(sizeof(SpellRecord) == 0x9c, "SpellRecord must match scoreth16.dat 0x9c");

// 0x4493c0：机体块（文件偏移 = 8 + 机体 × 0x5318；下列注释为块内偏移）
struct ScoreBlock {
    static constexpr u32 kMagic = 0x5243;     // "CR"
    static constexpr u32 kSize = 0x5318;
    static constexpr int kDifficulties = 6;
    static constexpr int kRanks = 10;
    static constexpr int kSpellRecords = 119;
    static constexpr int kPracticeSlots = kDifficulties * 8;  // 难度 × 8 + 关卡

    u16 magic = 0;                            // +0x00
    u16 version = 0;                          // +0x02
    u32 checksum = 0;                         // +0x04
    u32 size = 0;                             // +0x08
    u32 index = 0;                            // +0x0c：0x449a00 写机体序号，0x449880 据此放回
    ScoreEntry ranks[kDifficulties][kRanks];  // +0x10
    // +0x790。尾部 0x9c 在 0x456d50 里被当成 spells[-1] 读取（未解锁槽 id == -1）；默认全 0。
    u8 unused_790[0x140] = {};
    SpellRecord spells[kSpellRecords];        // +0x8d0
    u32 play_count = 0;                       // +0x5154：0x42cb60 新游戏且非回放 +1（上限 9999999）
    u64 play_time = 0;                        // +0x5158：0x42dbc0 累计游戏时长（1/100 秒）
    u32 clears[7] = {};                       // +0x5160：0x42e150 各难度通关次数（上限 99999）
    u32 no_continue_clears[5] = {};           // +0x517c：未续关通关次数
    PracticeRecord practice[kPracticeSlots];  // +0x5190
    u8 unused_5310[8] = {};                   // +0x5310
};
static_assert(sizeof(ScoreBlock) == ScoreBlock::kSize, "ScoreBlock must be 0x5318 bytes");
static_assert(offsetof(ScoreBlock, spells) == 0x8d0, "spells at block +0x8d0 (file +0x8d8)");
static_assert(offsetof(ScoreBlock, spells) >= sizeof(SpellRecord), "spells[-1] stays inside the block");
static_assert(offsetof(ScoreBlock, play_count) == 0x5154, "play count at file +0x515c");
static_assert(offsetof(ScoreBlock, clears) == 0x5160, "clears at file +0x5168");
static_assert(offsetof(ScoreBlock, no_continue_clears) == 0x517c, "no-continue clears at file +0x5184");
static_assert(offsetof(ScoreBlock, practice) == 0x5190, "practice at file +0x5198");

// 0x449720：公共块（文件偏移 0x19f80；下列注释为块内偏移）
struct ScoreStats {
    static constexpr u32 kMagic = 0x5453;     // "ST"
    static constexpr u32 kSize = 0x42c;
    static constexpr int kBgmFlags = 32;
    static constexpr int kNoise = 0x1ee;

    u16 magic = 0;                            // +0x00
    u16 version = 0;                          // +0x02
    u32 checksum = 0;                         // +0x04
    u32 size = 0;                             // +0x08
    char last_name[9] = {};                   // +0x0c：最近输入的名字（默认 8 空格）
    u8 pad_15 = 0;                            // +0x15
    // +0x16：8 个结局「见过」位图（bit0 见过；bit1.. 难度标记）；+0x1e 任一结局见过（0x44a930 把 +0x16..+0x25 全置 0x11）
    u8 ending_seen[8] = {};
    u8 any_ending_seen = 0;
    u8 unused_1f[7] = {};
    // +0x26：音乐室「已听过」位（0x43c3f0 / 0x429ff0 / 0x42dee0 播曲时置 1；0x4546f0 据此显示曲名或 ？？？）
    u8 bgm_heard[kBgmFlags] = {};
    u8 pad_46[2] = {};
    u32 play_time[2] = {};                    // +0x48：0x42dbc0 总游戏时长（u64 低 / 高，1/100 秒；块只按 4 字节对齐）
    u16 noise[kNoise] = {};                   // +0x50：0x449720 以 DAT_004a6d88 逐个单步填充
};
static_assert(sizeof(ScoreStats) == ScoreStats::kSize, "ScoreStats must be 0x42c bytes");
static_assert(offsetof(ScoreStats, ending_seen) == 0x16, "ending flags at file +0x19f96");
static_assert(offsetof(ScoreStats, bgm_heard) == 0x26, "bgm flags at file +0x19fa6");
static_assert(offsetof(ScoreStats, play_time) == 0x48, "total play time at file +0x19fc8");

struct ScoreFile {
    static constexpr int kShots = 5;          // 机体块数（shot + shot_aux；块 4 为全机体合计）
    static constexpr int kAllShotsBlock = 4;
    static constexpr int kSpellRecords = ScoreBlock::kSpellRecords;
    static constexpr u32 kCounterLimit = 99999;
    static constexpr int kDifficulties = ScoreBlock::kDifficulties;
    static constexpr int kRanks = ScoreBlock::kRanks;
    static constexpr int kPracticeSlots = ScoreBlock::kPracticeSlots;
    static constexpr int kBgmFlags = ScoreStats::kBgmFlags;
    static constexpr const char* kDefaultName = "        ";   // DAT_00492828
    static constexpr u32 kFileMagic = 0x31364854;             // "TH61"
    static constexpr u32 kFileHeaderSize = 0x18;
    static constexpr u8 kCryptKey = 0xac, kCryptStep = '5';
    static constexpr u32 kCryptBlock = 0x10;

    ScoreBlock blocks[kShots];                // +0x08
    ScoreStats stats;                         // +0x19f80
    // 0x449880 读入成功时保留原文件头（+0xc 等字段原样写回）；否则为新建头
    u8 file_header[kFileHeaderSize] = {};

    ScoreFile() { reset(); }

    // 0x4497e0 去掉读盘：0x449720 公共块 + 5 × 0x4493c0 机体块 + 新建文件头。noise 由 GameSession 用全局 RNG 填
    void reset();
    // 0x449880：解析 scoreth16.dat 原始字节，合法块覆盖当前内容；返回是否采用了文件头
    bool load_image(const u8* bytes, u32 size);
    // 0x449a00：生成写盘字节（同时像原版一样回写各块校验和 / 序号与文件头尺寸）
    std::vector<u8> save_image();
    // 0x449350：块 +8 起到 size 的逐字节和
    static u32 block_checksum(const void* block, u32 size);

    u64 total_play_time() const { return u64(stats.play_time[0]) | (u64(stats.play_time[1]) << 32); }

    void mark_bgm_heard(int track) {
        if (track >= 0 && track < kBgmFlags) stats.bgm_heard[track] = 1;
    }

    // 0x43e250：返回插入位置，未入榜返回 -1
    i32 insert_rank(int shot, int difficulty, u32 score, u8 stage, u8 continues, i32 season, u32 slowdown, i64 time) {
        ScoreEntry* table = blocks[shot].ranks[difficulty];
        for (int rank = 0; rank < kRanks; ++rank) {
            if (table[rank].score > score) continue;
            for (int i = kRanks - 1; i > rank; --i) table[i] = table[i - 1];
            ScoreEntry& e = table[rank];
            e.score = score;
            e.stage = stage;
            e.continues = continues;
            e.time = time;
            std::memcpy(e.name, kDefaultName, 9);
            e.slowdown = slowdown;
            e.season = season;
            return rank;
        }
        return -1;
    }

    // 0x417f00 符卡开始（非回放）：当前机体块与合计块都登记名字、挑战数 +1
    void record_spell_attempt(int shot, int spell, bool practice, const char* name) {
        if (!spell_record_valid(shot, spell)) return;
        for (int block : {shot, kAllShotsBlock}) {
            SpellRecord& record = blocks[block].spells[spell];
            std::strncpy(record.name, name, sizeof(record.name) - 1);
            record.name[sizeof(record.name) - 1] = '\0';
            if (record.attempts[practice] < kCounterLimit) ++record.attempts[practice];
        }
    }
    // 0x4182f0 符卡取得（非回放）：两块取得数 +1
    void record_spell_capture(int shot, int spell, bool practice) {
        if (!spell_record_valid(shot, spell)) return;
        for (int block : {shot, kAllShotsBlock}) {
            SpellRecord& record = blocks[block].spells[spell];
            if (record.captures[practice] < kCounterLimit) ++record.captures[practice];
        }
    }
    const SpellRecord* spell_record(int shot, int spell) const {
        return spell_record_valid(shot, spell) ? &blocks[shot].spells[spell] : nullptr;
    }
    const SpellRecord& all_shots_spell(int spell) const { return blocks[kAllShotsBlock].spells[spell]; }
    static bool spell_record_valid(int shot, int spell) {
        return shot >= 0 && shot < kAllShotsBlock && spell >= 0 && spell < kSpellRecords;
    }

    // 0x42e150 第 6 面 / Extra 通关（非回放）：通关次数 +1，未续关再 +1
    void record_clear(int shot, int difficulty, bool no_continue) {
        ScoreBlock& b = blocks[shot];
        if (b.clears[difficulty] < kCounterLimit) ++b.clears[difficulty];
        if (no_continue && b.no_continue_clears[difficulty] < kCounterLimit) ++b.no_continue_clears[difficulty];
    }
    // 0x42e150 过关（非回放、非符卡练习）：+0x519c
    void mark_practice_cleared(int shot, int difficulty, int stage) { blocks[shot].practice[difficulty * 8 + stage].cleared = 1; }
    // 0x42cb60 装载关卡（非回放）：+0x519d
    void mark_practice_reached(int shot, int difficulty, int stage) { blocks[shot].practice[difficulty * 8 + stage].reached = 1; }
    // 0x42cb60 新游戏（非回放）：+0x515c 游玩次数
    void record_play(int shot) {
        if (blocks[shot].play_count < 9999999u) ++blocks[shot].play_count;
    }
    // 0x44a930：Player Data 隐藏键序的全解锁
    void unlock_all();

    // 0x44a800：该机体在 Easy..Lunatic 任一难度有未续关通关（+0x5184 + d × 4）→ Extra 可选该机体
    bool shot_extra_unlocked(int shot) const {
        for (int d = 0; d < 4; ++d)
            if (blocks[shot].no_continue_clears[d] != 0) return true;
        return false;
    }
    // 0x44a850：机体 0..3 任一满足 0x44a800 → 标题 Extra 项可选
    bool extra_unlocked() const {
        for (int shot = 0; shot < 4; ++shot)
            if (shot_extra_unlocked(shot)) return true;
        return false;
    }
    // 0x44a8e0：四个机体在该难度都有未续关通关（难度面板保留 0x9e+d 通关章）
    bool difficulty_cleared_by_all(int difficulty) const {
        for (int shot = 0; shot < 4; ++shot)
            if (blocks[shot].no_continue_clears[difficulty] == 0) return false;
        return true;
    }

    // 0x43f7e0 练习分支：练习最高分只升不降
    void update_practice(int shot, int difficulty, int stage, u32 score) {
        PracticeRecord& r = blocks[shot].practice[difficulty * 8 + stage];
        if (r.score < score) r.score = score;
    }

    // 0x42e150 符卡练习：+0x970 最高分只升不降（分值先 /10*10）
    void update_spell_high_score(int shot, int spell, u32 score) {
        if (!spell_record_valid(shot, spell)) return;
        const u32 floored = (score / 10u) * 10u;
        SpellRecord& record = blocks[shot].spells[spell];
        if (record.high_score < floored) record.high_score = floored;
    }
};
static_assert(offsetof(ScoreFile, stats) == 0x19f78, "ST block follows the 5 CR blocks (file +0x19f80)");

}  // namespace th16
