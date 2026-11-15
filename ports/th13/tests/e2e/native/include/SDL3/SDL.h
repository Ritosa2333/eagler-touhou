#pragma once
// Native E2E boundary stubs（仅测试宿主；不链接任何 SDL/GL 实现）。
// 同 tests/submission/include/SDL3/SDL.h 的最小集，另加：
//   - SDL_GetTicks：固定 0（boot 种子由测试显式 reseed，等价 web 的
//     window.__th13_boot_seed 确定性钩子）；
//   - SDL_LoadFile：把 "/assets/<name>" 解析到 TH13_NATIVE_ASSET_ROOT/<name>
//     （run.sh 注入），生产 read() 路径消费真实原版资产而不需要 VFS；
//   - Surface 小集 + SDL_PIXELFORMAT_RGBA32：TTF bridge（native_stub.cpp 的
//     freetype 实现）喂 RuntimeFontRaster 的唯一接口面。
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

struct SDL_Window {};
using SDL_GLContext = void*;
using Uint8 = unsigned char;
using Uint32 = unsigned int;

inline constexpr const char* SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT = "keyboard";
inline void SDL_SetHint(const char*, const char*) {}
inline SDL_Window* SDL_GL_GetCurrentWindow() { return nullptr; }
inline void SDL_SetWindowTitle(SDL_Window*, const char*) {}
inline void SDL_SetWindowSize(SDL_Window*, int, int) {}
inline Uint32 SDL_GetTicks() { return 0; }
inline void SDL_free(void* p) { std::free(p); }

inline void* SDL_LoadFile(const char* path, size_t* size) {
    std::string resolved(path);
    if (const char* root = std::getenv("TH13_NATIVE_ASSET_ROOT")) {
        static constexpr const char kPrefix[] = "/assets/";
        if (resolved.compare(0, sizeof kPrefix - 1, kPrefix) == 0)
            resolved = std::string(root) + "/" + resolved.substr(sizeof kPrefix - 1);
    }
    std::FILE* file = std::fopen(resolved.c_str(), "rb");
    if (!file) return nullptr;
    if (std::fseek(file, 0, SEEK_END) != 0) { std::fclose(file); return nullptr; }
    const long length = std::ftell(file);
    if (length < 0 || std::fseek(file, 0, SEEK_SET) != 0) { std::fclose(file); return nullptr; }
    void* data = std::malloc(size_t(length));
    if (!data) { std::fclose(file); return nullptr; }
    const size_t got = length ? std::fread(data, 1, size_t(length), file) : 0;
    std::fclose(file);
    if (got != size_t(length)) { std::free(data); return nullptr; }
    if (size) *size = size_t(length);
    return data;
}

// ---- Surface 小集（SDL3 布局子集；TTF bridge 两侧同型）----
using SDL_PixelFormat = unsigned;
inline constexpr SDL_PixelFormat SDL_PIXELFORMAT_RGBA32 = 0x61727467u;
struct SDL_Color { Uint8 r, g, b, a; };
struct SDL_Surface {
    int flags = 0;
    SDL_PixelFormat format = SDL_PIXELFORMAT_RGBA32;
    int w = 0, h = 0, pitch = 0;
    void* pixels = nullptr;
    int refcount = 0;
    void* reserved = nullptr;
};
inline SDL_Surface* SDL_ConvertSurface(SDL_Surface* src, SDL_PixelFormat format) {
    if (!src || src->format != format) return nullptr;  // bridge 只产 RGBA32
    SDL_Surface* out = new SDL_Surface(*src);
    out->pixels = std::malloc(size_t(src->pitch) * size_t(src->h));
    if (!out->pixels) { delete out; return nullptr; }
    std::memcpy(out->pixels, src->pixels, size_t(src->pitch) * size_t(src->h));
    out->refcount = 1;
    return out;
}
inline void SDL_DestroySurface(SDL_Surface* surface) {
    if (!surface) return;
    std::free(surface->pixels);
    delete surface;
}
