#include <eagler/netplay/InputReplay.hpp>
#include <eagler/netplay/NetplayCore.hpp>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>

using namespace Netplay;
using Bytes = std::vector<std::uint8_t>;

static void put(Bytes& bytes, std::size_t offset, std::uint32_t value)
{
    assert(offset + 4 <= bytes.size());
    for (unsigned i = 0; i < 4; ++i) bytes[offset + i] = static_cast<std::uint8_t>(value >> (i * 8));
}
static void seal(Bytes& bytes)
{
    std::uint32_t hash = 2166136261u;
    for (std::size_t i = 0; i < bytes.size(); ++i)
        if (i < 36 || i >= 40) { hash ^= bytes[i]; hash *= 16777619u; }
    put(bytes, 36, hash);
}
static InputReplayConfig config(unsigned count)
{
    InputReplayConfig value; value.gameId = 10; value.gameplayAbi = 0x12345678;
    value.playerCount = static_cast<std::uint8_t>(count);
    value.recordedPlayer = static_cast<std::uint8_t>(count - 1);
    value.description = {1, 3, 5, 7, 9}; return value;
}
static InputReplay::Frame frame(unsigned tick)
{
    InputReplay::Frame result{};
    for (unsigned seat = 0; seat < result.size(); ++seat)
    {
        auto& input = result[seat]; input.buttons = static_cast<std::uint16_t>(tick + 100 * seat);
        input.analogMode = static_cast<AnalogMode>((tick + seat) % 5);
        input.x = tick ? float(tick) / 4 : -0.0f; input.y = -float(seat) / 8;
        input.unlimited = (tick & 1) != 0; input.touchUsed = (tick & 2) != 0;
        input.touchBomb = (tick & 4) != 0;
    }
    return result;
}
static void same(const FrameInput& a, const FrameInput& b)
{
    assert(a == b);
    assert(std::memcmp(&a.x, &b.x, sizeof(float)) == 0);
    assert(std::memcmp(&a.y, &b.y, sizeof(float)) == 0);
}
static Bytes make_file(unsigned count)
{
    InputReplay tape; const auto cfg = config(count); assert(tape.Begin(cfg));
    Bytes out{99}; assert(!tape.Encode(&out) && out == Bytes{99});
    for (unsigned tick = 0; tick < 180; ++tick)
    {
        const auto inputs = frame(tick);
        assert(tape.Append(tick, tick < 90 ? 1 : 2, inputs.data(), count));
    }
    assert(tape.Encode(&out)); assert(tape.Info().frameCount == 180);
    assert(!tape.FrameAt(180));
    const auto input = frame(180); const auto before = out;
    assert(!tape.Append(179, 2, input.data(), count));
    assert(!tape.Append(181, 2, input.data(), count));
    assert(!tape.Append(180, 1, input.data(), count));
    assert(!tape.Append(180, 2, input.data(), count - 1));
    auto invalid = input; invalid[1].x = std::numeric_limits<float>::quiet_NaN();
    assert(!tape.Append(180, 3, invalid.data(), count));
    assert(tape.Encode(&out) && out == before);
    auto bad = cfg; bad.recordedPlayer = bad.playerCount;
    assert(!tape.Begin(bad)); assert(tape.Encode(&out) && out == before);

    InputReplay loaded; assert(loaded.Decode(out.data(), out.size()));
    assert(loaded.Loaded() && !loaded.Recording());
    assert(!loaded.Append(180, 3, input.data(), count));
    assert(loaded.Info().config.recordedPlayer == count - 1);
    assert(loaded.Info().config.description == cfg.description);
    assert(loaded.Info().chapterCount == 2 && loaded.Info().chapters[1].firstFrame == 90);
    for (unsigned tick = 0; tick < 180; ++tick)
    {
        const auto expected = frame(tick);
        for (unsigned seat = 0; seat < count; ++seat) same((*loaded.FrameAt(tick))[seat], expected[seat]);
        for (unsigned seat = count; seat < MAX_PLAYERS; ++seat) same((*loaded.FrameAt(tick))[seat], FrameInput{});
    }
    Bytes encoded; assert(loaded.Encode(&encoded) && encoded == out);
    const Bytes description{2, 4, 6}; assert(loaded.Encode(&encoded, &description));
    InputReplayInfo inspected; assert(InputReplay::Inspect(encoded.data(), encoded.size(), &inspected));
    assert(inspected.config.description == description);
    assert(loaded.Info().config.description == cfg.description);
    assert(loaded.Encode(&encoded,nullptr,90));
    assert(InputReplay::Inspect(encoded.data(),encoded.size(),&inspected));
    assert(inspected.frameCount==90&&inspected.chapterCount==1);
    const auto prefix=encoded;
    assert(!loaded.Encode(&encoded,nullptr,0)&&encoded==prefix);
    assert(!loaded.Encode(&encoded,nullptr,181)&&encoded==prefix);
    return out;
}

