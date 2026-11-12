// TH13 MP 会话驱动（M7：纯延迟）。桥接 RunScene 帧循环与 NetplayRuntime：
//   - 本地输入采集（每游戏帧一次）→ 延迟窗；
//   - 帧推进门：全员实际输入确认（Prepare canAdvance）才放行 RunScene
//     update——纯延迟停等（无预测帧）；
//   - 输入供给：本地席位走 Runtime 延迟窗（playback_input 接缝——与回放
//     同款），远端席位每帧从 ConfirmedInputs 注入 seat_inputs；
//   - 帧号栅栏：sim_frame 单调推进；MarkSimulated 落账。
// M8 起扩展：captureFrame/confirmed/commit 分离 + 回滚（WorldState 快照）。
#pragma once

#include "InputLanes.hpp"
#include "NetplayRuntime.hpp"
#include "WorldState.hpp"

#include "../game/GameInput.hpp"
#include "../sdl/AudioDevice.hpp"  // th13::sdl::th13::sdl::AudioDevice::RequestObservation

#include <array>
#include <cstdint>
#include <deque>  // applied_audio_ 观测面（P4：records_ 已环化；deque 保留给调试日志）
#include <functional>
#include <vector>

namespace th13 {
class StageBattle;  // game/StageBattle.hpp（Session.cpp 内含）
class RunScene;     // game/RunScene.hpp（P3.2 宿主域：震动/对话/STD 快照面）
}

