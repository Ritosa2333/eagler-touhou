#include <eagler/netplay/BrowserPeerTransport.hpp>

#include <cassert>
#include <cstring>
#include <vector>

int main()
{
    Netplay::BrowserPeerTransport transport;
    std::vector<std::uint8_t> packet;

    assert(!transport.IsOpen());
    assert(transport.Failed());
    assert(transport.BufferedAmount() == 0);
    assert(std::strcmp(transport.Mode(), "unsupported") == 0);
    assert(!transport.Poll(&packet));
    assert(!transport.Send(nullptr, 0));
    assert(!transport.SendTo(1, nullptr, 0));
    assert(!transport.SendRepairTo(1, nullptr, 0));
    assert(!transport.SendControl(nullptr, 0));
    assert(!transport.SendSpectator(nullptr, 0));
    assert(!transport.HasSpectators());
    assert(!transport.Connect("wss://example.invalid/relay", 0, 2));
    assert(transport.LastError() == "browser peer transport is Web-only");
    assert(!transport.ConnectSpectator("wss://example.invalid/relay", "spectator", 2));
}
