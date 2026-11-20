#include <eagler/netplay/NativeUdpTransport.hpp>

#include <enet/enet.h>

#include <charconv>
#include <stdexcept>
#include <utility>

namespace Netplay
{
namespace
{
constexpr std::uint8_t inputs = 0, control = 1;
constexpr std::uint32_t protocol = 0x45414701; // EAG, protocol 1
constexpr std::uint32_t incompatible = 1;
constexpr std::uint16_t defaultPort = 10800;
constexpr std::size_t packetLimit = 8192, queueLimit = 256, outgoingLimit = 256 * 1024;

unsigned parseNumber(std::string_view text, unsigned min, unsigned max, const char *error)
{
    unsigned value = 0;
    if (text.empty()) throw std::invalid_argument(error);
    const auto [end, status] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (status != std::errc{} || end != text.data() + text.size() || value < min || value > max)
        throw std::invalid_argument(error);
    return value;
}

ENetAddress addressFor(bool host, std::string_view endpoint)
{
    const auto first = endpoint.find_first_not_of(" \t\r\n");
    if (first != std::string_view::npos)
        endpoint = endpoint.substr(first, endpoint.find_last_not_of(" \t\r\n") - first + 1);
    if (host) return {ENET_HOST_ANY, static_cast<std::uint16_t>(parseNumber(endpoint, 1, 65535, "UDP 端口无效"))};
    const auto colon = endpoint.find(':');
    const auto port = colon == std::string_view::npos ? defaultPort :
        parseNumber(endpoint.substr(colon + 1), 1, 65535, "UDP 端口无效");
    auto ip = endpoint.substr(0, colon);
    std::uint32_t value = 0;
    for (unsigned part = 0; part < 4; ++part) {
        const auto dot = ip.find('.');
        if ((part == 3) != (dot == std::string_view::npos))
            throw std::invalid_argument("请输入有效的 IPv4 地址");
        value = (value << 8) | parseNumber(ip.substr(0, dot), 0, 255, "请输入有效的 IPv4 地址");
        if (dot != std::string_view::npos) ip.remove_prefix(dot + 1);
    }
    if (!value || value >= 0xe0000000u) throw std::invalid_argument("请输入对手的 IPv4 地址");
    return {ENET_HOST_TO_NET_32(value), static_cast<std::uint16_t>(port)};
}
}

NativeUdpTransport::NativeUdpTransport(bool host, std::string_view endpoint, std::string identity, std::uint32_t run)
    : socket_(nullptr, [](ENetHost *socket) {
          enet_host_destroy(socket);
          enet_deinitialize();
      }), identity_(std::move(identity)), run_(host ? run : 0), host_(host)
{
    const auto address = addressFor(host, endpoint);
    if (enet_initialize() != 0) throw std::runtime_error("无法初始化 UDP");
    socket_.reset(enet_host_create(host ? &address : nullptr, 1, 2, 0, 0));
    if (!socket_) {
        enet_deinitialize();
        throw std::runtime_error(host ? "无法监听 UDP 端口，可能已被占用" : "无法打开 UDP 套接字");
    }
    socket_->maximumPacketSize = packetLimit;
    socket_->maximumWaitingData = queueLimit * packetLimit;
    if (!host) {
        peer_ = enet_host_connect(socket_.get(), &address, 2, protocol);
        if (!peer_) throw std::runtime_error("无法发起 UDP 连接");
        enet_host_flush(socket_.get());
    }
}

NativeUdpTransport::~NativeUdpTransport()
{
    if (peer_) enet_peer_disconnect_now(peer_, 0);
}

void NativeUdpTransport::RejectPeer()
{
    enet_peer_disconnect_now(std::exchange(peer_, nullptr), incompatible);
    if (!host_) error_ = "双方游戏版本不一致";
}

void NativeUdpTransport::Pump()
{
    if (Failed() || Disconnected()) return;
    ENetEvent event;
    for (std::size_t count = 0; count < queueLimit; ++count) {
        const auto result = enet_host_service(socket_.get(), &event, 0);
        if (result < 0) {
            error_ = "UDP 连接出错";
            return;
        }
        if (!result) break;
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT: {
            peer_ = event.peer;
            if (host_ && event.data != protocol) {
                RejectPeer();
                break;
            }
            enet_peer_ping_interval(peer_, 500);
            enet_peer_timeout(peer_, 8, 4000, 8000);
            helloSentAt_ = enet_time_get();
            std::vector<std::uint8_t> hello(identity_.begin(), identity_.end());
            for (unsigned shift = 0; shift < 32; shift += 8)
                hello.push_back(static_cast<std::uint8_t>(run_ >> shift));
            if (!Send(control, hello.data(), hello.size())) error_ = "未能发送 UDP 握手";
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
            peer_ = nullptr;
            if (!host_ && !ready_) error_ = event.data == incompatible ? "双方游戏版本不一致" : "房主已结束等待";
            break;
        case ENET_EVENT_TYPE_RECEIVE: {
            std::unique_ptr<ENetPacket, void (*)(ENetPacket *)> packet(event.packet, enet_packet_destroy);
            const auto *data = packet->data;
            const auto size = packet->dataLength;
            if (ready_) {
                if (size && incoming_.size() < queueLimit) incoming_.emplace_back(data, data + size);
                else if (event.channelID == control) error_ = "UDP 控制消息无效";
            } else if (event.channelID == control) {
                if (size != identity_.size() + 4 ||
                    std::string_view(reinterpret_cast<const char *>(data), identity_.size()) != identity_) {
                    RejectPeer();
                    break;
                }
                std::uint32_t remoteRun = 0;
                for (unsigned byte = 0; byte < 4; ++byte)
                    remoteRun |= std::uint32_t(data[identity_.size() + byte]) << (8 * byte);
                if (host_ ? remoteRun != 0 : remoteRun == 0) {
                    RejectPeer();
                    break;
                }
                if (!host_) run_ = remoteRun;
                ready_ = true;
            }
            break;
        }
        default:
            break;
        }
        if (Failed() || Disconnected()) break;
    }
    if (peer_ && !ready_ && peer_->state == ENET_PEER_STATE_CONNECTED &&
        enet_time_get() - helloSentAt_ >= 5000) {
        enet_peer_disconnect_now(std::exchange(peer_, nullptr), 0);
        if (!host_) error_ = "UDP 握手超时";
    }
}

std::optional<unsigned> NativeUdpTransport::RoundTripMs() const
{
    if (!IsOpen()) return {};
    return peer_->roundTripTime;
}

bool NativeUdpTransport::Send(std::uint8_t channel, const std::uint8_t *data, std::size_t size)
{
    if (!peer_ || !size || size > packetLimit || outgoingBytes_ + size > outgoingLimit) return false;
    const auto flags = channel == control ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT;
    auto *packet = enet_packet_create(data, size, flags);
    if (!packet) return false;
    if (enet_peer_send(peer_, channel, packet) != 0) {
        enet_packet_destroy(packet);
        return false;
    }
    outgoingBytes_ += size;
    packet->userData = &outgoingBytes_;
    packet->freeCallback = [](ENetPacket *freed) {
        *static_cast<std::size_t *>(freed->userData) -= freed->dataLength;
    };
    enet_host_flush(socket_.get());
    return true;
}

bool NativeUdpTransport::SendTo(std::uint8_t peer, const std::uint8_t *data, std::size_t size)
{
    return peer == (host_ ? 1 : 0) && IsOpen() && Send(inputs, data, size);
}

bool NativeUdpTransport::SendRepairTo(std::uint8_t peer, const std::uint8_t *data, std::size_t size)
{
    return peer == (host_ ? 1 : 0) && SendControl(data, size);
}

bool NativeUdpTransport::SendControl(const std::uint8_t *data, std::size_t size)
{
    return IsOpen() && Send(control, data, size);
}

bool NativeUdpTransport::Poll(std::vector<std::uint8_t> *packet)
{
    if (!IsOpen() || incoming_.empty()) return false;
    *packet = std::move(incoming_.front());
    incoming_.pop_front();
    return true;
}
}
