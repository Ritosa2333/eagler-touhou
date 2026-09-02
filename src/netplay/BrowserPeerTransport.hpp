#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace Netplay
{
// Browser-only transport which races a WebRTC DataChannel mesh against the
// existing WebSocket relay. ICE may resolve the RTC path directly or through
// TURN; the game transport does not care which candidate pair won. A small
// signaling barrier selects exactly one route before frame zero, so
// rollback/session code never sees a mixed RTC/WebSocket path.
class BrowserPeerTransport
{
public:
    BrowserPeerTransport() = default;
    ~BrowserPeerTransport();

    BrowserPeerTransport(const BrowserPeerTransport &) = delete;
    BrowserPeerTransport &operator=(const BrowserPeerTransport &) = delete;

    bool Connect(const char *relayUrl, std::uint8_t localPlayer, std::uint8_t playerCount);
    bool ConnectSpectator(const char *relayUrl, const char *spectatorId,
                          std::uint8_t playerCount);
    void Close();
    bool IsOpen() const;
    bool Failed() const;
    // Per-frame input/ACK traffic: unordered and non-retransmitting on RTC.
    bool Send(const std::uint8_t *data, std::size_t size);
    // Peer-relative input packets carry that peer's ACK and frame-advantage
    // estimate. RTC sends directly to one DataChannel; relay mode uses the
    // relay's small transport envelope and delivers the unchanged payload.
    bool SendTo(std::uint8_t peer, const std::uint8_t *data, std::size_t size);
    // Session/control traffic: reliable and ordered on RTC.
    bool SendControl(const std::uint8_t *data, std::size_t size);
    // Best-effort, relay-only confirmed input stream. Failure is deliberately
    // isolated from gameplay transport health.
    bool SendSpectator(const std::uint8_t *data, std::size_t size);
    bool HasSpectators() const;
    bool Poll(std::vector<std::uint8_t> *packet);
    std::size_t BufferedAmount() const;
    const std::string &LastError() const;
    const char *Mode() const;

private:
    mutable std::string lastError_;
};
} // namespace Netplay
