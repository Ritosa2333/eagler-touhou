#pragma once

#include "Types.hpp"

#include <string>
#include <vector>

namespace th13 {

inline constexpr i32 kSpellPracticeStages = 7;
inline constexpr i32 kSpellPracticeSlots[kSpellPracticeStages] = {4, 4, 4, 4, 5, 7, 13};
inline constexpr i32 kSpellPracticeDifficulties = 5;
inline constexpr i32 kSpellPracticeCharacters = 4;

struct SpellPracticeRecord {
    i32 captures[2] = {}, attempts[2] = {};
    i32 best_score = 0;
    bool encountered = false;
};

struct TitleSpellPracticePlatform {
    virtual ~TitleSpellPracticePlatform() = default;
    virtual bool spell_record(i32, i32, SpellPracticeRecord&) const { return false; }
    virtual i32 stage_count() const { return 0; }
    virtual bool stage_available(i32) const { return false; }
    virtual std::string stage_name(i32) const { return {}; }
    virtual i32 spell_count(i32) const { return 0; }
    virtual bool spell_unlocked(i32, i32) const { return false; }
    virtual std::string spell_name(i32, i32) const { return {}; }
    virtual i32 spell_id(i32, i32, i32) const { return -1; }
    virtual i32 spell_rank(i32, i32, i32) const { return -1; }
    virtual bool spell_variant_available(i32 stage, i32 slot, i32) const { return spell_unlocked(stage, slot); }
    virtual std::string spell_variant_name(i32, i32, i32) const { return {}; }
    virtual i32 character_count() const { return 0; }
    virtual std::string character_name(i32) const { return {}; }
};

struct SpellPracticeStageRow {
    i32 index = -1;
    i32 stage = -1;
    bool available = false;
    std::string name;
};

struct SpellPracticeSpellRow {
    i32 slot = -1;
    bool unlocked = false;
    std::string name;
};

struct SpellPracticeDifficultyRow {
    i32 difficulty = -1;
    i32 spell_id = -1;
    i32 rank = -1;
    bool available = false;
    std::string name;
};

enum class SpellPracticeLevel : i32 { Stage = 0, Spell = 1, Difficulty = 2 };
enum class SpellPracticeAction : i32 { None = 0, Cancel = 1, Launch = 2 };

struct SpellPracticeInput {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool confirm = false;
    bool cancel = false;
};

struct SpellPracticeCommand {
    SpellPracticeAction action = SpellPracticeAction::None;
    i32 stage = -1;
    i32 spell_slot = -1;
    i32 difficulty = -1;
    i32 spell_id = -1;
    i32 rank = -1;
    i32 character = -1;
};

class TitleSpellPracticeMenu {
public:
    explicit TitleSpellPracticeMenu(TitleSpellPracticePlatform& platform, i32 initial_character = 0);

    bool update(const SpellPracticeInput& input);

    SpellPracticeLevel level() const noexcept { return level_; }
    i32 cursor() const noexcept { return cursor_; }
    i32 cursor_count() const noexcept;
    i32 character() const noexcept { return character_; }
    i32 selected_stage() const noexcept { return selected_stage_; }
    i32 selected_slot() const noexcept { return selected_slot_; }

    const std::vector<SpellPracticeStageRow>& stage_rows() const noexcept { return stage_rows_; }
    const std::vector<SpellPracticeSpellRow>& spell_rows() const noexcept { return spell_rows_; }
    const std::vector<SpellPracticeDifficultyRow>& difficulty_rows() const noexcept { return difficulty_rows_; }

    const SpellPracticeCommand& command() const noexcept { return command_; }
    void clear_command() noexcept { command_ = {}; }

    bool restore_selection(i32 stage, i32 slot, i32 difficulty);

    std::string error;
    std::string blocked;

private:
    TitleSpellPracticePlatform& platform_;
    SpellPracticeLevel level_ = SpellPracticeLevel::Stage;
    i32 cursor_ = 0;
    i32 character_ = 0;
    i32 selected_stage_ = -1;
    i32 selected_slot_ = -1;
    std::vector<SpellPracticeStageRow> stage_rows_;
    std::vector<SpellPracticeSpellRow> spell_rows_;
    std::vector<SpellPracticeDifficultyRow> difficulty_rows_;
    SpellPracticeCommand command_;

    void rebuild_stage();
    void rebuild_spells();
    void rebuild_difficulties();
    void move_cursor(i32 delta);
    bool cycle_character(i32 delta);
    bool character_ready() const;
    bool confirm();
    bool cancel();
    void emit(SpellPracticeAction action);
};

}
