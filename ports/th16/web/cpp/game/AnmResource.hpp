// th16 ANM（version 8）资源解析——原版 0x46cdd0（bank 装载）/ 0x46d0c0（条目校验）的格式面。
//
// 0x46cdd0：沿 +0x24 nextoffset 链遍历全部 entry；累加 +0x04 u16 精灵数（运行时每精灵 0x44 字节）
//   与 +0x06 u16 脚本数（每脚本 4 字节指针 + 0x5fc 字节 VM 槽）。
// 0x46d0c0：+0x00 version 必须为 8；+0x20 hasdata 为 0 且名字首字符非 '@' 时从档案另读贴图文件。
// entry 头布局（0x40 字节，与 thtk anm_header11_t 一致）：
//   +0x00 u32 version   +0x04 u16 sprites  +0x06 u16 scripts  +0x08 u16 zero
//   +0x0a u16 w         +0x0c u16 h        +0x0e u16 format   +0x10 u32 nameoffset
//   +0x14 u16 x         +0x16 u16 y        +0x18 u32 memorypriority  +0x1c u32 thtxoffset
//   +0x20 u16 hasdata   +0x22 u8 lowresscale +0x23 u8 pad     +0x24 u32 nextoffset
//   +0x28 … 0x40 零填充；随后 sprites×u32 精灵偏移、scripts×{i32 id,u32 offset}
// 精灵记录：u32 id, f32 x,y,w,h。脚本指令：u16 type, u16 length, i16 time, u16 param_mask, data[]。
// THTX：'THTX' u16 zero u16 format u16 w u16 h u32 size data[]；format 1=BGRA8888 3=RGB565 5=ARGB4444 7=GRAY8。
#pragma once
#include "Types.hpp"

namespace th16 {

struct AnmSprite {
    u32 id = 0;
    float x = 0, y = 0, w = 0, h = 0;
};

struct AnmInstruction {
    u16 type = 0, length = 0;
    i16 time = 0;
    u16 param_mask = 0;
    u32 offset = 0;      // 指令在脚本字节流中的偏移（跳转目标用）
    const u8* args = nullptr;
    u32 arg_bytes = 0;
};

struct AnmScript {
    i32 id = 0;
    u32 offset = 0;
    std::vector<u8> bytes;                 // 原始指令流（含哨兵）
    std::vector<AnmInstruction> instructions;
};

struct AnmTexture {
    u16 format = 0, width = 0, height = 0;
    std::vector<u8> bgra;                  // 解码后的 BGRA8，width*height*4
    bool present() const { return !bgra.empty(); }
};

struct AnmEntry {
    u32 version = 0;
    u16 width = 0, height = 0, format = 0;
    u16 x = 0, y = 0;
    u32 memory_priority = 0;
    bool has_data = false;
    u8 low_res_scale = 0;
    std::string name;
    std::vector<AnmSprite> sprites;
    std::vector<AnmScript> scripts;
    AnmTexture texture;
};

class AnmResource {
public:
    std::string name, error;
    std::vector<AnmEntry> entries;
    u32 total_sprites = 0, total_scripts = 0;

    bool parse(const u8* bytes, u32 size, const char* name);
    // 外部贴图（hasdata=0 且名字非 '@'）：由调用方从档案读取同名文件后调用，解码写入对应 entry。
    bool attach_external_texture(u32 entry, const u8* bytes, u32 size);

    // 把 THTX 像素解码为 BGRA8；返回 false 时 error 说明原因。
    static bool decode_thtx(const u8* bytes, u32 size, AnmTexture& out, std::string& error);
};

}  // namespace th16
