// th16 宿主音频：原版 DirectSound 的两套缓冲由 miniaudio（无设备模式混音）+ SDL3 音频流实现。
//   音效声部（DAT_004db894 ×0x4e，SoundOutput）：每个声部一个 se_*.wav 解码源（th16.dat 内）→ ma_sound；
//     SoundEffects::restart 的 Stop / SetCurrentPosition / SetPan / SetVolume / Play 逐一映射。
//   乐曲串流器（DAT_004df470，MusicOutput）：thbgm.dat 按 thbgm.fmt 条目直接读 PCM（自定义 ma_data_source，
//     循环点 = 条目 +0x18 / +0x1c）；无 thbgm.dat 时退回 <music_directory>/<曲名>.ogg（帧数须与原版 PCM 一致）。
//   音量：SoundEffects::voice_volume / MusicManager::ds_volume 给出 DirectSound 百分之一 dB，这里换算为线性增益。
#pragma once
#include "../game/Archive.hpp"
#include "../game/Music.hpp"
#include "../game/Sound.hpp"

#include <memory>
#include <string>

namespace th16::sdl {

class AudioDevice {
public:
    std::string error;
    std::string thbgm_path;                // thbgm.dat（与 th16.dat 同目录；空或不存在 → OGG 退路）
    std::string music_directory = "/music";
    const MusicManager* music = nullptr;   // ds_volume 曲线（DAT_004df49c）

    AudioDevice();
    ~AudioDevice();

    // 解码全部 se_*.wav 并建混音器；device = true 时另开 SDL3 播放流（Emscripten / TH16_HAS_SDL）
    bool initialize(Archive& archive, bool device = true);
    SoundOutput& sfx_output();
    MusicOutput& music_output();

    void pump();                 // 每逻辑帧末：补足 SDL 流队列
    void suspend(bool value);    // 页面失焦等：暂停/恢复 SDL 流
    bool mix(float* stereo, u32 frames);   // 直接取混音结果（测试与 pump 共用）
    bool ready() const;
    // 观测文本："ready= stream= queued=<帧> mixed=<帧> mix_fail= music=<open>/<playing>/<游标> cur=<当前曲> q=<命令数> err="
    std::string status() const;
    bool music_open() const;     // 串流器已绑定曲目
    u64 music_cursor() const;    // 当前曲目 PCM 帧游标
    bool music_playing() const;
    // 宿主健康上报：SDL 流当前排队帧数；自上次读取以来的最低排队帧数（读后归位）
    u32 queued_frames() const;
    u32 take_min_queued_frames();
    // 启动器“音乐关闭”：串流器仍按原版推进，只把增益压为 0（不影响逻辑 / 命令队列）
    void set_music_enabled(bool on);
    bool music_enabled() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    void open_stream(const BgmEntry& entry);   // 0x470bb0
    void apply_music_volume(i32 base);         // 0x4711f0
};

}  // namespace th16::sdl
