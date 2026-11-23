#include "AnmResource.hpp"

namespace th16 {
namespace {

bool parse_script(const u8* base, u32 size, u32 offset, AnmScript& script, std::string& error) {
    if (offset >= size) { error = "script offset outside file"; return false; }
    script.offset = offset;
    // 脚本以 {type=0xffff, length=0} 哨兵结束（thanm 同约定）；哨兵 8 字节一并保留在 bytes 中，
    // 供解释器按原始偏移寻址并在哨兵处停机。
    u32 cursor = offset;
    while (cursor + 8 <= size) {
        AnmInstruction ins;
        ins.type = read_u16(base + cursor);
        ins.length = read_u16(base + cursor + 2);
        ins.time = i16(read_u16(base + cursor + 4));
        ins.param_mask = read_u16(base + cursor + 6);
        ins.offset = cursor - offset;
        if (ins.type == 0xffff) { script.instructions.push_back(ins); cursor += 8; break; }
        if (ins.length < 8 || cursor + ins.length > size) {
            error = "script instruction length out of range";
            return false;
        }
        ins.arg_bytes = ins.length - 8;
        script.instructions.push_back(ins);
        cursor += ins.length;
    }
    script.bytes.assign(base + offset, base + cursor);
    for (auto& ins : script.instructions) ins.args = script.bytes.data() + ins.offset + 8;
    return true;
}

}  // namespace

bool AnmResource::decode_thtx(const u8* p, u32 size, AnmTexture& out, std::string& error) {
    if (size < 16 || std::memcmp(p, "THTX", 4)) { error = "THTX magic missing"; return false; }
    out.format = read_u16(p + 6);
    out.width = read_u16(p + 8);
    out.height = read_u16(p + 10);
    const u32 bytes = read_u32(p + 12);
    if (bytes > size - 16) { error = "THTX payload truncated"; return false; }
    const u8* d = p + 16;
    const u32 pixels = u32(out.width) * out.height;
    out.bgra.resize(pixels * 4);
    switch (out.format) {
    case 1:  // BGRA8888
        if (bytes < pixels * 4) { error = "THTX BGRA8888 payload short"; return false; }
        std::memcpy(out.bgra.data(), d, pixels * 4);
        return true;
    case 3:  // RGB565
        if (bytes < pixels * 2) { error = "THTX RGB565 payload short"; return false; }
        for (u32 i = 0; i < pixels; ++i) {
            const u16 v = read_u16(d + i * 2);
            const u8 r5 = u8((v >> 11) & 31), g6 = u8((v >> 5) & 63), b5 = u8(v & 31);
            u8* o = out.bgra.data() + i * 4;
            o[0] = u8((b5 << 3) | (b5 >> 2));
            o[1] = u8((g6 << 2) | (g6 >> 4));
            o[2] = u8((r5 << 3) | (r5 >> 2));
            o[3] = 255;
        }
        return true;
    case 5:  // ARGB4444（字节序 0xGB 0xAR）
        if (bytes < pixels * 2) { error = "THTX ARGB4444 payload short"; return false; }
        for (u32 i = 0; i < pixels; ++i) {
            const u16 v = read_u16(d + i * 2);
            const u8 a = u8((v >> 12) & 15), r = u8((v >> 8) & 15), g = u8((v >> 4) & 15), b = u8(v & 15);
            u8* o = out.bgra.data() + i * 4;
            o[0] = u8(b * 17);
            o[1] = u8(g * 17);
            o[2] = u8(r * 17);
            o[3] = u8(a * 17);
        }
        return true;
    case 7:  // GRAY8
        if (bytes < pixels) { error = "THTX GRAY8 payload short"; return false; }
        for (u32 i = 0; i < pixels; ++i) {
            u8* o = out.bgra.data() + i * 4;
            o[0] = o[1] = o[2] = d[i];
            o[3] = 255;
        }
        return true;
    default:
        error = "THTX unknown format " + std::to_string(out.format);
        out.bgra.clear();
        return false;
    }
}

bool AnmResource::parse(const u8* bytes, u32 size, const char* file_name) {
    entries.clear();
    error.clear();
    name = file_name ? file_name : "";
    total_sprites = total_scripts = 0;
    u32 at = 0;
    for (;;) {
        if (at + 0x40 > size) { error = "ANM entry header truncated"; return false; }
        const u8* h = bytes + at;
        AnmEntry e;
        e.version = read_u32(h);
        if (e.version != 8) { error = "ANM version " + std::to_string(e.version) + " (expected 8)"; return false; }
        const u16 sprites = read_u16(h + 4), scripts = read_u16(h + 6);
        e.width = read_u16(h + 0x0a);
        e.height = read_u16(h + 0x0c);
        e.format = read_u16(h + 0x0e);
        const u32 name_offset = read_u32(h + 0x10);
        e.x = read_u16(h + 0x14);
        e.y = read_u16(h + 0x16);
        e.memory_priority = read_u32(h + 0x18);
        const u32 thtx_offset = read_u32(h + 0x1c);
        e.has_data = read_u16(h + 0x20) != 0;
        e.low_res_scale = h[0x22];
        const u32 next = read_u32(h + 0x24);
        if (name_offset >= size - at) { error = "ANM name offset outside file"; return false; }
        {
            const u8* n = h + name_offset;
            u32 len = 0;
            while (n + len < bytes + size && n[len]) ++len;
            e.name.assign(reinterpret_cast<const char*>(n), len);
        }
        const u32 table = at + 0x40;
        if (table + u32(sprites) * 4 + u32(scripts) * 8 > size) { error = "ANM offset table truncated"; return false; }
        e.sprites.reserve(sprites);
        for (u32 i = 0; i < sprites; ++i) {
            const u32 so = read_u32(bytes + table + i * 4);
            if (so + 20 > size - at) { error = "ANM sprite record outside file"; return false; }
            const u8* s = h + so;
            e.sprites.push_back({read_u32(s), read_f32(s + 4), read_f32(s + 8), read_f32(s + 12), read_f32(s + 16)});
        }
        e.scripts.reserve(scripts);
        const u32 entry_limit = next ? next : size - at;  // 脚本不跨 entry
        for (u32 i = 0; i < scripts; ++i) {
            const u8* rec = bytes + table + sprites * 4 + i * 8;
            AnmScript script;
            script.id = read_i32(rec);
            const u32 so = read_u32(rec + 4);
            if (!parse_script(h, entry_limit, so, script, error)) return false;
            e.scripts.push_back(std::move(script));
        }
        if (e.has_data && thtx_offset) {
            if (thtx_offset >= size - at) { error = "ANM THTX offset outside file"; return false; }
            if (!decode_thtx(h + thtx_offset, size - at - thtx_offset, e.texture, error)) return false;
        }
        total_sprites += sprites;
        total_scripts += scripts;
        entries.push_back(std::move(e));
        if (!next) break;
        if (next < 0x40 || next > size - at) { error = "ANM nextoffset out of range"; return false; }
        at += next;
    }
    return true;
}

bool AnmResource::attach_external_texture(u32 index, const u8* bytes, u32 size) {
    if (index >= entries.size()) { error = "entry index out of range"; return false; }
    return decode_thtx(bytes, size, entries[index].texture, error);
}

}  // namespace th16
