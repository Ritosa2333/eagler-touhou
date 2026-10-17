#include <eagler/netplay/SpectatorStageHistory.hpp>
#include <eagler/netplay/SpectatorStagePlayback.hpp>
#include <eagler/netplay/SpectatorStagePublisher.hpp>
#include <eagler/netplay/SpectatorStageView.hpp>
#include <eagler/netplay/SpectatorStageScalars.hpp>
#include <cassert>
#include <cstdio>

using namespace Netplay;
int main() {
    for (const auto game : {'6', '7', '8', 'A', 'B', 'F'}) {
        SpectatorStageEvent begin;
        begin.game = game; begin.playerCount = 3; begin.sessionId = 0x123456789abcdef0ull;
        begin.gameplayAbi = 0x10203040; begin.frame = 600; begin.stage = 4; begin.serial = 1;
        begin.schema = 7; begin.state = {4, 10, 3, 2};
        std::vector<std::uint8_t> wire;
        assert(EncodeSpectatorStageEvent(begin, &wire));
        SpectatorStageEvent decoded;
        assert(DecodeSpectatorStageEvent(wire.data(), wire.size(), &decoded));
        assert(decoded.sessionId == begin.sessionId && decoded.frame == 600 && decoded.state == begin.state);
        for (std::size_t size = 0; size < wire.size(); ++size) {
            decoded.frame = 123;
            assert(!DecodeSpectatorStageEvent(wire.data(), size, &decoded) && decoded.frame == 123);
        }
        for (const auto at : {4, 7, 28, 32, 36}) {
            auto broken = wire; broken[at] = 0;
            if (at == 7) broken[at] = 1;
            assert(!DecodeSpectatorStageEvent(broken.data(), broken.size(), &decoded));
        }
        SpectatorStageHistory history;
        assert(history.Accept(begin));
        SpectatorFramePacket input;
        input.sessionId = begin.sessionId; input.gameplayAbi = begin.gameplayAbi; input.playerCount = 3;
        input.frame = 600; input.inputs[1].buttons = 7;
        assert(history.Accept(input));
        assert(!history.Accept(input) && history.Received() == 601);
        SpectatorStageEvent boss = begin;
        boss.kind = SpectatorEventKind::Node; boss.state.clear(); boss.schema = 0;
        boss.frame = 601; boss.serial = 2; boss.checkpoint = 1;
        boss.node = SpectatorNodeKind::Boss; boss.bossSlots = 3;
        assert(EncodeSpectatorStageEvent(boss, &wire));
        assert(DecodeSpectatorStageEvent(wire.data(), wire.size(), &decoded));
        assert(history.Accept(decoded));
        auto wrong = boss; wrong.serial = 3; wrong.checkpoint = 2;
        const auto bytes = history.Bytes();
        assert(!history.Accept(wrong) && history.Bytes() == bytes && history.Serial() == 2);
        // A Boss mark never removes the earlier stage input needed to reach it.
        assert(history.Locate(601)->inputs[0].inputs[1].buttons == 7);
        auto second = begin; second.frame = 601; second.stage = 5; second.serial = 3;
        second.state = {5, 8, 2, 1};
        assert(history.Accept(second));
        assert(history.Locate(600)->checkpoint.stage == 4);
        assert(history.Locate(601)->checkpoint.stage == 5);
        auto bootstrap = second; bootstrap.kind = SpectatorEventKind::Bootstrap; bootstrap.state.clear();
        assert(history.BootstrapMatches(bootstrap));
        assert(!history.Accept(bootstrap));
        assert(EncodeSpectatorStageEvent(bootstrap, &wire));
        assert(wire.size() == 32 && DecodeSpectatorStageEvent(wire.data(), wire.size(), &decoded));
        auto end = second; end.kind = SpectatorEventKind::End; end.state.clear(); end.serial = 4;
        assert(history.Accept(end) && history.Ended());
        assert(!history.Accept(second));
        history.Clear(); assert(history.Segments().empty() && !history.Bytes());
    }
    SpectatorStageEvent excluded;
    excluded.game = '9'; excluded.playerCount = 2; excluded.sessionId = excluded.gameplayAbi = 1;
    excluded.frame = 0; excluded.serial = excluded.schema = 1; excluded.state = {1};
    std::vector<std::uint8_t> wire = {9};
    assert(!EncodeSpectatorStageEvent(excluded, &wire) && wire == std::vector<std::uint8_t>{9});
    {
        SpectatorStagePlayback playback;
        SpectatorStageEvent initial; initial.game = '6'; initial.playerCount = 2;
        initial.sessionId = initial.gameplayAbi = initial.schema = initial.serial = 1;
        initial.frame = 600; initial.stage = 4; initial.state = {10};
        assert(playback.Receive(initial));
        SpectatorFramePacket input; input.sessionId = input.gameplayAbi = 1; input.playerCount = 2;
        auto boss = initial; boss.state.clear(); boss.kind = SpectatorEventKind::Node;
        boss.serial = 2; boss.frame = 700; boss.checkpoint = 1; boss.node = SpectatorNodeKind::Boss; boss.bossSlots = 3;
        for (unsigned frame = 600; frame < 1000; ++frame) {
            input.frame = frame; input.inputs[0].buttons = 1;
            assert(playback.Receive(input));
            if (frame == 699) assert(playback.Receive(boss));
        }
        auto ready = initial; ready.kind = SpectatorEventKind::Bootstrap; ready.state.clear(); ready.frame = 999; ready.serial = 2;
        assert(!playback.Receive(ready)); ready.frame = 1000; assert(playback.Receive(ready));
        assert(playback.Target() == 700 && !playback.Receive(ready));
        unsigned value = 0, restores = 0, ticks = 0;
        auto restore = [&](const auto& cp) {value = cp.state[0]; ++restores; return true;};
        auto tick = [&](const auto& packet, bool) {value += packet.inputs[0].buttons; ++ticks; return true;};
        auto yield = [] {return false;};
        assert(playback.Step(restore, tick, yield, 300));
        assert(value == 110 && ticks == 100 && restores == 1 && playback.Position() == 700);
        assert(playback.Control(SpectatorStagePlayback::Action::Pause));
        assert(playback.Step(restore, tick, yield, 300) && ticks == 100);
        assert(playback.Control(SpectatorStagePlayback::Action::Seek, 650));
        assert(playback.Paused());
        assert(playback.Step(restore, tick, yield, 300) && value == 60 && restores == 2);
        const auto settledTicks = ticks;
        assert(playback.Paused() && !playback.Seeking() && playback.Position() == 650);
        assert(playback.Step(restore, tick, yield, 300) && ticks == settledTicks);
        assert(playback.Control(SpectatorStagePlayback::Action::Live));
        assert(playback.Target() == 910);
        input.frame = 1000; assert(playback.Receive(input));
        assert(playback.Target() == 910); // Fixed destination, not a moving edge.
        assert(playback.Step(restore, tick, yield, 300));
        assert(playback.Position() == 910 && playback.Following() && restores == 2);
        assert(!playback.Control(SpectatorStagePlayback::Action::Seek, 599));
        auto end = initial; end.kind = SpectatorEventKind::End; end.state.clear(); end.frame = 1001; end.serial = 3;
        assert(playback.Receive(end) && playback.Edge() == 1001);
        assert(playback.Step(restore, tick, yield, 300) && playback.Position() == 1001 && value == 411);
    }
    {
        // Once following has started, a later checkpoint is followed without
        // any stage-specific control call. Its initializer replaces the old
        // world's end state before the first new-segment input is ticked.
        const auto checkpoint = [](std::uint32_t frame, std::uint32_t stage,
                                   std::uint32_t serial, std::uint32_t seed) {
            SpectatorStageEvent event; event.game = '6'; event.playerCount = 2;
            event.sessionId = 0x2233445566778899ull; event.gameplayAbi = 0x33445566;
            event.frame = frame; event.stage = stage; event.serial = serial; event.schema = 1;
            event.state = {std::uint8_t(seed), std::uint8_t(seed >> 8),
                           std::uint8_t(seed >> 16), std::uint8_t(seed >> 24)};
            return event;
        };
        const auto packet = [](std::uint32_t frame) {
            SpectatorFramePacket value; value.sessionId = 0x2233445566778899ull;
            value.gameplayAbi = 0x33445566; value.playerCount = 2; value.frame = frame;
            value.inputs[0].buttons = 1; return value;
        };
        const auto initializer = [](const SpectatorStageEvent& event) {
            return std::uint32_t(event.state[0]) | (std::uint32_t(event.state[1]) << 8) |
                (std::uint32_t(event.state[2]) << 16) | (std::uint32_t(event.state[3]) << 24);
        };
        SpectatorStagePlayback playback;
        assert(playback.Receive(checkpoint(0, 1, 1, 4)));
        for (std::uint32_t frame = 0; frame < 100; ++frame) assert(playback.Receive(packet(frame)));
        auto bootstrap = checkpoint(0, 1, 1, 4);
        bootstrap.kind = SpectatorEventKind::Bootstrap; bootstrap.state.clear(); bootstrap.frame = 100;
        assert(playback.Receive(bootstrap));
        assert(playback.Control(SpectatorStagePlayback::Action::Live));

        std::vector<std::uint32_t> restoredSerials, tickFrames;
        std::uint32_t nativeValue = 0;
        const auto restore = [&](const SpectatorStageEvent& event) {
            restoredSerials.push_back(event.serial); nativeValue = initializer(event); return true;
        };
        const auto tick = [&](const SpectatorFramePacket& input, bool) {
            tickFrames.push_back(input.frame); nativeValue += input.inputs[0].buttons; return true;
        };
        const auto yield = [] { return false; };
        assert(playback.Step(restore, tick, yield, 32));
        assert(playback.Position() == 10 && playback.Following() && nativeValue == 14 &&
               restoredSerials == std::vector<std::uint32_t>{1});

        assert(playback.Receive(checkpoint(100, 2, 2, 9000)));
        for (std::uint32_t frame = 100; frame < 300; ++frame) assert(playback.Receive(packet(frame)));
        while (playback.Position() < playback.Edge()) assert(playback.Step(restore, tick, yield, 32));
        assert(playback.Position() == 210 && playback.Following() && !playback.Seeking() &&
               nativeValue == 9110 && restoredSerials == (std::vector<std::uint32_t>{1, 2}));
        assert(tickFrames.size() == 210);
        for (std::uint32_t frame = 0; frame < tickFrames.size(); ++frame) assert(tickFrames[frame] == frame);
        assert(playback.Step(restore, tick, yield, 32));
        assert(playback.Position() == 210 && restoredSerials.size() == 2 && tickFrames.size() == 210);
    }
    {
        // If a segment is announced at the current stream edge and ends before
        // its first input, playback must not construct that empty segment.
        SpectatorStagePlayback playback;
        SpectatorStageEvent first; first.game = '7'; first.playerCount = 2;
        first.sessionId = first.gameplayAbi = first.schema = first.serial = 1;
        first.frame = 0; first.stage = 1; first.state = {3}; assert(playback.Receive(first));
        SpectatorFramePacket input; input.sessionId = input.gameplayAbi = 1; input.playerCount = 2;
        for (std::uint32_t frame = 0; frame <= 100; ++frame) {
            input.frame = frame; assert(playback.Receive(input));
        }
        auto bootstrap = first; bootstrap.kind = SpectatorEventKind::Bootstrap;
        bootstrap.state.clear(); bootstrap.frame = 101; assert(playback.Receive(bootstrap));
        assert(playback.Control(SpectatorStagePlayback::Action::Live));
        unsigned restores = 0, emptySegmentRestoreAttempts = 0, ticks = 0;
        std::vector<std::uint32_t> restoredSerials;
        const auto restore = [&](const SpectatorStageEvent& event) {
            ++restores; restoredSerials.push_back(event.serial);
            if (event.stage == 2) ++emptySegmentRestoreAttempts;
            return true;
        };
        const auto tick = [&](const SpectatorFramePacket&, bool) { ++ticks; return true; };
        const auto yield = [] { return false; };
        assert(playback.Step(restore, tick, yield, 32) && playback.Position() == 11 && restores == 1);

        SpectatorStageEvent second = first; second.frame = 101; second.stage = 2;
        second.serial = 2; second.state = {9}; assert(playback.Receive(second));
        auto end = second; end.kind = SpectatorEventKind::End; end.state.clear();
        end.serial = 3; assert(playback.Receive(end));
        assert(playback.Edge() == 101 && playback.Step(restore, tick, yield, 256));
        assert(playback.Position() == 101 && ticks == 101 && restores == 1 && emptySegmentRestoreAttempts == 0);
        assert(playback.Step(restore, tick, yield, 256));
        assert(playback.Position() == 101 && ticks == 101 && restores == 1 && emptySegmentRestoreAttempts == 0);

        // The native world is still Stage 1 at this boundary. An explicit
        // paused seek to the same frame must install Stage 2 exactly once.
        assert(playback.Control(SpectatorStagePlayback::Action::Pause));
        assert(playback.Control(SpectatorStagePlayback::Action::Seek, 101) && playback.Paused());
        assert(playback.Step(restore, tick, yield, 256));
        assert(playback.Position() == 101 && playback.Paused() && !playback.Seeking() &&
               restores == 2 && emptySegmentRestoreAttempts == 1 &&
               restoredSerials == (std::vector<std::uint32_t>{1, 2}) && ticks == 101);
        assert(playback.Step(restore, tick, yield, 256));
        assert(restores == 2 && emptySegmentRestoreAttempts == 1 && ticks == 101);
    }
    {
        // Following playback crosses into the second native world as soon as
        // its first confirmed input is available. Pausing on Stage 1 must not
        // eagerly install Stage 2; Play then crosses once and preserves every
        // input exactly once, including across a bounded yield.
        const auto checkpoint = [](std::uint32_t frame, std::uint32_t stage,
                                   std::uint32_t serial, std::uint32_t seed) {
            SpectatorStageEvent event; event.game = '6'; event.playerCount = 2;
            event.sessionId = 0x1020304050607080ull; event.gameplayAbi = 0x10203040;
            event.frame = frame; event.stage = stage; event.serial = serial; event.schema = 1;
            event.state = {std::uint8_t(seed), std::uint8_t(seed >> 8),
                           std::uint8_t(seed >> 16), std::uint8_t(seed >> 24)};
            return event;
        };
        const auto packet = [](std::uint32_t frame) {
            SpectatorFramePacket value; value.sessionId = 0x1020304050607080ull;
            value.gameplayAbi = 0x10203040; value.playerCount = 2; value.frame = frame;
            value.inputs[0].buttons = 1; return value;
        };
        const auto initializer = [](const SpectatorStageEvent& event) {
            return std::uint32_t(event.state[0]) | (std::uint32_t(event.state[1]) << 8) |
                (std::uint32_t(event.state[2]) << 16) | (std::uint32_t(event.state[3]) << 24);
        };
        const auto first = checkpoint(0, 1, 1, 7);
        const auto second = checkpoint(100, 2, 2, 1000);
        SpectatorStagePlayback playback;
        assert(playback.Receive(first));
        for (std::uint32_t frame = 0; frame < 100; ++frame) assert(playback.Receive(packet(frame)));
        auto bootstrap = first; bootstrap.kind = SpectatorEventKind::Bootstrap;
        bootstrap.state.clear(); bootstrap.frame = 100;
        assert(playback.Receive(bootstrap));
        assert(playback.Control(SpectatorStagePlayback::Action::Live));

        std::vector<std::uint32_t> restoredSerials, tickFrames;
        std::uint32_t nativeValue = 0;
        const auto restore = [&](const SpectatorStageEvent& event) {
            restoredSerials.push_back(event.serial); nativeValue = initializer(event); return true;
        };
        const auto tick = [&](const SpectatorFramePacket& input, bool) {
            tickFrames.push_back(input.frame); nativeValue += input.inputs[0].buttons; return true;
        };
        bool yieldAfterStageTwoEntry = true;
        const auto yield = [&] {
            if (yieldAfterStageTwoEntry && !tickFrames.empty() && tickFrames.back() == 100) {
                yieldAfterStageTwoEntry = false; return true;
            }
            return false;
        };
        assert(playback.Step(restore, tick, yield, 32));
        assert(playback.Position() == 10 && playback.Following() &&
               restoredSerials == std::vector<std::uint32_t>{1});
        assert(playback.Control(SpectatorStagePlayback::Action::Pause));
        assert(playback.Paused() && playback.Following());
        assert(playback.Receive(second));
        for (std::uint32_t frame = 100; frame < 300; ++frame) assert(playback.Receive(packet(frame)));
        assert(playback.Step(restore, tick, yield, 256));
        assert(playback.Paused() && playback.Position() == 10 && tickFrames.size() == 10 &&
               restoredSerials == std::vector<std::uint32_t>{1});

        assert(playback.Control(SpectatorStagePlayback::Action::Play));
        assert(!playback.Paused() && playback.Following());
        assert(playback.Step(restore, tick, yield, 256));
        assert(playback.Position() == 101 && playback.Following() &&
               restoredSerials == (std::vector<std::uint32_t>{1, 2}) &&
               tickFrames.size() == 101 && tickFrames.back() == 100 && nativeValue == 1001);
        assert(playback.Step(restore, tick, yield, 256));
        assert(playback.Position() == playback.Edge() && playback.Position() == 210 &&
               playback.Following() && nativeValue == 1110 && tickFrames.size() == 210);
        // At the edge there is no packet to consume and no reason to rebuild.
        assert(playback.Step(restore, tick, yield, 256));
        assert(restoredSerials == (std::vector<std::uint32_t>{1, 2}) && tickFrames.size() == 210);
        for (std::uint32_t frame = 300; frame < 310; ++frame) assert(playback.Receive(packet(frame)));
        assert(playback.Step(restore, tick, yield, 256));
        assert(playback.Position() == playback.Edge() && playback.Position() == 220 &&
               playback.Following() && nativeValue == 1120 && tickFrames.size() == 220);
        for (std::uint32_t frame = 0; frame < tickFrames.size(); ++frame) assert(tickFrames[frame] == frame);
        assert(restoredSerials == (std::vector<std::uint32_t>{1, 2}));
    }
    {
        // A paused seek across segments restores the selected initializer and
        // retains pause. Seeking back and then Live explicitly rebuilds the
        // latest segment while preserving the fixed live destination.
        const auto checkpoint = [](std::uint32_t frame, std::uint32_t stage,
                                   std::uint32_t serial, std::uint32_t seed) {
            SpectatorStageEvent event; event.game = '7'; event.playerCount = 2;
            event.sessionId = 0x1111222233334444ull; event.gameplayAbi = 0x55667788;
            event.frame = frame; event.stage = stage; event.serial = serial; event.schema = 1;
            event.state = {std::uint8_t(seed), std::uint8_t(seed >> 8),
                           std::uint8_t(seed >> 16), std::uint8_t(seed >> 24)};
            return event;
        };
        const auto packet = [](std::uint32_t frame) {
            SpectatorFramePacket value; value.sessionId = 0x1111222233334444ull;
            value.gameplayAbi = 0x55667788; value.playerCount = 2; value.frame = frame;
            value.inputs[0].buttons = 1; return value;
        };
        const auto initializer = [](const SpectatorStageEvent& event) {
            return std::uint32_t(event.state[0]) | (std::uint32_t(event.state[1]) << 8) |
                (std::uint32_t(event.state[2]) << 16) | (std::uint32_t(event.state[3]) << 24);
        };
        const auto first = checkpoint(0, 1, 1, 12);
        const auto second = checkpoint(100, 2, 2, 2000);
        SpectatorStagePlayback playback; assert(playback.Receive(first));
        for (std::uint32_t frame = 0; frame < 100; ++frame) assert(playback.Receive(packet(frame)));
        auto bootstrap = first; bootstrap.kind = SpectatorEventKind::Bootstrap;
        bootstrap.state.clear(); bootstrap.frame = 100; assert(playback.Receive(bootstrap));
        assert(playback.Receive(second));
        for (std::uint32_t frame = 100; frame < 300; ++frame) assert(playback.Receive(packet(frame)));

        std::vector<std::uint32_t> restoredSerials, tickFrames;
        std::uint32_t nativeValue = 0;
        const auto restore = [&](const SpectatorStageEvent& event) {
            restoredSerials.push_back(event.serial); nativeValue = initializer(event); return true;
        };
        const auto tick = [&](const SpectatorFramePacket& input, bool) {
            tickFrames.push_back(input.frame); nativeValue += input.inputs[0].buttons; return true;
        };
        const auto yield = [] { return false; };
        assert(playback.Control(SpectatorStagePlayback::Action::Pause));
        assert(playback.Control(SpectatorStagePlayback::Action::Seek, 105) && playback.Paused());
        assert(playback.Step(restore, tick, yield, 32));
        assert(playback.Paused() && !playback.Seeking() && !playback.Following() &&
               playback.Position() == 105 && nativeValue == 2005 &&
               restoredSerials == std::vector<std::uint32_t>{2} &&
               tickFrames == (std::vector<std::uint32_t>{100, 101, 102, 103, 104}));

        assert(playback.Control(SpectatorStagePlayback::Action::Seek, 5) && playback.Paused());
        assert(playback.Step(restore, tick, yield, 32));
        assert(playback.Paused() && !playback.Seeking() && playback.Position() == 5 &&
               nativeValue == 17 && restoredSerials == (std::vector<std::uint32_t>{2, 1}));

        assert(playback.Control(SpectatorStagePlayback::Action::Live));
        assert(playback.Target() == 210 && playback.Seeking() && !playback.Following());
        assert(playback.Step(restore, tick, yield, 256));
        assert(playback.Position() == 210 && playback.Following() && !playback.Seeking() &&
               !playback.Paused() && nativeValue == 2110 &&
               restoredSerials == (std::vector<std::uint32_t>{2, 1, 2}));
        assert(playback.Step(restore, tick, yield, 256));
        assert(playback.Position() == 210 && restoredSerials.size() == 3 && tickFrames.size() == 120);
    }
    {
        // A failing second-segment constructor is terminal only for this
        // playback instance and never consumes the first Stage 2 input.
        SpectatorStagePlayback playback;
        SpectatorStageEvent first; first.game = '8'; first.playerCount = 2;
        first.sessionId = first.gameplayAbi = first.schema = first.serial = 1;
        first.frame = 0; first.stage = 1; first.state = {1}; assert(playback.Receive(first));
        SpectatorFramePacket input; input.sessionId = input.gameplayAbi = 1; input.playerCount = 2;
        for (std::uint32_t frame = 0; frame < 100; ++frame) {
            input.frame = frame; assert(playback.Receive(input));
        }
        auto bootstrap = first; bootstrap.kind = SpectatorEventKind::Bootstrap;
        bootstrap.state.clear(); bootstrap.frame = 100; assert(playback.Receive(bootstrap));
        assert(playback.Control(SpectatorStagePlayback::Action::Live));
        unsigned ticks = 0, stageTwoRestoreAttempts = 0;
        const auto restore = [&](const SpectatorStageEvent& event) {
            if (event.stage == 2) { ++stageTwoRestoreAttempts; return false; }
            return true;
        };
        const auto tick = [&](const SpectatorFramePacket&, bool) { ++ticks; return true; };
        assert(playback.Step(restore, tick, [] { return false; }, 32));
        assert(playback.Position() == 10 && ticks == 10 && stageTwoRestoreAttempts == 0);
        auto second = first; second.kind = SpectatorEventKind::Checkpoint;
        second.frame = 100; second.stage = 2; second.serial = 2; second.state = {2};
        assert(playback.Receive(second));
        for (std::uint32_t frame = 100; frame < 300; ++frame) {
            input.frame = frame; assert(playback.Receive(input));
        }
        assert(!playback.Step(restore, tick, [] { return false; }, 256));
        assert(playback.Failed() && ticks == 100 && stageTwoRestoreAttempts == 1);
        assert(!playback.Step(restore, tick, [] { return false; }, 256));
        assert(ticks == 100 && stageTwoRestoreAttempts == 1);
    }
    {
        // Exercise watchdog eligibility from actual playback state changes.
        // Wall-clock timeout behavior is covered by the TH08 long-seek browser
        // case; this shared test owns only the state predicate.
        SpectatorStagePlayback playback;
        assert(!playback.Ready() && !playback.WaitingForLiveInput());

        SpectatorStageEvent cp; cp.game = '8'; cp.playerCount = 2;
        cp.sessionId = cp.gameplayAbi = cp.schema = cp.serial = 1;
        cp.frame = 0; cp.stage = 1; cp.state = {1};
        assert(playback.Receive(cp));
        SpectatorFramePacket input; input.sessionId = input.gameplayAbi = 1; input.playerCount = 2;
        for (std::uint32_t frame = 0; frame < 1'000; ++frame) {
            input.frame = frame;
            assert(playback.Receive(input));
        }
        auto ready = cp; ready.kind = SpectatorEventKind::Bootstrap; ready.state.clear(); ready.frame = 1'000;
        assert(playback.Receive(ready) && playback.Ready());
        assert(!playback.WaitingForLiveInput());

        auto restore = [](const auto&) { return true; };
        auto tick = [](const auto&, bool) { return true; };
        auto yield = [] { return false; };
        assert(playback.Step(restore, tick, yield, 1)); // finish bootstrap restore
        assert(playback.Control(SpectatorStagePlayback::Action::Live));
        assert(playback.Seeking() && !playback.WaitingForLiveInput()); // Live remains a seek to its fixed edge.
        assert(playback.Step(restore, tick, yield, 1'000));
        assert(playback.Position() == playback.Edge() && playback.Following());
        assert(playback.WaitingForLiveInput());

        input.frame = 1'000;
        assert(playback.Receive(input));
        assert(playback.Following() && !playback.Paused() && !playback.Seeking());
        assert(playback.Position() + 1 == playback.Edge() && !playback.WaitingForLiveInput());
        assert(playback.Step(restore, tick, yield, 1));
        assert(playback.Position() == playback.Edge() && playback.WaitingForLiveInput());

        assert(playback.Control(SpectatorStagePlayback::Action::Pause));
        assert(playback.Paused() && !playback.WaitingForLiveInput());

        assert(playback.Control(SpectatorStagePlayback::Action::Seek, 400));
        assert(playback.Paused() && playback.Seeking() && playback.Position() >= playback.Edge());
        assert(!playback.WaitingForLiveInput()); // seek backlog is not live starvation
        assert(playback.Step(restore, tick, yield, 1'000));
        assert(playback.Paused() && !playback.Seeking() && playback.Position() == 400);

        assert(playback.Control(SpectatorStagePlayback::Action::Play));
        assert(!playback.Paused() && !playback.Following() && playback.Position() < playback.Edge());
        assert(!playback.WaitingForLiveInput()); // playing older history is intentional
        assert(playback.Step(restore, tick, yield, 1'000));
        assert(playback.Position() == playback.Edge() && !playback.Following() && !playback.Paused() && !playback.Seeking());
        assert(!playback.WaitingForLiveInput()); // historical Play at the edge is still not following live input

        assert(playback.Control(SpectatorStagePlayback::Action::Live));
        assert(playback.Seeking() && !playback.WaitingForLiveInput());
        assert(playback.Step(restore, tick, yield, 1'000));
        assert(playback.Position() == playback.Edge() && playback.Following());
        assert(playback.WaitingForLiveInput());

        SpectatorStageEvent end = cp; end.kind = SpectatorEventKind::End;
        end.state.clear(); end.frame = 1'001; end.serial = 2;
        assert(playback.Receive(end));
        assert(playback.Control(SpectatorStagePlayback::Action::Live));
        assert(playback.Step(restore, tick, yield, 1'000));
        assert(playback.Position() == playback.Edge() && playback.Following() && !playback.Paused() && !playback.Seeking());
        assert(!playback.WaitingForLiveInput()); // Ended at the edge is terminal, not a stalled live stream.
    }
    {
        SpectatorStagePublisher publisher; assert(publisher.Reset('6', 3, 1, 2));
        SpectatorStagePublisher::Frame frame; frame.frame = 0; frame.stage = 1; frame.schema = 1; frame.initializer = {10}; frame.bossSlots = 1;
        assert(publisher.Record(frame));
        // Rollback replaces an unconfirmed Boss registration, including its
        // removal. It cannot amend anything already released to spectators.
        frame.bossSlots = 0; assert(publisher.Record(frame));
        std::vector<std::vector<std::uint8_t>> sent;
        auto send = [&](const auto& bytes) {sent.push_back(bytes); return true;};
        auto inputs = [](auto, auto& values) {values[1].buttons = 3; return true;};
        assert(publisher.Publish(INVALID_FRAME, 0, inputs, send) && sent.empty());
        unsigned attempts = 0;
        auto blocked = [&](const auto& bytes) {if (++attempts == 2) return false; return send(bytes);};
        assert(!publisher.Publish(0, 0, inputs, blocked) && publisher.Serial() == 1 && publisher.Published() == 0);
        assert(publisher.Record(frame)); // Identical historical rerun is idempotent.
        frame.initializer={11}; assert(!publisher.Record(frame)); frame.initializer={10};
        assert(publisher.Publish(0, 0, inputs, send) && publisher.Published() == 1 && sent.size() == 2);
        frame = {}; frame.frame = 1; frame.stage = 1; frame.bossSlots = 3; assert(publisher.Record(frame));
        auto bossBlocked = [&](const auto& bytes) {return bytes[2] == 'S' ? false : send(bytes);};
        assert(!publisher.Publish(1, 1, inputs, bossBlocked) && sent.size() == 3);
        assert(publisher.Publish(1, 1, inputs, send) && sent.size() == 4);
        SpectatorStageEvent boss; assert(DecodeSpectatorStageEvent(sent.back().data(), sent.back().size(), &boss));
        assert(boss.frame == 2 && boss.bossSlots == 3 && boss.checkpoint == 1 && boss.serial == 2);
        assert(!publisher.Finish(3, send));
        assert(publisher.Finish(2, send) && !publisher.Finish(2, send));
    }
    {
        SpectatorStagePublisher publisher;
        assert(!publisher.Reset('F', 3, 1, 2, INVALID_FRAME));
        assert(publisher.Reset('F', 3, 1, 2, 153));
        SpectatorStagePublisher::Frame frame;
        frame.frame = 153; frame.stage = 1; frame.schema = 1; frame.initializer = {10};
        assert(publisher.Record(frame));
        SpectatorStageHistory history;
        auto send = [&](const auto& bytes) {
            SpectatorStageEvent event;
            SpectatorFramePacket input;
            return DecodeSpectatorStageEvent(bytes.data(), bytes.size(), &event)
                ? history.Accept(event)
                : DecodeSpectatorFramePacket(bytes.data(), bytes.size(), &input) && history.Accept(input);
        };
        assert(publisher.Publish(152, 153, [](auto, auto&) {return true;}, send));
        assert(history.Segments().empty());
        assert(publisher.Publish(153, 153, [](auto, auto&) {return true;}, send));
        assert(history.Segments().front().checkpoint.frame == 153 && history.Received() == 154);
        assert(publisher.Finish(154, send) && history.Ended());
    }
    {
        // The title may retire its confirmed input ring immediately after
        // reconcile. The publisher owns the executed/resimulated lanes until
        // their ordered checkpoint + input prefix has actually been sent.
        SpectatorStagePublisher publisher; assert(publisher.Reset('7', 2, 1, 2));
        SpectatorStagePublisher::Frame frame; frame.frame=0; frame.stage=1;
        frame.schema=1; frame.initializer={1}; frame.hasInputs=true; frame.inputs[1].buttons=3;
        assert(publisher.Record(frame)); frame.inputs[1].buttons=9; assert(publisher.Record(frame));
        std::vector<std::vector<std::uint8_t>> sent;
        unsigned attempts=0;
        auto send=[&](const auto& bytes) {if (++attempts==2) return false; sent.push_back(bytes); return true;};
        auto retired=[](auto,auto&) {return false;};
        assert(!publisher.Publish(0,0,retired,send) && publisher.Serial()==1);
        assert(publisher.Publish(0,0,retired,send) && sent.size()==2);
        SpectatorFramePacket packet; assert(DecodeSpectatorFramePacket(sent[1].data(),sent[1].size(),&packet));
        assert(packet.frame==0 && packet.inputs[1].buttons==9);
        assert(publisher.Record(frame) && publisher.Published()==1);
        frame.inputs[1].buttons=7;
        assert(!publisher.Record(frame) && publisher.Published()==1 && sent.size()==2);
    }
    {
        // A command arriving before the first Step must not cancel the
        // mandatory frame-zero restore merely because its cursor is also zero.
        SpectatorStagePlayback playback;
        SpectatorStageEvent cp; cp.game = '7'; cp.playerCount = 2;
        cp.sessionId = cp.gameplayAbi = cp.schema = cp.serial = 1; cp.frame = 0; cp.stage = 1; cp.state = {4};
        assert(playback.Receive(cp));
        auto ready = cp; ready.kind = SpectatorEventKind::Bootstrap; ready.state.clear();
        assert(playback.Receive(ready) && playback.Control(SpectatorStagePlayback::Action::Live));
        unsigned restored = 0;
        assert(playback.Step([&](const auto&) {++restored; return true;}, [](const auto&, bool) {return true;}, [] {return false;}));
        assert(restored == 1);
    }
    {
        SpectatorScalars::Writer writer;
        const std::int16_t negative = -123; const float coordinate = 13.5f; const bool enabled = true;
        assert(writer.Values(std::uint32_t(0x12345678), negative, coordinate, enabled));
        assert(writer.Bytes()[0] == 0x78 && writer.Bytes()[3] == 0x12 && writer.Bytes().back() == 1);
        SpectatorScalars::Reader reader(writer.Bytes());
        std::uint32_t number = 0; std::int16_t signedValue = 0; float position = 0; bool flag = false;
        assert(reader.Values(number, signedValue, position, flag) && reader.Done());
        assert(number == 0x12345678 && signedValue == negative && position == coordinate && flag);
        assert(!reader.Value(number) && !reader.Done());
        const std::vector<std::uint8_t> badBool{2}; SpectatorScalars::Reader bad(badBool);
        assert(!bad.Value(flag));
        // Larger local preimages must still fail the unchanged wire gate.
        const std::vector<std::uint8_t> large(SpectatorStageMaxStateBytes + 17, 1);
        SpectatorScalars::Writer ordinary;
        assert(!ordinary.Array(large) && !ordinary.Good());
        assert(ordinary.Bytes().size() == SpectatorStageMaxStateBytes);
        SpectatorScalars::Writer local(large.size());
        assert(local.Array(large) && local.Good());
        SpectatorStageEvent oversized; oversized.game='6'; oversized.playerCount=3;
        oversized.sessionId=oversized.gameplayAbi=oversized.serial=oversized.schema=1;
        oversized.frame=0; oversized.stage=1; oversized.state=local.Bytes();
        std::vector<std::uint8_t> untouched{7};
        assert(!EncodeSpectatorStageEvent(oversized,&untouched) && untouched == std::vector<std::uint8_t>{7});
        assert(!local.Value(std::uint8_t(1)) && !local.Good());
        SpectatorScalars::Writer zero(0), excessive(SpectatorScalars::MaxLocalBytes+1);
        assert(!zero.Good() && !zero.Value(1));
        assert(!excessive.Good() && !excessive.Value(1));
        SpectatorStagePlayback playback; SpectatorStageView view;
        assert(!view.State(playback));
        SpectatorStageEvent cp; cp.game = '8'; cp.playerCount = 2; cp.sessionId = 1;
        cp.gameplayAbi = 2; cp.schema = cp.serial = 1; cp.frame = 600; cp.stage = 4; cp.state = {4};
        assert(playback.Receive(cp));
        auto ready = cp; ready.kind = SpectatorEventKind::Bootstrap; ready.state.clear();
        assert(playback.Receive(ready) && !view.State(playback));
        assert(playback.Step([](const auto&) {return true;}, [](const auto&, bool) {return true;}, [] {return false;}));
        const std::string state = view.State(playback);
        assert(state.find("\"run\":\"s0000000000000001_00000002\"") != std::string::npos);
        assert(state.find("\"kind\":\"stage\"") != std::string::npos);
        assert(view.Control(playback, SpectatorStageView::Run(playback).c_str(), "pause") && playback.Paused());
        assert(!view.Control(playback, "old-run", "play") && playback.Paused());
        assert(!view.Control(playback, SpectatorStageView::Run(playback).c_str(), "seek", 601));
    }
    std::puts("PASS spectator stage codec, playback, live-input watchdog eligibility and controls");
}
