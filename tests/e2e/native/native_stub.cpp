// Native E2E 宿主桩：三件事，全部只属于测试进程，不改产品语义。
// 1) touhou::sdl::Renderer / State 的无 GPU 桩（同 tests/submission 契约；
//    不累积任何顶点/日志，长跑 11 万帧无内存增长）。
// 2) SDL3_ttf 面 → freetype（系统 libfreetype.so.6）真实栅格：字体路径
//    TH13_NATIVE_FONT（run.sh 注入 web/sdl-runtime/assets/msgothic.ttc），
//    生产 RuntimeFontRaster 原样消费 /msgothic.ttc 路径。字体只影响对话
//    纹理表现，不进入录像字节面；真实栅格而非空返回，避免对话路径 fail。
// 3) 无其他。SDL_LoadFile 资产重定向在 include/SDL3/SDL.h（头内内联）。
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
// Renderer/State 声明面（与 submission_test 同源：产品头 + 本目录 SDL 桩）。
#include "../../../web/cpp/sdl/GraphicsDevice.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>

// ---- 1) Renderer / State 桩 ----
namespace touhou::sdl {

State::State() {
    for (auto& m : matrix) m[0] = m[5] = m[10] = m[15] = 1;
}
Renderer::Renderer(int v, Resolve r, void* o) : version(v), resolve(r), owner(o) {}
Renderer::~Renderer() = default;
bool Renderer::initialize() { return true; }
void Renderer::flush() {}
void Renderer::discard() {}
bool Renderer::commit() { return true; }
void Renderer::transform(MatrixKind, const void*) {}
void Renderer::viewport(const Viewport&) {}
void Renderer::draw(Topology, u32, const void*, u32, const void*, IndexType) {}
void Renderer::draw_batch(u32, const void*, u32) {}
void Renderer::clear(u32, u32, float, u32, const i32*, u32) {}
void Renderer::copy(u32, const i32*, u32, const i32*) {}
bool Renderer::resample(u32, const i32*, u32, const i32*, const float*, u32, u32) { return true; }
void Renderer::changed(u32, u32, const std::array<u32, 4>&) {}
void Renderer::read(u32) {}
void Renderer::release(u32) {}
void Renderer::present(u32) {}
void Renderer::prepare(u32) {}

}  // namespace touhou::sdl

// ---- 2) TTF → freetype 桥 ----
namespace {

struct Face {
    FT_Library library = nullptr;
    FT_Face face = nullptr;
    float height = 0.f;
};

FT_Library& ft_library() {
    static FT_Library library = [] {
        FT_Library value = nullptr;
        if (FT_Init_FreeType(&value) != 0) value = nullptr;
        return value;
    }();
    return library;
}

const char* font_path(const char* path) {
    // RuntimeFontRaster 固定尝试 /msgothic.ttc 与 /fonts/msgothic.ttc；测试
    // 进程无根文件系统写权限，重定向到 run.sh 注入的原版资产副本。
    static const char kPaths[][24] = {"/msgothic.ttc", "/fonts/msgothic.ttc"};
    for (const auto& candidate : kPaths)
        if (std::strcmp(path, candidate) == 0) {
            const char* font = std::getenv("TH13_NATIVE_FONT");
            return font ? font : path;
        }
    return path;
}

}  // namespace

bool TTF_Init() { return ft_library() != nullptr; }
void TTF_Quit() {}

TTF_Font* TTF_OpenFont(const char* path, float height) {
    if (!ft_library()) return nullptr;
    auto* face = new Face;
    face->height = height;
    if (FT_New_Face(ft_library(), font_path(path), 0, &face->face) != 0) {
        delete face;
        return nullptr;
    }
    FT_Set_Pixel_Sizes(face->face, 0, (FT_UInt)height);
    return reinterpret_cast<TTF_Font*>(face);
}

void TTF_CloseFont(TTF_Font* font) {
    if (!font) return;
    auto* face = reinterpret_cast<Face*>(font);
    if (face->face) FT_Done_Face(face->face);
    delete face;
}

void TTF_SetFontKerning(TTF_Font*, bool) {}
void TTF_SetFontHinting(TTF_Font*, int) {}
void TTF_SetFontStyle(TTF_Font*, int) {}

bool TTF_FontHasGlyph(TTF_Font* font, unsigned code) {
    if (!font) return false;
    auto* face = reinterpret_cast<Face*>(font);
    return face->face && FT_Get_Char_Index(face->face, code) != 0;
}

bool TTF_GetGlyphMetrics(TTF_Font* font, unsigned code, int* left, int* right,
                         int* bottom, int* top, int* advance) {
    if (!font) return false;
    auto* face = reinterpret_cast<Face*>(font);
    if (!face->face) return false;
    const FT_UInt index = FT_Get_Char_Index(face->face, code);
    if (!index || FT_Load_Glyph(face->face, index, FT_LOAD_NO_BITMAP) != 0) return false;
    const FT_GlyphSlot slot = face->face->glyph;
    if (left) *left = slot->bitmap_left;
    if (right) *right = slot->bitmap_left + (int)slot->metrics.width / 64;
    if (bottom) *bottom = -slot->bitmap_top;
    if (top) *top = slot->bitmap_top;
    if (advance) *advance = (int)((slot->metrics.horiAdvance + 32) / 64);
    return true;
}

SDL_Surface* TTF_RenderGlyph_Blended(TTF_Font* font, unsigned code, SDL_Color color) {
    if (!font) return nullptr;
    auto* face = reinterpret_cast<Face*>(font);
    if (!face->face) return nullptr;
    const FT_UInt index = FT_Get_Char_Index(face->face, code);
    if (!index || FT_Load_Glyph(face->face, index, FT_LOAD_RENDER) != 0) return nullptr;
    const FT_Bitmap& bitmap = face->face->glyph->bitmap;
    if (bitmap.rows <= 0 || bitmap.width <= 0) {
        // 空白字形（如全角空格）：给 1x1 透明面，保持调用方成功路径。
        auto* blank = new SDL_Surface;
        blank->w = blank->h = 1;
        blank->pitch = 4;
        blank->pixels = std::calloc(1, 4);
        return blank;
    }
    auto* surface = new SDL_Surface;
    surface->format = SDL_PIXELFORMAT_RGBA32;
    surface->w = (int)bitmap.width;
    surface->h = (int)bitmap.rows;
    surface->pitch = surface->w * 4;
    surface->pixels = std::calloc(1, size_t(surface->pitch) * size_t(surface->h));
    if (!surface->pixels) { delete surface; return nullptr; }
    auto* out = static_cast<unsigned char*>(surface->pixels);
    const unsigned char* src = bitmap.buffer;  // FT_PIXEL_MODE_GRAY（probe 实证 mode=2）
    for (unsigned y = 0; y < bitmap.rows; ++y) {
        auto* row = out + size_t(y) * surface->pitch;
        for (unsigned x = 0; x < bitmap.width; ++x) {
            row[x * 4 + 0] = color.r;
            row[x * 4 + 1] = color.g;
            row[x * 4 + 2] = color.b;
            row[x * 4 + 3] = src[y * bitmap.pitch + x];
        }
    }
    return surface;
}
