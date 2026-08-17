#include "Localization.hpp"

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>

#ifdef TH_DEV_TOOLS
#include <SDL3/SDL_log.h>
#endif

#include "AnmManager.hpp"
#include "FileSystem.hpp"

namespace
{
std::uint16_t ReadU16(const unsigned char *data)
{
    return static_cast<std::uint16_t>(data[0]) |
           (static_cast<std::uint16_t>(data[1]) << 8);
}

std::int16_t ReadI16(const unsigned char *data)
{
    return static_cast<std::int16_t>(ReadU16(data));
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
        const std::uint32_t count = ReadU32(data + 4);
        std::size_t offset = 8;
        std::unordered_map<std::uint64_t, std::string> parsed;
        for (std::uint32_t index = 0; index < count; index++)
        {
            if (offset > size || size - offset < 8)
            {
                std::free(data);
                return;
            }
            const std::uint32_t key = ReadU32(data + offset);
            const std::uint16_t line = ReadU16(data + offset + 4);
            const std::uint16_t length = ReadU16(data + offset + 6);
            offset += 8;
            if (offset > size || size - offset < length)
            {
                std::free(data);
                return;
            }
            const std::uint64_t packed = (static_cast<std::uint64_t>(key) << 16) | line;
            parsed[packed] = std::string(reinterpret_cast<const char *>(data + offset), length);
            offset += length;
        }
        entries = std::move(parsed);
        available = true;
        std::free(data);
    }

    bool loaded = false;
    bool available = false;
    std::unordered_map<std::uint64_t, std::string> entries;
};

struct AsciiRecord
{
    std::string id;
    std::string translation;
    std::string baseline;
    float extraX = 0.0f;
    bool hasTranslation = false;
    bool hasAlignment = false;
};

struct AsciiIdValue
{
    std::string translation;
    bool hasTranslation = false;
};

class AsciiTable
{
public:
    const AsciiRecord *Lookup(const char *fallback)
    {
        Load();
        if (!available || fallback == nullptr)
            return nullptr;
        const auto found = aliases.find(fallback);
        return found == aliases.end() ? nullptr : &found->second;
    }

    const AsciiIdValue *LookupId(const char *id)
    {
        Load();
        if (!available || id == nullptr)
            return nullptr;
        const auto found = ids.find(id);
        return found == ids.end() ? nullptr : &found->second;
    }

private:
    static bool DecodeString(const unsigned char *data, std::size_t size, std::size_t &offset,
                             std::uint16_t length, std::string &value, bool allowEmpty)
    {
        if (offset > size || size - offset < length)
            return false;
        if (!allowEmpty && length == 0)
            return false;
        if (length != 0 && std::memchr(data + offset, 0, length) != nullptr)
            return false;
        value.assign(reinterpret_cast<const char *>(data + offset), length);
        offset += length;
        return IsValidUtf8(reinterpret_cast<const unsigned char *>(value.c_str()));
    }

