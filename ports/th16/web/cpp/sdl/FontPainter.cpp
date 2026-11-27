#include "FontPainter.hpp"

#include "GraphicsDevice.hpp"

#include <cstring>
#include <string_view>

#if defined(TH16_HAS_TTF)
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#endif

namespace th16::sdl {
namespace {

enum class Face { Gothic, Mincho, Meiryo };

struct FontSpec {
    i32 height;
    i32 weight;
    Face face;
};

// 0x458db0：句柄 DAT_004df90c/910/914/918/91c/920/924/928/908 按 0x459240 的 switch 序排成字体号 0..8
constexpr FontSpec kGothicTable[9] = {
    {0x20, 400, Face::Gothic}, {0x20, 600, Face::Mincho}, {0x0f, 700, Face::Gothic},
    {0x0f, 700, Face::Mincho}, {0x28, 400, Face::Gothic}, {0x28, 600, Face::Mincho},
    {0x0f, 700, Face::Gothic}, {0x0f, 700, Face::Mincho}, {0x1c, 400, Face::Gothic},
};
constexpr FontSpec kMeiryoTable[9] = {
    {0x30, 400, Face::Meiryo}, {0x20, 600, Face::Mincho}, {0x10, 400, Face::Meiryo},
    {0x0f, 700, Face::Mincho}, {0x3c, 400, Face::Meiryo}, {0x28, 600, Face::Mincho},
    {0x10, 400, Face::Meiryo}, {0x0f, 700, Face::Mincho}, {0x2a, 400, Face::Meiryo},
};
// 字体号越界时 0x459240 用 DAT_004df904
constexpr FontSpec kGothicDefault = {0x18, 400, Face::Gothic};
constexpr FontSpec kMeiryoDefault = {0x24, 400, Face::Meiryo};

}  // namespace

#if defined(TH16_HAS_TTF)
struct FontPainter::Faces final : TextGlyphSource {
    TTF_Font* fonts[10] = {};
    bool has_meiryo = false;
    std::vector<u16> encoding;
    TTF_Font* raster_font = nullptr;
    std::string raster_text;
    SDL_Surface* raster_surface = nullptr;

    ~Faces() override {
        SDL_DestroySurface(raster_surface);
        for (TTF_Font* f : fonts)
            if (f) TTF_CloseFont(f);
        TTF_Quit();
    }

    static std::string find(const char* name) {
        const std::string candidates[] = {std::string("/") + name, std::string("/fonts/") + name};
        for (const std::string& path : candidates) {
            if (SDL_IOStream* io = SDL_IOFromFile(path.c_str(), "rb")) {
                SDL_CloseIO(io);
                return path;
            }
        }
        return {};
    }

    // CreateFontA 正高度为单元高（ascent + descent）：先按高度开，再按实测单元高换算 em
    static TTF_Font* open(const std::string& path, const FontSpec& spec) {
        if (path.empty()) return nullptr;
        TTF_Font* font = TTF_OpenFont(path.c_str(), float(spec.height));
        if (font == nullptr) return nullptr;
        const int cell = TTF_GetFontAscent(font) - TTF_GetFontDescent(font);
        if (cell > 0 && cell != spec.height) TTF_SetFontSize(font, float(spec.height) * float(spec.height) / float(cell));
        TTF_SetFontKerning(font, false);
        TTF_SetFontHinting(font, TTF_HINTING_NORMAL);
        if (spec.weight >= 600) TTF_SetFontStyle(font, TTF_STYLE_BOLD);
        return font;
    }

    bool initialize(std::string& error) {
        if (!TTF_Init()) {
            error = "TTF_Init failed";
            return false;
        }
        const std::string gothic = find("msgothic.ttc");
        const std::string mincho = find("msmincho.ttc");
        const std::string meiryo = find("meiryo.ttc");
        if (gothic.empty() && meiryo.empty()) {
            error = "no msgothic.ttc / meiryo.ttc for text-into-texture";
            return false;
        }
        has_meiryo = !meiryo.empty();
        const FontSpec* table = has_meiryo ? kMeiryoTable : kGothicTable;
        for (int i = 0; i < 10; ++i) {
            const FontSpec& spec = i < 9 ? table[i] : (has_meiryo ? kMeiryoDefault : kGothicDefault);
            const std::string& path = spec.face == Face::Meiryo ? meiryo
                                      : spec.face == Face::Mincho && !mincho.empty() ? mincho
                                                                                      : gothic;
            fonts[i] = open(path.empty() ? gothic : path, spec);
        }
        encoding.assign(65536, 0x30fb);
        for (u32 c = 0; c < 0x80; ++c) encoding[c] = u16(c);
        size_t size = 0;
        if (void* bytes = SDL_LoadFile("/fonts/cp932.bin", &size)) {
            if (size == 131072) {
                const u8* b = static_cast<const u8*>(bytes);
                for (size_t i = 0; i < 65536; ++i) encoding[i] = u16(b[i * 2] | (b[i * 2 + 1] << 8));
            }
            SDL_free(bytes);
        }
        return true;
    }

