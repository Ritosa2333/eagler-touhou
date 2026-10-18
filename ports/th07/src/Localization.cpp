#include "Localization.hpp"

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>

#include "AnmManager.hpp"
#include "FileSystem.hpp"
#ifdef TH_DEV_TOOLS
#include "Supervisor.hpp"
#endif

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
            {
                nextIds.emplace(record.id, AsciiIdValue{record.translation, record.hasTranslation});
            }
            else if (idFound->second.hasTranslation != record.hasTranslation ||
                     idFound->second.translation != record.translation)
            {
                valid = false;
                break;
            }

            // Empty aliases are EAS1 lookup-only records used by TH06 helper
            // IDs. They intentionally populate only the ID map.
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
        // None of the TH07 Stats/Result contracts use length modifiers.
        // Reject them instead of guessing a va_list type.
        if (*cursor == 'h' || *cursor == 'l' || *cursor == 'j' || *cursor == 'z' ||
            *cursor == 't' || *cursor == 'L')
            return false;

        switch (*cursor)
        {
        case 'd':
        case 'i':
            signature.push_back('i');
            break;
        case 'u':
        case 'o':
        case 'x':
        case 'X':
            signature.push_back('u');
            break;
        case 'c':
            signature.push_back('c');
            break;
        case 's':
            signature.push_back('s');
            break;
        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
        case 'a':
        case 'A':
            signature.push_back('f');
            break;
        default:
            return false;
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
        while (cursor < json.size() &&
               std::isspace(static_cast<unsigned char>(json[cursor])))
            cursor++;
        if (cursor >= json.size() || json[cursor++] != '\"')
            return false;
        const std::size_t end = json.find('\"', cursor);
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
        if (fontFile.find("..") != std::string::npos ||
            fontFile.find('/') != std::string::npos ||
            fontFile.find('\\') != std::string::npos ||
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
bool g_BossTitleImageAttempted = false;
bool g_BossTitleImageReady = false;
bool g_BossNameImageAttempted = false;
bool g_BossNameImageReady = false;

// Runtime-only localization textures must not alias ANM file/texture slots.
// TH07MP uses 50/51 for the P2/P3 player ANMs, exactly where the old boss
// title/name images lived. Keep them immediately below the top six CPU-backed
// slots reserved for multiplayer ANMs.
constexpr i32 TEXTURE_SLOT_LOCALIZED_BOSS_TITLE = 248;
constexpr i32 TEXTURE_SLOT_LOCALIZED_BOSS_NAME = 249;

bool LoadTextImage(i32 textureSlot, const char *path, bool &attempted, bool &ready)
{
    if (!attempted)
    {
        attempted = true;
        // thcrap's textimage path preserves the complete PNG alpha channel.
        // Passing zero here also keeps transparent black outline pixels.
        ready = g_AnmManager != nullptr &&
                g_AnmManager->LoadTexture(textureSlot, path, 0) == ZUN_SUCCESS;
#ifdef TH_DEV_TOOLS
        Supervisor::DebugPrint("th07 thcrap textimage load: slot=%d path=%s ready=%d size=%ux%u\n",
                               textureSlot, path, ready ? 1 : 0,
                               g_AnmManager != nullptr ? g_AnmManager->textureWidths[textureSlot] : 0,
                               g_AnmManager != nullptr ? g_AnmManager->textureHeights[textureSlot] : 0);
#endif
    }
    return ready;
}

bool LoadBossImages()
{
    // thcrap declares the title and name images as one group.  Keep the same
    // all-or-nothing activation rule so a missing half never leaves the
    // intro with one translated image and one rendered Japanese line.
    return LoadTextImage(TEXTURE_SLOT_LOCALIZED_BOSS_TITLE, "ti_bosstitle.png",
                         g_BossTitleImageAttempted,
                         g_BossTitleImageReady) &&
           LoadTextImage(TEXTURE_SLOT_LOCALIZED_BOSS_NAME, "ti_bossname.png",
                         g_BossNameImageAttempted,
                         g_BossNameImageReady);
}

bool ResolveBossImageRow(std::uint32_t stage, i32 rightPortraitSprite, i32 &row)
{
    if (stage < 1 || stage > 8)
        return false;
    if (stage == 4)
    {
        const i32 faceDelta = rightPortraitSprite - 0x4ad;
        if (faceDelta < 0 || faceDelta % 3 != 0)
            return false;
        const i32 face = faceDelta / 3;
        if (face < 0 || face > 2)
            return false;
        row = 3 + face;
        return true;
    }
    row = stage <= 3 ? static_cast<i32>(stage - 1)
                     : static_cast<i32>(stage + 1);
    return true;
}

bool ApplyTextImage(AnmVm *vm, u32 spriteSlot, i32 textureSlot, i32 row, i32 width, i32 height)
{
    if (vm == nullptr || g_AnmManager == nullptr || row < 0)
        return false;

    const i32 top = row * height;
    if (g_AnmManager->textureWidths[textureSlot] < static_cast<u32>(width) ||
        g_AnmManager->textureHeights[textureSlot] < static_cast<u32>(top + height))
        return false;

    AnmLoadedSprite sprite{};
    sprite.sourceFileIndex = textureSlot;
    sprite.startPixelInclusive = {0.0f, static_cast<f32>(top)};
    sprite.endPixelInclusive =
        {static_cast<f32>(width), static_cast<f32>(top + height)};
    sprite.textureWidth = static_cast<f32>(g_AnmManager->textureWidths[textureSlot]);
    sprite.textureHeight = static_cast<f32>(g_AnmManager->textureHeights[textureSlot]);
    sprite.cols = 1.0f;
    sprite.rows = 1.0f;

    g_AnmManager->LoadSprite(spriteSlot, &sprite);
    if (g_AnmManager->SetActiveSprite(vm, spriteSlot) != ZUN_SUCCESS)
        return false;

    vm->visible = 1;
    return true;
}

bool ApplyBossImage(AnmVm *vm, std::uint32_t stage, i32 rightPortraitSprite,
                    i32 textureSlot, const char *path, bool &attempted, bool &ready,
                    u32 spriteSlot)
{
    if (!Localization::Active() || stage < 1 || stage > 8 || !LoadBossImages())
        return false;

    // This is the row expression from thcrap's th07.v1.00b textimage_set
    // hook.  In the original 32-bit GuiImpl, 0x1fe0c is
    // msg.portraits[1], and AnmVm+0x1d4 is activeSpriteIdx.  MSG_CHANGE_FACE
    // uses 0x4ad (1197) as the right-portrait sprite base, with the three
    // Prismriver faces spaced by 3 sprites.
    i32 row;
    if (!ResolveBossImageRow(stage, rightPortraitSprite, row))
        return false;
#ifdef TH_DEV_TOOLS
    Supervisor::DebugPrint("th07 thcrap textimage apply: stage=%u texture=%d row=%d sprite=%u before=%d\n",
                           stage, textureSlot, row, spriteSlot,
                           vm != nullptr ? static_cast<int>(vm->activeSpriteIdx) : -1);
#endif
    return ApplyTextImage(vm, spriteSlot, textureSlot, row, 384, 64);
}
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
    return g_Options.available && !g_Options.fontName.empty() ? g_Options.fontName.c_str()
                                                               : nullptr;
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
    const char *result = record != nullptr && record->hasTranslation ? record->translation.c_str()
                                                                        : fallback;
#ifdef TH_DEV_TOOLS
    if (record != nullptr && record->hasTranslation && id != nullptr)
    {
        static std::unordered_map<std::string, bool> logged;
        if (logged.emplace(id, true).second)
            SDL_Log("th07 thcrap strings_lookup: id=%s text=%s", id,
                    result != nullptr ? result : "(null)");
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
        SDL_Log("th07 thcrap strings_format: id=%s signature=%s text=%s", id,
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

    // base_tsa installs strings_lookup directly on the fmt argument of both
    // GameErrorContext::Log and ::Fatal. Keep the same fallback-address
    // semantics in typed form: only literals that have a TH07 stringloc ID are
    // eligible, and FormatStringById rejects translations that change the
    // printf signature before the format ever reaches C varargs.
    static const LogStringEntry entries[] = {
        {"th06_log_unable_to_read_file", "%sが読み込めないです。\r\n"},
        {"th06_log_texture_read_error", "テクスチャ %s が読み込めません。データが失われてるか壊れています\r\n"},
        {"th06_log_sound_file_read_error", "error : Sound ファイルが読み込めない データを確認 %s\r\n"},
        {"th06_log_directsound_init", "DirectSound は正常に初期化されました\r\n"},
        {"th06_log_read_only_full_disk", "フォルダが書込み禁止属性になっているか、ディスクがいっぱいいっぱいになってませんか？\r\n"},
        {"th06_log_export_failure", "ファイルが書き出せません %s\r\n"},
        {"th07_log_Vsync", "垂直同期を取りません\r\n"},
        {"th07_log_BGM_memory", "ＢＧＭをメモリに読み込みます\r\n"},
        {"th07_log_draw_screen", "画面周りを毎回描画します\r\n"},
        {"th06_log_directinput_usage", "パッド、キーボードの入力に DirectInput を使用しません\r\n"},
        {"th06_log_rasterizer_mode", "リファレンスラスタライザを強制します\r\n"},
        {"th06_log_window_mode", "ウィンドウモードで起動します\r\n"},
        {"th06_log_color_composition", "テクスチャの色合成を抑制しますn"},
        {"th06_log_depth_test", "デプステストを抑制します\r\n"},
        {"th06_log_Gouraud_shading", "グーローシェーディングを抑制します\r\n"},
        {"th06_log_minimum_graphics", "ゲーム周りのアイテムの描画を抑制します\r\n"},
        {"th06_log_16bit_textures", "16Bit のテクスチャの使用を強制します\r\n"},
        {"th06_log_fog", "フォグの使用を抑制します\r\n"},
        {"th06_log_vertex", "頂点バッファの使用を抑制します\r\n"},
        {"th07_log_reinit_abnormal_config", "コンフィグデータが異常でしたので再初期化しました\r\n"},
        {"th06_log_reinit_missing_config", "コンフィグデータが見つからないので初期化しました\r\n"},
        {"th06_log_character_init_failure", "error : 文字の初期化に失敗しました\r\n"},
        {"th06_log_valid_pad", "有効なパッドを発見しました\r\n"},
        {"th06_log_directinput_init", "DirectInput は正常に初期化されました\r\n"},
        {"th06_error_two_instances", "二つは起動できません\r\n"},
        {"th06_log_tl_hal", "T&L HAL で動作しま～す\r\n"},
        {"th06_log_direct3d_init_failure", "Direct3D の初期化に失敗、これではゲームは出来ません\r\n"},
        {"th07_log_refresh_rate_suggestion", "*** リフレッシュレートを60Hzに変更することを推奨します ***\r\n"},
        {"th07_log_async_update_failure", "非同期更新も行えません。一番汚いモードに変更します\r\n"},
        {"th06_log_hal_unavailable", "HAL も使用できないようです\r\n"},
        {"th06_log_tl_hal_unavailable", "T&L HAL は使用できないようです\r\n"},
        {"th07_log_async_vsync_test", "VSync非同期可能かどうかを試みます\r\n"},
        {"th06_log_first_startup_32bit", "初回起動、画面を 32Bits で初期化しました\r\n"},
        {"th06_log_no_gamepad", "使えるパッドが存在しないようです、残念\r\n"},
        {"th06_log_header", "東方動作記録 --------------------------------------------- \r\n"},
    };

    for (const LogStringEntry &entry : entries)
    {
        if (std::strcmp(fallback, entry.fallback) == 0)
            return FormatStringById(entry.id, fallback);
    }
    return fallback;
}

bool Localization::ApplyBossTitleImage(AnmVm *vm, std::uint32_t stage,
                                       std::int32_t rightPortraitSprite)
{
    return ApplyBossImage(vm, stage, rightPortraitSprite, TEXTURE_SLOT_LOCALIZED_BOSS_TITLE,
                          "ti_bosstitle.png",
                          g_BossTitleImageAttempted, g_BossTitleImageReady, 0x702);
}

bool Localization::ApplyBossNameImage(AnmVm *vm, std::uint32_t stage,
                                      std::int32_t rightPortraitSprite)
{
    return ApplyBossImage(vm, stage, rightPortraitSprite, TEXTURE_SLOT_LOCALIZED_BOSS_NAME,
                          "ti_bossname.png",
                          g_BossNameImageAttempted, g_BossNameImageReady, 0x703);
}

#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
bool Localization::DebugAsciiTableSelfTest()
{
    AsciiEntryView fullPower{};
    if (!LookupAscii("Full Power Mode!", fullPower) ||
        std::strcmp(fullPower.id, "th07 Full Power") != 0 ||
        !fullPower.hasAlignment ||
        std::strcmp(fullPower.baseline, "Full Power Mode!") != 0 ||
        fullPower.extraX != 8.5f ||
        std::strcmp(fullPower.text, "Full Power Mode!") != 0)
        return false;

    AsciiEntryView stage1{};
    if (!LookupAscii("Stage1  ", stage1) ||
        std::strcmp(stage1.id, "th06_ascii_replay_stage_1") != 0 ||
        stage1.hasAlignment ||
        std::strcmp(stage1.text, "Stage1  ") != 0)
        return false;

    const char *unknown = "ASCII audit unknown";
    if (LookupAscii(unknown, stage1) || AsciiString(unknown) != unknown)
        return false;
    if (std::strcmp(AsciiStringById("th07 Full Power", "fallback"), "fallback") != 0)
        return false;
    return true;
}

bool Localization::DebugStringTableSelfTest()
{
    static const char *const requiredIds[] = {
        "th07 Key Shot", "th07 Key Bomb", "th07 Key Slow", "th07 Key Skip",
        "th07 Key Pause", "th07 Key Up", "th07 Key Down", "th07 Key Left",
        "th07 Key Right", "th07 Key ShotSlow", "th07 Key Reset", "th07 Key Quit",
        "th07 Option Player", "th07 Option Graphic", "th07 Option BGM", "th07 Option Sound",
        "th07 Option Window Mode", "th07 Option Slow Mode", "th07 Option Reset",
        "th07 Option Key Config", "th07 Option Quit",
        "th07 Menu Start", "th07 Menu Extra Start", "th07 Menu Practice Start",
        "th07 Menu Replay", "th07 Menu Result", "th07 Menu Music Room", "th07 Menu Option",
        "th07 Menu Quit",
        "th07 Bomb Reimu A unfocused", "th07 Bomb Reimu A focused", "th06 Bomb Reimu B",
        "th07 Bomb Reimu B focused", "th06 Bomb Marisa A", "th07 Bomb Marisa A focused",
        "th07 Bomb Marisa B unfocused", "th06 Bomb Marisa B",
        "th07 Bomb Sakuya A unfocused", "th07 Bomb Sakuya A focused",
        "th07 Bomb Sakuya B unfocused", "th07 Bomb Sakuya B focused",
        "th07 Stats Retries", "th07 Stats Retries +Phantasm",
        "th07 Stats Practice", "th07 Stats Practice +Phantasm",
        "th07 Stats Continue", "th07 Stats Continue +Phantasm",
        "th07 Stats Clear Count", "th07 Stats Clear Count +Phantasm",
        "th07 Stats Character Format", "th07 Stats Character Format +Phantasm",
        "th07 Stats Play Count", "th07 Stats Play Count +Phantasm",
        "th07 Stats Total Playtime", "th07 Stats Time Since Startup",
        "th07 Spell Result Character Select", "th07 Stats Total (All Characters)",
        "th07 Stats SakuyaB", "th07 Stats SakuyaA", "th06 Stats MarisaB",
        "th06 Stats MarisaA", "th06 Stats ReimuB", "th06 Stats ReimuA",
        "th06_log_unable_to_read_file", "th06_log_texture_read_error",
        "th06_log_sound_file_read_error", "th06_log_directsound_init",
        "th06_log_read_only_full_disk", "th06_log_export_failure", "th07_log_Vsync",
        "th07_log_BGM_memory", "th07_log_draw_screen", "th06_log_directinput_usage",
        "th06_log_rasterizer_mode", "th06_log_window_mode", "th06_log_color_composition",
        "th06_log_depth_test", "th06_log_Gouraud_shading", "th06_log_minimum_graphics",
        "th06_log_16bit_textures", "th06_log_fog", "th06_log_vertex",
        "th07_log_reinit_abnormal_config", "th06_log_reinit_missing_config",
        "th06_log_character_init_failure", "th06_log_valid_pad", "th06_log_directinput_init",
        "th06_error_two_instances", "th06_log_tl_hal", "th06_log_direct3d_init_failure",
        "th07_log_refresh_rate_suggestion", "th07_log_async_update_failure",
        "th06_log_hal_unavailable", "th06_log_tl_hal_unavailable", "th07_log_async_vsync_test",
        "th06_log_first_startup_32bit", "th06_log_no_gamepad", "th06_log_header",
    };
    for (const char *id : requiredIds)
    {
        const StringRecord *record = g_Strings.Lookup(id);
        if (record == nullptr || !record->hasTranslation || record->translation.empty())
            return false;
    }
    const char *fallback = "TH07 EST1 audit unknown";
    if (StringById("th07 EST1 unknown id", fallback) != fallback)
        return false;
    static const char retriesFallback[] = "リトライ回数  　 %6d %6d %6d %6d %6d %6d";
    const char *retries = FormatStringById("th07 Stats Retries", retriesFallback);
    if (retries == retriesFallback || std::strstr(retries, "<l$Retries>") == nullptr)
        return false;
    if (FormatStringById("th07 Stats Retries", "%s") != std::string("%s"))
        return false;
    static const char twoInstancesFallback[] = "二つは起動できません\r\n";
    if (LogString(twoInstancesFallback) == twoInstancesFallback)
        return false;
    static const char unableToReadFallback[] = "%sが読み込めないです。\r\n";
    const char *unableToRead = LogString(unableToReadFallback);
    if (unableToRead == unableToReadFallback || std::strstr(unableToRead, "%s") == nullptr)
        return false;
    static const char unknownLogFallback[] = "TH07 log audit unknown";
    if (LogString(unknownLogFallback) != unknownLogFallback)
        return false;
    return true;
}

bool Localization::DebugBossImageRowContractSelfTest()
{
    struct Case
    {
        std::uint32_t stage;
        i32 sprite;
        i32 expectedRow;
    };
    static constexpr Case cases[] = {
        {1, -1, 0}, {2, -1, 1}, {3, -1, 2},
        {4, 1197, 3}, {4, 1200, 4}, {4, 1203, 5},
        {5, -1, 6}, {6, -1, 7}, {7, -1, 8}, {8, -1, 9},
    };
    for (const Case &test : cases)
    {
        i32 row = -1;
        if (!ResolveBossImageRow(test.stage, test.sprite, row) || row != test.expectedRow)
            return false;
    }
    for (const i32 invalidSprite : {1196, 1198, 1201, 1204, 1210})
    {
        i32 row = -1;
        if (ResolveBossImageRow(4, invalidSprite, row))
            return false;
    }
    return true;
}
#endif

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
