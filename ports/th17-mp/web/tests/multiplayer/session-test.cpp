#include "Session.hpp"
#include "FrameState.hpp"
#include <array>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <map>
#include <vector>

namespace {
void require(bool value, const char* message) {
    if (!value) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

struct Link final : Netplay::PeerTransport {
    std::array<Link*, Netplay::MAX_PLAYERS> links{};
    std::deque<std::vector<std::uint8_t>> inbox;
    bool mute_input = false;

    bool IsOpen() const override { return true; }
    bool Failed() const override { return false; }
    std::size_t BufferedAmount() const override { return 0; }
    bool SendTo(std::uint8_t peer, const std::uint8_t* data, std::size_t size) override {
        if (!links[peer]) return false;
        if (!mute_input) links[peer]->inbox.emplace_back(data, data + size);
        return true;
    }
    bool SendRepairTo(std::uint8_t peer, const std::uint8_t* data, std::size_t size) override {
        return SendTo(peer, data, size);
    }
    bool SendControl(const std::uint8_t* data, std::size_t size) override {
        for (auto* link : links) {
            if (link) link->inbox.emplace_back(data, data + size);
        }
        return true;
    }
    bool Poll(std::vector<std::uint8_t>* packet) override {
        if (inbox.empty()) return false;
        *packet = std::move(inbox.front());
        inbox.pop_front();
        return true;
    }
};

struct World final : th17::multiplayer::World {
    struct State {
        std::array<int, 3> position{};
        unsigned bombs = 0, updates = 0, skip_frames = 0;
    };
    State state;
    struct Snapshot {State world; th17::multiplayer::FrameState schedule;};
    std::map<std::uint32_t, Snapshot> snapshots;
    std::map<std::uint32_t, unsigned> outputs;
    th17::FrameCallback stage_callback;
    th17::FrameScheduler scheduler;
    bool end_callback = false;
    unsigned restores = 0, bombs_committed = 0, frames_committed = 0;
    bool prediction_allowed = true;
    bool speculative=false, block_prediction=false, frame_blocked=false;
    bool logical_draw=false;
    unsigned replay_limit=0,logical_steps=0;

    World() {
        stage_callback.owner = this;
        stage_callback.enabled = true;
        stage_callback.run = [](void* owner) -> th17::i32 {
            auto& world = *static_cast<World*>(owner);
            ++world.state.updates;
            return world.end_callback ? 0 : 1;
        };
        require(scheduler.add(stage_callback, th17::FramePass::Update, 16) >= 0, "install stage callback");
    }

    bool can_predict() const override { return prediction_allowed; }
    void speculating(bool active) override { speculative=active; }
    void replaying(bool intermediate) override { logical_draw=intermediate; }
    bool replay_budget(unsigned replayed,unsigned) const override { return !replay_limit||replayed<replay_limit; }
    bool blocked() const override { return frame_blocked; }
    bool capture(std::uint32_t frame) override {
        auto [snapshot, fresh] = snapshots.try_emplace(frame);
        snapshot->second.world = state;
        return fresh && snapshot->second.schedule.capture(scheduler);
    }
    bool step(std::uint32_t frame, const Netplay::FrameDecision& decision) override {
        frame_blocked=false;
        unsigned bombs = 0;
        for (unsigned seat = 0; seat < decision.inputs.size(); ++seat) {
            const auto buttons = decision.inputs[seat].buttons;
            state.position[seat] += ((buttons & 0x80) != 0) - ((buttons & 0x40) != 0);
            bombs += (buttons & 2) != 0;
        }
        state.bombs += bombs;
        state.skip_frames += (decision.inputs[1].buttons & 0x200) != 0;
        if(speculative&&block_prediction){frame_blocked=true;return false;}
        if(logical_draw)++logical_steps;
        end_callback = frame == 1 && (decision.inputs[1].buttons & 0x80) != 0;
        return scheduler.update() >= 0 && outputs.emplace(frame, bombs).second;
    }
    bool restore(std::uint32_t frame) override {
        const auto checkpoint = snapshots.find(frame);
        if (checkpoint == snapshots.end()) return false;
        if (!checkpoint->second.schedule.restore(scheduler)) return false;
        state = checkpoint->second.world;
        snapshots.erase(checkpoint, snapshots.end());
        outputs.erase(outputs.lower_bound(frame), outputs.end());
        ++restores;
        return true;
    }
    bool confirm(std::uint32_t frame,
                 const std::array<Netplay::FrameInput, Netplay::MAX_PLAYERS>&) override {
        if (frame != frames_committed || !outputs.count(frame)) return false;
        bombs_committed += outputs.at(frame);
        outputs.erase(frame);
        snapshots.erase(frame);
        ++frames_committed;
        return true;
    }
};

using th17::multiplayer::Session;
using th17::multiplayer::SessionSetup;
constexpr auto advanced = Session::Tick::Advanced;
constexpr auto waiting = Session::Tick::Waiting;

void scenario(unsigned seats, bool rollback) {
    std::array<Link, 3> links;
    std::array<Session, 3> sessions{Session(links[0]), Session(links[1]), Session(links[2])};
    std::array<World, 3> worlds;
    SessionSetup setup;
    setup.room = 117;
    setup.build[0] = 812;
    setup.seed = 112;
    setup.seats = std::uint8_t(seats);
    setup.rollback = rollback;
    setup.loadouts[1] = {1, 2};
    for (unsigned seat = 0; seat < seats; ++seat) {
        for (unsigned peer = 0; peer < seats; ++peer) {
            if (seat != peer) links[seat].links[peer] = &links[peer];
        }
        setup.local = std::uint8_t(seat);
        require(sessions[seat].begin(setup, 0), "room start");
    }

    std::uint64_t now = 0;
    const auto pump = [&](unsigned iterations) {
        for (unsigned i = 0; i < iterations; ++i) {
            now += 17;
            for (unsigned seat = 0; seat < seats; ++seat)
                require(sessions[seat].pump(now), sessions[seat].error());
        }
    };
    const auto capture = [&](unsigned seat, std::uint16_t input) {
        auto& session = sessions[seat];
        require(session.needs_input(), "one capture is due");
        require(session.capture_input(Netplay::FrameInput(input), now), "input capture");
        require(!session.needs_input(), "physical sample cannot be taken twice");
    };

    require(sessions[0].tick(worlds[0]) == waiting, "gate holds before HELLO/READY");
    pump(24);
    for (unsigned seat = 0; seat < seats; ++seat) capture(seat, seat == 1 ? 0x280 : 0);
    pump(3);
    for (unsigned seat = 0; seat < seats; ++seat) {
        require(sessions[seat].tick(worlds[seat]) == advanced, "initial frame advanced");
        require(worlds[seat].frames_committed == 1, "initial frame confirmed");
    }

    links[1].mute_input = true;
    for (unsigned seat = 0; seat < seats; ++seat) capture(seat, seat == 1 ? 0x242 : 0);
    pump(3);
    require(sessions[0].tick(worlds[0]) == (rollback ? advanced : waiting), "late input policy");
    require(worlds[0].state.position[1] == (rollback ? 2 : 1), "only safe direction predicted");
    require(worlds[0].state.skip_frames == (rollback ? 2 : 1), "held skip predicts across combat frames");
    require(worlds[0].bombs_committed == 0, "pending bomb not published");
    require(worlds[0].scheduler.contains(worlds[0].stage_callback) != rollback,
            "speculative update can remove a callback");

    if (rollback) {
        worlds[0].prediction_allowed = false;
        capture(0, 0x80);
        require(sessions[0].tick(worlds[0]) == waiting, "lifecycle boundary waits for exact input");
    }
    links[1].mute_input = false;
    pump(8);
    for (unsigned seat = 0; seat < seats; ++seat) {
        const auto result = sessions[seat].tick(worlds[seat]);
        require(result == advanced, "corrected frame");
        require(worlds[seat].state.position[1] == 0, "corrected position converges");
        require(worlds[seat].state.skip_frames == 2, "confirmed skip survives a correction");
        require(worlds[seat].scheduler.contains(worlds[seat].stage_callback), "rollback restores callback");
        require(worlds[seat].state.bombs == 1 && worlds[seat].bombs_committed == 1,
                "confirmed bomb published exactly once");
        require(worlds[seat].frames_committed == 2, "confirmed output is contiguous");
    }
    require(worlds[0].restores == unsigned(rollback), "late mismatch triggers one restore");

    for (unsigned seat = rollback ? 1 : 0; seat < seats; ++seat)
        capture(seat, seat == 0 ? 0x80 : 0);
    pump(3);
    for (unsigned seat = 0; seat < seats; ++seat) {
        require(sessions[seat].tick(worlds[seat]) == advanced, "confirmed lifecycle frame");
        require(worlds[seat].state.position[0] == 1, "captured input survives correction");
        require(worlds[seat].state.updates == 3, "scheduler replays the same logical updates");
        require(worlds[seat].bombs_committed == 1, "committed output is never replayed");
    }
    if(rollback){
        worlds[0].prediction_allowed=true;
        worlds[0].block_prediction=true;
        links[1].mute_input=true;
        for(unsigned seat=0;seat<seats;++seat)capture(seat,seat==0?0x82:0);
        pump(3);
        require(sessions[0].tick(worlds[0])==waiting,"a boundary discovered during simulation stalls the frame");
        require(!sessions[0].forward_deferred(),"a confirmed-input wait does not requeue a live tick");
        require(sessions[0].next_frame()==3&&worlds[0].state.position[0]==1&&worlds[0].state.bombs==1,
                "aborted speculative side effects are restored");
        links[1].mute_input=false;
        pump(8);
        require(sessions[0].tick(worlds[0])==advanced,"confirmed input crosses the boundary");
        require(worlds[0].state.position[0]==2&&worlds[0].bombs_committed==2,"confirmed boundary publishes one action");
    }
    if(rollback&&seats==2){
        worlds[0].block_prediction=false;
        worlds[0].replay_limit=1;
        require(sessions[1].tick(worlds[1])==advanced,"guest reaches the repair fixture");
        links[1].mute_input=true;
        capture(1,0x82);
        for(unsigned i=0;i<4;++i){
            capture(0,0);
            require(sessions[0].tick(worlds[0])==advanced,"predict a burst of late inputs");
        }
        links[1].mute_input=false;
        pump(8);
        for(unsigned i=0;i<2;++i){
            sessions[0].capture_phase(false,now*1000);
            capture(0,0);
            now+=17;
        }
        sessions[0].capture_phase(false,now*1000);
        require(!sessions[0].needs_input(),"repair input keeps a two-frame lead at most");
        for(unsigned i=0;i<3;++i){
            require(sessions[0].tick(worlds[0])==waiting&&sessions[0].repair_pending()&&sessions[0].forward_deferred(),
                    "a sliced repair defers the unused forward tick");
            require(worlds[0].frames_committed==5&&worlds[0].bombs_committed==3,
                    "contiguous corrected output is committed once before yielding");
            if(i==0){
                sessions[0].capture_phase(false,now*1000);
                capture(0,0);
                now+=17;
                sessions[0].capture_phase(false,now*1000);
                require(!sessions[0].needs_input(),"an ongoing repair caps queued input at three samples");
            }
        }
        require(sessions[0].tick(worlds[0])==advanced&&!sessions[0].repair_pending()&&sessions[0].forward_deferred(),
                "the completed repair preserves the forward tick when the budget is spent");
        require(!sessions[0].has_confirmed_forward_input(),"repair cannot spend saved time predicting another tick");
        require(worlds[0].logical_steps==3&&!worlds[0].logical_draw&&sessions[0].next_frame()==8,
                "only the final repaired frame submits a draw and no extra input tick runs");
        sessions[0].capture_phase(false,now*1000);
        require(!sessions[0].needs_input(),"ordinary simulation drains input captured during repair");
    }
}

void incompatible_room(bool challenge_mismatch) {
    Link a, b;
    a.links[1] = &b;
    b.links[0] = &a;
    Session p1(a), p2(b);
    SessionSetup setup;
    setup.room = 19;
    setup.build[1] = 14;
    require(p1.begin(setup, 0), "first contract valid");
    setup.local = 1;
    if (challenge_mismatch) setup.challenge = true;
    else setup.loadouts[1].animal = 1;
    require(p2.begin(setup, 0), "second contract valid");
    require(p1.pump(1), "first sends HELLO");
    require(!p2.pump(2), "room rules or loadout mismatch fails HELLO");
}

void confirmed_input_pipeline() {
    Link a,b;
    a.links[1]=&b;b.links[0]=&a;
    Session p1(a),p2(b);
    World world;
    world.prediction_allowed=false;
    SessionSetup setup;
    setup.room=170;
    setup.build[0]=1;
    require(p1.begin(setup,0),"pipeline host setup");
    setup.local=1;
    require(p2.begin(setup,0),"pipeline guest setup");
    for(unsigned now=1;now<=32;++now)
        require(p1.pump(now)&&p2.pump(now),"pipeline ready");

    using Clock=Netplay::ConfirmedInputClock;
    const std::uint64_t start=40'000;
    for(unsigned n=0;n<Clock::MaxLead;++n){
        const auto due=start+n*Clock::IntervalUs;
        p1.capture_phase(n!=0,due);
        p2.capture_phase(n!=0,due);
        require(p1.capture_input(Netplay::FrameInput(n==3?2:0),due/1000),"queue host input during transition");
        require(p2.capture_input(Netplay::FrameInput{},due/1000),"queue guest input during transition");
        require(!p1.needs_input()&&!p2.needs_input(),"one physical capture per due time");
    }
    p1.capture_phase(true,start+8*Clock::IntervalUs);
    require(!p1.needs_input(),"transition input queue is bounded");
    const auto now=(start+8*Clock::IntervalUs)/1000;
    require(p2.pump(now)&&p1.pump(now),"deliver queued input");
    require(p1.has_confirmed_forward_input(),"the delivered remote prefix permits exact catch-up");
    p1.capture_phase(false,now*1000);
    for(unsigned n=0;n<Clock::MaxLead;++n){
        require(!p1.needs_input(),"queued samples drain on resume before taking fresh input");
        require(p1.tick(world)==advanced,"transition advances using exact queued inputs");
    }
    require(p1.needs_input()&&world.frames_committed==Clock::MaxLead&&world.bombs_committed==1,
            "pipeline preserves a single bomb edge and restores ordinary capture");
}
}

int main() {
    scenario(2, true);
    scenario(3, true);
    scenario(2, false);
    incompatible_room(false);
    incompatible_room(true);
    confirmed_input_pipeline();
    std::cout << "TH17 session transport, late correction and confirmed output passed\n";
}