    bool meiryo() const override { return has_meiryo; }

    SDL_Surface* rasterize(TTF_Font* font, std::string_view bytes) {
        if (raster_surface != nullptr && raster_font == font && raster_text == bytes) return raster_surface;

        std::string utf8;
        const u8* p = reinterpret_cast<const u8*>(bytes.data());
        const u8* end = p + bytes.size();
        while (p < end) {
            u32 code = *p++;
            if (((code >= 0x81 && code <= 0x9f) || (code >= 0xe0 && code <= 0xfc)) && p < end) code = (code << 8) | *p++;
            const u32 cp = encoding[code];
            if (code == 0 || cp == 0) continue;
            if (cp < 0x80) {
                utf8 += char(cp);
            } else if (cp < 0x800) {
                utf8 += char(0xc0 | (cp >> 6));
                utf8 += char(0x80 | (cp & 0x3f));
            } else {
                utf8 += char(0xe0 | (cp >> 12));
                utf8 += char(0x80 | ((cp >> 6) & 0x3f));
                utf8 += char(0x80 | (cp & 0x3f));
            }
        }
        if (utf8.empty()) return nullptr;
        SDL_Surface* surface = TTF_RenderText_Blended(font, utf8.c_str(), utf8.size(), SDL_Color{255, 255, 255, 255});
        if (surface == nullptr) return nullptr;
        SDL_Surface* rgba = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(surface);
        if (rgba == nullptr) return nullptr;

        SDL_DestroySurface(raster_surface);
        raster_surface = rgba;
        raster_font = font;
        raster_text.assign(bytes);
        return raster_surface;
    }

    void text_out(TextDib& dib, i32 font, i32 x, i32 y, u32 colorref, const char* bytes, i32 len) override {
        TTF_Font* f = fonts[font >= 0 && font < 9 ? font : 9];
        if (f == nullptr || len <= 0) return;
        // The shadow and foreground reuse the same glyph coverage at different offsets and colors.
        const SDL_Surface* rgba = rasterize(f, std::string_view(bytes, size_t(len)));
        if (rgba == nullptr) return;
        const u8* src = static_cast<const u8*>(rgba->pixels);
        for (int gy = 0; gy < rgba->h; ++gy) {
            const i32 dy = y + gy;
            if (dy < 0 || dy >= TextDib::kAllocRows) continue;
            for (int gx = 0; gx < rgba->w; ++gx) {
                const i32 dx = x + gx;
                if (dx < 0 || dx >= TextDib::kWidth) continue;
                const u8 coverage = src[gy * rgba->pitch + gx * 4 + 3];
                if (coverage == 0) continue;
                u16& pixel = dib.pixels[size_t(dy) * TextDib::kWidth + size_t(dx)];
                pixel = text_dib_blend(pixel, colorref, coverage);
            }
        }
    }
};
#else
struct FontPainter::Faces final : TextGlyphSource {
    bool initialize(std::string&) { return true; }
    bool meiryo() const override { return false; }
    void text_out(TextDib&, i32, i32, i32, u32, const char*, i32) override {}
};
#endif

FontPainter::FontPainter() = default;
FontPainter::~FontPainter() = default;

bool FontPainter::initialize() {
    if (faces_) return true;
    if (tried_) return false;
    tried_ = true;
    auto faces = std::make_unique<Faces>();
    if (!faces->initialize(error)) return false;
    faces_ = std::move(faces);
    return true;
}

bool FontPainter::flush(GraphicsDevice& graphics, AnmManager& animations, std::vector<TextIntoTextureRequest>& queue) {
    if (queue.empty()) return true;
    initialize();
    struct NoGlyphs final : TextGlyphSource {
        bool meiryo() const override { return false; }
        void text_out(TextDib&, i32, i32, i32, u32, const char*, i32) override {}
    } no_glyphs;
    TextGlyphSource& glyphs = faces_ ? static_cast<TextGlyphSource&>(*faces_) : no_glyphs;
    for (const TextIntoTextureRequest& req : queue) {
        AnmVm* vm = animations.find(req.vm_id);
        if (vm == nullptr) continue;
        const i32 slot = vm->sprite_source_slot >= 0 ? vm->sprite_source_slot : vm->file_slot;
        AnmFile* file = animations.file(slot);
        if (file == nullptr || file->resource == nullptr || vm->sprite_index < 0 ||
            u32(vm->sprite_index) >= file->sprites.size())
            continue;
        const AnmRuntimeSprite& spr = file->sprites[u32(vm->sprite_index)];
        const u32 handle = graphics.texture(*file->resource, u32(spr.entry_index));
        if (handle == 0) continue;
        i32 w = 0, h = 0;
        text_raster_paint(req, glyphs, dib_, rect_, w, h);
        graphics.write_texture_argb4444(handle, req.left, req.top, rect_.data(), w, h);
    }
    queue.clear();
    return true;
}

}  // namespace th16::sdl