static void malformed_files()
{
    const auto original = make_file(3); InputReplay existing;
    assert(existing.Decode(original.data(), original.size()));
    const auto reject = [&](const Bytes& candidate, std::size_t size) {
        InputReplayInfo preview; preview.frameCount = 71; preview.config.gameId = 42;
        assert(!InputReplay::Inspect(candidate.data(), size, &preview));
        assert(preview.frameCount == 71 && preview.config.gameId == 42);
        assert(!existing.Decode(candidate.data(), size));
        Bytes after; assert(existing.Encode(&after) && after == original);
    };
    for (std::size_t length = 0; length < original.size(); ++length) reject(original, length);
    auto bad = original; bad.push_back(0); reject(bad, bad.size());
    bad = original; bad[40] ^= 1; reject(bad, bad.size());
    for (auto offset : {8u, 24u, 28u, 32u})
    { bad = original; put(bad, offset, 0xffffffffu); seal(bad); reject(bad, bad.size()); }
    for (auto offset : {12u, 16u})
    { bad = original; put(bad, offset, 0); seal(bad); reject(bad, bad.size()); }
    for (auto offset : {20u, 21u, 22u, 23u})
    { bad = original; bad[offset] = 9; seal(bad); reject(bad, bad.size()); }
    constexpr std::size_t chapters = 45, samples = chapters + 16;
    bad = original; put(bad, chapters, 0); seal(bad); reject(bad, bad.size());
    bad = original; put(bad, chapters + 4, 1); seal(bad); reject(bad, bad.size());
    bad = original; put(bad, chapters + 8, 1); seal(bad); reject(bad, bad.size());
    for (auto value : {0u, 180u, 0xffffffffu})
    { bad = original; put(bad, chapters + 12, value); seal(bad); reject(bad, bad.size()); }
    bad = original; bad[samples + 2] = 5; seal(bad); reject(bad, bad.size());
    bad = original; bad[samples + 3] = 8; seal(bad); reject(bad, bad.size());
    for (auto bits : {0x7fc00000u, 0x7f800000u, 0xff800000u})
    { bad = original; put(bad, samples + 4, bits); seal(bad); reject(bad, bad.size()); }
    assert(!InputReplay::Inspect(nullptr, original.size(), nullptr));
}

static void confirmed_export()
{
    RollbackCore core; CoreConfig cfg; cfg.sessionId = 42; cfg.playerCount = 3;
    cfg.maxRollbackFrames = 8; assert(core.Reset(cfg));
    InputReplay::Frame out{}; out[0].buttons = 999; const auto untouched = out;
    assert(!core.ConfirmedInputs(0, &out) && out == untouched);
    for (unsigned tick = 0; tick < 3; ++tick)
    {
        assert(core.ScheduleLocalInput(tick, FrameInput(1)));
        assert(core.SubmitRemoteInput(1, tick, FrameInput(2)) == RemoteInputResult::Accepted);
        if (tick != 1) assert(core.SubmitRemoteInput(2, tick, FrameInput(3)) == RemoteInputResult::Accepted);
        assert(core.MarkSimulated(tick, core.PrepareFrame(tick)));
    }
    assert(core.ConfirmedInputs(0, &out));
    out = untouched; assert(!core.ConfirmedInputs(2, &out) && out == untouched);
    assert(core.SubmitRemoteInput(2, 1, FrameInput(9)) == RemoteInputResult::RollbackRequired);
    assert(!core.ConfirmedInputs(1, &out)); assert(!core.ConfirmedInputs(2, &out));
    assert(core.RewindSimulationTo(1)); assert(!core.ConfirmedInputs(1, &out));
    assert(core.MarkSimulated(1, core.PrepareFrame(1))); assert(core.ConfirmedInputs(1, &out));
    assert(out[2].buttons == 9); assert(!core.ConfirmedInputs(2, &out));
    assert(core.MarkSimulated(2, core.PrepareFrame(2))); assert(core.ConfirmedInputs(2, &out));
    assert(out[2].buttons == 3);
    for (unsigned tick = 3; tick < 260; ++tick)
    {
        assert(core.ScheduleLocalInput(tick, FrameInput(1)));
        core.SubmitRemoteInput(1, tick, FrameInput(2)); core.SubmitRemoteInput(2, tick, FrameInput(3));
        assert(core.MarkSimulated(tick, core.PrepareFrame(tick)));
    }
    assert(!core.ConfirmedInputs(0, &out)); assert(core.ConfirmedInputs(259, &out));
    assert(!core.ConfirmedInputs(INVALID_FRAME, &out)); assert(!core.ConfirmedInputs(259, nullptr));
}

int main()
{
    make_file(2); malformed_files(); confirmed_export();
    std::puts("eagler-common all-seat input Replay: PASS");
}
