#include "AnmManager.hpp"
#include "AnmIdx.hpp"
#include "FileSystem.hpp"
#include "GameErrorContext.hpp"
#include "GameWindow.hpp"
#include "Localization.hpp"
#include "Rng.hpp"
#include "Supervisor.hpp"
#include "TextHelper.hpp"
#include <algorithm>
#include <cmath>
#include "ZunMath.hpp"
#include "i18n.hpp"
#include "utils.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

static VertexTex1DiffuseXyzrhw g_PrimitivesToDrawVertexBuf[4];
static VertexTex1DiffuseXyzrhw g_PrimitivesToDrawNoVertexBuf[4];
static VertexTex1DiffuseXyz g_PrimitivesToDrawUnknown[4];
AnmManager *g_AnmManager;

static ZunColor LerpColor(ZunColor from, ZunColor to, f32 amount)
{
    ZunColor result = 0;
    for (i32 component = 0; component < 4; component++)
    {
        const f32 value = COLOR_GET_COMPONENT(from, component) * (1.0f - amount) +
                          COLOR_GET_COMPONENT(to, component) * amount;
        COLOR_SET_COMPONENT(result, component, static_cast<u8>(value));
    }
    return result;
}

static const SDL_PixelFormat g_TextureFormatSDLMapping[6] = {SDL_PIXELFORMAT_UNKNOWN,  SDL_PIXELFORMAT_RGBA32,
                                                                 SDL_PIXELFORMAT_RGBA5551, SDL_PIXELFORMAT_RGB565,
                                                                 SDL_PIXELFORMAT_RGB24,    SDL_PIXELFORMAT_RGBA4444};

static const PixelFormat g_TextureFormatTypeGfxMapping[6] = {
    static_cast<PixelFormat>(0), PIXEL_RGBA, PIXEL_RGBA, PIXEL_RGB, PIXEL_RGB, PIXEL_RGBA};

static const PixelDataType g_TextureFormatTypeMapping[6] = {static_cast<PixelDataType>(0), // ugh
                                                            PIXEL_UNSIGNED_BYTE,           PIXEL_UNSIGNED_SHORT_5_5_5_1,
                                                            PIXEL_UNSIGNED_SHORT_5_6_5,    PIXEL_UNSIGNED_BYTE,
                                                            PIXEL_UNSIGNED_SHORT_4_4_4_4};

static const u8 g_TextureFormatBytesPerPixel[6] = {0, 4, 2, 2, 3, 2};

namespace
{
constexpr i32 SPRITE_EXTRUSION_GUTTER = 1;
constexpr u32 SPRITE_EXTRUSION_MAX_ATLAS_SIZE = 2048;

struct SpriteExtrusionRect
{
    i32 srcX;
    i32 srcY;
    i32 width;
    i32 height;
    i32 dstX;
    i32 dstY;
    std::vector<i32> spriteIndices;
};

static u32 NextPowerOfTwo(u32 value)
{
    if (value <= 1)
        return 1;
    value--;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    return value + 1;
}

static bool ToPixelCoordinate(f32 value, i32 &result)
{
    result = static_cast<i32>(std::lround(value));
    return std::fabs(value - static_cast<f32>(result)) <= 0.001f;
}

static bool TryPackSpriteExtrusionRects(std::vector<SpriteExtrusionRect> &rects, u32 atlasWidth,
                                        u32 &usedHeight)
{
    u32 x = 0;
    u32 y = 0;
    u32 rowHeight = 0;

    for (SpriteExtrusionRect &rect : rects)
    {
        const u32 packedWidth = static_cast<u32>(rect.width + SPRITE_EXTRUSION_GUTTER * 2);
        const u32 packedHeight = static_cast<u32>(rect.height + SPRITE_EXTRUSION_GUTTER * 2);
        if (packedWidth > atlasWidth || packedHeight > SPRITE_EXTRUSION_MAX_ATLAS_SIZE)
            return false;

        if (x + packedWidth > atlasWidth)
        {
            y += rowHeight;
            x = 0;
            rowHeight = 0;
        }
        if (y + packedHeight > SPRITE_EXTRUSION_MAX_ATLAS_SIZE)
            return false;

        rect.dstX = static_cast<i32>(x);
        rect.dstY = static_cast<i32>(y);
        x += packedWidth;
        rowHeight = std::max(rowHeight, packedHeight);
    }

    usedHeight = y + rowHeight;
    return usedHeight > 0 && usedHeight <= SPRITE_EXTRUSION_MAX_ATLAS_SIZE;
}

static bool BuildSpriteExtrusionAtlas(AnmManager *manager, i32 textureIdx,
                                      const std::vector<i32> &spriteIndices)
{
    if (!manager || textureIdx < 0 || textureIdx >= ARRAY_SIZE_SIGNED(manager->textures))
        return false;

    TextureData &sourceTexture = manager->textures[textureIdx];
    if (!sourceTexture.handle || !sourceTexture.textureData || sourceTexture.width == 0 ||
        sourceTexture.height == 0 || sourceTexture.format <= TEX_FMT_UNKNOWN ||
        sourceTexture.format > TEX_FMT_A4R4G4B4)
        return false;

    const u32 bytesPerPixel = g_TextureFormatBytesPerPixel[sourceTexture.format];
    if (bytesPerPixel == 0 || sourceTexture.width > SPRITE_EXTRUSION_MAX_ATLAS_SIZE ||
        sourceTexture.height > SPRITE_EXTRUSION_MAX_ATLAS_SIZE)
        return false;

    if (manager->spriteAtlasTextures[textureIdx])
    {
        if (manager->currentTextureHandle == manager->spriteAtlasTextures[textureIdx])
            manager->currentTextureHandle = 0;
        g_GfxBackend->DeleteTexture(manager->spriteAtlasTextures[textureIdx]);
        manager->spriteAtlasTextures[textureIdx] = 0;
    }

    std::vector<SpriteExtrusionRect> rects;
    rects.reserve(spriteIndices.size());
    for (const i32 spriteIdx : spriteIndices)
    {
        if (spriteIdx < 0 || spriteIdx >= ARRAY_SIZE_SIGNED(manager->sprites))
            continue;

        AnmLoadedSprite &sprite = manager->sprites[spriteIdx];
        sprite.extrudedUvStart = sprite.uvStart;
        sprite.extrudedUvEnd = sprite.uvEnd;
        sprite.hasExtrudedUv = false;
        if (sprite.sourceFileIndex != textureIdx)
            continue;

        i32 x0, y0, x1, y1;
        if (!ToPixelCoordinate(sprite.startPixelInclusive.x, x0) ||
            !ToPixelCoordinate(sprite.startPixelInclusive.y, y0) ||
            !ToPixelCoordinate(sprite.endPixelInclusive.x, x1) ||
            !ToPixelCoordinate(sprite.endPixelInclusive.y, y1))
            continue;

        const i32 width = x1 - x0;
        const i32 height = y1 - y0;
        if (width <= 0 || height <= 0 || x0 < 0 || y0 < 0 ||
            x1 > static_cast<i32>(sourceTexture.width) || y1 > static_cast<i32>(sourceTexture.height))
            continue;

        SpriteExtrusionRect *existing = nullptr;
        for (SpriteExtrusionRect &rect : rects)
        {
            if (rect.srcX == x0 && rect.srcY == y0 && rect.width == width && rect.height == height)
            {
                existing = &rect;
                break;
            }
        }
        if (existing)
        {
            existing->spriteIndices.push_back(spriteIdx);
        }
        else
        {
            SpriteExtrusionRect rect = {x0, y0, width, height, 0, 0, {spriteIdx}};
            rects.push_back(std::move(rect));
        }
    }

    if (rects.empty())
        return false;

    std::sort(rects.begin(), rects.end(), [](const SpriteExtrusionRect &a, const SpriteExtrusionRect &b) {
        if (a.height != b.height)
            return a.height > b.height;
        return a.width > b.width;
    });

    u32 widestPackedRect = 1;
    for (const SpriteExtrusionRect &rect : rects)
        widestPackedRect = std::max(widestPackedRect,
                                    static_cast<u32>(rect.width + SPRITE_EXTRUSION_GUTTER * 2));

    u32 atlasWidth = NextPowerOfTwo(std::max(sourceTexture.width, widestPackedRect));
    u32 usedHeight = 0;
    while (atlasWidth <= SPRITE_EXTRUSION_MAX_ATLAS_SIZE &&
           !TryPackSpriteExtrusionRects(rects, atlasWidth, usedHeight))
        atlasWidth <<= 1;

    if (atlasWidth > SPRITE_EXTRUSION_MAX_ATLAS_SIZE || usedHeight == 0)
        return false;

    const u32 atlasHeight = NextPowerOfTwo(usedHeight);
    if (atlasHeight > SPRITE_EXTRUSION_MAX_ATLAS_SIZE)
        return false;

    std::vector<u8> atlas(static_cast<size_t>(atlasWidth) * atlasHeight * bytesPerPixel, 0);
    const u8 *source = sourceTexture.textureData;
    const size_t sourcePitch = static_cast<size_t>(sourceTexture.width) * bytesPerPixel;
    const size_t atlasPitch = static_cast<size_t>(atlasWidth) * bytesPerPixel;

    for (const SpriteExtrusionRect &rect : rects)
    {
        const i32 contentX = rect.dstX + SPRITE_EXTRUSION_GUTTER;
        const i32 contentY = rect.dstY + SPRITE_EXTRUSION_GUTTER;
        const size_t copyBytes = static_cast<size_t>(rect.width) * bytesPerPixel;

        for (i32 row = 0; row < rect.height; row++)
        {
            const u8 *sourceRow = source + static_cast<size_t>(rect.srcY + row) * sourcePitch +
                                  static_cast<size_t>(rect.srcX) * bytesPerPixel;
            u8 *destinationRow = atlas.data() + static_cast<size_t>(contentY + row) * atlasPitch +
                                 static_cast<size_t>(contentX) * bytesPerPixel;
            std::memcpy(destinationRow, sourceRow, copyBytes);
            std::memcpy(destinationRow - bytesPerPixel, sourceRow, bytesPerPixel);
            std::memcpy(destinationRow + copyBytes, sourceRow + copyBytes - bytesPerPixel,
                        bytesPerPixel);
        }

        u8 *firstPackedRow = atlas.data() + static_cast<size_t>(contentY) * atlasPitch +
                             static_cast<size_t>(rect.dstX) * bytesPerPixel;
        u8 *lastPackedRow = atlas.data() + static_cast<size_t>(contentY + rect.height - 1) * atlasPitch +
                            static_cast<size_t>(rect.dstX) * bytesPerPixel;
        const size_t packedRowBytes = static_cast<size_t>(rect.width + 2 * SPRITE_EXTRUSION_GUTTER) *
                                      bytesPerPixel;
        std::memcpy(firstPackedRow - atlasPitch, firstPackedRow, packedRowBytes);
        std::memcpy(lastPackedRow + atlasPitch, lastPackedRow, packedRowBytes);
    }

    manager->CreateTextureObject();
    const GfxTextureHandle atlasTexture = manager->currentTextureHandle;
    g_GfxBackend->SetTextureImage(atlasWidth, atlasHeight,
                                  g_TextureFormatTypeGfxMapping[sourceTexture.format],
                                  g_TextureFormatTypeMapping[sourceTexture.format], atlas.data());
    if (g_GfxBackend->HasError())
    {
        g_GfxBackend->DeleteTexture(atlasTexture);
        manager->currentTextureHandle = 0;
        return false;
    }

    manager->spriteAtlasTextures[textureIdx] = atlasTexture;
    manager->currentSprite = nullptr;
    for (const SpriteExtrusionRect &rect : rects)
    {
        const f32 u0 = static_cast<f32>(rect.dstX + SPRITE_EXTRUSION_GUTTER) / atlasWidth;
        const f32 v0 = static_cast<f32>(rect.dstY + SPRITE_EXTRUSION_GUTTER) / atlasHeight;
        const f32 u1 = static_cast<f32>(rect.dstX + SPRITE_EXTRUSION_GUTTER + rect.width) / atlasWidth;
        const f32 v1 = static_cast<f32>(rect.dstY + SPRITE_EXTRUSION_GUTTER + rect.height) / atlasHeight;
        for (const i32 spriteIdx : rect.spriteIndices)
        {
            AnmLoadedSprite &sprite = manager->sprites[spriteIdx];
            sprite.extrudedUvStart = ZunVec2(u0, v0);
            sprite.extrudedUvEnd = ZunVec2(u1, v1);
            sprite.hasExtrudedUv = true;
        }
    }
    return true;
}

static bool UseSpriteExtrusionAtlas(const AnmManager *manager, const AnmLoadedSprite *sprite,
                                    const ZunVec2 &drawUv)
{
    return manager && sprite && sprite->hasExtrudedUv && drawUv.x == 0.0f && drawUv.y == 0.0f &&
           sprite->sourceFileIndex >= 0 &&
           sprite->sourceFileIndex < ARRAY_SIZE_SIGNED(manager->spriteAtlasTextures) &&
           static_cast<bool>(manager->spriteAtlasTextures[sprite->sourceFileIndex]);
}

static void InvalidateSpriteExtrusionAtlas(AnmManager *manager, i32 textureIdx)
{
    if (!manager || textureIdx < 0 || textureIdx >= ARRAY_SIZE_SIGNED(manager->spriteAtlasTextures))
        return;

    const GfxTextureHandle atlas = manager->spriteAtlasTextures[textureIdx];
    if (atlas)
    {
        if (manager->currentTextureHandle == atlas)
            manager->currentTextureHandle = 0;
        g_GfxBackend->DeleteTexture(atlas);
        manager->spriteAtlasTextures[textureIdx] = 0;
        manager->currentSprite = nullptr;
    }

    for (AnmLoadedSprite &sprite : manager->sprites)
    {
        if (sprite.sourceFileIndex == textureIdx)
            sprite.hasExtrudedUv = false;
    }
}

static void ResolveSpriteDrawSampling(const AnmManager *manager, const AnmLoadedSprite *sprite,
                                      const ZunVec2 &drawUv, ZunVec2 &uvStart, ZunVec2 &uvEnd,
                                      GfxTextureHandle &texture)
{
    const bool useExtrusion = UseSpriteExtrusionAtlas(manager, sprite, drawUv);
    if (useExtrusion)
    {
        uvStart = sprite->extrudedUvStart;
        uvEnd = sprite->extrudedUvEnd;
        texture = manager->spriteAtlasTextures[sprite->sourceFileIndex];
    }
    else
    {
        uvStart = ZunVec2(sprite->uvStart.x + drawUv.x, sprite->uvStart.y + drawUv.y);
        uvEnd = ZunVec2(sprite->uvEnd.x + drawUv.x, sprite->uvEnd.y + drawUv.y);
        texture = manager->textures[sprite->sourceFileIndex].handle;
    }
}
} // namespace

#ifdef __EMSCRIPTEN__
struct WebTransitionSurfaceCacheEntry
{
    const char *path;
    SDL_Surface *surface;
};

static WebTransitionSurfaceCacheEntry g_WebTransitionSurfaceCache[] = {
    {"data/title/title00.jpg", nullptr},
    {"data/title/select00.jpg", nullptr},
    {"data/result/music.jpg", nullptr},
    {"data/result/result.jpg", nullptr},
};

static u8 g_WebTransitionSurfaceOwner[32] = {};

static WebTransitionSurfaceCacheEntry *FindWebTransitionSurface(const char *path, u8 *ownerId = nullptr)
{
    for (u8 i = 0; i < static_cast<u8>(std::size(g_WebTransitionSurfaceCache)); i++)
    {
        if (std::strcmp(g_WebTransitionSurfaceCache[i].path, path) == 0)
        {
            if (ownerId)
                *ownerId = static_cast<u8>(i + 1);
            return &g_WebTransitionSurfaceCache[i];
        }
    }
    return nullptr;
}

struct WebTransitionAnmSpriteBinding
{
    i32 index;
    AnmLoadedSprite sprite;
};

struct WebTransitionAnmScriptBinding
{
    i32 index;
    const AnmRawInstr *script;
    i32 spriteIndex;
};

struct WebTransitionAnmCacheEntry
{
    const char *path;
    i32 anmIdx;
    i32 spriteIdxOffset;
    bool loaded = false;
    bool active = false;
    std::vector<WebTransitionAnmSpriteBinding> sprites;
    std::vector<WebTransitionAnmScriptBinding> scripts;
};

static WebTransitionAnmCacheEntry g_WebTransitionAnmCache[] = {
    {"data/title01.anm", ANM_FILE_TITLE01, ANM_OFFSET_TITLE01},
    {"data/title02.anm", ANM_FILE_TITLE02, ANM_OFFSET_TITLE02},
    {"data/title03.anm", ANM_FILE_TITLE03, ANM_OFFSET_TITLE03},
    {"data/title04.anm", ANM_FILE_TITLE04, ANM_OFFSET_TITLE04},
    {"data/title01s.anm", ANM_FILE_TITLE01S, ANM_OFFSET_TITLE01S},
    {"data/title04s.anm", ANM_FILE_TITLE04S, ANM_OFFSET_TITLE04S},
    {"data/select01.anm", ANM_FILE_SELECT01, ANM_OFFSET_SELECT01},
    {"data/select02.anm", ANM_FILE_SELECT02, ANM_OFFSET_SELECT02},
    {"data/select03.anm", ANM_FILE_SELECT03, ANM_OFFSET_SELECT03},
    {"data/select04.anm", ANM_FILE_SELECT04, ANM_OFFSET_SELECT04},
    {"data/select05.anm", ANM_FILE_SELECT05, ANM_OFFSET_SELECT05},
    {"data/slpl00a.anm", ANM_FILE_SLPL00A, ANM_OFFSET_SLPL00A},
    {"data/slpl00b.anm", ANM_FILE_SLPL00B, ANM_OFFSET_SLPL00B},
    {"data/slpl01a.anm", ANM_FILE_SLPL01A, ANM_OFFSET_SLPL01A},
    {"data/slpl01b.anm", ANM_FILE_SLPL01B, ANM_OFFSET_SLPL01B},
    {"data/replay00.anm", ANM_FILE_REPLAY, ANM_OFFSET_REPLAY},
    {"data/result00.anm", ANM_FILE_RESULT00, ANM_OFFSET_RESULT00},
    {"data/result01.anm", ANM_FILE_RESULT01, ANM_OFFSET_RESULT01},
    {"data/result02.anm", ANM_FILE_RESULT02, ANM_OFFSET_RESULT02},
    {"data/result03.anm", ANM_FILE_RESULT03, ANM_OFFSET_RESULT03},
    {"data/music00.anm", ANM_FILE_MUSIC00, ANM_OFFSET_MUSIC00},
    {"data/music01.anm", ANM_FILE_MUSIC01, ANM_OFFSET_MUSIC01},
    {"data/music02.anm", ANM_FILE_MUSIC02, ANM_OFFSET_MUSIC02},
};

static WebTransitionAnmCacheEntry *FindWebTransitionAnm(i32 anmIdx, const char *path = nullptr)
{
    for (WebTransitionAnmCacheEntry &entry : g_WebTransitionAnmCache)
    {
        if (entry.anmIdx == anmIdx && (!path || std::strcmp(entry.path, path) == 0))
            return &entry;
    }
    return nullptr;
}

