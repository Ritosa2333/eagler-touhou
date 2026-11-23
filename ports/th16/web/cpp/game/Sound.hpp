// th16 音效——原版 SoundManager DAT_004d9e10 的音效部分（0x45d510 初始化，DirectSound 缓冲由宿主实现）。
//
//   请求队列（主线程登记，帧尾 0x45e330 消费）：12 个槽，每槽一个声部号 DAT_004d9e2c[i]、计数 DAT_004d9e5c[i]
//   与最多 60 个声像 DAT_004d9e8c[i][0x80]（步长 0x200）。同一帧对同一声部的多次请求合并为一次播放，声像取平均。
//   0x45e150  播放（声像 0）              0x45e1f0  按 x 定位播放（声像 = x × 1000 / 192，单精度截断）
//   0x45e2a0  停止（id < 0：立即停全部并记下是否在播）
//   0x45e330  尾段（DAT_004c12c2 音效开时）：逐槽取出，计数 < 0 → 停止并记下是否在播；否则 0x45e8d0 重播
//   0x45e8d0  声部重播：Stop → SetCurrentPosition(0) → SetPan → SetVolume(音量曲线) → Play(定义 +0xc 标志)
//   0x42e4d0  暂停：队列槽 0 清空，全部声部停下并记下是否在播     0x440bd0  恢复：记下在播的声部重新 Play
#pragma once
#include <array>
#include <vector>

#include "Types.hpp"

namespace th16 {

constexpr int kSoundVoiceCount = 78;     // DAT_004db894 声部表（0x18 字节 × 0x4e）
constexpr int kSoundSampleCount = 67;    // DAT_004dbfe4 采样缓冲
constexpr int kSoundQueueSlots = 12;
constexpr int kSoundQueueDepth = 60;     // 0x45e150：计数 > 0x3b 不再追加声像
constexpr int kSoundQueueStride = 0x80;  // DAT_004d9e8c 每槽 0x80 个 dword

// DAT_004a2a30 一行（0x14 字节）
struct SoundDefinition {
    i32 voice;
    i32 sample;
    i16 volume;   // 百分之一 dB（DirectSound SetVolume 单位）
    i16 renew;    // 0x45e150 抄进声部 +4
    u32 flags;    // Play 第三参（1 = DSBPLAY_LOOPING）
    u32 reserved;
};

extern const std::array<SoundDefinition, kSoundVoiceCount> kSoundDefinitions;
extern const std::array<const char*, kSoundSampleCount> kSoundSamples;

// 宿主侧的 IDirectSoundBuffer 族（按声部号寻址；available = 声部缓冲存在）
struct SoundOutput {
    virtual ~SoundOutput() = default;
    virtual bool available(i32 voice) const = 0;
    virtual bool playing(i32 voice) = 0;                 // +0x24 GetStatus & DSBSTATUS_PLAYING
    virtual void stop(i32 voice) = 0;                    // +0x48
    virtual void set_position(i32 voice, u32 byte) = 0;  // +0x34
    virtual void set_pan(i32 voice, i32 pan) = 0;        // +0x40
    virtual void set_volume(i32 voice, i32 volume) = 0;  // +0x3c
    virtual void play(i32 voice, u32 flags) = 0;         // +0x30
};

class SoundEffects;

// 本帧音效请求（0x45e150 / 0x45e1f0 调用序）：ids 供对拍，pans 为 0x45e1f0 声像（0x45e150 等同 x = 0）；
// 挂上 effects 后每条请求按原版调用点即时进 SoundEffects 队列（0x45e2a0 停止亦然，不计入 ids）
struct SoundRequests {
    std::vector<i32> ids;
    std::vector<i32> pans;
    SoundEffects* effects = nullptr;
    void push(i32 id, float x = 0.0f);
    void stop(i32 id);
    void clear() { ids.clear(); pans.clear(); }
};

class SoundEffects {
public:
    struct Voice {
        i32 renew = -1;                            // +0x04
        const SoundDefinition* definition = nullptr;  // +0x08：声部号匹配的定义行
        i32 pan = 0;                               // +0x10
        bool resume = false;                       // +0x14：暂停时仍在播
    };

    std::array<i32, kSoundQueueSlots> queue_voice{};
    std::array<i32, kSoundQueueSlots> queue_count{};
    std::array<std::array<i32, kSoundQueueStride>, kSoundQueueSlots> queue_pan{};
    std::array<Voice, kSoundVoiceCount> voices{};
    bool enabled = true;     // DAT_004c12c2：配置 音效开
    i32 master_volume = 100; // DAT_004df4a0：配置 音效音量（0..100）
    SoundOutput* output = nullptr;

    SoundEffects() { reset(); }
    void reset();                      // 0x45d510：队列置空、声部绑定定义行
    void play(i32 voice);              // 0x45e150
    void play_at(i32 voice, float x);  // 0x45e1f0
    void play_pan(i32 voice, i32 pan) { enqueue(voice, pan); }
    void stop(i32 voice);              // 0x45e2a0
    void process();                    // 0x45e330 尾段
    void suspend();                    // 0x42e4d0
    void resume();                     // 0x440bd0
    static i32 pan_for_x(float x);     // 0x45e1f0：x × 1000 / 192
    static i32 voice_volume(i16 volume, i32 master);  // 0x45e8d0 音量曲线

private:
    void enqueue(i32 voice, i32 pan);
    void halt(i32 voice);              // GetStatus → resume、Stop
    void restart(i32 voice, i32 pan);  // 0x45e8d0
};

}  // namespace th16