namespace th13::multiplayer {

class Session {
public:
    // P3.1：帧号 + 音频观察。audio_out_/voice_out_/applied_audio_ 共用。
    // confirmed = 宿主冲账时刻的确认前沿（只读日志面；Voice 首播可 < frame，
    // Stop/Pan/Music 要求 >= frame——「不早于确认帧」的浏览器断言依据）。
    struct AudioRequest {
        std::uint32_t frame = 0;
        th13::sdl::AudioDevice::RequestObservation observation;
        std::uint32_t confirmed = 0;
    };
private:
    // th13::StageBattle（非本 namespace）——BindWorld 的世界接线指针。
    // ---- M8：帧记录（预测帧 checkpoint + 输入 + 音频请求汇）----
    // 回滚语义：预测帧（predictedMask != 0）在模拟前 capture WorldState；
    // 确认输入到达且与预测不一致 → RollbackCore 请求回滚 → restore 最旧
    // 错误帧的 checkpoint → 丢弃其后 records → 同输入重放（预算 13 帧
    // = 12 回滚窗 + 1 新进度）。
    // P3.1 音频恰播一次：Voice 首播同帧出声（回滚重放按 replayed_audio_
    // 账本吞掉重发）；Stop/Pan/Music 确认后经 audio_out_ 冲账。
    struct Record {
        std::uint32_t frame = 0;
        std::uint32_t stage = 1;
        bool has_snapshot = false;
        WorldState snapshot;
        // P3.4：线上 digest 采样（仅预测帧的 60 倍帧号；Commit 退役时
        // 经 NetplayRuntime::SendDigestControl 发 T13D 控制包）。
        std::uint64_t wire_digest = 0;
        bool has_wire_digest = false;
        std::array<Netplay::FrameInput, 3> inputs{};
        // 本地席输入沿快照（checkpoint 帧）：回滚 restore 后 local_input_
        // 的 pressed/duration 历史必须回到 checkpoint 时刻——沿状态机
        // 从错误预测期直接续推会令重放帧的边沿与连续基线不同（M8 实证）。
        GameInput local_input{};
        std::array<GameInput, 3> frame_inputs{};
        std::array<char, 2048> domains{};
        std::string detail;
        std::uint64_t full_digest = 0;
        std::uint64_t host_digest = 0;
        std::string host_detail;
        bool audited = false;
        std::vector<th13::sdl::AudioDevice::RequestObservation> audio;
        std::function<bool()> restore_host, confirmed_output;
    };
    // 未确认帧记录（checkpoint/输入/音频汇；确认前沿退役）。
    // P4：16 槽固定环形环（12 回滚窗 + 余量；远端落后 >12 帧即停等，
    // 活跃 record 数被窗天然限界——环满即停等是防御断言，理论不可达）。
    // Record 的 vector 成员（snapshot 内域/audio）在槽复用时保留容量，
    // 稳态零堆分配。头尾语义：head_ = 最旧（front），tail_ = 下一写入位。
    static constexpr std::size_t kRecordSlots = 16;
    std::array<Record, kRecordSlots> record_slots_{};
    std::size_t record_head_ = 0, record_count_ = 0;
    bool record_overflow_ = false;
    // ---- 环访问面（Session.cpp 内使用；语义与原 deque 一致）----
    std::size_t record_tail() const { return (record_head_ + record_count_) % kRecordSlots; }
    Record& record_front() { return record_slots_[record_head_]; }
    const Record& record_front() const { return record_slots_[record_head_]; }
    Record& record_back() { return record_slots_[(record_tail() + kRecordSlots - 1) % kRecordSlots]; }
    Record& record_at(std::size_t logical) {
        return record_slots_[(record_head_ + logical) % kRecordSlots];
    }
    const Record& record_at(std::size_t logical) const {
        return record_slots_[(record_head_ + logical) % kRecordSlots];
    }
    // 尾部压入（复用槽位的 vector 容量；audio 先清空保序）。
    Record& record_push(Record&& source);
    void record_pop_front();
    void record_clear();
    // 确认后冲账队列（非 Voice：Stop/Pan/Music，只来自已确认退役的 Record）；
    // 宿主在 commit 段取走并喂 AudioDevice::apply_request。非模拟期请求不进
    // 本队列（宿主观察器直接 apply_request，同单人立即触达）。
    std::vector<AudioRequest> audio_out_;
    // 首播 Voice 队列（P3.1）：预测帧的 SFX 首次即真响——宿主在同帧 commit
    // 段冲账，不等待确认；回滚重放若命中 replayed_audio_ 账本则不入队。
    std::vector<AudioRequest> voice_out_;
    // 回滚重放账本：被 ExecuteRollback 丢弃段的 Voice 请求按帧号取模 16
    // 入环（附帧号标签防 16 帧环回冲突）；重模拟帧的同类同 id Voice 逐条
    // 消费并吞掉（首播已响过）。对应帧 commit 退役时清空。
    static constexpr std::size_t kReplayedSlots = 16;
    static constexpr std::uint32_t kNoLedgerFrame = 0xFFFFFFFFu;
    std::array<std::vector<th13::sdl::AudioDevice::RequestObservation>, kReplayedSlots>
        replayed_audio_{};
    std::array<std::uint32_t, kReplayedSlots> replayed_frames_{};
    // 宿主冲账后的只读日志（浏览器调试面；容量有界，覆盖最旧）。
    static constexpr std::size_t kAppliedLogCapacity = 4096;
    std::deque<AudioRequest> applied_audio_;
    std::uint64_t sfx_suppressed_ = 0;   // 去重吞掉的 Voice 数
    std::uint64_t music_deferred_ = 0;   // 推迟到确认拍的 Music 数
    std::uint64_t music_dropped_ = 0;    // 随丢弃 Record 消失的 Music 数
    std::uint64_t voice_captured_ = 0, music_captured_ = 0;
    std::uint64_t voice_applied_ = 0, music_applied_ = 0;
    std::uint32_t committed_ = 0, restores_ = 0, resimulated_ = 0, checkpoints_ = 0;
    std::uint32_t max_rollback_distance_ = 0;
    std::uint32_t last_rollback_distance_ = 0;
    std::uint32_t resimulated_during_correction_ = 0;

public:
    // 配置（坏身份在 DecodeSessionSetup 拒绝——frame0 前）。
    bool Configure(const std::uint32_t* words, std::size_t count);
    void Stop();
    void Stop(bool keep_terminal_transport);
    bool PumpTerminal(std::uint64_t now_ms) { return runtime_.PumpTerminal(now_ms); }
    // R-G: after the last confirmed battle frame, ending/results use local input.
    bool FinishWorldSimulation(bool completed = false);
    bool WorldCompleted() const { return world_completed_; }

