#include "NetplayProtocol.hpp"

#include <cmath>
#include <cstring>

namespace Netplay
{
namespace
{
constexpr std::uint8_t MAGIC[4] = {'E', '6', 'N', 'P'};
constexpr std::size_t HEADER_SIZE = 44;
constexpr std::size_t SESSION_PACKET_SIZE = 32;
constexpr std::size_t INPUT_SAMPLE_SIZE = 12;
constexpr std::size_t SPECTATOR_FRAME_HEADER_SIZE = 24;

void PutU16(std::vector<std::uint8_t> *out, std::uint16_t value)
{
    out->push_back(static_cast<std::uint8_t>(value));
    out->push_back(static_cast<std::uint8_t>(value >> 8));
}

void PutU32(std::vector<std::uint8_t> *out, std::uint32_t value)
{
    for (int shift = 0; shift < 32; shift += 8)
        out->push_back(static_cast<std::uint8_t>(value >> shift));
}

void PutU64(std::vector<std::uint8_t> *out, std::uint64_t value)
{
    for (int shift = 0; shift < 64; shift += 8)
        out->push_back(static_cast<std::uint8_t>(value >> shift));
}

bool GetU16(const std::uint8_t *data, std::size_t size, std::size_t *at, std::uint16_t *out)
{
    if (*at + 2 > size)
        return false;
    *out = static_cast<std::uint16_t>(data[*at]) |
           static_cast<std::uint16_t>(data[*at + 1] << 8);
    *at += 2;
    return true;
}

bool GetU32(const std::uint8_t *data, std::size_t size, std::size_t *at, std::uint32_t *out)
{
    if (*at + 4 > size)
        return false;
    *out = static_cast<std::uint32_t>(data[*at]) |
           (static_cast<std::uint32_t>(data[*at + 1]) << 8) |
           (static_cast<std::uint32_t>(data[*at + 2]) << 16) |
           (static_cast<std::uint32_t>(data[*at + 3]) << 24);
    *at += 4;
    return true;
}

bool GetU64(const std::uint8_t *data, std::size_t size, std::size_t *at, std::uint64_t *out)
{
    if (*at + 8 > size)
        return false;
    std::uint64_t value = 0;
    for (int shift = 0; shift < 64; shift += 8)
        value |= static_cast<std::uint64_t>(data[(*at)++]) << shift;
    *out = value;
    return true;
}

bool ValidInput(const FrameInput &input)
{
    return (input.analogMode == AnalogMode::None ||
            input.analogMode == AnalogMode::Joystick ||
            input.analogMode == AnalogMode::DirectTouch) &&
           std::isfinite(input.x) && std::isfinite(input.y);
}

void PutFloat(std::vector<std::uint8_t> *out, float value)
{
    std::uint32_t bits = 0;
    static_assert(sizeof(bits) == sizeof(value));
    std::memcpy(&bits, &value, sizeof(bits));
    PutU32(out, bits);
}

bool GetFloat(const std::uint8_t *data, std::size_t size, std::size_t *at, float *out)
{
    std::uint32_t bits = 0;
    if (!GetU32(data, size, at, &bits))
        return false;
    std::memcpy(out, &bits, sizeof(bits));
    return std::isfinite(*out);
}

void PutInput(std::vector<std::uint8_t> *out, const FrameInput &input)
{
    PutU16(out, input.buttons);
    out->push_back(static_cast<std::uint8_t>(input.analogMode));
    out->push_back((input.unlimited ? 1u : 0u) |
                   (input.touchUsed ? 2u : 0u) |
                   (input.touchBomb ? 4u : 0u));
    PutFloat(out, input.x);
    PutFloat(out, input.y);
}

bool GetInput(const std::uint8_t *data, std::size_t size, std::size_t *at, FrameInput *out)
{
    if (!GetU16(data, size, at, &out->buttons) || *at + 2 > size)
        return false;
    out->analogMode = static_cast<AnalogMode>(data[(*at)++]);
    const std::uint8_t flags = data[(*at)++];
    out->unlimited = (flags & 1u) != 0;
    out->touchUsed = (flags & 2u) != 0;
    out->touchBomb = (flags & 4u) != 0;
    return (flags & ~7u) == 0 &&
           GetFloat(data, size, at, &out->x) &&
           GetFloat(data, size, at, &out->y) &&
           ValidInput(*out);
}
} // namespace

bool operator==(const FrameInput &left, const FrameInput &right)
{
    return left.buttons == right.buttons && left.analogMode == right.analogMode &&
           left.x == right.x && left.y == right.y && left.unlimited == right.unlimited &&
           left.touchUsed == right.touchUsed && left.touchBomb == right.touchBomb;
}

bool PeekPacketType(const std::uint8_t *data, std::size_t size, PacketType *out)
{
    if (!data || !out || size < 6 ||
        data[0] != MAGIC[0] || data[1] != MAGIC[1] ||
        data[2] != MAGIC[2] || data[3] != MAGIC[3] ||
        data[4] != PROTOCOL_VERSION)
        return false;
    const auto type = static_cast<PacketType>(data[5]);
    if (type != PacketType::Input && type != PacketType::Session &&
        type != PacketType::SpectatorFrame)
        return false;
    *out = type;
    return true;
}

bool EncodeInputPacket(const InputPacket &packet, std::vector<std::uint8_t> *out)
{
    if (!out || packet.playerCount < 2 || packet.playerCount > MAX_PLAYERS ||
        packet.senderPlayer >= packet.playerCount ||
        packet.inputCount > MAX_REDUNDANT_INPUTS)
        return false;
    if (packet.inputCount > 0)
    {
        if (packet.firstInputFrame == INVALID_FRAME || packet.latestFrame == INVALID_FRAME ||
            packet.firstInputFrame > INVALID_FRAME - (packet.inputCount - 1) ||
            packet.firstInputFrame + packet.inputCount - 1 != packet.latestFrame)
            return false;
    }

    out->clear();
    for (std::size_t i = 0; i < packet.inputCount; ++i)
        if (!ValidInput(packet.inputs[i]))
            return false;

    out->reserve(HEADER_SIZE + packet.inputCount * INPUT_SAMPLE_SIZE);
    out->insert(out->end(), MAGIC, MAGIC + 4);
    out->push_back(PROTOCOL_VERSION);
    out->push_back(static_cast<std::uint8_t>(PacketType::Input));
    out->push_back(packet.senderPlayer);
    out->push_back(packet.playerCount);
    PutU64(out, packet.sessionId);
    PutU32(out, packet.sequence);
    PutU32(out, packet.ackSequence);
    PutU32(out, packet.latestFrame);
    PutU32(out, packet.ackFrame);
    PutU32(out, packet.firstInputFrame);
    PutU32(out, packet.senderFrame);
    PutU16(out, static_cast<std::uint16_t>(packet.frameAdvantage));
    out->push_back(packet.inputCount);
    out->push_back(0);
    for (std::size_t i = 0; i < packet.inputCount; ++i)
        PutInput(out, packet.inputs[i]);
    return out->size() == HEADER_SIZE + packet.inputCount * INPUT_SAMPLE_SIZE;
}

bool DecodeInputPacket(const std::uint8_t *data, std::size_t size, InputPacket *out)
{
    if (!data || !out || size < HEADER_SIZE ||
        data[0] != MAGIC[0] || data[1] != MAGIC[1] ||
        data[2] != MAGIC[2] || data[3] != MAGIC[3] ||
        data[4] != PROTOCOL_VERSION || data[5] != static_cast<std::uint8_t>(PacketType::Input))
        return false;

    InputPacket packet;
    packet.senderPlayer = data[6];
    packet.playerCount = data[7];
    if (packet.playerCount < 2 || packet.playerCount > MAX_PLAYERS ||
        packet.senderPlayer >= packet.playerCount)
        return false;

    std::size_t at = 8;
    if (!GetU64(data, size, &at, &packet.sessionId) ||
        !GetU32(data, size, &at, &packet.sequence) ||
        !GetU32(data, size, &at, &packet.ackSequence) ||
        !GetU32(data, size, &at, &packet.latestFrame) ||
        !GetU32(data, size, &at, &packet.ackFrame) ||
        !GetU32(data, size, &at, &packet.firstInputFrame) ||
        !GetU32(data, size, &at, &packet.senderFrame))
        return false;
    std::uint16_t frameAdvantageBits = 0;
    if (!GetU16(data, size, &at, &frameAdvantageBits) || at + 2 > size)
        return false;
    packet.frameAdvantage = static_cast<std::int16_t>(frameAdvantageBits);
    packet.inputCount = data[at];
    at += 2; // count + reserved
    if (packet.inputCount > MAX_REDUNDANT_INPUTS ||
        size != HEADER_SIZE + packet.inputCount * INPUT_SAMPLE_SIZE)
        return false;
    if (packet.inputCount == 0)
    {
        if (packet.firstInputFrame != INVALID_FRAME)
            return false;
    }
    else
    {
        if (packet.firstInputFrame == INVALID_FRAME || packet.latestFrame == INVALID_FRAME ||
            packet.firstInputFrame > INVALID_FRAME - (packet.inputCount - 1) ||
            packet.firstInputFrame + packet.inputCount - 1 != packet.latestFrame)
            return false;
    }
    for (std::size_t i = 0; i < packet.inputCount; ++i)
        if (!GetInput(data, size, &at, &packet.inputs[i]))
            return false;
    *out = packet;
    return true;
}

bool EncodeSessionPacket(const SessionPacket &packet, std::vector<std::uint8_t> *out)
{
    if (!out || packet.playerCount < 2 || packet.playerCount > MAX_PLAYERS ||
        packet.senderPlayer >= packet.playerCount ||
        (packet.phase != SessionPhase::Hello && packet.phase != SessionPhase::Ready))
        return false;

    out->clear();
    out->reserve(SESSION_PACKET_SIZE);
    out->insert(out->end(), MAGIC, MAGIC + 4);
    out->push_back(PROTOCOL_VERSION);
    out->push_back(static_cast<std::uint8_t>(PacketType::Session));
    out->push_back(packet.senderPlayer);
    out->push_back(packet.playerCount);
    PutU64(out, packet.sessionId);
    PutU32(out, packet.seed);
    PutU32(out, packet.gameplayAbi);
    PutU32(out, packet.gameId);
    out->push_back(static_cast<std::uint8_t>(packet.phase));
    out->push_back(0);
    out->push_back(0);
    out->push_back(0);
    return out->size() == SESSION_PACKET_SIZE;
}

bool DecodeSessionPacket(const std::uint8_t *data, std::size_t size, SessionPacket *out)
{
    if (!data || !out || size != SESSION_PACKET_SIZE)
        return false;
    PacketType type;
    if (!PeekPacketType(data, size, &type) || type != PacketType::Session)
        return false;

    SessionPacket packet;
    packet.senderPlayer = data[6];
    packet.playerCount = data[7];
    if (packet.playerCount < 2 || packet.playerCount > MAX_PLAYERS ||
        packet.senderPlayer >= packet.playerCount)
        return false;
    std::size_t at = 8;
    if (!GetU64(data, size, &at, &packet.sessionId) ||
        !GetU32(data, size, &at, &packet.seed) ||
        !GetU32(data, size, &at, &packet.gameplayAbi) ||
        !GetU32(data, size, &at, &packet.gameId))
        return false;
    packet.phase = static_cast<SessionPhase>(data[at]);
    if (packet.phase != SessionPhase::Hello && packet.phase != SessionPhase::Ready)
        return false;
    *out = packet;
    return true;
}

bool EncodeSpectatorFramePacket(const SpectatorFramePacket &packet,
                                std::vector<std::uint8_t> *out)
{
    if (!out || packet.frame == INVALID_FRAME || packet.playerCount < 2 ||
        packet.playerCount > MAX_PLAYERS)
        return false;
    for (std::uint8_t player = 0; player < packet.playerCount; ++player)
        if (!ValidInput(packet.inputs[player]))
            return false;
    out->clear();
    out->reserve(SPECTATOR_FRAME_HEADER_SIZE + packet.playerCount * INPUT_SAMPLE_SIZE);
    out->insert(out->end(), MAGIC, MAGIC + 4);
    out->push_back(PROTOCOL_VERSION);
    out->push_back(static_cast<std::uint8_t>(PacketType::SpectatorFrame));
    out->push_back(packet.playerCount);
    out->push_back(0);
    PutU64(out, packet.sessionId);
    PutU32(out, packet.frame);
    PutU32(out, packet.gameplayAbi);
    for (std::uint8_t player = 0; player < packet.playerCount; ++player)
        PutInput(out, packet.inputs[player]);
    return out->size() == SPECTATOR_FRAME_HEADER_SIZE +
                              packet.playerCount * INPUT_SAMPLE_SIZE;
}

bool DecodeSpectatorFramePacket(const std::uint8_t *data, std::size_t size,
                                SpectatorFramePacket *out)
{
    PacketType type;
    if (!data || !out || !PeekPacketType(data, size, &type) ||
        type != PacketType::SpectatorFrame || size < SPECTATOR_FRAME_HEADER_SIZE)
        return false;
    SpectatorFramePacket packet;
    packet.playerCount = data[6];
    if (packet.playerCount < 2 || packet.playerCount > MAX_PLAYERS || data[7] != 0 ||
        size != SPECTATOR_FRAME_HEADER_SIZE + packet.playerCount * INPUT_SAMPLE_SIZE)
        return false;
    std::size_t at = 8;
    if (!GetU64(data, size, &at, &packet.sessionId) ||
        !GetU32(data, size, &at, &packet.frame) ||
        !GetU32(data, size, &at, &packet.gameplayAbi) || packet.frame == INVALID_FRAME)
        return false;
    for (std::uint8_t player = 0; player < packet.playerCount; ++player)
        if (!GetInput(data, size, &at, &packet.inputs[player]))
            return false;
    *out = packet;
    return true;
}
} // namespace Netplay
