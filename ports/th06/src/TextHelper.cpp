#include "TextHelper.hpp"
#include "GameErrorContext.hpp"
#include "GameWindow.hpp"
#include "FileSystem.hpp"
#include "Localization.hpp"
#include "Supervisor.hpp"
#include "i18n.hpp"

#include "thirdparty/sjis_converter.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>

#if defined(_WIN32) && defined(TH_ENABLE_THCRAP)
#include <windows.h>
#endif

static TTF_Font *g_Font;
static TTF_Font *g_LocalizedFallbackFont;
static u8 *g_LocalizedFontData;
static size_t g_LocalizedFontDataSize;

#define TEXT_BUFFER_HEIGHT 64

// GDI's positive CreateFont height is a requested *cell height*. SDL_ttf's
// TTF_SetFontSize, however, is a point/em size; fonts with taller ascender /
// descender metrics (notably Noto Sans) can therefore produce a raster cell
// much taller than the same numeric value. TH06 thcrap still copies a fixed
// 32-pixel-high 2x source region, so treating 30pt as a 30px GDI cell clips
// descenders such as y/g/p/q/j at the bottom.
//
// Cache the point size that produces the largest SDL_ttf cell not exceeding
// the requested GDI-style cell height. The cache is per active primary /
// fallback font and is cleared whenever the text buffer is recreated.
static std::array<float, TEXT_BUFFER_HEIGHT + 1> g_LocalizedPrimaryPointSize{};
static std::array<float, TEXT_BUFFER_HEIGHT + 1> g_LocalizedFallbackPointSize{};

static void SetLocalizedFontSize(i32 fontHeight);

static void ClearLocalizedFontSizeCache()
{
    g_LocalizedPrimaryPointSize.fill(0.0f);
    g_LocalizedFallbackPointSize.fill(0.0f);
}

static bool FontRasterFitsTh06Copy(TTF_Font *font, i32 targetCellHeight)
{
    if (font == nullptr || targetCellHeight <= 0 || TTF_GetFontHeight(font) <= 0 ||
        TTF_GetFontHeight(font) > targetCellHeight)
        return false;

    // base_tsa's text_scale_y patch makes the localized TH06 path copy a
    // fixed 32-pixel-high source region. SDL_ttf can report a cell height that
    // fits the requested GDI CreateFont height while still rasterizing a
    // descender one or two rows below that cell. Validate the actual raster,
    // otherwise the cross-platform fallback clips y/g/p/q/j at the bottom.
    SDL_Color white{255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(font, "Agypqj", 0, white);
    if (surface == nullptr)
        return false;

    i32 bottomInkRow = -1;
    if (SDL_LockSurface(surface))
    {
        const u8 *pixels = static_cast<const u8 *>(surface->pixels);
        for (i32 y = surface->h - 1; y >= 0 && bottomInkRow < 0; --y)
        {
            const u8 *row = pixels + y * surface->pitch;
            for (i32 x = 0; x < surface->w; ++x)
            {
                if (row[x * 4 + 3] != 0)
                {
                    bottomInkRow = y;
                    break;
                }
            }
        }
        SDL_UnlockSurface(surface);
    }
    SDL_DestroySurface(surface);
    return bottomInkRow >= 0 && bottomInkRow < 32;
}

static bool SetFontCellHeight(TTF_Font *font, i32 targetHeight, float *cachedPointSize)
{
    if (font == nullptr || targetHeight <= 0)
        return false;

    if (cachedPointSize != nullptr && *cachedPointSize > 0.0f)
        return TTF_SetFontSize(font, *cachedPointSize);

    // FreeType rounds metrics to whole pixels. Binary-search the greatest
    // point size whose resulting font cell still fits the GDI target height.
    float low = 1.0f;
    float high = static_cast<float>(targetHeight * 2);
    float best = low;
    for (int iteration = 0; iteration < 16; iteration++)
    {
        const float candidate = (low + high) * 0.5f;
        if (!TTF_SetFontSize(font, candidate))
            return false;
        if (FontRasterFitsTh06Copy(font, targetHeight))
        {
            best = candidate;
            low = candidate;
        }
        else
        {
            high = candidate;
        }
    }

    if (!TTF_SetFontSize(font, best) || !FontRasterFitsTh06Copy(font, targetHeight))
        return false;
    if (cachedPointSize != nullptr)
        *cachedPointSize = best;
    return true;
}

#if defined(_WIN32) && defined(TH_ENABLE_THCRAP)
static HDC g_LocalizedTextDc;
static HBITMAP g_LocalizedTextBitmap;
static HGDIOBJ g_LocalizedTextBitmapOriginal;
static u32 *g_LocalizedTextPixels;
static HANDLE g_LocalizedFontResource;
static std::wstring g_LocalizedGdiFontName;

static std::wstring Utf8ToWide(const char *text)
{
    if (text == nullptr || *text == '\0')
        return {};
    const int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, nullptr, 0);
    if (length <= 0)
        return {};
    std::wstring result(static_cast<std::size_t>(length), L'\0');
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, result.data(), length) == 0)
        return {};
    result.resize(static_cast<std::size_t>(length - 1));
    return result;
}

