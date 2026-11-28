#pragma once

#include "InputLanes.hpp"
#include "NetplayRuntime.hpp"
#include "WorldState.hpp"

#include "../game/GameInput.hpp"
#include "../sdl/AudioDevice.hpp"

#include <array>
#include <cstdint>
#include <deque>
#include <functional>
#include <vector>

namespace th13 {
class StageBattle;
class RunScene;
}

namespace th13::multiplayer {

class Session {
public:
    struct AudioRequest {
        std::uint32_t frame = 0;
        th13::sdl::AudioDevice::RequestObservation observation;
        std::uint32_t confirmed = 0;
    };
private:
    struct Record {
        std::uint32_t frame = 0;
        std::uint32_t stage = 1;
        bool has_snapshot = false;
        WorldState snapshot;
        std::uint64_t wire_digest = 0;
        bool has_wire_digest = false;
        std::array<Netplay::FrameInput, 3> inputs{};
        GameInput local_input{};
        std::array<GameInput, 3> frame_inputs{};
        std::array<char, 2048> domains{};
        std::string detail;
        std::uint64_t full_digest = 0;
        std::uint64_t host_digest = 0;
        std::string host_detail;
        bool audited = false;
        std::vector<th13::sdl::AudioDevice::RequestObservation> audio;
        std::function<bool()> restore_host, confirmed_output;
    };
    static constexpr std::size_t kRecordSlots = 16;
    std::array<Record, kRecordSlots> record_slots_{};
    std::size_t record_head_ = 0, record_count_ = 0;
    bool record_overflow_ = false;
    std::size_t record_tail() const { return (record_head_ + record_count_) % kRecordSlots; }
    Record& record_front() { return record_slots_[record_head_]; }
    const Record& record_front() const { return record_slots_[record_head_]; }
    Record& record_back() { return record_slots_[(record_tail() + kRecordSlots - 1) % kRecordSlots]; }
    Record& record_at(std::size_t logical) {
        return record_slots_[(record_head_ + logical) % kRecordSlots];
    }
    const Record& record_at(std::size_t logical) const {
        return record_slots_[(record_head_ + logical) % kRecordSlots];
    }
    Record& record_push();
    void record_pop_front();
    void record_clear();
    std::vector<AudioRequest> audio_out_;
    std::vector<AudioRequest> voice_out_;
    static constexpr std::size_t kReplayedSlots = 16;
    static constexpr std::uint32_t kNoLedgerFrame = 0xFFFFFFFFu;
    std::array<std::vector<th13::sdl::AudioDevice::RequestObservation>, kReplayedSlots>
        replayed_audio_{};
    std::array<std::uint32_t, kReplayedSlots> replayed_frames_{};
    static constexpr std::size_t kAppliedLogCapacity = 4096;
    std::deque<AudioRequest> applied_audio_;
    std::uint64_t sfx_suppressed_ = 0;
    std::uint64_t music_deferred_ = 0;
    std::uint64_t music_dropped_ = 0;
    std::uint64_t voice_captured_ = 0, music_captured_ = 0;
    std::uint64_t voice_applied_ = 0, music_applied_ = 0;
    std::uint32_t committed_ = 0, restores_ = 0, resimulated_ = 0, checkpoints_ = 0;
    std::uint32_t max_rollback_distance_ = 0;
    std::uint32_t last_rollback_distance_ = 0;
    std::uint32_t resimulated_during_correction_ = 0;

public:
    bool Configure(const std::uint32_t* words, std::size_t count);
    void Stop();
    void Stop(bool keep_terminal_transport);
    bool PumpTerminal(std::uint64_t now_ms) { return runtime_.PumpTerminal(now_ms); }
    bool RunReleased() const { return runtime_.TerminalReleased(); }
    bool FinishWorldSimulation(bool completed = false);
    bool WorldCompleted() const { return world_completed_; }

