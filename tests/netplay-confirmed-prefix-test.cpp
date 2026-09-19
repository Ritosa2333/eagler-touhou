#include <eagler/netplay/SnapshotPolicy.hpp>
#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/RollbackJournal.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <vector>

namespace
{
constexpr unsigned Frames = 4200;
struct World
{
    std::uint32_t tick = 0, rng = 0x91ab7231;
    std::array<std::uint32_t, 64> bullets{};
    std::array<std::int32_t, 3> players{};
    bool operator==(const World &other) const
    {
        return tick == other.tick && rng == other.rng && bullets == other.bullets && players == other.players;
    }
};
Netplay::FrameInput Actual(unsigned player, unsigned frame)
{
    Netplay::FrameInput input;
    input.buttons = static_cast<std::uint16_t>(((frame / 7 + player) % 4) | (frame % 41 == 0 ? 8 : 0));
    input.analogMode = frame % 29 == 0 ? Netplay::AnalogMode::DirectTouchBegin : Netplay::AnalogMode::DirectTouchDelta;
    input.x = static_cast<float>(static_cast<int>((frame * 13 + player * 7) % 17) - 8);
    input.y = static_cast<float>(static_cast<int>((frame * 3 + player * 11) % 7) - 3);
    input.touchBomb = frame % 71 == player;
    return input;
}
void Step(World &world, const std::array<Netplay::FrameInput, Netplay::MAX_PLAYERS> &inputs, unsigned players)
{
    for (unsigned p = 0; p < players; ++p)
    {
        const auto &input = inputs[p];
        world.players[p] += static_cast<std::int32_t>(input.x) + static_cast<std::int32_t>(input.y);
        world.rng = world.rng * 1664525u + 1013904223u + input.buttons + static_cast<unsigned>(input.touchBomb);
    }
    for (unsigned i = 0; i < world.bullets.size(); ++i)
        world.bullets[i] = (world.bullets[i] * 73u + world.rng) ^ static_cast<std::uint32_t>(world.players[i % players]);
    ++world.tick;
}
struct Delivery { unsigned time, player, frame; };
struct Totals { unsigned captured = 0, skipped = 0, corrected = 0, replayed = 0; };

Totals Run(unsigned mode, unsigned players, unsigned scenario)
{
    auto core = std::make_unique<Netplay::RollbackCore>();
    Netplay::CoreConfig config;
    config.playerCount = static_cast<std::uint8_t>(players);
    config.localPlayer = 0;
    config.maxRollbackFrames = 12;
    config.predictableButtons = 3;
    assert(core->Reset(config));
    Netplay::RollbackJournal journal;
    assert(journal.Reset({8, sizeof(World) * 2, 8}));
    std::vector<World> expected(Frames);
    World reference;
    std::vector<Delivery> network;
    for (unsigned frame = 0; frame < Frames; ++frame)
    {
        std::array<Netplay::FrameInput, Netplay::MAX_PLAYERS> input{};
        for (unsigned p = 0; p < players; ++p) input[p] = Actual(p, frame);
        Step(reference, input, players);
        expected[frame] = reference;
        for (unsigned p = 1; p < players; ++p)
        {
            const unsigned phase = (frame / 53 + p + scenario) % 5;
            // Exact stretches, delayed bursts, reordered holes and >window waits.
            unsigned delay = phase == 0 ? 0 : phase == 1 ? 1 : phase == 2 ? 5 : phase == 3 ? 9 : 15;
            delay += frame % 13 == p ? 4 : 0;
            network.push_back({frame + delay, p, frame});
            if (frame % 19 == 0) network.push_back({frame + delay + 1, p, frame});
        }
    }
    std::stable_sort(network.begin(), network.end(), [](const auto &a, const auto &b) { return a.time < b.time; });
    World world;
    unsigned sim = 0, phase = 0;
    std::size_t next = 0;
    Totals totals;
    auto confirmed = [&]() {
        unsigned result = Netplay::INVALID_FRAME;
        for (unsigned p = 1; p < players; ++p)
        {
            const auto current = core->ConfirmedThrough(static_cast<std::uint8_t>(p));
            if (current == Netplay::INVALID_FRAME) return current;
            result = std::min(result, current);
        }
        return result;
    };
    auto simulate = [&](unsigned f, bool resim) {
        const auto decision = core->PrepareFrame(f);
        assert(decision.canAdvance && !core->HasRollbackRequest());
        const auto prefix = confirmed();
        const bool capture = mode == 0 || Netplay::NeedsRollbackSnapshot(f, prefix, decision.predictedMask, resim, mode == 2);
        if (capture)
        {
            assert(journal.BeginFrame(f, phase != 0));
            assert(journal.Touch(&world, sizeof(world)));
            ++totals.captured;
        }
        else
        {
            journal.DiscardBefore(f);
            assert(journal.FrameCount() == 0);
            phase = 0;
            ++totals.skipped;
        }
        Step(world, decision.inputs, players);
        if (capture) { assert(journal.EndFrame()); phase = (phase + 1) % 2; }
        assert(core->MarkSimulated(f, decision));
        if (prefix != Netplay::INVALID_FRAME && f <= prefix) assert(world == expected[f]);
        if (resim) ++totals.replayed;
    };
    for (unsigned wall = 0; wall < Frames + 100; ++wall)
    {
        if (wall < Frames) assert(core->ScheduleLocalInput(wall, Actual(0, wall)));
        while (next < network.size() && network[next].time <= wall)
        {
            const auto packet = network[next++];
            const auto status = core->SubmitRemoteInput(static_cast<std::uint8_t>(packet.player), packet.frame, Actual(packet.player, packet.frame));
            assert(status != Netplay::RemoteInputResult::ConflictingConfirmedInput);
            assert(status != Netplay::RemoteInputResult::TooOld && status != Netplay::RemoteInputResult::InvalidPlayer);
        }
        if (core->HasRollbackRequest())
        {
            std::uint32_t from = core->RollbackFrame();
            assert(journal.UndoTo(from, &from));
            phase = 0;
            core->ClearRollbackRequest();
            ++totals.corrected;
            for (unsigned f = from; f < sim; ++f) simulate(f, true);
        }
        if (sim < Frames && core->PrepareFrame(sim).canAdvance) simulate(sim++, false);
        if (sim == Frames && next == network.size()) break;
    }
    assert(sim == Frames && next == network.size() && !core->HasRollbackRequest());
    assert(world == expected.back() && totals.corrected > 100);
    return totals;
}
} // namespace

int main()
{
    for (unsigned players : {2u, 3u})
        for (unsigned scenario : {0u, 1u, 2u})
        {
            const auto always = Run(0, players, scenario);
            const auto conservative = Run(1, players, scenario);
            const auto frontier = Run(2, players, scenario);
            assert(frontier.captured < conservative.captured && frontier.captured < always.captured);
            std::printf("prefix %uP/%u: saved ticks always=%u demand=%u frontier=%u; corrections=%u replay=%u\n",
                        players, scenario, always.captured, conservative.captured, frontier.captured, frontier.corrected, frontier.replayed);
        }
    std::puts("TH07 confirmed-prefix rollback: PASS (18 runs x 4200 frames)");
}