static HFONT CreateLocalizedGdiFont(i32 height)
{
    // Portable builds deliberately use one face on every language/platform.
    // Registering the same Unifont OTF below keeps the faithful native GDI
    // raster path without allowing a language pack or installed system font
    // to silently select a different face.
    const wchar_t *faceName = L"Unifont";
    return CreateFontW(height, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       ANTIALIASED_QUALITY, FF_ROMAN | FIXED_PITCH, faceName);
}

static bool CreateLocalizedGdiSurface(i32 width)
{
    BITMAPINFO bitmapInfo{};
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -TEXT_BUFFER_HEIGHT;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    g_LocalizedTextBitmap = CreateDIBSection(nullptr, &bitmapInfo, DIB_RGB_COLORS,
                                             reinterpret_cast<void **>(&g_LocalizedTextPixels), nullptr, 0);
    g_LocalizedTextDc = CreateCompatibleDC(nullptr);
    if (g_LocalizedTextBitmap == nullptr || g_LocalizedTextDc == nullptr || g_LocalizedTextPixels == nullptr)
    {
        if (g_LocalizedTextDc != nullptr)
            DeleteDC(g_LocalizedTextDc);
        if (g_LocalizedTextBitmap != nullptr)
            DeleteObject(g_LocalizedTextBitmap);
        g_LocalizedTextDc = nullptr;
        g_LocalizedTextBitmap = nullptr;
        g_LocalizedTextPixels = nullptr;
        return false;
    }
    g_LocalizedTextBitmapOriginal = SelectObject(g_LocalizedTextDc, g_LocalizedTextBitmap);
    SetBkMode(g_LocalizedTextDc, TRANSPARENT);
    return true;
}
#endif

TextHelper::TextHelper()
{
    //    this->format = (D3DFORMAT)-1;
    //    this->width = 0;
    //    this->height = 0;
    //    this->hdc = 0;
    //    this->gdiObj2 = 0;
    //    this->gdiObj = 0;
    //    this->buffer = NULL;
}

TextHelper::~TextHelper()
{
    TTF_Quit();
}

