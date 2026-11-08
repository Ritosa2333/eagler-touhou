#include "TitleSpellPracticeMenu.hpp"

#include <algorithm>

namespace th13 {
namespace {
i32 clamp_i32(i32 value, i32 low, i32 high) {
    return value < low ? low : (value > high ? high : value);
}
}  // namespace

TitleSpellPracticeMenu::TitleSpellPracticeMenu(TitleSpellPracticePlatform& platform,
                                               i32 initial_character)
    : platform_(platform) {
    const i32 characters = clamp_i32(platform_.character_count(), 0,
                                     kSpellPracticeCharacters);
    character_ = characters > 0 ? clamp_i32(initial_character, 0, characters - 1) : 0;
    rebuild_stage();
    if (stage_rows_.empty()) error = "spell practice catalog unavailable";
}

void TitleSpellPracticeMenu::rebuild_stage() {
    stage_rows_.clear();
    const i32 count = clamp_i32(platform_.stage_count(), 0, kSpellPracticeStages);
    stage_rows_.reserve(static_cast<size_t>(count));
    for (i32 index = 0; index < count; ++index) {
        SpellPracticeStageRow row;
        row.index = index;
        row.stage = index + 1;
        row.available = platform_.stage_available(row.stage);
        row.name = platform_.stage_name(row.stage);
        stage_rows_.push_back(std::move(row));
    }
    cursor_ = clamp_i32(cursor_, 0, stage_rows_.empty() ? 0 : i32(stage_rows_.size()) - 1);
}

void TitleSpellPracticeMenu::rebuild_spells() {
    spell_rows_.clear();
    i32 count = 0;
    if (selected_stage_ >= 1 && selected_stage_ <= kSpellPracticeStages) {
        count = clamp_i32(platform_.spell_count(selected_stage_), 0,
                          kSpellPracticeSlots[selected_stage_ - 1]);
    }
    spell_rows_.reserve(static_cast<size_t>(count));
    for (i32 slot = 0; slot < count; ++slot) {
        SpellPracticeSpellRow row;
        row.slot = slot;
        row.unlocked = platform_.spell_unlocked(selected_stage_, slot);
        if (row.unlocked) row.name = platform_.spell_name(selected_stage_, slot);
        spell_rows_.push_back(std::move(row));
    }
    cursor_ = clamp_i32(cursor_, 0, spell_rows_.empty() ? 0 : i32(spell_rows_.size()) - 1);
}

void TitleSpellPracticeMenu::rebuild_difficulties() {
    difficulty_rows_.clear();
    difficulty_rows_.reserve(kSpellPracticeDifficulties);
    for (i32 difficulty = 0; difficulty < kSpellPracticeDifficulties; ++difficulty) {
        SpellPracticeDifficultyRow row;
        row.difficulty = difficulty;
        row.spell_id = platform_.spell_id(selected_stage_, selected_slot_, difficulty);
        row.rank = row.spell_id >= 0
                       ? platform_.spell_rank(selected_stage_, selected_slot_, difficulty)
                       : -1;
        row.available = row.spell_id >= 0 && row.rank >= 0 &&
                        platform_.spell_variant_available(selected_stage_, selected_slot_, difficulty);
        if (row.available)
            row.name = platform_.spell_variant_name(selected_stage_, selected_slot_, difficulty);
        difficulty_rows_.push_back(std::move(row));
    }
    cursor_ = clamp_i32(cursor_, 0, kSpellPracticeDifficulties - 1);
    if (!difficulty_rows_[size_t(cursor_)].available) {
        for (i32 row = 0; row < kSpellPracticeDifficulties; ++row)
            if (difficulty_rows_[size_t(row)].available) { cursor_ = row; break; }
    }
}

i32 TitleSpellPracticeMenu::cursor_count() const noexcept {
    if (level_ == SpellPracticeLevel::Stage) return i32(stage_rows_.size());
    if (level_ == SpellPracticeLevel::Spell) return i32(spell_rows_.size());
    return i32(difficulty_rows_.size());
}

void TitleSpellPracticeMenu::move_cursor(i32 delta) {
    const i32 count = cursor_count();
    if (count <= 0) {
        cursor_ = 0;
        return;
    }
    if (level_ != SpellPracticeLevel::Difficulty) {
        cursor_ = (cursor_ + delta + count) % count;
        return;
    }
    for (i32 step = 0; step < count; ++step) {
        const i32 candidate = (cursor_ + delta * (step + 1) + count * (step + 1)) % count;
        if (difficulty_rows_[size_t(candidate)].available) { cursor_ = candidate; return; }
    }
}

bool TitleSpellPracticeMenu::character_ready() const {
    const i32 count = clamp_i32(platform_.character_count(), 0, kSpellPracticeCharacters);
    return count > 0 && character_ >= 0 && character_ < count;
}

bool TitleSpellPracticeMenu::cycle_character(i32 delta) {
    const i32 count = clamp_i32(platform_.character_count(), 0, kSpellPracticeCharacters);
    if (count <= 0) return false;
    const i32 next = (character_ + delta + count) % count;
    if (next == character_) return false;
    character_ = next;
    if (level_ == SpellPracticeLevel::Spell) {
        rebuild_spells();
    } else if (level_ == SpellPracticeLevel::Difficulty) {
        const i32 difficulty_cursor = cursor_;
        rebuild_spells();
        cursor_ = difficulty_cursor;
        rebuild_difficulties();
    }
    return true;
}

bool TitleSpellPracticeMenu::update(const SpellPracticeInput& input) {
    blocked.clear();
    if (input.up) move_cursor(-1);
    if (input.down) move_cursor(1);
    // 004560a0 also calls 004564c0 on Stage Select.
    if (input.left) cycle_character(-1);
    if (input.right) cycle_character(1);
    if (input.confirm) return confirm();
    if (input.cancel) return cancel();
    return false;
}

bool TitleSpellPracticeMenu::confirm() {
    if (level_ == SpellPracticeLevel::Stage) {
        if (stage_rows_.empty() || cursor_ < 0 || cursor_ >= i32(stage_rows_.size())) {
            blocked = "spell practice catalog unavailable";
            return false;
        }
        const SpellPracticeStageRow& row = stage_rows_[size_t(cursor_)];
        if (!row.available) {
            blocked = "stage unavailable";
            return false;
        }
        selected_stage_ = row.stage;
        selected_slot_ = -1;
        cursor_ = 0;
        rebuild_spells();
        level_ = SpellPracticeLevel::Spell;
        if (spell_rows_.empty()) {
            blocked = "no spell cards";
            return false;
        }
        if (!character_ready()) {
            blocked = "spell practice character data unavailable";
        }
        return false;
    }

    if (level_ == SpellPracticeLevel::Spell) {
        if (spell_rows_.empty() || cursor_ < 0 || cursor_ >= i32(spell_rows_.size())) {
            blocked = "spell cards unavailable";
            return false;
        }
        const SpellPracticeSpellRow& row = spell_rows_[size_t(cursor_)];
        if (!row.unlocked) {  // 0x456af0：锁定卡仍可浏览，不可确认。
            blocked = "spell card locked";
            return false;
        }
        if (!character_ready()) {
            blocked = "spell practice character data unavailable";
            return false;
        }
        selected_slot_ = row.slot;
        cursor_ = 0;
        rebuild_difficulties();
        level_ = SpellPracticeLevel::Difficulty;
        return false;
    }

    if (difficulty_rows_.empty() || cursor_ < 0 ||
        cursor_ >= i32(difficulty_rows_.size())) {
        blocked = "spell variants unavailable";
        return false;
    }
    const SpellPracticeDifficultyRow& row = difficulty_rows_[size_t(cursor_)];
    if (!row.available || row.spell_id < 0 || row.rank < 0) {
        blocked = "spell variant unavailable";
        return false;
    }
    if (!character_ready()) {
        blocked = "spell practice character data unavailable";
        return false;
    }
    emit(SpellPracticeAction::Launch);
    command_.spell_slot = selected_slot_;
    command_.difficulty = row.difficulty;
    command_.spell_id = row.spell_id;
    command_.rank = row.rank;
    return true;
}

bool TitleSpellPracticeMenu::cancel() {
    if (level_ == SpellPracticeLevel::Difficulty) {
        level_ = SpellPracticeLevel::Spell;
        cursor_ = clamp_i32(selected_slot_, 0,
                            spell_rows_.empty() ? 0 : i32(spell_rows_.size()) - 1);
        return false;
    }
    if (level_ == SpellPracticeLevel::Spell) {
        level_ = SpellPracticeLevel::Stage;
        cursor_ = clamp_i32(selected_stage_ - 1, 0,
                            stage_rows_.empty() ? 0 : i32(stage_rows_.size()) - 1);
        return false;
    }
    emit(SpellPracticeAction::Cancel);
    return true;
}

void TitleSpellPracticeMenu::emit(SpellPracticeAction action) {
    command_ = {};
    command_.action = action;
    command_.character = character_;
    if (action == SpellPracticeAction::Launch) {
        // 0x4576c0：DAT_004be81c = stage_index + 1；stage 已是 1 基。
        command_.stage = selected_stage_;
    }
}

}  // namespace th13
