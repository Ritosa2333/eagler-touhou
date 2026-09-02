#include "NetplayCore.hpp"

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
    peerAckOfLocal_.fill(INVALID_FRAME);
    lastSimulatedFrame_ = INVALID_FRAME;
    rollbackFrame_ = INVALID_FRAME;
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

bool RollbackCore::ScheduleLocalInput(std::uint32_t captureFrame, const FrameInput &input)
{
    if (!configured_ || captureFrame > INVALID_FRAME - config_.inputDelay)
        return false;
    const std::uint32_t frame = captureFrame + config_.inputDelay;
    InputSlot *slot = GetInputSlot(config_.localPlayer, frame);
    if (slot->present && slot->input != input)
        return false;
    slot->input = input;
    slot->present = true;
    AdvanceConfirmedThrough(config_.localPlayer);
    return true;
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
    if (FrameIsTooOld(frame))
        return RemoteInputResult::TooOld;

    InputSlot *slot = GetInputSlot(player, frame);
    if (slot->present)
        return slot->input == input ? RemoteInputResult::Duplicate
                                    : RemoteInputResult::ConflictingConfirmedInput;
    slot->input = input;
    slot->present = true;
    AdvanceConfirmedThrough(player);

    const UsedSlot *used = FindUsedSlot(frame);
    if (!used || lastSimulatedFrame_ == INVALID_FRAME || frame > lastSimulatedFrame_)
        return RemoteInputResult::Accepted;

    const std::uint8_t mask = static_cast<std::uint8_t>(1u << player);
    if ((used->predictedMask & mask) == 0)
        return used->inputs[player] == input ? RemoteInputResult::Duplicate
                                             : RemoteInputResult::ConflictingConfirmedInput;
    if (used->inputs[player] == input)
        return RemoteInputResult::PredictionCorrect;

    if (rollbackFrame_ == INVALID_FRAME || frame < rollbackFrame_)
        rollbackFrame_ = frame;
    return RemoteInputResult::RollbackRequired;
}

FrameInput RollbackCore::PredictInput(std::uint8_t player, std::uint32_t frame) const
{
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
            // Joystick axes and buttons describe held state. Direct-touch axes
            // describe displacement consumed exactly once on that logical
            // frame; repeating the last delta during packet jitter makes the
            // remote ship race away before rollback corrects it.
            if (predicted.analogMode == AnalogMode::DirectTouch && distance > 1)
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
    if (!configured_)
        return decision;

    for (std::uint8_t player = 0; player < config_.playerCount; ++player)
    {
        const InputSlot *slot = FindInputSlot(player, frame);
        if (slot)
        {
            decision.inputs[player] = slot->input;
            continue;
        }
        if (player == config_.localPlayer)
            return decision;

        const std::uint32_t confirmed = confirmedThrough_[player];
        if (confirmed != INVALID_FRAME && frame <= confirmed)
            return decision; // a hole inside confirmed history means corruption
        const std::uint32_t predictionDistance =
            confirmed == INVALID_FRAME ? frame + 1 : frame - confirmed;
        if (predictionDistance > config_.maxRollbackFrames)
            return decision;
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
    UsedSlot &slot = used_[frame % INPUT_HISTORY_SIZE];
    slot.frame = frame;
    slot.inputs = decision.inputs;
    slot.predictedMask = decision.predictedMask;
    if (lastSimulatedFrame_ == INVALID_FRAME || frame > lastSimulatedFrame_)
        lastSimulatedFrame_ = frame;
    return true;
}

void RollbackCore::AdvanceConfirmedThrough(std::uint8_t player)
{
    std::uint32_t next = confirmedThrough_[player] == INVALID_FRAME
                             ? 0
                             : confirmedThrough_[player] + 1;
    while (FindInputSlot(player, next))
    {
        confirmedThrough_[player] = next;
        if (next == INVALID_FRAME - 1)
            break;
        ++next;
    }
}

std::uint32_t RollbackCore::ConfirmedThrough(std::uint8_t player) const
{
    return configured_ && player < config_.playerCount ? confirmedThrough_[player] : INVALID_FRAME;
}

FrameInput RollbackCore::LocalInput(std::uint32_t frame, bool *present) const
{
    const InputSlot *slot = configured_ ? FindInputSlot(config_.localPlayer, frame) : nullptr;
    if (present)
        *present = slot != nullptr;
    return slot ? slot->input : FrameInput{};
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
    if (packet.ackFrame != INVALID_FRAME)
    {
        std::uint32_t &ack = peerAckOfLocal_[packet.senderPlayer];
        if (ack == INVALID_FRAME || packet.ackFrame > ack)
            ack = packet.ackFrame;
    }

    RemoteInputResult worst = RemoteInputResult::Duplicate;
    for (std::uint8_t i = 0; i < packet.inputCount; ++i)
    {
        const RemoteInputResult result = SubmitRemoteInput(
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
    if (worstResult)
        *worstResult = worst;
    return true;
}
} // namespace Netplay
