#include "Session.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <sstream>
#include <locale>

#include "../game/StageBattle.hpp"
#include "../game/RunScene.hpp"

namespace th13::multiplayer {

Session session;

std::uint64_t Session::NowMicros() {
    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count());
}

Session::Record& Session::record_push() {
    if (record_count_ == kRecordSlots) {
        record_overflow_ = true;
        record_pop_front();
    }
    Record& slot = record_slots_[record_tail()];
    slot.frame = 0;
    slot.has_snapshot = false;
    slot.inputs = {};
    slot.local_input = {};
    slot.frame_inputs = {};
    slot.audited = false;
    slot.domains[0] = 0;
    slot.wire_digest = 0;
    slot.has_wire_digest = false;
    slot.audio.clear();
    slot.restore_host = {};
    slot.confirmed_output = {};
    ++record_count_;
    return slot;
}

void Session::record_pop_front() {
    if (record_count_ == 0) return;
    Record& slot = record_front();
    slot.frame = 0;
    slot.has_snapshot = false;
    slot.has_wire_digest = false;
    slot.wire_digest = 0;
    slot.audio.clear();
    slot.restore_host = {};
    slot.confirmed_output = {};
    record_head_ = (record_head_ + 1) % kRecordSlots;
    --record_count_;
}

void Session::record_clear() {
    for (std::size_t logical = 0; logical < record_count_; ++logical) {
        Record& slot = record_at(logical);
        slot.frame = 0;
        slot.has_snapshot = false;
        slot.has_wire_digest = false;
        slot.wire_digest = 0;
        slot.audio.clear();
    slot.restore_host = {};
    slot.confirmed_output = {};
    }
    record_head_ = 0;
    record_count_ = 0;
    record_overflow_ = false;
}

bool Session::Configure(const std::uint32_t* words, std::size_t count) {
    SetAudit(audit_enabled_);
    SessionSetup setup;
    if (!DecodeSessionSetup(setup, words, count)) {
        runtime_.Clear();
        active_ = false;
        return false;
    }
    if (!runtime_.Reset(setup)) return false;
    diagnostic_input_count_ = 0;
    diagnostic_samples_ = {};
    local_input_ = menu_input_ = {};
    frame_inputs_ = {};
    record_clear();
    audio_out_.clear();
    voice_out_.clear();
    replayed_audio_ = {};
    replayed_frames_.fill(kNoLedgerFrame);
    applied_audio_.clear();
    sfx_suppressed_ = music_deferred_ = music_dropped_ = 0;
    voice_captured_ = music_captured_ = voice_applied_ = music_applied_ = 0;
    pending_audio_ = nullptr;
    pending_audio_frame_ = 0;
    sim_frame_ = 0;
    committed_ = restores_ = resimulated_ = checkpoints_ = 0;
    simulating_ = false;
    world_finished_ = world_completed_ = spectator_finished_ = false;
    frame_simulated_ = false;
    frame_granted_ = false;
    prediction_barrier_ = false;
    rollback_pending_ = false;
    rollback_failed_ = false;
    capture_us_max_ = capture_us_sum_ = capture_us_count_ = 0;
    restore_us_max_ = restore_us_sum_ = 0;
    update_us_max_ = update_us_sum_ = update_us_count_ = 0;
    resim_us_sum_ = 0;
    rollback_distance_sum_ = 0;
    max_rollback_distance_ = last_rollback_distance_ = 0;
    stall_frames_ = 0;
    frame_granted_us_ = 0;
    active_ = true;
    return true;
}

bool Session::FinishWorldSimulation(bool completed) {
    if (!AllFramesCommitted()) return false;
    if (!simulating_) return true;
    if (audit_enabled_ && battle_ && visual_rng_) {
        std::ostringstream out;
        if (digest_snapshot_.capture(*battle_, *visual_rng_, nullptr,
                world_presentation_, world_scene_, transition_handle_ ? *transition_handle_ : 0)) {
            std::array<char,2048> domains{};
            digest_snapshot_.digest_domains(domains.data(), domains.size());
            out << "{\"frames\":" << sim_frame_ << ",\"completed\":" << (completed ? "true" : "false")
                << ",\"core\":\"" << std::hex << digest_snapshot_.digest_core()
                << "\",\"full\":\"" << digest_snapshot_.digest()
                << "\",\"host\":\"" << (digest_host ? digest_host() : 0)
                << "\",\"domains\":\"" << domains.data() << "\"}";
        } else out << "{\"error\":\"Final world capture failed\"}";
        terminal_audit_ = out.str();
    }
    world_finished_ = true;
    world_completed_ = completed;
    simulating_ = false;
    runtime_.PublishSpectator(ConfirmedThrough(), sim_frame_ ? sim_frame_ - 1 : Netplay::INVALID_FRAME);
    spectator_finished_ = runtime_.FinishSpectator(sim_frame_);
    return true;
}

