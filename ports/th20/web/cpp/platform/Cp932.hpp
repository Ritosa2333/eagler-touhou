// CP932 <-> UTF-16 conversion for the TH_SDL3 build, replacing the
// MultiByteToWideChar(932)/WideCharToMultiByte(932) calls. Forward mapping
// uses the generated table; the reverse map is built lazily from it (first
// mapping wins for duplicate code points, matching the codec's canonical
// direction).
#pragma once

#include "cp932_table.inc"
#include <cstdint>
#include <cstring>

namespace th20::web::cp932 {

inline std::size_t to_utf16(const char* text, wchar_t* out, std::size_t capacity) {
    std::size_t count = 0;
    const char* p = text;
    while (*p && count + 1 < capacity) {
        out[count++] = static_cast<wchar_t>(map(p));
    }
    if (capacity) out[count] = 0;
    return count;
}

inline std::uint16_t reverse_lookup(std::uint16_t code) {
    static std::uint16_t reverse[65536];
    static bool built = false;
    if (!built) {
        built = true;
        for (int i = 0; i < 256; ++i)
            if (single[i] && !reverse[single[i]]) reverse[single[i]] = static_cast<std::uint16_t>(i);
        for (int lead = 0; lead < lead_count; ++lead)
            for (int trail = 0; trail < 256; ++trail) {
                const auto value = pairs[lead][trail];
                if (value && !reverse[value]) reverse[value] = static_cast<std::uint16_t>((leads[lead] << 8) | trail);
            }
    }
    return reverse[code];
}

inline std::size_t from_utf16(const wchar_t* text, char* out, std::size_t capacity) {
    std::size_t count = 0;
    for (const wchar_t* p = text; *p && count + 1 < capacity; ++p) {
        const auto value = static_cast<std::uint16_t>(*p);
        const auto mapped = reverse_lookup(value);
        if (mapped >> 8) {
            if (count + 2 >= capacity) break;
            out[count++] = static_cast<char>(mapped >> 8);
            out[count++] = static_cast<char>(mapped & 0xff);
        } else {
            out[count++] = static_cast<char>(mapped ? mapped : value < 128 ? value : '?');
        }
    }
    if (capacity) out[count] = 0;
    return count;
}
}
