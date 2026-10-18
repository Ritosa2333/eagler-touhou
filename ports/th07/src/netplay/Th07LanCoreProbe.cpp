#include "Th07LanCoreProbe.hpp"

#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/NetplayProtocol.hpp>
#include <eagler/netplay/WebSocketTransport.hpp>

#include "Controller.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace Netplay::Th07LanCoreProbe
{
namespace
{
constexpr std::uint64_t SESSION_ID = 0x5448374c414e3031ull; // TH7LAN01
constexpr std::uint32_t TEST_FRAMES = 180;
constexpr std::uint32_t MAX_TICKS = 900;

WebSocketTransport g_Transport;
RollbackCore g_Core;
bool g_Initialized = false;
bool g_Done = false;
std::uint8_t g_LocalPlayer = 0;
std::uint32_t g_LocalFrame = 0;
std::uint32_t g_Ticks = 0;
std::uint32_t g_Sequence = 1;
std::uint32_t g_LastReceivedSequence = 0;
std::uint32_t g_SentPackets = 0;
std::uint32_t g_ReceivedPackets = 0;
std::uint32_t g_PredictedFrames = 0;
std::uint32_t g_RollbackRequests = 0;

bool Requested()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({
        return Module.eaglerOptions?.debugHarness === 'netplay-lan-core' ? 1 : 0;
    }) != 0;
#else
    return false;
#endif
}

std::uint8_t ReadPlayer()
{
#ifdef __EMSCRIPTEN__
    const int value = EM_ASM_INT({ return Module.eaglerOptions?.netplayPlayer ?? -1; });
    return value >= 0 && value <= 1 ? static_cast<std::uint8_t>(value) : 0xff;
#else
    return 0xff;
#endif
}

bool ReadUrl(char *out, std::size_t capacity)
{
#ifdef __EMSCRIPTEN__
    if (!out || capacity == 0)
        return false;
    EM_ASM({
        stringToUTF8(Module.eaglerOptions?.netplayUrl || "", $0, $1);
    }, out, capacity);
    return out[0] != '\0';
#else
    (void)out;
    (void)capacity;
    return false;
#endif
}

std::uint16_t ScriptInput(std::uint8_t player, std::uint32_t frame)
{
    std::uint16_t bits = TH_BUTTON_SHOOT;
    const std::uint32_t phase = (frame / 30) & 3u;
    if (player == 0)
        bits |= phase == 0 ? TH_BUTTON_LEFT :
                phase == 1 ? TH_BUTTON_UP :
                phase == 2 ? TH_BUTTON_RIGHT : TH_BUTTON_DOWN;
    else
        bits |= phase == 0 ? TH_BUTTON_RIGHT :
                phase == 1 ? TH_BUTTON_DOWN :
                phase == 2 ? TH_BUTTON_LEFT : TH_BUTTON_UP;
    if ((frame / 45) & 1u)
        bits |= TH_BUTTON_FOCUS;
    return bits;
}

void Fail(const char *reason)
{
    if (g_Done)
        return;
    g_Done = true;
    std::printf(
        "netplay lan core: FAIL player=%u reason=%s frame=%u sent=%u recv=%u rollback=%u predicted=%u confirmed=%u buffered=%llu error=%s\n",
        static_cast<unsigned>(g_LocalPlayer), reason, g_LocalFrame,
        g_SentPackets, g_ReceivedPackets, g_RollbackRequests, g_PredictedFrames,
        static_cast<unsigned>(g_Core.ConfirmedThrough(static_cast<std::uint8_t>(1 - g_LocalPlayer))),
        static_cast<unsigned long long>(g_Transport.BufferedAmount()),
        g_Transport.LastError().c_str());
}

void Pass()
{
    g_Done = true;
    std::printf(
        "netplay lan core: PASS player=%u frames=%u sent=%u recv=%u rollback=%u predicted=%u confirmed=%u buffered=%llu\n",
        static_cast<unsigned>(g_LocalPlayer), TEST_FRAMES, g_SentPackets, g_ReceivedPackets,
        g_RollbackRequests, g_PredictedFrames,
        static_cast<unsigned>(g_Core.ConfirmedThrough(static_cast<std::uint8_t>(1 - g_LocalPlayer))),
        static_cast<unsigned long long>(g_Transport.BufferedAmount()));
}

bool Initialize()
{
    g_LocalPlayer = ReadPlayer();
    char url[512] = {};
    if (g_LocalPlayer > 1 || !ReadUrl(url, sizeof(url)))
        return false;

    CoreConfig config;
    config.sessionId = SESSION_ID;
    config.playerCount = 2;
    config.localPlayer = g_LocalPlayer;
    config.inputDelay = 0;
    config.maxRollbackFrames = 8;
    if (!g_Core.Reset(config) || !g_Transport.Connect(url))
        return false;
    std::printf("netplay lan core: CONNECT player=%u url=%s\n",
                static_cast<unsigned>(g_LocalPlayer), url);
    return true;
}

void DrainPackets()
{
    std::vector<std::uint8_t> wire;
    while (g_Transport.Poll(&wire))
    {
        ++g_ReceivedPackets;
        InputPacket packet;
        if (!DecodeInputPacket(wire.data(), wire.size(), &packet) ||
            packet.senderPlayer == g_LocalPlayer || packet.sessionId != SESSION_ID)
        {
            Fail("invalid packet");
            return;
        }
        g_LastReceivedSequence = std::max(g_LastReceivedSequence, packet.sequence);
        RemoteInputResult result = RemoteInputResult::Duplicate;
        if (!g_Core.ApplyInputPacket(packet, &result))
        {
            Fail("core rejected packet");
            return;
        }
        if (result == RemoteInputResult::RollbackRequired)
        {
            ++g_RollbackRequests;
            g_Core.ClearRollbackRequest();
        }
    }
}
} // namespace

