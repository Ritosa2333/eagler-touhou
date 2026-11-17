#include "TitlePlayerDataMenu.hpp"

#include <algorithm>

namespace th13 {
namespace {

// 六字节输入位（与其他标题屏一致）：↑↓←→ / X(Esc) / Z。
constexpr u32 kInputUp = 0x10;
constexpr u32 kInputDown = 0x20;
constexpr u32 kInputLeft = 0x40;
constexpr u32 kInputRight = 0x80;
constexpr u32 kInputCancel = 0x102;
constexpr u32 kInputConfirm = 0x80001;

}  // namespace

bool TitlePlayerDataMenu::fail(const std::string& reason) {
    if (error.empty()) error = reason;
    return false;
}

void TitlePlayerDataMenu::sound(i32 id) {
    if (play_sound) play_sound(id);
}

bool TitlePlayerDataMenu::reload() {
    records_.clear();
    summary_ = {};
    if (host.summary(difficulty_, character_, summary_)) summary_.available = true;
    const i32 count = host.record_count(difficulty_);
    if (count < 0 || count > kPlayerDataRecordSlots) {
        return fail("Title player data record count out of range");
    }
    records_.reserve(static_cast<std::size_t>(count));
    for (i32 ordinal = 0; ordinal < count; ++ordinal) {
        TitlePlayerDataRecord record;
        if (!host.record(difficulty_, character_, ordinal, record)) {
            return fail("Title player data record unavailable");
        }
        if (record.slot < 0 || record.slot >= kPlayerDataRecordSlots) {
            return fail("Title player data slot out of range");
        }
        // Older hosts expose aggregate ordinary attempts through the legacy
        // score DTO field. Preserve that established host contract.
        if (record.score > 0) record.recorded = true;
        if (!record.recorded) record.name.clear();
        records_.push_back(std::move(record));
    }
    const i32 pages = page_count();
    if (page_ < 0) page_ = 0;
    if (page_ >= pages) page_ = pages - 1;
    return true;
}

bool TitlePlayerDataMenu::open() {
    if (opened_) return true;
    if (!host.open_player_data()) {
        return fail("Title player data unavailable");
    }
    opened_ = true;
    if (host.difficulty_count() != kPlayerDataDifficulties ||
        host.character_count() != kPlayerDataCharacters) {
        host.close_player_data();
        opened_ = false;
        return fail("Title player data shape unavailable");
    }
    difficulty_ = 1; // 004531a5..004531b7: FUN_00415030(1), initial Normal
    character_ = 0;
    page_ = 0;
    frames_ = 0;
    if (!reload()) {
        host.close_player_data();
        opened_ = false;
        return false;
    }
    state.change_substate(1);
    return true;
}

bool TitlePlayerDataMenu::update(u32 pressed, u32 repeated) {
    const u32 navigation = pressed | repeated;
    switch (state.substate) {
    case 0:
        return open();
    case 1:
        ++frames_;
        if (frames_ > 6) state.change_substate(2);
        break;
    case 2: {
        if (pressed & kInputCancel) return back();
        bool changed = false;
        if (navigation & kInputUp) {
            difficulty_ = (difficulty_ + kPlayerDataDifficulties - 1) % kPlayerDataDifficulties;
            if (page_ > 0) page_ = 1; // 00453449: remain in the card-page category
            changed = true;
        }
        if (navigation & kInputDown) {
            difficulty_ = (difficulty_ + 1) % kPlayerDataDifficulties;
            if (page_ > 0) page_ = 1;
            changed = true;
        }
        if (navigation & kInputLeft) {
            character_ = (character_ + kPlayerDataCharacters - 1) % kPlayerDataCharacters;
            changed = true;
        }
        if (navigation & kInputRight) {
            character_ = (character_ + 1) % kPlayerDataCharacters;
            changed = true;
        }
        if (changed) {
            if (!reload()) return false;
            sound(10);  // 0x453170 选择变化音（FUN_00460bc0(...,10)）
        }
        if (pressed & kInputConfirm) {
            page_ = (page_ + 1) % page_count();
            sound(7);  // 00453170: confirm advances the page cursor.
        }
        break;
    }
    case 3:
        ++frames_;
        if (frames_ > 5) {  // 原版 case3：5 < age
            host.close_player_data();
            opened_ = false;
            if (state.menu.depth > 0) state.menu.pop();
            state.change_screen(TitleScreen::Main);
        }
        break;
    default:
        break;
    }
    return true;
}

bool TitlePlayerDataMenu::back() {
    if (state.substate != 2) return false;
    sound(9);  // case2 X/Esc：FUN_00460bc0(...,9)
    state.change_substate(3);
    frames_ = 0;
    return true;
}

bool TitlePlayerDataMenu::next_page() {
    if (state.substate != 2 || page_ + 1 >= page_count()) return false;
    ++page_;
    return true;
}

bool TitlePlayerDataMenu::previous_page() {
    if (state.substate != 2 || page_ <= 0) return false;
    --page_;
    return true;
}

bool TitlePlayerDataMenu::set_page(i32 page) {
    if (state.substate != 2) return false;
    const i32 clamped = std::clamp(page, 0, page_count() - 1);
    if (clamped == page_) return false;
    page_ = clamped;
    return true;
}

i32 TitlePlayerDataMenu::page_count() const {
    // 00453170: count is exclusive, page0 is the ordinary ranking.
    const i32 count = static_cast<i32>(records_.size());
    return 1 + (count + kPlayerDataPageSize - 1) / kPlayerDataPageSize;
}

std::vector<TitlePlayerDataRecord> TitlePlayerDataMenu::visible_records() const {
    std::vector<TitlePlayerDataRecord> visible;
    if (page_ == 0) return visible;
    const i32 begin = (page_ - 1) * kPlayerDataPageSize;
    const i32 end = std::min(begin + kPlayerDataPageSize,
                             static_cast<i32>(records_.size()));
    for (i32 i = begin; i < end; ++i) visible.push_back(records_[static_cast<std::size_t>(i)]);
    return visible;
}

}  // namespace th13
