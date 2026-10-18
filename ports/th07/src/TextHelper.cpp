#include "TextHelper.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "AnmManager.hpp"
#include "FileSystem.hpp"
#include "GameErrorContext.hpp"
#include "Localization.hpp"
#include "Supervisor.hpp"
#include "graphics/ZunGraphics.hpp"
#include "inttypes.hpp"
#include "sj2utf8/sj2utf8.h"

static TTF_Font *g_Font = nullptr;
static TTF_Font *g_OriginalFallbackFont = nullptr;
static u8 *g_LocalizedFontData = nullptr;
static std::array<float, 65> g_LocalizedPrimaryPointSize{};
static std::array<float, 65> g_LocalizedFallbackPointSize{};

static void ClearLocalizedFontSizeCache()
{
    g_LocalizedPrimaryPointSize.fill(0.0f);
    g_LocalizedFallbackPointSize.fill(0.0f);
}

static bool FontRasterFitsCell(TTF_Font *font, i32 targetHeight)
{
    if (font == nullptr || TTF_GetFontHeight(font) <= 0 || TTF_GetFontHeight(font) > targetHeight)
        return false;

    // TH07 draws the main glyph at y=2 and copies sourceHeight=target+2.
    // Therefore the bottom-most source glyph pixel must be < targetHeight.
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
    return bottomInkRow >= 0 && bottomInkRow < targetHeight;
}

static bool SetFontCellHeight(TTF_Font *font, i32 targetHeight, float *cachedPointSize)
{
    if (font == nullptr || targetHeight <= 0)
        return false;
    if (cachedPointSize != nullptr && *cachedPointSize > 0.0f)
        return TTF_SetFontSize(font, *cachedPointSize);

    float low = 1.0f;
    float high = static_cast<float>(targetHeight * 2);
    float best = low;
    for (int iteration = 0; iteration < 16; iteration++)
    {
        const float candidate = (low + high) * 0.5f;
        if (!TTF_SetFontSize(font, candidate))
            return false;
        if (FontRasterFitsCell(font, targetHeight))
        {
            best = candidate;
            low = candidate;
        }
        else
        {
            high = candidate;
        }
    }
    if (!TTF_SetFontSize(font, best) || !FontRasterFitsCell(font, targetHeight))
        return false;
    if (cachedPointSize != nullptr)
        *cachedPointSize = best;
    return true;
}

static void SetTextFontSize(i32 fontSize)
{
    if (Localization::Active() && fontSize > 0 && fontSize < static_cast<i32>(g_LocalizedPrimaryPointSize.size()))
    {
        if (!SetFontCellHeight(g_Font, fontSize,
                               &g_LocalizedPrimaryPointSize[static_cast<std::size_t>(fontSize)]))
            TTF_SetFontSize(g_Font, static_cast<float>(fontSize));
        if (g_OriginalFallbackFont != nullptr &&
            !SetFontCellHeight(g_OriginalFallbackFont, fontSize,
                               &g_LocalizedFallbackPointSize[static_cast<std::size_t>(fontSize)]))
            TTF_SetFontSize(g_OriginalFallbackFont, static_cast<float>(fontSize));
        return;
    }
    TTF_SetFontSize(g_Font, static_cast<float>(fontSize));
    if (g_OriginalFallbackFont != nullptr)
        TTF_SetFontSize(g_OriginalFallbackFont, static_cast<float>(fontSize));
}

// stolen from
// https://stackoverflow.com/questions/3404199/how-to-find-out-the-encoding-of-a-file-c-sharp/3404317#3404317
bool IsUtf8(const char *string)
{
    i32 charByteCounter = 1;
    unsigned char curByte;

    size_t len = strlen(string);
    for (size_t i = 0; i < len; i++)
    {
        curByte = string[i];
        if (charByteCounter == 1)
        {
            if (curByte >= 0x80)
            {
                while (((curByte <<= 1) & 0x80) != 0)
                {
                    charByteCounter++;
                }
                if (charByteCounter == 1 || charByteCounter > 6)
                {
                    return false;
                }
            }
        }
        else
        {
            if ((curByte & 0xC0) != 0x80)
            {
                return false;
            }
            charByteCounter--;
        }
    }
    if (charByteCounter > 1)
    {
        return false;
    }

    return true;
}