    bool Connect(const char* relay_url);
    bool Connected() const { return runtime_.Connected(); }
    bool ConnectSpectator(const char* relay_url, const char* spectator_id) {
        if (!active_) return false;
        return runtime_.ConnectSpectator(relay_url, spectator_id);
    }
    bool Spectator() const { return runtime_.Spectator(); }
    NetplayRuntime& Runtime() { return runtime_; }

    bool Pump(std::uint64_t now_ms);

    bool CanStart() const { return runtime_.CanStart(); }
    void MarkLocalReady() { runtime_.MarkLocalReady(); }
    bool Start() {
        if (!runtime_.CanStart() || simulating_) return simulating_;
        simulating_ = true;
        return true;
    }
    bool BeginLocalSimulation() {
        if (!active_ || simulating_) return simulating_;
        simulating_ = true;
        runtime_.SetLocalSimulation(true);
        return true;
    }

    bool CaptureLocal(std::uint32_t held, std::uint64_t now_ms,
                      StagePlayerState::TouchMotion touch = {});
    void AdvanceCapturePhase() { runtime_.AdvanceCapturePhase(); }
    std::uint32_t CaptureFrame() const { return runtime_.CaptureFrame(); }

    void SetPredictionBarrier(bool active) { prediction_barrier_ = active; }

    bool CanSimulate();
    void OnFrameSimulated();
    Netplay::RemoteInputResult SubmitRemote(unsigned seat, std::uint32_t frame,
                                             const Netplay::FrameInput& input) {
        return runtime_.SubmitRemote(seat, frame, input);
    }

    const GameInput* LocalDelayedInput();
    const GameInput& MenuInput() const { return menu_input_; }
    std::function<std::function<bool()>()> capture_host;
    std::function<std::uint64_t()> digest_host;
    std::function<std::string()> diagnostic_host;
    void SetConfirmedOutput(std::function<bool()> output) { record_back().confirmed_output = std::move(output); }
    bool AllFramesCommitted() const { return record_count_ == 0 && !Correcting(); }
    bool PredictionBarrier() const { return prediction_barrier_; }
    void SetAudit(bool enabled) {
        audit_enabled_ = enabled; audit_rows_.clear(); audit_overflow_ = 0;
        terminal_audit_.clear();
        timing_head_ = timing_count_ = 0; timing_overflow_ = 0;
        for (std::size_t i = 0; i < record_count_; ++i) record_at(i).audited = false;
    }
    std::string TakeAuditJson();
    void TraceTiming(const char* kind, std::uint32_t frame,
                     std::uint32_t value = 0, std::uint32_t detail = 0);
    std::string TakeTimingJson();

    void BindWorld(th13::StageBattle* battle, GameRandom* visual,
                   StagePresentation* presentation, th13::RunScene* scene = nullptr,
                   u32* transition_handle = nullptr);
    void TryBindWorld(th13::StageBattle* battle, GameRandom* visual,
                      StagePresentation* presentation, th13::RunScene* scene = nullptr,
                      u32* transition_handle = nullptr) {
        if (!active_ || !battle) return;
        BindWorld(battle, visual, presentation, scene, transition_handle);
    }

