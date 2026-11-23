// th16 宿主键盘对照——SDL_Scancode / DOM KeyboardEvent.code ↔ DirectInput 扫描码（KeyboardMap.inc 第三列）。
// 原版 0x401d50 的键位是固定的（Key Config 只配手柄按钮，见 GameGlobals::pad_buttons）：宿主先把物理键折成
// 0x402130 的 DIK 快照（TitleScene::keyboard_state 也读它），再由 keyboard_input_bits 按 DirectInput 分支合成输入位。
#pragma once
#include "Input.hpp"
#include "Types.hpp"

#include <cstring>

#if defined(__EMSCRIPTEN__) || defined(TH16_HAS_SDL)
#include <SDL3/SDL.h>
#endif

namespace th16 {

// 0x401d50 DirectInput 分支（DAT_004c1800 & 0x400）：DIK → 输入位
inline u32 keyboard_input_bits(const u8 keys[256]) {
    auto down = [&](int dik) { return (keys[dik] & 0x80) != 0; };
    u32 bits = 0;
    if (down(0x2c)) bits |= InputState::kShot;                        // Z
    if (down(0x2d)) bits |= InputState::kBomb;                        // X
    if (down(0x2a) || down(0x36)) bits |= InputState::kSlow;          // LShift / RShift
    if (down(0x1d) || down(0x9d)) bits |= InputState::kSkip;          // LCtrl / RCtrl
    if (down(0x2e)) bits |= InputState::kSkip | InputState::kRelease; // C
    if (down(0xc8) || down(0x48)) bits |= InputState::kUp;            // ↑ / 小键盘 8
    if (down(0xd0) || down(0x50)) bits |= InputState::kDown;          // ↓ / 2
    if (down(0xcb) || down(0x4b)) bits |= InputState::kLeft;          // ← / 4
    if (down(0xcd) || down(0x4d)) bits |= InputState::kRight;         // → / 6
    if (down(0x47)) bits |= InputState::kUp | InputState::kLeft;      // 7
    if (down(0x49)) bits |= InputState::kUp | InputState::kRight;     // 9
    if (down(0x4f)) bits |= InputState::kDown | InputState::kLeft;    // 1
    if (down(0x51)) bits |= InputState::kDown | InputState::kRight;   // 3
    if (down(0x01)) bits |= InputState::kEscape;
    if (down(0x10)) bits |= InputState::kQ;
    if (down(0x1f)) bits |= InputState::kS;
    if (down(0x19) || down(0xc7)) bits |= InputState::kPause;         // P / Home
    if (down(0x1c) || down(0x9c)) bits |= InputState::kEnter;         // Enter / 小键盘 Enter
    if (down(0x20)) bits |= InputState::kD;
    if (down(0x13)) bits |= InputState::kR;
    if (down(0x44)) bits |= InputState::kF10;
    return bits;
}

// DOM KeyboardEvent.code（启动器 keyboard 命令的规范键名）→ DIK；未知返回 -1
inline i16 dom_code_to_di(const char* code) {
    struct Row { const char* code; i16 dik; };
    static constexpr Row kRows[] = {
        {"Escape", 0x01}, {"Enter", 0x1c}, {"NumpadEnter", 0x9c}, {"ControlLeft", 0x1d}, {"ControlRight", 0x9d},
        {"ShiftLeft", 0x2a}, {"ShiftRight", 0x36}, {"AltLeft", 0x38}, {"AltRight", 0xb8}, {"Space", 0x39},
        {"KeyA", 0x1e}, {"KeyB", 0x30}, {"KeyC", 0x2e}, {"KeyD", 0x20}, {"KeyE", 0x12}, {"KeyF", 0x21}, {"KeyG", 0x22},
        {"KeyH", 0x23}, {"KeyI", 0x17}, {"KeyJ", 0x24}, {"KeyK", 0x25}, {"KeyL", 0x26}, {"KeyM", 0x32}, {"KeyN", 0x31},
        {"KeyO", 0x18}, {"KeyP", 0x19}, {"KeyQ", 0x10}, {"KeyR", 0x13}, {"KeyS", 0x1f}, {"KeyT", 0x14}, {"KeyU", 0x16},
        {"KeyV", 0x2f}, {"KeyW", 0x11}, {"KeyX", 0x2d}, {"KeyY", 0x15}, {"KeyZ", 0x2c},
        {"Digit1", 0x02}, {"Digit2", 0x03}, {"Digit3", 0x04}, {"Digit4", 0x05}, {"Digit5", 0x06}, {"Digit6", 0x07},
        {"Digit7", 0x08}, {"Digit8", 0x09}, {"Digit9", 0x0a}, {"Digit0", 0x0b},
        {"ArrowUp", 0xc8}, {"ArrowDown", 0xd0}, {"ArrowLeft", 0xcb}, {"ArrowRight", 0xcd}, {"Home", 0xc7},
        {"Numpad0", 0x52}, {"Numpad1", 0x4f}, {"Numpad2", 0x50}, {"Numpad3", 0x51}, {"Numpad4", 0x4b}, {"Numpad5", 0x4c},
        {"Numpad6", 0x4d}, {"Numpad7", 0x47}, {"Numpad8", 0x48}, {"Numpad9", 0x49},
        {"F1", 0x3b}, {"F2", 0x3c}, {"F3", 0x3d}, {"F4", 0x3e}, {"F5", 0x3f}, {"F6", 0x40}, {"F7", 0x41}, {"F8", 0x42},
        {"F9", 0x43}, {"F10", 0x44}, {"F11", 0x57}, {"F12", 0x58},
    };
    if (code == nullptr) return -1;
    for (const Row& row : kRows)
        if (std::strcmp(row.code, code) == 0) return row.dik;
    return -1;
}

#if defined(__EMSCRIPTEN__) || defined(TH16_HAS_SDL)
inline i16 sdl_to_di(SDL_Scancode s) {
    switch (s) {
    case SDL_SCANCODE_ESCAPE: return 0x01;
    case SDL_SCANCODE_RETURN: return 0x1c;
    case SDL_SCANCODE_KP_ENTER: return 0x9c;
    case SDL_SCANCODE_LCTRL: return 0x1d;
    case SDL_SCANCODE_RCTRL: return 0x9d;
    case SDL_SCANCODE_LSHIFT: return 0x2a;
    case SDL_SCANCODE_RSHIFT: return 0x36;
    case SDL_SCANCODE_LALT: return 0x38;
    case SDL_SCANCODE_RALT: return 0xb8;
    case SDL_SCANCODE_SPACE: return 0x39;
    case SDL_SCANCODE_A: return 0x1e;
    case SDL_SCANCODE_B: return 0x30;
    case SDL_SCANCODE_C: return 0x2e;
    case SDL_SCANCODE_D: return 0x20;
    case SDL_SCANCODE_E: return 0x12;
    case SDL_SCANCODE_F: return 0x21;
    case SDL_SCANCODE_G: return 0x22;
    case SDL_SCANCODE_H: return 0x23;
    case SDL_SCANCODE_I: return 0x17;
    case SDL_SCANCODE_J: return 0x24;
    case SDL_SCANCODE_K: return 0x25;
    case SDL_SCANCODE_L: return 0x26;
    case SDL_SCANCODE_M: return 0x32;
    case SDL_SCANCODE_N: return 0x31;
    case SDL_SCANCODE_O: return 0x18;
    case SDL_SCANCODE_P: return 0x19;
    case SDL_SCANCODE_Q: return 0x10;
    case SDL_SCANCODE_R: return 0x13;
    case SDL_SCANCODE_S: return 0x1f;
    case SDL_SCANCODE_T: return 0x14;
    case SDL_SCANCODE_U: return 0x16;
    case SDL_SCANCODE_V: return 0x2f;
    case SDL_SCANCODE_W: return 0x11;
    case SDL_SCANCODE_X: return 0x2d;
    case SDL_SCANCODE_Y: return 0x15;
    case SDL_SCANCODE_Z: return 0x2c;
    case SDL_SCANCODE_1: return 0x02;
    case SDL_SCANCODE_2: return 0x03;
    case SDL_SCANCODE_3: return 0x04;
    case SDL_SCANCODE_4: return 0x05;
    case SDL_SCANCODE_5: return 0x06;
    case SDL_SCANCODE_6: return 0x07;
    case SDL_SCANCODE_7: return 0x08;
    case SDL_SCANCODE_8: return 0x09;
    case SDL_SCANCODE_9: return 0x0a;
    case SDL_SCANCODE_0: return 0x0b;
    case SDL_SCANCODE_UP: return 0xc8;
    case SDL_SCANCODE_DOWN: return 0xd0;
    case SDL_SCANCODE_LEFT: return 0xcb;
    case SDL_SCANCODE_RIGHT: return 0xcd;
    case SDL_SCANCODE_HOME: return 0xc7;
    case SDL_SCANCODE_KP_0: return 0x52;
    case SDL_SCANCODE_KP_1: return 0x4f;
    case SDL_SCANCODE_KP_2: return 0x50;
    case SDL_SCANCODE_KP_3: return 0x51;
    case SDL_SCANCODE_KP_4: return 0x4b;
    case SDL_SCANCODE_KP_5: return 0x4c;
    case SDL_SCANCODE_KP_6: return 0x4d;
    case SDL_SCANCODE_KP_7: return 0x47;
    case SDL_SCANCODE_KP_8: return 0x48;
    case SDL_SCANCODE_KP_9: return 0x49;
    case SDL_SCANCODE_F1: return 0x3b;
    case SDL_SCANCODE_F2: return 0x3c;
    case SDL_SCANCODE_F3: return 0x3d;
    case SDL_SCANCODE_F4: return 0x3e;
    case SDL_SCANCODE_F5: return 0x3f;
    case SDL_SCANCODE_F6: return 0x40;
    case SDL_SCANCODE_F7: return 0x41;
    case SDL_SCANCODE_F8: return 0x42;
    case SDL_SCANCODE_F9: return 0x43;
    case SDL_SCANCODE_F10: return 0x44;
    case SDL_SCANCODE_F11: return 0x57;
    case SDL_SCANCODE_F12: return 0x58;
    default: return -1;
    }
}
#endif

}  // namespace th16