void OnSimulationTick()
{
    if (!Requested() || g_Done)
        return;
    if (!g_Initialized)
    {
        g_Initialized = true;
        if (!Initialize())
        {
            Fail("initialize");
            return;
        }
    }

    ++g_Ticks;
    DrainPackets();
    if (g_Done)
        return;
    if (g_Transport.Failed())
    {
        Fail("transport");
        return;
    }

    if (g_Transport.IsOpen() && g_LocalFrame < TEST_FRAMES)
    {
        bool localPresent = false;
        (void)g_Core.LocalInput(g_LocalFrame, &localPresent);
        if (!localPresent &&
            !g_Core.ScheduleLocalInput(g_LocalFrame, ScriptInput(g_LocalPlayer, g_LocalFrame)))
        {
            Fail("schedule local input");
            return;
        }
        const std::uint8_t peer = static_cast<std::uint8_t>(1 - g_LocalPlayer);
        const InputPacket packet = g_Core.BuildInputPacket(
            peer, g_LocalFrame, g_Sequence++, g_LastReceivedSequence);
        std::vector<std::uint8_t> wire;
        if (!EncodeInputPacket(packet, &wire) || !g_Transport.Send(wire.data(), wire.size()))
        {
            Fail("send packet");
            return;
        }
        ++g_SentPackets;

        if (g_LocalFrame == 0 && g_Core.ConfirmedThrough(peer) == INVALID_FRAME)
            return;

        const FrameDecision decision = g_Core.PrepareFrame(g_LocalFrame);
        if (!decision.canAdvance)
            return;
        if (decision.predictedMask != 0)
            ++g_PredictedFrames;
        if (!g_Core.MarkSimulated(g_LocalFrame, decision))
        {
            Fail("mark simulated");
            return;
        }
        ++g_LocalFrame;
    }

    DrainPackets();
    const std::uint8_t peer = static_cast<std::uint8_t>(1 - g_LocalPlayer);
    if (g_LocalFrame >= TEST_FRAMES && g_Core.ConfirmedThrough(peer) >= TEST_FRAMES - 1)
    {
        Pass();
        return;
    }
    if (g_Ticks >= MAX_TICKS)
        Fail("timeout");
}
} // namespace Netplay::Th07LanCoreProbe