namespace
{
struct LayoutToken
{
    bool isCommand = false;
    std::string text;
    std::vector<std::string> params;
};

struct LayoutFragment
{
    std::string text;
    i32 xFull = 0;
};

// thcrap_tsa/layout.cpp keeps Layout_Tabs alive until module shutdown. Keep
// the same full-resolution 2x text-bitmap coordinate system here so odd tab
// positions preserve GDI's half-game-pixel precision.
static std::vector<i32> g_LayoutTabsFull;

static std::string PrepareLayoutText(const std::string &input)
{
    std::string noTabs;
    noTabs.reserve(input.size());
    for (char ch : input)
    {
        // GetTextExtentPoint32A/TextOutA treat U+0009 as zero-width in the
        // TH07 GDI path. Stats translations use it only as a visual separator.
        if (ch != '\t')
            noTabs.push_back(ch);
    }
    if (noTabs.empty() || IsUtf8(noTabs.c_str()))
        return noTabs;
    const std::string converted = sj2utf8(noTabs.c_str());
    return converted.empty() ? noTabs : converted;
}

static i32 MeasureLayoutTextFull(const std::string &input)
{
    if (g_Font == nullptr)
        return 0;
    const std::string text = PrepareLayoutText(input);
    if (text.empty())
        return 0;
    int width = 0;
    int height = 0;
    if (!TTF_GetStringSize(g_Font, text.c_str(), 0, &width, &height))
        return 0;
    return width;
}

// Equivalent to layout_match(): split only top-level '$' delimiters and
// include the closing '>' in consumed. A token needs at least one '$'.
static bool MatchLayoutToken(const std::string &str, std::size_t offset,
                             std::size_t &consumed, std::vector<std::string> &params)
{
    params.clear();
    consumed = str.size() - offset;
    if (offset >= str.size() || str[offset] != '<')
        return false;

    std::size_t i = offset + 1;
    std::size_t argStart = i;
    int nesting = 0;
    for (; i < str.size() && nesting >= 0; ++i)
    {
        const char ch = str[i];
        nesting += ch == '<';
        nesting -= ch == '>';
        if ((nesting == 0 && ch == '$') || (nesting == -1 && ch == '>'))
        {
            params.emplace_back(str.substr(argStart, i - argStart));
            argStart = i + 1;
        }
    }
    consumed = argStart - offset;
    return params.size() > 1;
}

static std::vector<LayoutToken> TokenizeLayout(const char *string)
{
    std::vector<LayoutToken> tokens;
    if (string == nullptr || string[0] == '\0')
        return tokens;

    const std::string str(string);
    std::size_t offset = 0;
    while (offset < str.size())
    {
        std::size_t consumed = str.size() - offset;
        std::vector<std::string> params;
        if (MatchLayoutToken(str, offset, consumed, params))
        {
            LayoutToken token;
            token.isCommand = true;
            token.params = std::move(params);
            tokens.emplace_back(std::move(token));
            offset += consumed;
            continue;
        }

        const std::size_t next = str.find('<', offset + 1);
        const std::size_t end = next == std::string::npos ? str.size() : next;
        if (end > offset)
        {
            LayoutToken token;
            token.text = str.substr(offset, end - offset);
            tokens.emplace_back(std::move(token));
        }
        offset = end;
    }
    return tokens;
}

static bool BuildLayoutFragments(const char *string, i32 bitmapWidthFull,
                                 std::vector<i32> &tabsFull,
                                 std::vector<LayoutFragment> &fragments)
{
    fragments.clear();
    const std::vector<LayoutToken> tokens = TokenizeLayout(string);
    if (tokens.empty())
        return false;

    bool hasLayoutCommand = false;
    for (const LayoutToken &token : tokens)
        hasLayoutCommand |= token.isCommand;
    if (!hasLayoutCommand)
        return false;

    i32 curX = 0;
    std::size_t curTab = 0;
    for (std::size_t tokenIndex = 0; tokenIndex < tokens.size(); ++tokenIndex)
    {
        const LayoutToken &token = tokens[tokenIndex];
        if (!token.isCommand)
        {
            const i32 width = MeasureLayoutTextFull(token.text);
            if (!token.text.empty())
                fragments.push_back({token.text, curX});
            curX += width;
            continue;
        }

        const std::string &cmd = token.params[0];
        const std::string &drawText = token.params[1];
        i32 curWidth = MeasureLayoutTextFull(drawText);
        const std::size_t tabsCount = tabsFull.size();
        i32 tabEnd;
        if (token.params.size() > 2)
        {
            tabEnd = token.params[2].empty()
                         ? bitmapWidthFull
                         : curX + MeasureLayoutTextFull(token.params[2]);
        }
        else if (curTab < tabsCount)
        {
            tabEnd = tabsFull[curTab];
        }
        else if (tabsCount > 0 && tokenIndex == tokens.size() - 1)
        {
            tabEnd = bitmapWidthFull;
        }
        else
        {
            tabEnd = curX + curWidth;
        }

        std::size_t recognized = cmd.size();
        bool draw = true;
        for (char command : cmd)
        {
            switch (command)
            {
            case 's':
                tabEnd = curX;
                draw = false;
                break;
            case 't':
            {
                i32 widest = curWidth;
                for (std::size_t index = 2; index < token.params.size(); ++index)
                    widest = std::max(widest, MeasureLayoutTextFull(token.params[index]));
                tabEnd = curX + widest;
                if (curTab >= tabsFull.size())
                    tabsFull.resize(curTab + 1, 0);
                tabsFull[curTab] = tabEnd;
                break;
            }
            case 'l':
                break;
            case 'c':
                curX += ((tabEnd - curX) / 2) - (curWidth / 2);
                break;
            case 'r':
                curX = tabEnd - curWidth;
                break;
            default:
                if (recognized > 0)
                    --recognized;
                break;
            }
        }
        if (recognized != 0)
        {
            ++curTab;
            curWidth = tabEnd - curX;
        }
        if (draw && !drawText.empty())
            fragments.push_back({drawText, curX});
        curX += curWidth;
    }
    return true;
}

static void RenderLayoutFragments(TextHelper &textHelper,
                                  const std::vector<LayoutFragment> &fragments,
                                  i32 xPos, u32 textColor, u32 outlineType)
{
    SDL_Color white = {255, 255, 255, 255};
    auto drawPass = [&](u8 r, u8 g, u8 b, i32 offsetXFull, i32 offsetYFull) {
        for (const LayoutFragment &fragment : fragments)
        {
            const std::string text = PrepareLayoutText(fragment.text);
            if (text.empty())
                continue;
            SDL_Surface *surface = TTF_RenderText_Blended(g_Font, text.c_str(), 0, white);
            if (surface == nullptr)
                continue;
            SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
            SDL_SetSurfaceColorMod(surface, r, g, b);
            SDL_Rect dstRect = {xPos * 2 + offsetXFull + fragment.xFull,
                                offsetYFull, surface->w, surface->h};
            SDL_BlitSurface(surface, NULL, textHelper.buffer, &dstRect);
            SDL_DestroySurface(surface);
        }
    };

    if (outlineType != 0xffffffff)
    {
        static constexpr i32 dx[4] = {4, 0, 2, 2};
        static constexpr i32 dy[4] = {2, 2, 0, 4};
        for (i32 index = 0; index < 4; ++index)
            drawPass(0, 0, 0, dx[index], dy[index]);
    }
    else
    {
        static constexpr i32 dx[4] = {3, 1, 2, 2};
        static constexpr i32 dy[4] = {2, 2, 1, 3};
        for (i32 index = 0; index < 4; ++index)
            drawPass(0, 0, 0, dx[index], dy[index]);
    }
    drawPass((textColor >> 16) & 0xff, (textColor >> 8) & 0xff, textColor & 0xff, 2, 2);
}
} // namespace