    // relay 连接（RTC/relay 竞速在 transport 内收敛）。
    bool Connect(const char* relay_url);
    bool Connected() const { return runtime_.Connected(); }
    // 观众连接（M9/P6：公共观战——只收流；世界驱动由宿主经
    // SpectatorStep 消费 playback 历史）。
    bool ConnectSpectator(const char* relay_url, const char* spectator_id) {
        if (!active_) return false;
        return runtime_.ConnectSpectator(relay_url, spectator_id);
    }
    bool Spectator() const { return runtime_.Spectator(); }
    NetplayRuntime& Runtime() { return runtime_; }  // 观战导出面转发

    // 帧泵（每浏览器帧）：信道收发 + HELLO/READY 门推进。
    bool Pump(std::uint64_t now_ms);

    // frame0 屏障：全员 READY 才允许世界启动。CanStart 后宿主调 Start
    //（开始采集 + 放行模拟——simulating_ 置位，Pump 的 world_ready 面）。
    bool CanStart() const { return runtime_.CanStart(); }
    void MarkLocalReady() { runtime_.MarkLocalReady(); }
    bool Start() {
        if (!runtime_.CanStart() || simulating_) return simulating_;
        simulating_ = true;
        // 采集相位启动：next_capture_ 保持 0——首个 capture 帧号 = 0
        //（capture c → 生效帧 c + input_delay；Start 不消耗采集相位。
        // 此处曾误用 AdvanceCapturePhase 令 capture 0 永久缺失，delay 窗
        // 后首帧起对端停等——M8 browser 实证修正）。
        return true;
    }
    // 直驱模拟启动（native 测试/本地回放面）：绕过 frame0 屏障的
    // transport 门（无信道会话的 RollbackCore 输入面驱动）。
    // 生产联机会话仍走 Start（gate 拒绝时返回假）。
    bool BeginLocalSimulation() {
        if (!active_ || simulating_) return simulating_;
        simulating_ = true;
        runtime_.SetLocalSimulation(true);  // correction 的 gate 替代门
        return true;
    }

    // 本地输入采集（每游戏帧恰一次；暂停/对话期也采集——输入沿语义
    // 与回放一致：EOF/暂停帧不消耗游戏帧但采集相位照常推进）。
    // 采集门 = simulating_（Start 后）；capture 相位 0 是合法首帧号。
    bool CaptureLocal(std::uint32_t held, std::uint64_t now_ms,
                      StagePlayerState::TouchMotion touch = {});
    void AdvanceCapturePhase() { runtime_.AdvanceCapturePhase(); }
    std::uint32_t CaptureFrame() const { return runtime_.CaptureFrame(); }

    // P3.3：确认帧门（MSG 对话/换关过渡期由宿主每帧置位）。门开时只放行
    // 纯确认帧（predictedMask == 0）：预测帧本帧停等确认输入，不建
    // Record/checkpoint——回滚快照域不跨越 MSG 暂停/换关 bank 卸载点。
    // 已放行的帧经 CanSimulate 的 frame_granted_ 幂等门直接返回（门不撤
    // 单帧）。Configure/Stop 清零。
    void SetPredictionBarrier(bool active) { prediction_barrier_ = active; }

    // 帧推进门：sim_frame 的全员输入已确认 → true（宿主此刻泵 RunScene
    // update；随后 OnFrameSimulated 落账）。远端席位输入由本调用注入
    // seat_inputs（battle 引用经 Begin 注入）。
    bool CanSimulate();
    void OnFrameSimulated();
    // 远端席位输入提交（native 直驱面；生产信道走 SessionChannel——
    // 同一下游 RollbackCore::SubmitRemoteInput）。
    Netplay::RemoteInputResult SubmitRemote(unsigned seat, std::uint32_t frame,
                                             const Netplay::FrameInput& input) {
        return runtime_.SubmitRemote(seat, frame, input);
    }