static void CaptureWebTransitionAnmBindings(AnmManager *manager, WebTransitionAnmCacheEntry *entry)
{
    AnmRawEntry *anm = manager->anmFiles[entry->anmIdx];
    if (!anm)
        return;

    entry->sprites.clear();
    entry->scripts.clear();

    const LE<u32> *offset = anm->spriteOffsets;
    for (i32 i = 0; i < anm->numSprites; i++, offset++)
    {
        const AnmRawSprite *rawSprite = reinterpret_cast<const AnmRawSprite *>(
            reinterpret_cast<const u8 *>(anm) + *offset);
        const i32 target = rawSprite->id + entry->spriteIdxOffset;
        entry->sprites.push_back({target, manager->sprites[target]});
    }

    for (i32 i = 0; i < anm->numScripts; i++, offset += 2)
    {
        const i32 target = offset[0] + entry->spriteIdxOffset;
        entry->scripts.push_back({target, manager->scripts[target], manager->spriteIndices[target]});
    }

    entry->loaded = true;
    entry->active = true;
}

static void RestoreWebTransitionAnmBindings(AnmManager *manager, WebTransitionAnmCacheEntry *entry)
{
    for (const WebTransitionAnmSpriteBinding &binding : entry->sprites)
        manager->sprites[binding.index] = binding.sprite;
    for (const WebTransitionAnmScriptBinding &binding : entry->scripts)
    {
        manager->scripts[binding.index] = binding.script;
        manager->spriteIndices[binding.index] = binding.spriteIndex;
    }
    manager->anmFilesSpriteIndexOffsets[entry->anmIdx] = entry->spriteIdxOffset;
    entry->active = true;
}

static void ClearWebTransitionAnmBindings(AnmManager *manager, WebTransitionAnmCacheEntry *entry)
{
    for (const WebTransitionAnmSpriteBinding &binding : entry->sprites)
    {
        std::memset(&manager->sprites[binding.index], 0, sizeof(manager->sprites[binding.index]));
        manager->sprites[binding.index].sourceFileIndex = -1;
    }
    for (const WebTransitionAnmScriptBinding &binding : entry->scripts)
    {
        manager->scripts[binding.index] = nullptr;
        manager->spriteIndices[binding.index] = 0;
    }
    manager->anmFilesSpriteIndexOffsets[entry->anmIdx] = 0;
    entry->active = false;
}
#endif

enum class RuntimePatchAlphaState
{
    Empty,
    Opaque,
    Mixed,
};

static RuntimePatchAlphaState AnalyzeRuntimePatchAlpha(const u8 *pixels, i32 pitch, i32 textureFormat,
                                                       i32 left, i32 top, i32 width, i32 height)
{
    if (textureFormat == TEX_FMT_R5G6B5 || textureFormat == TEX_FMT_R8G8B8)
        return RuntimePatchAlphaState::Opaque;

    bool sawZero = false;
    bool sawOpaque = false;
    for (i32 y = top; y < top + height; y++)
    {
        const u8 *row = pixels + y * pitch + left * g_TextureFormatBytesPerPixel[textureFormat];
        for (i32 x = 0; x < width; x++)
        {
            i32 alpha = 0;
            i32 alphaMax = 0;
            if (textureFormat == TEX_FMT_A8R8G8B8)
            {
                alpha = row[x * 4 + 3];
                alphaMax = 0xff;
            }
            else if (textureFormat == TEX_FMT_A4R4G4B4)
            {
                alpha = reinterpret_cast<const u16 *>(row)[x] & 0x000f;
                alphaMax = 0x0f;
            }
            else if (textureFormat == TEX_FMT_A1R5G5B5)
            {
                alpha = reinterpret_cast<const u16 *>(row)[x] & 0x0001;
                alphaMax = 0x01;
            }

            if (alpha == 0)
                sawZero = true;
            else if (alpha == alphaMax)
                sawOpaque = true;
            else
                return RuntimePatchAlphaState::Mixed;

            if (sawZero && sawOpaque)
                return RuntimePatchAlphaState::Mixed;
        }
    }
    return sawOpaque ? RuntimePatchAlphaState::Opaque : RuntimePatchAlphaState::Empty;
}

static void BlendRuntimePatchRow(u8 *destination, const u8 *replacement, i32 pixels, i32 textureFormat)
{
    if (textureFormat == TEX_FMT_A8R8G8B8)
    {
        for (i32 x = 0; x < pixels; x++, destination += 4, replacement += 4)
        {
            const i32 replacementAlpha = replacement[3];
            const i32 destinationWeight = 0xff - replacementAlpha;
            destination[0] = static_cast<u8>(
                (destination[0] * destinationWeight + replacement[0] * replacementAlpha) >> 8);
            destination[1] = static_cast<u8>(
                (destination[1] * destinationWeight + replacement[1] * replacementAlpha) >> 8);
            destination[2] = static_cast<u8>(
                (destination[2] * destinationWeight + replacement[2] * replacementAlpha) >> 8);
            destination[3] = static_cast<u8>(std::min<i32>(destination[3] + replacementAlpha, 0xff));
        }
        return;
    }

    if (textureFormat == TEX_FMT_A4R4G4B4)
    {
        u16 *dst = reinterpret_cast<u16 *>(destination);
        const u16 *rep = reinterpret_cast<const u16 *>(replacement);
        for (i32 x = 0; x < pixels; x++)
        {
            const i32 repR = (rep[x] >> 12) & 0xf;
            const i32 repG = (rep[x] >> 8) & 0xf;
            const i32 repB = (rep[x] >> 4) & 0xf;
            const i32 repA = rep[x] & 0xf;
            const i32 dstR = (dst[x] >> 12) & 0xf;
            const i32 dstG = (dst[x] >> 8) & 0xf;
            const i32 dstB = (dst[x] >> 4) & 0xf;
            const i32 dstA = dst[x] & 0xf;
            const i32 destinationWeight = 0xf - repA;
            const i32 outR = (dstR * destinationWeight + repR * repA) >> 4;
            const i32 outG = (dstG * destinationWeight + repG * repA) >> 4;
            const i32 outB = (dstB * destinationWeight + repB * repA) >> 4;
            const i32 outA = std::min(dstA + repA, 0xf);
            dst[x] = static_cast<u16>((outR << 12) | (outG << 8) | (outB << 4) | outA);
        }
        return;
    }

    if (textureFormat == TEX_FMT_A1R5G5B5)
    {
        u16 *dst = reinterpret_cast<u16 *>(destination);
        const u16 *rep = reinterpret_cast<const u16 *>(replacement);
        for (i32 x = 0; x < pixels; x++)
        {
            if ((rep[x] & 1) != 0)
                dst[x] = rep[x] | 1;
        }
        return;
    }

    memcpy(destination, replacement, static_cast<size_t>(pixels) * g_TextureFormatBytesPerPixel[textureFormat]);
}

static SDL_Surface *LoadRuntimePatchSurface(const char *textureName)
{
    u8 *data = FileSystem::OpenRuntimeOverride(textureName);
    if (data == nullptr)
        return nullptr;
    const size_t size = g_LastFileSize;
    SDL_IOStream *stream = SDL_IOFromConstMem(data, size);
    SDL_Surface *surface = stream ? IMG_Load_IO(stream, true) : nullptr;
    std::free(data);
    return surface;
}

static void ApplyRuntimePatchRect(TextureData &texture, const SDL_Surface *patch, const SDL_Rect &rect,
                                  i32 &patchedRects, i32 &fallbackRects)
{
    if (rect.x >= patch->w || rect.y >= patch->h)
    {
        fallbackRects++;
        return;
    }
    const i32 copyWidth = std::min(rect.w, patch->w - rect.x);
    const i32 copyHeight = std::min(rect.h, patch->h - rect.y);
    if (copyWidth <= 0 || copyHeight <= 0)
    {
        fallbackRects++;
        return;
    }

    const i32 bytesPerPixel = g_TextureFormatBytesPerPixel[texture.format];
    const i32 destinationPitch = static_cast<i32>(texture.width) * bytesPerPixel;
    const auto replacementAlpha = AnalyzeRuntimePatchAlpha(
        static_cast<const u8 *>(patch->pixels), patch->pitch, texture.format,
        rect.x, rect.y, copyWidth, copyHeight);
    if (replacementAlpha == RuntimePatchAlphaState::Empty)
    {
        fallbackRects++;
        return;
    }

    const auto destinationAlpha = AnalyzeRuntimePatchAlpha(
        texture.textureData, destinationPitch, texture.format,
        rect.x, rect.y, copyWidth, copyHeight);
    patchedRects++;
    for (i32 y = rect.y; y < rect.y + copyHeight; y++)
    {
        u8 *destinationRow = texture.textureData + y * destinationPitch + rect.x * bytesPerPixel;
        const u8 *replacementRow = static_cast<const u8 *>(patch->pixels) + y * patch->pitch +
                                   rect.x * bytesPerPixel;
        if (destinationAlpha == RuntimePatchAlphaState::Opaque)
            BlendRuntimePatchRow(destinationRow, replacementRow, copyWidth, texture.format);
        else
            memcpy(destinationRow, replacementRow, static_cast<size_t>(copyWidth) * bytesPerPixel);
    }
}

static bool ApplyRuntimeSpritePatch(AnmManager *manager, i32 textureIdx, const AnmRawEntry *entry,
                                    SDL_Surface *runtimePatch, i32 &patchedRects, i32 &fallbackRects)
{
    patchedRects = 0;
    fallbackRects = 0;
    TextureData &texture = manager->textures[textureIdx];
    if (runtimePatch == nullptr || texture.textureData == nullptr || entry == nullptr ||
        texture.format <= TEX_FMT_UNKNOWN || texture.format > TEX_FMT_A4R4G4B4)
        return false;

    SDL_Surface *patch = SDL_ConvertSurface(runtimePatch, g_TextureFormatSDLMapping[texture.format]);
    if (patch == nullptr)
        return false;

    const LE<u32> *spriteOffset = entry->spriteOffsets;
    for (i32 index = 0; index < entry->numSprites; index++, spriteOffset++)
    {
        const auto *sprite = reinterpret_cast<const AnmRawSprite *>(
            reinterpret_cast<const u8 *>(entry) + static_cast<u32>(*spriteOffset));
        if (sprite->size.x <= 0.0f || sprite->size.y <= 0.0f || entry->width <= 0 || entry->height <= 0)
            continue;

        SDL_Rect lowerRight = {
            static_cast<i32>(sprite->offset.x) % static_cast<i32>(entry->width),
            static_cast<i32>(sprite->offset.y) % static_cast<i32>(entry->height),
            std::min(static_cast<i32>(sprite->size.x), static_cast<i32>(entry->width)),
            std::min(static_cast<i32>(sprite->size.y), static_cast<i32>(entry->height)),
        };
        if (lowerRight.x < 0)
            lowerRight.x += entry->width;
        if (lowerRight.y < 0)
            lowerRight.y += entry->height;

        const i32 splitWidth = lowerRight.x + lowerRight.w - entry->width;
        const i32 splitHeight = lowerRight.y + lowerRight.h - entry->height;
        const i32 originalWidth = lowerRight.w;
        const i32 originalHeight = lowerRight.h;
        if (splitWidth > 0)
            lowerRight.w = entry->width - lowerRight.x;
        if (splitHeight > 0)
            lowerRight.h = entry->height - lowerRight.y;
        if (splitWidth > 0 && splitHeight > 0)
            ApplyRuntimePatchRect(texture, patch, SDL_Rect{0, 0, splitWidth, splitHeight},
                                  patchedRects, fallbackRects);
        if (splitWidth > 0)
            ApplyRuntimePatchRect(texture, patch,
                                  SDL_Rect{0, lowerRight.y, splitWidth, lowerRight.h},
                                  patchedRects, fallbackRects);
        if (splitHeight > 0)
            ApplyRuntimePatchRect(texture, patch,
                                  SDL_Rect{lowerRight.x, 0, lowerRight.w, splitHeight},
                                  patchedRects, fallbackRects);
        (void)originalWidth;
        (void)originalHeight;
        ApplyRuntimePatchRect(texture, patch, lowerRight, patchedRects, fallbackRects);
    }

    SDL_DestroySurface(patch);
    manager->SetCurrentTexture(texture.handle);
    g_GfxBackend->SetTextureImage(texture.width, texture.height,
                                  g_TextureFormatTypeGfxMapping[texture.format],
                                  g_TextureFormatTypeMapping[texture.format], texture.textureData);
    return !g_GfxBackend->HasError();
}

void AnmManager::CreateTextureObject()
{
    this->currentTextureHandle = g_GfxBackend->CreateTexture();
    g_GfxBackend->BindTexture(this->currentTextureHandle);

    g_GfxBackend->SetTextureFilter();
}

SDL_Surface *AnmManager::LoadToSurfaceWithFormat(const char *filename, SDL_PixelFormat format, u8 **fileData,
                                                 ZunColor colorKey, bool isExternalResource,
                                                 bool allowRuntimeOverride)
{
    u8 *data;
    SDL_Surface *imageSrcSurface;
    SDL_Surface *imageTargetSurface;
    SDL_IOStream *rwData;

    data = allowRuntimeOverride ? FileSystem::OpenPath(filename, isExternalResource ? 1 : 0)
                                : FileSystem::OpenOriginalPath(filename, isExternalResource ? 1 : 0);

    if (data == NULL)
    {
        return NULL;
    }

    rwData = SDL_IOFromConstMem(data, g_LastFileSize);

    if (rwData == NULL)
    {
        std::free(data);
        return NULL;
    }

    imageSrcSurface = IMG_Load_IO(rwData, true);

    if (imageSrcSurface == NULL)
    {
        std::free(data);
        return NULL;
    }

    if (colorKey != 0)
    {
        const SDL_PixelFormatDetails *details = SDL_GetPixelFormatDetails(imageSrcSurface->format);
        const u32 mappedKey = SDL_MapRGB(details, nullptr, (colorKey >> 16) & 0xff,
                                         (colorKey >> 8) & 0xff, colorKey & 0xff);
        if (!SDL_SetSurfaceColorKey(imageSrcSurface, true, mappedKey))
        {
            SDL_DestroySurface(imageSrcSurface);
            return NULL;
        }
    }

    imageTargetSurface = SDL_ConvertSurface(imageSrcSurface, format);

    SDL_DestroySurface(imageSrcSurface);

    if (imageTargetSurface != NULL && fileData != NULL)
    {
        *fileData = data;
    }
    else
    {
        std::free(data);
    }

    return imageTargetSurface;
}

u8 *AnmManager::ExtractSurfacePixels(SDL_Surface *src, u8 pixelDepth)
{
    SDL_LockSurface(src);

    const i32 dstPitch = src->w * pixelDepth;
    const i32 srcPitch = src->pitch;

    u8 *pixelData = new u8[dstPitch * src->h];
    u8 *dstPtr = pixelData;
    const u8 *srcPtr = (u8 *)src->pixels;

    for (int i = 0; i < src->h; i++)
    {
        std::memcpy(dstPtr, srcPtr, dstPitch);
        dstPtr += dstPitch;
        srcPtr += srcPitch;
    }

    SDL_UnlockSurface(src);

    return pixelData;
}

void AnmManager::ReleaseSurfaces(void)
{
    for (i32 idx = 0; idx < ARRAY_SIZE_SIGNED(this->surfaces); idx++)
    {
        ReleaseSurface(idx);
    }
}

void AnmManager::TakeScreenshotIfRequested()
{
    if (this->screenshotTextureId >= 0)
    {
        this->TakeScreenshot(this->screenshotTextureId, this->screenshotLeft, this->screenshotTop,
                             this->screenshotWidth, this->screenshotHeight);
        this->screenshotTextureId = -1;
    }
    return;
}

#ifdef TH_ENABLE_THCRAP
static i32 g_ThcrapSnapshotRequests = 0;

void AnmManager::QueueThcrapSnapshotIfRequested()
{
    // Preserve the upstream 60 Hz breakpoint/key sampling cadence, but defer
    // the actual backbuffer read until the current portable draw is complete.
    const bool *keyboard = SDL_GetKeyboardState(NULL);
    if (keyboard != NULL && keyboard[SDL_SCANCODE_P] && g_ThcrapSnapshotRequests < 1000)
        ++g_ThcrapSnapshotRequests;
}

void AnmManager::TakeThcrapSnapshotIfRequested()
{
    if (g_ThcrapSnapshotRequests <= 0)
        return;
    --g_ThcrapSnapshotRequests;

    FileSystem::CreateDir("snapshot");

    char relativePath[32];
    std::string outputPath;
    bool foundFreeSlot = false;
    for (i32 index = 0; index < 1000; ++index)
    {
        std::snprintf(relativePath, sizeof(relativePath), "snapshot/th%03d.png", index);
        outputPath = FileSystem::GetPrefPath(relativePath);
        FILE *existing = FileSystem::FopenUTF8(relativePath, "rb");
        if (existing == NULL)
        {
            foundFreeSlot = true;
            break;
        }
        std::fclose(existing);
    }
    if (!foundFreeSlot)
        return;

    const i32 readWidth = GAME_WINDOW_WIDTH_REAL;
    const i32 readHeight = GAME_WINDOW_HEIGHT_REAL;
    std::vector<u8> pixels(static_cast<size_t>(readWidth) * static_cast<size_t>(readHeight) * 4u);
    g_GfxBackend->ReadPixels(0, 0, readWidth, readHeight, pixels.data());

    SDL_Surface *source = SDL_CreateSurfaceFrom(readWidth, readHeight, SDL_PIXELFORMAT_RGBA32, pixels.data(),
                                                readWidth * 4);
    SDL_Surface *snapshot = SDL_CreateSurface(GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, SDL_PIXELFORMAT_RGBA32);
    if (source == NULL || snapshot == NULL)
    {
        if (source != NULL)
            SDL_DestroySurface(source);
        if (snapshot != NULL)
            SDL_DestroySurface(snapshot);
        return;
    }

    const SDL_Rect sourceRect = {0, 0, readWidth, readHeight};
    const SDL_Rect targetRect = {0, 0, GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT};
    if (!SDL_BlitSurfaceScaled(source, &sourceRect, snapshot, &targetRect, SDL_SCALEMODE_LINEAR) ||
        !IMG_SavePNG(snapshot, outputPath.c_str()))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TH06 thcrap snapshot failed: %s", outputPath.c_str());
    }
#ifdef TH_DEV_TOOLS
    else
    {
        SDL_Log("TH06 thcrap snapshot: %s size=%dx%d source=%dx%d", outputPath.c_str(), GAME_WINDOW_WIDTH,
                GAME_WINDOW_HEIGHT, readWidth, readHeight);
    }
#endif

    SDL_DestroySurface(snapshot);
    SDL_DestroySurface(source);
}
#endif

AnmManager::~AnmManager()
{
    if (this->dummyTextureHandle != 0)
    {
        g_GfxBackend->DeleteTexture(this->dummyTextureHandle);
        this->dummyTextureHandle = 0;
    }

}