    void Load()
    {
        if (loaded)
            return;
        loaded = true;

        unsigned char *data = FileSystem::OpenRuntimeOverride("localization/ascii.etl");
        if (data == nullptr)
            return;
        const std::size_t size = g_LastFileSize;
        bool valid = size >= 8 && std::memcmp(data, "EAS1", 4) == 0;
        std::unordered_map<std::string, AsciiRecord> nextAliases;
        std::unordered_map<std::string, AsciiIdValue> nextIds;
        std::size_t offset = 8;
        std::uint32_t count = valid ? ReadU32(data + 4) : 0;
        if (count > 4096)
            valid = false;

        for (std::uint32_t index = 0; valid && index < count; index++)
        {
            if (offset > size || size - offset < 12)
            {
                valid = false;
                break;
            }
            const std::uint16_t aliasLength = ReadU16(data + offset);
            const std::uint16_t idLength = ReadU16(data + offset + 2);
            const std::uint16_t translationLength = ReadU16(data + offset + 4);
            const std::uint16_t baselineLength = ReadU16(data + offset + 6);
            const std::int16_t extraHalf = ReadI16(data + offset + 8);
            const std::uint16_t flags = ReadU16(data + offset + 10);
            offset += 12;
            if ((flags & ~0x3u) != 0)
            {
                valid = false;
                break;
            }

            std::string alias;
            AsciiRecord record;
            if (!DecodeString(data, size, offset, aliasLength, alias, true) ||
                !DecodeString(data, size, offset, idLength, record.id, false) ||
                !DecodeString(data, size, offset, translationLength, record.translation, true) ||
                !DecodeString(data, size, offset, baselineLength, record.baseline, true))
            {
                valid = false;
                break;
            }
            record.hasTranslation = (flags & 1u) != 0;
            record.hasAlignment = (flags & 2u) != 0;
            record.extraX = static_cast<float>(extraHalf) * 0.5f;
            if ((!record.hasTranslation && !record.translation.empty()) ||
                (!record.hasAlignment && (!record.baseline.empty() || extraHalf != 0)) ||
                (record.hasAlignment && record.baseline.empty()))
            {
                valid = false;
                break;
            }

            const auto idFound = nextIds.find(record.id);
            if (idFound == nextIds.end())
                nextIds.emplace(record.id, AsciiIdValue{record.translation, record.hasTranslation});
            else if (idFound->second.hasTranslation != record.hasTranslation ||
                     idFound->second.translation != record.translation)
            {
                valid = false;
                break;
            }

            if (!alias.empty() && !nextAliases.emplace(alias, std::move(record)).second)
            {
                valid = false;
                break;
            }
        }
        if (valid && offset != size)
            valid = false;
        std::free(data);

        if (!valid)
            return;
        aliases.swap(nextAliases);
        ids.swap(nextIds);
        available = true;
    }

    bool loaded = false;
    bool available = false;
    std::unordered_map<std::string, AsciiRecord> aliases;
    std::unordered_map<std::string, AsciiIdValue> ids;
};

struct StringRecord
{
    std::string translation;
    bool hasTranslation = false;
};

class StringTable
{
public:
    const StringRecord *Lookup(const char *id)
    {
        Load();
        if (!available || id == nullptr)
            return nullptr;
        const auto found = entries.find(id);
        return found == entries.end() ? nullptr : &found->second;
    }

private:
    static bool DecodeString(const unsigned char *data, std::size_t size, std::size_t &offset,
                             std::uint16_t length, std::string &value, bool allowEmpty)
    {
        if (offset > size || size - offset < length)
            return false;
        if (!allowEmpty && length == 0)
            return false;
        if (length != 0 && std::memchr(data + offset, 0, length) != nullptr)
            return false;
        value.assign(reinterpret_cast<const char *>(data + offset), length);
        offset += length;
        return IsValidUtf8(reinterpret_cast<const unsigned char *>(value.c_str()));
    }

    void Load()
    {
        if (loaded)
            return;
        loaded = true;

        unsigned char *data = FileSystem::OpenRuntimeOverride("localization/strings.etl");
        if (data == nullptr)
            return;
        const std::size_t size = g_LastFileSize;
        bool valid = size >= 8 && std::memcmp(data, "EST1", 4) == 0;
        std::unordered_map<std::string, StringRecord> parsed;
        std::size_t offset = 8;
        const std::uint32_t count = valid ? ReadU32(data + 4) : 0;
        if (count > 4096)
            valid = false;

        for (std::uint32_t index = 0; valid && index < count; index++)
        {
            if (offset > size || size - offset < 8)
            {
                valid = false;
                break;
            }
            const std::uint16_t idLength = ReadU16(data + offset);
            const std::uint16_t translationLength = ReadU16(data + offset + 2);
            const std::uint16_t flags = ReadU16(data + offset + 4);
            const std::uint16_t reserved = ReadU16(data + offset + 6);
            offset += 8;
            if ((flags & ~0x1u) != 0 || reserved != 0)
            {
                valid = false;
                break;
            }

            std::string id;
            StringRecord record;
            if (!DecodeString(data, size, offset, idLength, id, false) ||
                !DecodeString(data, size, offset, translationLength, record.translation, true))
            {
                valid = false;
                break;
            }
            record.hasTranslation = (flags & 1u) != 0;
            if ((!record.hasTranslation && !record.translation.empty()) ||
                !parsed.emplace(std::move(id), std::move(record)).second)
            {
                valid = false;
                break;
            }
        }
        if (valid && offset != size)
            valid = false;
        std::free(data);

        if (!valid)
            return;
        entries.swap(parsed);
        available = true;
    }

