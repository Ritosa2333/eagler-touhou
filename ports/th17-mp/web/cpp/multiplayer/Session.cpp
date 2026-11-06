#include "Session.hpp"
#include <algorithm>

namespace th17::multiplayer {
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
    confirmed_phase_ = false;
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
    input.maxDirectionPredictionFrames = 2;
    input.directTouchIsAbsolute = true;
    Netplay::SessionChannelConfig channel;
    channel.inputResendMs = 16;
    return (gate_.Reset(identity) && core_.Reset(input) && channel_.BeginSession(identity, now_ms, channel))
        || fail("TH17 room could not initialize");
}

std::uint32_t Session::next_frame() const {
    const auto last = core_.LastSimulatedFrame();
    return last == Netplay::INVALID_FRAME ? 0 : last + 1;
}

bool Session::pump(std::uint64_t now_ms) {
    return !error_ && channel_.Pump(gate_, core_, now_ms, next_capture_ != 0);
}

void Session::capture_phase(bool confirmed, std::uint64_t now_us) {
    confirmed = setup_.rollback && confirmed;
    if (confirmed && !confirmed_phase_) capture_clock_.BeginPhase(now_us);
    confirmed_phase_ = confirmed;
    capture_now_us_ = now_us;
}

bool Session::needs_input() const {
    return !error_ && channel_.Active() && channel_.Error() == Netplay::SessionChannel::Failure::None &&
        gate_.CanStart() && capture_clock_.CanCapture(next_capture_, std::max(next_frame(), replay_end_),
            confirmed_phase_, capture_now_us_);
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
        if (replayed && !world.replay_budget(replayed)) {
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
    if (correcting && (!advance || !world.replay_budget(1))) return Tick::Advanced;
    if (!advance) return Tick::Waiting;
    const auto result = simulate(world);
    if (correcting && result == Tick::Waiting) return Tick::Advanced;
    return result == Tick::Advanced && !commit(world) ? Tick::Failed : result;
}
}
