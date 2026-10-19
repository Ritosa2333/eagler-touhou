#include "Localization.hpp"

#include <cstring>

namespace
{
std::size_t UnitLength(const unsigned char *text)
{
    if (*text < 0x80) return 1;
    if ((*text & 0xe0) == 0xc0 && text[1]) return 2;
    if ((*text & 0xf0) == 0xe0 && text[1] && text[2]) return 3;
    if ((*text & 0xf8) == 0xf0 && text[1] && text[2] && text[3]) return 4;
    return text[1] ? 2 : 1;
}
}

const char *Localization::SpellName(std::uint32_t, const char *fallback) { return fallback; }
const char *Localization::StageName(std::uint32_t, const char *fallback) { return fallback; }
const char *Localization::MusicTitle(std::uint32_t, const char *fallback) { return fallback; }
const char *Localization::MusicComment(std::uint32_t, std::uint16_t, const char *fallback) { return fallback; }
bool Localization::LookupAscii(const char *, AsciiEntryView &) { return false; }
const char *Localization::AsciiString(const char *fallback) { return fallback; }
const char *Localization::AsciiStringById(const char *, const char *fallback) { return fallback; }
const char *Localization::StringById(const char *, const char *fallback) { return fallback; }
const char *Localization::FormatStringById(const char *, const char *fallback) { return fallback; }
const char *Localization::LogString(const char *fallback) { return fallback; }
bool Localization::Active() { return false; }
const char *Localization::FontFile() { return nullptr; }
const char *Localization::FontName() { return nullptr; }
bool Localization::ApplyBossTitleImage(AnmVm *, std::uint32_t, std::int32_t) { return false; }
bool Localization::ApplyBossNameImage(AnmVm *, std::uint32_t, std::int32_t) { return false; }
#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
bool Localization::DebugBossImageRowContractSelfTest() { return false; }
bool Localization::DebugAsciiTableSelfTest() { return false; }
bool Localization::DebugStringTableSelfTest() { return false; }
#endif
void Localization::CopyCodepointChunk(char *destination, std::size_t capacity, const char *source, std::size_t first, std::size_t maximum)
{
    if (!destination || capacity == 0) return; destination[0] = '\0'; if (!source) return;
    const auto *cursor = reinterpret_cast<const unsigned char *>(source);
    for (std::size_t i = 0; i < first && *cursor; ++i) cursor += UnitLength(cursor);
    std::size_t written = 0;
    for (std::size_t i = 0; i < maximum && *cursor; ++i) { const auto n = UnitLength(cursor); if (written + n >= capacity) break; std::memcpy(destination + written, cursor, n); written += n; cursor += n; }
    destination[written] = '\0';
}
void Localization::CopyText(char *destination, std::size_t capacity, const char *source) { if (destination != source) CopyCodepointChunk(destination, capacity, source, 0, static_cast<std::size_t>(-1)); }
void Localization::CopyDisplayColumnChunk(char *destination, std::size_t capacity, const char *source, std::size_t first, std::size_t maximum)
{
    if (!destination || capacity == 0) return; destination[0] = '\0'; if (!source) return;
    const auto *cursor = reinterpret_cast<const unsigned char *>(source); std::size_t column = 0;
    while (*cursor && column < first) { const auto n = UnitLength(cursor); column += n == 1 ? 1 : 2; cursor += n; }
    std::size_t written = 0;
    while (*cursor && column < first + maximum) { const auto n = UnitLength(cursor); const auto width = n == 1 ? 1u : 2u; if (column + width > first + maximum || written + n >= capacity) break; std::memcpy(destination + written, cursor, n); written += n; cursor += n; column += width; }
    destination[written] = '\0';
}
