#include <eagler/netplay/NetplayInput.hpp>
#include <netplay/NetplayInputConfig.hpp>

#include <array>
#include <cassert>
#include <cstdint>

int main()
{
    using Netplay::AnalogMode;
    using Netplay::FrameInput;
    namespace Input = Netplay::Input;
    namespace Config = Netplay::InputConfig;

    Input::ClearReplayOverride();
    Input::ClearPlayerButtonOverrides();

    Input::BeginCapture();
    assert(Input::CaptureActive());
    assert(Input::ResolveLocal(0x1234) == 0x1234);
    Input::CaptureJoystick(1.25f, -2.5f);
    const FrameInput captured = Input::EndCapture();
    assert(!Input::CaptureActive());
    assert(captured.buttons == 0x1234);
    assert(captured.analogMode == AnalogMode::Joystick);
    assert(captured.x == 1.25f && captured.y == -2.5f);
    assert(!captured.unlimited);

    FrameInput replay{};
    replay.buttons = 0x55aa;
    replay.analogMode = AnalogMode::DirectTouch;
    replay.x = 9.0f;
    replay.y = -4.0f;
    replay.unlimited = true;
    Input::SetReplayOverride(replay);
    assert(Input::ReplayOverrideActive());
    assert(Input::ResolveLocal(0xffff) == 0x55aa);
    Input::ClearReplayOverride();
    assert(!Input::ReplayOverrideActive());

    std::array<FrameInput, Netplay::MAX_PLAYERS> players{};
    players[0].buttons = 0x0001;
    players[0].analogMode = AnalogMode::Joystick;
    players[0].x = 2.0f;
    players[0].y = 3.0f;
    players[1].buttons = 0x0002;
    players[1].analogMode = AnalogMode::DirectTouch;
    players[1].x = 4.0f;
    players[1].y = 5.0f;
    players[1].unlimited = true;
    players[1].touchUsed = true;
    players[1].touchBomb = true;

    Config::ResetFixture();
    Input::SetPlayerInputOverrides(players.data(), players.size());
    assert(Input::PlayerButtonOverridesActive());
    assert(Config::commitCount == 1);
    assert(Config::committed[0] == 0x0001);
    assert(Config::committed[1] == 0x0002);

    float x = 0.0f;
    float y = 0.0f;
    bool unlimited = false;
    assert(Input::ReplayJoystick(0, &x, &y));
    assert(x == 2.0f && y == 3.0f);
    assert(Input::ReplayDirectTouch(1, &x, &y, &unlimited));
    assert(x == 4.0f && y == 5.0f && unlimited);
    assert(Input::PlayerTouchUsed(1));
    assert(Input::PlayerTouchBomb(1));
    assert(!Input::ReplayJoystick(Netplay::MAX_PLAYERS, nullptr, nullptr));

    std::array<std::uint16_t, Netplay::MAX_PLAYERS + 2> buttons{};
    for (std::size_t i = 0; i < buttons.size(); ++i)
        buttons[i] = static_cast<std::uint16_t>(10 + i);
    Config::ResetFixture();
    Input::SetPlayerButtonOverrides(buttons.data(), buttons.size());
    assert(Config::commitCount == 1);
    for (std::size_t i = 0; i < Netplay::MAX_PLAYERS; ++i)
        assert(Input::PlayerButtonOverride(i) == buttons[i]);

    Input::ClearPlayerButtonOverrides();
    assert(!Input::PlayerButtonOverridesActive());
    assert(Input::PlayerButtonOverride(0) == 0);
}