    bool loaded = false;
    bool available = false;
    std::unordered_map<std::string, StringRecord> entries;
};

static bool ParsePrintfSignature(const char *format, std::string &signature)
{
    signature.clear();
    if (format == nullptr)
        return false;
    for (const char *cursor = format; *cursor != '\0'; ++cursor)
    {
        if (*cursor != '%')
            continue;
        ++cursor;
        if (*cursor == '%')
            continue;
        if (*cursor == '\0')
            return false;
        while (*cursor == '-' || *cursor == '+' || *cursor == ' ' || *cursor == '#' ||
               *cursor == '0' || *cursor == '\'')
            ++cursor;
        if (*cursor == '*')
            return false;
        while (std::isdigit(static_cast<unsigned char>(*cursor)))
            ++cursor;
        if (*cursor == '.')
        {
            ++cursor;
            if (*cursor == '*')
                return false;
            while (std::isdigit(static_cast<unsigned char>(*cursor)))
                ++cursor;
        }
        if (*cursor == 'h' || *cursor == 'l' || *cursor == 'j' || *cursor == 'z' ||
            *cursor == 't' || *cursor == 'L')
            return false;
        switch (*cursor)
        {
        case 'd':
        case 'i': signature.push_back('i'); break;
        case 'u':
        case 'o':
        case 'x':
        case 'X': signature.push_back('u'); break;
        case 'c': signature.push_back('c'); break;
        case 's': signature.push_back('s'); break;
        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
        case 'a':
        case 'A': signature.push_back('f'); break;
        default: return false;
        }
    }
    return true;
}

Table g_Spells;
Table g_Stages;
Table g_Themes;
Table g_MusicComments;
AsciiTable g_Ascii;
StringTable g_Strings;

struct Options
{
    static bool ReadString(const std::string &json, const char *name, std::string &value)
    {
        const std::string key = std::string("\"") + name + "\"";
        std::size_t cursor = json.find(key);
        if (cursor == std::string::npos)
            return false;
        cursor = json.find(':', cursor + key.size());
        if (cursor == std::string::npos)
            return false;
        cursor++;
        while (cursor < json.size() && std::isspace(static_cast<unsigned char>(json[cursor])))
            cursor++;
        if (cursor >= json.size() || json[cursor++] != '"')
            return false;
        const std::size_t end = json.find('"', cursor);
        if (end == std::string::npos)
            return false;
        value.assign(json, cursor, end - cursor);
        return !value.empty();
    }

    void Load()
    {
        if (loaded)
            return;
        loaded = true;
        unsigned char *data = FileSystem::OpenRuntimeOverride("localization/options.json");
        if (data == nullptr)
            return;
        const std::string json(reinterpret_cast<const char *>(data), g_LastFileSize);
        std::free(data);

        if (!ReadString(json, "fontFile", fontFile))
            return;
        if (fontFile.empty() || fontFile.find("..") != std::string::npos ||
            fontFile.find('/') != std::string::npos || fontFile.find('\\') != std::string::npos ||
            fontFile.find(':') != std::string::npos)
        {
            fontFile.clear();
            return;
        }
        ReadString(json, "font", fontName);
        available = true;
    }

    bool loaded = false;
    bool available = false;
    std::string fontFile;
    std::string fontName;
};

