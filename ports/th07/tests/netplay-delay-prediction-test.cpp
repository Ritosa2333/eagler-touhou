#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/DirectTouchState.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

using namespace Netplay;

static FrameInput Trace(unsigned player, unsigned frame, unsigned kind)
{
    FrameInput input(static_cast<std::uint16_t>((frame / 40 + player) % 2 + 1));
    input.analogMode = AnalogMode::DirectTouch;
    input.touchUsed = true;
    input.unlimited = (frame / 97) % 2 != 0;
    input.x = player == 0 ? 0.5f : -0.5f;
    input.y = 0.125f;
    const unsigned phase = (frame + player * 13) % 300;
    if (kind == 1)
    {
        input.x += (static_cast<int>(phase % 17) - 8) * 0.015625f;
        input.y = (static_cast<int>(phase % 11) - 5) * 0.03125f;
    }
    if (kind == 2 && phase >= 120) input.x = input.y = 0;
    if (kind == 3 && phase >= 150) { input.x = -input.x; input.y = -input.y; }
    if (kind == 4) { input.x *= phase % 3 == 0 ? 3 : 0; input.y *= phase % 3 == 0 ? 3 : 0; }
    if (kind >= 5)
    {
        // Same physical 24-unit drag every ten ticks, speed capped at four.
        // Legacy sends the shrinking pending snapshot; fresh sends each event
        // once and leaves the six-tick remainder in the simulation.
        input.unlimited = false;
        input.y = 0;
        const float sign = player == 0 ? 1.0f : -1.0f;
        input.x = sign * (kind == 5 ? std::max(0, 24 - static_cast<int>(frame % 10) * 4)
                                    : (frame % 10 == 0 ? 24 : 0));
        if (kind == 6)
            input.analogMode = frame == 0 ? AnalogMode::DirectTouchBegin : AnalogMode::DirectTouchDelta;
    }
    if (frame % 173 == 0) { input.buttons |= 4; input.touchBomb = true; }
    return input;
}

static void TestCaptureAddressingAndPrediction()
{
    RollbackCore core;
    assert(core.LocalFrameForCapture(0) == INVALID_FRAME && !core.HasLocalCapture(0));
    CoreConfig config;
    config.inputDelay = 3;
    config.predictableButtons = 3;
    assert(core.Reset(config));
    assert(core.LocalFrameForCapture(0) == 3);
    assert(!core.HasLocalCapture(0)); // neutral frame 0 is not captured frame 3
    for (unsigned capture = 0; capture < 10; ++capture)
    {
        assert(!core.HasLocalCapture(capture));
        const auto input = Trace(0, capture, 0);
        assert(core.ScheduleLocalInput(capture, input));
        assert(core.HasLocalCapture(capture));
        assert(core.ScheduleLocalInput(capture, input));
        auto different = input; different.x += 1;
        assert(!core.ScheduleLocalInput(capture, different));
        assert(core.LocalInput(capture + 3) == input);
    }
    assert(!core.ScheduleLocalInput(INVALID_FRAME - 3, {}));
    assert(core.LocalFrameForCapture(INVALID_FRAME - 4) == INVALID_FRAME - 1);
    auto held = Trace(1, 0, 0);
    assert(core.SubmitRemoteInput(1, 0, held) == RemoteInputResult::Accepted);
    assert(core.SubmitRemoteInput(1, 1, held) == RemoteInputResult::Accepted);
    auto predicted = core.PrepareFrame(2).inputs[1];
    assert(predicted.x == held.x && predicted.y == held.y);
    assert(!predicted.touchBomb && (predicted.buttons & 4) == 0);
    predicted = core.PrepareFrame(3).inputs[1];
    assert(predicted.x == 0 && predicted.y == 0);
    predicted = core.PrepareFrame(4).inputs[1];
    assert(predicted.x == 0 && predicted.y == 0);
    held.x = held.y = 0;
    assert(core.SubmitRemoteInput(1, 2, held) == RemoteInputResult::Accepted);
    predicted = core.PrepareFrame(3).inputs[1];
    assert(predicted.x == 0 && predicted.y == 0);
    core.Clear();
    assert(!core.HasLocalCapture(0));
}

struct World
{
    std::array<float, 3> x{}, y{};
    std::array<DirectTouchState, 3> touch{};
    std::uint64_t digest = 1469598103934665603ull;
    bool operator==(const World &other) const
    {
        if (x != other.x || y != other.y || digest != other.digest) return false;
        for (unsigned i = 0; i < touch.size(); ++i)
            if (touch[i].x != other.touch[i].x || touch[i].y != other.touch[i].y ||
                touch[i].active != other.touch[i].active) return false;
        return true;
    }
    void Step(const FrameDecision &decision, unsigned players)
    {
        assert(decision.canAdvance);
        for (unsigned player = 0; player < players; ++player)
        {
            const auto &input = decision.inputs[player];
            touch[player].Apply(input);
            const float dx = std::clamp(touch[player].active ? touch[player].x : input.x, -4.0f, 4.0f);
            const float dy = std::clamp(touch[player].active ? touch[player].y : input.y, -4.0f, 4.0f);
            x[player] += dx; y[player] += dy;
            if (touch[player].active) touch[player].Consume(dx, dy);
            digest = (digest ^ input.buttons) * 1099511628211ull;
            digest = (digest ^ input.touchBomb) * 1099511628211ull;
            digest = (digest ^ input.unlimited) * 1099511628211ull;
        }
    }
};

struct Endpoint
{
    RollbackCore core;
    unsigned frame = 0, captures = 0, lastSent = 0, rollbacks = 0, resimulated = 0;
    World world;
    std::vector<World> before;
};

