#include "TitleMusicRoomMenu.hpp"

namespace th13 {
namespace {
constexpr i32 kWindowRows = 10;
constexpr float kRowStep = 20.f;
constexpr float kOriginX = 58.f, kOriginY = 80.f;
constexpr i32 kCommentLines = 8;
}

bool TitleMusicRoomMenu::sound(i32 id) {
    if (play_sound && play_sound(id)) return true;
    return fail("Title music room sound service unavailable");
}

bool TitleMusicRoomMenu::leave() {
    if (!host.resume_title_music()) fail("Title music room title music resume unavailable");
    if (state.menu.depth > 0) state.menu.pop();
    state.change_screen(TitleScreen::Main);
    return true;
}

bool TitleMusicRoomMenu::update(u32 pressed, u32 repeated) {
    const u32 input = pressed | repeated;
    const i32 count = host.track_count();
    ++frames_;
    switch (state.substate) {
    case 0:
        row_ = 0;
        comment_track_ = 0;
        reveal_lines_ = 0;
        locked_armed_ = false;
        if (state.menu.depth == 0) {
            state.menu.push();
            state.menu.wrapping = true;
            state.menu.count = count > 0 ? count : 1;
            state.menu.select(0);
        }
        if (frames_ >= 10) {
            state.change_substate(1);
            frames_ = 0;
        }
        break;
    case 1:
        if ((frames_ & 1) == 0 && reveal_lines_ < kCommentLines) ++reveal_lines_;
        if (frames_ > 4) {
            state.change_substate(2);
            frames_ = 0;
        }
        break;
    case 2: {
        if (count <= 0) {
            if (pressed & 0x102) {
                if (!sound(9)) return false;
                state.change_substate(3);
                frames_ = 0;
            }
            break;
        }
        if ((frames_ & 1) == 0 && reveal_lines_ < kCommentLines) ++reveal_lines_;
        const i32 previous = row_;
        if (input & 0x10)
            row_ = (row_ + count - 1) % count;
        else if (input & 0x20)
            row_ = (row_ + 1) % count;
        if (row_ != previous) {
            if (!sound(10)) return false;
            if (reveal_lines_ >= kCommentLines) locked_armed_ = false;
        }
        if (pressed & 0x80001) {
            comment_track_ = row_;
            reveal_lines_ = 0;
            if (!host.track_heard(comment_track_) && !locked_armed_) {
                if (!host.play_locked_response()) {
                    fail("Title music room locked response unavailable");
                    return false;
                }
                locked_armed_ = true;
            } else {
                if (!host.play_track(comment_track_)) {
                    fail("Title music room track playback unavailable");
                    return false;
                }
                host.mark_heard(0);
                locked_armed_ = false;
            }
            return true;
        }
        if (pressed & 0x102) {
            if (!sound(9)) return false;
            state.change_substate(3);
            frames_ = 0;
        }
        break;
    }
    case 3:
        if (frames_ > 9) {
            if (!leave()) return false;
        }
        break;
    default:
        break;
    }
    return true;
}

bool TitleMusicRoomMenu::draw_texts(HudDrawServices& draw) {
    if (i32(state.screen) != kTitleScreenMusicRoom) return true;
    if (state.substate != 1 && state.substate != 2) return true;
    const i32 count = host.track_count();
    if (count <= 0) return true;
    HudTextStyle style;
    const auto put = [&](float x, float y, u32 color, const std::string& text) {
        style.color = color;
        return draw.hud_text({{x, y, 0}, style, text});
    };
    const i32 top = row_ / kWindowRows * kWindowRows;
    for (i32 r = 0; r < kWindowRows; ++r) {
        const i32 index = top + r;
        if (index >= count) break;
        const bool heard = host.track_heard(index);
        const bool selected = index == row_;
        const std::string title = heard ? host.track_title(index) : host.locked_title(index);
        const float x = kOriginX + (selected ? 4.f : 0.f);
        if (!put(x, kOriginY + kRowStep * r, 0xffffffffu, title)) return false;
    }
    const bool locked = !host.track_heard(comment_track_) && locked_armed_;
    for (i32 line = 0; line < reveal_lines_ && line < kCommentLines; ++line) {
        const std::string text = locked ? host.locked_comment(line) : host.track_comment(comment_track_, line);
        if (text.empty()) continue;
        if (!put(kOriginX + 20.f, kOriginY + kRowStep * kWindowRows + 14.f * line, locked ? 0xff8080ffu : 0xffffffffu,
                 text))
            return false;
    }
    return true;
}

}
