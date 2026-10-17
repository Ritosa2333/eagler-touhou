#pragma once

#include <cstddef>
#include <cstdint>

struct AnmVm;

namespace Localization
{
struct AsciiEntryView
{
    const char *id;
    const char *text;
    const char *baseline;
    float extraX;
    bool hasTranslation;
    bool hasAlignment;
};

bool Active();
const char *FontFile();
const char *FontName();
const char *SpellName(std::uint32_t id, const char *fallback);
const char *StageName(std::uint32_t id, const char *fallback);
const char *MusicTitle(std::uint32_t track, const char *fallback);
const char *MusicComment(std::uint32_t track, std::uint16_t line, const char *fallback);
bool LookupAscii(const char *fallback, AsciiEntryView &view);
const char *AsciiString(const char *fallback);
const char *AsciiStringById(const char *id, const char *fallback);
const char *StringById(const char *id, const char *fallback);
const char *FormatStringById(const char *id, const char *fallback);
const char *LogString(const char *fallback);
bool ApplyBossTitleImage(AnmVm *vm, std::uint32_t stage, std::int32_t rightPortraitSprite);
bool ApplyBossNameImage(AnmVm *vm, std::uint32_t stage, std::int32_t rightPortraitSprite);
#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
bool DebugBossImageRowContractSelfTest();
bool DebugAsciiTableSelfTest();
bool DebugStringTableSelfTest();
#endif
void CopyText(char *destination, std::size_t capacity, const char *source);
void CopyCodepointChunk(char *destination, std::size_t capacity, const char *source,
                        std::size_t firstCodepoint, std::size_t maximumCodepoints);
void CopyDisplayColumnChunk(char *destination, std::size_t capacity, const char *source,
                            std::size_t firstColumn, std::size_t maximumColumns);
} // namespace Localization