TextHelper::TextHelper()
{
    this->buffer = NULL;
    this->width = 0;
    this->height = 0;
}

TextHelper::~TextHelper()
{
    ReleaseBuffer();
}

bool TextHelper::ReleaseBuffer()
{
    if (this->buffer)
    {
        SDL_DestroySurface(this->buffer);
        this->buffer = NULL;
        this->width = 0;
        this->height = 0;
        return true;
    }
    return false;
}

bool TextHelper::AllocateBuffer(i32 width, i32 height)
{
    ReleaseBuffer();
    this->buffer = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
    if (!this->buffer)
    {
        return false;
    }
    SDL_FillSurfaceRect(this->buffer, NULL, 0);
    this->width = width;
    this->height = height;
    return true;
}

bool TextHelper::InvertAlpha(i32 x, i32 y, i32 spriteWidth, i32 fontHeight, i32 param5)
{
    i32 doubleArea = spriteWidth * fontHeight * 2;
    if (doubleArea == 0 || !this->buffer)
    {
        return false;
    }

    SDL_LockSurface(this->buffer);
    u8 *pixels = (u8 *)this->buffer->pixels;
    i32 pitch = this->buffer->pitch;

    for (i32 py = 0; py < fontHeight; py++)
    {
        for (i32 px = 0; px < spriteWidth; px++)
        {
            u8 *p = &pixels[(py + y) * pitch + (px + x) * 4];
            u8 r = p[0];
            u8 g = p[1];
            u8 b = p[2];
            u8 a = p[3];

            if (a > 0)
            {
                i32 i = (py * spriteWidth + px) * 2;

                if (!param5)
                {
                    if (r >= b)
                    {
                        r = r - (r * i * 2) / doubleArea / 3;
                        g = g - (g * i * 2) / doubleArea / 3;
                    }
                    else
                    {
                        b = b - (b * i) / doubleArea / 2;
                        g = g - (g * i) / doubleArea / 2;
                    }
                }
                else
                {
                    if (r >= b)
                    {
                        r = r - (r * i) / doubleArea / 4;
                        g = g - (g * i) / doubleArea / 4;
                    }
                    else
                    {
                        b = b - (b * i) / doubleArea / 4;
                        g = g - (g * i) / doubleArea / 4;
                    }
                }

                p[0] = r;
                p[1] = g;
                p[2] = b;
                p[3] = a;
            }
            else
            {
                p[0] = 0;
                p[1] = 0;
                p[2] = 0;
                p[3] = 0;
            }
        }
    }

    SDL_UnlockSurface(this->buffer);
    return true;
}