void Session::Stop() { Stop(false); }

void Session::Stop(bool keep_terminal_transport) {
    SetAudit(false);
    world_finished_ = world_completed_ = spectator_finished_ = false;
    if (!keep_terminal_transport) runtime_.Clear();
    battle_ = nullptr;
    visual_rng_ = nullptr;
    capture_host = {};
    digest_host = {};
    diagnostic_host = {};
    record_clear();
    audio_out_.clear();
    voice_out_.clear();
    replayed_audio_ = {};
    replayed_frames_.fill(kNoLedgerFrame);
    applied_audio_.clear();
    sfx_suppressed_ = music_deferred_ = music_dropped_ = 0;
    voice_captured_ = music_captured_ = voice_applied_ = music_applied_ = 0;
    pending_audio_ = nullptr;
    pending_audio_frame_ = 0;
    sim_frame_ = 0;
    committed_ = restores_ = resimulated_ = checkpoints_ = 0;
    simulating_ = false;
    frame_simulated_ = false;
    frame_granted_ = false;
    prediction_barrier_ = false;
    rollback_pending_ = false;
    rollback_failed_ = false;
    capture_us_max_ = capture_us_sum_ = capture_us_count_ = 0;
    restore_us_max_ = restore_us_sum_ = 0;
    update_us_max_ = update_us_sum_ = update_us_count_ = 0;
    resim_us_sum_ = 0;
    rollback_distance_sum_ = 0;
    max_rollback_distance_ = last_rollback_distance_ = 0;
    stall_frames_ = 0;
    frame_granted_us_ = 0;
    active_ = false;
}

bool Session::Pump(std::uint64_t now_ms) {
    if (world_finished_) {
        if (!runtime_.PumpTerminal(now_ms)) return false;
        if (!spectator_finished_) {
            runtime_.PublishSpectator(ConfirmedThrough(), sim_frame_ ? sim_frame_ - 1 : Netplay::INVALID_FRAME);
            spectator_finished_ = runtime_.FinishSpectator(sim_frame_);
        }
        return true;
    }
    if (frame_granted_ && record_back().has_snapshot) {
        auto& record = record_back();
        if (!record.snapshot.restore(*battle_, *visual_rng_, nullptr,
                world_presentation_, world_scene_, transition_handle_) ||
            (record.restore_host && !record.restore_host())) {
            rollback_failed_ = true;
            return false;
        }
        local_input_ = record.local_input;
        frame_inputs_ = record.frame_inputs;
        record.has_snapshot = false;
        record.audio.clear();
        record.restore_host = {};
        record.confirmed_output = {};
        --record_count_;
        pending_audio_ = nullptr;
        pending_audio_frame_ = 0;
        frame_granted_ = false;
    }
    if (!runtime_.Pump(simulating_, now_ms)) return false;
    if (!rollback_pending_ && runtime_.HasRollbackRequest()) rollback_pending_ = true;
    if (rollback_pending_ && simulating_ && !ExecuteRollback()) return false;
    if (simulating_) Commit();
    return !rollback_failed_;
}

bool Session::Connect(const char* relay_url) {
    if (!active_) return false;
    return runtime_.Connect(relay_url);
}

bool Session::CaptureLocal(std::uint32_t held, std::uint64_t now_ms,
                           StagePlayerState::TouchMotion touch) {
    if (!active_ || !simulating_) return true;
    Netplay::FrameInput input;
    if (!InputLanes::Capture(held, (held & 0x100u) != 0, input)) return false;
    if (held & 0x80000u) input.buttons |= InputLanes::Confirm;
    if (held & 0x200000u) input.buttons |= InputLanes::Restart;
    if (held & 0x10000u) input.buttons |= InputLanes::Quit;
    if ((held & 0x200u) && !(held & InputLanes::TranceBit))
        input.buttons |= InputLanes::Dialogue;
    if (touch.mode) {
        input.analogMode = Netplay::AnalogMode::DirectTouch;
        input.x = touch.x;
        input.y = touch.y;
        input.unlimited = touch.mode == 2;
    }
    const auto frame = runtime_.CaptureFrame();
    TraceTiming("sample", frame, input.buttons);
    const auto sent_before = runtime_.PacketsSent();
    const bool captured = runtime_.Capture(frame, input, now_ms);
    if (captured) TraceTiming(runtime_.PacketsSent() > sent_before ? "send" : "schedule",
                              frame, input.buttons, runtime_.PacketsSent());
    return captured;
}