// void AnmManager::ReleaseVertexBuffer()
// {
//     if (this->vertexBuffer != NULL)
//     {
//         this->vertexBuffer->Release();
//         this->vertexBuffer = NULL;
//     }
// }

AnmManager::AnmManager()
{

    this->maybeLoadedSpriteCount = 0;

    std::memset(this, 0, sizeof(AnmManager));
    ClearVertexBuffer();

    for (i32 spriteIndex = 0; spriteIndex < ARRAY_SIZE_SIGNED(this->sprites); spriteIndex++)
    {
        this->sprites[spriteIndex].sourceFileIndex = -1;
    }

    g_PrimitivesToDrawVertexBuf[3].position.w = 1.0;
    g_PrimitivesToDrawVertexBuf[2].position.w = g_PrimitivesToDrawVertexBuf[3].position.w;
    g_PrimitivesToDrawVertexBuf[1].position.w = g_PrimitivesToDrawVertexBuf[2].position.w;
    g_PrimitivesToDrawVertexBuf[0].position.w = g_PrimitivesToDrawVertexBuf[1].position.w;
    g_PrimitivesToDrawVertexBuf[0].textureUV.x = 0.0;
    g_PrimitivesToDrawVertexBuf[0].textureUV.y = 0.0;
    g_PrimitivesToDrawVertexBuf[1].textureUV.x = 1.0;
    g_PrimitivesToDrawVertexBuf[1].textureUV.y = 0.0;
    g_PrimitivesToDrawVertexBuf[2].textureUV.x = 0.0;
    g_PrimitivesToDrawVertexBuf[2].textureUV.y = 1.0;
    g_PrimitivesToDrawVertexBuf[3].textureUV.x = 1.0;
    g_PrimitivesToDrawVertexBuf[3].textureUV.y = 1.0;

    g_PrimitivesToDrawNoVertexBuf[3].position.w = 1.0;
    g_PrimitivesToDrawNoVertexBuf[2].position.w = g_PrimitivesToDrawNoVertexBuf[3].position.w;
    g_PrimitivesToDrawNoVertexBuf[1].position.w = g_PrimitivesToDrawNoVertexBuf[2].position.w;
    g_PrimitivesToDrawNoVertexBuf[0].position.w = g_PrimitivesToDrawNoVertexBuf[1].position.w;
    g_PrimitivesToDrawNoVertexBuf[0].textureUV.x = 0.0;
    g_PrimitivesToDrawNoVertexBuf[0].textureUV.y = 0.0;
    g_PrimitivesToDrawNoVertexBuf[1].textureUV.x = 1.0;
    g_PrimitivesToDrawNoVertexBuf[1].textureUV.y = 0.0;
    g_PrimitivesToDrawNoVertexBuf[2].textureUV.x = 0.0;
    g_PrimitivesToDrawNoVertexBuf[2].textureUV.y = 1.0;
    g_PrimitivesToDrawNoVertexBuf[3].textureUV.x = 1.0;
    g_PrimitivesToDrawNoVertexBuf[3].textureUV.y = 1.0;

    //    this->vertexBuffer = NULL;
    this->currentBlendMode = 0;
    this->screenshotTextureId = -1;
    this->projectionMode = PROJECTION_MODE_PERSPECTIVE;

    this->dirtyFlags = 0;

    for (u32 i = 0; i < ARRAY_SIZE_SIGNED(this->transformMatrices); i++)
    {
        transformMatrices[i].Identity();
        dirtyTransformMatrices[i].Identity();
    }
}

void AnmManager::SetupVertexBuffer()
{
    this->vertexBufferContents[2].position.x = -128;
    this->vertexBufferContents[0].position.x = -128;
    this->vertexBufferContents[3].position.x = 128;
    this->vertexBufferContents[1].position.x = 128;

    this->vertexBufferContents[1].position.y = -128;
    this->vertexBufferContents[0].position.y = -128;
    this->vertexBufferContents[3].position.y = 128;
    this->vertexBufferContents[2].position.y = 128;

    this->vertexBufferContents[3].position.z = 0;
    this->vertexBufferContents[2].position.z = 0;
    this->vertexBufferContents[1].position.z = 0;
    this->vertexBufferContents[0].position.z = 0;

    this->vertexBufferContents[2].textureUV.x = 0;
    this->vertexBufferContents[0].textureUV.x = 0;
    this->vertexBufferContents[3].textureUV.x = 1;
    this->vertexBufferContents[1].textureUV.x = 1;
    this->vertexBufferContents[1].textureUV.y = 0;
    this->vertexBufferContents[0].textureUV.y = 0;
    this->vertexBufferContents[3].textureUV.y = 1;
    this->vertexBufferContents[2].textureUV.y = 1;

    g_PrimitivesToDrawUnknown[0].position = this->vertexBufferContents[0].position;
    g_PrimitivesToDrawUnknown[1].position = this->vertexBufferContents[1].position;
    g_PrimitivesToDrawUnknown[2].position = this->vertexBufferContents[2].position;
    g_PrimitivesToDrawUnknown[3].position = this->vertexBufferContents[3].position;

    g_PrimitivesToDrawUnknown[0].textureUV.x = this->vertexBufferContents[0].textureUV.x;
    g_PrimitivesToDrawUnknown[0].textureUV.y = this->vertexBufferContents[0].textureUV.y;
    g_PrimitivesToDrawUnknown[1].textureUV.x = this->vertexBufferContents[1].textureUV.x;
    g_PrimitivesToDrawUnknown[1].textureUV.y = this->vertexBufferContents[1].textureUV.y;
    g_PrimitivesToDrawUnknown[2].textureUV.x = this->vertexBufferContents[2].textureUV.x;
    g_PrimitivesToDrawUnknown[2].textureUV.y = this->vertexBufferContents[2].textureUV.y;
    g_PrimitivesToDrawUnknown[3].textureUV.x = this->vertexBufferContents[3].textureUV.x;
    g_PrimitivesToDrawUnknown[3].textureUV.y = this->vertexBufferContents[3].textureUV.y;

    //    RenderVertexInfo *buffer;

    if (((g_Supervisor.cfg.opts >> GCOS_DONT_USE_VERTEX_BUF) & 1) == 0)
    {
        //        g_Supervisor.d3dDevice->CreateVertexBuffer(sizeof(this->vertexBufferContents), 0, D3DFVF_TEX1 |
        //        D3DFVF_XYZ,
        //                                                   D3DPOOL_MANAGED, &this->vertexBuffer);
        //
        //        this->vertexBuffer->Lock(0, 0, (BYTE **)&buffer, 0);
        //        memcpy(buffer, this->vertexBufferContents, sizeof(this->vertexBufferContents));
        //        this->vertexBuffer->Unlock();
        //
        //        g_Supervisor.d3dDevice->SetStreamSource(0, g_AnmManager->vertexBuffer, sizeof(RenderVertexInfo));
        this->SetAttributePointer(VERTEX_ARRAY_POSITION, sizeof(*vertexBufferContents),
                                  &this->vertexBufferContents[0].position);
        this->SetAttributePointer(VERTEX_ARRAY_TEX_COORD, sizeof(*vertexBufferContents),
                                  &this->vertexBufferContents[0].textureUV);
    }
}

ZunResult AnmManager::LoadTexture(i32 textureIdx, const char *textureName, i32 textureFormat, ZunColor colorKey,
                                  bool isExternalResource, bool clearTransparentRgb,
                                  bool allowRuntimeOverride)
{
    if (textureIdx < 0 || textureIdx >= ARRAY_SIZE_SIGNED(this->textures))
        return ZUN_ERROR;

    u8 *rawTextureData;
    SDL_Surface *textureSurface;

    ReleaseTexture(textureIdx);

    if (((g_Supervisor.cfg.opts >> GCOS_FORCE_16BIT_COLOR_MODE) & 1) != 0)
    {
        if (g_TextureFormatSDLMapping[textureFormat] == SDL_PIXELFORMAT_RGBA32 ||
            g_TextureFormatSDLMapping[textureFormat] == SDL_PIXELFORMAT_UNKNOWN)
        {
            textureFormat = TEX_FMT_A4R4G4B4;
        }
        else if (g_TextureFormatSDLMapping[textureFormat] == SDL_PIXELFORMAT_RGB24)
        {
            textureFormat = TEX_FMT_R5G6B5;
        }
    }

    textureSurface = LoadToSurfaceWithFormat(textureName, g_TextureFormatSDLMapping[textureFormat],
                                             (u8 **)&this->textures[textureIdx].fileData, colorKey,
                                             isExternalResource, allowRuntimeOverride);

    if (textureSurface == NULL)
    {
        free((void *)this->textures[textureIdx].fileData);
        this->textures[textureIdx].fileData = NULL;
        return ZUN_ERROR;
    }

    // Hideous hack to account for ANM entries that report a different texture size than the actual size
    // Runtime-only textures do not necessarily have ANM metadata. Keep this
    // lookup guarded if the texture and ANM table capacities diverge later.
    const AnmRawEntry *entry =
        textureIdx < ARRAY_SIZE_SIGNED(this->anmFiles) ? this->anmFiles[textureIdx] : nullptr;
    if (entry && (textureSurface->w != entry->width || textureSurface->h != entry->height))
    {
        SDL_Surface *textureSurface2 =
            SDL_CreateSurface(entry->width, entry->height, g_TextureFormatSDLMapping[textureFormat]);
        SDL_Rect srcRect = {0, 0, textureSurface->w, textureSurface->h};
        SDL_Rect dstRect = {0, 0, entry->width, entry->height};
        SDL_BlitSurfaceScaled(textureSurface, &srcRect, textureSurface2, &dstRect, SDL_SCALEMODE_LINEAR);
        SDL_DestroySurface(textureSurface);
        textureSurface = textureSurface2;
    }

    CreateTextureObject();

    // Clear any errors that might be pending
    while (g_GfxBackend->HasError())
    {
    }

    rawTextureData = ExtractSurfacePixels(textureSurface, g_TextureFormatBytesPerPixel[textureFormat]);

    // Some PNG encoders leave arbitrary RGB values in fully transparent
    // pixels. They are invisible with point sampling, but straight-alpha
    // linear filtering interpolates that RGB into neighboring visible pixels
    // and produces a bright fringe. Keep this opt-in: ordinary TH06 textures
    // retain their original loading contract, while callers emulating D3DX
    // text-image uploads can request canonical transparent black.
    if (clearTransparentRgb && textureFormat == TEX_FMT_A8R8G8B8)
    {
        const std::size_t pixelCount = static_cast<std::size_t>(textureSurface->w) * textureSurface->h;
        for (std::size_t pixel = 0; pixel < pixelCount; pixel++)
        {
            u8 *rgba = rawTextureData + pixel * 4;
            if (rgba[3] == 0)
                rgba[0] = rgba[1] = rgba[2] = 0;
        }
    }

    this->textures[textureIdx].handle = this->currentTextureHandle;
    this->textures[textureIdx].textureData = rawTextureData;
    this->textures[textureIdx].width = textureSurface->w;
    this->textures[textureIdx].height = textureSurface->h;
    this->textures[textureIdx].format = textureFormat;

    // The original D3DX call used an invalid filter combination. Sampling is
    // controlled by the texture object, while color-key transparency has
    // already been applied to the SDL source surface before conversion.

    // g_glFuncTable.glTexImage2D(GL_TEXTURE_2D, 0, g_TextureFormatTypeGfxMapping[textureFormat], textureSurface->w,
    //                            textureSurface->h, 0, g_TextureFormatTypeGfxMapping[textureFormat],
    //                            g_TextureFormatTypeMapping[textureFormat], rawTextureData);
    g_GfxBackend->SetTextureImage(textureSurface->w, textureSurface->h, g_TextureFormatTypeGfxMapping[textureFormat],
                                  g_TextureFormatTypeMapping[textureFormat], rawTextureData);

    SDL_DestroySurface(textureSurface);

    if (g_GfxBackend->HasError())
    {
        ReleaseTexture(textureIdx);

        return ZUN_ERROR;
    }

    return ZUN_SUCCESS;
}

ZunResult AnmManager::LoadEmbeddedTexture(i32 textureIdx, const AnmRawEntry *entry)
{
    if (!entry || !entry->hasData)
        return ZUN_ERROR;
    struct EmbeddedHeader
    {
        i16 magic, colorDepth, imageType, format, width, height;
        i32 unused;
        u8 data[];
    };
    const auto *image = reinterpret_cast<const EmbeddedHeader *>(
        reinterpret_cast<const u8 *>(entry) + static_cast<u32>(entry->textureOffset));
    const i32 format = image->format;
    if (format <= 0 || format >= 6 || image->width <= 0 || image->height <= 0)
        return ZUN_ERROR;
    ReleaseTexture(textureIdx);
    CreateTextureObject();
    const size_t size = static_cast<size_t>(image->width) * image->height *
                        g_TextureFormatBytesPerPixel[format];
    u8 *copy = new u8[size];
    memcpy(copy, image->data, size);
    this->textures[textureIdx].handle = this->currentTextureHandle;
    this->textures[textureIdx].textureData = copy;
    this->textures[textureIdx].width = image->width;
    this->textures[textureIdx].height = image->height;
    this->textures[textureIdx].format = format;
    g_GfxBackend->SetTextureImage(image->width, image->height, g_TextureFormatTypeGfxMapping[format],
                                  g_TextureFormatTypeMapping[format], copy);
    return g_GfxBackend->HasError() ? ZUN_ERROR : ZUN_SUCCESS;
}

ZunResult AnmManager::LoadTextureAlphaChannel(i32 textureIdx, const char *textureName, i32 textureFormat,
                                              ZunColor colorKey, bool allowRuntimeOverride)
{
    SDL_Surface *alphaSurface;
    TextureData *textureDesc;

    u8 *dstData;
    const u8 *srcData;
    u8 *dstData8;
    const u8 *srcData8;
    u16 *dstData16;
    u32 x;
    u32 y;

    textureDesc = this->textures + textureIdx;

    if (textureDesc->format != TEX_FMT_A8R8G8B8 && textureDesc->format != TEX_FMT_A4R4G4B4 &&
        textureDesc->format != TEX_FMT_A1R5G5B5)
    {
        g_GameErrorContext.Fatal(TH_ERR_ANMMANAGER_UNK_TEX_FORMAT);
        return ZUN_ERROR;
    }

    // Alpha masks are intensity images. Normalize their byte layout first;
    // otherwise forced 16-bit mode can make this function read an RGBA32 mask
    // through a u16 pointer selected from the destination texture format.
    alphaSurface = LoadToSurfaceWithFormat(textureName, SDL_PIXELFORMAT_RGBA32, NULL, 0, false,
                                           allowRuntimeOverride);

    if (alphaSurface == NULL)
    {
        return ZUN_ERROR;
    }

    if (alphaSurface->w != static_cast<i32>(textureDesc->width) ||
        alphaSurface->h != static_cast<i32>(textureDesc->height))
    {
        SDL_DestroySurface(alphaSurface);
        return ZUN_ERROR;
    }

    SDL_LockSurface(alphaSurface);

    dstData = (u8 *)textureDesc->textureData;
    srcData = (u8 *)alphaSurface->pixels;

    // Copy over the alpha channel from the source to the destination, taking
    // into account the texture format.
    switch (textureDesc->format)
    {
    case TEX_FMT_A8R8G8B8:
        dstData8 = dstData;
        for (y = 0; y < textureDesc->height; y++)
        {
            srcData8 = srcData + alphaSurface->pitch * y;

            for (x = 0; x < textureDesc->width; x++, srcData8 += 4, dstData8 += 4)
            {
                dstData8[3] = srcData8[0];
            }
        }
        break;

        // The dereferences here make the assumption that rows are 16-bit aligned. With SDL, this is guaranteed

    case TEX_FMT_A1R5G5B5:
        dstData16 = (u16 *)dstData;
        for (y = 0; y < textureDesc->height; y++)
        {
            srcData8 = srcData + alphaSurface->pitch * y;

            for (x = 0; x < textureDesc->width; x++, srcData8 += 4, dstData16++)
            {
                const u8 alpha = srcData8[0];
                *dstData16 = (*dstData16 & 0xfffe) | (alpha >> 7);
            }
        }
        break;

    case TEX_FMT_A4R4G4B4:
        dstData16 = (u16 *)dstData;
        for (y = 0; y < textureDesc->height; y++)
        {
            srcData8 = srcData + alphaSurface->pitch * y;

            for (x = 0; x < textureDesc->width; x++, srcData8 += 4, dstData16++)
            {
                const u8 alpha = srcData8[0];
                *dstData16 = (*dstData16 & 0xfff0) | (alpha >> 4);
            }
        }
        break;
    }

    SDL_UnlockSurface(alphaSurface);
    SDL_DestroySurface(alphaSurface);

    this->SetCurrentTexture(this->textures[textureIdx].handle);
    g_GfxBackend->SetTextureImage(textureDesc->width, textureDesc->height, PIXEL_RGBA,
                                  g_TextureFormatTypeMapping[textureDesc->format], textureDesc->textureData);

    return ZUN_SUCCESS;
}

ZunResult AnmManager::CreateEmptyTexture(i32 textureIdx, u32 width, u32 height, i32 textureFormat)
{
    CreateTextureObject();

    this->textures[textureIdx].handle = this->currentTextureHandle;
    this->textures[textureIdx].width = BitCeil(width);
    this->textures[textureIdx].height = BitCeil(height);
    this->textures[textureIdx].format = textureFormat;

    g_GfxBackend->SetTextureImage(textures[textureIdx].width, textures[textureIdx].height,
                                  g_TextureFormatTypeGfxMapping[textureFormat],
                                  g_TextureFormatTypeMapping[textureFormat], NULL);

    return ZUN_SUCCESS;
}

