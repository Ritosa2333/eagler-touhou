#pragma once

#include <eagler/netplay/PeerTransport.hpp>

#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

struct _ENetHost;
struct _ENetPeer;

namespace Netplay
{
// A two-player ENet link. Both callers supply the same title and build identity.
// Only the host binds a user-selected port. Pump and all sends run on the owner thread.
class NativeUdpTransport final : public PeerTransport
{
public:
    NativeUdpTransport(bool host, std::string_view endpoint, std::string identity, std::uint32_t run);
    ~NativeUdpTransport() override;

    void Pump();
    std::uint32_t Run() const { return run_; }
    std::optional<unsigned> RoundTripMs() const;
    const std::string &LastError() const { return error_; }

    bool IsOpen() const override { return ready_ && peer_ && !Failed(); }
    bool Failed() const override { return !error_.empty(); }
    bool Disconnected() const override { return ready_ && !peer_; }
    bool SendTo(std::uint8_t peer, const std::uint8_t *data, std::size_t size) override;
    bool SendRepairTo(std::uint8_t peer, const std::uint8_t *data, std::size_t size) override;
    bool SendControl(const std::uint8_t *data, std::size_t size) override;
    bool Poll(std::vector<std::uint8_t> *packet) override;
    std::size_t BufferedAmount() const override { return outgoingBytes_; }

private:
    bool Send(std::uint8_t channel, const std::uint8_t *data, std::size_t size);
    void RejectPeer();

    std::size_t outgoingBytes_ = 0;
    std::unique_ptr<_ENetHost, void (*)(_ENetHost *)> socket_;
    _ENetPeer *peer_ = nullptr;
    std::deque<std::vector<std::uint8_t>> incoming_;
    std::string identity_, error_;
    std::uint32_t run_, helloSentAt_ = 0;
    bool host_, ready_ = false;
};
}