void Session::BindWorld(th13::StageBattle* battle, GameRandom* visual,
                        StagePresentation* presentation, th13::RunScene* scene,
                        u32* transition_handle) {
    battle_ = battle;
    visual_rng_ = visual;
    world_presentation_ = presentation;
    world_scene_ = scene;
    transition_handle_ = transition_handle;
    if (battle_ && frame_granted_) {
        const auto decision = runtime_.Prepare(sim_frame_);
        for (unsigned seat = 0; seat < PlayerCount(); ++seat) {
            battle_->seat_inputs[seat] = frame_inputs_[seat];
            InputLanes::ApplyTouch(*battle_, seat, decision.inputs[seat]);
        }
    }
}

bool Session::CanSimulate() {
    if (!active_ || !simulating_) return false;
    if (frame_simulated_) return false;
    if (frame_granted_) return true;
    const auto decision = runtime_.Prepare(sim_frame_);
    if (!decision.canAdvance) {
        ++stall_frames_;
        runtime_.NoteInputStarved();
        return false;
    }
    bool menu_edge = false;
    for (unsigned seat = 0; seat < PlayerCount(); ++seat)
        menu_edge |= (decision.inputs[seat].buttons & InputLanes::Pause) != 0;
    if ((prediction_barrier_ || menu_edge) && decision.predictedMask != 0) {
        runtime_.NoteInputStarved();
        return false;
    }
    if (decision.predictedMask && battle_ && visual_rng_) {
        Record& record = record_push();
        record.frame = sim_frame_;
        record.has_snapshot = true;
        record.inputs = decision.inputs;
        record.local_input = local_input_;
        record.frame_inputs = frame_inputs_;
        if (capture_host) record.restore_host = capture_host();
        const std::uint64_t capture_started = NowMicros();
        const bool captured = record.snapshot.capture(
            *battle_, *visual_rng_, nullptr, world_presentation_, world_scene_,
            transition_handle_ ? *transition_handle_ : 0);
        const std::uint64_t capture_elapsed = NowMicros() - capture_started;
        capture_us_sum_ += capture_elapsed;
        if (capture_elapsed > capture_us_max_) capture_us_max_ = capture_elapsed;
        ++capture_us_count_;
        if (!captured) {
            record.has_snapshot = false;
        }
        ++checkpoints_;
    } else {
        Record& record = record_push();
        record.frame = sim_frame_;
        record.inputs = decision.inputs;
    }
    record_back().stage = stage_source ? stage_source() : 1u;
    auto& record = record_back();
    if (battle_ && visual_rng_ && (audit_enabled_ ||
        sim_frame_ % NetplayRuntime::kDigestIntervalFrames == 0)) {
        WorldState* state = record.has_snapshot ? &record.snapshot : &digest_snapshot_;
        if (!record.has_snapshot && !state->capture(*battle_, *visual_rng_, nullptr,
                world_presentation_, world_scene_, transition_handle_ ? *transition_handle_ : 0)) {
            rollback_failed_ = true;
            return false;
        }
        record.has_wire_digest = sim_frame_ % NetplayRuntime::kDigestIntervalFrames == 0;
        record.audited = audit_enabled_;
        if (!record.has_snapshot) {
            record.wire_digest = state->digest_core();
            state->digest_domains(record.domains.data(), record.domains.size());
            if (record.has_wire_digest || record.audited) record.detail = state->diagnostic_detail();
            if (record.audited) record.full_digest = state->digest();
        }
        if (record.audited) {
            record.host_digest = digest_host ? digest_host() : 0;
            record.host_detail = record.has_wire_digest && diagnostic_host
                ? diagnostic_host() : std::string{};
        }
    }
    if (spectator_sink) spectator_sink(sim_frame_, decision.inputs);
    menu_input_ = {};
    for (unsigned seat = 0; seat < PlayerCount(); ++seat) {
        auto& input = frame_inputs_[seat];
        input.update(InputLanes::Controls(decision.inputs[seat]));
        menu_input_.held |= input.held;
        menu_input_.pressed |= input.pressed;
        menu_input_.released |= input.released;
        menu_input_.repeated |= input.repeated;
        for (unsigned bit = 0; bit < 32; ++bit)
            menu_input_.duration[bit] = std::max(menu_input_.duration[bit], input.duration[bit]);
    }
    local_input_ = frame_inputs_[LocalSeat()];
    if (battle_) {
        const auto count = PlayerCount();
        for (std::uint32_t seat = 0; seat < count && seat < 3; ++seat) {
            InputLanes::ApplyTouch(*battle_, seat, decision.inputs[seat]);
            battle_->seat_inputs[seat] = frame_inputs_[seat];
        }
    }
    pending_audio_ = &record_back().audio;
    pending_audio_frame_ = sim_frame_;
    frame_granted_ = true;
    TraceTiming("apply", sim_frame_, decision.inputs[LocalSeat()].buttons,
                decision.predictedMask | (runtime_.Correcting() ? 0x100u : 0));
    frame_granted_us_ = NowMicros();
    return true;
}

