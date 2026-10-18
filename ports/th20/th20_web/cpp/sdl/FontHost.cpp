#include "../platform/Fonts.hpp"
#include "../platform/cp932_table.inc"
#include <cstring>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

namespace th20::web::fonts {
namespace {
FT_Library library = nullptr;
FT_Face faces[2]{};
bool tried = false;

FT_Face open(const char* const* candidates) {
    for (const char* const* path = candidates; *path; ++path) {
        FT_Face face = nullptr;
        if (!FT_New_Face(library, *path, 0, &face) && face) return face;
    }
    return nullptr;
}

void size_for(FT_Face face, int height_px) {
    if (face->size->metrics.y_ppem != height_px) FT_Set_Pixel_Sizes(face, 0, FT_UInt(height_px));
}

struct Pen { std::uint8_t* bgra; int pitch, rows, y; std::uint32_t r, g, b; };

void blend_glyph(Pen& pen, FT_Face face, int left) {
    FT_GlyphSlot slot = face->glyph;
    const int ascender = int(face->size->metrics.ascender >> 6);
    const int base_y = pen.y + ascender - slot->bitmap_top;
    for (unsigned row = 0; row < slot->bitmap.rows; ++row) {
        const int y = base_y + int(row);
        if (y < 0 || y >= pen.rows) continue;
        for (unsigned col = 0; col < slot->bitmap.width; ++col) {
            const int x = left + int(col);
            if (x < 0 || x >= pen.pitch / 4) continue;
            const unsigned coverage = slot->bitmap.buffer[row * slot->bitmap.pitch + col];
            if (!coverage) continue;
            auto* dst = pen.bgra + y * pen.pitch + x * 4;
            const unsigned keep = 255 - coverage;
            const unsigned channels[4]{pen.b, pen.g, pen.r, 0};
            for (int c = 0; c < 4; ++c)
                dst[c] = std::uint8_t((channels[c] * coverage + dst[c] * keep + 127) / 255);
        }
    }
}

void render(std::uint8_t* bgra, int pitch, int rows, int x, int y, const char* cp932,
            int height_px, int family, std::uint32_t text_color, int spacing, int* advanced) {
    FT_Face face = faces[family] ? faces[family] : faces[gothic];
    if (!face || !cp932) return;
    size_for(face, height_px);
    Pen pen{bgra, pitch, rows, y, (text_color >> 16) & 0xff, (text_color >> 8) & 0xff, text_color & 0xff};
    int pen_x = x;
    const char* p = cp932;
    while (*p) {
        const std::uint16_t code = cp932::map(p);
        if (!code) continue;
        if (!FT_Load_Char(face, code, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP)) {
            FT_GlyphSlot slot = face->glyph;
            FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
            blend_glyph(pen, face, pen_x + slot->bitmap_left);
            pen_x += spacing > 0 ? spacing : int(slot->advance.x >> 6);
        }
    }
    if (advanced) *advanced = pen_x - x;
}
}

bool initialize() {
    if (tried) return faces[gothic] != nullptr;
    tried = true;
    if (FT_Init_FreeType(&library)) return false;
    static const char* const gothic_candidates[]{
        "/fonts/msgothic.ttc", "/fonts/msgothic.ttf", "/fonts/meiryo.ttc",
        "/fonts/simhei.ttf", // launcher chs shared-font alias (/unifont.otf)
        "/fonts/unifont.otf", "/fonts/NotoSansJP-Regular.otf", "/fonts/unifont.ttf", nullptr};
    static const char* const mincho_candidates[]{
        "/fonts/msmincho.ttc", "/fonts/YuMincho.ttf", "/fonts/NotoSerifJP-Regular.otf", nullptr};
    faces[gothic] = open(gothic_candidates);
    faces[mincho] = open(mincho_candidates);
    return faces[gothic] != nullptr;
}

bool available(int family) {
    if (!tried) initialize();
    return family == mincho ? faces[mincho] != nullptr : faces[gothic] != nullptr;
}

void measure(const char* cp932, int height_px, int family, int& cx, int& cy) {
    cx = cy = 0;
    FT_Face face = faces[family] ? faces[family] : faces[gothic];
    if (!face || !cp932) return;
    size_for(face, height_px);
    cy = int(face->size->metrics.height >> 6);
    const char* p = cp932;
    while (*p) {
        const std::uint16_t code = cp932::map(p);
        if (!code) continue;
        if (!FT_Load_Char(face, code, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP))
            cx += int(face->glyph->advance.x >> 6);
    }
}

void draw(std::uint8_t* bgra, int pitch, int rows, int x, int y, const char* cp932,
          int height_px, int family, std::uint32_t text_color, int spacing) {
    render(bgra, pitch, rows, x, y, cp932, height_px, family, text_color, spacing, nullptr);
}
}