    void ObserveAudio(const th13::sdl::AudioDevice::RequestObservation& observation) {
        using Kind = th13::sdl::AudioDevice::RequestObservation::Kind;
        if (!pending_audio_) return;
        pending_audio_->push_back(observation);
        if (observation.kind == Kind::Voice) {
            ++voice_captured_;
            if (!ConsumeReplayedVoice(observation))
                voice_out_.push_back({pending_audio_frame_, observation});
        } else if (observation.kind == Kind::Music) {
            ++music_captured_;
            ++music_deferred_;
        }
    }
    bool RecordingAudio() const { return pending_audio_ != nullptr; }
    const std::vector<AudioRequest>& AudioOut() const { return audio_out_; }
    const std::vector<AudioRequest>& VoiceOut() const { return voice_out_; }
    void ConsumeAudioOut(std::size_t count) {
        if (count >= audio_out_.size()) audio_out_.clear();
        else audio_out_.erase(audio_out_.begin(), audio_out_.begin() + count);
    }
    void ConsumeVoiceOut(std::size_t count) {
        if (count >= voice_out_.size()) voice_out_.clear();
        else voice_out_.erase(voice_out_.begin(), voice_out_.begin() + count);
    }
    void ClearAudioOut() { audio_out_.clear(); }
    void NoteApplied(std::uint32_t frame,
                     const th13::sdl::AudioDevice::RequestObservation& observation) {
        using Kind = th13::sdl::AudioDevice::RequestObservation::Kind;
        if (observation.kind == Kind::Voice) ++voice_applied_;
        if (observation.kind == Kind::Music) ++music_applied_;
        if (applied_audio_.size() >= kAppliedLogCapacity) applied_audio_.pop_front();
        AudioRequest entry;
        entry.frame = frame;
        entry.observation = observation;
        entry.confirmed = runtime_.ConfirmedThroughAll();
        applied_audio_.push_back(entry);
    }
    const std::deque<AudioRequest>& AppliedAudio() const { return applied_audio_; }
    void ClearAppliedAudio() { applied_audio_.clear(); }
    std::uint64_t SfxSuppressed() const { return sfx_suppressed_; }
    std::uint64_t MusicDeferred() const { return music_deferred_; }
    std::uint64_t MusicDropped() const { return music_dropped_; }
    std::uint64_t VoiceCaptured() const { return voice_captured_; }
    std::uint64_t MusicCaptured() const { return music_captured_; }
    std::uint64_t VoiceApplied() const { return voice_applied_; }
    std::uint64_t MusicApplied() const { return music_applied_; }
    std::uint32_t Restores() const { return restores_; }
    std::uint32_t Resimulated() const { return resimulated_; }
    std::uint32_t Checkpoints() const { return checkpoints_; }
    std::uint32_t Committed() const { return committed_; }
    std::uint32_t MaxRollbackDistance() const { return max_rollback_distance_; }
    std::uint32_t LastRollbackDistance() const { return last_rollback_distance_; }

    std::uint32_t PacketsSent() const { return runtime_.PacketsSent(); }
    std::uint32_t PacketsReceived() const { return runtime_.PacketsReceived(); }
    std::uint32_t RepairsSent() const { return runtime_.RepairsSent(); }
    std::uint32_t PacketsIgnored() const { return runtime_.PacketsIgnored(); }
    bool TransportOpen() const { return runtime_.TransportOpen(); }
    std::uint32_t RouteCode() const { return runtime_.RouteCode(); }
    std::uint32_t CorrectionEnd() const { return runtime_.CorrectionEnd(); }

    std::uint64_t CaptureUsMax() const { return capture_us_max_; }
    std::uint64_t CaptureUsSum() const { return capture_us_sum_; }
    std::uint64_t CaptureUsCount() const { return capture_us_count_; }
    std::uint64_t RestoreUsMax() const { return restore_us_max_; }
    std::uint64_t RestoreUsSum() const { return restore_us_sum_; }
    std::uint64_t UpdateUsMax() const { return update_us_max_; }
    std::uint64_t UpdateUsSum() const { return update_us_sum_; }
    std::uint64_t UpdateUsCount() const { return update_us_count_; }
    std::uint64_t ResimUsSum() const { return resim_us_sum_; }
    std::uint64_t RollbackDistanceSum() const { return rollback_distance_sum_; }
    std::uint64_t StallFrames() const { return stall_frames_; }

    std::size_t RecordCount() const { return record_count_; }
    bool RecordOverflow() const { return record_overflow_; }
    bool LastRollbackFailed() const { return rollback_failed_; }
    bool HasRollbackRequest() const { return rollback_pending_; }
    bool PredictedFrame() const;

