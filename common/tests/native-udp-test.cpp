#include <eagler/netplay/NativeUdpTransport.hpp>

#include <cassert>
#include <chrono>
#include <stdexcept>
#include <thread>

using Netplay::NativeUdpTransport;
using namespace std::chrono_literals;

template<class Done>
static bool pumpUntil(NativeUdpTransport &host, NativeUdpTransport &guest, Done done)
{
    const auto deadline = std::chrono::steady_clock::now() + 4s;
    while (std::chrono::steady_clock::now() < deadline) {
        host.Pump();
        guest.Pump();
        if (done()) return true;
        std::this_thread::sleep_for(1ms);
    }
    return false;
}

int main()
{
    const std::uint32_t run = 8675309;
    const std::string build = "test:1";
    std::unique_ptr<NativeUdpTransport> host;
    unsigned port = 0;
    // Avoid racing another local process between reserving and listening.
    for (port = 39100; port < 39130; ++port) {
        try {
            host = std::make_unique<NativeUdpTransport>(true, std::to_string(port), build, run);
            break;
        } catch (const std::runtime_error &) {}
    }
    assert(host);
    const auto endpoint = "127.0.0.1:" + std::to_string(port);

    {
        NativeUdpTransport oldGuest(false, endpoint, "test:0", 0);
        assert(pumpUntil(*host, oldGuest, [&] { return oldGuest.Failed(); }));
        assert(!host->IsOpen() && !host->Failed());
    }
    {
        NativeUdpTransport guest(false, endpoint, build, 0);
        assert(pumpUntil(*host, guest, [&] { return host->IsOpen() && guest.IsOpen(); }));
        assert(guest.Run() == run && host->RoundTripMs().has_value());
        const std::vector<std::uint8_t> control{7, 11}, input{13, 17};
        assert(host->SendControl(control.data(), control.size()));
        assert(!guest.SendTo(1, input.data(), input.size()));
        assert(guest.SendTo(0, input.data(), input.size()));
        std::vector<std::uint8_t> hostPacket, guestPacket;
        assert(pumpUntil(*host, guest, [&] {
            if (hostPacket.empty()) host->Poll(&hostPacket);
            if (guestPacket.empty()) guest.Poll(&guestPacket);
            return !hostPacket.empty() && !guestPacket.empty() && !host->BufferedAmount();
        }));
        assert(hostPacket == input && guestPacket == control);
        // The guest closes before the host. A new listener can then reuse the port.
    }
    const auto deadline = std::chrono::steady_clock::now() + 4s;
    while (!host->Disconnected() && std::chrono::steady_clock::now() < deadline) {
        host->Pump();
        std::this_thread::sleep_for(1ms);
    }
    assert(host->Disconnected());
    host.reset();
    NativeUdpTransport reopened(true, std::to_string(port), build, run);
    assert(!reopened.IsOpen() && !reopened.Failed());
}
