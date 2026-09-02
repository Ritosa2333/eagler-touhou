#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace Netplay
{
constexpr std::uint8_t PROTOCOL_VERSION = 4;
constexpr std::uint32_t INVALID_FRAME = 0xffffffffu;
constexpr std::size_t MAX_PLAYERS = 3;
// Final sbrik resends a 32-frame input tail. Keep the same loss/reorder
// recovery window above the reliable WebSocket transport so relay restarts,
// deliberate jitter tests and a missing final frame do not require a second
// simulation protocol.
constexpr std::size_t MAX_REDUNDANT_INPUTS = 32;

enum class PacketType : std::uint8_t
{
    Input = 1,
    Session = 2,
    SpectatorFrame = 3,
};

enum class SessionPhase : std::uint8_t
{
    Hello = 1,
    Ready = 2,
};

enum class AnalogMode : std::uint8_t
{
    None = 0,
    Joystick = 1,
    DirectTouch = 2,
};

// One transport-neutral logical input sample. Buttons remain game-defined;
// the optional two-axis channel lets clients carry joystick or direct-pointer
// movement without letting host input leak into unsynchronized simulation.
struct FrameInput
{
    std::uint16_t buttons = 0;
    AnalogMode analogMode = AnalogMode::None;
    float x = 0.0f;
    float y = 0.0f;
    bool unlimited = false;
    bool touchUsed = false;
    bool touchBomb = false;

    FrameInput() = default;
    FrameInput(std::uint16_t value) : buttons(value) {}
};

// Read-only stream emitted by P1 only after every gameplay peer has confirmed
// the frame. Spectators never contribute a player slot or send this packet.
struct SpectatorFramePacket
{
    std::uint64_t sessionId = 0;
    std::uint32_t frame = INVALID_FRAME;
    // Spectators do not participate in the player HELLO/READY gate, so every
    // confirmed frame carries the deterministic gameplay contract explicitly.
    // This occupies the existing reserved u32 in the wire header; packet size
    // remains unchanged.
    std::uint32_t gameplayAbi = 0;
    std::uint8_t playerCount = 0;
    std::array<FrameInput, MAX_PLAYERS> inputs{};
};

bool operator==(const FrameInput &left, const FrameInput &right);
inline bool operator!=(const FrameInput &left, const FrameInput &right)
{
    return !(left == right);
}

// Transport-neutral input packet. WebRTC/WebSocket are deliberately outside
// this layer. Packet loss is tolerated by repeating unacknowledged inputs in
// later packets, following the useful part of Giuroll's packet strategy.
struct InputPacket
{
    std::uint64_t sessionId = 0;
    std::uint32_t sequence = 0;
    std::uint32_t ackSequence = 0;
    std::uint32_t latestFrame = INVALID_FRAME;
    std::uint32_t ackFrame = INVALID_FRAME;
    std::uint32_t firstInputFrame = INVALID_FRAME;
    // Sender's current simulation frame is intentionally separate from
    // latestFrame: retransmission may make latestFrame point at an older
    // unacknowledged input.  frameAdvantage is the sender's estimate of
    // senderFrame - newest remote senderFrame it has observed, used only by
    // wall-clock time synchronization and never by deterministic gameplay.
    std::uint32_t senderFrame = INVALID_FRAME;
    std::int16_t frameAdvantage = 0;
    std::uint8_t senderPlayer = 0;
    std::uint8_t playerCount = 0;
    std::uint8_t inputCount = 0;
    std::array<FrameInput, MAX_REDUNDANT_INPUTS> inputs{};
};

// Fixed-size control packet exchanged before frame zero. The room/lobby layer
// supplies the descriptor; Runtime only verifies that every peer received the
// exact same deterministic session contract before simulation is released.
struct SessionPacket
{
    std::uint64_t sessionId = 0;
    std::uint32_t seed = 0;
    std::uint32_t gameplayAbi = 0;
    std::uint32_t gameId = 0;
    std::uint8_t senderPlayer = 0;
    std::uint8_t playerCount = 0;
    SessionPhase phase = SessionPhase::Hello;
};

bool PeekPacketType(const std::uint8_t *data, std::size_t size, PacketType *out);
bool EncodeInputPacket(const InputPacket &packet, std::vector<std::uint8_t> *out);
bool DecodeInputPacket(const std::uint8_t *data, std::size_t size, InputPacket *out);
bool EncodeSessionPacket(const SessionPacket &packet, std::vector<std::uint8_t> *out);
bool DecodeSessionPacket(const std::uint8_t *data, std::size_t size, SessionPacket *out);
bool EncodeSpectatorFramePacket(const SpectatorFramePacket &packet,
                                std::vector<std::uint8_t> *out);
bool DecodeSpectatorFramePacket(const std::uint8_t *data, std::size_t size,
                                SpectatorFramePacket *out);
} // namespace Netplay
