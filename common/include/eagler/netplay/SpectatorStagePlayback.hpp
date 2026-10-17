#pragma once
#include <eagler/netplay/SpectatorStageHistory.hpp>
#include <algorithm>

namespace Netplay {
// The adapter owns native construction and fixed ticks. This controller owns
// receive/playback separation and never samples input, changes gameplay rate,
// or seeks by skipping a simulation tick. Call Step with a callback time budget.
class SpectatorStagePlayback {
public:
    static constexpr std::uint32_t BufferFrames = 90;
    enum class Action { Pause, Play, Seek, Live };
    bool Receive(const SpectatorStageEvent& event) {
        if (event.kind == SpectatorEventKind::Bootstrap) {
            if (bootstrapped_ || !history_.BootstrapMatches(event)) return false;
            bootstrapped_ = true;
            const auto& segment = history_.Segments().back();
            std::uint32_t target = segment.checkpoint.frame;
            for (const auto& node : segment.nodes) if (node.frame <= event.frame) target = node.frame;
            return Request(target, false);
        }
        return history_.Accept(event);
    }
    bool Receive(const SpectatorFramePacket& input) { return history_.Accept(input); }
    bool Control(Action action, std::uint32_t frame = 0) {
        if (!bootstrapped_ || failed_) return false;
        switch (action) {
        case Action::Pause: paused_ = true; return true;
        case Action::Play: paused_ = false; return true;
        case Action::Seek: {
            if (frame > Edge() || !history_.Locate(frame)) return false;
            const bool paused = paused_;
            if (!Request(frame, false)) return false;
            paused_ = paused;
            return true;
        }
        case Action::Live: return Request(Edge(), true);
        }
        return false;
    }
    // Restore and tick failures are terminal to this viewer, never to players.
    // shouldYield is checked after every tick, including background rebuilds.
    template<class Restore, class Tick, class ShouldYield>
    bool Step(Restore restore, Tick tick, ShouldYield shouldYield, unsigned budget = 1) {
        if (!bootstrapped_ || (paused_ && !seeking_) || failed_) return !failed_;
        if (restore_) {
            const auto* segment = history_.Locate(target_);
            if (!segment || !restore(segment->checkpoint)) return !(failed_ = true);
            restored_checkpoint_serial_ = segment->checkpoint.serial;
            has_restored_checkpoint_ = true;
            position_ = segment->checkpoint.frame; restore_ = false;
        }
        for (unsigned n = 0; n < budget; ++n) {
            const auto limit = seeking_ ? target_ : Edge();
            if (position_ >= limit) {
                if (seeking_) {seeking_ = false; following_ = live_requested_;}
                break;
            }
            const auto* segment = history_.Locate(position_);
            if (!segment || position_ - segment->checkpoint.frame >= segment->inputs.size()) break;
            // A received checkpoint starts an independent native world. When
            // forward playback reaches its first buffered input, install that
            // checkpoint before consuming the input. No restore occurs merely
            // because the event arrived, so a paused viewer stays where it is.
            if (!has_restored_checkpoint_ ||
                restored_checkpoint_serial_ != segment->checkpoint.serial) {
                if (position_ != segment->checkpoint.frame || !restore(segment->checkpoint))
                    return !(failed_ = true);
                restored_checkpoint_serial_ = segment->checkpoint.serial;
                has_restored_checkpoint_ = true;
            }
            const auto& packet = segment->inputs[position_ - segment->checkpoint.frame];
            if (!tick(packet, seeking_)) return !(failed_ = true);
            ++position_;
            if (seeking_ && position_ == target_) {seeking_ = false; following_ = live_requested_; break;}
            if (shouldYield()) break;
        }
        return true;
    }
    std::uint32_t Edge() const {
        if (history_.Segments().empty()) return 0;
        const auto min = history_.Segments().front().checkpoint.frame, received = history_.Received();
        return history_.Ended() ? received : received - std::min(BufferFrames, received - min);
    }
    std::uint32_t Position() const {return position_;}
    std::uint32_t Target() const {return target_;}
    bool Paused() const {return paused_;}
    bool Seeking() const {return seeking_;}
    bool Following() const {return following_;}
    bool Ready() const {return bootstrapped_;}
    // A no-input timeout applies only while this viewer has caught up to the
    // live edge. Paused, seeking, historical playback and a completed stream
    // are all intentional states that may outlast the live-input deadline.
    bool WaitingForLiveInput() const {
        return Ready() && Following() && !Paused() && !Seeking() && !history_.Ended() && Position() >= Edge();
    }
    bool Failed() const {return failed_;}
    const SpectatorStageHistory& History() const {return history_;}
    void Clear() {*this = {};}
private:
    bool Request(std::uint32_t frame, bool live) {
        const auto* segment = history_.Locate(frame);
        if (!segment) return false;
        // Rebuild only when necessary. The target is latched once, including
        // Return-to-live, so incoming input cannot keep moving the destination.
        restore_ = restore_ || !initialized_ || frame < position_ ||
            !has_restored_checkpoint_ || restored_checkpoint_serial_ != segment->checkpoint.serial;
        initialized_ = true; target_ = frame; seeking_ = true;
        live_requested_ = live; following_ = false; paused_ = false; return true;
    }
    SpectatorStageHistory history_;
    std::uint32_t position_ = 0, target_ = 0;
    std::uint32_t restored_checkpoint_serial_ = 0;
    bool bootstrapped_ = false, initialized_ = false, restore_ = false;
    bool has_restored_checkpoint_ = false;
    bool paused_ = false, seeking_ = false, following_ = false, live_requested_ = false, failed_ = false;
};
}
