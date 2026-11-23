#include <eagler/netplay/BrowserPeerTransport.hpp>

#include <cstdint>
#include <cstring>
#include <vector>

namespace
{
Netplay::BrowserPeerTransport peer;
std::vector<std::uint8_t> incoming;
int seat = -1;
constexpr int maxPacket = 4096;
}

// Small binary bridge to the author's shared browser transport. TH128 uses
// its ordered lane until its input ledger has a separate retransmission ACK.
extern "C"
{
int th128_mp_open(const char *url, int localPlayer)
{
    if (!url || !*url || seat != -1 || localPlayer < 0 || localPlayer > 1) return 0;
    if (!peer.Connect(url, static_cast<std::uint8_t>(localPlayer), 2, localPlayer == 0)) return 0;
    seat = localPlayer;
    return 1;
}

#ifdef TH128_COOP_PEER
// This export exists only in the projection whose RTC receiver attaches the actual channel seat.
int th128_mp_open_coop(const char *url, int localPlayer, int players)
{
    if (!url || !*url || seat != -1 || (players != 2 && players != 3) || localPlayer < 0 || localPlayer >= players) return 0;
    if (!peer.Connect(url, static_cast<std::uint8_t>(localPlayer), static_cast<std::uint8_t>(players), localPlayer == 0)) return 0;
    seat = localPlayer;
    return 1;
}
#endif

void th128_mp_close()
{
    peer.Close();
    incoming.clear();
    seat = -1;
}

int th128_mp_state()
{
    if (peer.Failed() || peer.Disconnected()) return -1;
    return seat >= 0 && peer.IsOpen() ? 1 : 0;
}

const char *th128_mp_mode() { return peer.Mode(); }
const char *th128_mp_error() { return peer.LastError().c_str(); }
double th128_mp_buffered() { return static_cast<double>(peer.BufferedAmount()); }

int th128_mp_send(const std::uint8_t *data, int size)
{
    return seat >= 0 && size > 0 && size <= maxPacket && peer.SendControl(data, static_cast<std::size_t>(size));
}

int th128_mp_poll(std::uint8_t *target, int capacity)
{
    if (seat < 0 || !target || capacity < 1 || capacity > maxPacket + 2) return -1;
    if (!peer.Poll(&incoming)) return 0;
    if (incoming.empty() || incoming.size() > static_cast<std::size_t>(capacity)) return -1;
    std::memcpy(target, incoming.data(), incoming.size());
    return static_cast<int>(incoming.size());
}

int th128_mp_spectator_begin(std::uint32_t low, std::uint32_t high, std::uint32_t seed)
{
    if (seat != 0 || !(low || high) || seed > 65535) return 0;
    peer.BeginSpectatorSession((std::uint64_t(high) << 32) | low, seed);
    return 1;
}

int th128_mp_spectator_send(const std::uint8_t *data, int size)
{
    return seat == 0 && data && size >= 32 && size <= 4136 && peer.SendSpectator(data, static_cast<std::size_t>(size));
}
}
