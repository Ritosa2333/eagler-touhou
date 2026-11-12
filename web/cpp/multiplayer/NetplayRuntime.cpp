// TH13 MP 联机运行时实现（M7）。语义表见 NetplayRuntime.hpp 文件头。
#include "NetplayRuntime.hpp"

#include "SpectatorJournal.hpp"  // kGame/SpectatorStageSchema（观战线标识）

#include <cstring>

namespace th13::multiplayer {

namespace {
// P3.4：BrowserRuntime 调试统计镜像（最近构造的实例；生产唯一实例 =
// Session::runtime_）。native 多实例并存时不依赖此面。
const NetplayRuntime* g_debug_digest_instance = nullptr;

void DigestPacketThunk(void* context, std::uint32_t frame, std::uint32_t digest) {
    static_cast<NetplayRuntime*>(context)->NoteRemote(frame, digest);
}
}  // namespace

NetplayRuntime::NetplayRuntime() {
    digest_filter_.SetDigestHandler(&DigestPacketThunk, this);
    g_debug_digest_instance = this;
}

NetplayRuntime::~NetplayRuntime() {
    if (g_debug_digest_instance == this) g_debug_digest_instance = nullptr;
}

NetplayRuntime::DigestWireStats NetplayRuntime::DebugDigestStats() {
    DigestWireStats stats;
    if (!g_debug_digest_instance) return stats;
    stats.sent = g_debug_digest_instance->digests_sent_;
    stats.received = g_debug_digest_instance->digests_received_;
    stats.desync = g_debug_digest_instance->desync_detected_;
    stats.mismatched = g_debug_digest_instance->digest_mismatch_;
    return stats;
}

// 生产配置唯一真值（自 Reset 内联块原样提取；纯重构，行为不变）。
Netplay::CoreConfig NetplayRuntime::CoreConfigFor(const SessionSetup& setup) {
    // 纯延迟（mode=1）：本地输入在 capture+input_delay 帧生效；无预测
    // （allowPrediction=false——缺输入即停等，不做推测帧）。
    Netplay::CoreConfig config;
    config.sessionId = setup.session_id;
    config.playerCount = std::uint8_t(setup.player_count);
    config.localPlayer = std::uint8_t(setup.local_player);
    config.inputDelay = std::uint8_t(setup.input_delay);
    config.maxRollbackFrames = SessionSetup::MaxRollbackFrames;
    config.directTouchIsAbsolute = true;
    config.allowPrediction = setup.mode == 2;  // M8：预测模式（v2 线格式）
    if (config.allowPrediction) {
        // 预测安全键（缺远端输入时可重复）：方向（0x0f0）+ 射击（0x001）
        // + 低速（0x008）。炸弹/灵界/暂停是边缘动作——保持释放直到确切
        // 输入到达（错误预测触发回滚）。
        config.predictableButtons = 0x0f8 | 0x001;
        config.directionButtons = 0x0f0;
        // PR#225 预测内核：以各席位连续确认输入为基准的持续预测
        // （ConfirmedHold）。maxDirectionPredictionFrames 在该模式下
        // 不参与决策，故不再设置。
        config.prediction = Netplay::InputPrediction::ConfirmedHold;
    } else {
        config.predictableButtons = 0;  // 纯延迟：缺输入停等
        config.directionButtons = 0;
    }
    return config;
}

bool NetplayRuntime::Reset(const SessionSetup& setup) {
    if (!setup.valid()) {
        error_ = "invalid session setup";
        return false;
    }
    Clear();
    setup_ = setup;
    // P3.4：digest 包只认本会话身份（旧会话/噪声 T13D 在 filter 内计数丢弃）。
    digest_filter_.SetSessionId(setup.session_id);
    if (!core_.Reset(CoreConfigFor(setup))) {
        error_ = "rollback core rejected the session";
        return false;
    }
    Netplay::SessionConfig session;
    session.sessionId = setup.session_id;
    session.seed = setup.seed;
    session.gameplayAbi = GameplayAbi(setup);
    session.gameId = 0x54483133u;  // 'TH13'
    session.playerCount = std::uint8_t(setup.player_count);
    session.localPlayer = std::uint8_t(setup.local_player);
    if (!gate_.Reset(session)) {
        error_ = "session gate rejected the contract";
        return false;
    }
    configured_ = true;
    next_capture_ = 0;  // 采集相位 0 = 首帧采集号（Start 不消耗相位；
                         // 采集门由 Session 的 simulating_ 判定）。
    return true;
}

void NetplayRuntime::Clear() {
    core_.Clear();
    gate_.Clear();
    channel_.Clear();
    digest_filter_.Clear();
    transport_.Close();
    configured_ = false;
    local_simulation_ = false;
    next_capture_ = 0;
    // 纠正窗状态必须随会话重置（M8 native session_driven 实证：残留
    // 令下一次会话的 BeginCorrection 窗校验恒拒绝——target 与陈旧
    // next_frame_ 的距离超窗）。
    next_frame_ = 0;
    correction_end_ = Netplay::INVALID_FRAME;
    // P3.4：digest 槽/终态/统计随会话清零。
    digest_slots_ = {};
    mismatch_digests_ = {};
    digest_mismatch_ = false;
    digest_mismatch_frame_ = 0;
    digests_sent_ = digests_received_ = desync_detected_ = 0;
    error_.clear();
    // M9/P6：观战面随会话重置（发布器/回放/缓冲全清）。
    spectator_ = false;
    stage_received_ = false;
    publish_started_ = false;
    publish_failed_ = false;
    spectator_deadline_ms_ = 0;
    spectator_recorded_ = 0;
    receive_frame_ = 0;
    stage_publisher_ = {};
    stage_playback_.Clear();
    stage_view_.Clear();
    spectator_frames_.clear();
}

bool NetplayRuntime::Connect(const char* relay_url) {
    if (!configured_ || !relay_url || !*relay_url) {
        error_ = "connect before configure";
        return false;
    }
    // 观战历史中继线（stageSpectatorHistory）：席 0 保留发布面
    //（BeginSpectatorSession 通告 + 观战帧上载），其余席无发布权。
    if (!transport_.Connect(relay_url, std::uint8_t(setup_.local_player),
                           std::uint8_t(setup_.player_count), true)) {
        error_ = "transport connect failed";
        return false;
    }
    transport_.BeginSpectatorSession(setup_.session_id, setup_.seed);
    gate_.SetTransportAvailable(true);
    return true;
}

// ---- M9/P6：公共观战面 ----
// 观众连接：不参与 HELLO/READY 门；会话身份由 relay 的 spectator-session
// 通告携带（TakeSpectatorSession 在 DrainSpectator 内消费）。
bool NetplayRuntime::ConnectSpectator(const char* relay_url,
                                      const char* spectator_id) {
    if (!configured_ || !relay_url || !*relay_url || !spectator_id || !*spectator_id) {
        error_ = "invalid spectator admission";
        return false;
    }
    if (!transport_.ConnectSpectator(relay_url, spectator_id,
                                     std::uint8_t(setup_.player_count), true)) {
        error_ = "spectator transport could not connect";
        return false;
    }
    spectator_ = true;
    stage_playback_.Clear();
    stage_view_.Clear();
    spectator_frames_.clear();
    stage_received_ = false;
    receive_frame_ = 0;
    spectator_deadline_ms_ = 0;  // Pump 首轮以当前时刻起算
    return true;
}

// 发布端帧记录（每执行帧恰一次，含回滚重放帧——Record 幂等校验输入
// 一致）。首次带 initializer 时建立发布器；同关 NotRequired 传空。
bool NetplayRuntime::RecordSpectatorFrame(
    std::uint32_t frame, std::uint32_t stage, const Netplay::FrameInput* inputs,
    unsigned count, const std::vector<std::uint8_t>* initializer) {
    if (!configured_ || spectator_ || setup_.local_player != 0 || publish_failed_)
        return true;  // 无发布权/已退役：静默（不阻断玩家模拟）
    if (!publish_started_) {
        if (!initializer || initializer->empty()) return true;  // 首帧前无 checkpoint：不入流
        if (!stage_publisher_.Reset(SpectatorJournal::kGame,
                                    std::uint8_t(setup_.player_count),
                                    setup_.session_id, GameplayAbi(setup_), frame)) {
            publish_failed_ = true;
            transport_.StopSpectators();
            return true;
        }
        publish_started_ = true;
    }
    Netplay::SpectatorStagePublisher::Frame record;
    record.frame = frame;
    record.stage = stage;
    record.schema = initializer && !initializer->empty() ? SpectatorStageSchema : 0;
    if (initializer && !initializer->empty()) record.initializer = *initializer;
    for (unsigned seat = 0; seat < count && seat < 3; ++seat)
        record.inputs[seat] = inputs[seat];
    record.hasInputs = true;
    if (!stage_publisher_.Record(std::move(record))) {
        publish_failed_ = true;
        transport_.StopSpectators();
        return true;
    }
    spectator_recorded_ = frame;
    return true;
}

// 发布冲账：只发确认历史（confirmed 与 simulated 取 min）。
bool NetplayRuntime::PublishSpectator(std::uint32_t confirmed,
                                      std::uint32_t simulated) {
    if (!publish_started_ || publish_failed_ || spectator_ ||
        setup_.local_player != 0)
        return true;
    if (transport_.SpectatorState() < 0) {
        publish_failed_ = true;
        transport_.StopSpectators();
        return true;
    }
    Netplay::SpectatorStagePublisher::PublishFailure failure{};
    const bool published = stage_publisher_.Publish(
        confirmed, simulated,
        [&](std::uint32_t frame, std::array<Netplay::FrameInput, 3>& out) {
            return core_.ConfirmedInputs(frame, &out);
        },
        [&](const std::vector<std::uint8_t>& wire) {
            return transport_.SendSpectator(wire.data(), wire.size());
        },
        &failure);
    if (!published && !Netplay::SpectatorStagePublisher::Retryable(failure)) {
        publish_failed_ = true;
        transport_.StopSpectators();
    }
    return true;
}

// 终局 End 事件（发布端）。
bool NetplayRuntime::FinishSpectator(std::uint32_t end_frame) {
    if (!publish_started_ || publish_failed_ || spectator_ ||
        setup_.local_player != 0)
        return true;
    const bool finished = stage_publisher_.Finish(
        end_frame, [&](const std::vector<std::uint8_t>& wire) {
            return transport_.SendSpectator(wire.data(), wire.size());
        });
    if (finished) transport_.StopSpectators(true);
    return finished;
}

// 观众端收流：transport 轮询 → 事件（checkpoint/node/end/bootstrap）或
// 输入帧包。会话通告（spectator-session）重置身份与 playback。
bool NetplayRuntime::DrainSpectator(std::uint64_t now_ms) {
    std::uint64_t session = 0;
    std::uint32_t seed = 0;
    if (transport_.TakeSpectatorSession(&session, &seed)) {
        auto setup = setup_;
        setup.session_id = session;
        setup.seed = seed;
        if (!setup.valid()) {
            error_ = "invalid TH13 spectator session";
            return false;
        }
        // This announcement belongs to the open observer transport. Reset()
        // closes that transport and discards its queued bootstrap packets.
        setup_ = setup;
        spectator_frames_.clear();
        stage_playback_.Clear();
        stage_view_.Clear();
        stage_received_ = false;
        receive_frame_ = 0;
        spectator_deadline_ms_ = now_ms + 90'000;
    }
    if (!spectator_deadline_ms_) spectator_deadline_ms_ = now_ms + 90'000;
    std::vector<std::uint8_t> bytes;
    for (unsigned n = 0; n < 128 && transport_.Poll(&bytes); ++n) {
        if (Netplay::IsSpectatorStageEvent(bytes.data(), bytes.size())) {
            Netplay::SpectatorStageEvent event;
            if (!Netplay::DecodeSpectatorStageEvent(bytes.data(), bytes.size(),
                                                    &event) ||
                event.game != SpectatorJournal::kGame ||
                event.playerCount != setup_.player_count ||
                event.sessionId != setup_.session_id ||
                event.gameplayAbi != GameplayAbi(setup_) ||
                (event.kind == Netplay::SpectatorEventKind::Checkpoint &&
                 event.schema != SpectatorStageSchema) ||
                !stage_playback_.Receive(event)) {
                error_ = "invalid TH13 spectator stage stream";
                return false;
            }
            stage_received_ = true;
            spectator_frames_.clear();
            spectator_deadline_ms_ = now_ms + 15'000;
            continue;
        }
        if (stage_received_) {
            Netplay::SpectatorFramePacket packet;
            if (!Netplay::DecodeSpectatorFramePacket(bytes.data(), bytes.size(),
                                                     &packet) ||
                !stage_playback_.Receive(packet)) {
                error_ = "invalid TH13 spectator input stream";
                return false;
            }
            spectator_deadline_ms_ = now_ms + 15'000;
            continue;
        }
        // 旧输入流（无 stage 事件前）：按帧号连续入缓冲（对齐 th12 语义）。
        Netplay::SpectatorFramePacket packet;
        if (spectator_frames_.size() >= 8192 ||
            !Netplay::DecodeSpectatorFramePacket(bytes.data(), bytes.size(),
                                                 &packet) ||
            packet.sessionId != setup_.session_id ||
            packet.gameplayAbi != GameplayAbi(setup_) ||
            packet.playerCount != setup_.player_count ||
            packet.frame != receive_frame_) {
            error_ = "invalid TH13 spectator input stream";
            return false;
        }
        spectator_frames_.push_back(packet);
        ++receive_frame_;
        spectator_deadline_ms_ = now_ms + 15'000;
    }
    if (stage_received_) {
        if (stage_playback_.History().Ended()) return true;
        if (transport_.Failed()) {
            error_ = transport_.LastError();
            return false;
        }
        // bootstrap 有界等待；转直播后静默不判死（源可能暂停）。
        if (!stage_playback_.Ready() && now_ms >= spectator_deadline_ms_) {
            error_ = "TH13 spectator stage stream stalled; players are unaffected";
            return false;
        }
        return true;
    }
    if (spectator_frames_.empty()) {
        if (transport_.Failed()) {
            error_ = transport_.LastError();
            return false;
        }
        if (now_ms >= spectator_deadline_ms_) {
            error_ = "TH13 spectator stream ended or stalled; players are unaffected";
            return false;
        }
    }
    return true;
}

bool NetplayRuntime::Pump(bool world_ready, std::uint64_t now_ms) {
    if (!configured_) return true;  // 未配置：静默（单席/本地模式）
    // P3.4：失配是终态（error_ 已含帧号；Error() 优先返回它）。
    if (digest_mismatch_) return false;
    // 观众端：只收流，不推进玩家信道/门。
    if (spectator_) return DrainSpectator(now_ms);
    if (!local_simulation_ && !transport_.Recovering() &&
        (transport_.Disconnected() || transport_.Failed())) {
        error_ = transport_.Failed() ? "TH13 gameplay transport failed" : "TH13 gameplay transport disconnected";
        return false;
    }
    if (!channel_.Active() && transport_.IsOpen()) {
        // PR#225：输入重传周期与 th15 生产配置对齐（16 ms）；其余默认。
        Netplay::SessionChannelConfig policy;
        policy.inputResendMs = 16;
        if (!channel_.BeginSession(gate_.Config(), now_ms, policy)) {
            error_ = channel_.ErrorText();
            return false;
        }
    }
    if (channel_.Active() &&
        !channel_.Pump(gate_, core_, now_ms, world_ready)) {
        error_ = channel_.ErrorText();
        return false;
    }
    // P3.4：filter 在本次 channel_.Pump 内可能消化到非法 digest 包
    //（非 20B / 帧号未 60 对齐）——协议违规与失配同走终态。
    if (digest_filter_.MalformedDigestPackets()) {
        error_ = "TH13 malformed digest packet";
        return false;
    }
    if (digest_mismatch_) return false;  // 本次 Pump 内消化 T13D 触发
    return true;
}

bool NetplayRuntime::PumpTerminal(std::uint64_t now_ms) {
    if (!configured_ || local_simulation_ || spectator_ ||
        transport_.Disconnected() || transport_.Failed()) return true;
    if (channel_.Active()) {
        if (!channel_.Pump(gate_, core_, now_ms, false)) return false;
        const auto terminal = core_.LastSimulatedFrame();
        if (terminal == Netplay::INVALID_FRAME) return true;
        if (!channel_.FlushRetirementFence(core_, terminal, now_ms)) return false;
        if (channel_.CanRetire(core_, terminal))
            return channel_.Retire(core_, terminal, now_ms);
        return true;
    }
    return !channel_.Retiring() || channel_.PumpRetirement(now_ms);
}

bool NetplayRuntime::Capture(std::uint32_t capture_frame,
                             const Netplay::FrameInput& input,
                             std::uint64_t now_ms) {
    if (!configured_) {
        error_ = "capture before configure";
        return false;
    }
    if (!core_.ScheduleLocalInput(capture_frame, input)) {
        error_ = "input schedule rejected the capture frame";
        return false;
    }
    if (channel_.Active() &&
        !channel_.LocalCaptured(core_, capture_frame, now_ms)) {
        error_ = channel_.ErrorText();
        return false;
    }
    return true;
}

// 纠正区间（th12 NetplayRuntime:160-172 同语义；prediction_limit 窗校验）。
bool NetplayRuntime::BeginCorrection(std::uint32_t first) {
    // 门：生产联机走 gate（CanStart）；本地直驱（无 transport 的
    // RollbackCore 输入面）恒不可开 gate——用 local_simulation_ 替代。
    if (!configured_ || !(CanStart() || local_simulation_) || Correcting() ||
        first != core_.RollbackFrame() || first >= next_frame_ ||
        next_frame_ - first > setup_.prediction_limit)
        return false;
    const auto end = next_frame_;
    if (!core_.RewindSimulationTo(first)) return false;
    correction_end_ = end;
    next_frame_ = first;
    return true;
}

bool NetplayRuntime::RestartCorrection(std::uint32_t first) {
    if (!configured_ || !(CanStart() || local_simulation_) || !Correcting() ||
        first != core_.RollbackFrame() || first >= next_frame_ ||
        next_frame_ > correction_end_ ||
        correction_end_ - first > setup_.prediction_limit)
        return false;
    if (!core_.RewindSimulationTo(first)) return false;
    next_frame_ = first;
    return true;
}

bool NetplayRuntime::EndCorrection() {
    if (!Correcting() || core_.HasRollbackRequest() || next_frame_ != correction_end_)
        return false;
    correction_end_ = Netplay::INVALID_FRAME;
    return true;
}

// P3.4：写本地 digest 槽 + 发 T13D 控制包 + 比对已到的对端值。
// 采样口径 = 60 帧（kDigestIntervalFrames）；槽索引 (frame/60) % 8。
bool NetplayRuntime::SendDigestControl(std::uint32_t frame,
                                       std::uint32_t digest) {
    if (!configured_) {
        error_ = "digest before configure";
        return false;
    }
    if (frame % kDigestIntervalFrames != 0) {
        error_ = "TH13 invalid digest frame";
        return false;
    }
    auto& slot = digest_slots_[(frame / kDigestIntervalFrames) % kDigestSlots];
    if (slot.frame != frame) {
        slot = DigestSlot{};
        slot.frame = frame;
    }
    slot.local = digest;
    slot.hasLocal = true;
    ++digests_sent_;
    // 可靠有序控制通道（RTC control / relay 天然可靠）；transport 未开
    //（native 直驱/尚未连接）时只做本地账，不伪造发送失败。
    if (transport_.IsOpen()) {
        std::uint8_t packet[DigestWireFilter::kPacketBytes] = {'T', '1', '3', 'D'};
        const std::uint64_t session_id = setup_.session_id;
        std::memcpy(packet + 4, &session_id, sizeof session_id);
        std::memcpy(packet + 12, &frame, sizeof frame);
        std::memcpy(packet + 16, &digest, sizeof digest);
        if (!transport_.SendControl(packet, sizeof packet)) {
            error_ = "TH13 digest send failed";
            return false;
        }
    }
    if (slot.hasRemote && slot.remote != digest) {
        MarkDesync(frame);
        return false;
    }
    return true;
}

// P3.4：对端 digest 回填（filter 消化 T13D 或 native 直驱注入）。
void NetplayRuntime::NoteRemote(std::uint32_t frame, std::uint32_t digest) {
    if (frame % kDigestIntervalFrames != 0) return;  // 协议层已拦截
    ++digests_received_;
    auto& slot = digest_slots_[(frame / kDigestIntervalFrames) % kDigestSlots];
    if (slot.frame != frame) {
        slot = DigestSlot{};
        slot.frame = frame;
    }
    slot.remote = digest;
    slot.hasRemote = true;
    if (slot.hasLocal && slot.local != digest) MarkDesync(frame);
}

void NetplayRuntime::MarkDesync(std::uint32_t frame) {
    if (digest_mismatch_) return;  // 只记首次（终态）
    digest_mismatch_ = true;
    digest_mismatch_frame_ = frame;
    mismatch_digests_ = digest_slots_[(frame / kDigestIntervalFrames) % kDigestSlots];
    ++desync_detected_;
    error_ = "TH13 desync at frame " + std::to_string(frame);
}

const char* NetplayRuntime::Error() const {
    if (!error_.empty()) return error_.c_str();
    if (channel_.Error() != Netplay::SessionChannel::Failure::None)
        return channel_.ErrorText();
    if (transport_.Failed()) return "transport failed";
    if (transport_.Disconnected()) return "transport disconnected";
    return "";
}

}  // namespace th13::multiplayer
