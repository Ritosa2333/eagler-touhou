#include "TitleScene.hpp"
#include "TitleResultEntry.hpp"
#include "TitleMainMenu.hpp"

namespace th13 {

bool TitleResultEntry::fail(const std::string& reason) {
    if (error.empty())
        error = reason.empty() ? "Title result screen operation failed" : reason;
    return false;
}

// 0x454050 case3（DAT_004be7c8 != 0 分支）：FUN_00448080 + FUN_0044bac0(...,1)
// = 回主菜单。网页侧复用标题返回路径：flags bit1（原版 +0x5c40 bit1）令
// TitleMainMenu 跳过 180 帧 intro、直接展开菜单；光标归主菜单首项。
void TitleResultEntry::return_main() {
    state.flags |= 2u;
    state.return_reason = 1;
    state.menu.count = kTitleMainEntries;
    state.menu.wrapping = true;
    state.menu.disabled_count = 0;
    state.menu.select(0);
    state.change_screen(TitleScreen::Main);
}

// 0x454050 case2：名字编辑。无条件进入（原版 state0 负榜位只置 +0x59ac=1，
// 不改变进入编辑态的流程；本移植不接 result_needs_name 门）。
bool TitleResultEntry::name_editor(u32 pressed, u32 repeated) {
    constexpr i32 count = 91;  // 7 行 × 13 列（0x454050 case2 字面量）
    const i32 previous = state.result_name_cursor;
    // 方向键：0x10/0x20/0x40/0x80 与 PauseMenu phase18 同算式（跨行循环：
    // 左右在行首/行尾时绕到上一行/下一行）。
    if ((pressed | repeated) & 0x10)
        state.result_name_cursor = (state.result_name_cursor + count - 13) % count;
    if ((pressed | repeated) & 0x20)
        state.result_name_cursor = (state.result_name_cursor + 13) % count;
    if ((pressed | repeated) & 0x40)
        state.result_name_cursor =
            (state.result_name_cursor +
             (state.result_name_cursor % 13 == 0 ? 12 : count - 1)) % count;
    if ((pressed | repeated) & 0x80)
        state.result_name_cursor =
            (state.result_name_cursor +
             (state.result_name_cursor % 13 == 12 ? count - 12 : 1)) % count;
    if (previous != state.result_name_cursor && !host.sound(10))
        return fail("Result name cursor sound failed");
    if (pressed & 0x80001) {  // Z/Enter
        if (state.result_name_cursor == 89) {  // 0x59：DEL（长度 0 时不动作）
            if (!state.result_name.empty()) {
                state.result_name.pop_back();
                if (!host.sound(9)) return fail("Result name delete sound failed");
            }
        } else if (state.result_name_cursor == 90) {  // 0x5a：END
            // 0x454050 case2 END 拍：名字写回成绩表/默认名（宿主
            // record_result_name 等价）→ substate3。state3 DAT_004be7c8==0
            // → FUN_0044bac0(...,0xf) = 0x4549d0；否则回主菜单。
            if (!host.record_result_name(state.result_name))
                return fail("Result name record failed");
            if (!host.sound(7)) return fail("Result name confirm sound failed");
            if (host.result_replay_save_available()) {
                state.change_screen(TitleScreen::ResultReplaySlot);
            } else {
                return_main();
            }
            return true;
        } else {
            // 0..87 = 字符表项；0x58 (=88) = SPACE。满 8 字后覆盖第 8 字符
            // （原版 `+0x59a8 < 8 ? append : [7] = ch`），满 8 字光标跳 END。
            const char ch = state.result_name_cursor == 88
                                ? ' '
                                : kTitleResultNameCharacters[state.result_name_cursor];
            if (state.result_name.size() < 8)
                state.result_name += ch;
            else
                state.result_name[7] = ch;
            if (state.result_name.size() >= 8) state.result_name_cursor = 90;
            if (!host.sound(7)) return fail("Result name input sound failed");
        }
        return true;
    }
    if (pressed & 0x102) {  // X/Esc：原版有字时删末字符，空名不退出
        if (!state.result_name.empty()) {
            state.result_name.pop_back();
            if (!host.sound(9)) return fail("Result name cancel sound failed");
        }
    }
    return true;
}

// 0x4549d0 case2：25 个编号槽（+0x28，0 基）。槽序显示/落盘 = +1（1..25）。
bool TitleResultEntry::slot_selector(u32 pressed, u32 repeated) {
    constexpr i32 slots = 25;
    const i32 previous = state.result_slot;
    if ((pressed | repeated) & 0x10)
        state.result_slot = (state.result_slot + slots - 1) % slots;
    if ((pressed | repeated) & 0x20)
        state.result_slot = (state.result_slot + 1) % slots;
    if (previous != state.result_slot && !host.sound(10))
        return fail("Replay slot cursor sound failed");
    if (pressed & 0x102) {  // X/Esc：跳过保存回主菜单（原版未保存出口）
        if (!host.sound(9)) return fail("Replay slot cancel sound failed");
        return_main();
        return true;
    }
    if (pressed & 0x80001) {  // Z：0x4549d0 case3 END → 0x4484d0 写文件
        // 原版此拍 sprintf("th13_%.2d.rpy", +0x28+1) 后 FUN_004484d0 落盘。
        // 宿主 save_replay_slot 的第一个参数是 1 基槽号（与 RunPause 同签名）。
        if (!host.save_replay_slot(state.result_slot + 1, state.result_name))
            return fail("Replay save failed");
        if (!host.sound(7)) return fail("Replay slot confirm sound failed");
        return_main();
        return true;
    }
    return true;
}

bool TitleResultEntry::update(u32 pressed, u32 repeated) {
    if (!error.empty()) return false;
    switch (state.screen) {
    case TitleScreen::ResultName:
        switch (state.substate) {
        case 0:
            // 0x454050 case0：读榜位/默认名（原版 score+0x1b260）并建面。
            // 网页宿主暂无默认名字段，按空名 + END 光标进入（PauseMenu
            // phase18 的初始约定）；负榜位（+0x59ac=1）不影响进入编辑态。
            state.result_name.clear();
            state.result_name_cursor = 90;
            state.result_slot = 0;
            state.change_substate(1);
            break;
        case 1:
            // 0x454050 case1：>6 帧后 FUN_0044bb20(2) 进名字编辑。
            if (state.age.current > 6) state.change_substate(2);
            break;
        case 2:
            return name_editor(pressed, repeated);
        default:
            break;
        }
        break;
    case TitleScreen::ResultReplaySlot:
        switch (state.substate) {
        case 0:
            // 0x4549d0 case0：25 槽表 + 0x447fc0 查已存录像。宿主暂无逐槽
            // 查询接口，列表显示未接（见头文件窄化登记）；槽序归 0。
            state.result_slot = 0;
            state.change_substate(1);
            break;
        case 1:
            // 0x4549d0 case1：>6 帧后 FUN_0044bb20(2) 进槽选择。
            if (state.age.current > 6) state.change_substate(2);
            break;
        case 2:
            return slot_selector(pressed, repeated);
        default:
            break;
        }
        break;
    default:
        return fail("Title result screen out of range");
    }
    return true;
}

bool TitleResultEntry::draw_texts(HudDrawServices& draw) {
    if (!error.empty()) return false;
    HudTextStyle style;  // 队列默认：font0、左对齐、scale1（与 Replay/音楽室屏同）
    const auto put = [&](float x, float y, u32 color, const std::string& text) {
        style.color = color;
        return draw.hud_text({{x, y, 0}, style, text}) ||
               fail("Result screen text service failed");
    };
    if (state.screen == TitleScreen::ResultName) {
        if (state.substate > 2) return true;
        // 名字行：8 字缓冲，空位显示 '_'（编辑语义同 0x454050 case2）。
        std::string name = state.result_name;
        if (name.size() < 8) name.append(8 - name.size(), '_');
        if (!put(58, 48, 0xffffffffu, "RESULT NAME")) return false;
        if (!put(58, 72, 0xffdfdfdfu, "NAME  " + name)) return false;
        // 91 字符网格（7×13）：88=SPACE、89=DEL、90=END；选中黄色、
        // 未选灰色（标题菜单同族配色 0xffffff00 / 0xff808080）。
        for (i32 i = 0; i < 91; ++i) {
            const i32 row = i / 13, column = i % 13;
            const std::string glyph =
                i == 88 ? std::string("SPACE")
                         : i == 89 ? std::string("DEL")
                                   : i == 90 ? std::string("END")
                                             : std::string(1, kTitleResultNameCharacters[i]);
            const u32 color = i == state.result_name_cursor ? 0xffffff00u : 0xff808080u;
            if (!put(58 + column * 20, 104 + row * 16, color, glyph)) return false;
        }
        return true;
    }
    if (state.screen == TitleScreen::ResultReplaySlot) {
        if (state.substate > 2) return true;
        // 0x4549d0 的已存录像列表（0x447fc0 逐槽查 "th13_%.2d.rpy"）未接：
        // 只画槽号选择行，不伪造列表内容（窄化登记见头文件）。
        if (!put(240, 200, 0xffffffffu, "SAVE REPLAY")) return false;
        if (!put(240, 224, 0xffffff00u,
                 "REPLAY SLOT " + std::to_string(state.result_slot + 1) + " / 25"))
            return false;
        if (!put(240, 248, 0xff808080u, "Z SAVE   X SKIP")) return false;
        return true;
    }
    return true;
}

}  // namespace th13
