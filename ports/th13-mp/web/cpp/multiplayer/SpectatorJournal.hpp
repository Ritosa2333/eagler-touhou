#pragma once

#include "SessionSetup.hpp"
#include "StageEntry.hpp"
#include "netplay/NetplayProtocolConfig.hpp"

#include <eagler/netplay/SpectatorStagePlayback.hpp>
#include <eagler/netplay/SpectatorStagePublisher.hpp>

#include <cstdint>
#include <vector>

namespace th13::multiplayer {

class ReplayArchive;

inline constexpr std::uint32_t SpectatorStageSchema = StageEntry::Version;

class SpectatorJournal {
public:
    static constexpr std::uint8_t kGame = Netplay::ProtocolConfig::MagicGame;

    bool BeginPublish(const SessionSetup& setup);
    bool RecordFrame(std::uint32_t frame, std::uint32_t stage,
                     const Netplay::FrameInput* inputs, unsigned count,
                     const std::vector<std::uint8_t>* initializer);
    template<class Inputs, class Send>
    bool Publish(std::uint32_t confirmed, std::uint32_t simulated, Inputs inputs, Send send) {
        return publisher_.Publish(confirmed, simulated, inputs, send, &failure_);
    }
    template<class Send> bool Finish(std::uint32_t endFrame, Send send) {
        return publisher_.Finish(endFrame, send);
    }
    bool Publishing() const { return publisherActive_; }
    std::uint32_t Published() const { return publisher_.Published(); }
    auto NeedsCheckpoint(std::uint32_t frame, std::uint32_t stage) const {
        return publisher_.NeedsCheckpoint(frame, stage);
    }
    const char* PublishFailureName() const {
        return Netplay::SpectatorStagePublisher::FailureName(failure_);
    }

    bool BeginViewing(const SessionSetup& setup) {
        if (!setup.valid()) return false;
        setup_ = setup;
        playback_.Clear();
        viewerActive_ = false;
        return true;
    }
    bool ReceiveEvent(const Netplay::SpectatorStageEvent& event);
    bool ReceiveFrame(const Netplay::SpectatorFramePacket& packet);
    bool Control(Netplay::SpectatorStagePlayback::Action action, std::uint32_t frame = 0);
    template<class Restore, class Tick, class ShouldYield>
    bool Step(Restore restore, Tick tick, ShouldYield shouldYield, unsigned budget = 1) {
        return playback_.Step(restore, tick, shouldYield, budget);
    }
    bool Viewing() const { return viewerActive_; }
    std::uint32_t Position() const { return playback_.Position(); }
    std::uint32_t Target() const { return playback_.Target(); }
    bool Seeking() const { return playback_.Seeking(); }
    bool Paused() const { return playback_.Paused(); }
    std::uint32_t Received() const { return playback_.History().Received(); }
    bool HistoryEnded() const { return playback_.History().Ended(); }
    bool ArchiveFromHistory(ReplayArchive& archive) const;

    void Clear();

private:
    Netplay::SpectatorStagePublisher publisher_;
    Netplay::SpectatorStagePlayback playback_{
        Netplay::SpectatorStagePlayback::RestoreMode::Run,
        Netplay::SpectatorStageHistory::FullRunMaxFrames};
    SessionSetup setup_{};
    bool publisherActive_ = false;
    bool viewerActive_ = false;
    Netplay::SpectatorStagePublisher::PublishFailure failure_ =
        Netplay::SpectatorStagePublisher::PublishFailure::None;
};

}