    // 本地席位的延迟输入（RunScene playback_input 接缝的供给源）。
    // 返回 nullptr = 本帧无输入（停等——宿主跳过 RunScene update）。
    const GameInput* LocalDelayedInput();
    const GameInput& MenuInput() const { return menu_input_; }
    std::function<std::function<bool()>()> capture_host;
    std::function<std::uint64_t()> digest_host;
    std::function<std::string()> diagnostic_host;
    void SetConfirmedOutput(std::function<bool()> output) { record_back().confirmed_output = std::move(output); }
    bool AllFramesCommitted() const { return record_count_ == 0 && !Correcting(); }
    bool PredictionBarrier() const { return prediction_barrier_; }
    void SetAudit(bool enabled) {
        audit_enabled_ = enabled; audit_rows_.clear(); audit_overflow_ = 0;
        terminal_audit_.clear();
        timing_head_ = timing_count_ = 0; timing_overflow_ = 0;
        for (std::size_t i = 0; i < record_count_; ++i) record_at(i).audited = false;
    }
    std::string TakeAuditJson();
    // Optional bounded observations; timestamps never feed the game clock.
    void TraceTiming(const char* kind, std::uint32_t frame,
                     std::uint32_t value = 0, std::uint32_t detail = 0);
    std::string TakeTimingJson();

    // 世界接线（begin_run 后由宿主调用一次）——battle + visual RNG +
    // presentation（WorldState capture/restore 的完整面）。P3.2：scene
    //（RunScene 宿主域：震动/对话/STD）与 transition（RunTransitionEffect
    // 句柄的读写面——capture 取当前值、restore 回写）一并接线。
    void BindWorld(th13::StageBattle* battle, GameRandom* visual,
                   StagePresentation* presentation, th13::RunScene* scene = nullptr,
                   u32* transition_handle = nullptr);
    // Configure 后的补接线：宿主 begin_run 先于 session.Configure 的时序
    //（debug 页直入对局 → handshake 里 configure）下，BindWorld 的
    // session.Active() 门未过。Configure 成功后宿主重试接线（run 已存在
    // 即有效；无 run = 静默）。
    void TryBindWorld(th13::StageBattle* battle, GameRandom* visual,
                      StagePresentation* presentation, th13::RunScene* scene = nullptr,
                      u32* transition_handle = nullptr) {
        if (!active_ || !battle) return;
        BindWorld(battle, visual, presentation, scene, transition_handle);
    }