bool TextHelper::CopyTextToTexture(i32 yPos, i32 spriteWidth, i32 spriteHeight, i32 fontHeight,
                                   i32 fontWidth, GfxTextureHandle outTexture)
{
    SDL_Surface *outSurface = SDL_CreateSurface(spriteWidth, spriteHeight, SDL_PIXELFORMAT_RGBA32);
    if (!outSurface)
    {
        return false;
    }
    SDL_FillSurfaceRect(outSurface, NULL, 0);

    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = spriteWidth * 2;
    srcRect.h = fontHeight * 2;
    if (srcRect.w > this->width)
    {
        srcRect.w = this->width;
    }
    if (srcRect.h > this->height)
    {
        srcRect.h = this->height;
    }

    SDL_Rect dstRect;
    dstRect.x = 0;
    dstRect.y = 0;
    dstRect.w = spriteWidth;
    dstRect.h = fontWidth;

    SDL_StretchSurface(this->buffer, &srcRect, outSurface, &dstRect, SDL_SCALEMODE_LINEAR);

    // Text rendering is an out-of-band texture mutation. The GL binding may
    // have been changed by a loader/patch path that bypasses AnmManager's draw
    // cache, so never trust cached equality here. Finish the old batch first,
    // perform the real bind unconditionally, then synchronize the software
    // cache before glTexSubImage2D. This prevents font uploads from landing in
    // a title/menu texture when GL reuses a recently deleted texture name.
    g_AnmManager->Flush();
    g_Supervisor.gfxDevice->BindTexture(outTexture);
    g_AnmManager->SetTexture(outTexture);
    g_Supervisor.gfxDevice->SetTextureSubImage(0, yPos, outSurface->w, fontWidth,
                                               outSurface->pixels);
    SDL_DestroySurface(outSurface);
    return true;
}

ZunResult TextHelper::CreateTextBuffer()
{
    if (g_Font)
    {
        return ZUN_SUCCESS;
    }

    ClearLocalizedFontSizeCache();
    if (!TTF_Init())
    {
        g_GameErrorContext.Log("TTF_Init fail : %s\n", SDL_GetError());
        return ZUN_ERROR;
    }

    const char *fontName = Localization::Active() ? "unifont.otf" : "msgothic.ttc";
    g_Font = TTF_OpenFont(FileSystem::GetBasePath(fontName).c_str(), 10);
    if (!g_Font)
    {
        g_GameErrorContext.Log("TTF_OpenFont fail : %s\n", SDL_GetError());
        return ZUN_ERROR;
    }
    TTF_SetFontStyle(g_Font, TTF_STYLE_BOLD);
    return ZUN_SUCCESS;
}

