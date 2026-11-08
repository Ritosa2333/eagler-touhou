#include "MusicLayout.hpp"
namespace th13 {
namespace {
u32 word(const u8* p) { u32 v; std::memcpy(&v, p, 4); return v; }
u16 half(const u8* p) { u16 v; std::memcpy(&v, p, 2); return v; }
}
bool MusicLayout::open(const u8* bytes, u32 size) {
    error.clear();
    std::vector<MusicTrack> result;
    u32 at = 0;
    while (at + 52 <= size && bytes[at]) {
        const auto* p = bytes + at;
        u32 n = 0;
        while (n < 16 && p[n]) ++n;
        if (n == 16) { error = "Unterminated TH13 music filename"; return false; }
        MusicTrack t;
        t.filename.assign(reinterpret_cast<const char*>(p), n);
        t.offset = word(p + 16); t.length = word(p + 20);
        t.loop_start = word(p + 24); t.loop_end = word(p + 28);
        t.channels = half(p + 34); t.rate = word(p + 36);
        t.alignment = half(p + 44); t.bits = half(p + 46);
        if (half(p + 32) != 1 || t.channels != 2 ||
            (t.rate != 44100 && t.rate != 22050) || t.bits != 16 || t.alignment != 4 ||
            word(p + 40) != t.rate * t.alignment || t.loop_start >= t.loop_end ||
            t.loop_end > t.length || t.offset % 4 || t.length % 4 ||
            t.loop_start % 4 || t.loop_end % 4) {
            error = "Invalid TH13 PCM music layout"; return false;
        }
        result.push_back(std::move(t)); at += 52;
    }
    for (; at < size; ++at) if (bytes[at]) { error = "Truncated TH13 music layout"; return false; }
    if (result.empty()) { error = "Empty TH13 music layout"; return false; }
    tracks = std::move(result); return true;
}
i32 MusicLayout::index(const std::string& value) const {
    auto slash = value.find_last_of('/');
    if (slash == std::string::npos) slash = value.find_last_of('\\');
    auto name = slash == std::string::npos ? value : value.substr(slash + 1);
    if (name.size() < 4 || name.substr(name.size() - 4) != ".wav") name += ".wav";
    for (u32 n = 0; n < tracks.size(); ++n) if (tracks[n].filename == name) return i32(n);
    return -1; // Missing assets are errors, never silently substituted by title music.
}
}
