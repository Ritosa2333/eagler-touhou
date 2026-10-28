// 文件存储（真实持久化，替换旧桩：不再 return true 假成功）。
// 数据 = 原版 scoreth13.dat 的移植侧子集（v6；兼容 v1–v5）：
//   - BGM/SE 音量（原版存档字节 DAT_004dc892/893，默认 100/80 = 0x5064）。
//   - 每角色×难度通关标记 cleared(char 0..3, diff 0..4)：原版 score 数据块
//     布局 *(int*)(score + 0x56dc*char + 0x7e4 + diff*4) != 0（0x451450
//     case0 角色 クリア済 札门实证）；新档全 0 = 全隐，与真机缺省一致。
// 后端：native/WASI = 真实文件（TH13_SAVE_PATH 或 cwd th13-save.bin）；
// web = localStorage("th13.save")（EM_JS）。失败如实置 error 返回 false。
// 损坏/缺档：按原版 scoreth13.dat 语义取默认值，不报错。
#pragma once

#include "../game/Types.hpp"
#include "../game/TitlePlayerDataMenu.hpp"
#include <string>
#include <array>

namespace th13 { struct TitlePlayerDataRecord; }

namespace th13::sdl {

inline constexpr i32 kSaveCharacters = 4;   // 霊夢/魔理沙/早苗/妖夢
inline constexpr i32 kSaveDifficulties = 5; // Easy..Lunatic(0..3) + Extra(4)
inline constexpr i32 kSaveSpellSlots = 0x7f;
inline constexpr i32 kSaveSpellColumns = 5; // four characters and the aggregate column

struct SavedSpellCounters {
    // 00413270: +0x88/+0x8c attempts; 00413a60: +0x80/+0x84 captures.
    // index 0 = ordinary play, index 1 = spell practice; each saturates at 99999.
    std::array<i32, 2> captures{};
    std::array<i32, 2> attempts{};
    // 0042cd40 record+0x98: internal score units (displayed points / 10),
    // rounded down to an internal multiple of 10; 004579a0 prints "%8d0".
    i32 practice_best_score = 0;
};

struct SavedSpellRecord {
    std::string name; // original 0x80-byte name field, at most 127 bytes plus NUL
    std::array<SavedSpellCounters, kSaveSpellColumns> columns{};
};

struct SavedOrdinaryRecords {
    // 00449760 initializes six rank tables; menu 00453170 exposes the first five.
    std::array<std::array<TitlePlayerDataScore, 10>, 6> ranking{};
    std::array<i32, 6> clear_count{}, no_continue_clear_count{};
    i32 play_count = 0, total_frames = 0;
    bool history_complete = true;
    SavedOrdinaryRecords();
};

class FileStore {
public:
    std::string error;
    i32 bgm_volume = 100, effect_volume = 80;
    u32 cleared_mask = 0;  // bit = char*kSaveDifficulties + diff
    // v1 reserved[0]（buffer+24）：MusicRoom 曲目听过位（bit = 曲号，0x4554f0
    // 落档族；原版 score 块 +0x1b27a 字节 0 语义：新档全 0 = 全 "???"，
    // track0 由首次播曲解锁）。格式不变（v1 36B 布局原即保留 3 u32）。
    u32 heard_mask = 0;
    // v2 extension: 4 characters * 4 ordinary ranks, bits0..5 = stages1..6.
    // v1 has no played data, so every bit remains zero when loading old saves.
    std::array<u8, 16> practice_played{};
    bool practice_stage_played(i32 character, i32 difficulty, i32 stage) const;
    void mark_practice_stage_played(i32 character, i32 difficulty, i32 stage);
    // Extra 解锁位（原版 score 块 0x1b26b+char*2 bit0x10：0x416130 通关落档
    // diff>0 时置位、0x44bec0 读侧；0x44c570 case0 经 +0x5c40 bit1 门 Extra
    // 行 + 0x44cd10/0x451450 的 rank 屏 5 行分支）。v4 独立存于 +64；
    // v2/v3 曾与 practice_played[0] 的低4位重叠，旧档只能保留旧读法。
    u32 extra_unlocked_mask = 0;
    // v2/v3 +36 的重叠位无法区分来源；迁移保留两侧旧值并显式留存歧义，
    // 不据此推断新的原版进度。v4 新档为0，后续写入不再制造重叠。
    u32 legacy_practice_extra_overlap = 0;
    // v3 手柄五键绑定：0=Shot(Z), 1=Special(X), 2=Trance(C),
    // 3=Slow(Shift), 4=Pause(Esc)，对应0044fcc0行0..4。默认按钮序号0..4。
    std::array<i16, 5> pad_bindings = {0, 1, 2, 3, 4};
    std::array<SavedSpellRecord, kSaveSpellSlots> spell_records{};
    std::array<SavedOrdinaryRecords, kSaveCharacters> ordinary_records{};
    // 00416130 reads/writes score+0x1b26a..0x1b276 in memory. Persist with
    // the score at subsequent save points, never infer these from Extra bits.
    std::array<u8, 13> ending_seen_bytes{};
    bool ending_history_complete = true; // v1–v5 did not encode this history
    bool player_data_summary(i32 difficulty, i32 character, TitlePlayerDataSummary& out) const;
    bool record_run_start(i32 character);
    bool record_play_frame(i32 character);
    bool record_run_clear(i32 character, i32 difficulty, bool no_continue);
    // Returns inserted ranking ordinal (ties precede old entries), -1 if not
    // ranked/invalid. The caller supplies the actual clock/name/stage/slowdown.
    i32 record_high_score(i32 character, i32 difficulty, const TitlePlayerDataScore& score);
    const std::vector<u8>& migrated_save_bytes() const { return migrated_save_bytes_; }
    const std::vector<u8>& pre_ending_save_bytes() const { return pre_ending_save_bytes_; }