void Session::OnFrameSimulated() {
    if (!active_ || !simulating_ || frame_simulated_) return;
    if (!frame_granted_) return;
    const auto decision = runtime_.Prepare(sim_frame_);
    if (!runtime_.MarkSimulated(sim_frame_, decision)) {
#ifdef TH13_MULTIPLAYER_DEBUG_ROLLBACK
        std::fprintf(stderr, "on-simulated: mark rejected sim=%u canAdvance=%d\n",
                     sim_frame_, int(decision.canAdvance));
#endif
        return;
    }
    frame_simulated_ = true;
    TraceTiming("simulate", sim_frame_, decision.inputs[LocalSeat()].buttons,
                runtime_.Correcting() ? 1 : 0);
    pending_audio_ = nullptr;
    pending_audio_frame_ = 0;
    const std::uint64_t update_elapsed = NowMicros() - frame_granted_us_;
    update_us_sum_ += update_elapsed;
    if (update_elapsed > update_us_max_) update_us_max_ = update_elapsed;
    ++update_us_count_;
    if (runtime_.Correcting()) resim_us_sum_ += update_elapsed;
    ++sim_frame_;
    if (runtime_.Correcting()) ++resimulated_during_correction_;
    if (runtime_.Correcting() && sim_frame_ >= runtime_.CorrectionEnd()) {
        if (!runtime_.EndCorrection()) {
            simulating_ = false;
            return;
        }
        resimulated_ += resimulated_during_correction_;
        TraceTiming("corrected", sim_frame_ - 1, resimulated_during_correction_);
        resimulated_during_correction_ = 0;
    }
    Commit();
    frame_simulated_ = false;
    frame_granted_ = false;
}

bool Session::PredictedFrame() const {
    if (!active_ || !simulating_) return false;
    return runtime_.Prepare(sim_frame_).predictedMask != 0;
}