struct Delivery { unsigned tick, peer; InputPacket packet; };
struct Result { unsigned rollbacks = 0, resimulated = 0, ticks = 0; };

// Exercise real packet redundancy/ACKs, loss, reordering, mixed per-peer
// delays, the frame-zero barrier, ring wrap, correction and tail drainage.
static Result Run(unsigned players, std::array<unsigned, 3> delays,
                  unsigned kind, unsigned oneWay, bool jitter, bool drops,
                  unsigned predictionWindow = 12)
{
    constexpr unsigned frames = 1200;
    auto peers = std::make_unique<std::array<Endpoint, 3>>();
    std::vector<Delivery> queue;
    unsigned sequence = 1;
    for (unsigned id = 0; id < players; ++id)
    {
        CoreConfig config;
        config.sessionId = 0x12345678; config.playerCount = players; config.localPlayer = id;
        config.inputDelay = delays[id]; config.maxRollbackFrames = predictionWindow;
        config.predictableButtons = 3;
        assert((*peers)[id].core.Reset(config));
        (*peers)[id].before.resize(frames);
    }
    Result result;
    bool finished = false;
    for (unsigned tick = 0; tick < frames * 8; ++tick)
    {
        for (auto it = queue.begin(); it != queue.end();)
        {
            if (it->tick > tick) { ++it; continue; }
            assert((*peers)[it->peer].core.ApplyInputPacket(it->packet));
            it = queue.erase(it);
        }
        finished = true;
        for (unsigned id = 0; id < players; ++id)
        {
            auto &peer = (*peers)[id];
            if (peer.core.HasRollbackRequest())
            {
                const auto from = peer.core.RollbackFrame();
                assert(from < peer.frame);
                peer.world = peer.before[from];
                peer.core.ClearRollbackRequest();
                ++peer.rollbacks;
                for (unsigned frame = from; frame < peer.frame; ++frame)
                {
                    auto decision = peer.core.PrepareFrame(frame);
                    peer.before[frame] = peer.world;
                    peer.world.Step(decision, players);
                    assert(peer.core.MarkSimulated(frame, decision));
                    ++peer.resimulated;
                }
            }
            bool newlyCaptured = false;
            if (peer.frame < frames && !peer.core.HasLocalCapture(peer.frame))
            {
                assert(peer.core.ScheduleLocalInput(peer.frame, Trace(id, peer.frame, kind)));
                ++peer.captures;
                newlyCaptured = true;
                peer.lastSent = peer.core.LocalFrameForCapture(peer.frame);
            }
            if (newlyCaptured || tick % 3 == 0)
                for (unsigned target = 0; target < players; ++target)
                {
                    if (target == id) continue;
                    auto packet = peer.core.BuildInputPacket(target, peer.lastSent, sequence++, 0);
                    std::vector<std::uint8_t> bytes;
                    assert(EncodeInputPacket(packet, &bytes));
                    InputPacket decoded;
                    assert(DecodeInputPacket(bytes.data(), bytes.size(), &decoded));
                    if (drops && sequence % 13 == 0) continue;
                    const unsigned delay = oneWay + (jitter ? sequence % 3 : 0);
                    queue.push_back({tick + delay, target, decoded});
                }
            bool firstRemoteReady = true, allConfirmed = true;
            for (unsigned remote = 0; remote < players; ++remote)
            {
                if (remote == id) continue;
                const auto confirmed = peer.core.ConfirmedThrough(remote);
                firstRemoteReady &= confirmed != INVALID_FRAME;
                allConfirmed &= confirmed != INVALID_FRAME && confirmed >= frames - 1;
            }
            if (peer.frame < frames && (peer.frame != 0 || firstRemoteReady))
            {
                const auto decision = peer.core.PrepareFrame(peer.frame);
                if (decision.canAdvance)
                {
                    peer.before[peer.frame] = peer.world;
                    peer.world.Step(decision, players);
                    assert(peer.core.MarkSimulated(peer.frame, decision));
                    ++peer.frame;
                }
            }
            finished &= peer.frame == frames && allConfirmed && !peer.core.HasRollbackRequest();
        }
        result.ticks = tick + 1;
        if (finished) break;
    }
    assert(finished);
    World reference;
    for (unsigned frame = 0; frame < frames; ++frame)
    {
        FrameDecision decision; decision.canAdvance = true;
        for (unsigned id = 0; id < players; ++id)
            if (frame >= delays[id]) decision.inputs[id] = Trace(id, frame - delays[id], kind);
        reference.Step(decision, players);
    }
    for (unsigned id = 0; id < players; ++id)
    {
        const auto &peer = (*peers)[id];
        assert(peer.captures == frames); // stalled/retransmitted ticks never recapture
        assert(peer.world == reference);
        result.rollbacks += peer.rollbacks; result.resimulated += peer.resimulated;
    }
    return result;
}

int main(int argc, char **argv)
{
    TestCaptureAddressingAndPrediction();
    for (unsigned kind = 0; kind < 7; ++kind)
    {
        Run(2, {0, 3, 0}, kind, 3, true, true);
        Run(3, {0, 3, 6}, kind, 3, true, true);
        Run(3, {3, 3, 3}, kind, 3, true, true, 4);
    }
    if (argc > 1 && std::strcmp(argv[1], "--study") == 0)
        for (unsigned kind = 0; kind < 7; ++kind)
            for (unsigned delay : {0u, 3u, 4u})
            {
                const auto r = Run(2, {delay, delay, 0}, kind, 3, true, false);
                std::printf("{\"trace\":%u,\"delay\":%u,\"rollbacks\":%u,\"resimulated\":%u,\"ticks\":%u}\n",
                            kind, delay, r.rollbacks, r.resimulated, r.ticks);
            }
    std::puts("TH07 delayed input and bounded prediction: PASS");
}