    // Only real spell events call these methods; the caller excludes Replay,
    // matching score-session +0x10 != 1 in 00413270/00413a60.
    bool record_spell_attempt(i32 character, i32 slot, const std::string& name,
                              bool spell_practice);
    bool record_spell_capture(i32 character, i32 slot, bool spell_practice);
    bool record_spell_practice_score(i32 character, i32 slot, i32 score);
    const SavedSpellCounters* spell_counters(i32 character, i32 slot) const;
    static i32 spell_difficulty(i32 slot); // original DAT_004bbd30 table, not run rank
    static i32 player_data_record_count(i32 difficulty);
    bool player_data_record(i32 difficulty, i32 character, i32 ordinal,
                            TitlePlayerDataRecord& out) const;
    bool extra_unlocked(i32 character) const {
        return character >= 0 && character < 4 && (extra_unlocked_mask >> character) & 1u;
    }
    void set_extra_unlocked(i32 character) {
        if (character >= 0 && character < 4) extra_unlocked_mask |= 1u << character;
    }
    bool loaded = false;   // true = 读到并采用合法档；false = 新档默认值
    // Immutable migration provenance. Unknown bits and invalid-but-readable
    // settings remain recoverable even when the effective state is normalized.
    u32 legacy_save_size() const { return legacy_save_size_; }
    const std::array<u8, 64>& legacy_save_bytes() const { return legacy_save_bytes_; }

    bool initialize();  // 装载（缺档/坏档 = 默认值；I/O 硬失败 false + error）
    bool save();        // 真实写入；失败 false + error（绝不假成功）
    // 0042bfa0/004399e0: commit live ending bytes at a retail score save point.
    // Other port saves (settings/music/spell events) retain their last commit.
    bool save_score();

    bool cleared(i32 character, i32 difficulty) const;
    void set_cleared(i32 character, i32 difficulty);  // 通关落档（bit 置位）
    // MusicRoom 听过位（musicroom-host.md §2 建议的内联读写面）。
    bool music_heard(i32 track) const {
        return track >= 0 && track < 32 && (heard_mask >> u32(track)) & 1u;
    }
    void set_music_heard(i32 track) {
        if (track >= 0 && track < 32) heard_mask |= 1u << u32(track);
    }
private:
    u32 legacy_save_size_ = 0;
    std::array<u8, 64> legacy_save_bytes_{};
    std::vector<u8> migrated_save_bytes_;
    // Immutable full v5 import, including unknown values and its v1–v4 archive.
    std::vector<u8> pre_ending_save_bytes_;
    std::array<u8, 13> committed_ending_bytes_{};
    bool write_save(bool commit_ending);
};

}  // namespace th13::sdl
