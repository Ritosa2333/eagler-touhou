#pragma once

#include <eagler/netplay/SpectatorStageProtocol.hpp>
#include <cstddef>
#include <vector>

namespace Netplay {

// Receive state is independent of playback. The title restores a decoded
// initializer and re-feeds these exact inputs; it never samples a device.
class SpectatorStageHistory {
public:
    static constexpr std::size_t MaxFrames = 108000, MaxBytes = 16 * 1024 * 1024;
    struct Segment {
        SpectatorStageEvent checkpoint;
        std::vector<SpectatorFramePacket> inputs;
        std::vector<SpectatorStageEvent> nodes;
        bool ended = false;
    };
    bool Accept(const SpectatorStageEvent& event) {
        if (!SpectatorStageWire::Valid(event) || event.kind == SpectatorEventKind::Bootstrap) return false;
        if (segments_.empty()) {
            if (event.kind != SpectatorEventKind::Checkpoint) return false;
        } else {
            const auto& previous = segments_.back();
            const auto& identity = previous.checkpoint;
            if (ended_ || event.game != identity.game || event.sessionId != identity.sessionId ||
                event.gameplayAbi != identity.gameplayAbi || event.playerCount != identity.playerCount ||
                event.serial <= serial_ || event.frame != next_) return false;
            if (event.kind != SpectatorEventKind::Checkpoint && event.stage != identity.stage) return false;
            if (event.kind == SpectatorEventKind::Checkpoint && event.frame <= identity.frame) return false;
        }
        const auto size = SpectatorStageHeaderBytes + (event.kind == SpectatorEventKind::End ? 0 : 8) + event.state.size();
        if (size > MaxBytes - bytes_) return false;
        if (event.kind == SpectatorEventKind::Checkpoint) {
            Segment segment; segment.checkpoint = event;
            segments_.push_back(std::move(segment)); next_ = event.frame;
        } else if (event.kind == SpectatorEventKind::Node) {
            if (event.checkpoint != segments_.back().checkpoint.serial) return false;
            segments_.back().nodes.push_back(event);
        } else {
            segments_.back().ended = ended_ = true;
        }
        serial_ = event.serial; bytes_ += size;
        return true;
    }
    bool Accept(const SpectatorFramePacket& input) {
        if (segments_.empty() || ended_ || input.frame != next_ || input.frame == INVALID_FRAME ||
            frames_ >= MaxFrames || std::size_t(24 + 12 * input.playerCount) > MaxBytes - bytes_) return false;
        const auto& checkpoint = segments_.back().checkpoint;
        if (input.sessionId != checkpoint.sessionId || input.gameplayAbi != checkpoint.gameplayAbi ||
            input.playerCount != checkpoint.playerCount) return false;
        for (unsigned i = 0; i < input.playerCount; ++i) if (!IsValidFrameInput(input.inputs[i])) return false;
        segments_.back().inputs.push_back(input);
        ++next_; ++frames_; bytes_ += 24 + 12 * input.playerCount;
        return true;
    }
    const Segment* Locate(std::uint32_t frame) const {
        for (auto it = segments_.rbegin(); it != segments_.rend(); ++it)
            if (frame >= it->checkpoint.frame && frame - it->checkpoint.frame <= it->inputs.size()) return &*it;
        return nullptr;
    }
    const std::vector<Segment>& Segments() const { return segments_; }
    std::uint32_t Received() const { return next_; }
    std::uint32_t Serial() const { return serial_; }
    std::size_t Bytes() const { return bytes_; }
    bool Ended() const { return ended_; }
    bool BootstrapMatches(const SpectatorStageEvent& event) const {
        if (segments_.empty() || event.kind != SpectatorEventKind::Bootstrap) return false;
        const auto& checkpoint = segments_.back().checkpoint;
        return SpectatorStageWire::Valid(event) && event.game == checkpoint.game &&
            event.playerCount == checkpoint.playerCount && event.sessionId == checkpoint.sessionId &&
            event.gameplayAbi == checkpoint.gameplayAbi && event.frame == next_ &&
            event.stage == checkpoint.stage && event.serial == serial_;
    }
    void Clear() { segments_.clear(); next_ = serial_ = 0; bytes_ = frames_ = 0; ended_ = false; }
private:
    std::vector<Segment> segments_;
    std::uint32_t next_ = 0, serial_ = 0;
    std::size_t bytes_ = 0, frames_ = 0;
    bool ended_ = false;
};
}
