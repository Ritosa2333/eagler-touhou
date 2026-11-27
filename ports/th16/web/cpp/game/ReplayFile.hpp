// th16 回放文件——原版 0x448c10（读 replay/%s 或内嵌缓冲）+ 头 0x24（t16r / ver 2）。
//
// 文件：+0x00 magic 0x72363174 "t16r"、+0x04 u16 version=2、+0x1c 压缩载荷长、+0x20 展开长；
// 载荷经 0x402220(key=0x5c,step=-0x1f,block=0x400) 再 (0x7d,':',0x100) 后 0x457f00 LZSS。
// 展开缓冲：+0x00 用户块 0xa0（名字 / 关卡数 +0x80 等），+0xa0 起每关一段：
//   段头 0x294：+0 i16 关卡、+2 u16 RNG 种子、+4 i32 帧数、+8 i32 附加数据长（= 帧×6 + ⌈帧/30⌉）；
//   +0x294 起每帧 6 字节输入（held u16 + 两路 u16），其后每 30 帧一字节 FPS。
#pragma once
#include "Types.hpp"

#include <string>
#include <vector>

namespace th16 {

struct ReplayStageSegment {
    i16 stage = 0;
    u16 rng_seed = 0;
    i32 frames = 0;
    std::vector<u8> header;  // 完整 0x294（读档保留；录制时可空，写出时合成）
    std::vector<u8> inputs;  // frames × 6
    std::vector<u8> fps;     // 每 30 帧一字节（含第 0 帧）
};

// 段头 +0x240：0x14 个符卡实时用时槽（0x417bc0 按 Spellcard+0x88 计次写 / 读）
constexpr i32 kSpellTimeOffset = 0x240;
constexpr i32 kSpellTimeSlots = 0x14;

struct ReplayFile {
    static constexpr u32 kMagic = 0x72363174u;  // "t16r"
    static constexpr u16 kVersion = 2;
    static constexpr u32 kHeaderSize = 0x24;
    static constexpr u32 kUserSize = 0xa0;
    static constexpr u32 kStageHeaderSize = 0x294;

    char name[9] = {};
    i32 stage_count = 0;
    std::vector<u8> user;                 // 完整 0xa0
    std::vector<ReplayStageSegment> stages;
    std::string error;

    bool open(const u8* bytes, u32 size);
    bool open_file(const char* path);
    // 0x448400 压缩段：user + 各段头/体 → LZSS → 双重加密 → t16r 头（不含尾部 USER 说明块）
    bool encode(std::vector<u8>& out);
    bool write_file(const char* path);
};

// 0x448130 单帧：从当前关卡段取 6 字节写入 game_input 口径的 held / 辅助字，并刷新 fps 字节。
// frame_index 为段内已播放帧（调用后由调用方 +1，与原版 +0xd8 / +0x20c 同步）。
struct ReplayPlaybackCursor {
    i32 stage_index = 0;
    i32 frame_index = 0;
    u8 fps_byte = 60;
    bool ended = false;
    bool terminated = false;   // 读到 0xffff×3 终止帧（0x448130 → 0x43f240、+0x214 = -1）；调用方处理后清除
};

bool replay_playback_step(const ReplayFile& file, ReplayPlaybackCursor& cursor,
                          u32& held_low16, u32& aux0, u32& aux1, u8& fps_byte);

// 目录型槽位路径：th16_XX.rpy（0x447ef0）；先查 directory/ 再 directory/replay/
bool replay_slot_path(const char* directory, int slot, char* out, size_t out_n);
bool replay_slot_exists(const char* directory, int slot);

}  // namespace th16
