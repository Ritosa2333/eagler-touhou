#include "ShotTable.hpp"

namespace th16 {

bool ShotTable::parse(const u8* bytes, u32 size) {
    if (size < 0x1b8) { error = "sht too small"; return false; }
    header = i16(read_u16(bytes));
    set_count = i16(read_u16(bytes + 2));
    hitbox = read_f32(bytes + 0x04);
    graze = read_f32(bytes + 0x08);
    item = read_f32(bytes + 0x0c);
    speed = read_f32(bytes + 0x10);
    speed_focus = read_f32(bytes + 0x14);
    speed_diag = read_f32(bytes + 0x18);
    speed_diag_focus = read_f32(bytes + 0x1c);
    max_level = read_i32(bytes + 0x20);
    power_per_level = read_i32(bytes + 0x24);
    damage_cap = read_i32(bytes + 0x28);
    for (int i = 0; i < kOptionSlots; ++i) {
        option_unfocused[i] = {read_f32(bytes + 0x40 + i * 8), read_f32(bytes + 0x44 + i * 8)};
        option_focused[i] = {read_f32(bytes + 0xe8 + i * 8), read_f32(bytes + 0xec + i * 8)};
    }
    sets.clear();
    if (set_count < 0 || u32(set_count) * 4 + 0x190 > size) { error = "sht set table out of range"; return false; }
    for (int s = 0; s < set_count; ++s) {
        const i32 rel = read_i32(bytes + 0x190 + s * 4);
        std::vector<ShotEntry> set;
        u32 off = u32(0x1b8 + rel);
        while (true) {
            if (off >= size) { error = "sht set runs past end"; return false; }
            if (bytes[off] & 0x80) break;  // 首字节 < 0：组结束
            if (off + ShotEntry::kSize > size) { error = "sht entry truncated"; return false; }
            ShotEntry entry;
            std::memcpy(entry.raw, bytes + off, ShotEntry::kSize);
            set.push_back(entry);
            off += ShotEntry::kSize;
        }
        sets.push_back(std::move(set));
    }
    return true;
}

}  // namespace th16