// Extended to initialize all globals for text helper
ZunResult TextHelper::CreateTextBuffer()
{
    TTF_Init();
    ClearLocalizedFontSizeCache();

    const bool localized = Localization::Active();
    const std::string fontPath = FileSystem::GetBasePath(
        localized ? TH_LOCALIZED_FONT_FILENAME : TH_PRIMARY_FONT_FILENAME);
    g_Font = TTF_OpenFont(fontPath.c_str(), 10);
    if (g_Font == NULL)
    {
        std::printf("%s\n", SDL_GetError());

        g_GameErrorContext.Fatal(TH_ERR_FONTS_NOT_FOUND);
        return ZUN_ERROR;
    }

    if (localized)
    {
#if defined(_WIN32) && defined(TH_ENABLE_THCRAP)
        g_LocalizedFontData = static_cast<u8 *>(SDL_LoadFile(fontPath.c_str(), &g_LocalizedFontDataSize));
        if (g_LocalizedFontData != nullptr && g_LocalizedFontDataSize <= static_cast<size_t>(std::numeric_limits<DWORD>::max()))
        {
            DWORD fontCount = 0;
            g_LocalizedFontResource = AddFontMemResourceEx(
                g_LocalizedFontData, static_cast<DWORD>(g_LocalizedFontDataSize), nullptr, &fontCount);
            if (g_LocalizedFontResource != nullptr)
                g_LocalizedGdiFontName = L"Unifont";
        }
        if (g_LocalizedFontResource == nullptr)
            SDL_Log("th06 thcrap: failed to register GNU Unifont for the native GDI text path");
#endif

#if !defined(__EMSCRIPTEN__)
        // The faithful native TH06 text path requests FW_BOLD. Emscripten does
        // not use GDI, so preserve the established Web weight while using the
        // same Unifont face.
        TTF_SetFontStyle(g_Font, TTF_STYLE_BOLD);
#endif
    }

    // DrawTextToSprite copies a 2x source rectangle. Some vanilla text atlases
    // are 512 px wide, so the old 640 px scratch surface rejected their
    // 1022 px source rectangle before it could reach the destination texture.
    const i32 bufferWidth = std::max(GAME_WINDOW_WIDTH, 1024);
    g_TextBufferSurface = SDL_CreateSurface(bufferWidth, TEXT_BUFFER_HEIGHT, SDL_PIXELFORMAT_RGBA32);

    SDL_SetSurfaceBlendMode(g_TextBufferSurface, SDL_BLENDMODE_NONE);

#if defined(_WIN32) && defined(TH_ENABLE_THCRAP)
    if (Localization::Active() && !CreateLocalizedGdiSurface(bufferWidth))
        SDL_Log("th06 thcrap: failed to create the faithful GDI text surface; using SDL_ttf fallback");
#endif

    return ZUN_SUCCESS;
}

#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
bool TextHelper::DebugLocalizedFontMetricsSelfTest()
{
    FILE *auditFile = std::fopen("thcrap-font-selftest.txt", "wb");
    auto audit = [auditFile](const char *message) {
        if (auditFile != nullptr)
        {
            std::fputs(message, auditFile);
            std::fflush(auditFile);
        }
    };
    if (!Localization::Active())
    {
        SDL_Log("th06 thcrap font metrics self-test: localization inactive");
        std::fprintf(stderr, "th06 thcrap font metrics self-test: localization inactive\n");
        audit("th06 thcrap font metrics self-test: localization inactive\n");
        if (auditFile != nullptr)
            std::fclose(auditFile);
        return false;
    }
    if (CreateTextBuffer() != ZUN_SUCCESS || g_Font == nullptr)
    {
        std::fprintf(stderr, "th06 thcrap font metrics self-test: CreateTextBuffer failed\n");
        audit("th06 thcrap font metrics self-test: CreateTextBuffer failed\n");
        if (auditFile != nullptr)
            std::fclose(auditFile);
        return false;
    }

    static constexpr const char *sample = "Agypqj";
    bool passed = true;
    for (const i32 logicalHeight : {15, 16})
    {
        SetLocalizedFontSize(logicalHeight);
        const i32 targetCellHeight = logicalHeight * 2;
        const i32 cellHeight = TTF_GetFontHeight(g_Font);
        const i32 ascent = TTF_GetFontAscent(g_Font);
        const i32 descent = TTF_GetFontDescent(g_Font);
        SDL_Color white{255, 255, 255, 255};
        SDL_Surface *surface = TTF_RenderText_Blended(g_Font, sample, 0, white);
        if (surface == nullptr)
        {
            passed = false;
            continue;
        }

        i32 bottomInkRow = -1;
        if (SDL_LockSurface(surface))
        {
            const u8 *pixels = static_cast<const u8 *>(surface->pixels);
            for (i32 y = surface->h - 1; y >= 0 && bottomInkRow < 0; --y)
            {
                const u8 *row = pixels + y * surface->pitch;
                for (i32 x = 0; x < surface->w; ++x)
                {
                    if (row[x * 4 + 3] != 0)
                    {
                        bottomInkRow = y;
                        break;
                    }
                }
            }
            SDL_UnlockSurface(surface);
        }

        const bool fitsCell = cellHeight > 0 && cellHeight <= targetCellHeight;
        const bool descenderVisible = bottomInkRow >= 0 && bottomInkRow < 32;
        SDL_Log("th06 thcrap SDL_ttf font metrics: logical=%d targetCell=%d point=%.3f "
                "cell=%d ascent=%d descent=%d surface=%dx%d bottomInk=%d %s",
                logicalHeight, targetCellHeight, TTF_GetFontSize(g_Font), cellHeight,
                ascent, descent, surface->w, surface->h, bottomInkRow,
                (fitsCell && descenderVisible) ? "PASS" : "FAIL");
        std::fprintf(stderr,
                     "th06 thcrap SDL_ttf font metrics: logical=%d targetCell=%d point=%.3f "
                     "cell=%d ascent=%d descent=%d surface=%dx%d bottomInk=%d %s\n",
                     logicalHeight, targetCellHeight, TTF_GetFontSize(g_Font), cellHeight,
                     ascent, descent, surface->w, surface->h, bottomInkRow,
                     (fitsCell && descenderVisible) ? "PASS" : "FAIL");
        if (auditFile != nullptr)
        {
            std::fprintf(auditFile,
                         "logical=%d targetCell=%d point=%.3f cell=%d ascent=%d descent=%d "
                         "surface=%dx%d bottomInk=%d %s\n",
                         logicalHeight, targetCellHeight, TTF_GetFontSize(g_Font), cellHeight,
                         ascent, descent, surface->w, surface->h, bottomInkRow,
                         (fitsCell && descenderVisible) ? "PASS" : "FAIL");
            std::fflush(auditFile);
        }
        passed = passed && fitsCell && descenderVisible;
        SDL_DestroySurface(surface);
    }

    ReleaseTextBuffer();
    if (auditFile != nullptr)
        std::fclose(auditFile);
    return passed;
}
#endif

