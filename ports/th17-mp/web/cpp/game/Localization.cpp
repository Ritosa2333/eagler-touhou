#include "Localization.hpp"
#include <cstring>

namespace th17 {
namespace {
u16 read16(const u8* p) { return u16(p[0]) | u16(p[1]) << 8; }
u32 read32(const u8* p) { return u32(read16(p)) | u32(read16(p + 2)) << 16; }
u64 key(u32 id, u16 row) { return (u64(id) << 16) | row; }
}

bool valid_utf8(std::string_view text) {
    for (size_t i = 0; i < text.size();) {
        const u8 lead = u8(text[i++]);
        if (lead < 0x80) continue;
        const size_t tail = lead >= 0xc2 && lead <= 0xdf ? 1 :
                            lead >= 0xe0 && lead <= 0xef ? 2 :
                            lead >= 0xf0 && lead <= 0xf4 ? 3 : 0;
        if (!tail || tail > text.size() - i) return false;
        const u8 next = u8(text[i]);
        if ((lead == 0xe0 && next < 0xa0) || (lead == 0xed && next >= 0xa0) ||
            (lead == 0xf0 && next < 0x90) || (lead == 0xf4 && next >= 0x90)) return false;
        for (size_t n = 0; n < tail; ++n) if ((u8(text[i++]) & 0xc0) != 0x80) return false;
    }
    return true;
}

bool LocalizationTable::open(const u8* bytes, size_t size) {
    entries.clear();
    if (!bytes || size < 8 || size > 2 * 1024 * 1024 || std::memcmp(bytes, "ETL1", 4)) return false;
    const u32 count = read32(bytes + 4);
    if (count > (size - 8) / 8) return false;
    decltype(entries) parsed;
    size_t offset = 8;
    for (u32 n = 0; n < count; ++n) {
        if (size - offset < 8) return false;
        const auto id = read32(bytes + offset);
        const auto row = read16(bytes + offset + 4);
        const auto length = read16(bytes + offset + 6);
        offset += 8;
        if (size - offset < length || std::memchr(bytes + offset, 0, length)) return false;
        const auto* text = reinterpret_cast<const char*>(bytes + offset);
        if (!valid_utf8({text, length}) || !parsed.emplace(key(id, row), std::string(text, length)).second) return false;
        offset += length;
    }
    if (offset != size) return false;
    entries.swap(parsed);
    return true;
}

const std::string* LocalizationTable::find(u32 id, u16 row) const {
    const auto found = entries.find(key(id, row));
    return found == entries.end() ? nullptr : &found->second;
}

const std::string* LocalizationTable::spell_name(i32 id) const {
    const i32 rank = SpellCatalog::rank(id);
    if (rank < 0) return nullptr;
    // A missing difficulty inherits from an earlier difficulty in the same group.
    const i32 first = rank == 4 ? id : id - rank;
    for (; id >= first; --id) {
        if (const auto* name = find(u32(id)); name && !name->empty()) return name;
    }
    return nullptr;
}
}