    // ---- M8：回滚驱动面 ----
    // 每帧音频观察器接线（宿主在 audio_device.request_observer 转发到
    // 此——预测帧的音频请求随 Record 走；回滚丢弃未 commit 帧的请求）。
    // P3.1 恰播一次账本：
    //   - Voice：记入 Record 后进 voice_out_（首播同帧冲账）；若命中
    //     replayed_audio_ 的未消费同类同 id 条目则吞掉（回滚重放）。
    //   - Music：记入 Record（确认后经 audio_out_ 冲账），music_deferred_++。
    //   - Stop/Pan：记入 Record（确认后冲账）。
    //   - 非模拟期（pending_audio_ 空）：保持 M8 契约——不记录（返回）。
    //     宿主观察器在调用本函数前以 RecordingAudio() 分流：非模拟期请求
    //     直接走 AudioDevice::apply_request（同单人立即触达，defer 不吞
    //     菜单/标题/关界音频）。
    void ObserveAudio(const th13::sdl::AudioDevice::RequestObservation& observation) {
        using Kind = th13::sdl::AudioDevice::RequestObservation::Kind;
        if (!pending_audio_) return;
        pending_audio_->push_back(observation);
        if (observation.kind == Kind::Voice) {
            ++voice_captured_;
            if (!ConsumeReplayedVoice(observation))
                voice_out_.push_back({pending_audio_frame_, observation});
        } else if (observation.kind == Kind::Music) {
            ++music_captured_;
            ++music_deferred_;
        }
    }
    // 当前是否处于放行的模拟帧（宿主观察器的分流门）。
    bool RecordingAudio() const { return pending_audio_ != nullptr; }
    // commit 边界出队的音频（宿主每帧取走并喂 audio_device——恰播一次）。
    // P3.1 起 audio_out_ 只承载「确认后/非帧」类请求（Voice 走 VoiceOut 的
    // 首播同帧冲账）；条目带来源帧号（调试分组与「不早于确认帧」断言）。
    const std::vector<AudioRequest>& AudioOut() const { return audio_out_; }
    const std::vector<AudioRequest>& VoiceOut() const { return voice_out_; }
    void ConsumeAudioOut(std::size_t count) {
        if (count >= audio_out_.size()) audio_out_.clear();
        else audio_out_.erase(audio_out_.begin(), audio_out_.begin() + count);
    }
    void ConsumeVoiceOut(std::size_t count) {
        if (count >= voice_out_.size()) voice_out_.clear();
        else voice_out_.erase(voice_out_.begin(), voice_out_.begin() + count);
    }
    void ClearAudioOut() { audio_out_.clear(); }
    // 宿主冲账拍回填（apply_request 成功后调用；只读日志 + 守恒计数）。
    void NoteApplied(std::uint32_t frame,
                     const th13::sdl::AudioDevice::RequestObservation& observation) {
        using Kind = th13::sdl::AudioDevice::RequestObservation::Kind;
        if (observation.kind == Kind::Voice) ++voice_applied_;
        if (observation.kind == Kind::Music) ++music_applied_;
        if (applied_audio_.size() >= kAppliedLogCapacity) applied_audio_.pop_front();
        AudioRequest entry;
        entry.frame = frame;
        entry.observation = observation;
        entry.confirmed = runtime_.ConfirmedThroughAll();
        applied_audio_.push_back(entry);
    }
    const std::deque<AudioRequest>& AppliedAudio() const { return applied_audio_; }
    void ClearAppliedAudio() { applied_audio_.clear(); }
    // P3.1 统计（th13_debug_mp_net_rollback JSON 验收面）。
    std::uint64_t SfxSuppressed() const { return sfx_suppressed_; }
    std::uint64_t MusicDeferred() const { return music_deferred_; }
    std::uint64_t MusicDropped() const { return music_dropped_; }
    std::uint64_t VoiceCaptured() const { return voice_captured_; }
    std::uint64_t MusicCaptured() const { return music_captured_; }
    std::uint64_t VoiceApplied() const { return voice_applied_; }
    std::uint64_t MusicApplied() const { return music_applied_; }
    // 回滚统计（验收断言面：restores > 0 即真实回滚发生）。
    std::uint32_t Restores() const { return restores_; }
    std::uint32_t Resimulated() const { return resimulated_; }
    std::uint32_t Checkpoints() const { return checkpoints_; }
    std::uint32_t Committed() const { return committed_; }
    // 历史最大纠正距离（ExecuteRollback 时 sim_frame_ − target 的最大值；
    // 验收面：12 帧窗场景必须观测到 ≥12）。
    std::uint32_t MaxRollbackDistance() const { return max_rollback_distance_; }
    // P5：最近一次 ExecuteRollback 的纠正距离（生产 rollback_status[4]）。
    // 与 MaxRollbackDistance 分开：桥的 lastDepth 语义是最近一次，不是峰值。
    std::uint32_t LastRollbackDistance() const { return last_rollback_distance_; }

    // ---- P5：生产 status/rollback_status 导出的只读转发面 ----
    // 不新增统计语义：信道计数/路径/纠正终点全部来自现有 getter。
    std::uint32_t PacketsSent() const { return runtime_.PacketsSent(); }
    std::uint32_t PacketsReceived() const { return runtime_.PacketsReceived(); }
    std::uint32_t RepairsSent() const { return runtime_.RepairsSent(); }
    std::uint32_t PacketsIgnored() const { return runtime_.PacketsIgnored(); }
    bool TransportOpen() const { return runtime_.TransportOpen(); }
    std::uint32_t RouteCode() const { return runtime_.RouteCode(); }
    // 纠正终点（INVALID_FRAME = 未纠正；桥 rollback_status[7] barrier）。
    std::uint32_t CorrectionEnd() const { return runtime_.CorrectionEnd(); }