#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
bool TextHelper::DebugLocalizedFontMetricsSelfTest()
{
    FILE *auditFile = std::fopen("thcrap-font-selftest.txt", "wb");
    auto finish = [auditFile](bool result) {
        if (auditFile != nullptr)
            std::fclose(auditFile);
        return result;
    };

    if (!Localization::Active())
    {
        if (auditFile != nullptr)
            std::fputs("th07 thcrap font metrics self-test: localization inactive\n", auditFile);
        return finish(false);
    }
    if (CreateTextBuffer() != ZUN_SUCCESS || g_Font == nullptr)
    {
        if (auditFile != nullptr)
            std::fputs("th07 thcrap font metrics self-test: CreateTextBuffer failed\n", auditFile);
        return finish(false);
    }

    static constexpr const char *sample = "gypqj";
    bool passed = true;
    for (const i32 logicalHeight : {15, 16})
    {
        const i32 targetCellHeight = logicalHeight * 2 - 2;
        const i32 sourceHeight = logicalHeight * 2;
        SetTextFontSize(targetCellHeight);

        const i32 cellHeight = TTF_GetFontHeight(g_Font);
        const i32 ascent = TTF_GetFontAscent(g_Font);
        const i32 descent = TTF_GetFontDescent(g_Font);
        const i32 fallbackCell = g_OriginalFallbackFont != nullptr
                                     ? TTF_GetFontHeight(g_OriginalFallbackFont)
                                     : 0;
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

        // The original function draws the main glyph at y=2 and copies only
        // 2*logicalHeight rows. Keep exactly that contract.
        const bool primaryFits = cellHeight > 0 && cellHeight <= targetCellHeight &&
                                 surface->h <= targetCellHeight;
        const bool fallbackFits = g_OriginalFallbackFont == nullptr ||
                                  (fallbackCell > 0 && fallbackCell <= targetCellHeight);
        const bool descenderVisible = bottomInkRow >= 0 && bottomInkRow + 2 < sourceHeight;
        const bool rowPassed = primaryFits && fallbackFits && descenderVisible;

        SDL_Log("th07 thcrap SDL_ttf font metrics: logical=%d targetCell=%d source=%d "
                "point=%.3f cell=%d ascent=%d descent=%d fallbackCell=%d surface=%dx%d "
                "bottomInk=%d %s",
                logicalHeight, targetCellHeight, sourceHeight, TTF_GetFontSize(g_Font),
                cellHeight, ascent, descent, fallbackCell, surface->w, surface->h,
                bottomInkRow, rowPassed ? "PASS" : "FAIL");
        if (auditFile != nullptr)
        {
            std::fprintf(auditFile,
                         "logical=%d targetCell=%d source=%d point=%.3f cell=%d ascent=%d "
                         "descent=%d fallbackCell=%d surface=%dx%d bottomInk=%d %s\n",
                         logicalHeight, targetCellHeight, sourceHeight, TTF_GetFontSize(g_Font),
                         cellHeight, ascent, descent, fallbackCell, surface->w, surface->h,
                         bottomInkRow, rowPassed ? "PASS" : "FAIL");
            std::fflush(auditFile);
        }
        passed = passed && rowPassed;
        SDL_DestroySurface(surface);
    }

    ReleaseTextBuffer();
    return finish(passed);
}
#endif

void TextHelper::ReleaseTextBuffer()
{
    ClearLocalizedFontSizeCache();
    if (g_Font != nullptr)
    {
        TTF_ClearFallbackFonts(g_Font);
        TTF_CloseFont(g_Font);
    }
    g_Font = nullptr;
    if (g_OriginalFallbackFont != nullptr)
    {
        TTF_CloseFont(g_OriginalFallbackFont);
        g_OriginalFallbackFont = nullptr;
    }
    std::free(g_LocalizedFontData);
    g_LocalizedFontData = nullptr;
    TTF_Quit();
}

