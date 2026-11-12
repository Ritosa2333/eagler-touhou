// TH13 MP 联机运行时（M7：确认输入联网，先纯延迟）。
// 适配层：RollbackCore（输入确认/延迟窗）+ SessionGate（HELLO/READY
// frame0 屏障 + GameplayAbi 契约）+ SessionChannel（协议生命周期）+
// BrowserPeerTransport（RTC mesh vs WebSocket relay 竞速——共用栈不 fork）。
// 驱动（Session）拥有世界 restore/checkpoint 退役；本层不做包投递。
#pragma once

#include "DigestWireFilter.hpp"
#include "SessionSetup.hpp"

#include <eagler/netplay/BrowserPeerTransport.hpp>
#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/NetplaySession.hpp>
#include <eagler/netplay/SessionChannel.hpp>
#include <eagler/netplay/SpectatorStagePlayback.hpp>
#include <eagler/netplay/SpectatorStagePublisher.hpp>
#include <eagler/netplay/SpectatorStageView.hpp>

#include <array>
#include <cstdint>
#include <deque>
#include <string>
#include <vector>

namespace th13::multiplayer {

class NetplayRuntime {
public:
    // P3.4：digest 槽（th09 RollbackSession::HashSlot 同型；环 8 槽，
    // 索引 (frame / 60) % 8）。local/remote 各自到齐后比对——同帧同值
    // 无失步，异值进入终态错误。
    struct DigestSlot {
        std::uint32_t frame = 0;
        std::uint32_t local = 0, remote = 0;
        bool hasLocal = false, hasRemote = false;
    };
    static constexpr std::size_t kDigestSlots = 8;
    static constexpr std::uint32_t kDigestIntervalFrames =
        DigestWireFilter::kIntervalFrames;

    // 构造期接线：DigestWireFilter（注入 SessionChannel 的 PeerTransport&）
    // 消化 T13D 包时回调本实例的 NoteRemote。成员声明序保证 filter 先于
    // channel 构造（channel 持有 filter 引用）。
    NetplayRuntime();
    ~NetplayRuntime();
    NetplayRuntime(const NetplayRuntime&) = delete;
    NetplayRuntime& operator=(const NetplayRuntime&) = delete;

    // 配置（坏身份在 DecodeSessionSetup 已拒；此处校验 transport 可用性）。
    bool Reset(const SessionSetup& setup);
    void Clear();

    // 生产 RollbackCore 配置（唯一真值）：预测模式/安全按键/方向窗由会话
    // 身份派生。诊断工具（预测命中率评估器）必须复用本函数，避免测量配置
    // 与生产漂移。纯重构自 Reset 的内联构造——行为逐字段不变。
    static Netplay::CoreConfig CoreConfigFor(const SessionSetup& setup);

    // relay URL（ws(s)://…）：Connect 后 RTC/relay 竞速在 frame0 前收敛。
    // stageSpectatorHistory=true：席 0 保留观战历史中继线（发布端）。
    bool Connect(const char* relay_url);
    void Close() { transport_.Close(); }

