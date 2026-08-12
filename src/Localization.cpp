#include "Localization.hpp"

#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>

#include "FileSystem.hpp"

namespace
{
std::uint16_t ReadU16(const unsigned char *data)
{
    return static_cast<std::uint16_t>(data[0]) |
           (static_cast<std::uint16_t>(data[1]) << 8);
}

std::uint32_t ReadU32(const unsigned char *data)
{
    return static_cast<std::uint32_t>(data[0]) |
           (static_cast<std::uint32_t>(data[1]) << 8) |
           (static_cast<std::uint32_t>(data[2]) << 16) |
           (static_cast<std::uint32_t>(data[3]) << 24);
}

std::size_t Utf8SequenceLength(const unsigned char *text)
{
    const unsigned char first = text[0];
    std::size_t length = 1;
    if ((first & 0xe0) == 0xc0)
        length = 2;
    else if ((first & 0xf0) == 0xe0)
        length = 3;
    else if ((first & 0xf8) == 0xf0)
        length = 4;
    for (std::size_t index = 1; index < length; index++)
        if (text[index] == 0 || (text[index] & 0xc0) != 0x80)
            return 1;
    return length;
}

bool IsValidUtf8(const unsigned char *text)
{
    while (*text != 0)
    {
        const unsigned char first = *text;
        std::size_t length;
        if (first < 0x80)
            length = 1;
        else if (first >= 0xc2 && first <= 0xdf)
            length = 2;
        else if (first >= 0xe0 && first <= 0xef)
            length = 3;
        else if (first >= 0xf0 && first <= 0xf4)
            length = 4;
        else
            return false;
        for (std::size_t index = 1; index < length; index++)
            if (text[index] == 0 || (text[index] & 0xc0) != 0x80)
                return false;
        if ((length == 3 && first == 0xe0 && text[1] < 0xa0) ||
            (length == 3 && first == 0xed && text[1] >= 0xa0) ||
            (length == 4 && first == 0xf0 && text[1] < 0x90) ||
            (length == 4 && first == 0xf4 && text[1] >= 0x90))
            return false;
        text += length;
    }
    return true;
}

std::size_t TextUnitLength(const unsigned char *text, bool utf8)
{
    if (utf8)
        return Utf8SequenceLength(text);
    const unsigned char first = text[0];
    const bool shiftJisLead = (first >= 0x81 && first <= 0x9f) || (first >= 0xe0 && first <= 0xfc);
    return shiftJisLead && text[1] != 0 ? 2 : 1;
}

class Table
{
public:
    const char *Lookup(const char *path, std::uint32_t key, std::uint16_t line, const char *fallback,
                       bool blankMissing = false)
    {
        Load(path);
        const std::uint64_t packed = (static_cast<std::uint64_t>(key) << 16) | line;
        const auto found = entries.find(packed);
        return found == entries.end() ? (blankMissing && available ? "" : fallback) : found->second.c_str();
    }

private:
    void Load(const char *path)
    {
        if (loaded)
            return;
        loaded = true;
        unsigned char *data = FileSystem::OpenRuntimeOverride(path);
        if (data == nullptr)
            return;
        const std::size_t size = g_LastFileSize;
        if (size < 8 || std::memcmp(data, "ETL1", 4) != 0)
        {
            std::free(data);
            return;
        }
        available = true;
        const std::uint32_t count = ReadU32(data + 4);
        std::size_t offset = 8;
        for (std::uint32_t index = 0; index < count; index++)
        {
            if (offset > size || size - offset < 8)
                break;
            const std::uint32_t key = ReadU32(data + offset);
            const std::uint16_t line = ReadU16(data + offset + 4);
            const std::uint16_t length = ReadU16(data + offset + 6);
            offset += 8;
            if (offset > size || size - offset < length)
                break;
            const std::uint64_t packed = (static_cast<std::uint64_t>(key) << 16) | line;
            entries[packed] = std::string(reinterpret_cast<const char *>(data + offset), length);
            offset += length;
        }
        std::free(data);
    }

    bool loaded = false;
    bool available = false;
    std::unordered_map<std::uint64_t, std::string> entries;
};

Table g_Spells;
Table g_Stages;
Table g_Themes;
Table g_MusicComments;
} // namespace

const char *Localization::SpellName(std::uint32_t id, const char *fallback)
{
    return g_Spells.Lookup("localization/spells.etl", id, 0, fallback);
}

const char *Localization::StageName(std::uint32_t id, const char *fallback)
{
    return g_Stages.Lookup("localization/stages.etl", id, 0, fallback);
}

const char *Localization::MusicTitle(std::uint32_t track, const char *fallback)
{
    return g_Themes.Lookup("localization/themes.etl", track, 0, fallback);
}

const char *Localization::MusicComment(std::uint32_t track, std::uint16_t line, const char *fallback)
{
    return g_MusicComments.Lookup("localization/musiccmt.etl", track, line, fallback, true);
}

void Localization::CopyCodepointChunk(char *destination, std::size_t capacity, const char *source,
                                      std::size_t firstCodepoint, std::size_t maximumCodepoints)
{
    if (destination == nullptr || capacity == 0)
        return;
    destination[0] = '\0';
    if (source == nullptr)
        return;
    const unsigned char *cursor = reinterpret_cast<const unsigned char *>(source);
    const bool utf8 = IsValidUtf8(cursor);
    for (std::size_t index = 0; index < firstCodepoint && *cursor != 0; index++)
        cursor += TextUnitLength(cursor, utf8);
    std::size_t written = 0;
    for (std::size_t index = 0; index < maximumCodepoints && *cursor != 0; index++)
    {
        const std::size_t length = TextUnitLength(cursor, utf8);
        if (written + length >= capacity)
            break;
        std::memcpy(destination + written, cursor, length);
        written += length;
        cursor += length;
    }
    destination[written] = '\0';
}

void Localization::CopyText(char *destination, std::size_t capacity, const char *source)
{
    if (destination == source)
        return;
    CopyCodepointChunk(destination, capacity, source, 0, static_cast<std::size_t>(-1));
}

void Localization::CopyDisplayColumnChunk(char *destination, std::size_t capacity, const char *source,
                                          std::size_t firstColumn, std::size_t maximumColumns)
{
    if (destination == nullptr || capacity == 0)
        return;
    destination[0] = '\0';
    if (source == nullptr)
        return;
    const unsigned char *cursor = reinterpret_cast<const unsigned char *>(source);
    const bool utf8 = IsValidUtf8(cursor);
    std::size_t column = 0;
    while (*cursor != 0 && column < firstColumn)
    {
        const std::size_t length = TextUnitLength(cursor, utf8);
        column += length == 1 && *cursor < 0x80 ? 1 : 2;
        cursor += length;
    }
    std::size_t written = 0;
    const std::size_t endColumn = firstColumn + maximumColumns;
    while (*cursor != 0)
    {
        const std::size_t length = TextUnitLength(cursor, utf8);
        const std::size_t columns = length == 1 && *cursor < 0x80 ? 1 : 2;
        if (column + columns > endColumn || written + length >= capacity)
            break;
        std::memcpy(destination + written, cursor, length);
        written += length;
        cursor += length;
        column += columns;
    }
    destination[written] = '\0';
}