void Session::Commit() {
    const auto confirmed = runtime_.ConfirmedThroughAll();
    if (confirmed == 0xFFFFFFFFu) return;
    while (record_count_ != 0 && record_front().frame < sim_frame_ && record_front().frame <= confirmed) {
        auto& record = record_front();
        std::array<Netplay::FrameInput, 3> actual{};
        if (!runtime_.ConfirmedInputs(record.frame, actual) ||
            actual != record.inputs)
            break;
        if (record.has_snapshot && (record.audited || record.has_wire_digest)) {
            record.wire_digest = record.snapshot.digest_core();
            record.snapshot.digest_domains(record.domains.data(), record.domains.size());
            if (record.has_wire_digest || record.audited)
                record.detail = record.snapshot.diagnostic_detail();
            if (record.audited) record.full_digest = record.snapshot.digest();
        }
        for (const auto& observation : record.audio) {
            if (observation.kind == th13::sdl::AudioDevice::RequestObservation::Kind::Voice)
                continue;
            audio_out_.push_back({record.frame, observation});
        }
        const std::size_t slot = record.frame % kReplayedSlots;
        if (replayed_frames_[slot] == record.frame) {
            replayed_audio_[slot].clear();
            replayed_frames_[slot] = kNoLedgerFrame;
        }
        auto& input = diagnostic_inputs_[diagnostic_input_count_++ % diagnostic_inputs_.size()];
        input.frame = record.frame;
        input.stage = record.stage;
        for (std::size_t seat = 0; seat < input.buttons.size(); ++seat)
            input.buttons[seat] = actual[seat].buttons;
        if (record.has_wire_digest) {
            auto& sample = diagnostic_samples_[(record.frame / NetplayRuntime::kDigestIntervalFrames)
                                               % diagnostic_samples_.size()];
            sample = {};
            sample.frame = record.frame;
            sample.stage = record.stage;
            sample.domains = record.domains;
            sample.detail = record.detail;
            runtime_.SendDigestControl(record.frame, static_cast<std::uint32_t>(record.wire_digest));
        }
        if (record.audited) {
            std::ostringstream row;
            row << "{\"frame\":" << record.frame << ",\"stage\":" << record.stage
                << ",\"core\":\"" << std::hex << record.wire_digest
                << "\",\"full\":\"" << record.full_digest
                << "\",\"host\":\"" << record.host_digest << std::dec
                << "\",\"hostDetail\":\"" << record.host_detail
                << "\",\"domains\":\"" << record.domains.data()
                << "\",\"detail\":\"" << record.detail << "\",\"inputs\":[";
            for (unsigned seat = 0; seat < PlayerCount(); ++seat) {
                if (seat) row << ',';
                row << record.inputs[seat].buttons;
            }
            row << "]}";
            if (audit_rows_.size() == 512) { audit_rows_.pop_front(); ++audit_overflow_; }
            audit_rows_.push_back(row.str());
        }
        if (record.confirmed_output && !record.confirmed_output()) {
            rollback_failed_ = true;
            return;
        }
        TraceTiming("confirm", record.frame, actual[LocalSeat()].buttons);
        if (confirmed_sink) confirmed_sink(record.frame, record.stage, record.inputs);
        record_pop_front();
        ++committed_;
    }
}

std::string Session::TakeAuditJson() {
    std::ostringstream out;
    out << "{\"overflow\":" << audit_overflow_ << ",\"frames\":[";
    bool comma = false;
    for (const auto& row : audit_rows_) { if (comma) out << ','; comma = true; out << row; }
    out << "],\"terminal\":" << (terminal_audit_.empty() ? "null" : terminal_audit_) << '}';
    audit_rows_.clear();
    return out.str();
}

void Session::TraceTiming(const char* kind, std::uint32_t frame,
                           std::uint32_t value, std::uint32_t detail) {
    if (!audit_enabled_) return;
    if (timing_count_ == timing_events_.size()) {
        timing_head_ = (timing_head_ + 1) % timing_events_.size();
        --timing_count_; ++timing_overflow_;
    }
    timing_events_[(timing_head_ + timing_count_++) % timing_events_.size()] =
        {kind, NowMicros(), frame, value, detail};
}

std::string Session::TakeTimingJson() {
    std::ostringstream out;
    out << "{\"clock\":\"steady-microseconds\",\"overflow\":" << timing_overflow_
        << ",\"events\":[";
    for (std::size_t index = 0; index < timing_count_; ++index) {
        const auto& event = timing_events_[(timing_head_ + index) % timing_events_.size()];
        if (index) out << ',';
        out << "{\"kind\":\"" << event.kind << "\",\"us\":" << event.us
            << ",\"frame\":" << event.frame << ",\"value\":" << event.value
            << ",\"detail\":" << event.detail << '}';
    }
    out << "]}";
    timing_head_ = timing_count_ = 0;
    return out.str();
}

