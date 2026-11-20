#pragma once

#include "PeerTransport.hpp"

#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace rtc { class WebSocket; }

namespace Netplay
{
class NativeUdpTransport;
std::unique_ptr<rtc::WebSocket> OpenRoomWebSocket(const std::string &url);

struct RouteSwitchStatus {
    const char *from = nullptr;
    const char *to = nullptr;
    int fromMs = -1;
    int toMs = -1;
    bool switching = false;
};

class NativeRoomTransport final : public PeerTransport
{
public:
    NativeRoomTransport();
    ~NativeRoomTransport() override;

    // signalUrl is the room's player URL with signal=1. The worker owns initial
    // signaling and teardown. Pump adopts the ready route on the game thread.
    void Connect(const std::string &signalUrl, std::string tag, std::uint8_t localPlayer,
                 std::uint8_t playerCount, bool preferTurn = false);
    void Connect(const std::string &signalUrl, std::string tag, std::uint8_t localPlayer,
                 std::uint8_t playerCount, bool preferTurn, std::string_view initialRoute);
    void ConnectUdp(bool host, std::string_view endpoint, std::string identity, std::uint32_t run);
    std::uint32_t UdpRun() const;
    void Close();
    void Pump();
    void SetAdaptiveRouteActive(bool active);

    enum class Route { Connecting, Reconnecting, Direct, Nat4, Swarm, Turn, Udp };
    struct Link {
        Route route = Route::Connecting;
        std::optional<unsigned> rttMs;
        std::uint64_t revision = 0;
        bool probesUnanswered = false;
    };
    Link LinkStatus() const;
    RouteSwitchStatus SwitchStatus() const;
    bool ConfirmRouteSwitch();
    bool ReportMatch(std::uint32_t start, std::uint32_t frames, unsigned winner, unsigned first, unsigned second);
    bool MatchReplayEnabled() const;
    void PublishMatchReplay(std::uint32_t start, std::vector<std::uint8_t> file, std::string origin);
    const std::string &LastError() const;

    bool IsOpen() const override;
    bool Failed() const override;
    bool Recovering() const override;
    bool Disconnected() const override;
    bool SendTo(std::uint8_t player, const std::uint8_t *data, std::size_t size) override;
    bool SendRepairTo(std::uint8_t player, const std::uint8_t *data, std::size_t size) override;
    bool SendControl(const std::uint8_t *data, std::size_t size) override;
    bool Poll(std::vector<std::uint8_t> *packet) override;
    std::size_t BufferedAmount() const override;

private:
    struct Impl;
    struct ConnectionWorker;
    std::unique_ptr<Impl> impl_;
    std::unique_ptr<ConnectionWorker> worker_;
    std::unique_ptr<NativeUdpTransport> udp_;
    bool opening_ = false;
};
}
