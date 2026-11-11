#pragma once
#include <eagler/netplay/ConfirmedInputClock.hpp>
#include <eagler/netplay/SessionChannel.hpp>

namespace th17::multiplayer {
struct Loadout {
    std::uint8_t character = 0;
    std::uint8_t animal = 0;
};

struct SessionSetup {
    std::uint64_t room = 0;
    std::array<std::uint32_t, 4> build{};
    std::array<Loadout, Netplay::MAX_PLAYERS> loadouts{};
    std::uint16_t seed = 0;
    std::uint8_t seats = 2, local = 0, difficulty = 1;
    std::uint8_t delay = 0;
    bool rollback = true;
    bool challenge = false;

    bool valid() const;
    std::uint32_t gameplay_abi() const;
};

// The stage owns all mutable gameplay state and frame-scoped effects. Its
// checkpoint includes the shared spirit inventory and Roaring timer, along
// with every pilot. Network queues and physical captures stay in Session.
class World {
public:
    virtual ~World() = default;
    virtual bool can_predict() const = 0;
    virtual bool capture(std::uint32_t frame) = 0;
    virtual void speculating(bool) {}
    virtual void replaying(bool intermediate) {}
    virtual bool replay_budget(unsigned replayed, unsigned remaining) const { return true; }
    virtual bool blocked() const { return false; }
    virtual bool step(std::uint32_t frame, const Netplay::FrameDecision&) = 0;
    virtual bool restore(std::uint32_t frame) = 0;
    virtual bool confirm(std::uint32_t frame,
        const std::array<Netplay::FrameInput, Netplay::MAX_PLAYERS>&) = 0;
};

class Session {
public:
    static constexpr std::uint8_t rollback_frames = 12;
    enum class Tick { Waiting, Advanced, Failed };

    explicit Session(Netplay::PeerTransport& transport) : channel_(transport) {}
    bool begin(const SessionSetup&, std::uint64_t now_ms);
    bool pump(std::uint64_t now_ms, bool capture_follows = false);
    bool flush_inputs(std::uint64_t now_ms);
    void capture_phase(bool confirmed, std::uint64_t now_us, bool continuous_touch = false);
    bool can_capture_early() const;
    bool needs_input() const;
    bool capture_input(const Netplay::FrameInput&, std::uint64_t now_ms);
    bool retire(std::uint64_t now_ms);
    Tick tick(World&, bool advance = true);
    bool repair_pending() const { return replay_end_ != 0 || core_.HasRollbackRequest(); }
    std::uint32_t next_frame() const;
    std::uint32_t prediction_head() const;
    bool forward_deferred() const { return forward_deferred_; }
    bool has_confirmed_forward_input() const;
    std::uint32_t confirmed_frames() const { return next_commit_; }
    std::uint32_t corrections() const { return corrections_; }
    std::uint32_t resimulated_frames() const { return resimulated_; }
    std::array<unsigned, 7> input_frontiers() const;
    const char* error() const;

private:
    bool fail(const char*);
    bool behind_peer() const;
    bool commit(World&);
    Tick simulate(World&);
    SessionSetup setup_;
    Netplay::SessionGate gate_;
    Netplay::RollbackCore core_;
    Netplay::SessionChannel channel_;
    Netplay::ConfirmedInputClock capture_clock_;
    std::uint64_t capture_now_us_ = 0;
    std::uint32_t capture_lead_ = 0;
    bool forward_deferred_ = false;
    std::uint32_t next_capture_ = 0, next_commit_ = 0, replay_end_ = 0;
    std::uint32_t corrections_ = 0, resimulated_ = 0;
    const char* error_ = nullptr;
};
}