std::string Session::DiagnosticJson() const {
    std::ostringstream out;
    out.imbue(std::locale::classic());
    const auto& setup = Setup();
    out << "{\"localSeat\":" << unsigned(LocalSeat()) + 1
        << ",\"playerCount\":" << PlayerCount()
        << ",\"difficulty\":" << setup.difficulty << ",\"seed\":" << setup.seed
        << ",\"mode\":" << setup.mode << ",\"challenge\":" << (setup.challenge ? "true" : "false")
        << ",\"nextSimulationFrame\":" << sim_frame_ << ",\"captureFrame\":" << CaptureFrame()
        << ",\"confirmedThrough\":";
    if (ConfirmedThrough() == Netplay::INVALID_FRAME) out << "null"; else out << ConfirmedThrough();
    out << ",\"inputDelay\":" << setup.input_delay << ",\"predictionLimit\":" << setup.prediction_limit
        << ",\"predictionBarrier\":" << (prediction_barrier_ ? "true" : "false")
        << ",\"rollback\":{\"restores\":" << restores_ << ",\"resimulated\":" << resimulated_
        << ",\"lastDepth\":" << last_rollback_distance_ << ",\"maxDepth\":" << max_rollback_distance_
        << ",\"stalls\":" << stall_frames_ << ",\"checkpoints\":" << checkpoints_ << '}'
        << ",\"wireDigest\":{\"sent\":" << runtime_.DigestsSent()
        << ",\"received\":" << runtime_.DigestsReceived();
    if (runtime_.DigestMismatched()) {
        const auto& pair = runtime_.MismatchDigests();
        out << ",\"mismatch\":{\"frame\":" << pair.frame << ",\"local\":\"" << std::hex
            << pair.local << "\",\"remote\":\"" << pair.remote << "\"}" << std::dec;
    }
    out << "},\"checkpointSamples\":[";
    bool comma = false;
    for (const auto& sample : diagnostic_samples_) {
        if (!sample.domains[0]) continue;
        if (comma) out << ',';
        comma = true;
        out << "{\"frame\":" << sample.frame << ",\"stage\":" << sample.stage
            << ",\"domains\":\"" << sample.domains.data() << "\",\"detail\":\"" << sample.detail << "\"}";
    }
    out << "],\"confirmedInputs\":[";
    const auto first = diagnostic_input_count_ - std::min(diagnostic_input_count_, diagnostic_inputs_.size());
    for (auto i = first; i < diagnostic_input_count_; ++i) {
        const auto& input = diagnostic_inputs_[i % diagnostic_inputs_.size()];
        if (i != first) out << ',';
        out << "{\"frame\":" << input.frame << ",\"stage\":" << input.stage
            << ",\"buttons\":[" << input.buttons[0] << ',' << input.buttons[1] << ',' << input.buttons[2] << "]}";
    }
    out << "],\"pendingInputs\":[";
    for (std::size_t i = 0; i < record_count_; ++i) {
        const auto& record = record_at(i);
        if (i) out << ',';
        out << "{\"frame\":" << record.frame << ",\"stage\":" << record.stage
            << ",\"snapshot\":" << (record.has_snapshot ? "true" : "false")
            << ",\"buttons\":[" << record.inputs[0].buttons << ',' << record.inputs[1].buttons
            << ',' << record.inputs[2].buttons << "]}";
    }
    out << "]}";
    return out.str();
}

