#pragma once

#include <eagler/netplay/NativePeerTransport.hpp>
#include <nlohmann/json_fwd.hpp>
#include <functional>

namespace Netplay
{
struct RouteSwitchStatus;
// Implements the browser transport's etr1 control protocol for a native peer.
// The primary socket stays alive while the standby is negotiated and measured.
class NativeAdaptiveRoute
{
public:
    NativeAdaptiveRoute(NativePeerTransport &primary, std::string tag, std::uint8_t localPlayer,
                        rtc::Configuration standbyConfig, bool swarm, bool allowSwitching = true, bool manualConfirmation = false);
    ~NativeAdaptiveRoute();

    void Pump(bool running);
    bool Recover();
    bool Recovering() const;
    bool Failed() const;
    NativePeerTransport &Active() const;
    bool HasLatencyProbes() const;
    bool ProbesUnanswered() const;
    bool Stalled() const;
    std::optional<unsigned> ActiveRtt() const;
    bool ActiveUsesSwarm() const;
    std::uint32_t ActivePathEpoch() const;
    using SurveySignal = std::function<bool(const nlohmann::json &)>;
    using RetirePeer = std::function<void(std::unique_ptr<NativePeerTransport>)>;
    void ConfigureSurvey(std::string initialRoute, bool initialSurvey, SurveySignal send, RetirePeer retire);
    void BeginStartup();
    bool SelectStartup(std::uint32_t serial, bool keepPrimary = false);
    RouteSwitchStatus SwitchStatus() const;
    bool ConfirmOffer();
    void RelayStandby(const nlohmann::json &message, rtc::Configuration ice = {});
    bool SendControl(const std::uint8_t *data, std::size_t size);
    bool Poll(std::vector<std::uint8_t> *packet);

private:
    struct State;
    std::unique_ptr<State> state_;
};
}