    // ---- M9/P6：公共观战面（对齐 th12 NetplayRuntime 同名接口语义）----
    // 观众连接（relay 观战 WebSocket；id = 观众身份）。观众端只收不发，
    // 不参与 HELLO/READY 门；会话身份由 spectator-session 通告携带。
    bool ConnectSpectator(const char* relay_url, const char* spectator_id);
    bool Spectator() const { return spectator_; }
    // 观众端：收流（Pump 内驱动）+ 回放控制。StagePlaybackReady 后
    // SpectatorState/Control 才有效。
    bool StagePlaybackReady() const {
        return spectator_ && stage_received_ && stage_playback_.Ready();
    }
    Netplay::SpectatorStagePlayback& StagePlayback() { return stage_playback_; }
    const char* SpectatorState() {
        return StagePlaybackReady() ? stage_view_.State(stage_playback_) : nullptr;
    }
    bool SpectatorControl(const char* run, const char* action, std::uint32_t frame) {
        return StagePlaybackReady() &&
               stage_view_.Control(stage_playback_, run, action, frame);
    }
    // 发布端（本地席 0）：每执行帧 Record（initializer = 关头 StageEntry
    // 编码；同关 NotRequired 时空）；Publish 到 min(confirmed, simulated)。
    // 返回 false = 记录被拒（发布线退役，调用方 StopSpectators）。
    bool RecordSpectatorFrame(std::uint32_t frame, std::uint32_t stage,
                              const Netplay::FrameInput* inputs, unsigned count,
                              const std::vector<std::uint8_t>* initializer);
    // 发布冲账（Pump 尾调用；只发确认历史）。
    bool PublishSpectator(std::uint32_t confirmed, std::uint32_t simulated);
    // 终局 End 事件（观众端进结果面）。
    bool FinishSpectator(std::uint32_t end_frame);
    bool SpectatorPublishFailed() const { return publish_failed_; }
    // 关头需求查询（发布端宿主决定是否捕获 StageEntry）。发布器未建立
    //（首帧前）恒 NotRequired——宿主照常记录，RecordSpectatorFrame 首个
    // initializer 到达时建立发布器。
    auto SpectatorCheckpointNeeded(std::uint32_t frame, std::uint32_t stage) const {
        using Requirement = Netplay::SpectatorStagePublisher::CheckpointRequirement;
        if (publish_failed_ || spectator_ || setup_.local_player != 0)
            return Requirement::NotRequired;
        if (!publish_started_) return Requirement::Required;
        return stage_publisher_.NeedsCheckpoint(frame, stage);
    }
    // 观众端收流状态（deadline 观测；宿主超时面）。
    std::uint32_t SpectatorBacklog() const {
        return static_cast<std::uint32_t>(spectator_frames_.size());
    }
    bool SpectatorStreamEnded() const {
        return stage_received_ && stage_playback_.History().Ended();
    }

    // 帧泵：信道收发 + 门推进。world_ready = 宿主世界可用（frame0 后）。
    bool Pump(bool world_ready, std::uint64_t now_ms);
    // Keep the final input/ACK answerable while the room is displayed. A
    // slower peer must be able to finish before the next launch closes it.
    bool PumpTerminal(std::uint64_t now_ms);

    // 本地输入采集：capture_frame 的物理输入 → 延迟窗（frame+delay 生效）。
    bool Capture(std::uint32_t capture_frame, const Netplay::FrameInput& input,
                 std::uint64_t now_ms);
    // 采集相位：宿主 Start 后首帧 capture 0，每游戏帧恰一次 capture →
    // AdvanceCapturePhase +1。暂停/对话期宿主不调 Advance（相位冻结）。
    // capture c 生效于帧 c + input_delay（RollbackCore Reset 的 lead-in
    // neutral 填 0..delay-1）。采集是否进行由 Session 的 simulating_ 门
    //（CaptureLocal）判定，与本相位值无关——相位 0 是合法首帧号。
    std::uint32_t CaptureFrame() const { return next_capture_; }
    void AdvanceCapturePhase() { ++next_capture_; }

    // ---- M8：回滚请求透传（RollbackCore 的纠正前沿）----
    bool HasRollbackRequest() const { return core_.HasRollbackRequest(); }
    std::uint32_t RollbackFrame() const { return core_.RollbackFrame(); }
    void ClearRollbackRequest() { core_.ClearRollbackRequest(); }
    // 纠正区间（th12 NetplayRuntime 同语义）：BeginCorrection 回退决策
    // 历史（RewindSimulationTo）到回滚帧并记录纠正终点；重放追上终点后
    // EndCorrection 关闭。RestartCorrection 处理纠正中再次收到的更早回滚。
    // 本地直驱（无 transport 的 RollbackCore 输入面——native 测试/本地
    // 回放）下 gate 恒不可开：correction 的 CanStart 门用 local_simulation_
    // 替代（生产联机的 gate 语义不变）。
    void SetLocalSimulation(bool enabled) { local_simulation_ = enabled; }
    bool BeginCorrection(std::uint32_t first);
    bool RestartCorrection(std::uint32_t first);
    bool EndCorrection();
    bool Correcting() const { return correction_end_ != Netplay::INVALID_FRAME; }
    std::uint32_t CorrectionEnd() const { return correction_end_; }
    std::uint32_t NextFrame() const { return next_frame_; }

