#pragma once
// Native boundary test declarations only; no SDL/GL implementation is linked.
#include <cstddef>
struct SDL_Window {};
using SDL_GLContext = void*;
inline constexpr const char* SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT = "keyboard";
inline void SDL_SetHint(const char*, const char*) {}
inline SDL_Window* SDL_GL_GetCurrentWindow() { return nullptr; }
inline void SDL_SetWindowTitle(SDL_Window*, const char*) {}
inline void SDL_SetWindowSize(SDL_Window*, int, int) {}
inline void* SDL_LoadFile(const char*, size_t*) { return nullptr; }
inline void SDL_free(void*) {}
