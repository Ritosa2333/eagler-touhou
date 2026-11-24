#pragma once

#include <eagler/netplay/PeerTransport.hpp>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace th13::multiplayer {

class DigestWireFilter final : public Netplay::PeerTransport {
public:
    static constexpr std::uint32_t kIntervalFrames = 60;
    static constexpr std::size_t kPacketBytes = 20;
    static constexpr std::size_t kDrainBudget = 256;

    using DigestHandler = void (*)(void* context, std::uint32_t frame,
                                   std::uint32_t digest);

    explicit DigestWireFilter(Netplay::PeerTransport& inner) : inner_(inner) {}

    void SetSessionId(std::uint64_t session_id) {
        session_id_ = session_id;
        has_session_id_ = true;
    }
    void SetDigestHandler(DigestHandler handler, void* context) {
        handler_ = handler;
        context_ = context;
    }
    void Clear() {
        has_session_id_ = false;
        session_id_ = 0;
        ignored_ = 0;
        malformed_ = 0;
    }

    std::uint64_t IgnoredDigestPackets() const { return ignored_; }
    std::uint64_t MalformedDigestPackets() const { return malformed_; }

    bool IsOpen() const override { return inner_.IsOpen(); }
    bool Failed() const override { return inner_.Failed(); }
    bool Recovering() const override { return inner_.Recovering(); }
    bool Disconnected() const override { return inner_.Disconnected(); }
    bool CalibrationSuspended() const override {
        return inner_.CalibrationSuspended();
    }
    bool SendTo(std::uint8_t peer, const std::uint8_t* data,
                std::size_t size) override {
        return inner_.SendTo(peer, data, size);
    }
    bool SendRepairTo(std::uint8_t peer, const std::uint8_t* data,
                      std::size_t size) override {
        return inner_.SendRepairTo(peer, data, size);
    }
    bool SendControl(const std::uint8_t* data, std::size_t size) override {
        return inner_.SendControl(data, size);
    }
    std::size_t BufferedAmount() const override { return inner_.BufferedAmount(); }

    bool Poll(std::vector<std::uint8_t>* packet) override {
        for (std::size_t drained = 0; drained < kDrainBudget; ++drained) {
            if (!inner_.Poll(packet)) return false;
            if (!IsDigestPacket(*packet)) return true;
            ConsumeDigest(*packet);
        }
        return false;
    }

private:
    static bool IsDigestPacket(const std::vector<std::uint8_t>& packet) {
        return packet.size() >= 4 &&
               packet[0] == std::uint8_t('T') &&
               packet[1] == std::uint8_t('1') &&
               packet[2] == std::uint8_t('3') &&
               packet[3] == std::uint8_t('D');
    }

    void ConsumeDigest(const std::vector<std::uint8_t>& packet) {
        if (packet.size() != kPacketBytes) {
            ++malformed_;
            return;
        }
        std::uint64_t session = 0;
        std::uint32_t frame = 0, digest = 0;
        std::memcpy(&session, packet.data() + 4, sizeof session);
        std::memcpy(&frame, packet.data() + 12, sizeof frame);
        std::memcpy(&digest, packet.data() + 16, sizeof digest);
        if (!has_session_id_ || session != session_id_) {
            ++ignored_;
            return;
        }
        if (kIntervalFrames == 0 || frame % kIntervalFrames != 0) {
            ++malformed_;
            return;
        }
        if (handler_) handler_(context_, frame, digest);
    }

    Netplay::PeerTransport& inner_;
    DigestHandler handler_ = nullptr;
    void* context_ = nullptr;
    std::uint64_t session_id_ = 0;
    bool has_session_id_ = false;
    std::uint64_t ignored_ = 0;
    std::uint64_t malformed_ = 0;
};

}
