// TH13 MP 旁观接缝（M9）。
// 复用共用栈（不 fork）：SpectatorStagePublisher（发布端——只发连续确认
// 历史）、SpectatorStageHistory/Playback（观众端——晚入 Bootstrap、
// 暂停/定位/回直播、慢观众隔离）。TH13 侧职责：
//   - 发布端（本地席 0 拥有）：每模拟帧 Record（含重放帧）；关头变化时
//     捕获 StageEntry 为 checkpoint initializer；Publish 到 min(confirmed,
//     simulated)——只发布确认历史（预测帧不发布）；
//   - 观众端：接收事件/帧包 → SpectatorStagePlayback（重建 + 逐帧 tick）；
//     晚入 = Bootstrap 事件（从已发布 checkpoint 起回放）；慢观众 = 观众端
//     自持缓冲（BufferFrames），发布端不因观众慢而停（隔离）。
// 旁观不参与 HELLO/READY 门（SpectatorFramePacket 自带 gameplayAbi 契约）。
#pragma once

#include "SessionSetup.hpp"
#include "StageEntry.hpp"
#include "netplay/NetplayProtocolConfig.hpp"

#include <eagler/netplay/SpectatorStagePlayback.hpp>
#include <eagler/netplay/SpectatorStagePublisher.hpp>

#include <cstdint>
#include <vector>

namespace th13::multiplayer {

class ReplayArchive;  // ReplayArchive.hpp（ArchiveFromHistory 参数）

// 观众 schema：StageEntry 线格式版本（checkpoint initializer 的解释权）。
inline constexpr std::uint32_t SpectatorStageSchema = StageEntry::Version;

class SpectatorJournal {
public:
    static constexpr std::uint8_t kGame = Netplay::ProtocolConfig::MagicGame;

    // ---- 发布端（本地席 0）----
    // 会话启动（frame0 前）：Reset 发布器。firstFrame 恒 0。
    bool BeginPublish(const SessionSetup& setup);
    // 每模拟帧调用（含回滚重放帧——Record 幂等校验输入一致）。
    // confirmedInputs：该帧的全席确认输入（ConfirmedInputs 面）。
    // stage：当前关号。initializer：关头变化（NeedsCheckpoint Required）时
    // 由调用方捕获的 StageEntry 编码；否则空。
    bool RecordFrame(std::uint32_t frame, std::uint32_t stage,
                     const Netplay::FrameInput* inputs, unsigned count,
                     const std::vector<std::uint8_t>* initializer);
    // 发布确认历史（宿主每帧 Pump 后调用；send = 传输面）。
    template<class Inputs, class Send>
    bool Publish(std::uint32_t confirmed, std::uint32_t simulated, Inputs inputs, Send send) {
        return publisher_.Publish(confirmed, simulated, inputs, send, &failure_);
    }
    // 终局（End 事件——观众端进入结果面）。
    template<class Send> bool Finish(std::uint32_t endFrame, Send send) {
        return publisher_.Finish(endFrame, send);
    }
    bool Publishing() const { return publisherActive_; }
    std::uint32_t Published() const { return publisher_.Published(); }
    // 关头需求查询（调用方决定是否捕获 StageEntry）。
    auto NeedsCheckpoint(std::uint32_t frame, std::uint32_t stage) const {
        return publisher_.NeedsCheckpoint(frame, stage);
    }
    const char* PublishFailureName() const {
        return Netplay::SpectatorStagePublisher::FailureName(failure_);
    }

    // ---- 观众端 ----
    // 观众端身份（Connect 时 Configure 同会话身份——晚入观众先于事件流
    // 到达；发布端身份门按此校验 game/契约/席数/会话）。
    bool BeginViewing(const SessionSetup& setup) {
        if (!setup.valid()) return false;
        setup_ = setup;
        playback_.Clear();
        viewerActive_ = false;  // Bootstrap/首事件到达时置位
        return true;
    }
    // 传输事件喂入（wire 字节；SessionChannel 之外的独立接收面）。
    bool ReceiveEvent(const Netplay::SpectatorStageEvent& event);
    bool ReceiveFrame(const Netplay::SpectatorFramePacket& packet);
    // 回放控制（暂停/播放/定位/回直播）。
    bool Control(Netplay::SpectatorStagePlayback::Action action, std::uint32_t frame = 0);
    // 逐帧步进（restore/tick 由宿主供给——restore = StageEntry 重建世界，
    // tick = 喂确认输入推一帧）。观众端失败是观众端终态（不影响玩家）。
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
    // 观众端完整录像本地保存：历史段（checkpoint initializer + 输入流）→
    // ReplayArchive 编码（晚入观众从 Bootstrap 起的完整已收历史）。
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

}  // namespace th13::multiplayer