    // frame0 屏障：全员 HELLO+READY 且 transport 可用才放行。
    bool CanStart() const { return gate_.CanStart(); }
    bool AllPeersHello() const { return gate_.AllPeersHello(); }
    void MarkLocalReady() { gate_.MarkLocalReady(); }
    bool LocalReady() const { return gate_.LocalReady(); }

    // 模拟决策：frame 的输入到齐（纯延迟模式无预测——canAdvance 即
    // 全员实际输入确认；M8 起预测路径见 prediction_limit）。
    Netplay::FrameDecision Prepare(std::uint32_t frame) const {
        return core_.PrepareFrame(frame);
    }
    bool MarkSimulated(std::uint32_t frame, const Netplay::FrameDecision& decision) {
        if (!core_.MarkSimulated(frame, decision)) return false;
        // 纠正区间语义（th12 NetplayRuntime:143-148 同构）：next_frame_ 是
        // 驱动层的模拟前沿——BeginCorrection/RestartCorrection 的窗校验与
        // EndCorrection 的终点比对都以它为基准。非纠正路径下每帧落账 +1；
        // 纠正重放期间由 BeginCorrection/RestartCorrection 回退后同样逐帧
        // 推进，追上 correction_end_ 时 EndCorrection 关闭。
        if (!Correcting()) ++next_frame_;
        else if (next_frame_ < correction_end_) ++next_frame_;
        return true;
    }
    bool ConfirmedInputs(std::uint32_t frame,
                         std::array<Netplay::FrameInput, 3>& out) const {
        return core_.ConfirmedInputs(frame, &out);
    }

    // 只读诊断。
    const SessionSetup& Setup() const { return setup_; }
    // 远端席位输入提交（th12 NetplayRuntime:206-211 同构；native 测试
    // 无信道驱动的确认前沿/回滚请求注入面。生产信道走 SessionChannel
    // 的 ApplyInputPacket——同一下游 core_.SubmitRemoteInput）。
    Netplay::RemoteInputResult SubmitRemote(unsigned seat, std::uint32_t frame,
                                            const Netplay::FrameInput& input) {
        return core_.SubmitRemoteInput(std::uint8_t(seat), frame, input);
    }
    // P2：墙钟时间同步建议（SessionChannel 的 0.98..1.02 修剪均值；无
    // 样本/Adonis 相位恒 1.0）。只影响呈现/模拟节奏，不改逻辑帧。
    double IntervalScale() const { return channel_.IntervalScale(); }

    // P5：生产导出面（th13_mp_status 的会话/信道只读镜像）。直接转发
    // SessionChannel/transport 的现成 getter——不新增计数语义。
    std::uint32_t PacketsSent() const { return channel_.PacketsSent(); }
    std::uint32_t PacketsReceived() const { return channel_.PacketsReceived(); }
    std::uint32_t RepairsSent() const { return channel_.RepairsSent(); }
    std::uint32_t PacketsIgnored() const { return channel_.PacketsIgnored(); }
    bool TransportOpen() const { return transport_.IsOpen(); }
    // 竞速收敛后的实际路径：1=rtc，2=relay，0=未选（th15 Status 同码位）。
    std::uint32_t RouteCode() const {
        const char* mode = transport_.Mode();
        if (!mode) return 0;
        const std::string text(mode);
        return text == "rtc" ? 1 : text == "relay" ? 2 : 0;
    }

