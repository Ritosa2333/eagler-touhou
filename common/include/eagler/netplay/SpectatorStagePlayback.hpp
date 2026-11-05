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
            // Joining starts at the first available native Replay stage.
            // Boss marks are navigation only; receipt never seeks past input.
            return Request(history_.Segments().front().checkpoint.frame, false, false);
        }
        return history_.Accept(event);
    }
    bool Receive(const SpectatorFramePacket& input) { return history_.Accept(input); }
    bool Control(Action action, std::uint32_t frame = 0) {
        if (!bootstrapped_ || failed_) return false;
        switch (action) {
        case Action::Pause:
            // Match the timeline's immediate pause behavior: stop at the
            // native cursor reached so far and discard a pending active seek
            // or live jump. A Seek issued while paused still runs in the
            // background; another Pause can cancel that reconstruction too.
            if (seeking_) {
                // Before the first native checkpoint has been installed,
                // position_ is only the controller's default cursor. Keep
                // the required restore and stop at that checkpoint instead
                // of cancelling initialization or claiming an invalid frame.
                if (restore_ && !has_restored_checkpoint_) {
                    const auto* segment = history_.Locate(target_);
                    if (!segment) return false;
                    target_ = segment->checkpoint.frame;
                } else {
                    target_ = position_;
                    restore_ = false;
                }
                seeking_ = false;
                following_ = false;
                live_requested_ = false;
                accelerated_ = false;
            }
            paused_ = true; return true;
        case Action::Play: paused_ = false; return true;
        case Action::Seek: {
            if (frame > Edge() || !history_.Locate(frame)) return false;
            const bool paused = paused_;
            if (!Request(frame, false, true)) return false;
            paused_ = paused;
            return true;
        }
        case Action::Live:
            // Repeated clicks cannot move an in-flight Live destination.
            if (seeking_ && live_requested_) return true;
            return Request(Edge(), true, true);
        }
        return false;
    }
    // Restore and tick failures are terminal to this viewer, never to players.
    // shouldYield is checked after every tick, including background rebuilds.
    template<class Restore, class Tick, class ShouldYield>
    bool Step(Restore restore, Tick tick, ShouldYield shouldYield, unsigned budget = 1) {
        const bool mustInstallInitialCheckpoint = restore_ && !has_restored_checkpoint_;
        if (!bootstrapped_ || (paused_ && !seeking_ && !mustInstallInitialCheckpoint) || failed_) return !failed_;
        if (restore_) {
            const auto* segment = history_.Locate(target_);
            if (!segment || !restore(segment->checkpoint)) return !(failed_ = true);
            restored_checkpoint_serial_ = segment->checkpoint.serial;
            has_restored_checkpoint_ = true;
            position_ = segment->checkpoint.frame; restore_ = false;
        }
        // A Pause before the first Step still installs its required native
        // checkpoint, then holds there without advancing any authored input.
        if (paused_ && !seeking_) return true;
        for (unsigned n = 0; n < budget; ++n) {
            const auto limit = seeking_ ? target_ : Edge();
            if (position_ >= limit) {
                if (seeking_) {seeking_ = false; following_ = live_requested_; accelerated_ = false;}
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
            if (seeking_ && position_ == target_) {seeking_ = false; following_ = live_requested_; accelerated_ = false; break;}
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
    // Only an explicit Seek/Live operation may request bounded fast-forward.
    // Following, delayed input and bootstrap all keep normal playback pace.
    bool FastForwarding() const {
        return seeking_ && accelerated_;
    }
    bool Following() const {return following_;}
    bool Ready() const {return bootstrapped_;}
    bool Initialized() const {return has_restored_checkpoint_;}
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
    bool Request(std::uint32_t frame, bool live, bool accelerated) {
        const auto* segment = history_.Locate(frame);
        if (!segment) return false;
        // Rebuild only when necessary. The target is latched once, including
        // Return-to-live, so incoming input cannot keep moving the destination.
        restore_ = restore_ || frame < position_ ||
            !has_restored_checkpoint_ || restored_checkpoint_serial_ != segment->checkpoint.serial;
        target_ = frame; seeking_ = true; accelerated_ = accelerated;
        live_requested_ = live; following_ = false; paused_ = false; return true;
    }
    SpectatorStageHistory history_;
    std::uint32_t position_ = 0, target_ = 0;
    std::uint32_t restored_checkpoint_serial_ = 0;
    bool bootstrapped_ = false, restore_ = false;
    bool has_restored_checkpoint_ = false;
    bool paused_ = false, seeking_ = false, following_ = false, live_requested_ = false;
    bool accelerated_ = false, failed_ = false;
};
}
