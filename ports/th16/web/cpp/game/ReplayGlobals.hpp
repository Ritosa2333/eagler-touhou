// th16 回放段头 +0x14 的全局快照——原版把 DAT_004a5790 起 0x8a 个 dword（0x228 字节，止于 DAT_004a59b8 最高分）
// 整块 rep movsd：录制 0x447760 / 0x448eb0 写入，回放 0x447760 / 0x449030 读回。
// 本移植的 GameGlobals 是具名字段而非字节镜像，故按原版偏移逐字段映射；未建模的槽位录制时写 0、回放时忽略。
#pragma once
#include "Types.hpp"

namespace th16 {

struct GameGlobals;

constexpr u32 kReplayGlobalsOffset = 0x14;   // 段头内偏移
constexpr u32 kReplayGlobalsSize = 0x228;

void store_replay_globals(const GameGlobals& globals, u8* out);
void load_replay_globals(GameGlobals& globals, const u8* in);

}  // namespace th16
