#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace Netplay
{
// The protocol pump does not own browser connections or a title's simulation.
// Tests can substitute a loss/reorder link without replacing the game or wire
// codec. The owner opens/closes the transport outside deterministic rollback.
class PeerTransport
{
public:
    virtual ~PeerTransport() = default;
    virtual bool IsOpen() const = 0;
    virtual bool Failed() const = 0;
    virtual bool SendTo(std::uint8_t peer, const std::uint8_t *data, std::size_t size) = 0;
    virtual bool SendRepairTo(std::uint8_t peer, const std::uint8_t *data, std::size_t size) = 0;
    virtual bool SendControl(const std::uint8_t *data, std::size_t size) = 0;
    virtual bool Poll(std::vector<std::uint8_t> *packet) = 0;
    virtual std::size_t BufferedAmount() const = 0;
};
} // namespace Netplay