bool Session::ExecuteRollback() {
    if (!rollback_pending_) return true;
    const auto target = runtime_.RollbackFrame();
#ifdef TH13_MULTIPLAYER_DEBUG_ROLLBACK
    std::fprintf(stderr, "rollback: execute target=%u sim=%u records=%zu\n",
                 target, sim_frame_, record_count_);
    for (std::size_t logical = 0; logical < record_count_; ++logical)
        std::fprintf(stderr, "  record frame=%u snap=%d\n", record_at(logical).frame,
                     int(record_at(logical).has_snapshot));
#endif
    std::size_t record_logical = record_count_;
    for (std::size_t logical = 0; logical < record_count_; ++logical)
        if (record_at(logical).frame == target) { record_logical = logical; break; }
    const bool record_found = record_logical < record_count_;
    if (!record_found || !record_at(record_logical).has_snapshot) {
#ifdef TH13_MULTIPLAYER_DEBUG_ROLLBACK
        std::fprintf(stderr, "rollback: target=%u records=%zu found=%d snapshot=%d\n",
                     target, record_count_, record_found,
                     record_found ? int(record_at(record_logical).has_snapshot) : -1);
#endif
        rollback_pending_ = false;
        rollback_failed_ = true;
        return false;
    }
    const std::uint64_t restore_started = NowMicros();
    const bool restored = record_at(record_logical).snapshot.restore(
        *battle_, *visual_rng_, nullptr, world_presentation_, world_scene_,
        transition_handle_);
    const std::uint64_t restore_elapsed = NowMicros() - restore_started;
    restore_us_sum_ += restore_elapsed;
    if (restore_elapsed > restore_us_max_) restore_us_max_ = restore_elapsed;
    if (!restored) {
        rollback_pending_ = false;
        rollback_failed_ = true;
        return false;
    }
    TraceTiming("restore", target, sim_frame_ - target);
    const bool was_correcting = runtime_.Correcting();
    const bool began = was_correcting ? runtime_.RestartCorrection(target)
                                      : runtime_.BeginCorrection(target);
    if (!began) {
#ifdef TH13_MULTIPLAYER_DEBUG_ROLLBACK
        std::fprintf(stderr, "rollback: correction rejected target=%u correcting=%d next=%u end=%u restores=%u sim=%u\n",
                     target, int(runtime_.Correcting()), runtime_.NextFrame(),
                     runtime_.CorrectionEnd(), restores_, sim_frame_);
#endif
        rollback_pending_ = false;
        rollback_failed_ = true;
        return false;
    }
    using Kind = th13::sdl::AudioDevice::RequestObservation::Kind;
    for (std::size_t logical = record_logical; logical < record_count_; ++logical) {
        const Record& dropped = record_at(logical);
        const std::size_t slot = dropped.frame % kReplayedSlots;
        if (replayed_frames_[slot] != dropped.frame) {
            replayed_audio_[slot].clear();
            replayed_frames_[slot] = dropped.frame;
        }
        for (const auto& observation : dropped.audio) {
            if (observation.kind == Kind::Voice)
                replayed_audio_[slot].push_back(observation);
            else if (observation.kind == Kind::Music)
                ++music_dropped_;
        }
    }
    for (auto it = voice_out_.begin(); it != voice_out_.end();) {
        if (it->frame < target) { ++it; continue; }
        const std::size_t slot = it->frame % kReplayedSlots;
        if (replayed_frames_[slot] == it->frame) {
            auto& ledger = replayed_audio_[slot];
            auto match = std::find_if(
                ledger.begin(), ledger.end(), [&](const auto& observation) {
                    return observation.kind == Kind::Voice &&
                           observation.id == it->observation.id;
                });
            if (match != ledger.end()) ledger.erase(match);
        }
        it = voice_out_.erase(it);
    }
    const GameInput local_input_at_checkpoint = record_at(record_logical).local_input;
    const auto frame_inputs_at_checkpoint = record_at(record_logical).frame_inputs;
    if (record_at(record_logical).restore_host && !record_at(record_logical).restore_host()) return false;
    for (std::size_t logical = record_count_; logical-- > record_logical;) {
        Record& dropped = record_at(logical);
        dropped.frame = 0;
        dropped.has_snapshot = false;
        dropped.has_wire_digest = false;
        dropped.wire_digest = 0;
        dropped.audio.clear();
        dropped.restore_host = {};
        dropped.confirmed_output = {};
    }
    record_count_ = record_logical;
    const std::uint32_t distance = sim_frame_ > target ? sim_frame_ - target : 0;
    if (distance > max_rollback_distance_) max_rollback_distance_ = distance;
    last_rollback_distance_ = distance;
    rollback_distance_sum_ += distance;
    sim_frame_ = target;
    frame_simulated_ = false;
    frame_granted_ = false;
    local_input_ = local_input_at_checkpoint;
    frame_inputs_ = frame_inputs_at_checkpoint;
    pending_audio_ = nullptr;
    rollback_pending_ = false;
    if (!was_correcting) resimulated_during_correction_ = 0;
    ++restores_;
    rollback_failed_ = false;
    return true;
}

bool Session::ConsumeReplayedVoice(
    const th13::sdl::AudioDevice::RequestObservation& observation) {
    using Kind = th13::sdl::AudioDevice::RequestObservation::Kind;
    if (observation.kind != Kind::Voice) return false;
    const std::size_t slot = pending_audio_frame_ % kReplayedSlots;
    if (replayed_frames_[slot] != pending_audio_frame_) return false;
    auto& ledger = replayed_audio_[slot];
    const auto match = std::find_if(
        ledger.begin(), ledger.end(), [&](const auto& entry) {
            return entry.kind == Kind::Voice && entry.id == observation.id;
        });
    if (match == ledger.end()) return false;
    ledger.erase(match);
    ++sfx_suppressed_;
    return true;
}

const GameInput* Session::LocalDelayedInput() {
    if (!active_ || !simulating_) return nullptr;
    const auto decision = runtime_.Prepare(sim_frame_);
    if (!decision.canAdvance) return nullptr;
    return &local_input_;
}

}