Options g_Options;
bool g_StageImageAttempted = false;
bool g_StageImageReady = false;
bool g_StageLogoImageAttempted = false;
bool g_StageLogoImageReady = false;
bool g_StageLogoImageActive = false;
bool g_MusicImageAttempted = false;
bool g_MusicImageReady = false;
bool g_BossTitleImageAttempted = false;
bool g_BossTitleImageReady = false;
bool g_BossNameImageAttempted = false;
bool g_BossNameImageReady = false;

bool LoadTextImage(i32 textureSlot, const char *path, bool &attempted, bool &ready)
{
    if (!attempted)
    {
        attempted = true;
        // thcrap's textimage loader passes ColorKey=0 to D3DX and preserves
        // the PNG's complete RGBA data.  In particular, the shipped title
        // atlases contain opaque and semi-transparent black pixels that form
        // the authored shadow/outline.  COLOR_BLACK is nonzero (0xff000000)
        // and makes our SDL loader erase those pixels as a color key.
        ready = g_AnmManager != nullptr &&
                g_AnmManager->LoadTexture(textureSlot, path, TEX_FMT_A8R8G8B8, 0, false, true) == ZUN_SUCCESS;
    }
    return ready;
}

bool ApplyTextImage(AnmVm *vm, u32 spriteSlot, i32 textureSlot, i32 row, i32 width, i32 height)
{
    if (vm == nullptr || g_AnmManager == nullptr || row < 0)
        return false;
    const TextureData &texture = g_AnmManager->textures[textureSlot];
    const i32 top = row * height;
    if (texture.width < static_cast<u32>(width) || texture.height < static_cast<u32>(top + height))
        return false;

    AnmLoadedSprite sprite{};
    sprite.sourceFileIndex = textureSlot;
    sprite.startPixelInclusive = ZunVec2(0.0f, static_cast<f32>(top));
    sprite.endPixelInclusive = ZunVec2(static_cast<f32>(width), static_cast<f32>(top + height));
    sprite.textureWidth = static_cast<f32>(texture.width);
    sprite.textureHeight = static_cast<f32>(texture.height);
    g_AnmManager->LoadSprite(spriteSlot, &sprite);
    if (g_AnmManager->SetActiveSprite(vm, spriteSlot) != ZUN_SUCCESS)
        return false;

    // Keep SetActiveSprite()'s original TH06 matrix contract here:
    //   matrix = logical sprite size / backing texture size.
    // base_tsa's sprite3d_*_voodookill patches then perform the *single*
    // backingTexture/256 compensation at draw time.  Applying width/256 here
    // as well would compensate twice, stretching a 384px text image by an
    // extra 384/256 horizontally and similarly distorting the vertical axis.
    return true;
}

// Byte-for-byte equivalent of base_tsa/th06.js's replacement script for
// ti_bgm.png, except that sprite 0xff + ANM_OFFSET_TEXT selects our private
// 0x7ff slot. It slides the right-aligned 384x32 image in from x=384 to x=0,
// holds it, then exits below the playfield after 450 frames.
alignas(4) const u8 kMusicTitleScript[] = {
    0x00, 0x00, 0x01, 0x04, 0xff, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x17, 0x00,
    0x00, 0x00, 0x11, 0x0c, 0x00, 0x00, 0xc0, 0x43, 0x00, 0x00, 0xd0, 0x43, 0x00, 0x00, 0x00, 0x00,
    0x78, 0x00, 0x13, 0x10, 0x00, 0x00, 0x00, 0xbf, 0x00, 0xc0, 0xcf, 0x43, 0x00, 0x00, 0x00, 0x00,
    0x2d, 0x00, 0x00, 0x00,
    0xa4, 0x01, 0x13, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe8, 0x43, 0x00, 0x00, 0x00, 0x00,
    0x1e, 0x00, 0x00, 0x00,
    0xc2, 0x01, 0x00, 0x00,
};
} // namespace

bool Localization::Active()
{
    g_Options.Load();
    return g_Options.available;
}

