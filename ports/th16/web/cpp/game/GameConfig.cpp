#include "GameConfig.hpp"

#include <cstring>

namespace th16::config {

namespace {

u16 read16(const u8* p) { return u16(p[0] | (p[1] << 8)); }
u32 read32(const u8* p) { return u32(p[0]) | (u32(p[1]) << 8) | (u32(p[2]) << 16) | (u32(p[3]) << 24); }
void write16(u8* p, u16 v) { p[0] = u8(v); p[1] = u8(v >> 8); }
void write32(u8* p, u32 v) { p[0] = u8(v); p[1] = u8(v >> 8); p[2] = u8(v >> 16); p[3] = u8(v >> 24); }

}  // namespace

void reset(GameGlobals& g) {
    // _memset(&DAT_004c12a4, 0, 100) 后逐项写默认；DAT_004c12cc 只做 |= 0x100（memset 后即 0x100）
    for (int i = 0; i < GameGlobals::kPadButtonCount; ++i) g.pad_buttons[i] = GameGlobals::kPadButtonDefaults[i];
    g.window_mode = 0;
    g.bgm_mode = 1;
    g.se_enabled = 1;
    g.resolution = 5;
    g.frame_skip = 0;
    g.config_c5 = 2;
    g.bgm_volume = 100;
    g.se_volume = 80;
    g.option_flags = 0;
    g.config_c9 = 2;
    g.input_config = 0x100;
    g.window_x = i32(0x80000000);
    g.window_y = i32(0x80000000);
}

bool decode(GameGlobals& g, const u8* bytes, u32 size) {
    // 0x43c050 先按 0x19 个 dword 整块抄入再校验；校验失败整块回默认（含手柄表）
    if (size != kSize || bytes == nullptr || read32(bytes) != kMagic ||
        bytes[0x1c] >= 2 || bytes[0x1d] >= 3 || bytes[0x1e] >= 2 || bytes[0x1f] >= 6 || bytes[0x20] >= 3 ||
        bytes[0x21] >= 3) {
        reset(g);
        return false;
    }
    for (int i = 0; i < GameGlobals::kPadButtonCount; ++i) g.pad_buttons[i] = i16(read16(bytes + 4 + i * 2));
    g.window_mode = bytes[0x1c];
    g.bgm_mode = bytes[0x1d];
    g.se_enabled = bytes[0x1e];
    g.resolution = bytes[0x1f];
    g.frame_skip = bytes[0x20];
    g.config_c5 = bytes[0x21];
    g.bgm_volume = bytes[0x22];
    g.se_volume = bytes[0x23];
    g.option_flags = bytes[0x24];
    g.config_c9 = bytes[0x25];
    g.input_config = read32(bytes + 0x28);
    g.window_x = i32(read32(bytes + 0x2c));
    g.window_y = i32(read32(bytes + 0x30));
    return true;
}

void encode(const GameGlobals& g, u8 out[kSize]) {
    std::memset(out, 0, kSize);
    write32(out, kMagic);
    for (int i = 0; i < GameGlobals::kPadButtonCount; ++i) write16(out + 4 + i * 2, u16(g.pad_buttons[i]));
    write16(out + 0x18, 600);
    write16(out + 0x1a, 600);
    out[0x1c] = g.window_mode;
    out[0x1d] = g.bgm_mode;
    out[0x1e] = g.se_enabled;
    out[0x1f] = g.resolution;
    out[0x20] = g.frame_skip;
    out[0x21] = g.config_c5;
    out[0x22] = g.bgm_volume;
    out[0x23] = g.se_volume;
    out[0x24] = g.option_flags;
    out[0x25] = g.config_c9;
    write32(out + 0x28, g.input_config);
    write32(out + 0x2c, u32(g.window_x));
    write32(out + 0x30, u32(g.window_y));
}

}  // namespace th16::config
