#include <eagler/netplay/BrowserPeerTransport.hpp>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

namespace {
Netplay::BrowserPeerTransport transport;
char relay_url[1024]{};
char spectator_id[128]{};
std::uint8_t packet_buffer[128]{};
char error_buffer[256]{};
}

extern "C" {
#define TH09_PEER_EXPORT(name) __attribute__((export_name(name)))
TH09_PEER_EXPORT("th09_peer_url_buffer") char* th09_peer_url_buffer() { return relay_url; }
TH09_PEER_EXPORT("th09_peer_spectator_id_buffer") char* th09_peer_spectator_id_buffer() { return spectator_id; }
TH09_PEER_EXPORT("th09_peer_packet_buffer") std::uint8_t* th09_peer_packet_buffer() { return packet_buffer; }
TH09_PEER_EXPORT("th09_peer_error") const char* th09_peer_error() {
    const auto& error = transport.LastError();
    const auto count = std::min(error.size(), sizeof(error_buffer) - 1);
    std::memcpy(error_buffer, error.data(), count);
    error_buffer[count] = '\0';
    return error_buffer;
}
TH09_PEER_EXPORT("th09_peer_connect") int th09_peer_connect(int side) {
    if (side < 0 || side > 1 || !std::memchr(relay_url, 0, sizeof(relay_url))) return 0;
    return transport.Connect(relay_url, static_cast<std::uint8_t>(side), 2);
}
TH09_PEER_EXPORT("th09_peer_connect_spectator") int th09_peer_connect_spectator() {
    if (!std::memchr(relay_url, 0, sizeof(relay_url)) ||
        !spectator_id[0] || !std::memchr(spectator_id, 0, sizeof(spectator_id))) return 0;
    return transport.ConnectSpectator(relay_url, spectator_id, 2);
}
TH09_PEER_EXPORT("th09_peer_state") int th09_peer_state() {
    return transport.Failed() ? -1 : transport.IsOpen() ? 1 : 0;
}
TH09_PEER_EXPORT("th09_peer_send") int th09_peer_send(int length) {
    if (length < 1 || length > static_cast<int>(sizeof(packet_buffer))) return 0;
    return transport.SendControl(packet_buffer, static_cast<std::size_t>(length));
}
TH09_PEER_EXPORT("th09_peer_has_spectators") int th09_peer_has_spectators() {
    return transport.HasSpectators();
}
TH09_PEER_EXPORT("th09_peer_send_spectator") int th09_peer_send_spectator(int length) {
    if (length < 1 || length > static_cast<int>(sizeof(packet_buffer))) return 0;
    return transport.SendSpectator(packet_buffer, static_cast<std::size_t>(length));
}
TH09_PEER_EXPORT("th09_peer_poll") int th09_peer_poll() {
    std::vector<std::uint8_t> packet;
    if (!transport.Poll(&packet)) return 0;
    if (packet.size() > sizeof(packet_buffer)) return -1;
    std::memcpy(packet_buffer, packet.data(), packet.size());
    return static_cast<int>(packet.size());
}
TH09_PEER_EXPORT("th09_peer_close") void th09_peer_close() { transport.Close(); }
}