ZunResult AnmManager::LoadAnm(i32 anmIdx, const char *path, i32 spriteIdxOffset)
{
    if (anmIdx < 0 || anmIdx >= ARRAY_SIZE_SIGNED(this->anmFiles))
        return ZUN_ERROR;

#ifdef __EMSCRIPTEN__
    if (WebTransitionAnmCacheEntry *cache = FindWebTransitionAnm(anmIdx, path); cache && cache->loaded)
    {
        RestoreWebTransitionAnmBindings(this, cache);
        this->currentBlendMode = 0xff;
        this->currentTextureHandle = 0;
        this->currentSprite = nullptr;
        return ZUN_SUCCESS;
    }
#endif

    this->ReleaseAnm(anmIdx);
    this->anmFiles[anmIdx] = (AnmRawEntry *)FileSystem::OpenPath(path, 0);

    AnmRawEntry *anm = this->anmFiles[anmIdx];

    if (anm == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "th06: ANM file not found: %s", path);
        g_GameErrorContext.Fatal(TH_ERR_ANMMANAGER_SPRITE_CORRUPTED, path);
        return ZUN_ERROR;
    }

    anm->textureIdx = anmIdx;

    const char *anmName = (char *)((u8 *)anm + anm->nameOffset);
    const bool isMutableTexture = *anmName == '@';

    // D3D seems to treat unknown texture format as a wildcard, but SDL treats it as an error
    //   This is a hack to avoid that for now
    if (anm->format == TEX_FMT_UNKNOWN)
    {
        anm->format = TEX_FMT_A8R8G8B8;
    }

    SDL_Surface *runtimePatch = nullptr;
    if (*anmName != '@')
        runtimePatch = LoadRuntimePatchSurface(anmName);

    if (*anmName == '@')
    {
        this->CreateEmptyTexture(anm->textureIdx, anm->width, anm->height, anm->format);
    }
    else if (runtimePatch != nullptr)
    {
        // Upstream thcrap patches ANM PNGs on top of the game's original
        // texture, sprite by sprite. TH06 stores that original texture and its
        // alpha mask as external archive files, so both must be loaded without
        // allowing the runtime override to replace them before composition.
        if (this->LoadTexture(anm->textureIdx, anmName, anm->format, anm->colorKey,
                              false, false, false) != ZUN_SUCCESS)
        {
            SDL_DestroySurface(runtimePatch);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "th06: original ANM texture failed beneath runtime patch: %s (from %s)",
                         anmName, path);
            g_GameErrorContext.Fatal(TH_ERR_ANMMANAGER_TEXTURE_CORRUPTED, anmName);
            return ZUN_ERROR;
        }

        if (anm->alphaNameOffset != 0)
        {
            const char *alphaName = reinterpret_cast<const char *>(
                reinterpret_cast<const u8 *>(anm) + static_cast<u32>(anm->alphaNameOffset));
            if (this->LoadTextureAlphaChannel(anm->textureIdx, alphaName, anm->format,
                                              anm->colorKey, false) != ZUN_SUCCESS)
            {
                SDL_DestroySurface(runtimePatch);
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                             "th06: original ANM alpha failed beneath runtime patch: %s (from %s)",
                             alphaName, path);
                g_GameErrorContext.Fatal(TH_ERR_ANMMANAGER_TEXTURE_CORRUPTED, alphaName);
                return ZUN_ERROR;
            }
        }

        i32 patchedRects = 0;
        i32 fallbackRects = 0;
        const bool patchOk = ApplyRuntimeSpritePatch(this, anm->textureIdx, anm, runtimePatch,
                                                     patchedRects, fallbackRects);
        SDL_DestroySurface(runtimePatch);
        if (!patchOk)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "th06: ANM sprite patch failed: %s (from %s)", anmName, path);
            g_GameErrorContext.Fatal(TH_ERR_ANMMANAGER_TEXTURE_CORRUPTED, anmName);
            return ZUN_ERROR;
        }
#ifdef TH_DEV_TOOLS
        SDL_Log("th06 thcrap ANM sprite patch: %s patched=%d fallback=%d",
                anmName, patchedRects, fallbackRects);
#endif
        g_LastFileWasRuntimeOverride = true;
    }
    else if (this->LoadTexture(anm->textureIdx, anmName, anm->format, anm->colorKey) != ZUN_SUCCESS)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "th06: ANM texture failed: %s (from %s)", anmName, path);
        g_GameErrorContext.Fatal(TH_ERR_ANMMANAGER_TEXTURE_CORRUPTED, anmName);
        return ZUN_ERROR;
    }

    const bool textureWasRuntimeOverride = g_LastFileWasRuntimeOverride;

    if (anm->alphaNameOffset != 0 && !textureWasRuntimeOverride)
    {
        anmName = (char *)((u8 *)anm + anm->alphaNameOffset);
        if (this->LoadTextureAlphaChannel(anm->textureIdx, anmName, anm->format, anm->colorKey) != ZUN_SUCCESS)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "th06: ANM alpha texture failed: %s (from %s)", anmName, path);
            g_GameErrorContext.Fatal(TH_ERR_ANMMANAGER_TEXTURE_CORRUPTED, anmName);
            return ZUN_ERROR;
        }
    }

    anm->spriteIdxOffset = spriteIdxOffset;

    const LE<u32> *curSpriteOffset = anm->spriteOffsets;

    i32 index;
    const AnmRawSprite *rawSprite;

    std::vector<i32> loadedSpriteIndices;
    loadedSpriteIndices.reserve(this->anmFiles[anmIdx]->numSprites);
    for (index = 0; index < this->anmFiles[anmIdx]->numSprites; index++, curSpriteOffset++)
    {
        rawSprite = (AnmRawSprite *)((u8 *)anm + *curSpriteOffset);

        AnmLoadedSprite loadedSprite;
        loadedSprite.sourceFileIndex = this->anmFiles[anmIdx]->textureIdx;
        loadedSprite.startPixelInclusive.x = rawSprite->offset.x;
        loadedSprite.startPixelInclusive.y = rawSprite->offset.y;
        loadedSprite.endPixelInclusive.x = rawSprite->offset.x + rawSprite->size.x;
        loadedSprite.endPixelInclusive.y = rawSprite->offset.y + rawSprite->size.y;
        loadedSprite.textureWidth = (float)anm->width;
        loadedSprite.textureHeight = (float)anm->height;
        const i32 loadedSpriteIdx = rawSprite->id + spriteIdxOffset;
        this->LoadSprite(loadedSpriteIdx, &loadedSprite);
        loadedSpriteIndices.push_back(loadedSpriteIdx);
    }

    // Mutable '@' textures are written by text/screenshot paths at runtime and
    // therefore must keep their authoritative source layout. Static ANM
    // textures get a draw-only atlas with one-texel edge extrusion; failure to
    // build it is a compatibility fallback, never a load failure.
    if (!isMutableTexture)
        BuildSpriteExtrusionAtlas(this, anm->textureIdx, loadedSpriteIndices);

    for (index = 0; index < anm->numScripts; index++, curSpriteOffset += 2)
    {
        this->scripts[curSpriteOffset[0] + spriteIdxOffset] = (AnmRawInstr *)((u8 *)anm + curSpriteOffset[1]);
        this->spriteIndices[curSpriteOffset[0] + spriteIdxOffset] = spriteIdxOffset;
    }

    this->anmFilesSpriteIndexOffsets[anmIdx] = spriteIdxOffset;

#ifdef __EMSCRIPTEN__
    if (WebTransitionAnmCacheEntry *cache = FindWebTransitionAnm(anmIdx, path))
        CaptureWebTransitionAnmBindings(this, cache);
#endif

    return ZUN_SUCCESS;
}

#ifdef __EMSCRIPTEN__
ZunResult AnmManager::PreloadTransitionAnm(i32 anmIdx, const char *path, i32 spriteIdxOffset)
{
    WebTransitionAnmCacheEntry *cache = FindWebTransitionAnm(anmIdx, path);
    if (!cache)
        return ZUN_ERROR;
    if (cache->loaded)
        return ZUN_SUCCESS;

    const ZunResult result = LoadAnm(anmIdx, path, spriteIdxOffset);
    if (result != ZUN_SUCCESS)
        return result;
    ReleaseAnm(anmIdx);
    return ZUN_SUCCESS;
}
#endif

void AnmManager::ReleaseAnm(i32 anmIdx)
{
    if (anmIdx < 0 || anmIdx >= ARRAY_SIZE_SIGNED(this->anmFiles))
        return;

#ifdef __EMSCRIPTEN__
    if (WebTransitionAnmCacheEntry *cache = FindWebTransitionAnm(anmIdx); cache && cache->loaded)
    {
        if (cache->active)
            ClearWebTransitionAnmBindings(this, cache);
        this->currentBlendMode = 0xff;
        this->currentTextureHandle = 0;
        this->currentSprite = nullptr;
        return;
    }
#endif

    if (this->anmFiles[anmIdx] != NULL)
    {
        const LE<i32> *spriteIdx;
        i32 i;
        i32 spriteIdxOffset = this->anmFilesSpriteIndexOffsets[anmIdx];
        const LE<u32> *byteOffset = this->anmFiles[anmIdx]->spriteOffsets;
        for (i = 0; i < this->anmFiles[anmIdx]->numSprites; i++, byteOffset++)
        {
            spriteIdx = (LE<i32> *)((u8 *)this->anmFiles[anmIdx] + *byteOffset);
            memset(&this->sprites[*spriteIdx + spriteIdxOffset], 0,
                   sizeof(this->sprites[*spriteIdx + spriteIdxOffset]));
            this->sprites[*spriteIdx + spriteIdxOffset].sourceFileIndex = -1;
        }

        for (i = 0; i < this->anmFiles[anmIdx]->numScripts; i++, byteOffset += 2)
        {
            this->scripts[*byteOffset + spriteIdxOffset] = NULL;
            this->spriteIndices[*byteOffset + spriteIdxOffset] = 0;
        }
        this->anmFilesSpriteIndexOffsets[anmIdx] = 0;
        const AnmRawEntry *entry = this->anmFiles[anmIdx];
        this->ReleaseTexture(entry->textureIdx);
        AnmRawEntry *anmFilePtr = this->anmFiles[anmIdx];
        free(anmFilePtr);
        this->anmFiles[anmIdx] = 0;
        this->currentBlendMode = 0xff;
        this->currentTextureHandle = 0;
        this->currentSprite = nullptr;
    }
}

void AnmManager::ReleaseTexture(i32 textureIdx)
{
    if (textureIdx < 0 || textureIdx >= ARRAY_SIZE_SIGNED(this->textures))
        return;

    if (this->spriteAtlasTextures[textureIdx] != 0)
    {
        if (this->currentTextureHandle == this->spriteAtlasTextures[textureIdx])
            this->currentTextureHandle = 0;
        g_GfxBackend->DeleteTexture(this->spriteAtlasTextures[textureIdx]);
        this->spriteAtlasTextures[textureIdx] = 0;
        this->currentSprite = nullptr;
    }

    if (this->textures[textureIdx].handle != 0)
    {
        if (this->currentTextureHandle == this->textures[textureIdx].handle)
        {
            this->currentTextureHandle = 0;
        }

        g_GfxBackend->DeleteTexture(this->textures[textureIdx].handle);

        this->textures[textureIdx].handle = 0;
    }

    free((void *)this->textures[textureIdx].fileData);
    this->textures[textureIdx].fileData = NULL;

    delete[] this->textures[textureIdx].textureData;
    this->textures[textureIdx].textureData = NULL;
}

void AnmManager::LoadSprite(u32 spriteIdx, const AnmLoadedSprite *sprite)
{
    this->sprites[spriteIdx] = *sprite;
    this->sprites[spriteIdx].spriteId = this->maybeLoadedSpriteCount++;

    this->sprites[spriteIdx].uvStart.x =
        this->sprites[spriteIdx].startPixelInclusive.x / this->sprites[spriteIdx].textureWidth;
    this->sprites[spriteIdx].uvEnd.x =
        this->sprites[spriteIdx].endPixelInclusive.x / this->sprites[spriteIdx].textureWidth;
    this->sprites[spriteIdx].uvStart.y =
        this->sprites[spriteIdx].startPixelInclusive.y / this->sprites[spriteIdx].textureHeight;
    this->sprites[spriteIdx].uvEnd.y =
        this->sprites[spriteIdx].endPixelInclusive.y / this->sprites[spriteIdx].textureHeight;

    this->sprites[spriteIdx].extrudedUvStart = this->sprites[spriteIdx].uvStart;
    this->sprites[spriteIdx].extrudedUvEnd = this->sprites[spriteIdx].uvEnd;
    this->sprites[spriteIdx].hasExtrudedUv = false;

    this->sprites[spriteIdx].widthPx =
        this->sprites[spriteIdx].endPixelInclusive.x - this->sprites[spriteIdx].startPixelInclusive.x;
    this->sprites[spriteIdx].heightPx =
        this->sprites[spriteIdx].endPixelInclusive.y - this->sprites[spriteIdx].startPixelInclusive.y;
}

ZunResult AnmManager::SetActiveSprite(AnmVm *vm, u32 sprite_index)
{
    if (this->sprites[sprite_index].sourceFileIndex < 0)
    {
        return ZUN_ERROR;
    }

    vm->activeSpriteIndex = (i16)sprite_index;
    vm->sprite = this->sprites + sprite_index;
    vm->matrix.Identity();
    vm->matrix.m[0][0] = vm->sprite->widthPx / vm->sprite->textureWidth;
    vm->matrix.m[1][1] = vm->sprite->heightPx / vm->sprite->textureHeight;

    return ZUN_SUCCESS;
}

void AnmManager::SetActiveSpriteWidth(AnmVm *vm, f32 widthPx)
{
    if (vm == nullptr || vm->activeSpriteIndex < 0)
        return;

    AnmLoadedSprite *sprite = &this->sprites[vm->activeSpriteIndex];
    widthPx = std::clamp(widthPx, 0.0f, sprite->textureWidth);
    sprite->endPixelInclusive.x = sprite->startPixelInclusive.x + widthPx;
    sprite->uvEnd.x = sprite->endPixelInclusive.x / sprite->textureWidth;
    // This helper intentionally edits a sprite cell's source-space width at
    // runtime. Its prepacked extrusion rectangle no longer represents that
    // cell exactly, so fall back to the authoritative source texture.
    sprite->hasExtrudedUv = false;
    sprite->widthPx = widthPx;
    vm->sprite = sprite;
    vm->matrix.m[0][0] = widthPx / sprite->textureWidth;
}

void AnmManager::SetAndExecuteScript(AnmVm *vm, const AnmRawInstr *beginingOfScript)
{
    ZunTimer *timer;

    vm->flags.flip = 0;
    vm->Initialize();
    vm->beginingOfScript = beginingOfScript;
    vm->currentInstruction = vm->beginingOfScript;

    timer = &(vm->currentTimeInScript);
    timer->current = 0;
    timer->subFrame = 0.0;
    timer->previous = -999;

    vm->flags.isVisible = 0;
    if (beginingOfScript)
    {
        this->ExecuteScript(vm);
    }
    vm->UpdatePrev();
}

