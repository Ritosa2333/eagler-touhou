#include "TitleResultEntry.hpp"
#include "PauseState.hpp"
#include "TitleMainMenu.hpp"
#include "TitleScene.hpp"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <ctime>

namespace th13 {

namespace {

constexpr u32 kResultColorSelected = 0xffffff00u;
constexpr u32 kResultColorUnselected = 0xff808080u;
constexpr u32 kResultColorRowSelected = 0xffffffffu;
constexpr u32 kResultColorRowUnselected = 0xff404040u;
constexpr i32 kResultNameCount = 91;
constexpr char kTitleResultNameCharacters[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   ";
static_assert(sizeof(kTitleResultNameCharacters) == 92, "result name table must be 91 entries + NUL");

i32 read_i32(const u8* p) { return i32(u32(p[0]) | u32(p[1]) << 8 | u32(p[2]) << 16 | u32(p[3]) << 24); }

std::string format(const char* pattern, ...) {
    char text[256];
    va_list args;
    va_start(args, pattern);
    std::vsnprintf(text, sizeof text, pattern, args);
    va_end(args);
    return std::string(text);
}

constexpr const char* kCharacterNames[4] = {"Reimu  ", "Marisa ", "Sanae  ", "Youmu  "};
constexpr const char* kDifficultyNames[6] = {"Easy   ", "Normal ", "Hard   ", "Lunatic", "Extra  ", "O.D.   "};
constexpr const char* kRouteNames[10] = {"tst", "St1", "St2", "St3", "St4", "St5", "St6", "Ex ", "All", "ExA"};
constexpr const char* kStageNames[10] = {"test   ", "Stage 1", "Stage 2", "Stage 3", "Stage 4",
                                         "Stage 5", "Stage 6", "Extra  ", "Clear  ", "ExClear"};
constexpr const char* kEmptySlotRow = "No.%.2d -------- --/--/-- --:-- ------- ------- --- ---%%";

bool replay_date(const ReplayFile& file, i32& year, i32& month, i32& day, i32& hour, i32& minute) {
    if (file.top_header.size() < 0x14) return false;
    const std::time_t stamp = std::time_t(
        i64(u64(read_i32(file.top_header.data() + 0x0C)) | u64(read_i32(file.top_header.data() + 0x10)) << 32));
    std::tm broken{};
#if defined(_WIN32)
    if (localtime_s(&broken, &stamp) != 0) return false;
#else
    if (localtime_r(&stamp, &broken) == nullptr) return false;
#endif
    year = broken.tm_year % 100;
    month = broken.tm_mon + 1;
    day = broken.tm_mday;
    hour = broken.tm_hour;
    minute = broken.tm_min;
    return true;
}

}

bool TitleResultEntry::fail(const std::string& reason) {
    if (error.empty()) error = reason.empty() ? "Title result screen operation failed" : reason;
    return false;
}

bool TitleResultEntry::create_visual(i32 script, u32& handle) {
    handle = animations.create(title_bank, script, -1, 0);
    return handle != 0 || fail(animations.error.empty() ? "Title result animation unavailable" : animations.error);
}

bool TitleResultEntry::open_name_screen() {
    if (state.handles[89] == 0) {
        if (!create_visual(89, state.handles[89])) return false;
    }
    state.result_background = state.handles[89];
    if (!create_visual(107, state.handles[107])) return false;

    data_ = {};
    data_loaded_ = host.result_screen_data(data_);
    if (data_loaded_) {
        state.result_character = data_.character;
        state.result_difficulty = data_.difficulty;
        state.result_rank = data_.rank;
    } else {
        state.result_rank = 0;
    }
    state.result_character = std::clamp(state.result_character, 0, 3);
    state.result_difficulty = std::clamp(state.result_difficulty, 0, 4);
    if (!create_visual(143 + state.result_character, state.handles[143 + state.result_character])) return false;
    const i32 difficulty_script = state.result_difficulty >= 4 ? 151 : 147 + state.result_difficulty;
    if (!create_visual(difficulty_script, state.handles[difficulty_script])) return false;

    seed_result_name(host.last_result_name(), state.result_name, state.result_name_cursor);
    state.result_slot = 0;
    return true;
}

bool TitleResultEntry::open_slot_screen() {
    if (state.handles[89] == 0) {
        if (!create_visual(89, state.handles[89])) return false;
    }
    state.result_background = state.handles[89];
    if (!create_visual(108, state.handles[108])) return false;
    state.result_slot = 0;
    state.result_name_cursor = 90;
    return true;
}

void TitleResultEntry::retire_name_visuals() {
    for (i32 handle : {107, 143 + state.result_character, 147 + state.result_difficulty, 151}) {
        if (handle < 0 || handle >= i32(state.handles.size())) continue;
        if (state.handles[handle]) {
            animations.retire(state.handles[handle]);
            state.handles[handle] = 0;
        }
    }
}

void TitleResultEntry::retire_slot_visuals() {
    if (state.handles[108]) {
        animations.retire(state.handles[108]);
        state.handles[108] = 0;
    }
    if (state.result_background) {
        animations.retire(state.result_background);
        if (state.handles[89] == state.result_background) state.handles[89] = 0;
        state.result_background = 0;
    }
}

bool TitleResultEntry::return_main() {
    if (!host.queue_title_music(2, -1, "th13_00.wav")) return fail("Result return title music failed");
    state.flags |= 2u;
    state.return_reason = 1;
    state.menu.count = kTitleMainEntries;
    state.menu.wrapping = true;
    state.menu.disabled_count = 0;
    state.menu.select(0);
    state.change_screen(TitleScreen::Main);
    return true;
}

bool TitleResultEntry::edit_name(u32 pressed, u32 repeated, bool save_to_replay) {
    const i32 previous = state.result_name_cursor;
    if ((pressed | repeated) & 0x10)
        state.result_name_cursor = (state.result_name_cursor + kResultNameCount - 13) % kResultNameCount;
    if ((pressed | repeated) & 0x20) state.result_name_cursor = (state.result_name_cursor + 13) % kResultNameCount;
    if ((pressed | repeated) & 0x40)
        state.result_name_cursor =
            (state.result_name_cursor + (state.result_name_cursor % 13 == 0 ? 12 : kResultNameCount - 1)) %
            kResultNameCount;
    if ((pressed | repeated) & 0x80)
        state.result_name_cursor =
            (state.result_name_cursor + (state.result_name_cursor % 13 == 12 ? kResultNameCount - 12 : 1)) %
            kResultNameCount;
    if (previous != state.result_name_cursor && !host.sound(10)) return fail("Result name cursor sound failed");

    if (pressed & 0x80001) {
        if (state.result_name_cursor == 89) {
            if (!state.result_name.empty()) {
                state.result_name.pop_back();
                if (!host.sound(7)) return fail("Result name delete sound failed");
            }
        } else if (state.result_name_cursor == 90) {
            if (save_to_replay) {
                if (!host.sound(0x11)) return fail("Replay save sound failed");
                if (!host.save_replay_slot(state.result_slot + 1, state.result_name)) return fail("Replay save failed");
                if (!host.sound(7)) return fail("Replay slot confirm sound failed");
                state.change_substate(2);
            } else {
                if (state.result_rank >= 0 && !host.record_clear_result_name(state.result_name))
                    return fail("Result name record failed");
                if (!host.sound(7)) return fail("Result name confirm sound failed");
                state.change_substate(3);
            }
            return true;
        } else {
            const char ch = state.result_name_cursor == 88 ? ' ' : kTitleResultNameCharacters[state.result_name_cursor];
            if (state.result_name.size() < 8)
                state.result_name += ch;
            else
                state.result_name[7] = ch;
            if (state.result_name.size() >= 8) state.result_name_cursor = 90;
            if (!host.sound(7)) return fail("Result name input sound failed");
        }
        return true;
    }
    if (pressed & 0x102) {
        if (save_to_replay) {
            if (!state.result_name.empty()) {
                state.result_name.pop_back();
                if (!host.sound(9)) return fail("Result name cancel sound failed");
            } else {
                state.change_substate(2);
            }
        } else if (!state.result_name.empty()) {
            state.result_name.pop_back();
            if (!host.sound(9)) return fail("Result name cancel sound failed");
        }
    }
    return true;
}

bool TitleResultEntry::name_editor(u32 pressed, u32 repeated) {
    if (data_loaded_ && state.result_rank < 0) {
        if (pressed & (0x80001 | 0x102)) {
            if (!host.sound(7)) return fail("Result name confirm sound failed");
            state.change_substate(3);
        }
        return true;
    }
    return edit_name(pressed, repeated, false);
}

bool TitleResultEntry::slot_selector(u32 pressed, u32 repeated) {
    constexpr i32 slots = 25;
    const i32 previous = state.result_slot;
    if ((pressed | repeated) & 0x10) state.result_slot = (state.result_slot + slots - 1) % slots;
    if ((pressed | repeated) & 0x20) state.result_slot = (state.result_slot + 1) % slots;
    if (previous != state.result_slot && !host.sound(10)) return fail("Replay slot cursor sound failed");
    if (pressed & 0x102) {
        if (!host.sound(9)) return fail("Replay slot cancel sound failed");
        state.change_substate(4);
        return true;
    }
    if (pressed & 0x80001) {
        seed_result_name(host.last_result_name(), state.result_name, state.result_name_cursor);
        state.result_slot_name = state.result_name;
        if (!host.sound(7)) return fail("Replay slot confirm sound failed");
        state.change_substate(3);
        return true;
    }
    return true;
}

bool TitleResultEntry::slot_name_editor(u32 pressed, u32 repeated) { return edit_name(pressed, repeated, true); }

bool TitleResultEntry::update(u32 pressed, u32 repeated) {
    if (!error.empty()) return false;
    switch (state.screen) {
    case TitleScreen::ResultName:
        switch (state.substate) {
        case 0:
            if (!host.result_ranking_available()) {
                if (host.result_replay_save_available()) {
                    state.change_screen(TitleScreen::ResultReplaySlot);
                    return true;
                }
                return return_main();
            }
            if (!open_name_screen()) return false;
            state.change_substate(1);
            break;
        case 1:
            if (state.age.current > 6) state.change_substate(2);
            break;
        case 2:
            return name_editor(pressed, repeated);
        case 3:
            if (state.age.current > 5) {
                retire_name_visuals();
                if (host.result_replay_save_available())
                    state.change_screen(TitleScreen::ResultReplaySlot);
                else
                    return return_main();
            }
            break;
        default:
            break;
        }
        break;
    case TitleScreen::ResultReplaySlot:
        switch (state.substate) {
        case 0:
            if (!open_slot_screen()) return false;
            state.change_substate(1);
            break;
        case 1:
            if (state.age.current > 6) state.change_substate(2);
            break;
        case 2:
            return slot_selector(pressed, repeated);
        case 3:
            return slot_name_editor(pressed, repeated);
        case 4:
            if (state.age.current > 5) {
                retire_slot_visuals();
                return return_main();
            }
            break;
        default:
            break;
        }
        break;
    default:
        return fail("Title result screen out of range");
    }
    return true;
}

std::string TitleResultEntry::score_row_text(const TitlePlayerDataScore& record, i32 ordinal,
                                             const std::string& fallback_name) {
    std::string name = record.name;
    if (name.empty()) name = fallback_name;
    if (name.empty()) name = "--------";
    if (!record.available) {
        return format("%2d  --------  -----------  ----/--/-- --:--  Stage -  ---%%", ordinal + 1);
    }
    if (record.timestamp == 0) {
        return format("%2d  %s  %9d%d  ----/--/-- --:--  Stage -  ---%%", ordinal + 1, name.c_str(), record.score,
                      record.continues);
    }
    const std::time_t clock = std::time_t(record.timestamp);
    const std::tm* date = std::localtime(&clock);
    if (!date || record.stage < 0 || record.stage >= 10) {
        return format("%2d  %s  %9d%d  ----/--/-- --:--  Stage -  ---%%", ordinal + 1, name.c_str(), record.score,
                      record.continues);
    }
    return format("%2d  %s  %9d%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %2.1f%%", ordinal + 1, name.c_str(), record.score,
                  record.continues, date->tm_year + 1900, date->tm_mon + 1, date->tm_mday, date->tm_hour, date->tm_min,
                  kStageNames[record.stage], double(record.progress_percent));
}

bool TitleResultEntry::draw_score_rows(HudDrawServices& draw, const TitleResultScreenData& data) {
    HudTextStyle style;
    style.font = 0;
    style.shadow = true;
    const auto put = [&](float x, float y, u32 color, const std::string& text) {
        style.color = color;
        return draw.hud_text({{x, y, 0}, style, text}) || fail("Result screen text service failed");
    };
    for (i32 i = 0; i < i32(data.records.size()); ++i) {
        u32 color;
        if (data.rank < 0) {
            const u32 shade = u32(0xff - i * 0x10);
            color = 0xff0000ffu | (shade << 16) | (shade << 8);
        } else {
            color = i == data.rank ? kResultColorRowSelected : kResultColorRowUnselected;
        }
        auto record = data.records[size_t(i)];
        if (i == data.rank)
            record.name.assign(8, ' ');
        else if (record.available)
            record.name.resize(8, ' ');
        const std::string text = score_row_text(record, i, {});
        if (!put(48, 160 + float(i) * 18, color, text)) return false;
    }
    return true;
}

bool TitleResultEntry::draw_name_editor(HudDrawServices& draw, float name_x, float name_y) {
    HudTextStyle style;
    style.font = 0;
    style.shadow = true;
    const auto put = [&](float x, float y, u32 color, const std::string& text) {
        style.color = color;
        return draw.hud_text({{x, y, 0}, style, text}) || fail("Result name text service failed");
    };
    if (!put(name_x, name_y, 0xffffffffu, state.result_name)) return false;
    float underscore_x = name_x + float(state.result_name.size()) * 9.0f;
    if (state.result_name.size() >= 8) underscore_x -= 9.0f;
    if (!put(underscore_x, name_y, kResultColorSelected, "_")) return false;

    float x = 212, y = 360;
    for (i32 i = 0; i < kResultNameCount; ++i) {
        std::string glyph;
        if (i == 88)
            glyph.assign(1, char(0x81));
        else if (i == 89)
            glyph.assign(1, char(0x7f));
        else if (i == 90)
            glyph.assign(1, char(0x80));
        else
            glyph.assign(1, kTitleResultNameCharacters[i]);
        const u32 color = i == state.result_name_cursor ? kResultColorSelected : kResultColorUnselected;
        if (!put(x, y, color, glyph)) return false;
        if (i % 13 == 12) {
            x = 212;
            y += 16;
        } else {
            x += 18;
        }
    }
    return true;
}

std::string replay_save_slot_row(i32 slot, const ReplayFile* file, bool compact) {
    const char* empty = compact ? "No.%.2d -------- --/--/-- ------- -- St-" : kEmptySlotRow;
    i32 year = 0, month = 0, day = 0, hour = 0, minute = 0;
    if (!file || file->top_header.size() < 0x6C || !replay_date(*file, year, month, day, hour, minute))
        return format(empty, slot);
    const u8* top = file->top_header.data();
    const i32 character = std::clamp(read_i32(top + 0x5C) + read_i32(top + 0x60), 0, 3);
    const i32 difficulty = std::clamp(read_i32(top + 0x64), 0, 5);
    const i32 route = std::clamp(read_i32(top + 0x68), 0, 9);
    if (compact) {
        if ((top[0xA] & 2) && file->top_header.size() >= 0x74)
            return format("No.%.2d %s %.2d/%.2d/%.2d %s Sp %3d", slot, file->player_name.c_str(), year, month, day,
                          kCharacterNames[character], wrapping_add(read_i32(top + 0x70), 1));
        return format("No.%.2d %s %.2d/%.2d/%.2d %s %s %s", slot, file->player_name.c_str(), year, month, day,
                      kCharacterNames[character], kDifficultyNames[difficulty], kRouteNames[route]);
    }
    return format("No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%", slot, file->player_name.c_str(), year, month,
                  day, hour, minute, kCharacterNames[character], kDifficultyNames[difficulty], kRouteNames[route],
                  double(file->slow_rate));
}

std::string TitleResultEntry::slot_row_text(i32 index) const {
    return replay_save_slot_row(index + 1, host.replay_entry(index), false);
}

bool TitleResultEntry::draw_slot_list(HudDrawServices& draw) {
    HudTextStyle style;
    style.font = 0;
    style.shadow = true;
    const auto put = [&](float x, float y, u32 color, const std::string& text) {
        style.color = color;
        return draw.hud_text({{x, y, 0}, style, text}) || fail("Replay slot text service failed");
    };
    for (i32 i = 0; i < 25; ++i) {
        const u32 color = i == state.result_slot ? kResultColorSelected : kResultColorUnselected;
        if (!put(58, 80 + float(i) * 15, color, slot_row_text(i))) return false;
    }
    return true;
}

bool TitleResultEntry::draw_slot_selected(HudDrawServices& draw) {
    HudTextStyle style;
    style.font = 0;
    style.shadow = true;
    style.color = kResultColorSelected;
    std::string text = slot_row_text(state.result_slot);
    std::string slot_name = state.result_slot_name;
    if (slot_name.size() < 8) slot_name.resize(8, ' ');
    if (data_loaded_ && data_.current.available) {
        const std::time_t stamp = std::time_t(data_.current.timestamp);
        const std::tm* date = std::localtime(&stamp);
        if (!date) return fail("Recording timestamp unavailable");
        text = format("No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%", state.result_slot + 1, slot_name.c_str(),
                      date->tm_year % 100, date->tm_mon + 1, date->tm_mday, date->tm_hour, date->tm_min,
                      kCharacterNames[data_.character], kDifficultyNames[data_.difficulty], "All",
                      double(data_.current.progress_percent));
    }
    const float age = std::min(10.f, state.age.fractional);
    const float y = 240.f + (80.f + float(state.result_slot) * 15.f - 240.f) * (10.f - age) / 10.f;
    return draw.hud_text({{58, y, 0}, style, text}) || fail("Replay slot text service failed");
}

bool TitleResultEntry::draw_texts(HudDrawServices& draw) {
    if (!error.empty()) return false;
    if (state.screen == TitleScreen::ResultName) {
        if (state.substate != 2) return true;
        TitleResultScreenData data = data_;
        if (!data_loaded_) {
            data = {};
            data.rank = state.result_rank;
        }
        if (!draw_score_rows(draw, data)) return false;
        if (!data_loaded_ || state.result_rank >= 0) {
            const float name_y = 160 + float(std::max(state.result_rank, 0)) * 18;
            if (!draw_name_editor(draw, 84, name_y)) return false;
        }
        return true;
    }
    if (state.screen == TitleScreen::ResultReplaySlot) {
        if (state.substate == 2) return draw_slot_list(draw);
        if (state.substate == 3) {
            if (!draw_slot_selected(draw)) return false;
            return state.age.current <= 9 || draw_name_editor(draw, 112, 240);
        }
        return true;
    }
    return true;
}

}