    // ---- P3.4：线上 digest（60 帧一采样，T13D 控制包）----
    // 写本地槽 + 经 transport_.SendControl 发 20B 包 + 若对端值已到则比对。
    // 返回 false = 参数非法、发送失败或本次比对失配（失配已置终态 error_）。
    bool SendDigestControl(std::uint32_t frame, std::uint32_t digest);
    // 对端 digest 回填：生产路径由 DigestWireFilter 消化 T13D 时回调；
    // native 直驱（无 transport）测试直接注入以验证槽位比对逻辑。
    // 非 60 对齐帧忽略（线上协议层已由 filter 拦截）。
    void NoteRemote(std::uint32_t frame, std::uint32_t digest);
    // 失配终态：Pump 返回假（Session::Pump 随之停帧），Error() 文本为
    // "TH13 desync at frame N"（error_ 优先级高于 channel/transport 错误）。
    bool DigestMismatched() const { return digest_mismatch_; }
    std::uint32_t DigestMismatchFrame() const { return digest_mismatch_frame_; }
    const DigestSlot& MismatchDigests() const { return mismatch_digests_; }
    std::uint64_t DigestsSent() const { return digests_sent_; }
    std::uint64_t DigestsReceived() const { return digests_received_; }
    std::uint64_t DesyncDetected() const { return desync_detected_; }
    // BrowserRuntime 调试 JSON 的只读统计镜像：指向最近构造的实例
    //（生产只有 Session 的 runtime_ 一个；native 多实例不读此面）。
    // 不在 Session.hpp 增加透传访问器——该文件本阶段仅允许 Record 字段。
    struct DigestWireStats {
        std::uint64_t sent = 0;
        std::uint64_t received = 0;
        std::uint64_t desync = 0;
        bool mismatched = false;
    };
    static DigestWireStats DebugDigestStats();

    const char* Error() const;
    bool Connected() const { return transport_.IsOpen(); }
    bool TransportFailed() const { return transport_.Failed(); }
    bool TransportDisconnected() const { return transport_.Disconnected(); }
    std::uint32_t ConfirmedThroughAll() const {
        return core_.ConfirmedThroughAllRemotes();
    }
    std::uint32_t LastSimulated() const { return core_.LastSimulatedFrame(); }
    // 设备席位隔离：本地席位号（远端输入按 peer 席位路由，本地席位
    // 恒为 setup_.local_player——设备与席位一一绑定）。
    std::uint8_t LocalSeat() const { return setup_.local_player; }
    std::uint32_t GameplayAbiValue() const { return multiplayer::GameplayAbi(setup_); }

private:
    void MarkDesync(std::uint32_t frame);
    // 观众端收流（Pump 的 spectator_ 分支）：transport 收包 → 事件/帧
    // 解码 → stage_playback_。超时/失配置终态 error_。
    bool DrainSpectator(std::uint64_t now_ms);

    SessionSetup setup_{};
    bool configured_ = false;
    bool local_simulation_ = false;  // 本地直驱（correction 的 gate 替代门）
    std::uint32_t next_capture_ = 0;
    Netplay::RollbackCore core_;
    std::uint32_t next_frame_ = 0;          // 下一个待模拟帧
    std::uint32_t correction_end_ = Netplay::INVALID_FRAME;
    Netplay::SessionGate gate_;
    Netplay::BrowserPeerTransport transport_;
    // P3.4：digest 过滤代理必须先于 channel_ 声明（channel_ 持有其引用）。
    DigestWireFilter digest_filter_{transport_};
    Netplay::SessionChannel channel_{digest_filter_};
    std::array<DigestSlot, kDigestSlots> digest_slots_{};
    DigestSlot mismatch_digests_{};
    bool digest_mismatch_ = false;
    std::uint32_t digest_mismatch_frame_ = 0;
    std::uint64_t digests_sent_ = 0, digests_received_ = 0, desync_detected_ = 0;
    std::string error_;
    // ---- M9/P6：观战面 ----
    bool spectator_ = false;             // 观众端（ConnectSpectator 置位）
    bool stage_received_ = false;        // 首个 stage 事件已到
    bool publish_started_ = false;       // 发布器已 Reset（首个 initializer）
    bool publish_failed_ = false;        // 发布线退役（不可恢复）
    std::uint64_t spectator_deadline_ms_ = 0;  // bootstrap/活跃超时面
    std::uint32_t spectator_recorded_ = 0;     // 发布端已 Record 前沿
    std::uint32_t receive_frame_ = 0;           // 旧输入流帧号（无 stage 事件前）
    Netplay::SpectatorStagePublisher stage_publisher_{};
    Netplay::SpectatorStagePlayback stage_playback_{
        Netplay::SpectatorStagePlayback::RestoreMode::Run,
        Netplay::SpectatorStageHistory::FullRunMaxFrames};
    Netplay::SpectatorStageView stage_view_{};
    std::deque<Netplay::SpectatorFramePacket> spectator_frames_{};  // 旧输入流缓冲
};

}  // namespace th13::multiplayer
