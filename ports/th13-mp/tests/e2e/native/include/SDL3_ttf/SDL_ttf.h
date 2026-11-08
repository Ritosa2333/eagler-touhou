#pragma once
// Native E2E TTF 接口声明：由 tests/e2e/native/native_stub.cpp 以 freetype
// （系统 libfreetype.so.6 + emscripten-cache 头）真实实现。仅覆盖
// common/include/eagler/text/RuntimeFontRaster.hpp 消费的面。
#include "SDL3/SDL.h"

struct TTF_Font;

inline constexpr int TTF_STYLE_BOLD = 1;
inline constexpr int TTF_HINTING_NORMAL = 0;

bool TTF_Init();
void TTF_Quit();
TTF_Font* TTF_OpenFont(const char* path, float height);
void TTF_CloseFont(TTF_Font* font);
void TTF_SetFontKerning(TTF_Font* font, bool enabled);
void TTF_SetFontHinting(TTF_Font* font, int hinting);
void TTF_SetFontStyle(TTF_Font* font, int style);
bool TTF_FontHasGlyph(TTF_Font* font, unsigned code);
bool TTF_GetGlyphMetrics(TTF_Font* font, unsigned code, int* left, int* right,
                         int* bottom, int* top, int* advance);
SDL_Surface* TTF_RenderGlyph_Blended(TTF_Font* font, unsigned code, SDL_Color color);