const char *Localization::FontFile()
{
    g_Options.Load();
    return g_Options.available ? g_Options.fontFile.c_str() : nullptr;
}

const char *Localization::FontName()
{
    g_Options.Load();
    return g_Options.available && !g_Options.fontName.empty() ? g_Options.fontName.c_str() : nullptr;
}

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

bool Localization::LookupAscii(const char *fallback, AsciiEntryView &view)
{
    const AsciiRecord *record = g_Ascii.Lookup(fallback);
    if (record == nullptr)
        return false;
    view.id = record->id.c_str();
    view.text = record->hasTranslation ? record->translation.c_str() : fallback;
    view.baseline = record->baseline.c_str();
    view.extraX = record->extraX;
    view.hasTranslation = record->hasTranslation;
    view.hasAlignment = record->hasAlignment;
    return true;
}

const char *Localization::AsciiString(const char *fallback)
{
    AsciiEntryView view{};
    return LookupAscii(fallback, view) ? view.text : fallback;
}

const char *Localization::AsciiStringById(const char *id, const char *fallback)
{
    const AsciiIdValue *value = g_Ascii.LookupId(id);
    return value != nullptr && value->hasTranslation ? value->translation.c_str() : fallback;
}

const char *Localization::StringById(const char *id, const char *fallback)
{
    const StringRecord *record = g_Strings.Lookup(id);
    const char *result = record != nullptr && record->hasTranslation ? record->translation.c_str() : fallback;
#ifdef TH_DEV_TOOLS
    if (record != nullptr && record->hasTranslation && id != nullptr)
    {
        static std::unordered_map<std::string, bool> logged;
        if (logged.emplace(id, true).second)
            SDL_Log("TH06 thcrap strings_lookup: id=%s text=%s", id, result != nullptr ? result : "(null)");
    }
#endif
    return result;
}

const char *Localization::FormatStringById(const char *id, const char *fallback)
{
    const StringRecord *record = g_Strings.Lookup(id);
    if (record == nullptr || !record->hasTranslation || fallback == nullptr)
        return fallback;

    std::string fallbackSignature;
    std::string translatedSignature;
    if (!ParsePrintfSignature(fallback, fallbackSignature) ||
        !ParsePrintfSignature(record->translation.c_str(), translatedSignature) ||
        fallbackSignature != translatedSignature)
        return fallback;

#ifdef TH_DEV_TOOLS
    static std::unordered_map<std::string, bool> logged;
    if (id != nullptr && logged.emplace(id, true).second)
        SDL_Log("TH06 thcrap strings_format: id=%s signature=%s text=%s", id,
                translatedSignature.c_str(), record->translation.c_str());
#endif
    return record->translation.c_str();
}