void TextHelper::RenderTextToTextureBold(i32 xPos, i32 yPos, i32 spriteWidth, i32 spriteHeight,
                                         i32 fontHeight, i32 fontWidth, u32 textColor,
                                         u32 outlineType, char *string, GfxTextureHandle outTexture)
{
    // TH07 0x004322a3 calls CreateFontA(fontHeight*2-2, ..., FW_BOLD, ...).
    // SDL_ttf's numeric size is a point/em size rather than GDI's positive
    // cell height, so SetTextFontSize() calibrates localized fonts to this
    // exact GDI target instead of relying on the old font-specific +6 guess.
    i32 fontSize = fontHeight * 2 - 2;
    if (fontSize <= 0)
    {
        return;
    }

    if (CreateTextBuffer() != ZUN_SUCCESS)
    {
        return;
    }

    SetTextFontSize(fontSize);

    std::vector<LayoutFragment> layoutFragments;
    if (Localization::Active() &&
        BuildLayoutFragments(string, 1024, g_LayoutTabsFull, layoutFragments))
    {
        i32 dWidth = spriteWidth * 2;
        i32 dHeight = fontHeight * 2 + 6;
        if (dWidth > 1024)
            dWidth = 1024;
        if (dHeight > 64)
            dHeight = 64;
        if (dWidth <= 0 || dHeight <= 0)
            return;

        TextHelper textHelper;
        textHelper.AllocateBuffer(dWidth, dHeight);
        RenderLayoutFragments(textHelper, layoutFragments, xPos, textColor, outlineType);
        textHelper.InvertAlpha(0, 0, spriteWidth << 1, fontHeight * 2 + 6,
                               (u32)(outlineType == 0xffffffff));
        textHelper.CopyTextToTexture(yPos, spriteWidth, spriteHeight, fontHeight, fontWidth,
                                     outTexture);
#ifdef TH_DEV_TOOLS
        static bool loggedLayout = false;
        if (!loggedLayout)
        {
            SDL_Log("th07 thcrap layout: fragments=%zu tabs=%zu text=%s",
                    layoutFragments.size(), g_LayoutTabsFull.size(), string);
            loggedLayout = true;
        }
        static i32 layoutAuditCalls = 0;
        if (layoutAuditCalls < 32)
        {
            const i32 call = layoutAuditCalls++;
            SDL_Log("th07 thcrap layout audit: call=%d tabs=%zu tab0=%d fragments=%zu",
                    call, g_LayoutTabsFull.size(),
                    g_LayoutTabsFull.empty() ? -1 : g_LayoutTabsFull[0],
                    layoutFragments.size());
            for (std::size_t index = 0; index < layoutFragments.size(); ++index)
            {
                SDL_Log("th07 thcrap layout fragment: call=%d index=%zu xFull=%d text=%s",
                        call, index, layoutFragments[index].xFull,
                        layoutFragments[index].text.c_str());
            }
        }
#endif
        return;
    }

    char *convStr = string;
    bool needsFree = false;
    if (!IsUtf8(string))
    {
        std::string tmp = sj2utf8(string);
        if (!tmp.empty())
        {
            convStr = SDL_strdup(tmp.c_str());
            needsFree = true;
        }
    }

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *textSurf = TTF_RenderText_Blended(g_Font, convStr, 0, white);
    if (needsFree)
    {
        SDL_free(convStr);
    }
    if (!textSurf)
    {
        return;
    }

    i32 dWidth = spriteWidth * 2;
    i32 dHeight = fontHeight * 2 + 6;
    if (dWidth > 1024)
    {
        dWidth = 1024;
    }
    if (dHeight > 64)
    {
        dHeight = 64;
    }
    if (dWidth <= 0 || dHeight <= 0)
    {
        SDL_DestroySurface(textSurf);
        return;
    }

    TextHelper textHelper;
    textHelper.AllocateBuffer(dWidth, dHeight);

    SDL_SetSurfaceBlendMode(textSurf, SDL_BLENDMODE_BLEND);

    SDL_SetSurfaceColorMod(textSurf, 0, 0, 0);
    SDL_Rect dstRect;
    if (outlineType != 0xffffffff)
    {
        i32 dx[4] = {4, 0, 2, 2};
        i32 dy[4] = {2, 2, 0, 4};
        for (i32 i = 0; i < 4; i++)
        {
            dstRect = {xPos * 2 + dx[i], dy[i], textSurf->w, textSurf->h};
            SDL_BlitSurface(textSurf, NULL, textHelper.buffer, &dstRect);
        }
    }
    else
    {
        i32 dx[4] = {3, 1, 2, 2};
        i32 dy[4] = {2, 2, 1, 3};
        for (i32 i = 0; i < 4; i++)
        {
            dstRect = {xPos * 2 + dx[i], dy[i], textSurf->w, textSurf->h};
            SDL_BlitSurface(textSurf, NULL, textHelper.buffer, &dstRect);
        }
    }

    u8 r = (textColor >> 16) & 0xFF;
    u8 g = (textColor >> 8) & 0xFF;
    u8 b_col = textColor & 0xFF;
    SDL_SetSurfaceColorMod(textSurf, r, g, b_col);
    dstRect = {xPos * 2 + 2, 2, textSurf->w, textSurf->h};
    SDL_BlitSurface(textSurf, NULL, textHelper.buffer, &dstRect);

    SDL_DestroySurface(textSurf);

    textHelper.InvertAlpha(0, 0, spriteWidth << 1, fontHeight * 2 + 6,
                           (u32)(outlineType == 0xffffffff));
    textHelper.CopyTextToTexture(yPos, spriteWidth, spriteHeight, fontHeight, fontWidth,
                                 outTexture);
}

