// P3.4：线上 digest 控制包（T13D）的 port 侧过滤代理。
//
// 动机（方案 C，零 common/ 改动）：SessionChannel::Receive 只认 Session /
// Input 两类包，未知类型直接 Fail(MalformedPacket)（common/src/netplay/
// SessionChannel.cpp:299-302,368）。digest 包必须能被 NetplayRuntime 自己
// 识别而不能让共享信道判死，因此把本代理注入为 SessionChannel 的
// PeerTransport（SessionChannel 持有 PeerTransport&）：
//   - Poll：先取底层包；4 字节魔数 == "T13D" 的包在此消化（回填 digest
//     槽 / 计数），其余包原样返回给 SessionChannel；
//   - 发送与状态面（SendTo/SendRepairTo/SendControl/IsOpen/Failed/
//     Recovering/Disconnected/CalibrationSuspended/BufferedAmount）纯转发。
// NetplayRuntime 自己继续直接使用底层 BrowserPeerTransport 的完整面
//（Connect/Close/SendControl 等不经代理）。
//
// 包格式（20B，照 th09 RollbackSession.cpp:148-150 的 T9HC）：
//   0..3  "T13D"；4..11 sessionId（8B）；12..15 frame（4B，需 60 对齐）；
//   16..19 digest（4B，FNV-1a 64 的低 32 位）。
#pragma once

#include <eagler/netplay/PeerTransport.hpp>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace th13::multiplayer {

class DigestWireFilter final : public Netplay::PeerTransport {
public:
    // 与 NetplayRuntime/Session 的 60 帧采样同频（Session::OnFrameSimulated
    // 的 wire_digest 采样点）。
    static constexpr std::uint32_t kIntervalFrames = 60;
    static constexpr std::size_t kPacketBytes = 20;
    // 单次 Poll 最多消化的 digest 包数（防对端洪泛时本调用无界循环；
    // 剩余包下一拍继续——与 th09 收包循环 256 预算同口径）。
    static constexpr std::size_t kDrainBudget = 256;

    // 有效 digest 包回调（会话匹配、20B、frame % kIntervalFrames == 0）。
    using DigestHandler = void (*)(void* context, std::uint32_t frame,
                                   std::uint32_t digest);

    explicit DigestWireFilter(Netplay::PeerTransport& inner) : inner_(inner) {}

    // 会话身份（Reset 时设置；未设置前不派发任何 digest——旧会话/噪声包
    // 只会被计数丢弃，不会回填槽位）。
    void SetSessionId(std::uint64_t session_id) {
        session_id_ = session_id;
        has_session_id_ = true;
    }
    void SetDigestHandler(DigestHandler handler, void* context) {
        handler_ = handler;
        context_ = context;
    }
    // 会话重置：清身份与计数（handler 是构造期固定接线，保留）。
    void Clear() {
        has_session_id_ = false;
        session_id_ = 0;
        ignored_ = 0;
        malformed_ = 0;
    }

    // 诊断面：非本会话的 digest 包 / 线上格式非法（非 20B、帧号未对齐）。
    std::uint64_t IgnoredDigestPackets() const { return ignored_; }
    std::uint64_t MalformedDigestPackets() const { return malformed_; }

    // ---- PeerTransport：状态/发送面纯转发 ----
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

    // ---- 接收面：消化 T13D，其余原样返回 ----
    bool Poll(std::vector<std::uint8_t>* packet) override {
        for (std::size_t drained = 0; drained < kDrainBudget; ++drained) {
            if (!inner_.Poll(packet)) return false;
            if (!IsDigestPacket(*packet)) return true;
            ConsumeDigest(*packet);
        }
        // 预算耗尽：本拍没有可交给 SessionChannel 的普通包。
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

}  // namespace th13::multiplayer
