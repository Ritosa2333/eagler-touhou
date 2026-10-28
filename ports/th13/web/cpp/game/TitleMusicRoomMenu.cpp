#include "TitleMusicRoomMenu.hpp"

namespace th13 {
namespace {
constexpr i32 kWindowRows = 10;   // 0x4554f0:190 页顶+10 显隐门
constexpr float kRowStep = 20.f;  // 行距 20.0（fadd 步长实读）
constexpr float kOriginX = 58.f, kOriginY = 80.f;  // 队列起点（待对拍，见头注）
constexpr i32 kCommentLines = 8;  // musiccmt.txt 每块 8 行注释
}  // namespace

bool TitleMusicRoomMenu::fail(const std::string& reason) {
    if (error.empty()) error = reason;
    return false;
}

bool TitleMusicRoomMenu::sound(i32 id) {
    if (play_sound && play_sound(id)) return true;
    return fail("Title music room sound service unavailable");
}

// case3（>9 帧）：恢复标题曲 + 光标栈 pop + 回主菜单。
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
    case 0:  // case0：age≥10 进 case1（首帧建表/光标归 0 语义在屏入口）
        row_ = 0;
        reveal_lines_ = 0;
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
    case 1:  // case1：隔帧逐行显；age>4 → case2
        if ((frames_ & 1) == 0 && reveal_lines_ < kCommentLines) ++reveal_lines_;
        if (frames_ > 4) {
            state.change_substate(2);
            frames_ = 0;
        }
        break;
    case 2: {
        if (count <= 0) {
            if (input & 0x102) {  // 空表也可退出（宿主未接曲目表）
                if (!sound(9)) return false;
                state.change_substate(3);
                frames_ = 0;
            }
            break;
        }
        const i32 previous = row_;
        if (input & 0x10) row_ = (row_ + count - 1) % count;  // ↑ FUN_00473210(-1)
        else if (input & 0x20) row_ = (row_ + 1) % count;     // ↓
        if (row_ != previous) {
            if (!sound(10)) return false;
            reveal_lines_ = 0;  // 注释显影随选中重置（+0x74c=0 同拍）
        }
        if (input & 0x80001) {  // Z：0x4554f0:344 播放路径
            if (host.track_heard(row_)) {
                if (!host.play_track(row_)) fail("Title music room track playback unavailable");
                host.mark_heard(0);  // 原版任意播曲解锁 0 号曲（0x1b27a 字节 0）
            } else {
                if (!host.play_locked_response())
                    fail("Title music room locked response unavailable");
            }
        }
        if (input & 0x102) {  // X：音9 → case3
            if (!sound(9)) return false;
            state.change_substate(3);
            frames_ = 0;
        }
        break;
    }
    case 3:  // case3：>9 帧收面回主菜单（0x4554f0:426）
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
    HudTextStyle style;  // 队列默认：font0、左对齐、scale 1（同 Replay 屏）
    const auto put = [&](float x, float y, u32 color, const std::string& text) {
        style.color = color;
        return draw.hud_text({{x, y, 0}, style, text});
    };
    if (state.substate == 1) {
        // 入场隔帧逐行显（+0x74c 计数封 8：首 8 行逐行出现）。
        const i32 shown = reveal_lines_;
        for (i32 r = 0; r < shown && r < kWindowRows; ++r) {
            const bool heard = host.track_heard(r);
            const std::string& title = heard ? host.track_title(r) : host.locked_title();
            if (!put(kOriginX, kOriginY + kRowStep * r,
                     heard ? 0xffffff00u : 0xff8080ffu, title))
                return false;
        }
        return true;
    }
    // 列表：10 行窗口（页顶 = 选中/10*10），选中行 x+4（±4 poke 对）；
    // 锁定曲名占位色 0x8080ff（0x4554f0:193/237）。
    const i32 top = row_ / kWindowRows * kWindowRows;
    for (i32 r = 0; r < kWindowRows; ++r) {
        const i32 index = top + r;
        if (index >= count) break;
        const bool heard = host.track_heard(index);
        const bool selected = index == row_;
        const std::string& title = heard ? host.track_title(index) : host.locked_title();
        const float x = kOriginX + (selected ? 4.f : 0.f);
        if (!put(x, kOriginY + kRowStep * r, heard ? 0xffffff00u : 0xff8080ffu, title))
            return false;
    }
    // 注释：选中曲 8 行，显影计数内逐行（原版 +0x750=选中 +0x74c 计数）。
    for (i32 line = 0; line < reveal_lines_ && line < kCommentLines; ++line) {
        const std::string text = host.track_comment(row_, line);
        if (text.empty()) continue;
        if (!put(kOriginX + 20.f, kOriginY + kRowStep * kWindowRows + 14.f * line,
                 0xffffff00u, text))
            return false;
    }
    return true;
}

}  // namespace th13
