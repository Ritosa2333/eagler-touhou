#pragma once
#include <eagler/thprac/PracticeKeyboard.hpp>
#include <SDL3/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace eagler::thprac {
inline void sdl_keyboard_frame(ImGuiIO& io) {
    int count=0;
    const bool* keys=SDL_GetKeyboardState(&count);
    keyboard_frame(io,[&](int scan) {
#ifdef __EMSCRIPTEN__
        const int hosted=EM_ASM_INT({
            return globalThis.__eaglerKeyboardInputBridge?.scancodeState($0) ?? -1;
        },scan);
        if (hosted>=0) return hosted!=0;
#endif
        return keys && scan<count && keys[scan];
    });
}
}