const char *Localization::LogString(const char *fallback)
{
    if (fallback == nullptr)
        return nullptr;

    struct LogStringEntry
    {
        const char *id;
        const char *fallback;
    };

    // v1.02h stringlocs.v1.02h.js contains exactly these 35 Log/Fatal
    // literals. strings_lookup#cavesize_6 patches the fmt argument at both
    // GameErrorContext entry points, so preserve that original-address lookup
    // semantics in typed form and reject translations with a changed printf
    // signature before they can reach C varargs.
    static const LogStringEntry entries[] = {
        {"th06_error_two_instances", "二つは起動できません\n"},
        {"th06_log_header", "東方動作記録 --------------------------------------------- \n"},
        {"th06_log_tl_hal", "T&L HAL で動作しま～す\n"},
        {"th06_log_directsound_init", "DirectSound は正常に初期化されました\n"},
        {"th06_log_no_gamepad", "使えるパッドが存在しないようです、残念\n"},
        {"th06_log_valid_pad", "有効なパッドを発見しました\n"},
        {"th06_log_unable_to_read_file", "%sが読み込めないです。\n"},
        {"th06_log_directinput_init", "DirectInput は正常に初期化されました\n"},
        {"th06_log_sound_file_read_error", "error : Sound ファイルが読み込めない データを確認 %s\n"},
        {"th06_log_sprite_read_error", "スプライトアニメ %s が読み込めません。データが失われてるか壊れています\n"},
        {"th06_log_texture_read_error", "テクスチャ %s が読み込めません。データが失われてるか壊れています\n"},
        {"th06_log_export_failure", "ファイルが書き出せません %s\n"},
        {"th06_log_read_only_full_disk", "フォルダが書込み禁止属性になっているか、ディスクがいっぱいいっぱいになってませんか？\n"},
        {"th06_log_reinit_corrupt_config", "コンフィグデータが破壊されていたので再初期化しました\n"},
        {"th06_log_reinit_missing_config", "コンフィグデータが見つからないので初期化しました\n"},
        {"th06_log_first_startup_16bit", "初回起動、画面を 16Bits で初期化しました\n"},
        {"th06_log_first_startup_32bit", "初回起動、画面を 32Bits で初期化しました\n"},
        {"th06_log_character_init_failure", "error : 文字の初期化に失敗しました\n"},
        {"th06_log_tl_hal_unavailable", "T&L HAL は使用できないようです\n"},
        {"th06_log_hal_unavailable", "HAL も使用できないようです\n"},
        {"th06_log_backbuffer_nonlocking_suggestion", "バックバッファをロック不可能にしてみます\n"},
        {"th06_log_direct3d_init_failure", "Direct3D の初期化に失敗、これではゲームは出来ません\n"},
        {"th06_log_refresh_rate", "リフレッシュレートを60Hzに変更します\n"},
        {"th06_log_vertex", "頂点バッファの使用を抑制します\n"},
        {"th06_log_fog", "フォグの使用を抑制します\n"},
        {"th06_log_16bit_textures", "16Bit のテクスチャの使用を強制します\n"},
        {"th06_log_Gouraud_shading", "グーローシェーディングを抑制します\n"},
        {"th06_log_color_composition", "テクスチャの色合成を抑制しますn"},
        {"th06_log_rasterizer_mode", "リファレンスラスタライザを強制します\n"},
        {"th06_log_clear_buffer", "バックバッファの消去を強制します\n"},
        {"th06_log_minimum_graphics", "ゲーム周りのアイテムの描画を抑制します\n"},
        {"th06_log_depth_test", "デプステストを抑制します\n"},
        {"th06_log_force_frame", "６０フレーム強制モードにします\n"},
        {"th06_log_directinput_usage", "パッド、キーボードの入力に DirectInput を使用しません\n"},
        {"th06_log_window_mode", "ウィンドウモードで起動します\n"},
    };

    for (const LogStringEntry &entry : entries)
    {
        if (std::strcmp(fallback, entry.fallback) == 0)
            return FormatStringById(entry.id, fallback);
    }
    return fallback;
}

#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
bool Localization::DebugAsciiTableSelfTest()
{
    AsciiEntryView fullPower{};
    if (!LookupAscii("Full Power Mode!!", fullPower) ||
        std::strcmp(fullPower.id, "th06_ascii_fullpower") != 0 ||
        fullPower.hasAlignment ||
        std::strcmp(fullPower.text, "Full Power Mode!!") != 0)
        return false;

    AsciiEntryView practice{};
    if (!LookupAscii("STAGE %d  %.9d", practice) ||
        std::strcmp(practice.id, "th06_practice_format") != 0)
        return false;
    if (std::strcmp(AsciiStringById("th06_ascii_result_clear", "(C)"), "(C)") != 0 ||
        std::strcmp(AsciiStringById("th06_ascii_centered_stage_format", "STAGE %d"), "STAGE %d") != 0)
        return false;
    const char *unknown = "TH06 ASCII audit unknown";
    if (LookupAscii(unknown, practice) || AsciiString(unknown) != unknown)
        return false;
    return true;
}

