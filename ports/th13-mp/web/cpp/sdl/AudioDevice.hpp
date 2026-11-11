#pragma once
#include "../game/Types.hpp"
#include <memory>
#include <string>
#include <functional>

namespace th13::sdl {
// TH13 owns decoding and frame-channel sequencing. Browser device SFX use the
// shared immediate output; native/no-device diagnostics retain the PCM mixer.
// Diagnostic mode has no device, but still requires all original assets.
class AudioDevice {
    struct Impl;
    std::unique_ptr<Impl> impl;
public:
    struct RequestObservation {
        enum class Kind { Voice, Stop, Pan, Music } kind;
        i32 id = 0, value = 0;
        bool positional = false;
        float x = 0;
        std::string text;
    };
    // Read-only request tap. The caller supplies its own frame/sequence; the
    // observation is emitted before validation and does not alter the queue.
    std::function<void(const RequestObservation&)> request_observer;
    struct Statistics {
        u64 mixed_frames = 0, nonzero_samples = 0, requests = 0, started = 0;
        u64 music_loops = 0, submitted_frames = 0, dropped_requests = 0;
        u64 stopped = 0;
        u32 music_cursor = 0, queued_frames = 0, sample_rate = 44100;
        u32 active_voices = 0, pending_voices = 0;
        i32 track = -1;
        bool ready = false, device = false, suspended = false, music_paused = false;
    };
    std::string error;
    i32 music_volume = 100;
    bool music_enabled = true;
    // P3.1：确认后冲账模式（只由 MP 会话开启；单人/SP 路径恒 false）。
    // true 时 enqueue/stop/queue_music/trance_music 只发射 request_observer
    // 并做同步校验，不写 12 通道帧表、不触音乐命令队列（听感不变——宿主在
    // 该帧 commit 段经 apply_request 冲账，见 Session 的确认账本）。
    bool defer_output = false;
    // Global SE volume = original DAT_004e4770. 0x44ed70 writes it from the
    // config byte DAT_004dc893 every time the options change (0x44dd00), so it
    // is live state, not a startup constant. 0x4613c0 reads it at each voice
    // start and fixes that voice's attenuation; already-playing voices are not
    // retroactively changed (the original only calls the buffer volume setter
    // from 0x4613c0). Curve: v==0 mutes; otherwise
    //   atten = trunc((1-(1-v/100)^3)*(def+5000)) - 5000
    // with f32 stores at x, 1-x, (1-x)^3 and 1-(1-x)^3, then the 0x4971f0
    // truncation. Default 80 = original default config word 0x5064 high byte.
    i32 effect_volume = 80;
    AudioDevice();
    ~AudioDevice();
    bool initialize(bool device = true);
    bool enqueue(i32 id);
    // 0x460c50: same 12-channel table as enqueue(id), but every request stores
    // trunc(pan*1000/192) (milli-pan). Call sites pass the original centered
    // field x coordinate: 0 = center, and the 384-wide field spans -192..+192,
    // so the stored value is +-1000. 0x460dd0 averages the entries with signed
    // integer division and stores the average in voice+0x10 for the buffer
    // setter. A 0..384 screen coordinate must be recentered by the caller.
    bool enqueue(i32 id, float pan);
    // 0x460d40: id >= 0 queues a stop for that voice ID (negative count in the
    // 12-slot frame channel table); id < 0 stops every original voice object.
    bool stop(i32 id);
    // 0x446174/0x4461aa directly update the existing buffer pan without
    // restarting the voice or changing the queued start-time pan.
    bool set_voice_pan(i32 id, float pan);
    // M1/T4 F3 pan 两入口拆分（set_voice_pan = 两者合成，语义不变）：
    // observe_voice_pan = 捕获点入口：发射 request_observer(Kind::Pan)（观察
    // 先于校验，同原 :336 先于 :337-343 次序）+ 同步校验（校验条件与 error
    // 文案逐字同原实现，失败即返回 false），不写 buffer_pan、不触设备、
    // 不动 statistics。等待确认事件队列在捕获拍调用。
    bool observe_voice_pan(i32 id, float pan);
    // apply_voice_pan = commit 冲账入口：buffer_pan + web_sfx::pan 设备触达，
    // 无观察器发射（Pan tap 恰一次落在捕获拍）、无校验副作用（校验已在
    // 捕获拍同步完成；越界 id 静默忽略，同 voice_pan() 只读访问器惯例）。
    void apply_voice_pan(i32 id, float pan);
    // P3.1 冲账入口（M1 Pan 拆分的同构推广）：按 kind 分派到真正的设备触达
    // （Voice→queue_voice，entry 由 observation.x/positional 重算；Stop→stop
    // 体；Music→queue_music 体；Pan→apply_voice_pan）。不发射 request_observer
    //（恰一次落在捕获拍）。返回 false = 设备拒绝（error set-if-empty 语义
    // 与对应公开入口一致）。仅 defer_output 模式使用；单人路径不调用。
    bool apply_request(const RequestObservation& request);
#ifdef TH13_AUDIO_MIXER_ONLY
    // Loads the original SE table from an explicit directory for PCM-level
    // diagnostics. It intentionally has no music stream or output device.
    bool initialize_effects_for_test(const std::string& asset_root);
    // Effects + thbgm.fmt from asset_root; music_path defaults to
    // asset_root/thbgm.ogg. Used only by PCM diagnostics with real assets.
    bool initialize_for_test(const std::string& asset_root,
                             const std::string& music_path = "");
#endif
    bool queue_music(i32 code, i32 value, const std::string& text = "");
    bool trance_music(bool active);
    bool finish_music_requests();
    void pause_music(bool);
    void suspend(bool);
    void update();
    void pump();
    void close();
    bool mix(float* stereo, u32 frames);
    bool seek_music_frames(u32 frame);
    Statistics statistics() const;
    // 当前选中曲目文件名（thbgm.fmt 布局行；无曲目 = 空）。0x4e13c0 对应物：
    // 0043f570 疮痍快照 / 00440772 续关恢复的曲名源。defer 模式下读已应用
    // 态（impl->track 只在冲账拍变更），与听感一致。
    std::string music_filename() const;
    // Original voice+0x10 (averaged milli-pan from 0x460dd0) and the
    // attenuation fixed by 0x4613c0 at this ID's last start. Diagnostics for
    // PCM tests and traces; invalid IDs return 0.
    i32 voice_pan(i32 id) const;
    i32 voice_attenuation(i32 id) const;
private:
    bool queue_voice(i32 id, i32 entry);
    // P3.1 defer_output 捕获拍的同步校验（不写通道表/不作统计，error 文案
    // 与 queue_voice/stop/queue_music 的原实现逐字一致）。
    bool validate_voice(i32 id);
    bool validate_stop(i32 id);
    bool validate_music(i32 code, const std::string& text);
    // 无观察器发射的设备触达体（公开入口与 apply_request 共用；单人路径
    // 公开入口=观察器 + 本函数，逐字节等价原实现）。
    bool apply_stop(i32 id);
    bool apply_music(i32 code, i32 value, const std::string& text);
};
}