    std::uint32_t SimFrame() const { return sim_frame_; }
    double IntervalScale() const { return runtime_.IntervalScale(); }
    bool Correcting() const { return runtime_.Correcting(); }
    std::uint32_t ConfirmedThrough() const { return runtime_.ConfirmedThroughAll(); }
    std::uint8_t LocalSeat() const { return runtime_.LocalSeat(); }
    std::uint32_t PlayerCount() const { return runtime_.Setup().player_count; }
    const SessionSetup& Setup() const { return runtime_.Setup(); }
    const char* Error() const { return runtime_.Error(); }
    bool Active() const { return active_; }
    bool Simulating() const { return simulating_; }

    std::string DiagnosticJson() const;
    std::function<std::uint32_t()> stage_source;

    std::function<void(std::uint32_t, std::uint32_t, const std::array<Netplay::FrameInput, 3>&)>
        confirmed_sink;
    std::function<bool(std::uint32_t, const std::array<Netplay::FrameInput, 3>&)>
        spectator_sink;

private:
    struct DiagnosticInput {
        std::uint32_t frame = 0, stage = 0;
        std::array<std::uint32_t, 3> buttons{};
    };
    struct DiagnosticSample {
        std::uint32_t frame = 0, stage = 0;
        std::array<char, 2048> domains{};
        std::string detail;
    };
    std::array<DiagnosticInput, 128> diagnostic_inputs_{};
    std::size_t diagnostic_input_count_ = 0;
    std::array<DiagnosticSample, NetplayRuntime::kDigestSlots> diagnostic_samples_{};
    NetplayRuntime runtime_;
    th13::StageBattle* battle_ = nullptr;
    GameInput local_input_{};
    std::array<GameInput, 3> frame_inputs_{};
    GameInput menu_input_{};
    WorldState digest_snapshot_;
    bool audit_enabled_ = false;
    std::deque<std::string> audit_rows_;
    std::uint64_t audit_overflow_ = 0;
    struct TimingEvent {
        const char* kind = nullptr;
        std::uint64_t us = 0;
        std::uint32_t frame = 0, value = 0, detail = 0;
    };
    std::array<TimingEvent, 4096> timing_events_{};
    std::size_t timing_head_ = 0, timing_count_ = 0;
    std::uint64_t timing_overflow_ = 0;
    GameRandom* visual_rng_ = nullptr;
    std::uint32_t sim_frame_ = 0;
    bool simulating_ = false;
    bool world_finished_ = false, world_completed_ = false, spectator_finished_ = false;
    bool active_ = false;
    bool frame_simulated_ = false;
    bool frame_granted_ = false;
    bool prediction_barrier_ = false;
    bool rollback_pending_ = false;
    bool rollback_failed_ = false;
    std::vector<th13::sdl::AudioDevice::RequestObservation>* pending_audio_ = nullptr;
    std::uint32_t pending_audio_frame_ = 0;
    StagePresentation* world_presentation_ = nullptr;
    th13::RunScene* world_scene_ = nullptr;
    u32* transition_handle_ = nullptr;
    std::uint64_t capture_us_max_ = 0, capture_us_sum_ = 0, capture_us_count_ = 0;
    std::uint64_t restore_us_max_ = 0, restore_us_sum_ = 0;
    std::uint64_t update_us_max_ = 0, update_us_sum_ = 0, update_us_count_ = 0;
    std::uint64_t resim_us_sum_ = 0;
    std::uint64_t rollback_distance_sum_ = 0;
    std::uint64_t stall_frames_ = 0;
    std::uint64_t frame_granted_us_ = 0;
    std::string terminal_audit_;
    static std::uint64_t NowMicros();
    void Commit();
    bool ExecuteRollback();
    bool ConsumeReplayedVoice(
        const th13::sdl::AudioDevice::RequestObservation& observation);
};

extern Session session;

}
