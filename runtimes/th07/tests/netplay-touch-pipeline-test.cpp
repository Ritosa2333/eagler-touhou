#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/NetplayInput.hpp>
#include <eagler/netplay/RollbackJournal.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>

using namespace Netplay;

static FrameInput Delta(float x, float y = 0, bool begin = false)
{
    FrameInput input;
    input.analogMode = begin ? AnalogMode::DirectTouchBegin : AnalogMode::DirectTouchDelta;
    input.x = x; input.y = y; input.touchUsed = true;
    return input;
}

static void TestCodecAndPrediction()
{
    InputPacket packet;
    packet.playerCount = 2; packet.senderPlayer = 1;
    packet.firstInputFrame = 0; packet.latestFrame = 1; packet.inputCount = 2;
    packet.inputs[0] = Delta(15, -2, true);
    packet.inputs[0].touchBomb = true;
    packet.inputs[1] = Delta(0);
    std::vector<std::uint8_t> bytes;
    assert(EncodeInputPacket(packet, &bytes));
    InputPacket decoded;
    assert(DecodeInputPacket(bytes.data(), bytes.size(), &decoded));
    assert(decoded.inputs[0] == packet.inputs[0] && decoded.inputs[1] == packet.inputs[1]);

    RollbackCore core;
    assert(core.Reset({}));
    assert(core.ScheduleLocalInput(0, FrameInput{}));
    assert(core.ScheduleLocalInput(1, FrameInput{}));
    assert(core.SubmitRemoteInput(1, 0, packet.inputs[0]) == RemoteInputResult::Accepted);
    auto prediction = core.PrepareFrame(1);
    assert(prediction.canAdvance && prediction.predictedMask == 2);
    const auto &input = prediction.inputs[1];
    assert(input.analogMode == AnalogMode::DirectTouchDelta && input.x == 0 && input.y == 0);
    assert(!input.touchBomb); // never repeat a begin/reset or an edge action
    assert(core.MarkSimulated(1, prediction));
    assert(core.SubmitRemoteInput(1, 1, packet.inputs[1]) == RemoteInputResult::PredictionCorrect);

    // Legacy snapshots remain a distinct supported input representation.
    packet.inputs[0].analogMode = AnalogMode::DirectTouch;
    assert(EncodeInputPacket(packet, &bytes));
    assert(DecodeInputPacket(bytes.data(), bytes.size(), &decoded));
    assert(decoded.inputs[0].analogMode == AnalogMode::DirectTouch);
    packet.inputs[0].analogMode = static_cast<AnalogMode>(5);
    assert(!EncodeInputPacket(packet, &bytes));
}

static void TestGestureRemainderAndModes()
{
    Input::ResetDirectTouchStates();
    std::array<FrameInput, 3> input{Delta(15, -4, true), Delta(-9, 2, true), {}};
    Input::SetPlayerInputOverrides(input.data(), 3);
    float x = 0, y = 0; bool unlimited = true;
    assert(Input::ReplayDirectTouch(0, &x, &y, &unlimited) && x == 15 && y == -4 && !unlimited);
    Input::ConsumeDirectTouchRemainder(0, 4, -1);
    Input::ClearPlayerButtonOverrides();
    assert(Input::GetDirectTouchStates()[0].x == 11); // not transient override state
    input[0] = Delta(0); input[1] = Delta(0);
    Input::SetPlayerInputOverrides(input.data(), 3);
    assert(Input::ReplayDirectTouch(0, &x, &y, &unlimited) && x == 11 && y == -3);
    assert(Input::GetDirectTouchStates()[1].x == -9); // per-player isolation
    input[0] = Delta(-2, 1, true); // release/new DOWN between adjacent capture ticks
    Input::SetPlayerInputOverrides(input.data(), 3);
    assert(Input::ReplayDirectTouch(0, &x, &y, &unlimited) && x == -2 && y == 1);
    input[0] = {}; // finger release, cancel, or switching to joystick
    Input::SetPlayerInputOverrides(input.data(), 3);
    assert(!Input::ReplayDirectTouch(0, &x, &y, &unlimited));
    assert(!Input::GetDirectTouchStates()[0].active && Input::GetDirectTouchStates()[0].x == 0);
    input[0] = Delta(6); input[0].analogMode = AnalogMode::DirectTouch;
    Input::SetPlayerInputOverrides(input.data(), 3);
    assert(Input::ReplayDirectTouch(0, &x, &y, &unlimited) && x == 6);
    assert(!Input::UsesIncrementalDirectTouch(0));
    Input::ResetDirectTouchStates();
    Input::ClearPlayerButtonOverrides();
}

