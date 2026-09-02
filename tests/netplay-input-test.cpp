#include "netplay/NetplayInput.hpp"
#include "Supervisor.hpp"

#include <cassert>
#include <iostream>

u16 g_CurFrameInput = 0;
u16 g_CurFrameGameInputs[TH06_MULTI_MAX_PLAYERS] = {};
u16 g_LastFrameInput = 0;
u16 g_LastFrameGameInputs[TH06_MULTI_MAX_PLAYERS] = {};
u16 g_IsEighthFrameOfHeldInput = 0;
u16 g_NumOfFramesInputsWereHeld = 0;

int main()
{
    Netplay::FrameInput inputs[3];
    inputs[0].buttons = 0x10;
    inputs[1].buttons = 0x20;
    inputs[1].analogMode = Netplay::AnalogMode::DirectTouch;
    inputs[1].x = 6.5f;
    inputs[1].y = -2.25f;
    inputs[1].unlimited = true;
    inputs[1].touchUsed = true;
    inputs[1].touchBomb = true;

    Netplay::Input::SetPlayerInputOverrides(inputs, 2);
    assert(g_CurFrameGameInputs[0] == 0x10);
    assert(g_CurFrameGameInputs[1] == 0x20);

    float x = 0.0f;
    float y = 0.0f;
    bool unlimited = false;
    assert(!Netplay::Input::ReplayDirectTouch(0, &x, &y, &unlimited));
    assert(Netplay::Input::ReplayDirectTouch(1, &x, &y, &unlimited));
    assert(x == 6.5f && y == -2.25f && unlimited);
    assert(!Netplay::Input::PlayerTouchUsed(0));
    assert(Netplay::Input::PlayerTouchUsed(1));
    assert(Netplay::Input::PlayerTouchBomb(1));
    assert(!Netplay::Input::ReplayDirectTouch(2, &x, &y, &unlimited));

    Netplay::Input::ClearPlayerButtonOverrides();
    assert(!Netplay::Input::PlayerButtonOverridesActive());
    assert(!Netplay::Input::ReplayDirectTouch(1, &x, &y, &unlimited));
    std::cout << "TH06 netplay input lanes: PASS\n";
    return 0;
}
