#include "TitleReplayMenu.hpp"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>

namespace th13 {

namespace {
constexpr i32 kRowsPerPage = 25;
constexpr i32 kStageRows = 7;

constexpr const char* kCharacterNames[4] = {"Reimu  ", "Marisa ", "Sanae  ", "Youmu  "};
constexpr const char* kDifficultyNames[6] = {"Easy   ", "Normal ", "Hard   ", "Lunatic", "Extra  ", "O.D.   "};
constexpr const char* kStageNames[kStageRows + 1] = {nullptr,   "Stage 1", "Stage 2", "Stage 3",
                                                     "Stage 4", "Stage 5", "Stage 6", "Extra  "};
constexpr const char* kRouteNames[10] = {"tst", "St1", "St2", "St3", "St4", "St5", "St6", "Ex ", "All", "ExA"};

constexpr u32 kColorSelected = 0xffffff00u;
constexpr u32 kColorUnselected = 0xff808080u;
constexpr u32 kColorHeader = 0xffffffffu;

u32 read_u32(const u8* p) { return u32(p[0]) | u32(p[1]) << 8 | u32(p[2]) << 16 | u32(p[3]) << 24; }
i32 read_i32(const u8* p) { return i32(read_u32(p)); }
i64 read_i64(const u8* p) { return i64(u64(read_u32(p)) | u64(read_u32(p + 4)) << 32); }

bool save_date(const ReplayFile& file, i32& year2, i32& month, i32& day, i32& hour, i32& minute) {
    if (file.top_header.size() < 0x14) return false;
    const std::time_t stamp = std::time_t(read_i64(file.top_header.data() + 0x0C));
    std::tm broken{};
#if defined(_WIN32)
    if (localtime_s(&broken, &stamp) != 0) return false;
#else
    if (localtime_r(&stamp, &broken) == nullptr) return false;
#endif
    year2 = broken.tm_year % 100;
    month = broken.tm_mon + 1;
    day = broken.tm_mday;
    hour = broken.tm_hour;
    minute = broken.tm_min;
    return true;
}

std::string format(const char* pattern, ...) {
    char text[256];
    va_list args;
    va_start(args, pattern);
    std::vsnprintf(text, sizeof text, pattern, args);
    va_end(args);
    return std::string(text);
}

std::string file_identifier(const std::string& name) {
    if (name.size() >= 11) return name.substr(7, 4);
    return name;
}

std::string number_label(i32 index) { return format("%.2d", index + 1); }

constexpr const char* kEmptyListRow = " -------- --/--/-- --:-- ------- ------- --- ---%";

const std::string kEmptyListSuffix = kEmptyListRow;
}

bool TitleReplayMenu::sound(i32 id) {
    return play_sound ? play_sound(id) || fail("Replay sound request failed")
                      : fail("Replay sound service unavailable");
}

bool TitleReplayMenu::present(u32 handle) const { return handle && animations.registry.find(handle); }

bool TitleReplayMenu::start_launch_prayer() {
    if (launch_prayer) return true;
    launch_prayer = animations.create(resources.ascii, 17, -1, 0, {480.f, 392.f, 0.f});
    return launch_prayer != 0 || fail(animations.error);
}

void TitleReplayMenu::stop_launch_prayer() {
    if (!launch_prayer) return;
    animations.retire(launch_prayer);
    launch_prayer = 0;
}

TitleReplayMenu::~TitleReplayMenu() { stop_launch_prayer(); }

bool TitleReplayMenu::spell_practice(const ReplayFile& file) {
    return file.top_header.size() > 0x0A && (file.top_header[0x0A] & 2) != 0;
}

const ReplayFile* TitleReplayMenu::selected_file() const { return replay_host.replay_entry(selected_index); }

const ReplayStage* TitleReplayMenu::stage_block(const ReplayFile& file, i32 number) const {
    for (const ReplayStage& stage : file.stages)
        if (stage.number == u16(number)) return &stage;
    return nullptr;
}

i32 TitleReplayMenu::stage_chain_index(const ReplayFile& file) const {
    if (spell_practice(file)) return 0;
    const i32 number = selected_stage_row + 1;
    for (u32 i = 0; i < file.stages.size(); i++)
        if (file.stages[i].number == u16(number)) return i32(i);
    return -1;
}

bool TitleReplayMenu::open() {
    const i32 slots = replay_host.replay_count();
    if (slots <= 0) return fail("Replay catalog unavailable");
    pages = std::max(1, slots / kRowsPerPage);
    if (state.replay_index < 0 || state.replay_index >= slots) state.replay_index = 0;
    state.menu.wrapping = true;
    state.menu.count = kRowsPerPage;
    state.menu.select(state.replay_index % kRowsPerPage);
    page.wrapping = true;
    page.count = pages;
    page.select(state.replay_index / kRowsPerPage);
    if (!state.replay_ascii_root) {
        state.replay_ascii_root = animations.create(resources.ascii, resources.replay_particle_root, -1, 0);
        if (!state.replay_ascii_root) return fail(animations.error);
    }
    if (!present(state.handles[resources.replay_badge])) {
        state.handles[resources.replay_badge] = animations.create(resources.title, resources.replay_badge, -1, 0);
        if (!state.handles[resources.replay_badge]) return fail(animations.error);
    }
    if (!present(state.handles[resources.background_root])) {
        state.handles[resources.background_root] = animations.create(resources.title, resources.background_root, -1, 0);
        if (!state.handles[resources.background_root]) return fail(animations.error);
        if (!animations.interrupt(state.handles[resources.background_root], 3, false)) return fail(animations.error);
    }
    return true;
}

void TitleReplayMenu::consume_import_hint() {
    if (state.replay_import_hint < 0) return;
    const i32 slot = state.replay_import_hint;
    state.replay_import_hint = -1;
    if (slot < 0 || slot >= replay_host.replay_count()) return;
    state.replay_index = slot;
    page.select(slot / kRowsPerPage);
    state.menu.select(slot % kRowsPerPage);
}

bool TitleReplayMenu::confirm_file() {
    const i32 slots = replay_host.replay_count();
    selected_index = page.cursor * kRowsPerPage + state.menu.cursor;
    state.replay_index = selected_index;
    if (selected_index == slots - 1) {
        if (!replay_host.request_replay_file()) return true;
        return sound(7);
    }
    const ReplayFile* file = replay_host.replay_entry(selected_index);
    if (!file) return true;
    if (!sound(7)) return false;
    state.menu.push();
    state.menu.wrapping = true;
    state.menu.count = kStageRows;
    for (i32 row = 0; row < kStageRows; row++) {
        if (!stage_block(*file, row + 1) && !state.menu.disable(row)) return fail(state.menu.error);
    }
    state.menu.select(0);
    if (spell_practice(*file)) {
        selected_stage_row = 0;
        if (!sound(50)) return false;
        state.change_substate(3);
        return true;
    }
    state.change_substate(4);
    return true;
}

bool TitleReplayMenu::update(u32 pressed, u32 repeated) {
    switch (state.substate) {
    case 0:
        if (!open()) return false;
        state.change_substate(1);
        break;
    case 1:
        if (state.age.current > 6) state.change_substate(2);
        break;
    case 2: {
        consume_import_hint();
        state.menu.previous = state.menu.cursor;
        const i32 page_previous = page.cursor;
        if ((pressed | repeated) & 0x10) state.menu.move(-1);
        if ((pressed | repeated) & 0x20) state.menu.move(1);
        if (!state.menu.error.empty()) return fail(state.menu.error);
        if ((pressed | repeated) & 0x40) page.move(-1);
        if ((pressed | repeated) & 0x80) page.move(1);
        if (!page.error.empty()) return fail(page.error);
        if (state.menu.previous != state.menu.cursor || page_previous != page.cursor) {
            if (!sound(10)) return false;
        }
        if (pressed & 0x102) {
            if (!sound(9)) return false;
            state.change_substate(5);
            return true;
        }
        if (pressed & 0x80001) {
            if (!confirm_file()) return false;
        }
        break;
    }
    case 4:
        if (state.age.current > 14) {
            state.menu.previous = state.menu.cursor;
            if ((pressed | repeated) & 0x10) state.menu.move(-1);
            if ((pressed | repeated) & 0x20) state.menu.move(1);
            if (!state.menu.error.empty()) return fail(state.menu.error);
            if (state.menu.previous != state.menu.cursor) {
                if (!sound(10)) return false;
            }
            if (pressed & 0x102) {
                if (!sound(9)) return false;
                state.menu.pop();
                state.menu.count = kRowsPerPage;
                state.change_substate(2);
                return true;
            }
            if (pressed & 0x80001) {
                selected_stage_row = state.menu.cursor;
                if (!sound(50)) return false;
                state.change_substate(3);
            }
        }
        break;
    case 3:
        if (state.age.current == 2 && !launch_prayer) {
            if (!start_launch_prayer()) return false;
        }
        if (!launch_queued && state.age.current > 31) {
            const ReplayFile* file = selected_file();
            if (!file) return fail("Replay launch: selected entry unavailable");
            const i32 chain = stage_chain_index(*file);
            if (chain < 0)
                return fail("Replay launch: stage block missing for row " + std::to_string(selected_stage_row + 1));
            state.replay_index = selected_index;
            if (!replay_host.start_replay(selected_index, chain))
                return fail("Replay launch: host rejected start request");
            launch_queued = true;
            stop_launch_prayer();
        }
        break;
    case 5:
        if (state.age.current > 5) {
            if (state.handles[resources.replay_badge]) {
                animations.retire(state.handles[resources.replay_badge]);
                state.handles[resources.replay_badge] = 0;
            }
            if (state.replay_ascii_root) {
                animations.retire(state.replay_ascii_root);
                state.replay_ascii_root = 0;
            }
            state.change_screen(TitleScreen::Main);
            state.menu.pop();
        }
        break;
    default:
        break;
    }
    return true;
}

bool TitleReplayMenu::file_row_text(i32 index, std::string& out) {
    const i32 slots = replay_host.replay_count();
    const i32 page_index = index / kRowsPerPage;
    const ReplayFile* file = replay_host.replay_entry(index);
    if (index == slots - 1) {
        const std::string reason = replay_host.replay_import_error();
        out = reason.empty() ? "Import " + kEmptyListSuffix : "Import  " + reason;
        return true;
    }
    if (!file) {
        out = page_index == 0 ? "No." + number_label(index) + kEmptyListSuffix : "User " + kEmptyListSuffix;
        return true;
    }
    i32 year2 = 0, month = 0, day = 0, hour = 0, minute = 0;
    const bool dated = save_date(*file, year2, month, day, hour, minute);
    if (!dated) return fail("Replay save date unavailable: " + replay_host.replay_filename(index));
    const u8* top = file->top_header.data();
    const i32 character = std::clamp(read_i32(top + 0x5C) + read_i32(top + 0x60), 0, 3);
    const i32 difficulty = std::clamp(read_i32(top + 0x64), 0, 5);
    const i32 route = std::clamp(read_i32(top + 0x68), 0, 9);
    const double slow = double(file->slow_rate);
    if (page_index == 0) {
        if (spell_practice(*file)) {
            const i32 practice = read_i32(top + 0x70) + 1;
            out = format("No.%s %s %.2d/%.2d/%.2d %.2d:%.2d %s SpellPr %3d %2.1f%%", number_label(index).c_str(),
                         file->player_name.c_str(), year2, month, day, hour, minute, kCharacterNames[character],
                         practice, slow);
        } else {
            out = format("No.%s %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%", number_label(index).c_str(),
                         file->player_name.c_str(), year2, month, day, hour, minute, kCharacterNames[character],
                         kDifficultyNames[difficulty], kRouteNames[route], slow);
        }
    } else {
        out = format("%s  %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%",
                     file_identifier(replay_host.replay_filename(index)).c_str(), file->player_name.c_str(), year2,
                     month, day, hour, minute, kCharacterNames[character], kDifficultyNames[difficulty],
                     kRouteNames[route], slow);
    }
    return true;
}

bool TitleReplayMenu::stage_row_text(const ReplayFile& file, i32 row, std::string& out) {
    const i32 number = row + 1;
    if (number < 1 || number > kStageRows) return fail("Replay stage row out of range");
    if (!stage_block(file, number)) {
        out = format("%s  ---------", kStageNames[number]);
        return true;
    }
    const ReplayStage* next = number < 6 ? stage_block(file, number + 1) : nullptr;
    const u8* top = file.top_header.data();
    i32 score = 0, extra = 0;
    if (next) {
        score = next->score;
        extra = next->header.size() > 0x28 ? read_i32(next->header.data() + 0x24) : 0;
    } else {
        if (file.top_header.size() < 0x74) return fail("Replay top header unavailable");
        score = read_i32(top + 0x14);
        extra = read_i32(top + 0x6C);
    }
    out = format("%s  %.8d%d", kStageNames[number], score, extra);
    return true;
}

bool TitleReplayMenu::draw_texts(HudDrawServices& host) {
    if (i32(state.screen) != i32(TitleScreen::Replay)) return true;
    if (state.substate != 2 && state.substate != 4) return true;
    HudTextStyle style;
    const auto put = [&](float x, float y, u32 color, const std::string& text) {
        style.color = color;
        return host.hud_text({{x, y, 0}, style, text});
    };
    if (state.substate == 2) {
        for (i32 r = 0; r < kRowsPerPage; r++) {
            const i32 index = page.cursor * kRowsPerPage + r;
            if (index >= replay_host.replay_count()) break;
            std::string text;
            if (!file_row_text(index, text)) return false;
            const u32 color = r == state.menu.cursor ? kColorSelected : kColorUnselected;
            if (!put(58, 80 + 15 * r, color, text)) return false;
        }
        return true;
    }
    const ReplayFile* file = selected_file();
    if (file) {
        float header_y = 80;
        if (state.age.current < 10)
            header_y = 80 + float(selected_index % kRowsPerPage) * 15 * (10 - state.age.fractional) / 10;
        std::string text;
        if (!file_row_text(selected_index, text)) return false;
        if (!put(58, header_y, kColorHeader, text)) return false;
    }
    if (state.age.current > 9 && file) {
        for (i32 k = 0; k < kStageRows; k++) {
            std::string text;
            if (!stage_row_text(*file, k, text)) return false;
            const u32 color = k == state.menu.cursor ? kColorSelected : kColorUnselected;
            if (!put(220, 128 + 18 * k, color, text)) return false;
        }
    }
    return true;
}

}
