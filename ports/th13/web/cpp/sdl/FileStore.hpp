// TH13SAVE v7 应用存档与 scoreth13.dat 原版格式镜像。
// 应用存档保留完整设置和得分历史；原版格式通道同步双方可表达的字段。
// 原版通关标记地址：score + 0x56DC*character + 0x7E4 + difficulty*4。
// 后端：native/WASI = TH13_SAVE_PATH 或 cwd th13-save.bin；web = localStorage。
#pragma once

#include "../game/ReplayFile.hpp"
#include "../game/ScoreDat.hpp"
#include "../game/TitlePlayerDataMenu.hpp"
#include "../game/Types.hpp"
#include <array>
#include <string>

namespace th13 {
struct TitlePlayerDataRecord;
}

namespace th13::sdl {

inline constexpr i32 kSaveCharacters = 4;    // 霊夢/魔理沙/早苗/妖夢
inline constexpr i32 kSaveDifficulties = 5;  // Easy..Lunatic(0..3) + Extra(4)
inline constexpr i32 kSaveSpellSlots = 0x7f;
inline constexpr i32 kSaveSpellColumns = 5;  // four characters and the aggregate column

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
    std::string name;  // original 0x80-byte name field, at most 127 bytes plus NUL
    std::array<SavedSpellCounters, kSaveSpellColumns> columns{};
};

struct SavedOrdinaryRecords {
    // The original file has six ranking tables; the player-data UI exposes five.
    std::array<std::array<TitlePlayerDataScore, ScoreDat::kRankCount>, ScoreDat::kDifficultySlots> ranking{};
    std::array<i32, ScoreDat::kDifficultySlots> clear_count{}, no_continue_clear_count{};
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
    // Score ST+0x0C (0x449a55): the last confirmed name, eight bytes padded
    // with spaces. Every name entry seeds from it and every END writes it.
    std::string last_name = "        ";
    void remember_name(const std::string& name);
    bool ending_history_complete = true;  // v1–v5 did not encode this history
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
    bool record_spell_attempt(i32 character, i32 slot, const std::string& name, bool spell_practice);
    bool record_spell_capture(i32 character, i32 slot, bool spell_practice);
    bool record_spell_practice_score(i32 character, i32 slot, i32 score);
    const SavedSpellCounters* spell_counters(i32 character, i32 slot) const;
    static i32 spell_difficulty(i32 slot);  // original DAT_004bbd30 table, not run rank
    static i32 player_data_record_count(i32 difficulty);
    bool player_data_record(i32 difficulty, i32 character, i32 ordinal, TitlePlayerDataRecord& out) const;
    bool extra_unlocked(i32 character) const {
        return character >= 0 && character < 4 && (extra_unlocked_mask >> character) & 1u;
    }
    void set_extra_unlocked(i32 character) {
        if (character >= 0 && character < 4) extra_unlocked_mask |= 1u << character;
    }
    bool loaded = false;  // true = 读到并采用合法档；false = 新档默认值
    // Immutable migration provenance. Unknown bits and invalid-but-readable
    // settings remain recoverable even when the effective state is normalized.
    u32 legacy_save_size() const { return legacy_save_size_; }
    const std::array<u8, 64>& legacy_save_bytes() const { return legacy_save_bytes_; }

    bool initialize();  // 装载（缺档/坏档 = 默认值；I/O 硬失败 false + error）
    bool save();        // 真实写入；失败 false + error（绝不假成功）
    // 0042bfa0/004399e0: commit live ending bytes at a retail score save point.
    // Other port saves (settings/music/spell events) retain their last commit.
    bool save_score();

    // v7 "TH13SAVE" 是应用侧权威。th13.scoredat 是 save_score() 保存点写出的
    // 原版格式镜像；dat 仅在 v7 缺失/无效，或 dat 榜表最大时间戳更新时导入。
    // 块字节与字段视图见 game/ScoreDat.hpp，字段映射见 docs/plans/scoredat.md。
    ScoreDat score_dat;
    static_assert(kSaveSpellColumns == ScoreDat::kColumnCount);
    static_assert(kSaveDifficulties == ScoreDat::kDifficultySlots - 1);
    // boot 时 (u16)时钟（对照原版 (u16)timeGetTime() @0x43a040）：ST LCG 表
    // 每次开机生成一次；文件 ST 块校验通过则整体覆盖回文件值（0x449b00→0x449cd0），
    // 即首存后随存档持久、不重生成。
    u16 score_dat_seed = 0;
    bool score_dat_loaded() const { return score_dat_loaded_; }
    // dat 导入裁决：(a) 应用档缺失/无效，或 (b) dat 最大时间戳更新。
    bool score_dat_should_import(u64 dat_max_timestamp) const {
        return !loaded || dat_max_timestamp > u64(max_ranking_timestamp());
    }
    // 榜表（全部角色×表×行）最大 u64 时间戳；无任何有效行时为 0。
    i64 max_ranking_timestamp() const;
    // 得分面映射层（save_score 落镜像 / initialize 导入覆盖的内部步骤；public
    // 供单测直测映射保真——native 通道不激活，无这两个入口外的写入路径）。
    // 每帧推进一片 scoreth13.dat 压缩（kScoreDatTokensPerFrame 个 token ≈
    // 关终门帧预算的零头）；tokens 取 ~0u 即同步补齐。存储写失败同 save_score
    // 语义：false + error。
    static constexpr u32 kScoreDatTokensPerFrame = 400;
    bool advance_pending_writes(u32 tokens = kScoreDatTokensPerFrame);
    bool pending_writes() const { return score_dat_encoder_.active(); }
    void sync_score_face_to_dat();       // FileStore 字段 → ScoreDat 镜像（save_score）
    void overlay_score_face_from_dat();  // ScoreDat 镜像 → FileStore 字段（导入）

    bool cleared(i32 character, i32 difficulty) const;
    void set_cleared(i32 character, i32 difficulty);  // 通关落档（bit 置位）
    // MusicRoom 听过位（musicroom-host.md §2 建议的内联读写面）。
    bool music_heard(i32 track) const { return track >= 0 && track < 32 && (heard_mask >> u32(track)) & 1u; }
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
    // 存储层已持有的字节（web 通道）：序列化结果未变的保存点不重复写入，
    // scoreth13.dat 镜像未变时不重复压缩。
    std::vector<u8> persisted_save_bytes_;
    std::vector<u8> persisted_score_dat_image_;
    std::vector<u8> score_dat_image_build_;
    std::vector<u8> pending_score_dat_image_;
    LzssEncoder score_dat_encoder_;
    bool score_dat_loaded_ = false;
    bool write_save(bool commit_ending);
    // TH13SAVE 读档体（缺档/坏档 true + 默认值）。
    bool load_save();
    // th13.scoredat 通道读写：web=localStorage；native 通道不激活（读恒缺档、
    // 写恒 no-op，不触碰任何路径）。
    bool read_score_dat(std::vector<u8>& out);
    bool write_score_dat();
};

}  // namespace th13::sdl
