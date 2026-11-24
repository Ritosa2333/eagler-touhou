#include "../../ecl/ecl_test_fixtures.hpp"
#include "multiplayer/InputLanes.hpp"
#include "multiplayer/NetplayRuntime.hpp"
#include "multiplayer/SessionSetup.hpp"

using namespace th13;
using namespace th13::multiplayer;

namespace {

SessionSetup validSetup() {
    SessionSetup setup;
    setup.session_id = 0x1234'5678'9abc'def0ull;
    setup.player_count = 2;
    setup.local_player = 0;
    setup.difficulty = 2;
    setup.seed = 47932;
    setup.automatic = false;
    setup.requested_delay = setup.input_delay = 3;
    setup.selections = {0, 2, 0};
    setup.build = {1, 2, 3, 4};
    return setup;
}

}

TEST(contract_asset_mismatch) {
    const auto roundtrip = [](const SessionSetup& setup) {
        const auto words = EncodeSessionSetup(setup);
        SessionSetup decoded;
        if (!DecodeSessionSetup(decoded, words.data(), words.size())) return false;
        return decoded.valid() && GameplayAbi(decoded) == GameplayAbi(setup);
    };
    CHECK(roundtrip(validSetup()));
    {
        const auto words = EncodeSessionSetup(validSetup());
        SessionSetup decoded;
        CHECK(!DecodeSessionSetup(decoded, words.data(), words.size() - 1));
        CHECK(!DecodeSessionSetup(decoded, words.data(), words.size() + 1));
        CHECK(!DecodeSessionSetup(decoded, nullptr, words.size()));
    }
    {
        auto setup = validSetup();
        setup.version = 99;
        SessionSetup decoded;
        CHECK(!DecodeSessionSetup(decoded, EncodeSessionSetup(setup).data(),
                                  SessionSetup::Words));
    }
    {
        auto setup = validSetup();
        setup.difficulty = 6;
        SessionSetup decoded;
        CHECK(!DecodeSessionSetup(decoded, EncodeSessionSetup(setup).data(),
                                  SessionSetup::Words));
    }
    {
        auto setup = validSetup();
        setup.selections = {4, 0, 0};
        SessionSetup decoded;
        CHECK(!DecodeSessionSetup(decoded, EncodeSessionSetup(setup).data(),
                                  SessionSetup::Words));
    }
    {
        auto setup = validSetup();
        setup.player_count = 1;
        SessionSetup decoded;
        CHECK(!DecodeSessionSetup(decoded, EncodeSessionSetup(setup).data(),
                                  SessionSetup::Words));
    }
    {
        auto setup = validSetup();
        setup.player_count = 4;
        SessionSetup decoded;
        CHECK(!DecodeSessionSetup(decoded, EncodeSessionSetup(setup).data(),
                                  SessionSetup::Words));
    }
    {
        auto setup = validSetup();
        setup.local_player = 2;
        SessionSetup decoded;
        CHECK(!DecodeSessionSetup(decoded, EncodeSessionSetup(setup).data(),
                                  SessionSetup::Words));
    }
    {
        auto setup = validSetup();
        setup.session_id = 0;
        SessionSetup decoded;
        CHECK(!DecodeSessionSetup(decoded, EncodeSessionSetup(setup).data(),
                                  SessionSetup::Words));
    }
    {
        auto setup = validSetup();
        setup.build = {0, 0, 0, 0};
        SessionSetup decoded;
        CHECK(!DecodeSessionSetup(decoded, EncodeSessionSetup(setup).data(),
                                  SessionSetup::Words));
    }
    {
        auto setup = validSetup();
        setup.selections = {0, 2, 1};
        SessionSetup decoded;
        CHECK(!DecodeSessionSetup(decoded, EncodeSessionSetup(setup).data(),
                                  SessionSetup::Words));
    }
    {
        auto setup = validSetup();
        setup.version = SessionSetup::LegacyVersion;
        setup.mode = 2;
        SessionSetup decoded;
        CHECK(!DecodeSessionSetup(decoded, EncodeSessionSetup(setup).data(),
                                  SessionSetup::Words));
    }
    {
        const auto base = validSetup();
        auto other = base;
        other.difficulty = 3;
        CHECK(GameplayAbi(base) != GameplayAbi(other));
        auto same = base;
        CHECK(GameplayAbi(base) == GameplayAbi(same));
        auto otherBuild = base;
        otherBuild.build[0] = 9;
        CHECK(GameplayAbi(base) != GameplayAbi(otherBuild));
    }
}

