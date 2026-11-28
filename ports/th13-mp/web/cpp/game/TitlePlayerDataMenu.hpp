#pragma once

#include "TitleFailure.hpp"

#include "TitleState.hpp"

#include <array>
#include <functional>
#include <string>
#include <vector>

namespace th13 {

inline constexpr i32 kTitleScreenPlayerData = 10;
inline constexpr i32 kPlayerDataRecordSlots = 0x7f;
inline constexpr i32 kPlayerDataPageSize = 10;
inline constexpr i32 kPlayerDataDifficulties = 5;
inline constexpr i32 kPlayerDataCharacters = 4;

struct TitlePlayerDataScore {
    i32 score = 0, stage = 0, continues = 0;
    std::string name;
    i64 timestamp = 0;
    float progress_percent = 0;
    bool available = false;
};

struct TitlePlayerDataSummary {
    std::array<TitlePlayerDataScore, 10> ranking{};
    i32 play_count = 0, total_frames = 0, clear_count = 0, no_continue_clear_count = 0;
    bool available = false;
    bool history_complete = false;
};

struct TitlePlayerDataRecord {
    i32 slot = -1;
    std::string name;
    i32 score = 0;
    i32 number_a = 0;
    i32 number_b = 0;
    i32 normal_captures = 0, normal_attempts = 0;
    i32 practice_captures = 0, practice_attempts = 0;
    i32 practice_best_score = 0;
    bool recorded = false;
    bool character_flagged = false;
};

struct TitlePlayerDataPlatform {
    virtual ~TitlePlayerDataPlatform() = default;
    virtual bool open_player_data() { return false; }
    virtual void close_player_data() {}
    virtual i32 difficulty_count() const { return 0; }
    virtual i32 character_count() const { return 0; }
    virtual i32 record_count(i32 difficulty) const {
        (void)difficulty;
        return 0;
    }
    virtual bool record(i32 difficulty, i32 character, i32 ordinal, TitlePlayerDataRecord& out) const {
        (void)difficulty;
        (void)character;
        (void)ordinal;
        (void)out;
        return false;
    }
    virtual bool summary(i32 difficulty, i32 character, TitlePlayerDataSummary& out) const {
        (void)difficulty;
        (void)character;
        (void)out;
        return false;
    }
};

class TitlePlayerDataMenu : public TitleFailure {
    TitleState& state;
    TitlePlayerDataPlatform& host;
    i32 difficulty_ = 1;
    i32 character_ = 0;
    i32 page_ = 0;
    i32 frames_ = 0;
    bool opened_ = false;
    std::vector<TitlePlayerDataRecord> records_;
    TitlePlayerDataSummary summary_;
    void sound(i32 id);
    bool reload();

public:
    std::function<bool(i32)> play_sound;
    TitlePlayerDataMenu(TitleState& state_, TitlePlayerDataPlatform& host_) : state(state_), host(host_) {}

    bool open();
    bool update(u32 pressed, u32 repeated);
    bool back();

    bool next_page();
    bool previous_page();
    bool set_page(i32 page);

    i32 difficulty() const { return difficulty_; }
    i32 character() const { return character_; }
    i32 page() const { return page_; }
    i32 phase() const { return state.substate; }
    i32 page_count() const;
    i32 record_count() const { return static_cast<i32>(records_.size()); }
    const std::vector<TitlePlayerDataRecord>& records() const { return records_; }
    std::vector<TitlePlayerDataRecord> visible_records() const;
    const TitlePlayerDataSummary& summary() const { return summary_; }
    bool summary_available() const { return summary_.available; }
    bool opened() const { return opened_; }
};

}