bool TextHelper::InvertAlpha(i32 x, i32 y, i32 spriteWidth, i32 fontHeight)
{
    u8 *bufferCursor;
    i32 gradientArea;
    i32 i = 0;

    gradientArea = spriteWidth * fontHeight;

    SDL_LockSurface(g_TextBufferSurface);

    // In D3D EoSD this function mostly inverts the alpha, but on A1R5G5B5 surfaces specifically it also
    //   creates a gradient. D3D EoSD will always attempt to create an A1R5G5B5 surface for the text buffer,
    //   will only attempt use other formats as a fallback, and in those cases the text will be bugged anyway.
    //   As part of the port from GDI to SDL_ttf, we've converted the text buffer surface to always be RGBA32
    //   and no longer need the alpha inversion, but we still want that gradient to be applied

    for (bufferCursor = (u8 *)g_TextBufferSurface->pixels; i < gradientArea; i++, bufferCursor += 4)
    {
        if (bufferCursor[3]) // A
        {
            bufferCursor[0] = bufferCursor[0] - bufferCursor[0] * i / gradientArea / 2; // R
            bufferCursor[1] = bufferCursor[1] - bufferCursor[1] * i / gradientArea / 2; // G
            bufferCursor[2] = bufferCursor[2] - bufferCursor[2] * i / gradientArea / 4; // B
        }
    }

    SDL_UnlockSurface(g_TextBufferSurface);

    return true;
}