bool Localization::DebugStringTableSelfTest()
{
    static const char *const requiredIds[] = {
        "th06 BGM In-game format",
        "th06 Bomb Reimu A",
        "th06 Bomb Reimu B",
        "th06 Bomb Marisa A",
        "th06 Bomb Marisa B",
        "th06 Stats ReimuA",
        "th06 Stats ReimuB",
        "th06 Stats MarisaA",
        "th06 Stats MarisaB",
        "th06_error_two_instances",
        "th06_log_header",
        "th06_log_tl_hal",
        "th06_log_directsound_init",
        "th06_log_no_gamepad",
        "th06_log_valid_pad",
        "th06_log_unable_to_read_file",
        "th06_log_directinput_init",
        "th06_log_sound_file_read_error",
        "th06_log_sprite_read_error",
        "th06_log_texture_read_error",
        "th06_log_export_failure",
        "th06_log_read_only_full_disk",
        "th06_log_reinit_corrupt_config",
        "th06_log_reinit_missing_config",
        "th06_log_first_startup_16bit",
        "th06_log_first_startup_32bit",
        "th06_log_character_init_failure",
        "th06_log_tl_hal_unavailable",
        "th06_log_hal_unavailable",
        "th06_log_backbuffer_nonlocking_suggestion",
        "th06_log_direct3d_init_failure",
        "th06_log_refresh_rate",
        "th06_log_vertex",
        "th06_log_fog",
        "th06_log_16bit_textures",
        "th06_log_Gouraud_shading",
        "th06_log_color_composition",
        "th06_log_rasterizer_mode",
        "th06_log_clear_buffer",
        "th06_log_minimum_graphics",
        "th06_log_depth_test",
        "th06_log_force_frame",
        "th06_log_directinput_usage",
        "th06_log_window_mode",
    };
    for (const char *id : requiredIds)
    {
        const StringRecord *record = g_Strings.Lookup(id);
        // EST1 is a contract table, so every ID must exist even when the
        // selected language intentionally has no translation for it.  For
        // example, the current official Russian TH06 stack translates the
        // visible BGM/Bomb/Stats entries but leaves the diagnostic strings to
        // the original Japanese fallback.
        if (record == nullptr)
            return false;
    }
    const char *fallback = "TH06 EST1 audit unknown";
    if (StringById("th06 EST1 unknown id", fallback) != fallback)
        return false;
    static const char twoInstancesFallback[] = "二つは起動できません\n";
    const StringRecord *twoInstancesRecord = g_Strings.Lookup("th06_error_two_instances");
    const char *twoInstances = LogString(twoInstancesFallback);
    if (twoInstancesRecord == nullptr ||
        (twoInstancesRecord->hasTranslation
             ? (twoInstances == twoInstancesFallback ||
                std::strcmp(twoInstances, twoInstancesRecord->translation.c_str()) != 0)
             : twoInstances != twoInstancesFallback))
        return false;
    static const char unableToReadFallback[] = "%sが読み込めないです。\n";
    const StringRecord *unableToReadRecord = g_Strings.Lookup("th06_log_unable_to_read_file");
    const char *unableToRead = LogString(unableToReadFallback);
    if (unableToReadRecord == nullptr || std::strstr(unableToRead, "%s") == nullptr ||
        (unableToReadRecord->hasTranslation
             ? (unableToRead == unableToReadFallback ||
                std::strcmp(unableToRead, unableToReadRecord->translation.c_str()) != 0)
             : unableToRead != unableToReadFallback))
        return false;
    static const char wrongSignatureFallback[] = "%d";
    if (FormatStringById("th06_log_unable_to_read_file", wrongSignatureFallback) != wrongSignatureFallback)
        return false;
    static const char unknownLogFallback[] = "TH06 log audit unknown";
    if (LogString(unknownLogFallback) != unknownLogFallback)
        return false;
    return true;
}
#endif

