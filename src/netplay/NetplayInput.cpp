#include <eagler/netplay/NetplayInput.hpp>

#include <algorithm>
#include <array>

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include <netplay/NetplayInputConfig.hpp>
#endif

namespace Netplay::Input
{
namespace
{
bool g_CaptureActive = false;
FrameInput g_Captured{};
bool g_ReplayOverrideActive = false;
FrameInput g_ReplayOverride{};
bool g_PlayerButtonOverridesActive = false;
std::array<std::uint16_t, MAX_PLAYERS> g_PlayerButtonOverrides{};
std::array<FrameInput, MAX_PLAYERS> g_PlayerInputOverrides{};
DirectTouchStates g_DirectTouchStates{};
}

DirectTouchStates &GetDirectTouchStates() { return g_DirectTouchStates; }
void ResetDirectTouchStates() { g_DirectTouchStates = {}; }
void ResetPlayerDirectTouch(std::size_t player)
{
    if (player < g_DirectTouchStates.size())
        g_DirectTouchStates[player] = {};
}
bool UsesIncrementalDirectTouch(std::size_t player)
{
    return g_PlayerButtonOverridesActive && player < g_DirectTouchStates.size() &&
           g_DirectTouchStates[player].active;
}
void SetDirectTouchRemainder(std::size_t player, float x, float y)
{
    if (UsesIncrementalDirectTouch(player))
        g_DirectTouchStates[player].Set(x, y);
}
void ConsumeDirectTouchRemainder(std::size_t player, float x, float y)
{
    if (UsesIncrementalDirectTouch(player))
        g_DirectTouchStates[player].Consume(x, y);
}

void BeginCapture()
{
    g_Captured = {};
    g_CaptureActive = true;
}

FrameInput EndCapture()
{
    g_CaptureActive = false;
    return g_Captured;
}

bool CaptureActive()
{
    return g_CaptureActive;
}

std::uint16_t ResolveLocal(std::uint16_t physicalBits)
{
    if (g_ReplayOverrideActive)
        return g_ReplayOverride.buttons;
    if (g_CaptureActive)
        g_Captured.buttons = physicalBits;
    return physicalBits;
}

void CaptureJoystick(float x, float y)
{
    if (!g_CaptureActive || g_ReplayOverrideActive)
        return;
    g_Captured.analogMode = AnalogMode::Joystick;
    g_Captured.x = x;
    g_Captured.y = y;
    g_Captured.unlimited = false;
}

void CaptureDirectTouch(float x, float y, bool unlimited)
{
    if (!g_CaptureActive || g_ReplayOverrideActive)
        return;
    g_Captured.analogMode = AnalogMode::DirectTouch;
    g_Captured.x = x;
    g_Captured.y = y;
    g_Captured.unlimited = unlimited;
}

void CaptureDirectTouchDelta(float x, float y, bool unlimited, bool begin)
{
    CaptureDirectTouch(x, y, unlimited);
    if (g_CaptureActive && !g_ReplayOverrideActive)
        g_Captured.analogMode = begin ? AnalogMode::DirectTouchBegin : AnalogMode::DirectTouchDelta;
}

void SetReplayOverride(const FrameInput &input)
{
    g_ReplayOverride = input;
    g_ReplayOverrideActive = true;
}

void SetReplayOverride(std::uint16_t bits)
{
    FrameInput input;
    input.buttons = bits;
    SetReplayOverride(input);
}

void ClearReplayOverride()
{
    g_ReplayOverrideActive = false;
    g_ReplayOverride = {};
}

bool ReplayOverrideActive()
{
    return g_ReplayOverrideActive;
}

bool ReplayJoystick(std::size_t player, float *x, float *y)
{
    if (!g_PlayerButtonOverridesActive || player >= g_PlayerInputOverrides.size() ||
        g_PlayerInputOverrides[player].analogMode != AnalogMode::Joystick)
        return false;
    const FrameInput &input = g_PlayerInputOverrides[player];
    if (x)
        *x = input.x;
    if (y)
        *y = input.y;
    return true;
}

bool ReplayDirectTouch(std::size_t player, float *x, float *y, bool *unlimited)
{
    if (!g_PlayerButtonOverridesActive || player >= g_PlayerInputOverrides.size() ||
        (g_PlayerInputOverrides[player].analogMode != AnalogMode::DirectTouch &&
         !UsesIncrementalDirectTouch(player)))
        return false;
    const FrameInput &input = g_PlayerInputOverrides[player];
    if (x)
        *x = UsesIncrementalDirectTouch(player) ? g_DirectTouchStates[player].x : input.x;
    if (y)
        *y = UsesIncrementalDirectTouch(player) ? g_DirectTouchStates[player].y : input.y;
    if (unlimited)
        *unlimited = input.unlimited;
    return true;
}

bool PlayerTouchUsed(std::size_t player)
{
    return g_PlayerButtonOverridesActive && player < g_PlayerInputOverrides.size() &&
           g_PlayerInputOverrides[player].touchUsed;
}

bool PlayerTouchBomb(std::size_t player)
{
    return g_PlayerButtonOverridesActive && player < g_PlayerInputOverrides.size() &&
           g_PlayerInputOverrides[player].touchBomb;
}

void SetPlayerButtonOverrides(const std::uint16_t *buttons, std::size_t count)
{
    std::array<FrameInput, MAX_PLAYERS> converted{};
    if (buttons)
    {
        const std::size_t copyCount = std::min(count, converted.size());
        for (std::size_t i = 0; i < copyCount; ++i)
            converted[i].buttons = buttons[i];
    }
    SetPlayerInputOverrides(converted.data(), count);
}

void SetPlayerInputOverrides(const FrameInput *inputs, std::size_t count)
{
    g_PlayerButtonOverrides.fill(0);
    g_PlayerInputOverrides.fill({});
    if (inputs)
    {
        const std::size_t copyCount = std::min(count, g_PlayerButtonOverrides.size());
        for (std::size_t i = 0; i < copyCount; ++i)
        {
            g_PlayerInputOverrides[i] = inputs[i];
            g_PlayerButtonOverrides[i] = inputs[i].buttons;
        }
    }
    g_PlayerButtonOverridesActive = true;
    for (std::size_t player = 0; player < g_DirectTouchStates.size(); ++player)
        g_DirectTouchStates[player].Apply(g_PlayerInputOverrides[player]);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    InputConfig::CommitGameInputs(g_PlayerButtonOverrides);
#endif
}

void ClearPlayerButtonOverrides()
{
    g_PlayerButtonOverridesActive = false;
    g_PlayerButtonOverrides.fill(0);
    g_PlayerInputOverrides.fill({});
}

bool PlayerButtonOverridesActive()
{
    return g_PlayerButtonOverridesActive;
}

std::uint16_t PlayerButtonOverride(std::size_t player)
{
    return player < g_PlayerButtonOverrides.size() ? g_PlayerButtonOverrides[player] : 0;
}

bool PlayerInputOverride(std::size_t player, FrameInput *out)
{
    if (!out || !g_PlayerButtonOverridesActive || player >= g_PlayerInputOverrides.size())
        return false;
    *out = g_PlayerInputOverrides[player];
    return true;
}
} // namespace Netplay::Input
