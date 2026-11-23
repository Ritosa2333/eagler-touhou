// th16 乐曲——原版 SoundManager DAT_004d9e10 的 BGM 部分：命令队列与曲目表（串流播放器 DAT_004df470 由宿主实现）。
//
//   曲目表 DAT_004db790：thbgm.fmt（0x43adc0 装入），每条 0x34 字节：名 [16] / 数据偏移 / 长度 / 循环起点 / 长度 / WAVEFORMATEX
//   曲名槽 DAT_004de370[16][0x100]：0x43c370(槽, 名) → 命令 1 登记 "<名>.wav"；命令 2 按槽取名
//   当前曲 DAT_004dc0f0：命令 2 第 1 步写入（0x42d200 卸关清空）
//   命令队列 DAT_004dc1f0：0x1f 条 × {kind, track, step, name[0x100]}，0x45ed00 追加；0x45e330 每次只推进队头一步，
//   队头完成即出队，返回新队头 kind（空 = 0）——调用方以 do … while (ret != 0) 排空
//     1 登记曲名槽（一步）                  2 播放：stop → 取名 / 查表 / open → seek 0 → 填缓冲 play → start → 等两步 → 出队
//     3 停止：stop(关文件) → 一步后出队      4 关闭串流器：stop(关文件) →（声音线程句柄为空时）出队
//     5 淡出 track 秒（×60 帧）              6 暂停 / 7 恢复（DAT_004c12c1 乐曲开时）     8 音量重置     9 按名换循环曲目
//   整曲预载（DAT_004c12cc bit4，custom.exe 选项）的分支不移植：端口固定走串流。
#pragma once
#include <array>
#include <string>
#include <vector>

#include "Types.hpp"

namespace th16 {

// thbgm.fmt 一条（0x34 字节）
struct BgmEntry {
    std::string name;            // +0x00 "th16_01.wav"
    u32 offset = 0;              // +0x10 thbgm.dat 内偏移
    u32 size = 0;                // +0x14 数据长度（与 +0x1c 同值）
    u32 loop_start = 0;          // +0x18 循环起点（字节）
    u32 length = 0;              // +0x1c 数据长度（0 = 不循环）
    u16 format_tag = 1;          // +0x20
    u16 channels = 2;            // +0x22
    u32 sample_rate = 44100;     // +0x24
    u32 bytes_per_second = 0;    // +0x28
    u16 block_align = 4;         // +0x2c
    u16 bits_per_sample = 16;    // +0x2e
};

// 宿主串流播放器（原版 DAT_004df470：DirectSound 流缓冲 + thbgm.dat 读取器）
struct MusicOutput {
    virtual ~MusicOutput() = default;
    virtual void stop(bool close_file) = 0;                    // 0x471270
    virtual void open(const BgmEntry& entry) = 0;              // 0x470bb0：绑定曲目
    virtual void seek(const BgmEntry& entry, u32 offset) = 0;  // 0x4718a0：定位到曲内字节偏移并预读
    virtual void play(bool looped, u32 offset) = 0;            // 0x471720 + 0x470ed0：填满流缓冲
    virtual void start() = 0;                                  // 0x471120：IDirectSoundBuffer::Play
    virtual void pause() = 0;                                  // 0x4712f0
    virtual void resume() = 0;                                 // 0x471380
    virtual void halt() = 0;                                   // 0x43dcf0 淡出到底：仅 IDirectSoundBuffer::Stop（文件与位置保留）
    virtual void fade(i32) {}                                  // 0x45e330 case 5 登记淡出帧数（通知用；音量由 MusicManager::tick_fade 逐帧推）
    virtual void set_volume(i32 base) = 0;                     // 0x4711f0：base = 0 或淡出中间值（-5000..0）；宿主经 MusicManager::ds_volume(base) 换算
    virtual void switch_loop(const BgmEntry& entry) = 0;       // 0x471b00：保持相位换曲
    virtual double position_seconds() = 0;                     // 0x471bd0：当前播放时刻（秒）
    virtual void restart_at(double seconds) = 0;               // 0x471c90：从指定时刻重新开始
};

class MusicManager {
public:
    struct Command {
        i32 kind = 0;
        i32 track = 0;
        i32 step = 0;
        std::string name;
    };
    static constexpr int kQueueCapacity = 0x1f;
    static constexpr int kSlotCount = 16;

    std::vector<BgmEntry> entries;                  // DAT_004db790
    std::array<std::string, kSlotCount> slot_names; // DAT_004de370
    std::string current;                            // DAT_004dc0f0
    std::vector<Command> queue;                     // DAT_004dc1f0
    bool ready = true;        // DAT_004d9e1c：音频设备存在（为假时 0x45e330 整体空转，连音效也不消费）
    bool streamer_live = true;  // DAT_004df470 非空
    bool enabled = true;      // DAT_004c12c1：配置 乐曲开
    // 串流器淡出状态（DAT_004df470 +0x1c 模式 / +0x14 剩余帧 / +0x18 总帧）：命令 5 置模式 1，0x43dcf0 每帧推进
    i32 fade_mode = 0;
    i32 fade_remaining = 0;
    i32 fade_total = 0;
    i32 volume = 100;         // DAT_004df49c：配置 乐曲音量（0..100）
    MusicOutput* output = nullptr;

    bool load_table(const u8* data, size_t size);   // 0x43adc0：thbgm.fmt
    void enqueue(i32 kind, i32 track, const char* name);  // 0x45ed00
    void register_track(i32 slot, const char* name);      // 0x43c370(槽, 名)：命令 1 "<名>.wav"
    void play_slot(i32 slot);                             // 0x43c3f0 的命令部分：命令 2(槽, "dummy")
    void stop_track();                                    // 0x43c440：命令 3(0, "dummy")
    void fade_out(float seconds, float game_speed);       // 0x43c470：秒 ÷ 游戏速度（0 < speed ≤ 1）→ 命令 5
    // 0x45e330 前段：推进队头一步，返回处理后的队头 kind（0 = 队列空）
    i32 step();
    i32 head_kind() const { return queue.empty() ? 0 : queue.front().kind; }
    i32 find_track(const std::string& name) const;        // 0x45da30：去目录后按名查表；未命中回 0（首曲）
    const BgmEntry& entry(i32 index) const;               // 越界给零条目
    // 0x43dcf0（0x43b3d0 帧首紧随 0x45e330）：模式 1 每帧 音量 = 剩余 × 5000 / 总 − 5000，归零时 Stop
    void tick_fade();
    i32 ds_volume(i32 base = 0) const;                    // 0x4711f0 音量曲线：(1 − (1 − m/100)²) × (base + 5000) − 5000；m = 0 → -10000
};

}  // namespace th16