    // ---- P0 测量（只读统计；不改变任何控制流）----
    // 计时源 = std::chrono::steady_clock，单位微秒（整数截断）。
    // capture_us：CanSimulate 内 WorldState::capture 的耗时（max/sum/次数）。
    std::uint64_t CaptureUsMax() const { return capture_us_max_; }
    std::uint64_t CaptureUsSum() const { return capture_us_sum_; }
    std::uint64_t CaptureUsCount() const { return capture_us_count_; }
    // restore_us：ExecuteRollback 内 WorldState::restore 的耗时（max/sum）。
    std::uint64_t RestoreUsMax() const { return restore_us_max_; }
    std::uint64_t RestoreUsSum() const { return restore_us_sum_; }
    // update_us：CanSimulate 放行（frame_granted_）到 OnFrameSimulated
    // 落账之间的耗时（max/sum/次数）——近似一帧模拟耗时。
    std::uint64_t UpdateUsMax() const { return update_us_max_; }
    std::uint64_t UpdateUsSum() const { return update_us_sum_; }
    std::uint64_t UpdateUsCount() const { return update_us_count_; }
    // resim_us：纠正期间（runtime_.Correcting()）上述 update 时间的累计。
    std::uint64_t ResimUsSum() const { return resim_us_sum_; }
    // 每次回滚距离（sim_frame_ − target）的累计。
    std::uint64_t RollbackDistanceSum() const { return rollback_distance_sum_; }
    // CanSimulate 因 !decision.canAdvance 返回 false 的次数。
    std::uint64_t StallFrames() const { return stall_frames_; }

    std::size_t RecordCount() const { return record_count_; }
    // P4：环越界观测（防御断言面——理论不可达；真值出现即预测窗配置错误）。
    bool RecordOverflow() const { return record_overflow_; }
    // 最近一次 ExecuteRollback 的失败标记（checkpoint 缺失/恢复失败——
    // 观测面用；失败原因在 Pump 返回假时由 error 面补充）。
    bool LastRollbackFailed() const { return rollback_failed_; }
    // 回滚请求处理：Pump 检测 RollbackCore 请求 → ExecuteRollback（restore
    // + 后续 records 丢弃）；宿主在 CanSimulate 假时持续 Pump，恢复后
    // 重放预算 13 帧（12 回滚窗 + 1 新进度）。
    bool HasRollbackRequest() const { return rollback_pending_; }
    // 当前帧是否预测帧（predictedMask != 0——checkpoint 条件）。
    bool PredictedFrame() const;

    // 观测面（测试/诊断）。
    std::uint32_t SimFrame() const { return sim_frame_; }
    // P2：时间同步/纠正态转发（生产帧循环消费；只读）。
    double IntervalScale() const { return runtime_.IntervalScale(); }
    bool Correcting() const { return runtime_.Correcting(); }
    std::uint32_t ConfirmedThrough() const { return runtime_.ConfirmedThroughAll(); }
    std::uint8_t LocalSeat() const { return runtime_.LocalSeat(); }
    std::uint32_t PlayerCount() const { return runtime_.Setup().player_count; }
    // M9：会话身份（ReplayArchive/SpectatorJournal 的录制面——Active 时
    // 归档身份 = 会话身份）。
    const SessionSetup& Setup() const { return runtime_.Setup(); }
    const char* Error() const { return runtime_.Error(); }
    bool Active() const { return active_; }
    bool Simulating() const { return simulating_; }

    std::string DiagnosticJson() const;
    std::function<std::uint32_t()> stage_source;