// Text strings in asset files are encoded using Shift_JIS. This allows RenderTextToTexture to handle both UTF-8 and
// Shift_JIS. This also does not check for overlong encoding, but that shouldn't matter
bool isUTF8Encoded(const char *string)
{
#define UTF8_1BYTE_MASK 0x80
#define UTF8_2BYTE_MASK 0xE0
#define UTF8_3BYTE_MASK 0xF0
#define UTF8_4BYTE_MASK 0xF8

#define UTF8_2NDBYTE_MASK 0xC0

// 0xxx xxxx
#define UTF8_1BYTE_PREFIX 0x00
// 110x xxxx
#define UTF8_2BYTE_PREFIX 0xC0
// 1110 xxxx
#define UTF8_3BYTE_PREFIX 0xE0
// 1111 0xxx
#define UTF8_4BYTE_PREFIX 0xF0

// 10xx xxxx
#define UTF8_2NDBYTE_PREFIX 0x80

    bool isMultiByteParse = false;
    int codepointLen = 0;

    while (*string != '\0')
    {
        unsigned char c = *(unsigned char *)string;

        if (!isMultiByteParse)
        {
            if ((c & UTF8_1BYTE_MASK) != UTF8_1BYTE_PREFIX)
            {
                isMultiByteParse = true;

                if ((c & UTF8_2BYTE_MASK) == UTF8_2BYTE_PREFIX)
                    codepointLen = 1;
                else if ((c & UTF8_3BYTE_MASK) == UTF8_3BYTE_PREFIX)
                    codepointLen = 2;
                else if ((c & UTF8_4BYTE_MASK) == UTF8_4BYTE_PREFIX)
                    codepointLen = 3;
                else
                    return false;
            }
        }
        else
        {
            if ((c & UTF8_2NDBYTE_MASK) != UTF8_2NDBYTE_PREFIX)
                return false;

            if (--codepointLen == 0)
                isMultiByteParse = false;
        }

        string++;
    }

    return true;

#undef UTF8_1BYTE_MASK
#undef UTF8_2BYTE_MASK
#undef UTF8_3BYTE_MASK
#undef UTF8_4BYTE_MASK

#undef UTF8_2NDBYTE_MASK

#undef UTF8_1BYTE_PREFIX
#undef UTF8_2BYTE_PREFIX
#undef UTF8_3BYTE_PREFIX
#undef UTF8_4BYTE_PREFIX

#undef UTF8_2NDBYTE_PREFIX
}

static void SetLocalizedFontSize(i32 fontHeight)
{
    const i32 targetCellHeight = fontHeight * 2;
    if (Localization::Active() && targetCellHeight > 0 && targetCellHeight <= TEXT_BUFFER_HEIGHT)
    {
        if (!SetFontCellHeight(g_Font, targetCellHeight,
                               &g_LocalizedPrimaryPointSize[static_cast<std::size_t>(targetCellHeight)]))
            TTF_SetFontSize(g_Font, static_cast<float>(targetCellHeight));
        if (g_LocalizedFallbackFont != nullptr &&
            !SetFontCellHeight(g_LocalizedFallbackFont, targetCellHeight,
                               &g_LocalizedFallbackPointSize[static_cast<std::size_t>(targetCellHeight)]))
            TTF_SetFontSize(g_LocalizedFallbackFont, static_cast<float>(targetCellHeight));
        return;
    }

    const float size = static_cast<float>(targetCellHeight);
    TTF_SetFontSize(g_Font, size);
    if (g_LocalizedFallbackFont != nullptr)
        TTF_SetFontSize(g_LocalizedFallbackFont, size);
}

static void ConvertTextToUtf8(const char *string, char *destination, std::size_t capacity)
{
    if (destination == nullptr || capacity == 0)
        return;
    destination[0] = '\0';
    if (string == nullptr)
        return;
    if (!isUTF8Encoded(string))
    {
        char *utf8 = sjis2utf8(string);
        std::snprintf(destination, capacity, "%s", utf8 == nullptr ? "" : utf8);
        free(utf8);
    }
    else
    {
        std::snprintf(destination, capacity, "%s", string);
    }
}