TEST(frame_zero_load_barrier) {
    NetplayRuntime runtime;
    CHECK(runtime.Reset(validSetup()));
    CHECK(runtime.CaptureFrame() == 0);
    Netplay::FrameInput neutral;
    CHECK(runtime.Capture(runtime.CaptureFrame(), neutral, 0));
    runtime.AdvanceCapturePhase();
    CHECK(runtime.CaptureFrame() == 1);
    CHECK(!runtime.CanStart());
    CHECK(!runtime.LocalReady());
    runtime.MarkLocalReady();
    CHECK(!runtime.LocalReady());
    const auto decision = runtime.Prepare(0);
    CHECK(!decision.canAdvance);
    CHECK(decision.predictedMask == 0);
}

TEST(device_seat_isolation) {
    NetplayRuntime device_a, device_b;
    auto setup_a = validSetup();
    auto setup_b = validSetup();
    setup_b.local_player = 1;
    CHECK(device_a.Reset(setup_a));
    CHECK(device_b.Reset(setup_b));
    CHECK(device_a.LocalSeat() == 0);
    CHECK(device_b.LocalSeat() == 1);
    Netplay::FrameInput input_a, input_b;
    CHECK(InputLanes::Capture(0x001, false, input_a));
    CHECK(InputLanes::Capture(0x008, false, input_b));
    CHECK((input_a.buttons & InputLanes::HeldMask) == 0x001);
    CHECK((input_b.buttons & InputLanes::HeldMask) == 0x008);
    Netplay::FrameInput input_trance;
    CHECK(InputLanes::Capture(0xa00, false, input_trance));
    CHECK((input_trance.buttons & InputLanes::HeldMask) == 0x800);
    Netplay::FrameInput rejected;
    CHECK(!InputLanes::Capture(0x40000000u, false, rejected));
    std::array<Netplay::FrameInput, 3> frames{input_a, input_b, Netplay::FrameInput{}};
    std::array<u32, 3> held{};
    CHECK(InputLanes::Decode(frames.data(), 2, held));
    CHECK(held[0] == 0x001);
    CHECK(held[1] == 0x008);
    CHECK(held[2] == 0);
    frames[1].buttons |= InputLanes::Restart | InputLanes::Quit;
    CHECK(InputLanes::Decode(frames.data(), 2, held));
    CHECK(held[1] == 0x008);
    CHECK((InputLanes::Controls(frames[1]) & 0x210000u) == 0x210000u);
    Netplay::FrameInput dirty = input_a;
    dirty.buttons |= 0x400;
    std::array<Netplay::FrameInput, 3> bad{dirty, input_b, Netplay::FrameInput{}};
    CHECK(!InputLanes::Decode(bad.data(), 2, held));
}

TEST(confirmed_hash_lockstep) {
    NetplayRuntime host_a, host_b;
    auto setup_a = validSetup();
    auto setup_b = validSetup();
    setup_b.local_player = 1;
    CHECK(host_a.Reset(setup_a));
    CHECK(host_b.Reset(setup_b));
    Netplay::FrameInput inputs_a[10], inputs_b[10];
    for (unsigned frame = 0; frame < 10; ++frame) {
        CHECK(InputLanes::Capture(0x001u | (frame & 7u), false, inputs_a[frame]));
        CHECK(InputLanes::Capture(0x008u | (frame & 3u), false, inputs_b[frame]));
    }
    for (unsigned frame = 0; frame < 10; ++frame) {
        CHECK(host_a.Capture(frame, inputs_a[frame], 0));
        CHECK(host_b.Capture(frame, inputs_b[frame], 0));
        const auto decision_a = host_a.Prepare(frame);
        CHECK(!decision_a.canAdvance);
    }
    CHECK(host_a.ConfirmedThroughAll() == Netplay::INVALID_FRAME);
    CHECK(host_a.LastSimulated() == Netplay::INVALID_FRAME);
}

int main() { return th13test::run_all("m7.netplay"); }
