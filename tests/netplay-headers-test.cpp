#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/NetplayProtocol.hpp>
#include <eagler/netplay/NetplaySession.hpp>
#include <eagler/netplay/WebSocketTransport.hpp>

#include <cassert>
#include <cstdio>

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
    std::puts("eagler-common netplay headers: PASS");
    return 0;
}
