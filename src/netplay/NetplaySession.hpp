#pragma once

#include "NetplayProtocol.hpp"

#include <array>
#include <cstdint>

namespace Netplay
{
struct SessionConfig
{
    std::uint64_t sessionId = 0;
    std::uint32_t seed = 0;
    std::uint32_t gameplayAbi = 0;
    std::uint32_t gameId = 0;
    std::uint8_t playerCount = 2;
    std::uint8_t localPlayer = 0;
};

enum class SessionPacketResult
{
    Accepted,
    Duplicate,
    InvalidPeer,
    ContractMismatch,
    ReadyBeforeHello,
};

class SessionGate
{
public:
    bool Reset(const SessionConfig &config);
    void Clear();

    SessionPacket BuildPacket(SessionPhase phase) const;
    SessionPacketResult Apply(const SessionPacket &packet);

    bool PeerHello(std::uint8_t player) const;
    bool PeerReady(std::uint8_t player) const;
    bool AllPeersHello() const;
    bool AllPeersReady() const;
    bool CanSendReady() const { return AllPeersHello(); }
    bool CanStart() const { return localReady_ && AllPeersReady(); }
    void MarkLocalReady() { if (CanSendReady()) localReady_ = true; }
    bool LocalReady() const { return localReady_; }
    const SessionConfig &Config() const { return config_; }

private:
    bool MatchesContract(const SessionPacket &packet) const;

    SessionConfig config_{};
    bool configured_ = false;
    bool localReady_ = false;
    std::array<bool, MAX_PLAYERS> peerHello_{};
    std::array<bool, MAX_PLAYERS> peerReady_{};
};
} // namespace Netplay
