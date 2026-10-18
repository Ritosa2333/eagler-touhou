#pragma once
#include <eagler/netplay/SpectatorStageProtocol.hpp>
#include <array>

namespace Netplay {
// Record once for EVERY executed frame, including resimulation. The title
// supplies only a scalar initializer captured before its first stage input and
// Boss registrations observed during this tick. Publication follows reconcile.
class SpectatorStagePublisher {
public:
    static constexpr unsigned Capacity = 256;
    enum class CheckpointRequirement : std::uint8_t {
        Required,
        NotRequired,
        AlreadyPublished,
        MissingPredecessor,
    };
    enum class PublishFailure : std::uint8_t {
        None,
        InvalidPublisher,
        MissingRecord,
        StageCheckpointMissing,
        CheckpointEncode,
        CheckpointSend,
        InputUnavailable,
        InputEncode,
        InputSend,
        BossNodeEncode,
        BossNodeSend,
    };
    static const char* FailureName(PublishFailure failure) {
        switch (failure) {
            case PublishFailure::None: return "none";
            case PublishFailure::InvalidPublisher: return "invalid-publisher";
            case PublishFailure::MissingRecord: return "missing-record";
            case PublishFailure::StageCheckpointMissing: return "stage-checkpoint-missing";
            case PublishFailure::CheckpointEncode: return "checkpoint-encode";
            case PublishFailure::CheckpointSend: return "checkpoint-send";
            case PublishFailure::InputUnavailable: return "input-unavailable";
            case PublishFailure::InputEncode: return "input-encode";
            case PublishFailure::InputSend: return "input-send";
            case PublishFailure::BossNodeEncode: return "boss-node-encode";
            case PublishFailure::BossNodeSend: return "boss-node-send";
        }
        return "unknown";
    }
    struct Frame {
        std::uint32_t frame = INVALID_FRAME, stage = 0, schema = 0;
        std::vector<std::uint8_t> initializer;
        std::array<FrameInput, MAX_PLAYERS> inputs{};
        bool hasInputs = false;
        std::uint8_t bossSlots = 0;
    };
    bool Reset(std::uint8_t game, std::uint8_t count, std::uint64_t session, std::uint32_t abi,
               std::uint32_t firstFrame = 0) {
        SpectatorStageEvent check; check.game = game; check.playerCount = count;
        check.sessionId = session; check.gameplayAbi = abi; check.frame = firstFrame;
        check.serial = check.schema = 1; check.state = {1};
        if (!SpectatorStageWire::Valid(check)) return false;
        *this = {}; identity_ = check; identity_.state.clear(); identity_.schema = identity_.serial = 0;
        firstFrame_ = next_ = firstFrame;
        return true;
    }
    // Decide from the logical predecessor record, not a title-side mutable
    // current-stage cache. A rollback can replay a stage's first frame after
    // that cache has already advanced. The caller must recapture the complete
    // initializer whenever this returns Required; a missing predecessor is
    // explicit so the caller can retire the optional stream safely.
    CheckpointRequirement NeedsCheckpoint(std::uint32_t frame, std::uint32_t stage) const {
        if (!identity_.sessionId || frame == INVALID_FRAME || stage > 65535 || frame < firstFrame_)
            return CheckpointRequirement::MissingPredecessor;
        if (frame < next_) return CheckpointRequirement::AlreadyPublished;
        if (frame - next_ >= Capacity)
            return CheckpointRequirement::MissingPredecessor;
        if (frame == firstFrame_) return CheckpointRequirement::Required;
        // A cached record beyond a gap is not evidence about this frame's
        // actual predecessor. Require the pending record prefix to be
        // contiguous through frame-1 before consulting its stage.
        for (std::uint32_t pending = next_; pending < frame; ++pending) {
            if (records_[pending % Capacity].frame != pending)
                return CheckpointRequirement::MissingPredecessor;
        }
        const auto& predecessor = records_[(frame - 1) % Capacity];
        if (predecessor.frame != frame - 1)
            return CheckpointRequirement::MissingPredecessor;
        return predecessor.stage == stage ? CheckpointRequirement::NotRequired
                                          : CheckpointRequirement::Required;
    }
    bool Record(Frame frame) {
        if (!identity_.sessionId || ended_ || frame.frame == INVALID_FRAME || frame.stage > 65535 ||
            frame.initializer.size() > SpectatorStageMaxStateBytes ||
            bool(frame.schema) != !frame.initializer.empty()) return false;
        if (frame.frame < next_) {
            // Restoring an older journal checkpoint may replay an already
            // confirmed prefix. It cannot amend published history. Retain one
            // ring of lanes to distinguish this from a real input revision.
            const auto& old = records_[frame.frame % Capacity];
            return old.frame != frame.frame || SameInputs(old, frame);
        }
        if (frame.frame - next_ >= Capacity) return false;
        if (frame.frame == next_ && phase_) {
            const auto& old = records_[frame.frame % Capacity];
            return old.frame == frame.frame && old.stage == frame.stage && old.schema == frame.schema &&
                old.initializer == frame.initializer && old.bossSlots == frame.bossSlots && SameInputs(old, frame);
        }
        records_[frame.frame % Capacity] = std::move(frame); return true;
    }
    template<class Inputs, class Send>
    bool Publish(std::uint32_t confirmed, std::uint32_t simulated, Inputs inputs, Send send,
                 PublishFailure *failure = nullptr) {
        if (failure) *failure = PublishFailure::None;
        if (!identity_.sessionId || ended_)
            return Fail(failure, PublishFailure::InvalidPublisher);
        if (confirmed == INVALID_FRAME || simulated == INVALID_FRAME) return true;
        const auto last = std::min(confirmed, simulated);
        while (next_ <= last) {
            const auto& record = records_[next_ % Capacity];
            if (record.frame != next_) return Fail(failure, PublishFailure::MissingRecord);
            if (phase_ == 0) {
                if (!record.initializer.empty()) {
                    auto event = identity_; event.frame = next_; event.stage = record.stage;
                    event.kind = SpectatorEventKind::Checkpoint; event.serial = serial_ + 1;
                    event.schema = record.schema; event.state = record.initializer;
                    if (!Event(event, send, PublishFailure::CheckpointEncode,
                               PublishFailure::CheckpointSend, failure)) return false;
                    checkpoint_ = serial_; stage_ = record.stage;
                } else if (!checkpoint_ || record.stage != stage_)
                    return Fail(failure, PublishFailure::StageCheckpointMissing);
                phase_ = 1;
            }
            if (phase_ == 1) {
                SpectatorFramePacket packet; packet.sessionId = identity_.sessionId;
                packet.gameplayAbi = identity_.gameplayAbi; packet.playerCount = identity_.playerCount; packet.frame = next_;
                std::vector<std::uint8_t> wire;
                if (record.hasInputs) packet.inputs = record.inputs;
                else if (!inputs(next_, packet.inputs))
                    return Fail(failure, PublishFailure::InputUnavailable);
                if (!EncodeSpectatorFramePacket(packet, &wire))
                    return Fail(failure, PublishFailure::InputEncode);
                if (!send(wire)) return Fail(failure, PublishFailure::InputSend);
                phase_ = 2;
            }
            if (record.bossSlots) {
                auto event = identity_; event.kind = SpectatorEventKind::Node;
                event.frame = next_ + 1; event.stage = stage_; event.serial = serial_ + 1;
                event.checkpoint = checkpoint_; event.node = SpectatorNodeKind::Boss; event.bossSlots = record.bossSlots;
                if (!Event(event, send, PublishFailure::BossNodeEncode,
                           PublishFailure::BossNodeSend, failure)) return false;
            }
            ++next_; phase_ = 0;
        }
        return true;
    }
    template<class Send> bool Finish(std::uint32_t endFrame, Send send) {
        if (ended_ || !checkpoint_ || phase_ || endFrame != next_) return false;
        auto event = identity_; event.kind = SpectatorEventKind::End; event.frame = next_;
        event.stage = stage_; event.serial = serial_ + 1;
        if (!Event(event, send, PublishFailure::None, PublishFailure::None, nullptr)) return false;
        ended_ = true; return true;
    }
    std::uint32_t Published() const {return next_;}
    std::uint32_t Serial() const {return serial_;}
private:
    static bool Fail(PublishFailure *failure, PublishFailure reason) {
        if (failure) *failure = reason;
        return false;
    }
    bool SameInputs(const Frame& a, const Frame& b) const {
        if (!a.hasInputs || !b.hasInputs) return true;
        SpectatorFramePacket packet; packet.sessionId=identity_.sessionId;
        packet.gameplayAbi=identity_.gameplayAbi; packet.playerCount=identity_.playerCount; packet.frame=a.frame;
        std::vector<std::uint8_t> left,right; packet.inputs=a.inputs;
        if (!EncodeSpectatorFramePacket(packet,&left)) return false;
        packet.inputs=b.inputs; return EncodeSpectatorFramePacket(packet,&right) && left==right;
    }
    template<class Send>
    bool Event(const SpectatorStageEvent& event, Send send,
               PublishFailure encodeFailure, PublishFailure sendFailure,
               PublishFailure *failure) {
        if (!event.serial) return Fail(failure, encodeFailure);
        std::vector<std::uint8_t> wire;
        if (!EncodeSpectatorStageEvent(event, &wire)) return Fail(failure, encodeFailure);
        if (!send(wire)) return Fail(failure, sendFailure);
        serial_ = event.serial; return true;
    }
    std::array<Frame, Capacity> records_{};
    SpectatorStageEvent identity_;
    std::uint32_t firstFrame_ = 0, next_ = 0, stage_ = 0, serial_ = 0, checkpoint_ = 0;
    unsigned phase_ = 0; bool ended_ = false;
};
}
