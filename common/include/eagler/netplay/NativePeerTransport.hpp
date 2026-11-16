#pragma once

#include "PeerTransport.hpp"

#include <rtc/peerconnection.hpp>

#include <memory>
#include <optional>
#include <string>

namespace Netplay
{
// Each game supplies its channel tag and uses its room signaling to exchange
// descriptions and candidates. The transport owns ICE and both data lanes.
class NativePeerTransport final : public PeerTransport
{
public:
    NativePeerTransport(std::string tag, std::uint8_t localPlayer, std::uint8_t playerCount,
                        rtc::Configuration config = {});
    ~NativePeerTransport() override;

    void AddPeer(std::uint8_t player);
    void SetRemoteDescription(std::uint8_t player, rtc::Description description);
    void AddRemoteCandidate(std::uint8_t player, rtc::Candidate candidate);
    bool PollSignal(std::uint8_t &player, rtc::variant<rtc::Description, rtc::Candidate> &signal);
    enum class Route { Connecting, Direct, Nat4, Relay };
    struct Link {
        Route route = Route::Connecting;
        std::optional<unsigned> rttMs;
    };
    Link LinkStatus(bool readRtt = true) const;
    std::string ConnectionSummary() const;
    bool UsesRelay(bool localOnly = false) const;
    bool SendText(std::uint8_t player, bool control, std::string message);
    std::optional<rtc::message_variant> PollMessage(bool &control);

    bool IsOpen() const override;
    bool Failed() const override;
    bool Disconnected() const override;
    bool SendTo(std::uint8_t player, const std::uint8_t *data, std::size_t size) override;
    bool SendRepairTo(std::uint8_t player, const std::uint8_t *data, std::size_t size) override;
    bool SendControl(const std::uint8_t *data, std::size_t size) override;
    bool Poll(std::vector<std::uint8_t> *packet) override;
    std::size_t BufferedAmount() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
}
