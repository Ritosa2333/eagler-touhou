// th16 .sht 射击表——原版 0x443790 装载（pl0N.sht 机体表 / pl0Nsub.sht 子季节表）。
//
// 文件布局（0x443790 + 0x440fb0 / 0x4440e0 / 0x4456d0 的读法）：
//   +0x00 i16            未用（恒 4）
//   +0x02 i16            射击组数（count）
//   +0x04 f32            判定半径（装载后被 exe 表 DAT_00492c98[shot] 覆盖 = 3.0）
//   +0x08 f32            擦弹半径（被 DAT_00492c78[shot] 覆盖 = 5.0）
//   +0x0c f32            收点半径（被 DAT_00492c88[shot] 覆盖 = 60.0）
//   +0x10..+0x1c f32×4   速度：非低速 / 低速 / 斜向非低速 / 斜向低速（0x440fb0 ×128 取整）
//   +0x20 i32            最大火力等级（4）
//   +0x24 i32            每级火力（装载后被 0x440fb0 覆盖为 100）
//   +0x40 f32×2 ×21      子机位置（非低速），+0xe8 ×21 低速；下标来自 DAT_004a5e4c / DAT_004a5dac 表
//   +0x190 i32 ×count    各射击组相对 +0x1b8 的偏移
//   每组：0x58 字节条目若干，首字节 < 0 结束；+0x28/+0x2c/+0x30/+0x34 为回调表下标
//   （DAT_004919c0 / DAT_004919a0 / DAT_004a6f04 / DAT_00491980），装载时就地换成函数指针
#pragma once
#include "Types.hpp"

#include <string>
#include <vector>

namespace th16 {

// 射击组条目（0x58）：0x445470 / 0x445360 / 0x444e10 的读法
//   +0x00 i8  发射周期（主射击计时 % 周期）  +0x01 i8 发射相位      +0x02 i16 伤害
//   +0x04 f32 x 偏移  +0x08 f32 y 偏移      +0x0c/+0x10 f32 判定宽高  +0x14 f32 角度（≥995 取子机角度；≥1000 加随机）
//   +0x18 f32 速度    +0x20 i8 发射点（0 自机；1..8 子机；101.. 子季节子机） +0x21 i8 类型（2 = 每子机按火力等级限一发）
//   +0x22 i16 ANM 脚本（主表 +5）           +0x24 i16 音效（<0 无）
//   +0x26 i8  第二计时周期（非 0 时改用 shot_timer2）+0x27 i8 第二相位
//   +0x28 生成回调  +0x2c 逐帧回调  +0x30 颜色  +0x34 命中回调（表下标；装载时换成函数指针）
struct ShotEntry {
    static constexpr u32 kSize = 0x58;
    u8 raw[kSize] = {};

    i32 i32_at(u32 off) const { return read_i32(raw + off); }
    float f32_at(u32 off) const { return read_f32(raw + off); }
    i8 i8_at(u32 off) const { return i8(raw[off]); }
    i16 i16_at(u32 off) const { return i16(read_u16(raw + off)); }

    i8 period() const { return i8_at(0); }
    i8 phase() const { return i8_at(1); }
    i16 damage() const { return i16_at(2); }
    float offset_x() const { return f32_at(4); }
    float offset_y() const { return f32_at(8); }
    float hit_w() const { return f32_at(0xc); }
    float hit_h() const { return f32_at(0x10); }
    float angle() const { return f32_at(0x14); }
    float speed() const { return f32_at(0x18); }
    i8 source() const { return i8_at(0x20); }
    i8 kind() const { return i8_at(0x21); }
    i16 script() const { return i16_at(0x22); }
    i16 sfx() const { return i16_at(0x24); }
    i8 period2() const { return i8_at(0x26); }
    i8 phase2() const { return i8_at(0x27); }
    i32 create_callback() const { return i32_at(0x28); }
    i32 tick_callback() const { return i32_at(0x2c); }
    i32 color_index() const { return i32_at(0x30); }
    i32 hit_callback() const { return i32_at(0x34); }
};

struct ShotTable {
    static constexpr int kOptionSlots = 21;

    i16 header = 0;
    i16 set_count = 0;
    float hitbox = 0;        // +0x04
    float graze = 0;         // +0x08
    float item = 0;          // +0x0c
    float speed = 0;         // +0x10
    float speed_focus = 0;   // +0x14
    float speed_diag = 0;    // +0x18
    float speed_diag_focus = 0;  // +0x1c
    i32 max_level = 0;       // +0x20
    i32 power_per_level = 0; // +0x24
    i32 damage_cap = 0;      // +0x28（0x445a30 单次结算伤害上限）
    Vec2 option_unfocused[kOptionSlots];  // +0x40
    Vec2 option_focused[kOptionSlots];    // +0xe8
    std::vector<std::vector<ShotEntry>> sets;
    std::string error;

    bool parse(const u8* bytes, u32 size);
};

}  // namespace th16
