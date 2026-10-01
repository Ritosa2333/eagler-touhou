#include <eagler/netplay/SessionChannel.hpp>

#include <algorithm>
#include <limits>

namespace Netplay
{
namespace
{
constexpr std::size_t MaxPacketBytes = 1024;
bool atLeast(std::uint32_t value, std::uint32_t target)
{
    return value != INVALID_FRAME && value >= target;
}
bool newer(std::uint32_t value, std::uint32_t previous)
{
    return static_cast<std::int32_t>(value - previous) > 0;
}
}

SessionChannel::SessionChannel(PeerTransport &transport) : transport_(transport)
{
    // Reserve once in a fixed order, not on the arrival of an unpredictable
    // packet. Poll/encoding/retransmit reuses this bounded scratch space.
    incoming_.reserve(MaxPacketBytes);
    outgoing_.reserve(MaxPacketBytes);
    for (auto &packet : terminalAcks_) packet.reserve(MaxPacketBytes);
    Clear();
}

void SessionChannel::Clear()
{
    pacing_.Reset();
    policy_ = {};
    session_ = {};
    active_ = observedClock_ = helloSent_ = forceControl_ = forceInputs_ = false;
    failure_ = Failure::None;
    lastClock_ = beginTime_ = nextControl_ = nextInput_ = nextRetirementFence_ = 0;
    latestCapture_ = INVALID_FRAME;
    sentSequence_.fill(0);
    receivedSequence_.fill(0);
    peerFrame_.fill(INVALID_FRAME);
    repairFrontier_.fill(INVALID_FRAME);
    lastRepair_.fill(0);
    peerNeedsAck_.fill(false);
    for (auto &watchdog : watchdog_) watchdog.Disarm();
    incoming_.clear();
    outgoing_.clear();
    previousSession_ = nextRetiredSend_ = 0;
    previousCount_ = previousLocal_ = 0;
    previousNeeded_.fill(false);
    for (auto &packet : terminalAcks_) packet.clear();
    sent_ = received_ = ignored_ = repairs_ = 0;
}

bool SessionChannel::Fail(Failure failure)
{
    if (failure_ == Failure::None) failure_ = failure;
    return false;
}

bool SessionChannel::ObserveClock(std::uint64_t nowMs)
{
    if (observedClock_ && nowMs < lastClock_) return Fail(Failure::ClockReversed);
    observedClock_ = true;
    lastClock_ = nowMs;
    return true;
}

bool SessionChannel::BeginSession(const SessionConfig &session, std::uint64_t nowMs,
                                  const SessionChannelConfig &config)
{
    if (failure_ != Failure::None) return false;
    if (active_ || !session.sessionId || !session.gameId || !session.gameplayAbi ||
        session.playerCount < 2 || session.playerCount > MAX_PLAYERS ||
        session.localPlayer >= session.playerCount ||
        !config.connectTimeoutMs || !config.confirmedTimeoutMs ||
        !config.controlResendMs || !config.inputResendMs || !config.repairIntervalMs ||
        !config.bufferedLimit || !config.receiveBudget || config.receiveBudget > 4096 ||
        (previousSession_ && (session.sessionId == previousSession_ ||
         session.playerCount != previousCount_ || session.localPlayer != previousLocal_)))
        return Fail(Failure::InvalidConfiguration);
    if (!ObserveClock(nowMs)) return false;
    policy_ = config;
    pacing_.Reset();
    session_ = session;
    active_ = true;
    helloSent_ = false;
    forceControl_ = true;
    forceInputs_ = false;
    beginTime_ = nextControl_ = nextInput_ = nextRetirementFence_ = nowMs;
    latestCapture_ = INVALID_FRAME;
    sentSequence_.fill(0);
    receivedSequence_.fill(0);
    peerFrame_.fill(INVALID_FRAME);
    repairFrontier_.fill(INVALID_FRAME);
    lastRepair_.fill(nowMs);
    peerNeedsAck_.fill(false);
    for (auto &watchdog : watchdog_) watchdog.Disarm();
    return true;
}

bool SessionChannel::SendSession(const SessionGate &gate)
{
    if (!EncodeSessionPacket(gate.BuildPacket(SessionPhase::Hello), &outgoing_))
        return Fail(Failure::InvalidConfiguration);
    if (!transport_.SendControl(outgoing_.data(), outgoing_.size())) return true;
    ++sent_;
    helloSent_ = true;
    if (gate.LocalReady())
    {
        if (!EncodeSessionPacket(gate.BuildPacket(SessionPhase::Ready), &outgoing_))
            return Fail(Failure::InvalidConfiguration);
        if (transport_.SendControl(outgoing_.data(), outgoing_.size())) ++sent_;
    }
    return true;
}

bool SessionChannel::SendInputs(const RollbackCore &core, std::uint64_t nowMs, bool force)
{
    if (!active_ || !transport_.IsOpen() || latestCapture_ == INVALID_FRAME ||
        (!force && nowMs < nextInput_))
        return true;
    // Backpressure on the unreliable input lane must not also disable the
    // reliable repair lane. Otherwise two open RTC peers can both time out
    // with missing input while their control channels remain healthy. The
    // transport applies the repair lane's own small queue bound; retain its
    // interval below and keep ordinary sends blocked at the aggregate limit.
    const bool inputBackpressured = transport_.BufferedAmount() > policy_.bufferedLimit;
    for (std::uint8_t peer = 0; peer < session_.playerCount; ++peer)
    {
        if (peer == session_.localPlayer) continue;
        auto packet = core.BuildInputPacket(peer, core.LocalFrameForCapture(latestCapture_),
                                           ++sentSequence_[peer], receivedSequence_[peer]);
        const auto last = core.LastSimulatedFrame();
        packet.senderFrame = last == INVALID_FRAME ? 0 : last + 1;
        if (peerFrame_[peer] != INVALID_FRAME)
        {
            const auto difference = std::int64_t(packet.senderFrame) - peerFrame_[peer];
            packet.frameAdvantage = static_cast<std::int16_t>(
                std::max<std::int64_t>(-32768, std::min<std::int64_t>(32767, difference)));
        }
        if (!EncodeInputPacket(packet, &outgoing_)) return Fail(Failure::InvalidCapture);
        if (!inputBackpressured && transport_.SendTo(peer, outgoing_.data(), outgoing_.size())) ++sent_;

        // Keep normal input unordered. Only a stalled unacknowledged tail gets
        // a bounded reliable duplicate, so a final isolated dropped input can
        // recover even when neither title can advance enough to make new input.
        const auto frontier = core.AcknowledgedLocalThrough(peer);
        if (frontier != repairFrontier_[peer])
        {
            repairFrontier_[peer] = frontier;
            lastRepair_[peer] = nowMs;
        }
        // ACK-only packets also require repair when the other endpoint still
        // resends its tail. Otherwise a permanently lossy fast ACK direction
        // can deadlock retirement although both peers have all gameplay input.
        if ((packet.inputCount || peerNeedsAck_[peer]) &&
            nowMs - lastRepair_[peer] >= policy_.repairIntervalMs)
        {
            if (transport_.SendRepairTo(peer, outgoing_.data(), outgoing_.size()))
            {
                ++sent_;
                ++repairs_;
            }
            lastRepair_[peer] = nowMs;
        }
    }
    nextInput_ = nowMs + policy_.inputResendMs;
    forceInputs_ = false;
    return true;
}

bool SessionChannel::LocalCaptured(const RollbackCore &core, std::uint32_t captureFrame,
                                   std::uint64_t nowMs)
{
    if (failure_ != Failure::None) return false;
    if (!active_ || !core.HasLocalCapture(captureFrame) || captureFrame == INVALID_FRAME ||
        (latestCapture_ != INVALID_FRAME && captureFrame < latestCapture_))
        return Fail(Failure::InvalidCapture);
    if (!ObserveClock(nowMs)) return false;
    const bool fresh = latestCapture_ != captureFrame;
    latestCapture_ = captureFrame;
    return SendInputs(core, nowMs, fresh);
}

bool SessionChannel::FlushRetirementFence(const RollbackCore &core,
                                          std::uint32_t terminalFrame,
                                          std::uint64_t nowMs)
{
    if (failure_ != Failure::None) return false;
    if (!active_ || terminalFrame == INVALID_FRAME || latestCapture_ == INVALID_FRAME ||
        core.LastSimulatedFrame() != terminalFrame || core.HasRollbackRequest())
        return Fail(Failure::InvalidRetirement);
    if (!ObserveClock(nowMs)) return false;
    if (transport_.Failed()) return Fail(Failure::Transport);
    if (!transport_.IsOpen() || nowMs < nextRetirementFence_)
        return true;
    const bool inputBackpressured = transport_.BufferedAmount() > policy_.bufferedLimit;

    for (std::uint8_t peer = 0; peer < session_.playerCount; ++peer)
    {
        if (peer == session_.localPlayer) continue;
        auto packet = core.BuildInputPacket(peer, core.LocalFrameForCapture(latestCapture_),
                                           ++sentSequence_[peer], receivedSequence_[peer]);
        packet.senderFrame = terminalFrame + 1;
        if (peerFrame_[peer] != INVALID_FRAME)
        {
            const auto difference = std::int64_t(packet.senderFrame) - peerFrame_[peer];
            packet.frameAdvantage = static_cast<std::int16_t>(
                std::max<std::int64_t>(-32768, std::min<std::int64_t>(32767, difference)));
        }
        if (!EncodeInputPacket(packet, &outgoing_)) return Fail(Failure::InvalidCapture);
        if (transport_.SendRepairTo(peer, outgoing_.data(), outgoing_.size()))
        {
            ++sent_;
            ++repairs_;
        }
        else if (!inputBackpressured && transport_.SendTo(peer, outgoing_.data(), outgoing_.size()))
            ++sent_;
    }
    nextRetirementFence_ = nowMs + policy_.inputResendMs;
    return true;
}

bool SessionChannel::Receive(SessionGate &gate, RollbackCore &core, std::uint64_t nowMs)
{
    for (std::size_t count = 0; count < policy_.receiveBudget && transport_.Poll(&incoming_); ++count)
    {
        ++received_;
        PacketType type{};
        if (incoming_.size() > MaxPacketBytes ||
            !PeekPacketType(incoming_.data(), incoming_.size(), &type))
            return Fail(Failure::MalformedPacket);
        if (type == PacketType::Session)
        {
            SessionPacket packet{};
            if (!DecodeSessionPacket(incoming_.data(), incoming_.size(), &packet))
                return Fail(Failure::MalformedPacket);
            if (!active_ || packet.sessionId != session_.sessionId)
            {
                ++ignored_;
                continue;
            }
            const auto result = gate.Apply(packet);
            if (result == SessionPacketResult::ContractMismatch) return Fail(Failure::ContractMismatch);
            if (result == SessionPacketResult::InvalidPeer) return Fail(Failure::InvalidPeer);
            if (result == SessionPacketResult::ReadyBeforeHello) return Fail(Failure::MalformedPacket);
            if (packet.phase == SessionPhase::Hello)
            {
                if (previousSession_) previousNeeded_[packet.senderPlayer] = false;
                // Answer a peer which missed our READY even after we started.
                // Only a duplicate HELLO needs an immediate reply. Accepted
                // HELLOs are served below with the normal readiness transition.
                if (result == SessionPacketResult::Duplicate && gate.CanStart() && nowMs >= nextControl_)
                    forceControl_ = true;
            }
        }
        else if (type == PacketType::Input)
        {
            InputPacket packet{};
            if (!DecodeInputPacket(incoming_.data(), incoming_.size(), &packet))
                return Fail(Failure::MalformedPacket);
            if (!active_ || packet.sessionId != session_.sessionId)
            {
                ++ignored_;
                continue;
            }
            if (packet.senderPlayer >= session_.playerCount || packet.senderPlayer == session_.localPlayer)
                return Fail(Failure::InvalidPeer);
            if (packet.playerCount != session_.playerCount) return Fail(Failure::ContractMismatch);
            // Do not let an unbounded future packet overwrite still-live ring
            // slots. A valid peer is at most the prediction budget ahead; the
            // full history bound also admits conservative buffered adapters.
            const auto last = core.LastSimulatedFrame();
            const std::uint64_t next = last == INVALID_FRAME ? 0 : std::uint64_t(last) + 1;
            if (packet.inputCount && std::uint64_t(packet.latestFrame) >= next + INPUT_HISTORY_SIZE)
                return Fail(Failure::MalformedPacket);
            const auto localConfirmed = core.ConfirmedThrough(session_.localPlayer);
            if (packet.ackFrame != INVALID_FRAME &&
                (localConfirmed == INVALID_FRAME || packet.ackFrame > localConfirmed))
                return Fail(Failure::MalformedPacket);
            const auto before = core.ConfirmedThrough(packet.senderPlayer);
            RemoteInputResult result{};
            if (!core.ApplyInputPacket(packet, &result)) return Fail(Failure::InputConflict);
            if (before != core.ConfirmedThrough(packet.senderPlayer)) forceInputs_ = true;
            if (newer(packet.sequence, receivedSequence_[packet.senderPlayer]))
            {
                receivedSequence_[packet.senderPlayer] = packet.sequence;
                peerFrame_[packet.senderPlayer] = packet.senderFrame;
                peerNeedsAck_[packet.senderPlayer] = packet.inputCount != 0;
                // Only validated, newest packets can affect clock advice.
                // SessionPacing additionally ignores repeated sender frames,
                // so ACK-only retransmits cannot overweight a stalled peer.
                pacing_.Observe(static_cast<std::uint32_t>(next), packet);
            }
        }
        else return Fail(Failure::MalformedPacket);
    }
    (void)nowMs;
    return true;
}

bool SessionChannel::SendRetired(std::uint64_t nowMs)
{
    if (!previousSession_) return true;
    const bool needed = std::any_of(previousNeeded_.begin(), previousNeeded_.end(),
                                    [](bool value) { return value; });
    if (!needed)
    {
        previousSession_ = 0;
        for (auto &packet : terminalAcks_) packet.clear();
        return true;
    }
    if (nowMs < nextRetiredSend_ || !transport_.IsOpen()) return true;
    bool pending = false;
    for (std::uint8_t peer = 0; peer < previousCount_; ++peer)
    {
        if (peer == previousLocal_ || !previousNeeded_[peer]) continue;
        pending = true;
        const auto &packet = terminalAcks_[peer];
        if (transport_.SendRepairTo(peer, packet.data(), packet.size()))
        {
            ++sent_;
            ++repairs_;
        }
        else if (transport_.SendTo(peer, packet.data(), packet.size()))
        {
            // Relay already supplies a reliable stream and intentionally has
            // no separate RTC repair lane. Its targeted input path carries
            // the same terminal ACK without changing the wire protocol.
            ++sent_;
        }
    }
    if (!pending)
    {
        previousSession_ = 0;
        for (auto &packet : terminalAcks_) packet.clear();
    }
    nextRetiredSend_ = nowMs + policy_.inputResendMs;
    return true;
}

bool SessionChannel::Pump(SessionGate &gate, RollbackCore &core, std::uint64_t nowMs,
                          bool expectsInput)
{
    if (failure_ != Failure::None) return false;
    if (!ObserveClock(nowMs)) return false;
    if (transport_.Failed()) return Fail(Failure::Transport);
    if (active_ && (gate.Config().sessionId != session_.sessionId ||
        gate.Config().localPlayer != session_.localPlayer || gate.Config().playerCount != session_.playerCount))
        return Fail(Failure::InvalidConfiguration);
    if (!Receive(gate, core, nowMs) || !SendRetired(nowMs)) return false;
    if (!active_) return true;
    if (!gate.CanStart() && nowMs - beginTime_ >= policy_.connectTimeoutMs)
        return Fail(Failure::HandshakeTimeout);
    if (gate.CanSendReady() && !gate.LocalReady())
    {
        gate.MarkLocalReady();
        forceControl_ = true;
    }
    if (transport_.IsOpen() && (forceControl_ || (!gate.CanStart() && nowMs >= nextControl_)))
    {
        if (!SendSession(gate)) return false;
        forceControl_ = false;
        nextControl_ = nowMs + policy_.controlResendMs;
    }
    if (!SendInputs(core, nowMs, forceInputs_)) return false;
    for (std::uint8_t peer = 0; peer < session_.playerCount; ++peer)
    {
        if (peer == session_.localPlayer) continue;
        if (!expectsInput || !gate.CanStart() || latestCapture_ == INVALID_FRAME)
            watchdog_[peer].Disarm();
        else if (watchdog_[peer].Observe(core.ConfirmedThrough(peer), nowMs, policy_.confirmedTimeoutMs))
            return Fail(Failure::ConfirmedTimeout);
    }
    return true;
}

bool SessionChannel::CanRetire(const RollbackCore &core, std::uint32_t terminalFrame) const
{
    return active_ && failure_ == Failure::None && terminalFrame != INVALID_FRAME &&
           core.LastSimulatedFrame() == terminalFrame && !core.HasRollbackRequest() &&
           atLeast(core.ConfirmedThroughAllRemotes(), terminalFrame) &&
           atLeast(core.AcknowledgedLocalThroughAllRemotes(), terminalFrame);
}

bool SessionChannel::Retire(const RollbackCore &core, std::uint32_t terminalFrame,
                            std::uint64_t nowMs)
{
    if (!CanRetire(core, terminalFrame)) return false;
    if (!ObserveClock(nowMs)) return false;
    if (previousSession_) return Fail(Failure::InvalidRetirement);
    for (std::uint8_t peer = 0; peer < session_.playerCount; ++peer)
    {
        if (peer == session_.localPlayer) continue;
        InputPacket packet{};
        packet.sessionId = session_.sessionId;
        packet.senderPlayer = session_.localPlayer;
        packet.playerCount = session_.playerCount;
        packet.sequence = ++sentSequence_[peer];
        packet.ackSequence = receivedSequence_[peer];
        packet.latestFrame = packet.ackFrame = terminalFrame;
        packet.senderFrame = terminalFrame + 1;
        if (!EncodeInputPacket(packet, &terminalAcks_[peer])) return Fail(Failure::InvalidRetirement);
    }
    previousSession_ = session_.sessionId;
    previousCount_ = session_.playerCount;
    previousLocal_ = session_.localPlayer;
    previousNeeded_.fill(false);
    for (std::uint8_t peer = 0; peer < previousCount_; ++peer)
        previousNeeded_[peer] = peer != previousLocal_;
    nextRetiredSend_ = nowMs;
    active_ = false;
    for (auto &watchdog : watchdog_) watchdog.Disarm();
    return SendRetired(nowMs);
}

std::uint32_t SessionChannel::PeerFrame(std::uint8_t peer) const
{
    return peer < MAX_PLAYERS ? peerFrame_[peer] : INVALID_FRAME;
}

const char *SessionChannel::ErrorText() const
{
    switch (failure_)
    {
    case Failure::None: return "";
    case Failure::InvalidConfiguration: return "Invalid netplay session configuration";
    case Failure::Transport: return "Peer transport disconnected";
    case Failure::HandshakeTimeout: return "Peer session handshake timed out";
    case Failure::MalformedPacket: return "Invalid peer packet";
    case Failure::ContractMismatch: return "Peer gameplay contracts differ";
    case Failure::InvalidPeer: return "Invalid peer seat";
    case Failure::InputConflict: return "Peer changed a confirmed input";
    case Failure::ConfirmedTimeout: return "Confirmed peer input stopped advancing";
    case Failure::ClockReversed: return "Network clock moved backwards";
    case Failure::InvalidCapture: return "Invalid local input capture";
    case Failure::InvalidRetirement: return "Invalid netplay retirement fence";
    }
    return "Unknown netplay failure";
}
} // namespace Netplay
