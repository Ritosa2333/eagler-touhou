#include "NetplayRuntime.hpp"

#include "SpectatorJournal.hpp"

#include <cstring>

namespace th13::multiplayer {

namespace {
const NetplayRuntime* g_debug_digest_instance = nullptr;

void DigestPacketThunk(void* context, std::uint32_t frame, std::uint32_t digest) {
    static_cast<NetplayRuntime*>(context)->NoteRemote(frame, digest);
}
}

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

Netplay::CoreConfig NetplayRuntime::CoreConfigFor(const SessionSetup& setup) {
    Netplay::CoreConfig config;
    config.sessionId = setup.session_id;
    config.playerCount = std::uint8_t(setup.player_count);
    config.localPlayer = std::uint8_t(setup.local_player);
    config.inputDelay = std::uint8_t(setup.input_delay);
    config.maxRollbackFrames = SessionSetup::MaxRollbackFrames;
    config.directTouchIsAbsolute = true;
    config.allowPrediction = setup.mode == 2;
    if (config.allowPrediction) {
        config.predictableButtons = 0x0f8 | 0x001;
        config.directionButtons = 0x0f0;
        config.prediction = Netplay::InputPrediction::ConfirmedHold;
    } else {
        config.predictableButtons = 0;
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
    digest_filter_.SetSessionId(setup.session_id);
    if (!core_.Reset(CoreConfigFor(setup))) {
        error_ = "rollback core rejected the session";
        return false;
    }
    Netplay::SessionConfig session;
    session.sessionId = setup.session_id;
    session.seed = setup.seed;
    session.gameplayAbi = GameplayAbi(setup);
    session.gameId = 0x54483133u;
    session.playerCount = std::uint8_t(setup.player_count);
    session.localPlayer = std::uint8_t(setup.local_player);
    if (!gate_.Reset(session)) {
        error_ = "session gate rejected the contract";
        return false;
    }
    configured_ = true;
    next_capture_ = 0;
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
    next_frame_ = 0;
    correction_end_ = Netplay::INVALID_FRAME;
    digest_slots_ = {};
    mismatch_digests_ = {};
    digest_mismatch_ = false;
    digest_mismatch_frame_ = 0;
    digests_sent_ = digests_received_ = desync_detected_ = 0;
    error_.clear();
    transport_lost_ = false;
    terminal_released_ = false;
    publish_finished_ = false;
    terminal_started_ms_ = terminal_release_ms_ = 0;
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
    if (!transport_.Connect(relay_url, std::uint8_t(setup_.local_player),
                           std::uint8_t(setup_.player_count), true)) {
        error_ = "transport connect failed";
        return false;
    }
    transport_.BeginSpectatorSession(setup_.session_id, setup_.seed);
    gate_.SetTransportAvailable(true);
    return true;
}

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
    spectator_deadline_ms_ = 0;
    return true;
}

bool NetplayRuntime::RecordSpectatorFrame(
    std::uint32_t frame, std::uint32_t stage, const Netplay::FrameInput* inputs,
    unsigned count, const std::vector<std::uint8_t>* initializer) {
    if (!configured_ || spectator_ || setup_.local_player != 0 || publish_failed_)
        return true;
    if (!publish_started_) {
        if (!initializer || initializer->empty()) return true;
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

bool NetplayRuntime::FinishSpectator(std::uint32_t end_frame) {
    if (!publish_started_ || publish_failed_ || spectator_ ||
        setup_.local_player != 0)
        return true;
    const bool finished = stage_publisher_.Finish(
        end_frame, [&](const std::vector<std::uint8_t>& wire) {
            return transport_.SendSpectator(wire.data(), wire.size());
        });
    if (finished) {
        transport_.StopSpectators(true);
        publish_finished_ = true;
    }
    return finished;
}

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

bool NetplayRuntime::InputHistoryComplete() const {
    const auto simulated = core_.LastSimulatedFrame();
    const auto confirmed = core_.ConfirmedThroughAllRemotes();
    return simulated != Netplay::INVALID_FRAME && confirmed != Netplay::INVALID_FRAME &&
           confirmed >= simulated && !core_.HasRollbackRequest();
}

void NetplayRuntime::NoteInputStarved() {
    if (!configured_ || local_simulation_ || spectator_ || transport_lost_ ||
        transport_.Recovering() || !(transport_.Disconnected() || transport_.Failed()))
        return;
    transport_lost_ = true;
    error_ = transport_.Failed() ? "TH13 gameplay transport failed" : "TH13 gameplay transport disconnected";
}

bool NetplayRuntime::Pump(bool world_ready, std::uint64_t now_ms) {
    if (!configured_) return true;
    if (digest_mismatch_ || transport_lost_) return false;
    if (spectator_) return DrainSpectator(now_ms);
    if (!local_simulation_ && !transport_.Recovering() &&
        (transport_.Disconnected() || transport_.Failed())) {
        if (world_ready && !transport_.Failed() && InputHistoryComplete()) return true;
        error_ = transport_.Failed() ? "TH13 gameplay transport failed" : "TH13 gameplay transport disconnected";
        return false;
    }
    if (!channel_.Active() && transport_.IsOpen()) {
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
    if (digest_filter_.MalformedDigestPackets()) {
        error_ = "TH13 malformed digest packet";
        return false;
    }
    if (digest_mismatch_) return false;
    return true;
}

bool NetplayRuntime::PumpTerminal(std::uint64_t now_ms) {
    if (!configured_ || local_simulation_ || spectator_) return true;
    const auto terminal = core_.LastSimulatedFrame();
    if (terminal == Netplay::INVALID_FRAME) {
        if (transport_.Disconnected() || transport_.Failed()) return true;
        if (channel_.Active()) return channel_.Pump(gate_, core_, now_ms, false);
        return !channel_.Retiring() || channel_.PumpRetirement(now_ms);
    }
    if (!terminal_started_ms_) terminal_started_ms_ = now_ms;
    if (transport_.Disconnected() || transport_.Failed()) {
        ReleaseTerminalTransport();
        return true;
    }
    if (channel_.Active()) {
        if (!channel_.Pump(gate_, core_, now_ms, false)) return false;
        if (!channel_.FlushRetirementFence(core_, terminal, now_ms)) return false;
        if (channel_.CanRetire(core_, terminal)) {
            if (!channel_.Retire(core_, terminal, now_ms)) return false;
            terminal_release_ms_ = now_ms + kTerminalLingerMs;
        }
    } else if (channel_.Retiring() && !channel_.PumpRetirement(now_ms)) return false;
    const bool retired = terminal_release_ms_ && now_ms >= terminal_release_ms_;
    if (!terminal_released_ &&
        ((retired && SpectatorsSettled()) || now_ms - terminal_started_ms_ >= kTerminalTailMs))
        ReleaseTerminalTransport();
    return true;
}

void NetplayRuntime::ReleaseTerminalTransport() {
    if (terminal_released_) return;
    terminal_released_ = true;
    transport_.ReleaseRelayPresence();
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

bool NetplayRuntime::BeginCorrection(std::uint32_t first) {
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

void NetplayRuntime::NoteRemote(std::uint32_t frame, std::uint32_t digest) {
    if (frame % kDigestIntervalFrames != 0) return;
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
    if (digest_mismatch_) return;
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

}
