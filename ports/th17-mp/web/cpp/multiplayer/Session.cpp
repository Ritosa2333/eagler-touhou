#include "Session.hpp"
#include <algorithm>

namespace th17::multiplayer {
namespace {
constexpr unsigned touch_forecast_frames = 6;
constexpr unsigned lagging_capture_lead = 3;
}
bool SessionSetup::valid() const {
    if (!room || seats < 2 || seats > Netplay::MAX_PLAYERS || local >= seats ||
        difficulty > 4 || delay > 9 || !(build[0] | build[1] | build[2] | build[3])) return false;
    for (unsigned seat = 0; seat < loadouts.size(); ++seat) {
        const auto [character, animal] = loadouts[seat];
        if (character >= 3 || animal >= 3 || (seat >= seats && (character || animal))) return false;
    }
    return true;
}

std::uint32_t SessionSetup::gameplay_abi() const {
    std::uint32_t hash = 2166136261u;
    const auto word = [&](std::uint32_t value) {
        for (unsigned byte = 0; byte < 4; ++byte) {
            hash ^= (value >> (byte * 8)) & 255u;
            hash *= 16777619u;
        }
    };
    word(0x54483137u);
    word(2);
    word(seats);
    word(difficulty);
    word(delay);
    word(rollback);
    for (const auto& loadout : loadouts) {
        word(loadout.character);
        word(loadout.animal);
    }
    for (const auto value : build) word(value);
    if (challenge) word(1);
    return hash ? hash : 1u;
}

bool Session::fail(const char* reason) {
    if (!error_) error_ = reason;
    return false;
}

const char* Session::error() const {
    if (error_) return error_;
    return channel_.ErrorText();
}

bool Session::begin(const SessionSetup& setup, std::uint64_t now_ms) {
    channel_.Clear();
    gate_.Clear();
    core_.Clear();
    capture_clock_.Reset();
    capture_now_us_ = 0;
    capture_lead_ = 0;
    forward_deferred_ = false;
    next_capture_ = next_commit_ = replay_end_ = 0;
    corrections_ = resimulated_ = 0;
    error_ = nullptr;
    if (!setup.valid()) return fail("Invalid TH17 room setup");
    setup_ = setup;
    const Netplay::SessionConfig identity{
        setup.room, setup.seed, setup.gameplay_abi(), 17, setup.seats, setup.local
    };
    Netplay::CoreConfig input;
    input.sessionId = setup.room;
    input.playerCount = setup.seats;
    input.localPlayer = setup.local;
    input.inputDelay = setup.delay;
    input.allowPrediction = setup.rollback;
    input.maxRollbackFrames = rollback_frames;
    // Predict held controls during battle, including dialogue skip. The
    // dialogue boundary waits for confirmed input before consuming skip.
    // Bomb and pause still require actual input.
    input.predictableButtons = 0x2f9;
    input.directionButtons = 0xf0;
    input.prediction = Netplay::InputPrediction::ConfirmedHold;
    input.directTouchIsAbsolute = true;
    input.absoluteTouchForecastFrames = touch_forecast_frames;
    Netplay::SessionChannelConfig channel;
    channel.inputResendMs = 16;
    return (gate_.Reset(identity) && core_.Reset(input) && channel_.BeginSession(identity, now_ms, channel))
        || fail("TH17 room could not initialize");
}

std::uint32_t Session::next_frame() const {
    const auto last = core_.LastSimulatedFrame();
    return last == Netplay::INVALID_FRAME ? 0 : last + 1;
}

std::uint32_t Session::prediction_head() const {
    return std::max(next_frame(), replay_end_);
}

bool Session::has_confirmed_forward_input() const {
    const auto confirmed = core_.ConfirmedThroughAllRemotes();
    return confirmed != Netplay::INVALID_FRAME && confirmed >= prediction_head();
}

std::array<unsigned, 7> Session::input_frontiers() const {
    std::array<unsigned, 7> value{};
    value[0] = next_capture_;
    for (unsigned peer=0; peer<setup_.seats; ++peer) {
        value[1+peer] = core_.ConfirmedThrough(peer)+1;
        value[4+peer] = peer == setup_.local ? prediction_head() : channel_.PeerFrame(peer);
    }
    return value;
}

bool Session::pump(std::uint64_t now_ms, bool capture_follows) {
    const bool defer = capture_follows && setup_.rollback && needs_input();
    return !error_ && channel_.Pump(gate_, core_, now_ms, next_capture_ != 0, defer);
}

bool Session::flush_inputs(std::uint64_t now_ms) {
    return channel_.FlushDeferredInputs(core_, now_ms);
}

bool Session::behind_peer() const {
    if (!setup_.rollback || setup_.seats != 3) return false;
    const auto head = prediction_head();
    for (unsigned peer=0; peer<setup_.seats; ++peer) {
        if (peer == setup_.local) continue;
        const auto sent_head = channel_.PeerFrame(peer);
        if (sent_head != Netplay::INVALID_FRAME && sent_head > head)
            return true;
    }
    return false;
}

bool Session::can_capture_early() const {
    return next_capture_ == prediction_head() && behind_peer();
}

void Session::capture_phase(bool confirmed, std::uint64_t now_us, bool continuous_touch) {
    // Keep sending while a repair spans refreshes. One extra slot lets a
    // recovering peer cover another refresh before it starves its partners.
    const bool waiting = repair_pending() || next_frame() - next_commit_ >= rollback_frames;
    const auto lead = !setup_.rollback ? 0u :
        confirmed ? Netplay::ConfirmedInputClock::MaxLead : replay_end_ ? 3u : waiting ? 2u : 0u;
    // Keep the sampling deadline established by the last physical input.
    // Entering an exact-input phase must not postpone an already due sample.
    capture_lead_ = lead;
    // A peer's already-transmitted simulation head proves this lane is
    // behind even before accounting for the transit time. Keep sampling
    // while it recovers, with at most two future physical slots queued.
    if (!confirmed && continuous_touch && capture_lead_ < lagging_capture_lead && behind_peer())
        capture_lead_ = lagging_capture_lead;
    capture_now_us_ = now_us;
}

bool Session::needs_input() const {
    const auto head = prediction_head();
    return !error_ && channel_.Active() && channel_.Error() == Netplay::SessionChannel::Failure::None &&
        gate_.CanStart() && next_capture_ >= head && (next_capture_ == head || next_capture_ - head < capture_lead_) &&
        capture_clock_.CanCapture(next_capture_, head, capture_lead_ != 0, capture_now_us_);
}

bool Session::capture_input(const Netplay::FrameInput& input, std::uint64_t now_ms) {
    if (!needs_input()) return false;
    if (!Netplay::IsValidFrameInput(input) || !core_.ScheduleLocalInput(next_capture_, input)) return fail("Invalid TH17 physical input");
    capture_clock_.Captured(capture_now_us_);
    return channel_.LocalCaptured(core_, next_capture_++, now_ms);
}

bool Session::retire(std::uint64_t now_ms) {
    const auto frame=core_.LastSimulatedFrame();
    if(frame==Netplay::INVALID_FRAME||next_commit_!=frame+1)return false;
    if(!channel_.FlushRetirementFence(core_,frame,now_ms))return fail("TH17 terminal input flush failed");
    if(!channel_.CanRetire(core_,frame))return false;
    return channel_.Retire(core_,frame,now_ms)||fail("TH17 session retirement failed");
}

bool Session::commit(World& world) {
    std::array<Netplay::FrameInput, Netplay::MAX_PLAYERS> inputs;
    while (core_.ConfirmedInputs(next_commit_, &inputs)) {
        if (!world.confirm(next_commit_, inputs)) return fail("TH17 could not commit a confirmed frame");
        ++next_commit_;
    }
    return true;
}

Session::Tick Session::simulate(World& world) {
    const auto frame = next_frame();
    const auto decision = core_.PrepareFrame(frame);
    const auto confirmed = core_.ConfirmedThroughAllRemotes();
    if (!frame && (!next_capture_ || confirmed == Netplay::INVALID_FRAME || confirmed < setup_.delay)) return Tick::Waiting;
    const bool speculative = confirmed == Netplay::INVALID_FRAME || frame > confirmed;
    if (!decision.canAdvance || (speculative && !world.can_predict())) return Tick::Waiting;
    if (speculative && !world.capture(frame)) {
        fail("TH17 could not capture the rollback state");
        return Tick::Failed;
    }
    world.speculating(speculative);
    const bool stepped=world.step(frame, decision);
    world.speculating(false);
    if(!stepped&&speculative&&world.blocked()&&world.restore(frame))return Tick::Waiting;
    if (!stepped || !core_.MarkSimulated(frame, decision)) {
        fail("TH17 could not simulate the agreed frame");
        return Tick::Failed;
    }
    if(frame<replay_end_)++resimulated_;
    return Tick::Advanced;
}

Session::Tick Session::tick(World& world, bool advance) {
    forward_deferred_ = false;
    if (error_ || channel_.Error() != Netplay::SessionChannel::Failure::None) return Tick::Failed;
    if (!channel_.Active() || !gate_.CanStart()) return Tick::Waiting;

    if (core_.HasRollbackRequest()) {
        const auto first = core_.RollbackFrame();
        replay_end_ = std::max(replay_end_, next_frame());
        if (first < next_commit_ || !world.restore(first) || !core_.RewindSimulationTo(first)) {
            fail("TH17 rollback checkpoint expired");
            return Tick::Failed;
        }
        ++corrections_;
    }

    const bool correcting = replay_end_ != 0;
    for (unsigned replayed = 0; next_frame() < replay_end_; ++replayed) {
        if (replayed && !world.replay_budget(replayed, replay_end_ - next_frame())) {
            forward_deferred_ = advance;
            return commit(world) ? Tick::Waiting : Tick::Failed;
        }
        world.replaying(next_frame() + 1 < replay_end_);
        const auto result = simulate(world);
        world.replaying(false);
        if (result != Tick::Advanced) {
            return result == Tick::Waiting && !commit(world) ? Tick::Failed : result;
        }
    }
    replay_end_ = 0;
    if (!commit(world)) return Tick::Failed;
    // Keep this tick when a small correction leaves room for the next update.
    if (correcting && (!advance || !world.replay_budget(1, 0))) {
        forward_deferred_ = advance;
        return Tick::Advanced;
    }
    if (!advance) return Tick::Waiting;
    const auto result = simulate(world);
    if (correcting && result == Tick::Waiting) return Tick::Advanced;
    return result == Tick::Advanced && !commit(world) ? Tick::Failed : result;
}
}
