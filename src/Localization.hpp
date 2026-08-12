#pragma once

#include <cstddef>
#include <cstdint>

namespace Localization
{
const char *SpellName(std::uint32_t id, const char *fallback);
const char *StageName(std::uint32_t id, const char *fallback);
const char *MusicTitle(std::uint32_t track, const char *fallback);
const char *MusicComment(std::uint32_t track, std::uint16_t line, const char *fallback);
void CopyText(char *destination, std::size_t capacity, const char *source);
void CopyCodepointChunk(char *destination, std::size_t capacity, const char *source,
                        std::size_t firstCodepoint, std::size_t maximumCodepoints);
void CopyDisplayColumnChunk(char *destination, std::size_t capacity, const char *source,
                            std::size_t firstColumn, std::size_t maximumColumns);
} // namespace Localization