#if defined(_WIN32) && defined(TH_ENABLE_THCRAP)
static bool RenderLocalizedTextGdi(i32 xPos, i32 spriteWidth, i32 fontHeight,
                                   ZunColor textColor, ZunColor shadowColor, const char *utf8)
{
    if (!Localization::Active() || g_LocalizedTextDc == nullptr || g_LocalizedTextPixels == nullptr ||
        g_LocalizedFontResource == nullptr)
        return false;
    const std::wstring text = Utf8ToWide(utf8);
    HFONT font = CreateLocalizedGdiFont(fontHeight * 2);
    if (text.empty() || font == nullptr)
    {
        if (font != nullptr)
            DeleteObject(font);
        return false;
    }

    const i32 surfaceWidth = g_TextBufferSurface->w;
    std::fill_n(g_LocalizedTextPixels, static_cast<std::size_t>(surfaceWidth) * TEXT_BUFFER_HEIGHT, 0xff000000u);
    HGDIOBJ oldFont = SelectObject(g_LocalizedTextDc, font);
#ifdef TH_DEV_TOOLS
    static bool loggedFontMetrics = false;
    if (!loggedFontMetrics)
    {
        wchar_t selectedFace[LF_FACESIZE]{};
        TEXTMETRICW metrics{};
        GetTextFaceW(g_LocalizedTextDc, LF_FACESIZE, selectedFace);
        GetTextMetricsW(g_LocalizedTextDc, &metrics);
        SDL_Log("th06 thcrap GDI font: requested=Unifont selected=%ls height=%ld ascent=%ld descent=%ld "
                "internalLeading=%ld externalLeading=%ld weight=%ld charset=%u",
                selectedFace, metrics.tmHeight, metrics.tmAscent, metrics.tmDescent,
                metrics.tmInternalLeading, metrics.tmExternalLeading, metrics.tmWeight,
                static_cast<unsigned>(metrics.tmCharSet));
        loggedFontMetrics = true;
    }
#endif
    if (shadowColor != COLOR_WHITE)
    {
        SetTextColor(g_LocalizedTextDc, static_cast<COLORREF>(shadowColor & 0x00ffffff));
        TextOutW(g_LocalizedTextDc, xPos * 2 + 3, 2, text.c_str(), static_cast<int>(text.size()));
    }
    SetTextColor(g_LocalizedTextDc, static_cast<COLORREF>(textColor & 0x00ffffff));
    TextOutW(g_LocalizedTextDc, xPos * 2, 0, text.c_str(), static_cast<int>(text.size()));
    GdiFlush();
    SelectObject(g_LocalizedTextDc, oldFont);
    DeleteObject(font);

    SDL_LockSurface(g_TextBufferSurface);
    const i32 copyWidth = std::min(surfaceWidth, spriteWidth * 2);
    for (i32 y = 0; y < TEXT_BUFFER_HEIGHT; y++)
    {
        u8 *destination = static_cast<u8 *>(g_TextBufferSurface->pixels) + y * g_TextBufferSurface->pitch;
        const u32 *source = g_LocalizedTextPixels + y * surfaceWidth;
        for (i32 x = 0; x < copyWidth; x++)
        {
            const u32 bgra = source[x];
            destination[x * 4 + 0] = static_cast<u8>((bgra >> 16) & 0xff);
            destination[x * 4 + 1] = static_cast<u8>((bgra >> 8) & 0xff);
            destination[x * 4 + 2] = static_cast<u8>(bgra & 0xff);
            // TH06 pre-fills the A1 DIB alpha bit, lets GDI clear it while
            // drawing, then inverts it again. The high byte of this 32-bit
            // DIB gives us the same drawn-vs-background distinction,
            // including black shadows.
            destination[x * 4 + 3] = static_cast<u8>(0xff - ((bgra >> 24) & 0xff));
        }
    }
    SDL_UnlockSurface(g_TextBufferSurface);
    return true;
}
#endif

float TextHelper::MeasureTextWidth(const char *string, i32 fontHeight)
{
    if (g_Font == nullptr || string == nullptr)
        return 0.0f;
    char convertedText[1024];
    ConvertTextToUtf8(string, convertedText, sizeof(convertedText));
#if defined(_WIN32) && defined(TH_ENABLE_THCRAP)
    if (Localization::Active() && g_LocalizedTextDc != nullptr)
    {
        const std::wstring text = Utf8ToWide(convertedText);
        HFONT font = CreateLocalizedGdiFont(fontHeight * 2);
        if (!text.empty() && font != nullptr)
        {
            HGDIOBJ oldFont = SelectObject(g_LocalizedTextDc, font);
            SIZE size{};
            const BOOL measured = GetTextExtentPoint32W(g_LocalizedTextDc, text.c_str(),
                                                        static_cast<int>(text.size()), &size);
            SelectObject(g_LocalizedTextDc, oldFont);
            DeleteObject(font);
            if (measured)
                return size.cx / 2.0f;
        }
        else if (font != nullptr)
        {
            DeleteObject(font);
        }
    }
#endif
    SetLocalizedFontSize(fontHeight);
    int width = 0;
    int height = 0;
    if (!TTF_GetStringSize(g_Font, convertedText, 0, &width, &height))
        return 0.0f;
    return width / 2.0f;
}