    // P5：确认帧落账观察（生产 MP Replay 录制）。Commit 每退役一条确认
    // Record 调用一次（inputs = 已确认的全席输入）。默认空 = 零变化。
    std::function<void(std::uint32_t, std::uint32_t, const std::array<Netplay::FrameInput, 3>&)>
        confirmed_sink;
    // M9/P6：观战帧记录观察（发布端）。CanSimulate 每放行一帧、输入注入前调用
    // 一次（frame + 本帧决策输入）。宿主在回调内查关头需求、捕获
    // StageEntry 并转发 NetplayRuntime::RecordSpectatorFrame。默认空 =
    // 零变化。回调返回 false = 记录被拒（发布线退役，不阻断模拟）。
    std::function<bool(std::uint32_t, const std::array<Netplay::FrameInput, 3>&)>
        spectator_sink;

private:
    struct DiagnosticInput {
        std::uint32_t frame = 0, stage = 0;
        std::array<std::uint32_t, 3> buttons{};
    };
    struct DiagnosticSample {
        std::uint32_t frame = 0, stage = 0;
        std::array<char, 2048> domains{};
        std::string detail;
    };
    std::array<DiagnosticInput, 128> diagnostic_inputs_{};
    std::size_t diagnostic_input_count_ = 0;
    std::array<DiagnosticSample, NetplayRuntime::kDigestSlots> diagnostic_samples_{};
    NetplayRuntime runtime_;
    th13::StageBattle* battle_ = nullptr;
    GameInput local_input_{};   // 本地席位延迟输入（沿状态机由宿主泵驱动）
    std::array<GameInput, 3> frame_inputs_{};
    GameInput menu_input_{};
    WorldState digest_snapshot_;
    bool audit_enabled_ = false;
    std::deque<std::string> audit_rows_;
    std::uint64_t audit_overflow_ = 0;
    struct TimingEvent {
        const char* kind = nullptr;
        std::uint64_t us = 0;
        std::uint32_t frame = 0, value = 0, detail = 0;
    };
    std::array<TimingEvent, 4096> timing_events_{};
    std::size_t timing_head_ = 0, timing_count_ = 0;
    std::uint64_t timing_overflow_ = 0;
    GameRandom* visual_rng_ = nullptr;  // WorldState capture/restore 的 visual 面
    std::uint32_t sim_frame_ = 0;
    bool simulating_ = false;   // CanStart 后置位（Pump 的 world_ready 面）
    bool world_finished_ = false, world_completed_ = false, spectator_finished_ = false;
    bool active_ = false;
    // 本帧生命周期：CanSimulate 放行 → OnFrameSimulated 落账 → 末尾重置。
    // 同一 sim_frame 内的重复 CanSimulate 查询被挡（宿主每帧多泵）；
    // OnFrameSimulated 尾部重置放行下一帧。
    bool frame_simulated_ = false;
    // 放行幂等门：CanSimulate 放行副作用（远端注入/Record/checkpoint/
    // pending_audio）恰一次；OnFrameSimulated 落账后清除。停等轮的重复
    // 查询不再重复注入（沿状态机 duration/pressed 错位——M8 实证）。
    bool frame_granted_ = false;
    // P3.3：确认帧门（只挡预测帧；纯确认帧照常放行）。
    bool prediction_barrier_ = false;
    bool rollback_pending_ = false;
    bool rollback_failed_ = false;
    // 模拟帧进行中的音频缓冲（Record 落账后清空；P3.1 起记本帧帧号，
    // ObserveAudio 的 Voice 首播/去重按该帧号入账）。
    std::vector<th13::sdl::AudioDevice::RequestObservation>* pending_audio_ = nullptr;
    std::uint32_t pending_audio_frame_ = 0;
    StagePresentation* world_presentation_ = nullptr;
    // P3.2 宿主域接线：RunScene（震动/对话/STD 快照面）与转场句柄读写面。
    th13::RunScene* world_scene_ = nullptr;
    u32* transition_handle_ = nullptr;
    // P0 测量状态：Configure/Stop 清零。frame_granted_us_ = 最近一次
    // CanSimulate 放行时刻（update_us 的起点）。
    std::uint64_t capture_us_max_ = 0, capture_us_sum_ = 0, capture_us_count_ = 0;
    std::uint64_t restore_us_max_ = 0, restore_us_sum_ = 0;
    std::uint64_t update_us_max_ = 0, update_us_sum_ = 0, update_us_count_ = 0;
    std::uint64_t resim_us_sum_ = 0;
    std::uint64_t rollback_distance_sum_ = 0;
    std::uint64_t stall_frames_ = 0;
    std::uint64_t frame_granted_us_ = 0;
    std::string terminal_audit_;
    static std::uint64_t NowMicros();
    void Commit();
    bool ExecuteRollback();
    bool ConsumeReplayedVoice(
        const th13::sdl::AudioDevice::RequestObservation& observation);
};

extern Session session;

}  // namespace th13::multiplayer
