// th16.cfg——原版 100 字节配置块 DAT_004c12a4（0x43c050 启动时读、校验、回写；0x459830 退出前再写）。
//
// 块布局（相对 DAT_004c12a4）：
//   +0x00 u32  magic 0x160002
//   +0x04 i16×10 手柄按钮表（与 DAT_004a52e4 互抄；-1 = 未绑定）
//   +0x18 u16  600 / +0x1a u16 600（仅存档，无读者）
//   +0x1c u8   窗口模式 (<2)      +0x1d u8 BGM 模式 (<3；1 = WAV 串流)   +0x1e u8 SE 开关 (<2)
//   +0x1f u8   分辨率档 (<6)      +0x20 u8 跳帧 (<3)                     +0x21 u8 保留 (<3)
//   +0x22 u8   BGM 音量 0..100    +0x23 u8 SE 音量 0..100                +0x24 u8 Option 标志
//   +0x25 u8   2（0x45b330 / 0x45b7d0 窗口刷新模式）                    +0x28 u32 输入配置（|= 0x100）
//   +0x2c i32  窗口 X / +0x30 i32 窗口 Y（0x80000000 = 默认位置）        其余 0
// 校验（0x43c050）：六个字节在各自上限内、magic 相符、文件恰为 100 字节；否则整块回默认值。
#pragma once
#include "GameGlobals.hpp"
#include "Types.hpp"

namespace th16::config {

inline constexpr u32 kSize = 100;
inline constexpr u32 kMagic = 0x160002;

// 0x43c050 两处相同的默认块写入（手柄表取 DAT_004a52e4 的 exe 初值）
void reset(GameGlobals& g);
// 0x43c050 读入后的校验 + 采用；不合法时返回 false 并已回默认值
bool decode(GameGlobals& g, const u8* bytes, u32 size);
// 退出 / 读入后回写用的 100 字节映像
void encode(const GameGlobals& g, u8 out[kSize]);

}  // namespace th16::config