void SurfaceOverwriteBlend(SDL_Surface *srcSurface, SDL_Surface *dstSurface, i32 x)
{
    // Source surface is A8R8G8B8
    // Dest surface is RGBA32
    // We want to overwrite dest unless source has alpha 0

    SDL_LockSurface(srcSurface);
    SDL_LockSurface(dstSurface);

    u32 *srcData = (u32 *)srcSurface->pixels;
    u8 *dstData = (u8 *)dstSurface->pixels;

    for (int i = 0; i < srcSurface->h && i < dstSurface->h; i++)
    {
        for (int j = 0; j < srcSurface->w; j++)
        {
            const i32 destinationX = x + j;
            if (destinationX < 0 || destinationX >= dstSurface->w)
                continue;
            if ((srcData[j] & 0xFF00'0000) != 0)
            {
                dstData[i * dstSurface->pitch + destinationX * 4] = (srcData[j] >> 16) & 0xFF;
                dstData[i * dstSurface->pitch + destinationX * 4 + 1] = (srcData[j] >> 8) & 0xFF;
                dstData[i * dstSurface->pitch + destinationX * 4 + 2] = srcData[j] & 0xFF;
                dstData[i * dstSurface->pitch + destinationX * 4 + 3] = (srcData[j] >> 24) & 0xFF;
            }
        }

        srcData += srcSurface->pitch / 4;
    }

    SDL_UnlockSurface(dstSurface);
    SDL_UnlockSurface(srcSurface);
}

void TextHelper::RenderTextToTexture(i32 xPos, i32 yPos, i32 spriteWidth, i32 spriteHeight, i32 fontHeight,
                                     i32 fontWidth, ZunColor textColor, ZunColor shadowColor, const char *string,
                                     TextureData *outTexture)
{
    char convertedText[1024];
    SDL_Rect finalCopyDst;
    SDL_Rect finalCopySrc;
    SDL_Rect shadowRect;
    SDL_Rect textRect;

    ConvertTextToUtf8(string, convertedText, sizeof(convertedText));

    SetLocalizedFontSize(fontHeight);

    finalCopySrc.x = 0;
    finalCopySrc.y = 0;
    // base_tsa's TH06 text_scale_x/text_scale_y patches copy the full
    // double-width text region and a fixed 32-pixel-high DIB region into the
    // game's 16-pixel text sprite. Preserve the port's existing path when
    // localization is disabled.
    finalCopySrc.w = Localization::Active() ? spriteWidth * 2 : spriteWidth * 2 - 2;
    finalCopySrc.h = Localization::Active() ? 32 : fontHeight * 2 - 2;

    SDL_FillSurfaceRect(g_TextBufferSurface, &finalCopySrc, 0);

#if defined(_WIN32) && defined(TH_ENABLE_THCRAP)
    const bool renderedWithGdi = RenderLocalizedTextGdi(xPos, spriteWidth, fontHeight,
                                                        textColor, shadowColor, convertedText);
#else
    const bool renderedWithGdi = false;
#endif

    if (!renderedWithGdi && shadowColor != COLOR_WHITE)
    {
        SDL_Surface *shadowText;

        // Render shadow.
        SDL_Color sdlShadowColor;
        sdlShadowColor.a = 0xFF;
        sdlShadowColor.b = (shadowColor >> 16) & 0xFF;
        sdlShadowColor.g = (shadowColor >> 8) & 0xFF;
        sdlShadowColor.r = shadowColor & 0xFF;

        shadowText = TTF_RenderText_Blended(g_Font, convertedText, 0, sdlShadowColor);

        if (shadowText != NULL)
        {
            shadowRect.x = xPos * 2 + 3;
            shadowRect.y = 2;
            shadowRect.w = shadowText->w;
            shadowRect.h = shadowText->h;

            SDL_SetSurfaceBlendMode(shadowText, SDL_BLENDMODE_NONE);
            SDL_BlitSurface(shadowText, NULL, g_TextBufferSurface, &shadowRect);

            SDL_DestroySurface(shadowText);
        }
    }

    SDL_Color sdlTextColor;
    sdlTextColor.a = 0xFF;
    sdlTextColor.b = (textColor >> 16) & 0xFF;
    sdlTextColor.g = (textColor >> 8) & 0xFF;
    sdlTextColor.r = textColor & 0xFF;

    SDL_Surface *regularText = renderedWithGdi ? nullptr : TTF_RenderText_Blended(g_Font, convertedText, 0, sdlTextColor);

    if (regularText != NULL)
    {
        textRect.x = xPos * 2;
        textRect.y = 0;
        textRect.w = regularText->w;
        textRect.h = regularText->h;

        SurfaceOverwriteBlend(regularText, g_TextBufferSurface, xPos * 2);

        SDL_DestroySurface(regularText);
    }

    // Once we get an API abstraction layer for surface operations, this needs to change
    //   We really shouldn't be clobbering the texture format
    if (!outTexture->textureData || outTexture->format != TEX_FMT_A8R8G8B8)
    {
        free(outTexture->textureData);
        outTexture->textureData = (u8 *)malloc(outTexture->width * outTexture->height * 4);
        memset(outTexture->textureData, 0, outTexture->width * outTexture->height * 4);
    }

    outTexture->format = TEX_FMT_A8R8G8B8;
    SDL_Surface *textureSurface = SDL_CreateSurfaceFrom(outTexture->width, outTexture->height,
                                                        SDL_PIXELFORMAT_RGBA32, outTexture->textureData,
                                                        outTexture->width * 4);

    InvertAlpha(0, 0, spriteWidth * 2, fontHeight * 2 + 6);

    finalCopyDst.x = 0;
    finalCopyDst.y = yPos;
    finalCopyDst.w = spriteWidth;
    finalCopyDst.h = 16;

    if (!SDL_StretchSurface(g_TextBufferSurface, &finalCopySrc, textureSurface, &finalCopyDst,
                            SDL_SCALEMODE_LINEAR))
    {
        SDL_Log("SDL_BlitScaled failed! Error: %s", SDL_GetError());
    }

    g_AnmManager->SetCurrentTexture(outTexture->handle);

    g_GfxBackend->SetTextureImage(outTexture->width, outTexture->height, PIXEL_RGBA, PIXEL_UNSIGNED_BYTE,
                                  outTexture->textureData);

    SDL_DestroySurface(textureSurface);

    return;
}

// Extended to free all globals for text helper
void TextHelper::ReleaseTextBuffer()
{
    ClearLocalizedFontSizeCache();
#if defined(_WIN32) && defined(TH_ENABLE_THCRAP)
    if (g_LocalizedFontResource != nullptr)
    {
        RemoveFontMemResourceEx(g_LocalizedFontResource);
        g_LocalizedFontResource = nullptr;
    }
    g_LocalizedGdiFontName.clear();
    if (g_LocalizedTextDc != nullptr)
    {
        if (g_LocalizedTextBitmapOriginal != nullptr)
            SelectObject(g_LocalizedTextDc, g_LocalizedTextBitmapOriginal);
        DeleteDC(g_LocalizedTextDc);
        g_LocalizedTextDc = nullptr;
    }
    if (g_LocalizedTextBitmap != nullptr)
    {
        DeleteObject(g_LocalizedTextBitmap);
        g_LocalizedTextBitmap = nullptr;
        g_LocalizedTextPixels = nullptr;
    }
#endif
    if (g_Font != NULL)
    {
        TTF_ClearFallbackFonts(g_Font);
        TTF_CloseFont(g_Font);
        g_Font = NULL;
    }
    if (g_LocalizedFallbackFont != nullptr)
    {
        TTF_CloseFont(g_LocalizedFallbackFont);
        g_LocalizedFallbackFont = nullptr;
    }
    std::free(g_LocalizedFontData);
    g_LocalizedFontData = nullptr;
    g_LocalizedFontDataSize = 0;

    if (g_TextBufferSurface != NULL)
    {
        SDL_DestroySurface(g_TextBufferSurface);
        g_TextBufferSurface = NULL;
    }

    return;
}
