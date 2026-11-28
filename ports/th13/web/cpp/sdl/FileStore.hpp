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

inline constexpr i32 kSaveCharacters = 4;
inline constexpr i32 kSaveDifficulties = 5;
inline constexpr i32 kSaveSpellSlots = 0x7f;
inline constexpr i32 kSaveSpellColumns = 5;

struct SavedSpellCounters {
    std::array<i32, 2> captures{};
    std::array<i32, 2> attempts{};
    i32 practice_best_score = 0;
};

struct SavedSpellRecord {
    std::string name;
    std::array<SavedSpellCounters, kSaveSpellColumns> columns{};
};

struct SavedOrdinaryRecords {
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
    u32 cleared_mask = 0;
    u32 heard_mask = 0;
    std::array<u8, 16> practice_played{};
    bool practice_stage_played(i32 character, i32 difficulty, i32 stage) const;
    void mark_practice_stage_played(i32 character, i32 difficulty, i32 stage);
    std::array<u8, 16> practice_cleared{};
    bool practice_stage_cleared(i32 character, i32 difficulty, i32 stage) const;
    void mark_practice_stage_cleared(i32 character, i32 difficulty, i32 stage);
    u32 extra_unlocked_mask = 0;
    u32 legacy_practice_extra_overlap = 0;
    std::array<i16, 5> pad_bindings = {0, 1, 2, 3, 4};
    std::array<SavedSpellRecord, kSaveSpellSlots> spell_records{};
    std::array<SavedOrdinaryRecords, kSaveCharacters> ordinary_records{};
    std::array<u8, 13> ending_seen_bytes{};
    std::string last_name = "        ";
    void remember_name(const std::string& name);
    bool ending_history_complete = true;
    bool player_data_summary(i32 difficulty, i32 character, TitlePlayerDataSummary& out) const;
    bool record_run_start(i32 character);
    bool record_play_frame(i32 character);
    bool record_run_clear(i32 character, i32 difficulty, bool no_continue);
    i32 record_high_score(i32 character, i32 difficulty, const TitlePlayerDataScore& score);
    const std::vector<u8>& migrated_save_bytes() const { return migrated_save_bytes_; }
    const std::vector<u8>& pre_ending_save_bytes() const { return pre_ending_save_bytes_; }

    bool record_spell_attempt(i32 character, i32 slot, const std::string& name, bool spell_practice);
    bool record_spell_capture(i32 character, i32 slot, bool spell_practice);
    bool record_spell_practice_score(i32 character, i32 slot, i32 score);
    const SavedSpellCounters* spell_counters(i32 character, i32 slot) const;
    static i32 spell_difficulty(i32 slot);
    static i32 player_data_record_count(i32 difficulty);
    bool player_data_record(i32 difficulty, i32 character, i32 ordinal, TitlePlayerDataRecord& out) const;
    bool extra_unlocked(i32 character) const {
        return character >= 0 && character < 4 && (extra_unlocked_mask >> character) & 1u;
    }
    void set_extra_unlocked(i32 character) {
        if (character >= 0 && character < 4) extra_unlocked_mask |= 1u << character;
    }
    bool loaded = false;
    u32 legacy_save_size() const { return legacy_save_size_; }
    const std::array<u8, 64>& legacy_save_bytes() const { return legacy_save_bytes_; }

    bool initialize();
    bool save();
    bool save_score();

    ScoreDat score_dat;
    static_assert(kSaveSpellColumns == ScoreDat::kColumnCount);
    static_assert(kSaveDifficulties == ScoreDat::kDifficultySlots - 1);
    u16 score_dat_seed = 0;
    bool score_dat_loaded() const { return score_dat_loaded_; }
    bool score_dat_should_import(u64 dat_max_timestamp) const {
        return !loaded || dat_max_timestamp > u64(max_ranking_timestamp());
    }
    i64 max_ranking_timestamp() const;
    static constexpr u32 kScoreDatTokensPerFrame = 400;
    bool advance_pending_writes(u32 tokens = kScoreDatTokensPerFrame);
    bool pending_writes() const { return score_dat_encoder_.active(); }
    void sync_score_face_to_dat();
    void overlay_score_face_from_dat();

    bool cleared(i32 character, i32 difficulty) const;
    void set_cleared(i32 character, i32 difficulty);
    bool music_heard(i32 track) const { return track >= 0 && track < 32 && (heard_mask >> u32(track)) & 1u; }
    void set_music_heard(i32 track) {
        if (track >= 0 && track < 32) heard_mask |= 1u << u32(track);
    }

private:
    u32 legacy_save_size_ = 0;
    std::array<u8, 64> legacy_save_bytes_{};
    std::vector<u8> migrated_save_bytes_;
    std::vector<u8> pre_ending_save_bytes_;
    std::array<u8, 13> committed_ending_bytes_{};
    std::vector<u8> persisted_save_bytes_;
    std::vector<u8> persisted_score_dat_image_;
    std::vector<u8> score_dat_image_build_;
    std::vector<u8> pending_score_dat_image_;
    LzssEncoder score_dat_encoder_;
    bool score_dat_loaded_ = false;
    bool write_save(bool commit_ending);
    bool load_save();
    bool read_score_dat(std::vector<u8>& out);
    bool write_score_dat();
};

}
