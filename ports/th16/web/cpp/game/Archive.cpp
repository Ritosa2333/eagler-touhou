#include "Archive.hpp"

namespace th16 {

bool Archive::open(const u8* bytes, u32 size) {
    entries.clear();
    error.clear();
    source = nullptr;
    source_size = 0;
    if (size < 16) { error = "archive shorter than header"; return false; }
    u8 header[16];
    resource_crypt(bytes, header, 16, kHeaderCipher, false);
    if (std::memcmp(header, "THA1", 4)) { error = "THA1 magic mismatch"; return false; }
    const u32 unpacked = read_u32(header + 4) - 123456789u;
    const u32 packed = read_u32(header + 8) - 987654321u;
    const u32 count = read_u32(header + 12) - 135792468u;
    if (!count || count > 100000 || !packed || packed > size - 16 || unpacked > 64u * 1024 * 1024 || unpacked < count * 16) {
        error = "THA1 header fields out of range";
        return false;
    }
    const u32 offset = size - packed;
    std::vector<u8> encoded(packed), table(unpacked);
    CryptParams tableCipher = kTableCipher;
    tableCipher.limit = packed;
    resource_crypt(bytes + offset, encoded.data(), packed, tableCipher, false);
    u32 written = 0;
    if (!codec.decode(encoded.data(), packed, table.data(), unpacked, written) || written != unpacked) {
        error = "THA1 directory LZSS size mismatch";
        return false;
    }
    u32 cursor = 0;
    std::vector<ArchiveEntry> parsed;
    parsed.reserve(count);
    for (u32 i = 0; i < count; ++i) {
        const u32 begin = cursor;
        while (cursor < unpacked && table[cursor]) ++cursor;
        if (cursor == unpacked || cursor - begin > 1024) { error = "THA1 directory name overrun"; return false; }
        ArchiveEntry e;
        e.name.assign(reinterpret_cast<const char*>(table.data() + begin), cursor - begin);
        cursor = begin + ((cursor - begin + 4) & ~3u);  // 0x4574b0：名字长度含终止符后按 4 对齐
        if (cursor > unpacked || unpacked - cursor < 12) { error = "THA1 directory truncated"; return false; }
        e.offset = read_u32(table.data() + cursor);
        e.size = read_u32(table.data() + cursor + 4);
        e.reserved = read_u32(table.data() + cursor + 8);
        cursor += 12;
        if (e.offset < 16 || e.offset > offset || e.size > 64u * 1024 * 1024) { error = "THA1 entry outside archive"; return false; }
        if (!parsed.empty()) {
            if (e.offset < parsed.back().offset) { error = "THA1 entries not ascending"; return false; }
            parsed.back().compressed = e.offset - parsed.back().offset;
        }
        parsed.push_back(std::move(e));
    }
    parsed.back().compressed = offset - parsed.back().offset;
    entries.swap(parsed);
    source = bytes;
    source_size = size;
    return true;
}

i32 Archive::find(const char* name) const {
    for (u32 i = 0; i < entries.size(); ++i)
        if (entries[i].name == name) return i32(i);
    return -1;
}

bool Archive::read(u32 index, std::vector<u8>& output) {
    output.clear();
    if (!source || index >= entries.size()) { error = "archive not open or index out of range"; return false; }
    const auto& e = entries[index];
    u8 sum = 0;
    for (unsigned char c : e.name) sum = u8(sum + c);  // 0x457120：名字字节和 & 7 选密钥
    std::vector<u8> decoded(e.compressed);
    resource_crypt(source + e.offset, decoded.data(), e.compressed, kEntryCiphers[sum & 7], false);
    if (e.compressed == e.size) { output.swap(decoded); return true; }
    output.resize(e.size);
    u32 written = 0;
    if (!codec.decode(decoded.data(), u32(decoded.size()), output.data(), u32(output.size()), written) || written != e.size) {
        output.clear();
        error = "entry LZSS size mismatch: " + e.name;
        return false;
    }
    return true;
}

}  // namespace th16
