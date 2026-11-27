// th16 回放记录——原版 ReplayManager 记录侧（0x447fd0 逐帧写入、0x448eb0 开段、0x448400 落盘）。
//
// 锚点：
//   0x447fd0  每帧：自动低速（DAT_004c12cc bit9）后写 held/pressed/released 各 u16，每 30 帧写 FPS 字节
//   0x448eb0  关卡起点：清帧计数、为当前关分配输入块、抄 GameManager 快照进段头 0x294
//   0x448400  组装用户块 0xa0 + 各段 → LZSS(0x457b20) → 双重 0x402220 → 写 replay/th16_XX.rpy + USER 说明块
//
// GameSession 在非回放局把记录链接到本类；GameMenu::ReplayStore::save 调 write_slot。
#pragma once
#include "GameMenu.hpp"
#include "ReplayFile.hpp"
#include "Types.hpp"

#include <string>
#include <vector>

namespace th16 {

struct GameGlobals;

struct ReplayRecording {
    static constexpr u32 kMaxInputBytes = 2u * 1024u * 1024u;

    std::vector<u8> user;                         // 0xa0 用户块（开局填机体/难度/季节；保存时写名字/分数）
    std::vector<ReplayStageSegment> stages;       // 已封口的关卡段
    ReplayStageSegment current;                   // 进行中的段（frames 随 tick 增长）
    bool stage_open = false;
    bool sealed = false;
    i32 timestamp = 0;                            // 用户块 +0xc（time32）
    std::vector<u8> pending_globals;              // 下一段段头 +0x14 的全局快照（0x8a dword）
    bool pending_new_game = false;                // 段头 +0x290 bit0（DAT_004c17d0）
    std::string error;

    void reset();
    // 开局：填用户块中的机体 / 难度 / 季节 / 模式标志；时间戳由宿主提供
    void begin_run(const GameGlobals& globals, i32 time32);
    // 0x447760（新开局，ReplayManager 构造时）/ 0x448eb0（换关首帧）：抄 DAT_004a5790 全局块，供下一段段头用
    void snapshot_globals(const GameGlobals& globals, bool new_game);
    // 0x448eb0：开始新关卡段（封口上一关）；seed = game_rng 种子低 16 位；段头带上待用快照
    void begin_stage(i16 stage, u16 rng_seed);
    // 0x448eb0（换关，DAT_004c17d0 == 0）：当前段段头 +0x14 改为此刻的全局块
    void set_stage_globals(const GameGlobals& globals);
    // 0x448eb0：段头 +0xc / +0x10 自机定点坐标、+0x23c 低速标志（Player +0x61c / +0x620 / +0x165c8）
    void set_stage_start(i32 fixed_x, i32 fixed_y, i32 focus);
    // 装载后外部改写全局（harness 残机覆盖）时重抄：已开段写当前段头，否则覆盖待用快照
    void restamp_globals(const GameGlobals& globals);
    // 0x417bc0：当前段段头 +0x240 + index × 4 写符卡实时用时编码（0x447760 预填 0, -0x21522153, …）
    void set_spell_time(i32 index, i32 value);
    // 0x447fd0：记录一帧（held/pressed/released 低 16 位；fps 每 30 帧采样一次）
    bool tick(u16 held, u16 pressed, u16 released, u8 fps_byte);
    // 封口当前关（换关 / 结束前）；可选追加 0xffff 终止三元组（0x448400 非 spell 路径）
    void close_stage(bool append_terminator);
    // 0x4483b0 / 0x4406b8：选存档槽时重取时间戳，写当前关卡或通关标记。
    void set_save_metadata(i32 stage, i32 time32);
    // 本局用户块的列表摘要（名字栏为空格）
    ReplaySummary summary() const;
    // 0x448400：写出 th16_XX.rpy（directory 下优先 replay/ 子目录；失败时写 directory 根）
    bool write_slot(const char* directory, int slot, const char* player_name, i32 score, float slow_rate);
    // 结构化 → 文件字节（不含路径）；成功时 out 含头 + 密文 + USER 说明块
    bool encode(std::vector<u8>& out, const char* player_name, i32 score, float slow_rate);

    // 供菜单列表：从磁盘槽读头
    static bool slot_exists(const char* directory, int slot);
    static bool slot_summary(const char* directory, int slot, ReplaySummary& out);
};

}  // namespace th16
