// TH13 MP 旁观接缝实现（M9）。契约见 SpectatorJournal.hpp。
#include "SpectatorJournal.hpp"

#include "ReplayArchive.hpp"

namespace th13::multiplayer {

bool SpectatorJournal::BeginPublish(const SessionSetup& setup) {
    if (!setup.valid() || setup.local_player != 0) return false;  // 发布权 = 席位 0
    if (!publisher_.Reset(kGame, std::uint8_t(setup.player_count), setup.session_id,
                          GameplayAbi(setup), 0))
        return false;
    setup_ = setup;
    publisherActive_ = true;
    failure_ = Netplay::SpectatorStagePublisher::PublishFailure::None;
    return true;
}

bool SpectatorJournal::RecordFrame(std::uint32_t frame, std::uint32_t stage,
                                   const Netplay::FrameInput* inputs, unsigned count,
                                   const std::vector<std::uint8_t>* initializer) {
    if (!publisherActive_ || !inputs || count < 2 || count > 3) return false;
    Netplay::SpectatorStagePublisher::Frame record;
    record.frame = frame;
    record.stage = stage;
    record.schema = initializer && !initializer->empty() ? SpectatorStageSchema : 0;
    if (initializer && !initializer->empty()) record.initializer = *initializer;
    for (unsigned seat = 0; seat < count && seat < 3; ++seat)
        record.inputs[seat] = inputs[seat];
    record.hasInputs = true;
    return publisher_.Record(std::move(record));
}

bool SpectatorJournal::ReceiveEvent(const Netplay::SpectatorStageEvent& event) {
    // 观众端身份门：game/契约/席数必须与本端 Setup 一致（晚入观众在
    // Connect 时已 Configure 同身份）。
    if (event.game != kGame ||
        event.playerCount != setup_.player_count ||
        event.sessionId != setup_.session_id ||
        event.gameplayAbi != GameplayAbi(setup_))
        return false;
    const bool accepted = playback_.Receive(event);
    if (accepted) viewerActive_ = true;
    return accepted;
}

bool SpectatorJournal::ReceiveFrame(const Netplay::SpectatorFramePacket& packet) {
    if (!viewerActive_) return false;
    return playback_.Receive(packet);
}

bool SpectatorJournal::Control(Netplay::SpectatorStagePlayback::Action action,
                               std::uint32_t frame) {
    if (!viewerActive_) return false;
    return playback_.Control(action, frame);
}

bool SpectatorJournal::ArchiveFromHistory(ReplayArchive& archive) const {
    if (!viewerActive_) return false;
    const auto& history = playback_.History();
    if (history.Segments().empty()) return false;
    if (!archive.Begin(setup_, 0)) return false;
    // 段序 = 接收序（checkpoint 帧 + 输入流）。章号 = checkpoint.stage。
    for (const auto& segment : history.Segments()) {
        // 关头 initializer → StageEntry（观众端录像的关头延续）。
        const std::vector<std::uint8_t>* initializer = nullptr;
        if (!segment.checkpoint.state.empty()) initializer = &segment.checkpoint.state;
        if (initializer && !archive.AppendStageEntry(segment.checkpoint.stage,
                                                      *initializer))
            return false;
        for (const auto& packet : segment.inputs) {
            if (!archive.Append(packet.frame, segment.checkpoint.stage,
                                packet.inputs.data(), packet.playerCount))
                return false;
        }
    }
    return archive.Info().frameCount > 0;
}

void SpectatorJournal::Clear() {
    publisher_ = {};
    playback_.Clear();
    setup_ = {};
    publisherActive_ = viewerActive_ = false;
    failure_ = Netplay::SpectatorStagePublisher::PublishFailure::None;
}

}  // namespace th13::multiplayer
