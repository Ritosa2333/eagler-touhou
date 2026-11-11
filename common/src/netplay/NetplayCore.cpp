#include <eagler/netplay/NetplayCore.hpp>

#include <algorithm>

namespace Netplay
{
namespace
{
int ResultRank(RemoteInputResult result)
{
    switch (result)
    {
    case RemoteInputResult::ConflictingConfirmedInput: return 6;
    case RemoteInputResult::TooOld: return 5;
    case RemoteInputResult::RollbackRequired: return 4;
    case RemoteInputResult::PredictionCorrect: return 3;
    case RemoteInputResult::Accepted: return 2;
    case RemoteInputResult::Duplicate: return 1;
    case RemoteInputResult::InvalidPlayer: return 7;
    }
    return 0;
}
} // namespace

bool RollbackCore::Reset(const CoreConfig &config)
{
    Clear();
    if (config.playerCount < 2 || config.playerCount > MAX_PLAYERS ||
        config.localPlayer >= config.playerCount ||
        config.maxRollbackFrames == 0 ||
        config.maxRollbackFrames >= INPUT_HISTORY_SIZE / 2 ||
        config.inputDelay >= INPUT_HISTORY_SIZE / 4)
        return false;
    config_ = config;
    configured_ = true;
    for (std::uint8_t player = 0; player < config_.playerCount; ++player)
        confirmedThrough_[player] = INVALID_FRAME;

    // Input-delay lead-in is deterministic neutral input, not prediction.
    for (std::uint32_t frame = 0; frame < config_.inputDelay; ++frame)
    {
        InputSlot *slot = GetInputSlot(config_.localPlayer, frame);
        slot->input = {};
        slot->present = true;
    }
    AdvanceConfirmedThrough(config_.localPlayer);
    return true;
}

void RollbackCore::Clear()
{
    configured_ = false;
    config_ = CoreConfig{};
    for (auto &player : inputs_)
        for (auto &slot : player)
            slot = InputSlot{};
    for (auto &slot : used_)
        slot = UsedSlot{};
    confirmedThrough_.fill(INVALID_FRAME);
    directions_ = {};
    touchVelocity_ = {};
    peerAckOfLocal_.fill(INVALID_FRAME);
    lastSimulatedFrame_ = INVALID_FRAME;
    ClearRollbackRequest();
}

RollbackCore::InputSlot *RollbackCore::GetInputSlot(std::uint8_t player, std::uint32_t frame)
{
    InputSlot &slot = inputs_[player][frame % INPUT_HISTORY_SIZE];
    if (slot.frame != frame)
    {
        slot = InputSlot{};
        slot.frame = frame;
    }
    return &slot;
}

const RollbackCore::InputSlot *RollbackCore::FindInputSlot(std::uint8_t player,
                                                           std::uint32_t frame) const
{
    const InputSlot &slot = inputs_[player][frame % INPUT_HISTORY_SIZE];
    return slot.frame == frame && slot.present ? &slot : nullptr;
}

const RollbackCore::UsedSlot *RollbackCore::FindUsedSlot(std::uint32_t frame) const
{
    const UsedSlot &slot = used_[frame % INPUT_HISTORY_SIZE];
    return slot.frame == frame ? &slot : nullptr;
}

bool RollbackCore::HasConfirmedInputConflict(std::uint8_t player, std::uint32_t frame,
                                            const FrameInput &input) const
{
    if (const auto *existing = FindInputSlot(player, frame))
        return existing->input != input;
    const auto *used = FindUsedSlot(frame);
    return used && lastSimulatedFrame_ != INVALID_FRAME && frame <= lastSimulatedFrame_ &&
           !(used->predictedMask & (1u << player)) && used->inputs[player] != input;
}

bool RollbackCore::ScheduleLocalInput(std::uint32_t captureFrame, const FrameInput &input)
{
    const std::uint32_t frame = LocalFrameForCapture(captureFrame);
    if (frame == INVALID_FRAME)
        return false;
    InputSlot *slot = GetInputSlot(config_.localPlayer, frame);
    if (slot->present && slot->input != input)
        return false;
    slot->input = input;
    slot->present = true;
    AdvanceConfirmedThrough(config_.localPlayer);
    return true;
}

bool RollbackCore::SetLocalLeadInInput(std::uint32_t frame, const FrameInput &input)
{
    if (!configured_ || frame >= config_.inputDelay ||
        lastSimulatedFrame_ != INVALID_FRAME)
        return false;
    for (std::uint8_t peer = 0; peer < config_.playerCount; ++peer)
        if (peer != config_.localPlayer && peerAckOfLocal_[peer] != INVALID_FRAME)
            return false;
    InputSlot *slot = GetInputSlot(config_.localPlayer, frame);
    if (!slot->present || (slot->input != FrameInput{} && slot->input != input))
        return false;
    slot->input = input;
    return true;
}

std::uint32_t RollbackCore::LocalFrameForCapture(std::uint32_t captureFrame) const
{
    // INVALID_FRAME is a sentinel, never an addressable input frame.
    if (!configured_ || captureFrame >= INVALID_FRAME - config_.inputDelay)
        return INVALID_FRAME;
    return captureFrame + config_.inputDelay;
}

bool RollbackCore::HasLocalCapture(std::uint32_t captureFrame) const
{
    const auto target = LocalFrameForCapture(captureFrame);
    return target != INVALID_FRAME && FindInputSlot(config_.localPlayer, target) != nullptr;
}

bool RollbackCore::FrameIsTooOld(std::uint32_t frame) const
{
    return lastSimulatedFrame_ != INVALID_FRAME && frame <= lastSimulatedFrame_ &&
           lastSimulatedFrame_ - frame >= INPUT_HISTORY_SIZE;
}

RemoteInputResult RollbackCore::SubmitRemoteInput(std::uint8_t player, std::uint32_t frame,
                                                const FrameInput &input)
{
    if (!configured_ || player >= config_.playerCount || player == config_.localPlayer)
        return RemoteInputResult::InvalidPlayer;
    const auto previous = confirmedThrough_[player];
    const auto release = directions_[player].ReleaseFrame(previous);
    const auto result = StoreRemoteInput(player, frame, input);
    return NoteForecastChange(player, previous, release) &&
        result != RemoteInputResult::RollbackRequired && ForecastRollbackFrame(player) != INVALID_FRAME
        ? RemoteInputResult::RollbackRequired : result;
}

RemoteInputResult RollbackCore::StoreRemoteInput(std::uint8_t player, std::uint32_t frame,
                                               const FrameInput &input)
{
    if (FrameIsTooOld(frame))
        return RemoteInputResult::TooOld;
    if (HasConfirmedInputConflict(player, frame, input))
        return RemoteInputResult::ConflictingConfirmedInput;

    const auto &occupant = inputs_[player][frame % INPUT_HISTORY_SIZE];
    if (occupant.present && occupant.frame > frame &&
        confirmedThrough_[player] != INVALID_FRAME && frame <= confirmedThrough_[player])
        return RemoteInputResult::TooOld;

    InputSlot *slot = GetInputSlot(player, frame);
    if (slot->present)
        return RemoteInputResult::Duplicate;
    slot->input = input;
    slot->present = true;
    AdvanceConfirmedThrough(player);
    NoteActualInput(player, frame);

    const UsedSlot *used = FindUsedSlot(frame);
    const std::uint8_t mask = static_cast<std::uint8_t>(1u << player);
    auto result = RemoteInputResult::Accepted;
    if (used && lastSimulatedFrame_ != INVALID_FRAME && frame <= lastSimulatedFrame_)
    {
        if (!(used->predictedMask & mask)) result = RemoteInputResult::Duplicate;
        else if (used->inputs[player] == input) result = RemoteInputResult::PredictionCorrect;
        else
        {
            if (rollbackFrame_ == INVALID_FRAME || frame < rollbackFrame_)
                rollbackFrame_ = frame;
            return RemoteInputResult::RollbackRequired;
        }
    }
    return result;
}

EquivalentRemoteInputResult RollbackCore::SubmitEquivalentRemoteInput(std::uint8_t player,
                                                                       std::uint32_t frame,
                                                                       const FrameInput &input)
{
    if (!configured_ || player >= config_.playerCount || player == config_.localPlayer)
        return EquivalentRemoteInputResult::InvalidPlayer;
    if (FrameIsTooOld(frame))
        return EquivalentRemoteInputResult::TooOld;

    InputSlot *slot = GetInputSlot(player, frame);
    if (slot->present)
        return slot->input == input ? EquivalentRemoteInputResult::Duplicate
                                    : EquivalentRemoteInputResult::ConflictingConfirmedInput;

    const UsedSlot *used = FindUsedSlot(frame);
    if (!used || lastSimulatedFrame_ == INVALID_FRAME || frame > lastSimulatedFrame_)
        return EquivalentRemoteInputResult::NotPredicted;
    const std::uint8_t mask = static_cast<std::uint8_t>(1u << player);
    if ((used->predictedMask & mask) == 0)
        return EquivalentRemoteInputResult::NotPredicted;

    const auto previous = confirmedThrough_[player];
    const auto previousRelease = directions_[player].ReleaseFrame(previous);
    slot->input = input;
    slot->present = true;
    AdvanceConfirmedThrough(player);
    NoteActualInput(player, frame);
    NoteForecastChange(player, previous, previousRelease);
    return EquivalentRemoteInputResult::Confirmed;
}

bool RollbackCore::InputPresent(std::uint8_t player, std::uint32_t frame) const
{
    return configured_ && player < config_.playerCount && FindInputSlot(player, frame) != nullptr;
}

bool RollbackCore::UsedInput(std::uint8_t player, std::uint32_t frame, FrameInput *out,
                             bool *predicted) const
{
    if (!out || !configured_ || player >= config_.playerCount)
        return false;
    const UsedSlot *slot = FindUsedSlot(frame);
    if (!slot)
        return false;
    *out = slot->inputs[player];
    if (predicted)
        *predicted = (slot->predictedMask & static_cast<std::uint8_t>(1u << player)) != 0;
    return true;
}

std::uint32_t RollbackCore::ConfirmedThroughAllRemotes() const
{
    if (!configured_)
        return INVALID_FRAME;

    std::uint32_t confirmed = INVALID_FRAME;
    for (std::uint8_t player = 0; player < config_.playerCount; ++player)
    {
        if (player == config_.localPlayer)
            continue;
        const std::uint32_t value = ConfirmedThrough(player);
        if (value == INVALID_FRAME)
            return INVALID_FRAME;
        if (confirmed == INVALID_FRAME || value < confirmed)
            confirmed = value;
    }
    return confirmed;
}

FrameInput RollbackCore::HeldConfirmedInput(std::uint8_t player, std::uint32_t confirmed) const
{
    const auto* slot = confirmed != INVALID_FRAME ? FindInputSlot(player, confirmed) : nullptr;
    if (!slot) return {};
    auto held = slot->input;
    held.buttons &= config_.predictableButtons;
    held.touchBomb = false;
    if (held.analogMode == AnalogMode::DirectTouchDelta || held.analogMode == AnalogMode::DirectTouchBegin)
    {
        // Preserve the simulation's pending gesture remainder, add no new movement.
        held.analogMode = AnalogMode::DirectTouchDelta;
        held.x = held.y = 0;
    }
    else if (held.analogMode == AnalogMode::DirectTouch && !config_.directTouchIsAbsolute)
        held.x = held.y = 0;
    return held;
}

RollbackCore::TouchVelocity RollbackCore::ConfirmedTouchVelocity(std::uint8_t player, std::uint32_t frame) const
{
    if (frame < 4 || frame == INVALID_FRAME) return {};
    std::array<const InputSlot*, 5> path{};
    for (unsigned i=0; i<path.size(); ++i)
    {
        const auto* slot = FindInputSlot(player, frame-i);
        if (!slot || slot->input.analogMode != AnalogMode::DirectTouch || !slot->input.unlimited) return {};
        path[i] = slot;
    }
    for (unsigned i=0; i+1<path.size(); ++i)
    {
        const auto dx = path[i]->input.x - path[i+1]->input.x;
        const auto dy = path[i]->input.y - path[i+1]->input.y;
        if (!(dx*dx + dy*dy <= 64.f)) return {}; // Ignore a jump or new contact.
    }
    const auto &a=path[0]->input, &b=path[2]->input, &c=path[4]->input;
    const auto x=a.x-b.x, y=a.y-b.y, px=b.x-c.x, py=b.y-c.y;
    if (x*px + y*py <= 0) return {}; // A stop or turn falls back to hold.
    return {(x+px)*.25f, (y+py)*.25f};
}

FrameInput RollbackCore::ApplyTouchForecast(std::uint8_t player, FrameInput held, std::uint32_t distance) const
{
    const auto velocity = touchVelocity_[player];
    if (velocity.x == 0 && velocity.y == 0) return held;
    const auto steps = std::min<unsigned>(distance, config_.absoluteTouchForecastFrames);
    held.x = std::clamp(held.x + velocity.x*float(steps), -65536.f, 65536.f);
    held.y = std::clamp(held.y + velocity.y*float(steps), -65536.f, 65536.f);
    return held;
}

void RollbackCore::DirectionHistory::Observe(std::uint32_t frame, const FrameInput &input,
                                             std::uint16_t mask)
{
    if (input.analogMode != AnalogMode::None)
    {
        *this = {};
        return;
    }
    const auto direction = static_cast<std::uint16_t>(input.buttons & mask);
    if (direction == word) return;
    if (word)
    {
        if (frame - start <= 3) taps = {taps[1], taps[2], start};
        else taps.fill(INVALID_FRAME);
    }
    word = direction;
    start = frame;
}

std::uint32_t RollbackCore::DirectionHistory::ReleaseFrame(std::uint32_t confirmed) const
{
    // Three completed one-to-three-frame taps in the last half second.
    // Hold normally as soon as the new run is confirmed longer than a tap.
    return word && confirmed != INVALID_FRAME && start < INVALID_FRAME - 3 &&
        confirmed - start < 3 && taps[0] != INVALID_FRAME && confirmed - taps[0] < 32
        ? start + 3 : INVALID_FRAME;
}

FrameInput RollbackCore::ApplyDirectionRelease(FrameInput held, std::uint32_t frame,
                                               std::uint32_t release) const
{
    if (frame >= release)
        held.buttons &= static_cast<std::uint16_t>(~config_.directionButtons);
    return held;
}

bool RollbackCore::NoteForecastChange(std::uint8_t player, std::uint32_t previous,
                                      std::uint32_t previousRelease)
{
    if (config_.prediction != InputPrediction::ConfirmedHold) return false;
    const auto confirmed = confirmedThrough_[player];
    if (previous == confirmed || confirmed == INVALID_FRAME || lastSimulatedFrame_ == INVALID_FRAME ||
        lastSimulatedFrame_ <= confirmed || (rollbackFrame_ != INVALID_FRAME && rollbackFrame_ <= confirmed + 1))
        return false;
    const auto held = HeldConfirmedInput(player, confirmed);
    const auto release = directions_[player].ReleaseFrame(confirmed);
    if (release == previousRelease && held == HeldConfirmedInput(player, previous))
    {
        if (!config_.absoluteTouchForecastFrames || held.analogMode != AnalogMode::DirectTouch || !held.unlimited)
            return false;
        const auto before = ConfirmedTouchVelocity(player, previous), after = touchVelocity_[player];
        if (before.x == 0 && before.y == 0 && after.x == 0 && after.y == 0) return false;
    }
    forecastDirty_ |= static_cast<std::uint8_t>(1u << player);
    return true;
}

void RollbackCore::NoteActualInput(std::uint8_t player, std::uint32_t frame)
{
    if (forecastRollback_[player] == frame)
        forecastDirty_ |= static_cast<std::uint8_t>(1u << player);
}

std::uint32_t RollbackCore::ForecastRollbackFrame(std::uint8_t player) const
{
    const auto mask = static_cast<std::uint8_t>(1u << player);
    auto& candidate = forecastRollback_[player];
    if (!(forecastDirty_ & mask)) return candidate;
    forecastDirty_ &= static_cast<std::uint8_t>(~mask);
    candidate = INVALID_FRAME;
    const auto confirmed = confirmedThrough_[player];
    if (confirmed == INVALID_FRAME || lastSimulatedFrame_ == INVALID_FRAME || confirmed >= lastSimulatedFrame_)
        return candidate;
    const auto held = HeldConfirmedInput(player, confirmed);
    const auto release = directions_[player].ReleaseFrame(confirmed);
    // Resolve against the final confirmed prefix at the reconciliation
    // boundary. A matching exact packet can supersede a stale forecast.
    for (auto frame = confirmed + 1; frame <= lastSimulatedFrame_ && frame < rollbackFrame_; ++frame)
        if (const auto* used = FindUsedSlot(frame);
            used && (used->predictedMask & mask) &&
            !FindInputSlot(player, frame) &&
            used->inputs[player] != ApplyTouchForecast(player, ApplyDirectionRelease(held, frame, release), frame-confirmed))
        {
            candidate = frame;
            break;
        }
    return candidate;
}

std::uint32_t RollbackCore::RollbackFrame() const
{
    if (forecastDirty_)
        for (std::uint8_t player = 0; player < config_.playerCount; ++player)
            if (forecastDirty_ & (1u << player)) ForecastRollbackFrame(player);
    return std::min(rollbackFrame_, *std::min_element(forecastRollback_.begin(), forecastRollback_.end()));
}

void RollbackCore::ClearRollbackRequest()
{
    rollbackFrame_ = INVALID_FRAME;
    forecastRollback_.fill(INVALID_FRAME);
    forecastDirty_ = 0;
}

FrameInput RollbackCore::PredictInput(std::uint8_t player, std::uint32_t frame) const
{
    if (config_.prediction == InputPrediction::ConfirmedHold)
    {
        const auto confirmed = confirmedThrough_[player];
        return ApplyTouchForecast(player,
            ApplyDirectionRelease(HeldConfirmedInput(player, confirmed), frame,
                                  directions_[player].ReleaseFrame(confirmed)),
            confirmed == INVALID_FRAME ? 0 : frame-confirmed);
    }
    if (frame == 0)
        return {};
    const std::uint32_t search = std::min<std::uint32_t>(frame, INPUT_HISTORY_SIZE);
    for (std::uint32_t distance = 1; distance <= search; ++distance)
    {
        const InputSlot *slot = FindInputSlot(player, frame - distance);
        if (slot)
        {
            FrameInput predicted = slot->input;
            predicted.buttons &= config_.predictableButtons;
            if (distance > config_.maxDirectionPredictionFrames)
                predicted.buttons &= static_cast<std::uint16_t>(~config_.directionButtons);
            predicted.touchBomb = false;
            // Fresh delta streams already keep unapplied movement inside the
            // rewindable simulation. Predict no NEW displacement by default;
            // never repeat a gesture reset or add one device event twice.
            if (predicted.analogMode == AnalogMode::DirectTouchDelta ||
                predicted.analogMode == AnalogMode::DirectTouchBegin)
            {
                const bool holdDelta = predicted.analogMode == AnalogMode::DirectTouchDelta &&
                    distance <= config_.maxDirectTouchDeltaPredictionFrames;
                predicted.analogMode = AnalogMode::DirectTouchDelta;
                if (!holdDelta)
                    predicted.x = predicted.y = 0.0f;
            }
            // Joystick axes and buttons describe held state. Direct-touch axes
            // describe displacement consumed exactly once on that logical
            // frame; repeating the last delta during packet jitter makes the
            // remote ship race away before rollback corrects it.
            if (predicted.analogMode == AnalogMode::DirectTouch &&
                !config_.directTouchIsAbsolute && distance > 1)
            {
                predicted.x = 0.0f;
                predicted.y = 0.0f;
            }
            return predicted;
        }
    }
    return {};
}

FrameDecision RollbackCore::PrepareFrame(std::uint32_t frame) const
{
    FrameDecision decision;
    if (!configured_ || frame == INVALID_FRAME)
        return decision;

    for (std::uint8_t player = 0; player < config_.playerCount; ++player)
    {
        const std::uint32_t confirmed = confirmedThrough_[player];
        if (!config_.allowPrediction && (confirmed == INVALID_FRAME || confirmed < frame))
            return decision;
        if (player != config_.localPlayer)
        {
            // Exact input at this frame does not prove that the preceding
            // simulation is confirmed. A missing older sample still needs
            // every subsequent world/audio/Replay checkpoint for correction.
            // Bound that entire interval before accepting the exact slot too.
            const std::uint32_t distance = confirmed == INVALID_FRAME ? frame + 1 :
                frame > confirmed ? frame - confirmed : 0;
            if (distance > config_.maxRollbackFrames) return decision;
        }
        const InputSlot *slot = FindInputSlot(player, frame);
        if (slot)
        {
            decision.inputs[player] = slot->input;
            continue;
        }
        if (player == config_.localPlayer)
            return decision;

        if (confirmed != INVALID_FRAME && frame <= confirmed)
            return decision; // a hole inside confirmed history means corruption
        decision.inputs[player] = PredictInput(player, frame);
        decision.predictedMask |= static_cast<std::uint8_t>(1u << player);
    }
    decision.canAdvance = true;
    return decision;
}

bool RollbackCore::MarkSimulated(std::uint32_t frame, const FrameDecision &decision)
{
    if (!configured_ || !decision.canAdvance)
        return false;
    if (!config_.allowPrediction)
    {
        const auto exact = PrepareFrame(frame);
        const auto next = lastSimulatedFrame_ == INVALID_FRAME ? 0 : lastSimulatedFrame_ + 1;
        if (frame != next || decision.predictedMask || !exact.canAdvance ||
            decision.inputs != exact.inputs) return false;
    }
    UsedSlot &slot = used_[frame % INPUT_HISTORY_SIZE];
    slot.frame = frame;
    slot.inputs = decision.inputs;
    slot.predictedMask = decision.predictedMask;
    if (lastSimulatedFrame_ == INVALID_FRAME || frame > lastSimulatedFrame_)
        lastSimulatedFrame_ = frame;
    return true;
}

bool RollbackCore::RewindSimulationTo(std::uint32_t firstFrame)
{
    if (!configured_ || !config_.allowPrediction || firstFrame == INVALID_FRAME ||
        lastSimulatedFrame_ == INVALID_FRAME || firstFrame > lastSimulatedFrame_ ||
        lastSimulatedFrame_ - firstFrame >= INPUT_HISTORY_SIZE ||
        firstFrame > RollbackFrame())
        return false;
    // Validate the complete interval before mutating anything. MarkSimulated
    // is also used by diagnostic adapters, so do not assume they marked a
    // contiguous interval merely because a last-frame number exists.
    for (std::uint32_t frame = firstFrame; ; ++frame)
    {
        if (!FindUsedSlot(frame))
            return false;
        if (frame == lastSimulatedFrame_)
            break;
    }
    // The validated interval is the whole discarded tail. Keep older
    // decisions and avoid scanning the 256-slot ring on a short rewind.
    for (std::uint32_t frame = firstFrame; ; ++frame)
    {
        used_[frame % INPUT_HISTORY_SIZE] = UsedSlot{};
        if (frame == lastSimulatedFrame_)
            break;
    }
    lastSimulatedFrame_ = firstFrame == 0 ? INVALID_FRAME : firstFrame - 1;
    ClearRollbackRequest();
    return true;
}

void RollbackCore::AdvanceConfirmedThrough(std::uint8_t player)
{
    const auto previous = confirmedThrough_[player];
    std::uint32_t next = confirmedThrough_[player] == INVALID_FRAME
                             ? 0
                             : confirmedThrough_[player] + 1;
    while (const auto* slot = FindInputSlot(player, next))
    {
        if (config_.allowPrediction && config_.prediction == InputPrediction::ConfirmedHold &&
            config_.directionButtons && player != config_.localPlayer)
            directions_[player].Observe(next, slot->input, config_.directionButtons);
        confirmedThrough_[player] = next;
        if (next == INVALID_FRAME - 1)
            break;
        ++next;
    }
    if (config_.allowPrediction && config_.prediction == InputPrediction::ConfirmedHold &&
        config_.directTouchIsAbsolute && config_.absoluteTouchForecastFrames &&
        player != config_.localPlayer && previous != confirmedThrough_[player])
        touchVelocity_[player] = ConfirmedTouchVelocity(player, confirmedThrough_[player]);
}

std::uint32_t RollbackCore::ConfirmedThrough(std::uint8_t player) const
{
    return configured_ && player < config_.playerCount ? confirmedThrough_[player] : INVALID_FRAME;
}

bool RollbackCore::ConfirmedInputs(std::uint32_t frame,
                                   std::array<FrameInput, MAX_PLAYERS> *out) const
{
    if (!configured_ || !out || frame == INVALID_FRAME ||
        lastSimulatedFrame_ == INVALID_FRAME || frame > lastSimulatedFrame_ ||
        RollbackFrame() <= frame) return false;
    const auto *used = FindUsedSlot(frame);
    if (!used) return false;
    std::array<FrameInput, MAX_PLAYERS> candidate{};
    for (std::uint8_t seat = 0; seat < config_.playerCount; ++seat)
    {
        if (confirmedThrough_[seat] == INVALID_FRAME || confirmedThrough_[seat] < frame)
            return false;
        const auto *input = FindInputSlot(seat, frame);
        if (!input || input->input != used->inputs[seat]) return false;
        candidate[seat] = input->input;
    }
    *out = candidate; return true;
}

FrameInput RollbackCore::LocalInput(std::uint32_t frame, bool *present) const
{
    const InputSlot *slot = configured_ ? FindInputSlot(config_.localPlayer, frame) : nullptr;
    if (present)
        *present = slot != nullptr;
    return slot ? slot->input : FrameInput{};
}

std::uint32_t RollbackCore::AcknowledgedLocalThrough(std::uint8_t peer) const
{
    if (!configured_ || peer >= config_.playerCount || peer == config_.localPlayer)
        return INVALID_FRAME;
    return peerAckOfLocal_[peer];
}

std::uint32_t RollbackCore::AcknowledgedLocalThroughAllRemotes() const
{
    if (!configured_) return INVALID_FRAME;
    std::uint32_t minimum = INVALID_FRAME;
    for (std::uint8_t peer = 0; peer < config_.playerCount; ++peer)
    {
        if (peer == config_.localPlayer) continue;
        if (peerAckOfLocal_[peer] == INVALID_FRAME) return INVALID_FRAME;
        minimum = std::min(minimum, peerAckOfLocal_[peer]);
    }
    return minimum;
}

InputPacket RollbackCore::BuildInputPacket(std::uint8_t peer, std::uint32_t latestFrame,
                                            std::uint32_t sequence, std::uint32_t ackSequence) const
{
    InputPacket packet;
    if (!configured_ || peer >= config_.playerCount || peer == config_.localPlayer)
        return packet;
    packet.sessionId = config_.sessionId;
    packet.sequence = sequence;
    packet.ackSequence = ackSequence;
    packet.senderPlayer = config_.localPlayer;
    packet.playerCount = config_.playerCount;
    packet.latestFrame = latestFrame;
    packet.ackFrame = confirmedThrough_[peer];

    const std::uint32_t peerAck = peerAckOfLocal_[peer];
    std::uint32_t first = peerAck == INVALID_FRAME ? 0 : peerAck + 1;
    if (peerAck == INVALID_FRAME && latestFrame + 1 > MAX_REDUNDANT_INPUTS)
        first = latestFrame + 1 - MAX_REDUNDANT_INPUTS;
    if (first > latestFrame)
    {
        packet.firstInputFrame = INVALID_FRAME;
        packet.inputCount = 0;
        return packet;
    }
    packet.firstInputFrame = first;
    for (std::uint32_t frame = first; frame <= latestFrame &&
         packet.inputCount < MAX_REDUNDANT_INPUTS; ++frame)
    {
        const InputSlot *slot = FindInputSlot(config_.localPlayer, frame);
        if (!slot)
            break;
        packet.inputs[packet.inputCount++] = slot->input;
    }
    if (packet.inputCount == 0)
        packet.firstInputFrame = INVALID_FRAME;
    else
        packet.latestFrame = packet.firstInputFrame + packet.inputCount - 1;
    return packet;
}

bool RollbackCore::ApplyInputPacket(const InputPacket &packet, RemoteInputResult *worstResult)
{
    if (worstResult) *worstResult = RemoteInputResult::InvalidPlayer;
    if (!configured_ || packet.sessionId != config_.sessionId ||
        packet.playerCount != config_.playerCount ||
        packet.senderPlayer >= config_.playerCount ||
        packet.senderPlayer == config_.localPlayer ||
        packet.inputCount > MAX_REDUNDANT_INPUTS)
        return false;
    if (packet.inputCount == 0)
    {
        if (packet.firstInputFrame != INVALID_FRAME)
            return false;
    }
    else if (packet.firstInputFrame == INVALID_FRAME || packet.latestFrame == INVALID_FRAME ||
             packet.firstInputFrame > INVALID_FRAME - (packet.inputCount - 1) ||
             packet.firstInputFrame + packet.inputCount - 1 != packet.latestFrame)
        return false;
    // Validate the entire transaction before advancing any ACK, input or
    // rollback frontier. A conflict in the last redundant sample must not
    // silently commit the earlier samples or acknowledge uncaptured input.
    if (packet.ackFrame != INVALID_FRAME &&
        (confirmedThrough_[config_.localPlayer] == INVALID_FRAME ||
         packet.ackFrame > confirmedThrough_[config_.localPlayer]))
        return false;
    const std::uint64_t nextFrame = lastSimulatedFrame_ == INVALID_FRAME
        ? 0 : std::uint64_t(lastSimulatedFrame_) + 1;
    if (packet.inputCount && std::uint64_t(packet.latestFrame) >= nextFrame + INPUT_HISTORY_SIZE)
        return false;
    for (std::uint8_t i = 0; i < packet.inputCount; ++i)
    {
        if (!IsValidFrameInput(packet.inputs[i])) return false;
        const auto frame = packet.firstInputFrame + i;
        if (FrameIsTooOld(frame)) continue; // benign expired retransmission
        if (HasConfirmedInputConflict(packet.senderPlayer, frame, packet.inputs[i]))
        {
            if (worstResult) *worstResult = RemoteInputResult::ConflictingConfirmedInput;
            return false;
        }
        const auto &occupant = inputs_[packet.senderPlayer][frame % INPUT_HISTORY_SIZE];
        // An unordered retransmission may arrive after a newer frame reused
        // its slot. The confirmed prefix already accounted for that sample.
        if (occupant.present && occupant.frame > frame &&
            confirmedThrough_[packet.senderPlayer] != INVALID_FRAME &&
            frame <= confirmedThrough_[packet.senderPlayer])
            continue;
        const auto oldestRecoverable = lastSimulatedFrame_ == INVALID_FRAME ||
            lastSimulatedFrame_ < config_.maxRollbackFrames ? 0 :
            lastSimulatedFrame_ - config_.maxRollbackFrames + 1;
        if (occupant.present && occupant.frame != frame &&
            (occupant.frame >= oldestRecoverable ||
             occupant.frame == confirmedThrough_[packet.senderPlayer]))
            return false;
    }
    if (packet.ackFrame != INVALID_FRAME)
    {
        std::uint32_t &ack = peerAckOfLocal_[packet.senderPlayer];
        if (ack == INVALID_FRAME || packet.ackFrame > ack)
            ack = packet.ackFrame;
    }

    RemoteInputResult worst = RemoteInputResult::Duplicate;
    const auto previous = confirmedThrough_[packet.senderPlayer];
    const auto release = directions_[packet.senderPlayer].ReleaseFrame(previous);
    for (std::uint8_t i = 0; i < packet.inputCount; ++i)
    {
        const RemoteInputResult result = StoreRemoteInput(
            packet.senderPlayer, packet.firstInputFrame + i, packet.inputs[i]);
        if (ResultRank(result) > ResultRank(worst))
            worst = result;
        if (result == RemoteInputResult::ConflictingConfirmedInput ||
            result == RemoteInputResult::InvalidPlayer)
        {
            if (worstResult)
                *worstResult = result;
            return false;
        }
    }
    // Leave forecast-only corrections cancellable across the receive burst.
    if (NoteForecastChange(packet.senderPlayer, previous, release) && worstResult &&
        ResultRank(worst) < ResultRank(RemoteInputResult::RollbackRequired) &&
        ForecastRollbackFrame(packet.senderPlayer) != INVALID_FRAME)
        worst = RemoteInputResult::RollbackRequired;
    if (worstResult)
        *worstResult = worst;
    return true;
}
} // namespace Netplay
