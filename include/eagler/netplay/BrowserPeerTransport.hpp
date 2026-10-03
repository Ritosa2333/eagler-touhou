#pragma once

#include <eagler/netplay/PeerTransport.hpp>

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
class BrowserPeerTransport : public PeerTransport
{
public:
    BrowserPeerTransport() = default;
    ~BrowserPeerTransport() override;

    BrowserPeerTransport(const BrowserPeerTransport &) = delete;
    BrowserPeerTransport &operator=(const BrowserPeerTransport &) = delete;

    bool Connect(const char *relayUrl, std::uint8_t localPlayer, std::uint8_t playerCount);
    bool ConnectSpectator(const char *relayUrl, const char *spectatorId,
                          std::uint8_t playerCount);
    void Close();
    bool IsOpen() const override;
    bool Failed() const override;
    // Per-frame input/ACK traffic: unordered and non-retransmitting on RTC.
    bool Send(const std::uint8_t *data, std::size_t size);
    // Peer-relative input packets carry that peer's ACK and frame-advantage
    // estimate. RTC sends directly to one DataChannel; relay mode uses the
    // relay's small transport envelope and delivers the unchanged payload.
    bool SendTo(std::uint8_t peer, const std::uint8_t *data, std::size_t size) override;
    // Rare reliable duplicate of an already-captured input packet. This is
    // intentionally RTC-control-only: it repairs a stalled fast input lane
    // without converting the normal input path into a reliable stream. Repair
    // failure never takes ownership of overall transport health.
    bool SendRepairTo(std::uint8_t peer, const std::uint8_t *data, std::size_t size) override;
    // Session/control traffic: reliable and ordered on RTC.
    bool SendControl(const std::uint8_t *data, std::size_t size) override;
    // Best-effort, relay-only confirmed input stream. Failure is deliberately
    // isolated from gameplay transport health.
    bool SendSpectator(const std::uint8_t *data, std::size_t size);
    bool HasSpectators() const;
    // Optional output only: -1 unavailable/stopped, 0 connecting, 1 open.
    // SendSpectator separately enforces actual socket backpressure.
    int SpectatorState() const;
    // Terminal for this run; notify viewers without closing player transports.
    void StopSpectators();
    bool Poll(std::vector<std::uint8_t> *packet) override;
    std::size_t BufferedAmount() const override;
    // Read-only RTC lane diagnostics; relay bytes remain in BufferedAmount.
    std::size_t BufferedInputAmount() const;
    std::size_t BufferedControlAmount() const;
    const std::string &LastError() const;
    const char *Mode() const;

private:
    mutable std::string lastError_;
};
} // namespace Netplay
