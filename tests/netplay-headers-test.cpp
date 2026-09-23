#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/NetplayProtocol.hpp>
#include <eagler/netplay/NetplaySession.hpp>
#include <eagler/netplay/WebSocketTransport.hpp>
#include <eagler/netplay/SessionChannel.hpp>
#include <eagler/netplay/BrowserPeerTransport.hpp>

#include <cassert>
#include <cstdio>
#include <type_traits>

int main()
{
    Netplay::FrameInput input;
    input.buttons = 3;
    Netplay::CoreConfig core;
    core.playerCount = 2;
    Netplay::SessionConfig session;
    session.playerCount = 2;
    assert(Netplay::PROTOCOL_VERSION == 4);
    assert(input.buttons == 3);
    assert(core.playerCount == 2);
    assert(session.playerCount == 2);
    static_assert(sizeof(Netplay::WebSocketTransport) > 0);
    static_assert(std::is_base_of_v<Netplay::PeerTransport, Netplay::BrowserPeerTransport>);
    static_assert(!std::is_copy_constructible_v<Netplay::SessionChannel>);
    std::puts("eagler-common netplay headers: PASS");
    return 0;
}