float TextHelper::MeasureTextWidth(const char *string, i32 fontId)
{
    if (string == nullptr || fontId <= 0 || CreateTextBuffer() != ZUN_SUCCESS || g_Font == nullptr)
        return 0.0f;

    // For TH06-TH09, thcrap's GetTextExtentForFontID() receives the game's
    // internal text-height ID. TH07's font-cache formula converts that ID to
    // CreateFont(height = id*2-2). Use the exact same calibrated SDL_ttf font
    // that RenderTextToTextureBold() will rasterize, then divide the 2x text
    // surface width back into game coordinates just like thcrap's extent API.
    const i32 targetCellHeight = fontId * 2 - 2;
    if (targetCellHeight <= 0)
        return 0.0f;
    SetTextFontSize(targetCellHeight);

    std::string converted;
    const char *text = string;
    if (!IsUtf8(string))
    {
        converted = sj2utf8(string);
        if (!converted.empty())
            text = converted.c_str();
    }

    int width = 0;
    int height = 0;
    if (!TTF_GetStringSize(g_Font, text, 0, &width, &height))
        return 0.0f;
    return width / 2.0f;
}

#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
bool TextHelper::DebugLayoutSelfTest()
{
    if (CreateTextBuffer() != ZUN_SUCCESS || g_Font == nullptr)
        return false;
    SetTextFontSize(28);

    std::vector<i32> tabs;
    std::vector<LayoutFragment> fragments;
    if (!BuildLayoutFragments("<tl$Short$Longest candidate> tail", 1024, tabs, fragments) ||
        tabs.size() != 1 || fragments.size() != 2)
        return false;
    const i32 expectedTab = std::max(MeasureLayoutTextFull("Short"),
                                     MeasureLayoutTextFull("Longest candidate"));
    if (tabs[0] != expectedTab || fragments[0].xFull != 0 ||
        fragments[1].xFull != expectedTab)
        return false;

    const i32 persistentTab = tabs[0];
    if (!BuildLayoutFragments("<l$A> B", 1024, tabs, fragments) ||
        tabs.size() != 1 || fragments.size() != 2 ||
        fragments[0].xFull != 0 || fragments[1].xFull != persistentTab)
        return false;

    std::vector<i32> isolated;
    if (!BuildLayoutFragments("<c$ABC$>", 1024, isolated, fragments) || fragments.size() != 1)
        return false;
    const i32 abcWidth = MeasureLayoutTextFull("ABC");
    if (fragments[0].xFull != 512 - abcWidth / 2)
        return false;
    if (!BuildLayoutFragments("<r$ABC$>", 1024, isolated, fragments) ||
        fragments.size() != 1 || fragments[0].xFull != 1024 - abcWidth)
        return false;
    if (!BuildLayoutFragments("<s$Hidden>", 1024, isolated, fragments) || !fragments.empty())
        return false;

    if (BuildLayoutFragments("<ordinary text>", 1024, isolated, fragments))
        return false;
    if (MeasureLayoutTextFull("A\tB") != MeasureLayoutTextFull("AB"))
        return false;
    return true;
}
#endif

i32 TextHelper::GetLogicalStringWidth(const char *str)
{
    if (!IsUtf8(str))
    {
        return strlen(str);
    }

    i32 width = 0;
    while (*str)
    {
        if ((*str & 0x80) == 0)
        {
            width += 1;
            str += 1;
        }
        else if ((*str & 0xE0) == 0xC0)
        {
            width += 2;
            str += 2;
        }
        else if ((*str & 0xF0) == 0xE0)
        {
            width += 2;
            str += 3;
        }
        else if ((*str & 0xF8) == 0xF0)
        {
            width += 2;
            str += 4;
        }
        else
        {
            str++;
        }
    }
    return width;
}
