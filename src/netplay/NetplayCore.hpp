#pragma once

#include "NetplayProtocol.hpp"

#include <array>
#include <cstdint>

namespace Netplay
{
constexpr std::size_t INPUT_HISTORY_SIZE = 256;

struct CoreConfig
{
    std::uint64_t sessionId = 0;
    std::uint8_t playerCount = 2;
    std::uint8_t localPlayer = 0;
    std::uint8_t inputDelay = 0;
    std::uint8_t maxRollbackFrames = 8;
    // Game-defined held buttons that are safe to repeat while a remote frame
    // is missing. Edge actions such as Bomb/Menu must stay released until the
    // exact sample arrives and requests a rollback.
    std::uint16_t predictableButtons = 0xffffu;
    // Movement is safe to repeat briefly, but a long missing-input burst must
    // not keep driving a remote ship for the entire rollback window.  This is
    // independent from maxRollbackFrames: prediction/correction can continue
    // after movement becomes neutral.
    std::uint16_t directionButtons = 0;
    std::uint8_t maxDirectionPredictionFrames = 0xffu;
};

struct FrameDecision
{
    bool canAdvance = false;
    std::uint8_t predictedMask = 0;
    std::array<FrameInput, MAX_PLAYERS> inputs{};
};

enum class RemoteInputResult
{
    Accepted,
    Duplicate,
    PredictionCorrect,
    RollbackRequired,
    ConflictingConfirmedInput,
    TooOld,
    InvalidPlayer,
};

class RollbackCore
{
public:
    bool Reset(const CoreConfig &config);
    void Clear();

    // Physical input captured on captureFrame becomes the local input for
    // captureFrame + inputDelay. Frames introduced by delay are neutral.
    bool ScheduleLocalInput(std::uint32_t captureFrame, const FrameInput &input);
    bool ScheduleLocalInput(std::uint32_t captureFrame, std::uint16_t bits)
    {
        return ScheduleLocalInput(captureFrame, FrameInput(bits));
    }
    RemoteInputResult SubmitRemoteInput(std::uint8_t player, std::uint32_t frame,
                                        const FrameInput &input);
    RemoteInputResult SubmitRemoteInput(std::uint8_t player, std::uint32_t frame,
                                        std::uint16_t bits)
    {
        return SubmitRemoteInput(player, frame, FrameInput(bits));
    }

    // Returns prediction using the last confirmed input for a missing remote
    // player. If a peer is farther behind than maxRollbackFrames the core
    // stalls instead of predicting an unrecoverable frame.
    FrameDecision PrepareFrame(std::uint32_t frame) const;
    bool MarkSimulated(std::uint32_t frame, const FrameDecision &decision);

    bool HasRollbackRequest() const { return rollbackFrame_ != INVALID_FRAME; }
    std::uint32_t RollbackFrame() const { return rollbackFrame_; }
    void ClearRollbackRequest() { rollbackFrame_ = INVALID_FRAME; }

    std::uint32_t ConfirmedThrough(std::uint8_t player) const;
    std::uint32_t LastSimulatedFrame() const { return lastSimulatedFrame_; }
    FrameInput LocalInput(std::uint32_t frame, bool *present = nullptr) const;

    InputPacket BuildInputPacket(std::uint8_t peer, std::uint32_t latestFrame,
                                 std::uint32_t sequence, std::uint32_t ackSequence) const;
    bool ApplyInputPacket(const InputPacket &packet, RemoteInputResult *worstResult = nullptr);

private:
    struct InputSlot
    {
        std::uint32_t frame = INVALID_FRAME;
        FrameInput input{};
        bool present = false;
    };

    struct UsedSlot
    {
        std::uint32_t frame = INVALID_FRAME;
        std::array<FrameInput, MAX_PLAYERS> inputs{};
        std::uint8_t predictedMask = 0;
    };

    InputSlot *GetInputSlot(std::uint8_t player, std::uint32_t frame);
    const InputSlot *FindInputSlot(std::uint8_t player, std::uint32_t frame) const;
    const UsedSlot *FindUsedSlot(std::uint32_t frame) const;
    FrameInput PredictInput(std::uint8_t player, std::uint32_t frame) const;
    void AdvanceConfirmedThrough(std::uint8_t player);
    bool FrameIsTooOld(std::uint32_t frame) const;

    CoreConfig config_{};
    bool configured_ = false;
    std::array<std::array<InputSlot, INPUT_HISTORY_SIZE>, MAX_PLAYERS> inputs_{};
    std::array<UsedSlot, INPUT_HISTORY_SIZE> used_{};
    std::array<std::uint32_t, MAX_PLAYERS> confirmedThrough_{};
    std::array<std::uint32_t, MAX_PLAYERS> peerAckOfLocal_{};
    std::uint32_t lastSimulatedFrame_ = INVALID_FRAME;
    std::uint32_t rollbackFrame_ = INVALID_FRAME;
};
} // namespace Netplay
