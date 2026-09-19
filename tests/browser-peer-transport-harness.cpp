#include <eagler/netplay/BrowserPeerTransport.hpp>

#include <array>
#include <cstdint>
#include <vector>

#include <emscripten/emscripten.h>

EM_JS(void, peer_harness_copy_relay_url, (char *out, int capacity), {
    const params = new URLSearchParams(location.search);
    const room = params.get('room') || 'peer-harness';
    const run = params.get('run') || '1';
    const player = params.get('player') || '0';
    const players = params.get('players') || '2';
    const url = new URL('ws://127.0.0.1:18142/');
    url.searchParams.set('room', room);
    url.searchParams.set('run', run);
    url.searchParams.set('player', player);
    url.searchParams.set('players', players);
    const bytes = new TextEncoder().encode(url.href);
    const count = Math.min(bytes.byteLength, capacity - 1);
    HEAPU8.set(bytes.subarray(0, count), out);
    HEAPU8[out + count] = 0;
});

namespace
{
Netplay::BrowserPeerTransport g_Transport;
bool g_Sent = false;
bool g_ControlSent = false;
bool g_InputReceived = false;
bool g_ControlReceived = false;
std::uint8_t g_LocalPlayer = 0;
std::uint8_t g_PlayerCount = 2;

void Tick()
{
    if (g_Transport.Failed())
    {
        EM_ASM({
            globalThis.__peerHarnessFailed = true;
            globalThis.__peerHarnessError = UTF8ToString($0);
        }, g_Transport.LastError().c_str());
        return;
    }

    if (!g_Transport.IsOpen())
        return;

    EM_ASM({
        globalThis.__peerHarnessOpen = true;
        globalThis.__peerHarnessMode = globalThis.__eaglerNetplayTransport || 'unknown';
    });

    if (!g_Sent)
    {
        const std::array<std::uint8_t, 4> packet = {0x45, 0x37, 0x50, 0x32};
        const std::uint8_t peer = static_cast<std::uint8_t>((g_LocalPlayer + 1) % g_PlayerCount);
        if (g_Transport.SendTo(peer, packet.data(), packet.size()))
            g_Sent = true;
    }
    if (!g_ControlSent)
    {
        const std::array<std::uint8_t, 4> packet = {0x43, 0x54, 0x52, 0x4c};
        if (g_Transport.SendControl(packet.data(), packet.size()))
            g_ControlSent = true;
    }

    std::vector<std::uint8_t> packet;
    while (g_Transport.Poll(&packet))
    {
        if (packet.size() == 4 && packet[0] == 0x45 && packet[1] == 0x37 &&
            packet[2] == 0x50 && packet[3] == 0x32)
            g_InputReceived = true;
        if (packet.size() == 4 && packet[0] == 0x43 && packet[1] == 0x54 &&
            packet[2] == 0x52 && packet[3] == 0x4c)
            g_ControlReceived = true;
    }

    if (g_Sent && g_ControlSent && g_InputReceived && g_ControlReceived)
        EM_ASM({ globalThis.__peerHarnessPass = true; });
}
} // namespace

int main()
{
    const int player = EM_ASM_INT({
        return Number.parseInt(new URLSearchParams(location.search).get('player') || '0', 10) || 0;
    });
    const int players = EM_ASM_INT({
        return Number.parseInt(new URLSearchParams(location.search).get('players') || '2', 10) || 2;
    });
    g_LocalPlayer = static_cast<std::uint8_t>(player);
    g_PlayerCount = static_cast<std::uint8_t>(players);
    char relayUrl[512] = {};
    EM_ASM({
        Module.eaglerOptions = {
            netplayIceServers: [{ urls: ['stun:stun.cloudflare.com:3478'] }]
        };
    });
    peer_harness_copy_relay_url(relayUrl, sizeof(relayUrl));

    EM_ASM({
        globalThis.__peerHarnessPass = false;
        globalThis.__peerHarnessOpen = false;
        globalThis.__peerHarnessFailed = false;
        globalThis.__peerHarnessError = String();
    });

    if (!g_Transport.Connect(relayUrl, static_cast<std::uint8_t>(player), static_cast<std::uint8_t>(players)))
    {
        EM_ASM({ globalThis.__peerHarnessFailed = true; globalThis.__peerHarnessError = 'connect returned false'; });
        return 1;
    }

    emscripten_set_main_loop(Tick, 0, false);
    return 0;
}
