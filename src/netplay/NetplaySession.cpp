#include "NetplaySession.hpp"

namespace Netplay
{
bool SessionGate::Reset(const SessionConfig &config)
{
    Clear();
    if (config.playerCount < 2 || config.playerCount > MAX_PLAYERS ||
        config.localPlayer >= config.playerCount || config.gameId == 0 ||
        config.gameplayAbi == 0 || config.sessionId == 0)
        return false;
    config_ = config;
    configured_ = true;
    return true;
}

void SessionGate::Clear()
{
    config_ = {};
    configured_ = false;
    localReady_ = false;
    peerHello_.fill(false);
    peerReady_.fill(false);
}

SessionPacket SessionGate::BuildPacket(SessionPhase phase) const
{
    SessionPacket packet;
    if (!configured_)
        return packet;
    packet.sessionId = config_.sessionId;
    packet.seed = config_.seed;
    packet.gameplayAbi = config_.gameplayAbi;
    packet.gameId = config_.gameId;
    packet.senderPlayer = config_.localPlayer;
    packet.playerCount = config_.playerCount;
    packet.phase = phase;
    return packet;
}

bool SessionGate::MatchesContract(const SessionPacket &packet) const
{
    return packet.sessionId == config_.sessionId &&
           packet.seed == config_.seed &&
           packet.gameplayAbi == config_.gameplayAbi &&
           packet.gameId == config_.gameId &&
           packet.playerCount == config_.playerCount;
}

SessionPacketResult SessionGate::Apply(const SessionPacket &packet)
{
    if (!configured_ || packet.senderPlayer >= config_.playerCount ||
        packet.senderPlayer == config_.localPlayer)
        return SessionPacketResult::InvalidPeer;
    if (!MatchesContract(packet))
        return SessionPacketResult::ContractMismatch;

    const std::uint8_t player = packet.senderPlayer;
    if (packet.phase == SessionPhase::Hello)
    {
        if (peerHello_[player])
            return SessionPacketResult::Duplicate;
        peerHello_[player] = true;
        return SessionPacketResult::Accepted;
    }
    if (!peerHello_[player])
        return SessionPacketResult::ReadyBeforeHello;
    if (peerReady_[player])
        return SessionPacketResult::Duplicate;
    peerReady_[player] = true;
    return SessionPacketResult::Accepted;
}

bool SessionGate::PeerHello(std::uint8_t player) const
{
    return configured_ && player < config_.playerCount && player != config_.localPlayer &&
           peerHello_[player];
}

bool SessionGate::PeerReady(std::uint8_t player) const
{
    return configured_ && player < config_.playerCount && player != config_.localPlayer &&
           peerReady_[player];
}

bool SessionGate::AllPeersHello() const
{
    if (!configured_)
        return false;
    for (std::uint8_t player = 0; player < config_.playerCount; ++player)
        if (player != config_.localPlayer && !peerHello_[player])
            return false;
    return true;
}

bool SessionGate::AllPeersReady() const
{
    if (!configured_)
        return false;
    for (std::uint8_t player = 0; player < config_.playerCount; ++player)
        if (player != config_.localPlayer && !peerReady_[player])
            return false;
    return true;
}
} // namespace Netplay