static void TestDelayedCaptureWithRollback()
{
    CoreConfig config; config.inputDelay = 3;
    RollbackCore core; assert(core.Reset(config));
    RollbackJournal journal; assert(journal.Reset({20, 1024, 8}));
    Input::ResetDirectTouchStates();
    float position = 0;
    const auto step = [&](unsigned frame) {
        const auto decision = core.PrepareFrame(frame);
        assert(decision.canAdvance);
        assert(journal.BeginFrame(frame));
        assert(journal.Touch(&position, sizeof(position)));
        assert(journal.Touch(&Input::GetDirectTouchStates(), sizeof(Input::DirectTouchStates)));
        Input::SetPlayerInputOverrides(decision.inputs.data(), 2);
        float x = 0, y = 0; bool unlimited = false;
        if (Input::ReplayDirectTouch(0, &x, &y, &unlimited))
        {
            // Small independent movement fixture: speed cap 4, right edge 100.
            const float wanted = std::min(x, 100 - position);
            Input::SetDirectTouchRemainder(0, wanted, y);
            const float consumed = std::clamp(wanted, -4.0f, 4.0f);
            position += consumed;
            Input::ConsumeDirectTouchRemainder(0, consumed, 0);
        }
        Input::ClearPlayerButtonOverrides();
        assert(journal.EndFrame());
        assert(core.MarkSimulated(frame, decision));
    };
    for (unsigned frame = 0; frame < 13; ++frame)
    {
        FrameInput fresh = Delta(0);
        if (frame == 0) fresh = Delta(10, 0, true);
        if (frame == 3) fresh = Delta(4); // new input while the delayed old sample is consumed
        if (frame == 7) fresh = Delta(20);
        if (frame == 8) fresh = {}; // release cancels the older unfinished request
        if (frame == 9) fresh = Delta(2, 0, true);
        assert(!core.HasLocalCapture(frame));
        assert(core.ScheduleLocalInput(frame, fresh));
        assert(core.HasLocalCapture(frame));
        assert(core.ScheduleLocalInput(frame, fresh)); // retry does not add another sample
        assert(core.SubmitRemoteInput(1, frame, FrameInput{}) == RemoteInputResult::Accepted);
        step(frame);
        if (frame < 3) assert(position == 0);
        if (frame == 3) assert(position == 4);
        if (frame == 4) assert(position == 8);
        if (frame == 5) assert(position == 10);
        if (frame == 6) assert(position == 14);
        if (frame == 10) assert(position == 18);
        if (frame == 11) assert(position == 18 && !Input::GetDirectTouchStates()[0].active);
        if (frame == 12) assert(position == 20);
    }
    assert(journal.UndoTo(4));
    assert(position == 4 && Input::GetDirectTouchStates()[0].x == 6);
    for (unsigned frame = 4; frame < 13; ++frame) step(frame);
    assert(position == 20 && Input::GetDirectTouchStates()[0].x == 0);
    Input::ResetDirectTouchStates();
}

int main()
{
    TestCodecAndPrediction();
    TestGestureRemainderAndModes();
    TestDelayedCaptureWithRollback();
    std::puts("TH07 once-only delayed touch pipeline: PASS");
}
