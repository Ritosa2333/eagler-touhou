#include "SpectatorStream.hpp"
#include <eagler/netplay/NativeRoomTransport.hpp>
#include <rtc/websocket.hpp>
#include <array>
#include <algorithm>
#include <cstring>
#include <deque>
#include <exception>
#include <memory>
#include <stdexcept>
#include <utility>

namespace th09::desktop {
namespace {
std::unique_ptr<rtc::WebSocket> socket;
std::deque<std::vector<std::uint8_t>> packets;
std::size_t queuedBytes = 0;
std::array<std::uint8_t, 12> buildBytes;
bool viewing = false;
bool failed = false;
bool matchActive = false;
constexpr std::size_t maxQueued = 8192;

void word(std::vector<std::uint8_t>& packet, std::size_t at, std::uint32_t value) {
    for (unsigned i = 0; i < 4; ++i) packet[at + i] = static_cast<std::uint8_t>(value >> (8 * i));
}

std::vector<std::uint8_t> packet(const std::array<std::uint8_t, 8>& signature, std::uint32_t frame, std::size_t size) {
    std::vector<std::uint8_t> result(size + 1);
    result[0] = 0xe8;
    std::copy(signature.begin(), signature.end(), result.begin() + 1);
    word(result, 9, frame);
    std::copy(buildBytes.begin(), buildBytes.end(), result.begin() + 13);
    return result;
}

void enqueue(std::vector<std::uint8_t> value) {
    if (packets.size() >= maxQueued || queuedBytes + value.size() > 16 * 1024 * 1024) {
        failed = true;
        return;
    }
    queuedBytes += value.size();
    packets.push_back(std::move(value));
}
}

bool openSpectatorStream(const std::string& url, const std::string& build, bool viewer) {
    closeSpectatorStream();
    viewing = viewer;
    if (build.size() != 24 || build.find_first_not_of("0123456789abcdef") != std::string::npos) return false;
    try {
        for (std::size_t i = 0; i < buildBytes.size(); ++i)
            buildBytes[i] = static_cast<std::uint8_t>(std::stoul(build.substr(i * 2, 2), nullptr, 16));
        socket = Netplay::OpenRoomWebSocket(url);
        return true;
    } catch (const std::exception&) {
        closeSpectatorStream();
        return false;
    }
}

void closeSpectatorStream() {
    if (socket) socket->forceClose();
    socket.reset();
    packets.clear();
    queuedBytes = 0;
    matchActive = false;
    failed = false;
}

bool spectatorStreamFailed() { return failed || (socket && socket->isClosed()); }

void pumpSpectatorStream() {
    if (!socket || !socket->isOpen()) return;
    if (failed && !viewing) {
        const std::uint8_t stop[] = {0xe8, 'S', 'T', 'O', 'P', 1};
        try { socket->send(reinterpret_cast<const rtc::byte*>(stop), sizeof(stop)); }
        catch (const std::exception&) {}
        closeSpectatorStream();
        return;
    }
    try {
        for (unsigned i = 0; i < 32; ++i) {
            if (viewing) {
                auto next = socket->receive();
                if (!next) break;
                if (auto* binary = std::get_if<rtc::binary>(&*next)) {
                    const auto* bytes = reinterpret_cast<const std::uint8_t*>(binary->data());
                    if (binary->size() < 24 || std::memcmp(bytes + 12, buildBytes.data(), buildBytes.size())) {
                        failed = true;
                        break;
                    }
                    enqueue(std::vector<std::uint8_t>(bytes, bytes + binary->size()));
                }
            } else {
                if (packets.empty() || socket->bufferedAmount() > 64 * 1024) break;
                const auto& next = packets.front();
                socket->send(reinterpret_cast<const rtc::byte*>(next.data()), next.size());
                queuedBytes -= next.size();
                packets.pop_front();
            }
        }
    } catch (const std::exception&) { failed = true; }
}

bool takeSpectatorPacket(std::vector<std::uint8_t>& packet) {
    if (!viewing || packets.empty()) return false;
    packet = std::move(packets.front());
    queuedBytes -= packet.size();
    packets.pop_front();
    return true;
}

void publishTiming(const std::uint32_t* startup, bool automatic) {
    if (!socket || viewing || failed || startup[1] != 4 || !startup[13]) return;
    auto result = packet({'T','9','T','M',1,static_cast<std::uint8_t>(startup[13]),
        static_cast<std::uint8_t>(startup[10]),static_cast<std::uint8_t>(startup[11])}, startup[9], 40);
    word(result, 25, (std::max)(startup[5], startup[6]));
    word(result, 29, startup[7] + startup[8]);
    word(result, 33, automatic ? 1 : 0);
    word(result, 37, (std::min)(startup[14], startup[15]));
    packets.push_front(std::move(result));
    queuedBytes += 41;
}

void publishBegin(std::uint32_t frame, const void* bytes, std::uint32_t size) {
    if (!socket || viewing || failed || (size != 50 && size != 78)) return;
    auto result = packet({'T','9','B','G',static_cast<std::uint8_t>(size == 78 ? 2 : 1),0,2,0}, frame, 24 + size);
    std::memcpy(result.data() + 25, bytes, size);
    enqueue(std::move(result));
    matchActive = true;
}

void publishEnd(std::uint32_t frame) {
    if (!socket || viewing || failed || !matchActive) return;
    enqueue(packet({'T','9','S','E',1,0,2,0}, frame, 24));
    matchActive = false;
}

void publishRound(std::uint32_t frame, std::uint32_t round) {
    if (!socket || viewing || failed || !matchActive) return;
    auto result = packet({'T','9','R','D',1,0,2,0}, frame, 28);
    word(result, 25, round);
    enqueue(std::move(result));
}

void publishInput(std::uint32_t frame, unsigned left, unsigned right, int leftMode, float leftX, float leftY,
    int rightMode, float rightX, float rightY, const void* charge, unsigned chargeSize) {
    if (!socket || viewing || failed || !matchActive) return;
    const bool extended = charge && chargeSize == 80;
    auto result = packet({'T','9','S','P',static_cast<std::uint8_t>(extended ? 2 : 1),3,2,0}, frame, extended ? 126 : 46);
    const auto writeSide = [&](std::size_t at, unsigned keys, int mode, float x, float y) {
        result[at] = static_cast<std::uint8_t>(keys); result[at + 1] = static_cast<std::uint8_t>(keys >> 8);
        result[at + 2] = static_cast<std::uint8_t>(mode);
        std::uint32_t xBits, yBits;
        std::memcpy(&xBits, &x, sizeof(xBits));
        std::memcpy(&yBits, &y, sizeof(yBits));
        word(result, at + 3, xBits);
        word(result, at + 7, yBits);
    };
    writeSide(25, left, leftMode, leftX, leftY);
    writeSide(36, right, rightMode, rightX, rightY);
    if (extended) std::memcpy(result.data() + 47, charge, chargeSize);
    enqueue(std::move(result));
}
}