bool Localization::ApplyStageTitleImage(AnmVm *vm, std::uint32_t stage)
{
    if (!Active() || stage < 1 || stage > 7)
    {
        g_StageLogoImageActive = false;
        return false;
    }

    // In base_tsa slot 0x700 is a priority chain: ti_sttitle.png is the
    // 384x16 fallback and ti_stlogo.png is the later/higher-priority 384x48
    // replacement. The Russian pack currently ships the latter.
    if (LoadTextImage(47, "ti_stlogo.png", g_StageLogoImageAttempted, g_StageLogoImageReady) &&
        ApplyTextImage(vm, 0x7fe, 47, static_cast<i32>(stage - 1), 384, 48))
    {
        g_StageLogoImageActive = true;
#ifdef TH_DEV_TOOLS
        SDL_Log("TH06 thcrap stage textimage: stage=%u source=ti_stlogo.png row=%u size=384x48", stage,
                stage - 1);
#endif
        return true;
    }

    g_StageLogoImageActive = false;
    if (!LoadTextImage(47, "ti_sttitle.png", g_StageImageAttempted, g_StageImageReady))
        return false;
    // Keep the original script's sprite table untouched: its source texture is
    // shared by other gameplay VMs. Use private slots for the translated slice.
    const bool applied = ApplyTextImage(vm, 0x7fe, 47, static_cast<i32>(stage - 1), 384, 16);
#ifdef TH_DEV_TOOLS
    if (applied)
        SDL_Log("TH06 thcrap stage textimage: stage=%u source=ti_sttitle.png row=%u size=384x16", stage,
                stage - 1);
#endif
    return applied;
}

bool Localization::StageLogoImageActive()
{
    return Active() && g_StageLogoImageActive;
}

bool Localization::ApplyMusicTitleImage(AnmVm *vm, std::uint32_t stage, std::uint32_t cue)
{
    if (!Active() || stage < 1 || stage > 7 || cue > 1 ||
        !LoadTextImage(48, "ti_bgm.png", g_MusicImageAttempted, g_MusicImageReady))
        return false;
    const i32 row = static_cast<i32>((stage - 1) * 2 + cue);
    if (!ApplyTextImage(vm, 0x7ff, 48, row, 384, 32))
        return false;
    g_AnmManager->SetAndExecuteScript(vm, reinterpret_cast<const AnmRawInstr *>(kMusicTitleScript));
    // The script's initial SetActiveSprite deliberately rebuilds the original
    // logicalSprite/backingTexture matrix.  Draw2's base_tsa-equivalent
    // sprite3d correction owns the later backingTexture/256 conversion; do
    // not pre-apply that conversion here.
    vm->UpdatePrev();
    return true;
}

namespace
{
bool ApplyBossImage(AnmVm *vm, std::uint32_t stage, i32 textureSlot, const char *path,
                    bool &attempted, bool &ready, u32 spriteSlot)
{
    if (!Localization::Active() || stage < 1 || stage > 7 ||
        !LoadTextImage(textureSlot, path, attempted, ready) ||
        !ApplyTextImage(vm, spriteSlot, textureSlot, static_cast<i32>(stage - 1), 384, 64))
        return false;

    // thcrap replaces both original 336x16 boss-title scripts with a shared
    // 384x64 canvas at (224,352).  The authored PNGs contain their own line
    // placement, colors and outlines, so retaining the original per-line
    // positions shifts the two layers apart.
    vm->pos = ZunVec3(224.0f, 352.0f, 0.0f);
    vm->prevPos = vm->pos;
    return true;
}
} // namespace

bool Localization::ApplyBossTitleImage(AnmVm *vm, std::uint32_t stage)
{
    return ApplyBossImage(vm, stage, 49, "ti_bosstitle.png", g_BossTitleImageAttempted,
                          g_BossTitleImageReady, 0x7fc);
}

bool Localization::ApplyBossNameImage(AnmVm *vm, std::uint32_t stage)
{
    return ApplyBossImage(vm, stage, 50, "ti_bossname.png", g_BossNameImageAttempted,
                          g_BossNameImageReady, 0x7fd);
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