void AnmManager::SetRenderStateForVm(const AnmVm *vm)
{
    // IMPORTANT: Do not "finish" TH06's high-refresh presentation by globally
    // interpolating ANM properties here (color/alpha, scale, rotation, UV, etc.)
    // just because TH07 can safely do so. TH06 has legacy VM lifecycles which
    // memset, copy, reuse, rebind scripts, or overwrite only part of an AnmVm;
    // therefore a seemingly initialized prev* endpoint does NOT prove that the
    // previous/current pair belongs to the same logical animation state.
    //
    // This was verified by a real regression on 2026-08-23: even after adding a
    // generic "prev state valid" guard, enabling VM-wide presentation
    // interpolation caused severe animation breakage and had to be reverted.
    // The concrete real-device symptom was not merely "less smooth": after
    // bullets were fired, animation/effect sprites which should have finished
    // could remain on screen and flash repeatedly. That is evidence of broken
    // VM lifetime/state semantics, not a cosmetic interpolation mismatch. The
    // exact offending owner/path was not isolated because the experiment was
    // immediately rolled back; do not infer a narrower cause from this comment.
    // Earlier attempts also produced transient opacity errors on render-only
    // frames. Treat this as a lifecycle invariant, not as unfinished cleanup.
    //
    // If a particular TH06 animation needs smoother high-refresh presentation,
    // fix it at that animation/object owner only, after proving every create,
    // reset, reuse, script switch, temporary draw mutation, and freeze path keeps
    // valid prev/current endpoints. Do not copy TH07's global Lerp behavior into
    // AnmManager::Draw* / SetRenderStateForVm without that per-owner proof.
    // Keep discrete ANM properties at their authoritative simulation value here.
    const ZunColor drawColor = vm->color;
    if (this->currentBlendMode != vm->flags.blendMode)
    {
        this->FlushVertexBuffer();
        this->currentBlendMode = vm->flags.blendMode;
        if (this->currentBlendMode == AnmVmBlendMode_InvSrcAlpha)
        {
            g_GfxBackend->SetBlendMode(BLEND_INV_SRC_ALPHA);
            //            g_Supervisor.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        }
        else
        {
            g_GfxBackend->SetBlendMode(BLEND_ONE);
            //            g_Supervisor.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
        }
    }

    if (((g_Supervisor.cfg.opts >> GCOS_USE_D3D_HW_TEXTURE_BLENDING) & 1) == 0)
    {
        this->SetColorOp(COMPONENT_RGB, (ColorOp)vm->flags.colorOp);
    }

    if (((g_Supervisor.cfg.opts >> GCOS_DONT_USE_VERTEX_BUF) & 1) == 0)
    {
        for (i32 i = 0; i < 4; ++i)
        {
            g_PrimitivesToDrawVertexBuf[i].diffuse = drawColor;
        }
    }
    else
    {
        g_PrimitivesToDrawNoVertexBuf[0].diffuse = drawColor;
        g_PrimitivesToDrawNoVertexBuf[1].diffuse = drawColor;
        g_PrimitivesToDrawNoVertexBuf[2].diffuse = drawColor;
        g_PrimitivesToDrawNoVertexBuf[3].diffuse = drawColor;
        g_PrimitivesToDrawUnknown[0].diffuse = drawColor;
        g_PrimitivesToDrawUnknown[1].diffuse = drawColor;
        g_PrimitivesToDrawUnknown[2].diffuse = drawColor;
        g_PrimitivesToDrawUnknown[3].diffuse = drawColor;
    }

    this->SetDepthMask(!vm->flags.zWriteDisable);

    return;
}

void AnmManager::UpdateDirtyStates()
{
    while (this->dirtyFlags != 0)
    {
        u32 currFlagIndex = CountrZero(this->dirtyFlags);
        this->dirtyFlags &= ~(1 << currFlagIndex);

        // This would all be nicer if the enum was flag values rather than indices,
        //   but compilers just aren't able to deal with that in the switch statement :/
        switch (currFlagIndex)
        {
        case DIRTY_FOG:
            if (this->dirtyFogNear != this->fogNear || this->dirtyFogFar != this->fogFar)
            {
                this->fogNear = this->dirtyFogNear;
                this->fogFar = this->dirtyFogFar;
                g_GfxBackend->SetFogRange(this->fogNear, this->fogFar);
            }

            if (this->dirtyFogColor != this->fogColor)
            {
                this->fogColor = this->dirtyFogColor;
                g_GfxBackend->SetFogColor(this->fogColor);
            }

            break;
        case DIRTY_DEPTH_CONFIG:
            if (this->dirtyDepthMask != this->depthMask)
            {
                this->depthMask = this->dirtyDepthMask;
                g_GfxBackend->SetDepthMask(this->depthMask);
            }

            if (this->dirtyDepthFunc != this->depthFunc)
            {
                this->depthFunc = this->dirtyDepthFunc;

                g_GfxBackend->SetDepthFunc(this->depthFunc);
            }

            break;
        case DIRTY_VERTEX_ATTRIBUTE_ENABLE: {
            u8 changedAttributes = this->dirtyEnabledVertexAttributes ^ this->enabledVertexAttributes;
            this->enabledVertexAttributes = this->dirtyEnabledVertexAttributes;

            while (changedAttributes != 0)
            {
                u8 currBit = CountrZero(changedAttributes);
                g_GfxBackend->ToggleVertexAttribute(changedAttributes & (1 << currBit),
                                                    this->enabledVertexAttributes & (1 << currBit));
                changedAttributes &= ~(1 << currBit);
            }

            break;
        }
        case DIRTY_VERTEX_ATTRIBUTE_ARRAY:
            for (u32 i = 0; i < 3; i++)
            {
                if (!std::memcmp(&this->attribArrays[i], &this->dirtyAttribArrays[i], sizeof(*this->attribArrays)))
                {
                    continue;
                }

                this->attribArrays[i] = this->dirtyAttribArrays[i];

                g_GfxBackend->SetAttributePointer((VertexAttributeArrays)i, this->attribArrays[i].stride,
                                                  this->attribArrays[i].ptr);
            }

            break;
        case DIRTY_COLOR_OP:
            for (u32 i = 0; i < 2; i++)
            {
                if (this->colorOps[i] == this->dirtyColorOps[i])
                {
                    continue;
                }

                this->colorOps[i] = this->dirtyColorOps[i];

                g_GfxBackend->SetColorOp((TextureOpComponent)i, this->colorOps[i]);
            }

            break;
        case DIRTY_TEXTURE_FACTOR:
            this->textureFactor = this->dirtytTextureFactor;
            g_GfxBackend->SetTextureFactor(this->textureFactor);
            break;
        case DIRTY_MODEL_MATRIX:
        case DIRTY_VIEW_MATRIX:
        case DIRTY_PROJECTION_MATRIX:
        case DIRTY_TEXTURE_MATRIX:
            std::memcpy(&this->transformMatrices[currFlagIndex - DIRTY_MODEL_MATRIX],
                        &this->dirtyTransformMatrices[currFlagIndex - DIRTY_MODEL_MATRIX],
                        sizeof(*this->transformMatrices));
            g_GfxBackend->SetTransformMatrix((TransformMatrix)(currFlagIndex - DIRTY_MODEL_MATRIX),
                                             this->transformMatrices[currFlagIndex - DIRTY_MODEL_MATRIX]);
        }
    }
}

ZunResult AnmManager::DrawOrthographic(const AnmVm *vm)
{
    const ZunVec2 drawUv = vm->uvScrollPos;
    ZunVec2 drawUvStart;
    ZunVec2 drawUvEnd;
    GfxTextureHandle drawTexture;
    ResolveSpriteDrawSampling(this, vm->sprite, drawUv, drawUvStart, drawUvEnd, drawTexture);
    float triangleX1, triangleX2, triangleY1, triangleY2;
    g_PrimitivesToDrawVertexBuf[0].position.z = g_PrimitivesToDrawVertexBuf[1].position.z =
        g_PrimitivesToDrawVertexBuf[2].position.z = g_PrimitivesToDrawVertexBuf[3].position.z = vm->pos.z;

    triangleX1 = ZUN_MAX(g_PrimitivesToDrawVertexBuf[0].position.x, g_PrimitivesToDrawVertexBuf[1].position.x);
    triangleX1 = ZUN_MAX(g_PrimitivesToDrawVertexBuf[2].position.x, triangleX1);
    triangleX1 = ZUN_MAX(g_PrimitivesToDrawVertexBuf[3].position.x, triangleX1);

    triangleY1 = ZUN_MAX(g_PrimitivesToDrawVertexBuf[0].position.y, g_PrimitivesToDrawVertexBuf[1].position.y);
    triangleY1 = ZUN_MAX(g_PrimitivesToDrawVertexBuf[2].position.y, triangleY1);
    triangleY1 = ZUN_MAX(g_PrimitivesToDrawVertexBuf[3].position.y, triangleY1);

    triangleX2 = ZUN_MIN(g_PrimitivesToDrawVertexBuf[0].position.x, g_PrimitivesToDrawVertexBuf[1].position.x);
    triangleX2 = ZUN_MIN(g_PrimitivesToDrawVertexBuf[2].position.x, triangleX2);
    triangleX2 = ZUN_MIN(g_PrimitivesToDrawVertexBuf[3].position.x, triangleX2);

    triangleY2 = ZUN_MIN(g_PrimitivesToDrawVertexBuf[0].position.y, g_PrimitivesToDrawVertexBuf[1].position.y);
    triangleY2 = ZUN_MIN(g_PrimitivesToDrawVertexBuf[2].position.y, triangleY2);
    triangleY2 = ZUN_MIN(g_PrimitivesToDrawVertexBuf[3].position.y, triangleY2);

    if (triangleX1 < g_Supervisor.viewport.x || triangleY1 < g_Supervisor.viewport.y ||
        triangleX2 > (g_Supervisor.viewport.x + g_Supervisor.viewport.width) ||
        triangleY2 > (g_Supervisor.viewport.y + g_Supervisor.viewport.height))
    {
        return ZUN_SUCCESS;
    }

    g_PrimitivesToDrawVertexBuf[0].textureUV.x = g_PrimitivesToDrawVertexBuf[2].textureUV.x =
        drawUvStart.x;
    g_PrimitivesToDrawVertexBuf[1].textureUV.x = g_PrimitivesToDrawVertexBuf[3].textureUV.x =
        drawUvEnd.x;
    g_PrimitivesToDrawVertexBuf[0].textureUV.y = g_PrimitivesToDrawVertexBuf[1].textureUV.y =
        drawUvStart.y;
    g_PrimitivesToDrawVertexBuf[2].textureUV.y = g_PrimitivesToDrawVertexBuf[3].textureUV.y =
        drawUvEnd.y;

    if (this->currentSprite != vm->sprite)
    {
        this->currentSprite = vm->sprite;
        g_PrimitivesToDrawVertexBuf[0].textureUV.x = g_PrimitivesToDrawVertexBuf[2].textureUV.x =
            drawUvStart.x;
        g_PrimitivesToDrawVertexBuf[1].textureUV.x = g_PrimitivesToDrawVertexBuf[3].textureUV.x =
            drawUvEnd.x;
        g_PrimitivesToDrawVertexBuf[0].textureUV.y = g_PrimitivesToDrawVertexBuf[1].textureUV.y =
            drawUvStart.y;
        g_PrimitivesToDrawVertexBuf[2].textureUV.y = g_PrimitivesToDrawVertexBuf[3].textureUV.y =
            drawUvEnd.y;
    }
    // UV scrolling can switch a VM from the extruded atlas back to its source
    // texture without changing the sprite pointer, so texture selection cannot
    // be keyed only by `currentSprite`.
    this->SetCurrentTexture(drawTexture);

    if (((g_Supervisor.cfg.opts >> GCOS_DONT_USE_VERTEX_BUF) & 1) == 0)
    {
        this->SetVertexAttributes(VERTEX_ATTR_TEX_COORD);
    }
    else
    {
        this->SetVertexAttributes(VERTEX_ATTR_TEX_COORD | VERTEX_ATTR_DIFFUSE);
    }

    this->SetRenderStateForVm(vm);

    this->SetProjectionMode(PROJECTION_MODE_ORTHOGRAPHIC);

    //    if (roundToPixel)
    //    {
    //        g_glFuncTable.glMatrixMode(GL_MODELVIEW);
    //        g_glFuncTable.glTranslatef(0.5f, 0.5f, 0.0f);
    //    }

    if (((g_Supervisor.cfg.opts >> GCOS_DONT_USE_VERTEX_BUF) & 1) == 0)
    {
        this->AddSpriteToDrawBuffer(g_PrimitivesToDrawVertexBuf);
        /*this->SetAttributePointer(VERTEX_ARRAY_POSITION, sizeof(*g_PrimitivesToDrawVertexBuf),
                                  &g_PrimitivesToDrawVertexBuf[0].position);
        this->SetAttributePointer(VERTEX_ARRAY_TEX_COORD, sizeof(*g_PrimitivesToDrawVertexBuf),
                                  &g_PrimitivesToDrawVertexBuf[0].textureUV);*/

        //        g_Supervisor.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_PrimitivesToDrawVertexBuf, 0x18);
    }
    else
    {
        g_PrimitivesToDrawNoVertexBuf[0].position.x = g_PrimitivesToDrawVertexBuf[0].position.x;
        g_PrimitivesToDrawNoVertexBuf[0].position.y = g_PrimitivesToDrawVertexBuf[0].position.y;
        g_PrimitivesToDrawNoVertexBuf[0].position.z = g_PrimitivesToDrawVertexBuf[0].position.z;
        g_PrimitivesToDrawNoVertexBuf[1].position.x = g_PrimitivesToDrawVertexBuf[1].position.x;
        g_PrimitivesToDrawNoVertexBuf[1].position.y = g_PrimitivesToDrawVertexBuf[1].position.y;
        g_PrimitivesToDrawNoVertexBuf[1].position.z = g_PrimitivesToDrawVertexBuf[1].position.z;
        g_PrimitivesToDrawNoVertexBuf[2].position.x = g_PrimitivesToDrawVertexBuf[2].position.x;
        g_PrimitivesToDrawNoVertexBuf[2].position.y = g_PrimitivesToDrawVertexBuf[2].position.y;
        g_PrimitivesToDrawNoVertexBuf[2].position.z = g_PrimitivesToDrawVertexBuf[2].position.z;
        g_PrimitivesToDrawNoVertexBuf[3].position.x = g_PrimitivesToDrawVertexBuf[3].position.x;
        g_PrimitivesToDrawNoVertexBuf[3].position.y = g_PrimitivesToDrawVertexBuf[3].position.y;
        g_PrimitivesToDrawNoVertexBuf[3].position.z = g_PrimitivesToDrawVertexBuf[3].position.z;
        g_PrimitivesToDrawNoVertexBuf[0].textureUV.x = g_PrimitivesToDrawNoVertexBuf[2].textureUV.x =
            drawUvStart.x;
        g_PrimitivesToDrawNoVertexBuf[1].textureUV.x = g_PrimitivesToDrawNoVertexBuf[3].textureUV.x =
            drawUvEnd.x;
        g_PrimitivesToDrawNoVertexBuf[0].textureUV.y = g_PrimitivesToDrawNoVertexBuf[1].textureUV.y =
            drawUvStart.y;
        g_PrimitivesToDrawNoVertexBuf[2].textureUV.y = g_PrimitivesToDrawNoVertexBuf[3].textureUV.y =
            drawUvEnd.y;

        this->SetAttributePointer(VERTEX_ARRAY_POSITION, sizeof(*g_PrimitivesToDrawNoVertexBuf),
                                  &g_PrimitivesToDrawNoVertexBuf[0].position);
        this->SetAttributePointer(VERTEX_ARRAY_TEX_COORD, sizeof(*g_PrimitivesToDrawNoVertexBuf),
                                  &g_PrimitivesToDrawNoVertexBuf[0].textureUV);
        this->SetAttributePointer(VERTEX_ARRAY_DIFFUSE, sizeof(*g_PrimitivesToDrawNoVertexBuf),
                                  &g_PrimitivesToDrawNoVertexBuf[0].diffuse);
        //        g_Supervisor.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_PrimitivesToDrawNoVertexBuf, 0x1c);
        this->BackendDrawCall();
    }

    return ZUN_SUCCESS;
}

void AnmManager::ClearVertexBuffer()
{
    if (((g_Supervisor.cfg.opts >> GCOS_DONT_USE_VERTEX_BUF) & 1) != 0)
    {
        return;
    }
    this->spritesToDraw = 0;
    this->vertexBufferStartPtr = this->vertexBufferEndPtr = this->vertexBuffer;
}

void AnmManager::FlushVertexBuffer()
{
    if (((g_Supervisor.cfg.opts >> GCOS_DONT_USE_VERTEX_BUF) & 1) != 0)
        return;
    if (spritesToDraw == 0)
        return;

    g_GfxBackend->SetTextureArg(TEX_ARG_DIFFUSE);
    g_GfxBackend->SetColorOp(COMPONENT_ALPHA, COLOR_OP_MODULATE);
    g_GfxBackend->SetColorOp(COMPONENT_RGB, COLOR_OP_MODULATE);
    this->SetVertexAttributes(VERTEX_ATTR_TEX_COORD | VERTEX_ATTR_DIFFUSE);

    this->SetAttributePointer(VERTEX_ARRAY_POSITION, sizeof(VertexTex1DiffuseXyzrhw), &vertexBufferStartPtr->position);
    this->SetAttributePointer(VERTEX_ARRAY_TEX_COORD, sizeof(VertexTex1DiffuseXyzrhw), &vertexBufferStartPtr->textureUV);
    this->SetAttributePointer(VERTEX_ARRAY_DIFFUSE, sizeof(VertexTex1DiffuseXyzrhw), &vertexBufferStartPtr->diffuse);
    this->UpdateDirtyStates();

    g_GfxBackend->Draw(PRIM_TRIANGLES, 0, spritesToDraw * 6);

    this->ClearVertexBuffer();
    flushesThisFrame++;
}

/* This function copies 4 vertices creating a quad into 6 vertices
 * (2 triangles) for rendering.
 */

ZunResult AnmManager::AddSpriteToDrawBuffer(VertexTex1DiffuseXyzrhw *vertices)
{
    this->vertexBufferEndPtr[0] = vertices[0];
    this->vertexBufferEndPtr[1] = vertices[1];
    this->vertexBufferEndPtr[2] = vertices[2];
    this->vertexBufferEndPtr[3] = vertices[1];
    this->vertexBufferEndPtr[4] = vertices[2];
    this->vertexBufferEndPtr[5] = vertices[3];

    this->vertexBufferEndPtr += 6;
    this->spritesToDraw++;

    return ZUN_SUCCESS;
}

ZunResult AnmManager::DrawNoRotation(const AnmVm *vm)
{
    float fVar2;
    float fVar3;

    if (vm->flags.isVisible == 0)
    {
        return ZUN_ERROR;
    }
    if (vm->flags.flag1 == 0)
    {
        return ZUN_ERROR;
    }
    if (vm->color == 0)
    {
        return ZUN_ERROR;
    }
    const f32 drawScaleX = vm->scaleX;
    const f32 drawScaleY = vm->scaleY;
    fVar2 = (vm->sprite->widthPx * drawScaleX) / 2.0f;
    fVar3 = (vm->sprite->heightPx * drawScaleY) / 2.0f;
    if ((vm->flags.anchor & AnmVmAnchor_Left) == 0)
    {
        g_PrimitivesToDrawVertexBuf[0].position.x = g_PrimitivesToDrawVertexBuf[2].position.x = vm->pos.x - fVar2;
        g_PrimitivesToDrawVertexBuf[1].position.x = g_PrimitivesToDrawVertexBuf[3].position.x = fVar2 + vm->pos.x;
    }
    else
    {
        g_PrimitivesToDrawVertexBuf[0].position.x = g_PrimitivesToDrawVertexBuf[2].position.x = vm->pos.x;
        g_PrimitivesToDrawVertexBuf[1].position.x = g_PrimitivesToDrawVertexBuf[3].position.x =
            fVar2 + vm->pos.x + fVar2;
    }
    if ((vm->flags.anchor & AnmVmAnchor_Top) == 0)
    {
        g_PrimitivesToDrawVertexBuf[0].position.y = g_PrimitivesToDrawVertexBuf[1].position.y = vm->pos.y - fVar3;
        g_PrimitivesToDrawVertexBuf[2].position.y = g_PrimitivesToDrawVertexBuf[3].position.y = fVar3 + vm->pos.y;
    }
    else
    {
        g_PrimitivesToDrawVertexBuf[0].position.y = g_PrimitivesToDrawVertexBuf[1].position.y = vm->pos.y;
        g_PrimitivesToDrawVertexBuf[2].position.y = g_PrimitivesToDrawVertexBuf[3].position.y =
            fVar3 + vm->pos.y + fVar3;
    }
    return this->DrawOrthographic(vm);
}

void AnmManager::TranslateRotation(VertexTex1DiffuseXyzrhw *param_1, f32 x, f32 y, f32 sine, f32 cosine, f32 xOffset,
                                   f32 yOffset)
{
    param_1->position.x = x * cosine + y * sine + xOffset;
    param_1->position.y = -x * sine + y * cosine + yOffset;
    return;
}

ZunResult AnmManager::Draw(const AnmVm *vm)
{
    f32 zSine;
    f32 zCosine;
    f32 spriteXCenter;
    f32 spriteYCenter;
    f32 xOffset;
    f32 yOffset;
    f32 z;

    if (vm->rotation.z == 0.0f)
    {
        return this->DrawNoRotation(vm);
    }
    if (vm->flags.isVisible == 0)
    {
        return ZUN_ERROR;
    }
    if (vm->flags.flag1 == 0)
    {
        return ZUN_ERROR;
    }
    if (vm->color == 0)
    {
        return ZUN_ERROR;
    }
    z = vm->rotation.z;
    fsincos_wrapper(&zSine, &zCosine, z);
    xOffset = vm->pos.x;
    yOffset = vm->pos.y;
    const f32 drawScaleX = vm->scaleX;
    const f32 drawScaleY = vm->scaleY;
    spriteXCenter = (vm->sprite->widthPx * drawScaleX) / 2.0f;
    spriteYCenter = (vm->sprite->heightPx * drawScaleY) / 2.0f;
    this->TranslateRotation(&g_PrimitivesToDrawVertexBuf[0], -spriteXCenter - 0.5f, -spriteYCenter - 0.5f, zSine,
                            zCosine, xOffset, yOffset);
    this->TranslateRotation(&g_PrimitivesToDrawVertexBuf[1], spriteXCenter - 0.5f, -spriteYCenter - 0.5f, zSine,
                            zCosine, xOffset, yOffset);
    this->TranslateRotation(&g_PrimitivesToDrawVertexBuf[2], -spriteXCenter - 0.5f, spriteYCenter - 0.5f, zSine,
                            zCosine, xOffset, yOffset);
    this->TranslateRotation(&g_PrimitivesToDrawVertexBuf[3], spriteXCenter - 0.5f, spriteYCenter - 0.5f, zSine, zCosine,
                            xOffset, yOffset);
    g_PrimitivesToDrawVertexBuf[0].position.z = g_PrimitivesToDrawVertexBuf[1].position.z =
        g_PrimitivesToDrawVertexBuf[2].position.z = g_PrimitivesToDrawVertexBuf[3].position.z = vm->pos.z;
    if ((vm->flags.anchor & AnmVmAnchor_Left) != 0)
    {
        g_PrimitivesToDrawVertexBuf[0].position.x += spriteXCenter;
        g_PrimitivesToDrawVertexBuf[1].position.x += spriteXCenter;
        g_PrimitivesToDrawVertexBuf[2].position.x += spriteXCenter;
        g_PrimitivesToDrawVertexBuf[3].position.x += spriteXCenter;
    }
    if ((vm->flags.anchor & AnmVmAnchor_Top) != 0)
    {
        g_PrimitivesToDrawVertexBuf[0].position.y += spriteYCenter;
        g_PrimitivesToDrawVertexBuf[1].position.y += spriteYCenter;
        g_PrimitivesToDrawVertexBuf[2].position.y += spriteYCenter;
        g_PrimitivesToDrawVertexBuf[3].position.y += spriteYCenter;
    }
    return this->DrawOrthographic(vm);
}

ZunResult AnmManager::DrawFacingCamera(const AnmVm *vm)
{
    f32 centerX;
    f32 centerY;

    if (!vm->flags.isVisible)
    {
        return ZUN_ERROR;
    }
    if (!vm->flags.flag1)
    {
        return ZUN_ERROR;
    }
    if (vm->color == 0)
    {
        return ZUN_ERROR;
    }

    const f32 drawScaleX = vm->scaleX;
    const f32 drawScaleY = vm->scaleY;
    centerX = vm->sprite->widthPx * drawScaleX / 2.0f;
    centerY = vm->sprite->heightPx * drawScaleY / 2.0f;
    if ((vm->flags.anchor & AnmVmAnchor_Left) == 0)
    {
        g_PrimitivesToDrawVertexBuf[0].position.x = g_PrimitivesToDrawVertexBuf[2].position.x = vm->pos.x - centerX;
        g_PrimitivesToDrawVertexBuf[1].position.x = g_PrimitivesToDrawVertexBuf[3].position.x = vm->pos.x + centerX;
    }
    else
    {
        g_PrimitivesToDrawVertexBuf[0].position.x = g_PrimitivesToDrawVertexBuf[2].position.x = vm->pos.x;
        g_PrimitivesToDrawVertexBuf[1].position.x = g_PrimitivesToDrawVertexBuf[3].position.x =
            vm->pos.x + centerX + centerX;
    }
    if ((vm->flags.anchor & AnmVmAnchor_Top) == 0)
    {
        g_PrimitivesToDrawVertexBuf[0].position.y = g_PrimitivesToDrawVertexBuf[1].position.y = vm->pos.y - centerY;
        g_PrimitivesToDrawVertexBuf[2].position.y = g_PrimitivesToDrawVertexBuf[3].position.y = vm->pos.y + centerY;
    }
    else
    {
        g_PrimitivesToDrawVertexBuf[0].position.y = g_PrimitivesToDrawVertexBuf[1].position.y = vm->pos.y;
        g_PrimitivesToDrawVertexBuf[2].position.y = g_PrimitivesToDrawVertexBuf[3].position.y =
            vm->pos.y + centerY + centerY;
    }
    return this->DrawOrthographic(vm);
}

ZunResult AnmManager::Draw3(const AnmVm *vm)
{
    ZunMatrix worldTransformMatrix;
    ZunMatrix rotationMatrix;
    ZunMatrix textureMatrix;
    f32 scaledXCenter;
    f32 scaledYCenter;

    if (!vm->flags.isVisible)
    {
        return ZUN_ERROR;
    }
    if (!vm->flags.flag1)
    {
        return ZUN_ERROR;
    }
    if (vm->color == 0)
    {
        return ZUN_ERROR;
    }

    this->SetProjectionMode(PROJECTION_MODE_PERSPECTIVE);

    ZunMatrix originalView = this->dirtyTransformMatrices[MATRIX_VIEW];
    const f32 drawScaleX = vm->scaleX;
    const f32 drawScaleY = vm->scaleY;
    const ZunVec3 drawRotation = vm->rotation;
    const ZunVec2 drawUv = vm->uvScrollPos;
    ZunVec2 drawUvStart;
    ZunVec2 drawUvEnd;
    GfxTextureHandle drawTexture;
    ResolveSpriteDrawSampling(this, vm->sprite, drawUv, drawUvStart, drawUvEnd, drawTexture);
    const bool useExtrusion = UseSpriteExtrusionAtlas(this, vm->sprite, drawUv);

    worldTransformMatrix = vm->matrix;
    worldTransformMatrix.m[0][0] *= drawScaleX;
    worldTransformMatrix.m[1][1] *= -drawScaleY;
    if (Localization::Active())
    {
        // base_tsa::sprite3d_rotated_voodookill compensates the original
        // fixed -128..+128 3D vertex basis for backing textures that are not
        // 256x256. SetActiveSprite() stores spriteSize / textureSize in the
        // VM matrix, so without this factor a sprite on a 512px translated
        // texture is rendered at half its intended size.
        worldTransformMatrix.m[0][0] *= vm->sprite->textureWidth / 256.0f;
        worldTransformMatrix.m[1][1] *= vm->sprite->textureHeight / 256.0f;
#ifdef TH_DEV_TOOLS
        static bool loggedRotatedWideTexture = false;
        if (!loggedRotatedWideTexture &&
            (vm->sprite->textureWidth != 256.0f || vm->sprite->textureHeight != 256.0f))
        {
            SDL_Log("TH06 sprite3d rotated basis: texture=%.0fx%.0f sprite=%.0fx%.0f m00=%.6f m11=%.6f",
                    static_cast<double>(vm->sprite->textureWidth), static_cast<double>(vm->sprite->textureHeight),
                    static_cast<double>(vm->sprite->widthPx), static_cast<double>(vm->sprite->heightPx),
                    static_cast<double>(worldTransformMatrix.m[0][0]),
                    static_cast<double>(worldTransformMatrix.m[1][1]));
            loggedRotatedWideTexture = true;
        }
#endif
    }

    if (drawRotation.x != 0.0f)
    {
        //        D3DXMatrixRotationX(&rotationMatrix, vm->rotation.x);
        //        D3DXMatrixMultiply(&worldTransformMatrix, &worldTransformMatrix, &rotationMatrix);

        worldTransformMatrix.Rotate(drawRotation.x, 1.0f, 0.0f, 0.0f);
    }

    if (drawRotation.y != 0.0f)
    {
        //        D3DXMatrixRotationY(&rotationMatrix, vm->rotation.y);
        //        D3DXMatrixMultiply(&worldTransformMatrix, &worldTransformMatrix, &rotationMatrix);

        worldTransformMatrix.Rotate(drawRotation.y, 0.0f, 1.0f, 0.0f);
    }

    if (drawRotation.z != 0.0f)
    {
        //        D3DXMatrixRotationZ(&rotationMatrix, vm->rotation.z);
        //        D3DXMatrixMultiply(&worldTransformMatrix, &worldTransformMatrix, &rotationMatrix);

        worldTransformMatrix.Rotate(drawRotation.z, 0.0f, 0.0f, 1.0f);
    }

    if ((vm->flags.anchor & AnmVmAnchor_Left) == 0)
    {
        worldTransformMatrix.m[3][0] = vm->pos.x;
    }
    else
    {
        scaledXCenter = vm->sprite->widthPx * drawScaleX / 2.0f;
        worldTransformMatrix.m[3][0] = ZUN_FABSF(scaledXCenter) + vm->pos.x;
    }

    if ((vm->flags.anchor & AnmVmAnchor_Top) == 0)
    {
        worldTransformMatrix.m[3][1] = -vm->pos.y;
    }
    else
    {
        scaledYCenter = vm->sprite->heightPx * drawScaleY / 2.0f;
        worldTransformMatrix.m[3][1] = -vm->pos.y - ZUN_FABSF(scaledYCenter);
    }

    worldTransformMatrix.m[3][2] = vm->pos.z;

    // Now, set transform matrix.
    ZunMatrix modelView;
    if ((g_Supervisor.cfg.opts >> GCOS_DONT_USE_VERTEX_BUF & 1) != 0)
    {
        modelView = originalView * worldTransformMatrix;
        this->SetTransformMatrix(MATRIX_VIEW, modelView);
    }
    else
    {
        for (int i = 0; i < 4; i++)
            g_PrimitivesToDrawVertexBuf[i].position =
                ZunVec4(worldTransformMatrix * this->vertexBufferContents[i].position, 1.0f);

        g_PrimitivesToDrawVertexBuf[0].textureUV.x = g_PrimitivesToDrawVertexBuf[2].textureUV.x =
            drawUvStart.x;
        g_PrimitivesToDrawVertexBuf[1].textureUV.x = g_PrimitivesToDrawVertexBuf[3].textureUV.x =
            drawUvEnd.x;
        g_PrimitivesToDrawVertexBuf[0].textureUV.y = g_PrimitivesToDrawVertexBuf[1].textureUV.y =
            drawUvStart.y;
        g_PrimitivesToDrawVertexBuf[2].textureUV.y = g_PrimitivesToDrawVertexBuf[3].textureUV.y =
            drawUvEnd.y;
    }

    // Load sprite if vm->sprite is not the same as current sprite.
    if (this->currentSprite != vm->sprite)
    {
        this->currentSprite = vm->sprite;
    }
    this->SetCurrentTexture(drawTexture);
    if ((g_Supervisor.cfg.opts >> GCOS_DONT_USE_VERTEX_BUF & 1) != 0)
    {
        textureMatrix = vm->matrix;
        if (useExtrusion)
        {
            textureMatrix.m[0][0] = drawUvEnd.x - drawUvStart.x;
            textureMatrix.m[1][1] = drawUvEnd.y - drawUvStart.y;
        }
        textureMatrix.m[3][0] = drawUvStart.x;
        textureMatrix.m[3][1] = drawUvStart.y;
        this->SetTransformMatrix(MATRIX_TEXTURE, textureMatrix);
    }

    if (((g_Supervisor.cfg.opts >> GCOS_DONT_USE_VERTEX_BUF) & 1) == 0)
    {
        this->SetVertexAttributes(VERTEX_ATTR_TEX_COORD);
    }
    else
    {
        this->SetVertexAttributes(VERTEX_ATTR_TEX_COORD | VERTEX_ATTR_DIFFUSE);
    }

    // Reset the render state based on the settings fo the given VM.
    this->SetRenderStateForVm(vm);

    // Draw the VM.
    if ((g_Supervisor.cfg.opts >> GCOS_DONT_USE_VERTEX_BUF & 1) == 0)
    {

        this->AddSpriteToDrawBuffer(g_PrimitivesToDrawVertexBuf);
    }
    else
    {
        this->SetAttributePointer(VERTEX_ARRAY_POSITION, sizeof(*g_PrimitivesToDrawUnknown),
                                  &g_PrimitivesToDrawUnknown[0].position);
        this->SetAttributePointer(VERTEX_ARRAY_TEX_COORD, sizeof(*g_PrimitivesToDrawUnknown),
                                  &g_PrimitivesToDrawUnknown[0].textureUV);
        this->SetAttributePointer(VERTEX_ARRAY_DIFFUSE, sizeof(*g_PrimitivesToDrawUnknown),
                                  &g_PrimitivesToDrawUnknown[0].diffuse);

        this->BackendDrawCall();
        this->SetTransformMatrix(MATRIX_VIEW, originalView);
    }

    return ZUN_SUCCESS;
}

ZunResult AnmManager::Draw2(const AnmVm *vm)
{
    ZunMatrix worldTransformMatrix;
    ZunMatrix unusedMatrix;
    ZunMatrix textureMatrix;

    if (!vm->flags.isVisible)
    {
        return ZUN_ERROR;
    }
    if (!vm->flags.flag1)
    {
        return ZUN_ERROR;
    }

    if (vm->rotation.x != 0 || vm->rotation.y != 0 || vm->rotation.z != 0)
    {
        return this->Draw3(vm);
    }

    if (vm->color == 0)
    {
        return ZUN_ERROR;
    }

    SetProjectionMode(PROJECTION_MODE_PERSPECTIVE);

    const f32 drawScaleX = vm->scaleX;
    const f32 drawScaleY = vm->scaleY;
    const ZunVec2 drawUv = vm->uvScrollPos;
    ZunVec2 drawUvStart;
    ZunVec2 drawUvEnd;
    GfxTextureHandle drawTexture;
    ResolveSpriteDrawSampling(this, vm->sprite, drawUv, drawUvStart, drawUvEnd, drawTexture);
    const bool useExtrusion = UseSpriteExtrusionAtlas(this, vm->sprite, drawUv);

    worldTransformMatrix = vm->matrix;
    worldTransformMatrix.m[3][0] = vm->pos.x - 0.5f;
    worldTransformMatrix.m[3][1] = -vm->pos.y + 0.5f;
    if ((vm->flags.anchor & AnmVmAnchor_Left) != 0)
    {
        worldTransformMatrix.m[3][0] += (vm->sprite->widthPx * drawScaleX) / 2.0f;
    }
    if ((vm->flags.anchor & AnmVmAnchor_Top) != 0)
    {
        worldTransformMatrix.m[3][1] -= (vm->sprite->heightPx * drawScaleY) / 2.0f;
    }
    worldTransformMatrix.m[3][2] = vm->pos.z;
    worldTransformMatrix.m[0][0] *= drawScaleX;
    worldTransformMatrix.m[1][1] *= -drawScaleY;
    if (Localization::Active())
    {
        // Same base_tsa correction as Draw3(), for the unrotated 3D path.
        // Anchor offsets above already use widthPx/heightPx directly, which
        // is the typed portable equivalent of the corresponding patch math.
        worldTransformMatrix.m[0][0] *= vm->sprite->textureWidth / 256.0f;
        worldTransformMatrix.m[1][1] *= vm->sprite->textureHeight / 256.0f;
#ifdef TH_DEV_TOOLS
        static bool loggedUnrotatedWideTexture = false;
        if (!loggedUnrotatedWideTexture &&
            (vm->sprite->textureWidth != 256.0f || vm->sprite->textureHeight != 256.0f))
        {
            SDL_Log("TH06 sprite3d unrotated basis: texture=%.0fx%.0f sprite=%.0fx%.0f m00=%.6f m11=%.6f",
                    static_cast<double>(vm->sprite->textureWidth), static_cast<double>(vm->sprite->textureHeight),
                    static_cast<double>(vm->sprite->widthPx), static_cast<double>(vm->sprite->heightPx),
                    static_cast<double>(worldTransformMatrix.m[0][0]),
                    static_cast<double>(worldTransformMatrix.m[1][1]));
            loggedUnrotatedWideTexture = true;
        }
#endif
    }

    ZunMatrix originalView = this->dirtyTransformMatrices[MATRIX_VIEW];
    ZunMatrix modelView;

    if ((g_Supervisor.cfg.opts >> GCOS_DONT_USE_VERTEX_BUF & 1) != 0)
    {
        modelView = originalView * worldTransformMatrix;
        this->SetTransformMatrix(MATRIX_VIEW, modelView);
    }
    else
    {
        for (int i = 0; i < 4; i++)
            g_PrimitivesToDrawVertexBuf[i].position =
                ZunVec4(worldTransformMatrix * this->vertexBufferContents[i].position, 1.0f);

        g_PrimitivesToDrawVertexBuf[0].textureUV.x = g_PrimitivesToDrawVertexBuf[2].textureUV.x =
            drawUvStart.x;
        g_PrimitivesToDrawVertexBuf[1].textureUV.x = g_PrimitivesToDrawVertexBuf[3].textureUV.x =
            drawUvEnd.x;
        g_PrimitivesToDrawVertexBuf[0].textureUV.y = g_PrimitivesToDrawVertexBuf[1].textureUV.y =
            drawUvStart.y;
        g_PrimitivesToDrawVertexBuf[2].textureUV.y = g_PrimitivesToDrawVertexBuf[3].textureUV.y =
            drawUvEnd.y;
    }
    if (this->currentSprite != vm->sprite)
    {
        this->currentSprite = vm->sprite;
        //        if (this->currentTextureHandle != this->textures[vm->sprite->sourceFileIndex].handle)
        //        {
        //            this->currentTexture = this->textures[vm->sprite->sourceFileIndex];
        //            g_Supervisor.d3dDevice->SetTexture(0, this->currentTexture);
        //        }

        if (((g_Supervisor.cfg.opts >> GCOS_DONT_USE_VERTEX_BUF) & 1) == 0)
        {
            this->SetVertexAttributes(VERTEX_ATTR_TEX_COORD);
        }
        else
        {
            this->SetVertexAttributes(VERTEX_ATTR_TEX_COORD | VERTEX_ATTR_DIFFUSE);
        }
    }
    this->SetCurrentTexture(drawTexture);
    if ((g_Supervisor.cfg.opts >> GCOS_DONT_USE_VERTEX_BUF & 1) != 0)
    {
        textureMatrix = vm->matrix;
        if (useExtrusion)
        {
            textureMatrix.m[0][0] = drawUvEnd.x - drawUvStart.x;
            textureMatrix.m[1][1] = drawUvEnd.y - drawUvStart.y;
        }
        textureMatrix.m[3][0] = drawUvStart.x;
        textureMatrix.m[3][1] = drawUvStart.y;
        this->SetTransformMatrix(MATRIX_TEXTURE, textureMatrix);
    }

    this->SetRenderStateForVm(vm);

    if ((g_Supervisor.cfg.opts >> GCOS_DONT_USE_VERTEX_BUF & 1) == 0)
    {
        this->AddSpriteToDrawBuffer(g_PrimitivesToDrawVertexBuf);
    }
    else
    {
        this->SetAttributePointer(VERTEX_ARRAY_POSITION, sizeof(*g_PrimitivesToDrawUnknown),
                                  &g_PrimitivesToDrawUnknown[0].position);
        this->SetAttributePointer(VERTEX_ARRAY_TEX_COORD, sizeof(*g_PrimitivesToDrawUnknown),
                                  &g_PrimitivesToDrawUnknown[0].textureUV);
        this->SetAttributePointer(VERTEX_ARRAY_DIFFUSE, sizeof(*g_PrimitivesToDrawUnknown),
                                  &g_PrimitivesToDrawUnknown[0].diffuse);

        //        g_Supervisor.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, , 0x18);

        this->BackendDrawCall();

        this->SetTransformMatrix(MATRIX_VIEW, originalView);
    }

    return ZUN_SUCCESS;
}

#define AnmF32Arg(index) (*(LE<f32> *)&curInstr->args[index])
#define AnmI32Arg(index) (*(LE<i32> *)&curInstr->args[index])
#define AnmU32Arg(index) (*(LE<u32> *)&curInstr->args[index])
#define AnmI16Arg(index) (*(LE<i16> *)&curInstr->args[index])

i32 AnmManager::ExecuteScript(AnmVm *vm)
{
    const AnmRawInstr *curInstr;
    const AnmRawInstr *nextInstr;
    ZunColor local_28;
    ZunColor local_2c;
    f32 local_30;
    i32 local_34;
    i32 local_38;
    f32 local_3c;

    if (vm->currentInstruction == NULL)
    {
        return 1;
    }

    if (vm->pendingInterrupt != 0)
    {
        goto yolo;
    }

    while (curInstr = vm->currentInstruction, curInstr->time <= vm->currentTimeInScript.AsFrames())
    {
        switch (curInstr->opcode)
        {
        case AnmOpcode_Exit:
            vm->flags.isVisible = 0;
        case AnmOpcode_ExitHide:
            vm->currentInstruction = NULL;
            return 1;
        case AnmOpcode_SetActiveSprite:
            vm->flags.isVisible = 1;
            this->SetActiveSprite(vm, AnmI32Arg(0) + this->spriteIndices[vm->anmFileIndex]);
            vm->timeOfLastSpriteSet = vm->currentTimeInScript.AsFrames();
            break;
        case AnmOpcode_SetRandomSprite:
            vm->flags.isVisible = 1;
            this->SetActiveSprite(vm, AnmI32Arg(0) + g_Rng.GetRandomU16InRange(AnmI32Arg(1)) +
                                          this->spriteIndices[vm->anmFileIndex]);
            vm->timeOfLastSpriteSet = vm->currentTimeInScript.AsFrames();
            break;
        case AnmOpcode_SetScale:
            vm->scaleX = AnmF32Arg(0);
            vm->scaleY = AnmF32Arg(1);
            break;
        case AnmOpcode_SetAlpha:
            COLOR_SET_COMPONENT(vm->color, COLOR_ALPHA_BYTE_IDX, AnmI32Arg(0) & 0xff);
            break;
        case AnmOpcode_SetColor:
            vm->color = COLOR_COMBINE_ALPHA(AnmI32Arg(0), vm->color);
            break;
        case AnmOpcode_Jump:
            vm->currentInstruction = (AnmRawInstr *)(((u8 *)vm->beginingOfScript->args) + AnmI32Arg(0) - 4);
            vm->currentTimeInScript.current = vm->currentInstruction->time;
            continue;
        case AnmOpcode_FlipX:
            vm->flags.flip ^= 1;
            vm->scaleX *= -1.f;
            break;
        case AnmOpcode_UsePosOffset:
            vm->flags.usePosOffset = AnmI32Arg(0);
            break;
        case AnmOpcode_FlipY:
            vm->flags.flip ^= 2;
            vm->scaleY *= -1.f;
            break;
        case AnmOpcode_SetRotation:
            vm->rotation.x = AnmF32Arg(0);
            vm->rotation.y = AnmF32Arg(1);
            vm->rotation.z = AnmF32Arg(2);
            break;
        case AnmOpcode_SetAngleVel:
            vm->angleVel.x = AnmF32Arg(0);
            vm->angleVel.y = AnmF32Arg(1);
            vm->angleVel.z = AnmF32Arg(2);
            break;
        case AnmOpcode_SetScaleSpeed:
            vm->scaleInterpFinalX = AnmF32Arg(0);
            vm->scaleInterpFinalY = AnmF32Arg(1);
            vm->scaleInterpEndTime = 0;
            break;
        case AnmOpcode_ScaleTime:
            vm->scaleInterpFinalX = AnmF32Arg(0);
            vm->scaleInterpFinalY = AnmF32Arg(1);

            vm->scaleInterpEndTime = AnmI16Arg(2);
            vm->scaleInterpTime.InitializeForPopup();

            vm->scaleInterpInitialX = vm->scaleX;
            vm->scaleInterpInitialY = vm->scaleY;
            break;
        case AnmOpcode_Fade:
            vm->alphaInterpInitial = vm->color;
            vm->alphaInterpFinal = COLOR_SET_ALPHA2(vm->color, AnmU32Arg(0));
            vm->alphaInterpEndTime = AnmU32Arg(1);
            vm->alphaInterpTime.InitializeForPopup();
            break;
        case AnmOpcode_SetBlendAdditive:
            vm->flags.blendMode = AnmVmBlendMode_One;
            break;
        case AnmOpcode_SetBlendDefault:
            vm->flags.blendMode = AnmVmBlendMode_InvSrcAlpha;
            break;
        case AnmOpcode_SetPosition:
            if (vm->flags.usePosOffset == 0)
            {
                vm->pos = ZunVec3(AnmF32Arg(0), AnmF32Arg(1), AnmF32Arg(2));
            }
            else
            {
                vm->posOffset = ZunVec3(AnmF32Arg(0), AnmF32Arg(1), AnmF32Arg(2));
            }
            break;
        case AnmOpcode_PosTimeAccel:
            vm->flags.posTime = 2;
            goto PosTimeDoStuff;
        case AnmOpcode_PosTimeDecel:
            vm->flags.posTime = 1;
            goto PosTimeDoStuff;
        case AnmOpcode_PosTimeLinear:
            vm->flags.posTime = 0;
        PosTimeDoStuff:
            if (vm->flags.usePosOffset == 0)
            {
                // This was supposedly originally a memcpy, but any sane compiler should compile a struct assignment to
                // a memcpy
                vm->posInterpInitial = vm->pos;
            }
            else
            {
                // This was supposedly originally a memcpy, but any sane compiler should compile a struct assignment to
                // a memcpy
                vm->posInterpInitial = vm->posOffset;
            }
            vm->posInterpFinal = ZunVec3(AnmF32Arg(0), AnmF32Arg(1), AnmF32Arg(2));
            vm->posInterpEndTime = AnmI32Arg(3);
            vm->posInterpTime.InitializeForPopup();
            break;
        case AnmOpcode_StopHide:
            vm->flags.isVisible = 0;
        case AnmOpcode_Stop:
            if (vm->pendingInterrupt == 0)
            {
                vm->flags.isStopped = 1;
                vm->currentTimeInScript.Decrement(1);
                goto stop;
            }
        yolo:
            nextInstr = NULL;
            curInstr = vm->beginingOfScript;
            while ((curInstr->opcode != AnmOpcode_InterruptLabel || vm->pendingInterrupt != AnmI32Arg(0)) &&
                   curInstr->opcode != AnmOpcode_Exit && curInstr->opcode != AnmOpcode_ExitHide)
            {
                if (curInstr->opcode == AnmOpcode_InterruptLabel && AnmI32Arg(0) == -1)
                {
                    nextInstr = curInstr;
                }
                curInstr = (AnmRawInstr *)(((u8 *)curInstr->args) + curInstr->argsCount);
            }

            vm->pendingInterrupt = 0;
            vm->flags.isStopped = 0;
            if (curInstr->opcode != AnmOpcode_InterruptLabel)
            {
                if (nextInstr == NULL)
                {
                    vm->currentTimeInScript.Decrement(1);
                    goto stop;
                }
                curInstr = nextInstr;
            }

            curInstr = (AnmRawInstr *)(((u8 *)curInstr->args) + curInstr->argsCount);
            vm->currentInstruction = curInstr;
            vm->currentTimeInScript.SetCurrent(vm->currentInstruction->time);
            vm->flags.isVisible = 1;
            continue;
        case AnmOpcode_SetVisibility:
            vm->flags.isVisible = AnmI32Arg(0);
            break;
        case AnmOpcode_AnchorTopLeft:
            vm->flags.anchor = AnmVmAnchor_TopLeft;
            break;
        case AnmOpcode_SetAutoRotate:
            vm->autoRotate = AnmI32Arg(0);
            break;
        case AnmOpcode_UVScrollX:
            vm->uvScrollPos.x += AnmF32Arg(0);
            if (vm->uvScrollPos.x >= 1.0f)
            {
                vm->uvScrollPos.x -= 1.0f;
            }
            else if (vm->uvScrollPos.x < 0.0f)
            {
                vm->uvScrollPos.x += 1.0f;
            }
            break;
        case AnmOpcode_UVScrollY:
            vm->uvScrollPos.y += AnmF32Arg(0);
            if (vm->uvScrollPos.y >= 1.0f)
            {
                vm->uvScrollPos.y -= 1.0f;
            }
            else if (vm->uvScrollPos.y < 0.0f)
            {
                vm->uvScrollPos.y += 1.0f;
            }
            break;
        case AnmOpcode_SetZWriteDisable:
            vm->flags.zWriteDisable = AnmI32Arg(0);
            break;
        case AnmOpcode_Nop:
        case AnmOpcode_InterruptLabel:
        default:
            break;
        }
        vm->currentInstruction = (AnmRawInstr *)(((u8 *)curInstr->args) + curInstr->argsCount);
    }

stop:
    if (vm->angleVel.x != 0.0f)
    {
        vm->rotation.x =
            utils::AddNormalizeAngle(vm->rotation.x, g_Supervisor.effectiveFramerateMultiplier * vm->angleVel.x);
    }
    if (vm->angleVel.y != 0.0f)
    {
        vm->rotation.y =
            utils::AddNormalizeAngle(vm->rotation.y, g_Supervisor.effectiveFramerateMultiplier * vm->angleVel.y);
    }
    if (vm->angleVel.z != 0.0f)
    {
        vm->rotation.z =
            utils::AddNormalizeAngle(vm->rotation.z, g_Supervisor.effectiveFramerateMultiplier * vm->angleVel.z);
    }
    if (vm->scaleInterpEndTime > 0)
    {
        vm->scaleInterpTime.Tick();
        if (vm->scaleInterpTime.AsFrames() >= vm->scaleInterpEndTime)
        {
            vm->scaleY = vm->scaleInterpFinalY;
            vm->scaleX = vm->scaleInterpFinalX;
            vm->scaleInterpEndTime = 0;
            vm->scaleInterpFinalY = 0.0;
            vm->scaleInterpFinalX = 0.0;
        }
        else
        {
            vm->scaleX = (vm->scaleInterpFinalX - vm->scaleInterpInitialX) * vm->scaleInterpTime.AsFramesFloat() /
                             vm->scaleInterpEndTime +
                         vm->scaleInterpInitialX;
            vm->scaleY = (vm->scaleInterpFinalY - vm->scaleInterpInitialY) * vm->scaleInterpTime.AsFramesFloat() /
                             vm->scaleInterpEndTime +
                         vm->scaleInterpInitialY;
        }
        if ((vm->flags.flip & 1) != 0)
        {
            vm->scaleX = vm->scaleX * -1.f;
        }
        if ((vm->flags.flip & 2) != 0)
        {
            vm->scaleY = vm->scaleY * -1.f;
        }
    }
    else
    {
        vm->scaleY = g_Supervisor.effectiveFramerateMultiplier * vm->scaleInterpFinalY + vm->scaleY;
        vm->scaleX = g_Supervisor.effectiveFramerateMultiplier * vm->scaleInterpFinalX + vm->scaleX;
    }
    if (0 < vm->alphaInterpEndTime)
    {
        vm->alphaInterpTime.Tick();
        local_2c = vm->alphaInterpInitial;
        local_28 = vm->alphaInterpFinal;
        local_30 = vm->alphaInterpTime.AsFramesFloat() / (f32)vm->alphaInterpEndTime;
        if (local_30 >= 1.0f)
        {
            local_30 = 1.0;
        }
        for (local_38 = 0; local_38 < 4; local_38++)
        {
            local_34 = ((f32)COLOR_GET_COMPONENT(local_28, local_38) - (f32)COLOR_GET_COMPONENT(local_2c, local_38)) *
                           local_30 +
                       COLOR_GET_COMPONENT(local_2c, local_38);
            if (local_34 < 0)
            {
                local_34 = 0;
            }
            COLOR_SET_COMPONENT(local_2c, local_38, local_34 >= 256 ? 255 : local_34);
        }
        vm->color = local_2c;
        if (vm->alphaInterpTime.AsFrames() >= vm->alphaInterpEndTime)
        {
            vm->alphaInterpEndTime = 0;
        }
    }
    if (vm->posInterpEndTime != 0)
    {
        local_3c = vm->posInterpTime.AsFramesFloat() / (f32)vm->posInterpEndTime;
        if (local_3c >= 1.0f)
        {
            local_3c = 1.0;
        }
        switch (vm->flags.posTime)
        {
        case 1:
            local_3c = 1.0f - local_3c;
            local_3c *= local_3c;
            local_3c = 1.0f - local_3c;
            break;
        case 2:
            local_3c = 1.0f - local_3c;
            local_3c = local_3c * local_3c * local_3c * local_3c;
            local_3c = 1.0f - local_3c;
            break;
        }
        if (vm->flags.usePosOffset == 0)
        {
            vm->pos.x = local_3c * vm->posInterpFinal.x + (1.0f - local_3c) * vm->posInterpInitial.x;
            vm->pos.y = local_3c * vm->posInterpFinal.y + (1.0f - local_3c) * vm->posInterpInitial.y;
            vm->pos.z = local_3c * vm->posInterpFinal.z + (1.0f - local_3c) * vm->posInterpInitial.z;
        }
        else
        {
            vm->posOffset.x = local_3c * vm->posInterpFinal.x + (1.0f - local_3c) * vm->posInterpInitial.x;
            vm->posOffset.y = local_3c * vm->posInterpFinal.y + (1.0f - local_3c) * vm->posInterpInitial.y;
            vm->posOffset.z = local_3c * vm->posInterpFinal.z + (1.0f - local_3c) * vm->posInterpInitial.z;
        }

        if (vm->posInterpTime.AsFrames() >= vm->posInterpEndTime)
        {
            vm->posInterpEndTime = 0;
        }
        vm->posInterpTime.Tick();
    }
    vm->currentTimeInScript.Tick();
    return 0;
}

#undef AnmI32Arg
#undef AnmF32Arg
#undef AnmU32Arg
#undef AnmI16Arg

void AnmManager::DrawTextToSprite(u32 textureDstIdx, i32 xPos, i32 yPos, i32 spriteWidth, i32 spriteHeight,
                                  i32 fontWidth, i32 fontHeight, ZunColor textColor, ZunColor shadowColor,
                                  const char *strToPrint)
{
    if (fontWidth <= 0)
    {
        fontWidth = 15;
    }
    if (fontHeight <= 0)
    {
        fontHeight = 15;
    }

    InvalidateSpriteExtrusionAtlas(this, static_cast<i32>(textureDstIdx));

    TextHelper::RenderTextToTexture(xPos, yPos, spriteWidth, spriteHeight, fontWidth, fontHeight, textColor,
                                    shadowColor, strToPrint, &this->textures[textureDstIdx]);
    //
    //    this->SetCurrentTexture(this->textures[textureDstIdx].handle);
    //    g_glFuncTable.glTexImage2D(GL_TEXTURE_2D, 0, g_TextureFormatGLFormatMapping[]);

    return;
}

#ifdef TH_ENABLE_THCRAP
static std::vector<char> FormatThcrapAnmText(const char *format, va_list args)
{
    // TH06 base_tsa removes the original 64-byte copies in two formatter
    // variants and redirects DrawStringFormat's 0x50-byte sprintf through
    // strings_vsprintf(), whose storage grows to the full formatted length.
    // Do not reintroduce a fixed 1024-byte ceiling in the portable build.
    va_list measureArgs;
    va_copy(measureArgs, args);
    const int length = std::vsnprintf(nullptr, 0, format, measureArgs);
    va_end(measureArgs);
    if (length < 0)
        return std::vector<char>(1, '\0');

    std::vector<char> output(static_cast<std::size_t>(length) + 1u);
    va_list writeArgs;
    va_copy(writeArgs, args);
    std::vsnprintf(output.data(), output.size(), format, writeArgs);
    va_end(writeArgs);
    return output;
}
#endif

void AnmManager::DrawVmTextFmt(AnmVm *vm, ZunColor textColor, ZunColor shadowColor, const char *fmt, ...)
{
    u32 fontWidth;
#ifndef TH_ENABLE_THCRAP
    char buffer[1024];
#endif
    va_list argptr;

    fontWidth = vm->fontWidth;
    va_start(argptr, fmt);
#ifdef TH_ENABLE_THCRAP
    std::vector<char> dynamicBuffer = FormatThcrapAnmText(fmt, argptr);
    const char *buffer = dynamicBuffer.data();
#else
    std::vsnprintf(buffer, sizeof(buffer), fmt, argptr);
#endif
    va_end(argptr);
    // base_tsa's text_sprite_width/text_sprite_height patches deliberately
    // switch these arguments from the backing texture dimensions (0x18/0x14)
    // to the active sprite dimensions (0x30/0x2c).  This matters for the
    // widened 320 px dialogue sprites stored in a 512 px texture.
    const i32 spriteWidth = Localization::Active() ? static_cast<i32>(vm->sprite->widthPx)
                                                    : vm->sprite->textureWidth;
    const i32 spriteHeight = Localization::Active() ? static_cast<i32>(vm->sprite->heightPx)
                                                     : vm->sprite->textureHeight;
    this->DrawTextToSprite(vm->sprite->sourceFileIndex, vm->sprite->startPixelInclusive.x,
                           vm->sprite->startPixelInclusive.y, spriteWidth, spriteHeight,
                           fontWidth, vm->fontHeight, textColor, shadowColor, buffer);
    vm->flags.isVisible = true;
    return;
}

void AnmManager::DrawStringFormat(AnmVm *vm, ZunColor textColor, ZunColor shadowColor, const char *fmt, ...)
{
#ifndef TH_ENABLE_THCRAP
    char buf[1024];
#endif
    va_list args;
    i32 fontWidth;
    i32 secondPartStartX;

    fontWidth = vm->fontWidth <= 0 ? 15 : vm->fontWidth;
    va_start(args, fmt);
#ifdef TH_ENABLE_THCRAP
    std::vector<char> dynamicBuf = FormatThcrapAnmText(fmt, args);
    const char *buf = dynamicBuf.data();
#else
    std::vsnprintf(buf, sizeof(buf), fmt, args);
#endif
    va_end(args);
    const i32 spriteWidth = Localization::Active() ? static_cast<i32>(vm->sprite->widthPx)
                                                    : vm->sprite->textureWidth;
    const i32 spriteHeight = Localization::Active() ? static_cast<i32>(vm->sprite->heightPx)
                                                     : vm->sprite->textureHeight;
    this->DrawTextToSprite(vm->sprite->sourceFileIndex, vm->sprite->startPixelInclusive.x,
                           vm->sprite->startPixelInclusive.y, spriteWidth, spriteHeight,
                           fontWidth, vm->fontHeight, textColor, shadowColor, " ");
    // base_tsa::right_align uses GetTextExtentForFontID() + 2 after the
    // helper has already converted the 2x GDI extent to on-screen pixels.
    const f32 renderedWidth = Localization::Active() ? TextHelper::MeasureTextWidth(buf, vm->fontHeight) + 2.0f
                                                     : (f32)strlen(buf) * (f32)(fontWidth + 1) / 2.0f;
    const f32 availableWidth = Localization::Active() ? vm->sprite->widthPx : vm->sprite->textureWidth;
    secondPartStartX = vm->sprite->startPixelInclusive.x + availableWidth - renderedWidth;
#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
    if (Localization::Active())
    {
        static bool loggedRightAlign = false;
        if (!loggedRightAlign)
        {
            SDL_Log("TH06 thcrap right-align: text=%s start=%.3f available=%.3f measuredPlus2=%.3f",
                    buf, static_cast<double>(secondPartStartX), static_cast<double>(availableWidth),
                    static_cast<double>(renderedWidth));
            loggedRightAlign = true;
        }
    }
#endif
    this->DrawTextToSprite(vm->sprite->sourceFileIndex, secondPartStartX, vm->sprite->startPixelInclusive.y,
                           spriteWidth, spriteHeight, fontWidth, vm->fontHeight, textColor,
                           shadowColor, buf);
    vm->flags.isVisible = true;
    return;
}

void AnmManager::DrawStringFormat2(AnmVm *vm, ZunColor textColor, ZunColor shadowColor, const char *fmt, ...)
{
#ifndef TH_ENABLE_THCRAP
    char buf[1024];
#endif
    va_list args;
    i32 fontWidth;
    i32 secondPartStartX;

    fontWidth = vm->fontWidth <= 0 ? 15 : vm->fontWidth;
    va_start(args, fmt);
#ifdef TH_ENABLE_THCRAP
    std::vector<char> dynamicBuf = FormatThcrapAnmText(fmt, args);
    const char *buf = dynamicBuf.data();
#else
    std::vsnprintf(buf, sizeof(buf), fmt, args);
#endif
    va_end(args);
    const i32 spriteWidth = Localization::Active() ? static_cast<i32>(vm->sprite->widthPx)
                                                    : vm->sprite->textureWidth;
    const i32 spriteHeight = Localization::Active() ? static_cast<i32>(vm->sprite->heightPx)
                                                     : vm->sprite->textureHeight;
    this->DrawTextToSprite(vm->sprite->sourceFileIndex, vm->sprite->startPixelInclusive.x,
                           vm->sprite->startPixelInclusive.y, spriteWidth, spriteHeight,
                           fontWidth, vm->fontHeight, textColor, shadowColor, " ");
    // base_tsa::center_align likewise adds 2 before halving the measured
    // width for the center offset, i.e. a one-pixel left shift on screen.
    const f32 renderedWidth = Localization::Active() ? TextHelper::MeasureTextWidth(buf, vm->fontHeight) + 2.0f
                                                     : (f32)strlen(buf) * (f32)(fontWidth + 1) / 2.0f;
    const f32 availableWidth = Localization::Active() ? vm->sprite->widthPx : vm->sprite->textureWidth;
    secondPartStartX = vm->sprite->startPixelInclusive.x + (availableWidth - renderedWidth) / 2.0f;
#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
    if (Localization::Active())
    {
        static bool loggedCenterAlign = false;
        if (!loggedCenterAlign)
        {
            SDL_Log("TH06 thcrap center-align: text=%s start=%.3f available=%.3f measuredPlus2=%.3f",
                    buf, static_cast<double>(secondPartStartX), static_cast<double>(availableWidth),
                    static_cast<double>(renderedWidth));
            loggedCenterAlign = true;
        }
    }
#endif
    this->DrawTextToSprite(vm->sprite->sourceFileIndex, secondPartStartX, vm->sprite->startPixelInclusive.y,
                           spriteWidth, spriteHeight, fontWidth, vm->fontHeight, textColor,
                           shadowColor, buf);
    vm->flags.isVisible = true;
    return;
}

ZunResult AnmManager::LoadSurface(i32 surfaceIdx, const char *path)
{
#ifdef __EMSCRIPTEN__
    u8 ownerId = 0;
    if (WebTransitionSurfaceCacheEntry *entry = FindWebTransitionSurface(path, &ownerId))
    {
        if (entry->surface == nullptr)
        {
            entry->surface = LoadToSurfaceWithFormat(path, SDL_PIXELFORMAT_RGB24, NULL);
            if (entry->surface == nullptr)
                return ZUN_ERROR;
        }

        ReleaseSurface(surfaceIdx);
        this->surfaces[surfaceIdx] = entry->surface;
        g_WebTransitionSurfaceOwner[surfaceIdx] = ownerId;
        return ZUN_SUCCESS;
    }
#endif

    if (this->surfaces[surfaceIdx] != NULL)
    {
        this->ReleaseSurface(surfaceIdx);
    }

    this->surfaces[surfaceIdx] = LoadToSurfaceWithFormat(path, SDL_PIXELFORMAT_RGB24, NULL);

    if (this->surfaces[surfaceIdx] == NULL)
    {
        return ZUN_ERROR;
    }

    return ZUN_SUCCESS;

    //    u8 *data = FileSystem::OpenPath(path, 0);
    //    if (data == NULL)
    //    {
    //        GameErrorContext::Fatal(&g_GameErrorContext, TH_ERR_CANNOT_BE_LOADED, path);
    //        return ZUN_ERROR;
    //    }
    //
    //    LPDIRECT3DSURFACE8 surface;
    //    if (g_Supervisor.d3dDevice->CreateImageSurface(0x280, 0x400, g_Supervisor.presentParameters.BackBufferFormat,
    //                                                   &surface) != D3D_OK)
    //    {
    //        return ZUN_ERROR;
    //    }
    //
    //    if (D3DXLoadSurfaceFromFileInMemory(surface, NULL, NULL, data, g_LastFileSize, NULL, D3DX_FILTER_NONE, 0,
    //                                        &this->surfaceSourceInfo[surfaceIdx]) != D3D_OK)
    //    {
    //        goto fail;
    //    }
    //    if (g_Supervisor.d3dDevice->CreateRenderTarget(this->surfaceSourceInfo[surfaceIdx].Width,
    //                                                   this->surfaceSourceInfo[surfaceIdx].Height,
    //                                                   g_Supervisor.presentParameters.BackBufferFormat,
    //                                                   D3DMULTISAMPLE_NONE, TRUE, &this->surfaces[surfaceIdx]) !=
    //                                                   D3D_OK &&
    //        g_Supervisor.d3dDevice->CreateImageSurface(
    //            this->surfaceSourceInfo[surfaceIdx].Width, this->surfaceSourceInfo[surfaceIdx].Height,
    //            g_Supervisor.presentParameters.BackBufferFormat, &this->surfaces[surfaceIdx]) != D3D_OK)
    //    {
    //        goto fail;
    //    }
    //    if (g_Supervisor.d3dDevice->CreateImageSurface(
    //            this->surfaceSourceInfo[surfaceIdx].Width, this->surfaceSourceInfo[surfaceIdx].Height,
    //            g_Supervisor.presentParameters.BackBufferFormat, &this->surfacesBis[surfaceIdx]) != D3D_OK)
    //    {
    //        goto fail;
    //    }
    //
    //    if (D3DXLoadSurfaceFromSurface(this->surfaces[surfaceIdx], NULL, NULL, surface, NULL, NULL, D3DX_FILTER_NONE,
    //    0) !=
    //        D3D_OK)
    //    {
    //        goto fail;
    //    }
    //
    //    if (D3DXLoadSurfaceFromSurface(this->surfacesBis[surfaceIdx], NULL, NULL, surface, NULL, NULL,
    //    D3DX_FILTER_NONE,
    //                                   0) != D3D_OK)
    //    {
    //        goto fail;
    //    }
    //
    //    if (surface != NULL)
    //    {
    //        surface->Release();
    //        surface = NULL;
    //    }
    //    free(data);
    //
    // fail:
    //    if (surface != NULL)
    //    {
    //        surface->Release();
    //        surface = NULL;
    //    }
    //    free(data);
    //    return ZUN_ERROR;
}

#ifdef __EMSCRIPTEN__
ZunResult AnmManager::PreloadTransitionSurface(const char *path)
{
    WebTransitionSurfaceCacheEntry *entry = FindWebTransitionSurface(path);
    if (!entry)
        return ZUN_ERROR;
    if (!entry->surface)
        entry->surface = LoadToSurfaceWithFormat(path, SDL_PIXELFORMAT_RGB24, NULL);
    return entry->surface ? ZUN_SUCCESS : ZUN_ERROR;
}
#endif

void AnmManager::ReleaseSurface(i32 surfaceIdx)
{
#ifdef __EMSCRIPTEN__
    if (g_WebTransitionSurfaceOwner[surfaceIdx] != 0)
    {
        this->surfaces[surfaceIdx] = NULL;
        g_WebTransitionSurfaceOwner[surfaceIdx] = 0;
        return;
    }
#endif

    if (this->surfaces[surfaceIdx] != NULL)
    {
        SDL_DestroySurface(this->surfaces[surfaceIdx]);
        this->surfaces[surfaceIdx] = NULL;
    }
}

void AnmManager::CopySurfaceToBackBuffer(i32 surfaceIdx, i32 srcX, i32 srcY, i32 dstX, i32 dstY)
{
    SDL_Surface *srcSurface = this->surfaces[surfaceIdx];

    if (srcSurface == NULL)
    {
        return;
    }

    CopySurfaceRectToBackBuffer(surfaceIdx, dstX, dstY, srcX, srcY, srcSurface->w - srcX, srcSurface->h - srcY);
    //
    //    IDirect3DSurface8 *destSurface;
    //    if (g_Supervisor.d3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &destSurface) != D3D_OK)
    //    {
    //        return;
    //    }
    //    if (this->surfaces[surfaceIdx] == NULL)
    //    {
    //        if (g_Supervisor.d3dDevice->CreateRenderTarget(
    //                this->surfaceSourceInfo[surfaceIdx].Width, this->surfaceSourceInfo[surfaceIdx].Height,
    //                g_Supervisor.presentParameters.BackBufferFormat, D3DMULTISAMPLE_NONE, TRUE,
    //                &this->surfaces[surfaceIdx]) != D3D_OK)
    //        {
    //            if (g_Supervisor.d3dDevice->CreateImageSurface(
    //                    this->surfaceSourceInfo[surfaceIdx].Width, this->surfaceSourceInfo[surfaceIdx].Height,
    //                    g_Supervisor.presentParameters.BackBufferFormat, &this->surfaces[surfaceIdx]) != D3D_OK)
    //            {
    //                destSurface->Release();
    //                return;
    //            }
    //        }
    //        if (D3DXLoadSurfaceFromSurface(this->surfaces[surfaceIdx], NULL, NULL, this->surfacesBis[surfaceIdx],
    //        NULL,
    //                                       NULL, D3DX_FILTER_NONE, 0) != D3D_OK)
    //        {
    //            destSurface->Release();
    //            return;
    //        }
    //    }
    //
    //    RECT sourceRect;
    //    POINT destPoint;
    //    sourceRect.left = left;
    //    sourceRect.top = top;
    //    sourceRect.right = this->surfaceSourceInfo[surfaceIdx].Width;
    //    sourceRect.bottom = this->surfaceSourceInfo[surfaceIdx].Height;
    //    destPoint.x = x;
    //    destPoint.y = y;
    //    g_Supervisor.d3dDevice->CopyRects(this->surfaces[surfaceIdx], &sourceRect, 1, destSurface, &destPoint);
    //    destSurface->Release();
}

void AnmManager::CopySurfaceRectToBackBuffer(i32 surfaceIdx, i32 dstX, i32 dstY, i32 rectLeft, i32 rectTop,
                                             i32 rectWidth, i32 rectHeight)
{
    SDL_Surface *srcSurface = this->surfaces[surfaceIdx];

    if (srcSurface == NULL)
    {
        return;
    }

    ApplySurfaceToColorBuffer(srcSurface, (SDL_Rect){.x = rectLeft, .y = rectTop, .w = rectWidth, .h = rectHeight},
                              (SDL_Rect){.x = dstX, .y = dstY, .w = rectWidth, .h = rectHeight});
    //
    //    IDirect3DSurface8 *D3D_Surface;
    //    if (g_Supervisor.d3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &D3D_Surface) != D3D_OK)
    //    {
    //        return;
    //    }
    //
    //    if (this->surfaces[surfaceIdx] == NULL)
    //    {
    //        if (g_Supervisor.d3dDevice->CreateRenderTarget(
    //                this->surfaceSourceInfo[surfaceIdx].Width, this->surfaceSourceInfo[surfaceIdx].Height,
    //                g_Supervisor.presentParameters.BackBufferFormat, D3DMULTISAMPLE_NONE, TRUE,
    //                &this->surfaces[surfaceIdx]) != D3D_OK)
    //        {
    //            if (g_Supervisor.d3dDevice->CreateImageSurface(
    //                    this->surfaceSourceInfo[surfaceIdx].Width, this->surfaceSourceInfo[surfaceIdx].Height,
    //                    g_Supervisor.presentParameters.BackBufferFormat, &this->surfaces[surfaceIdx]) != D3D_OK)
    //            {
    //                D3D_Surface->Release();
    //                return;
    //            }
    //        }
    //        if (D3DXLoadSurfaceFromSurface(this->surfaces[surfaceIdx], NULL, NULL, this->surfacesBis[surfaceIdx],
    //        NULL,
    //                                       NULL, D3DX_FILTER_NONE, 0) != D3D_OK)
    //        {
    //            D3D_Surface->Release();
    //            return;
    //        }
    //    }
    //
    //    RECT rect;
    //    POINT point;
    //    rect.left = rectLeft;
    //    rect.top = rectTop;
    //    rect.right = rectLeft + width;
    //    rect.bottom = rectTop + height;
    //    point.x = rectX;
    //    point.y = rectY;
    //    g_Supervisor.d3dDevice->CopyRects(this->surfaces[surfaceIdx], &rect, 1, D3D_Surface, &point);
    //    D3D_Surface->Release();
}

void AnmManager::TakeScreenshot(i32 textureId, i32 left, i32 top, i32 width, i32 height)
{
    u8 *backBufferPixels = NULL;
    u8 *dstFormatPixels = NULL;
    SDL_Surface *dstFormatSurface = NULL;
    SDL_Rect stretchDstRect;
    SDL_Rect stretchSrcRect;
    SDL_Surface *stretchedSurface = NULL;
    SDL_Surface *unstretchedSurface = NULL;

    // OpenGL throws an error specifically for negative W / H and pixels are undefined for 0 inputs.
    if (this->textures[textureId].handle == 0 || width <= 0 || height <= 0)
    {
        return;
    }

    InvalidateSpriteExtrusionAtlas(this, textureId);

    this->SetCurrentTexture(this->textures[textureId].handle);

    backBufferPixels =
        new u8[((u32)(width * WIDTH_RESOLUTION_SCALE + 1)) * ((u32)(height * HEIGHT_RESOLUTION_SCALE + 1)) * 4];

    g_GfxBackend->ReadPixels(left * WIDTH_RESOLUTION_SCALE + VIEWPORT_OFF_X,
                             GAME_WINDOW_HEIGHT_REAL - ((top + height) * HEIGHT_RESOLUTION_SCALE) - VIEWPORT_OFF_Y,
                             width * WIDTH_RESOLUTION_SCALE, height * HEIGHT_RESOLUTION_SCALE, backBufferPixels);

    unstretchedSurface = SDL_CreateSurfaceFrom(width * WIDTH_RESOLUTION_SCALE, height * HEIGHT_RESOLUTION_SCALE,
                                               SDL_PIXELFORMAT_RGBA32, backBufferPixels,
                                               width * WIDTH_RESOLUTION_SCALE * 4);
    stretchedSurface = SDL_CreateSurface(this->textures[textureId].width, this->textures[textureId].height,
                                         SDL_PIXELFORMAT_RGBA32);

    if (unstretchedSurface == NULL || stretchedSurface == NULL)
    {
        goto cleanup;
    }

    // GfxInterface::ReadPixels returns rows in top-to-bottom image order.
    // GlesGraphics normalizes OpenGL's bottom-up readback at that boundary,
    // so flipping again here would vertically mirror pause screenshots.

    stretchSrcRect.x = 0;
    stretchSrcRect.y = 0;
    stretchSrcRect.h = height * HEIGHT_RESOLUTION_SCALE;
    stretchSrcRect.w = width * WIDTH_RESOLUTION_SCALE;

    stretchDstRect.x = 0;
    stretchDstRect.y = 0;
    stretchDstRect.h = this->textures[textureId].height;
    stretchDstRect.w = this->textures[textureId].width;

    if (!SDL_StretchSurface(unstretchedSurface, &stretchSrcRect, stretchedSurface, &stretchDstRect,
                            SDL_SCALEMODE_LINEAR))
    {
        goto cleanup;
    }

    dstFormatSurface =
        SDL_ConvertSurface(stretchedSurface, g_TextureFormatSDLMapping[this->textures[textureId].format]);

    if (dstFormatSurface == NULL)
    {
        goto cleanup;
    }

    dstFormatPixels =
        ExtractSurfacePixels(dstFormatSurface, g_TextureFormatBytesPerPixel[this->textures[textureId].format]);

    g_GfxBackend->SetTextureImage(this->textures[textureId].width, this->textures[textureId].height,
                                  g_TextureFormatTypeGfxMapping[this->textures[textureId].format],
                                  g_TextureFormatTypeMapping[this->textures[textureId].format], dstFormatPixels);

cleanup:
    SDL_DestroySurface(unstretchedSurface);
    SDL_DestroySurface(stretchedSurface);
    SDL_DestroySurface(dstFormatSurface);
    delete[] backBufferPixels;
    delete[] dstFormatPixels;
}

// Utter mess that needs to be rewritten
void AnmManager::ApplySurfaceToColorBuffer(SDL_Surface *src, const SDL_Rect &srcRect, const SDL_Rect &dstRect)
{
    ZunViewport originalViewport;
    ZunViewport fullscreenViewport;

    if (srcRect.w <= 0 || srcRect.h <= 0)
    {
        return;
    }

    originalViewport.Get();

    fullscreenViewport.x = 0;
    fullscreenViewport.y = 0;
    fullscreenViewport.height = GAME_WINDOW_HEIGHT;
    fullscreenViewport.width = GAME_WINDOW_WIDTH;
    fullscreenViewport.minZ = 0.0f;
    fullscreenViewport.maxZ = 1.0f;

    fullscreenViewport.Set();

    // Original TH06 used IDirect3DDevice8::CopyRects for this surface copy.
    // That operation bypassed the 3D render pipeline entirely. The GLES
    // replacement below renders a textured quad, so gameplay fog must not be
    // allowed to shade a menu/result background by view-space distance.
    const bool restoreFog = g_Supervisor.DisableFog() != 0;

    this->SetProjectionMode(PROJECTION_MODE_ORTHOGRAPHIC);

    CreateTextureObject();

    u32 textureWidth = BitCeil((u32)src->w);
    u32 textureHeight = BitCeil((u32)src->h);

    g_GfxBackend->SetTextureImage(textureWidth, textureHeight, PIXEL_RGB, PIXEL_UNSIGNED_BYTE, NULL);

    u8 *surfaceData = ExtractSurfacePixels(src, 3);

    g_GfxBackend->SetTextureSubImage(0, 0, src->w, src->h, surfaceData);

    delete[] surfaceData;

    VertexTex1DiffuseXyz verts[4];

    verts[0].position = ZunVec3(dstRect.x, dstRect.y, 0.0f);
    verts[1].position = ZunVec3(dstRect.x + dstRect.w, dstRect.y, 0.0f);
    verts[2].position = ZunVec3(dstRect.x, dstRect.y + dstRect.h, 0.0f);
    verts[3].position = ZunVec3(dstRect.x + dstRect.w, dstRect.y + dstRect.h, 0.0f);

    verts[0].textureUV = ZunVec2(0.0f, 0.0f);
    verts[1].textureUV = ZunVec2(((f32)src->w) / textureWidth, 0.0f);
    verts[2].textureUV = ZunVec2(0.0f, ((f32)src->h) / textureHeight);
    verts[3].textureUV = ZunVec2(((f32)src->w) / textureWidth, ((f32)src->h) / textureHeight);

    this->SetVertexAttributes(VERTEX_ATTR_TEX_COORD);

    this->SetAttributePointer(VERTEX_ARRAY_POSITION, sizeof(*verts), &verts[0].position);
    this->SetAttributePointer(VERTEX_ARRAY_TEX_COORD, sizeof(*verts), &verts[0].textureUV);

    this->SetColorOp(COMPONENT_ALPHA, COLOR_OP_REPLACE);
    this->SetColorOp(COMPONENT_RGB, COLOR_OP_REPLACE);

    this->SetDepthMask(false);
    this->SetDepthFunc(DEPTH_FUNC_ALWAYS);

    this->BackendDrawCall();

    this->SetColorOp(COMPONENT_ALPHA, COLOR_OP_MODULATE);
    this->SetColorOp(COMPONENT_RGB, COLOR_OP_MODULATE);

    g_GfxBackend->DeleteTexture(this->currentTextureHandle);

    this->SetCurrentSprite(NULL);
    this->SetCurrentTexture(0);
    this->SetCurrentBlendMode(0xff);

    if (restoreFog)
    {
        g_Supervisor.EnableFog();
    }

    originalViewport.Set();
}
