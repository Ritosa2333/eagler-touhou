#pragma once

#include <eagler/netplay/NetplayProtocol.hpp>
#include <algorithm>
#include <cstdint>
#include <vector>

namespace Netplay {

// An extension of the receive-only lane. Player packets and TH09 are unchanged.
enum class SpectatorEventKind : std::uint8_t { Checkpoint = 1, Node = 2, End = 3, Bootstrap = 4 };
enum class SpectatorNodeKind : std::uint8_t { Stage = 1, Boss = 2 };

struct SpectatorStageEvent {
    std::uint8_t game = 0, playerCount = 0;
    SpectatorEventKind kind = SpectatorEventKind::Checkpoint;
    std::uint64_t sessionId = 0;
    std::uint32_t gameplayAbi = 0, frame = INVALID_FRAME, stage = 0, serial = 0;
    std::uint32_t schema = 0, checkpoint = 0;
    SpectatorNodeKind node = SpectatorNodeKind::Stage;
    std::uint8_t bossSlots = 0;
    // Title-owned, versioned scalar initializer. Never a native memory image.
    std::vector<std::uint8_t> state;
};

inline constexpr std::size_t SpectatorStageHeaderBytes = 32;
// Native Replay stage headers and multiplayer resource extensions.
inline constexpr std::size_t SpectatorStageMaxStateBytes = 4 * 1024;

inline bool IsCooperativeSpectatorGame(std::uint8_t game) {
    return game == '6' || game == '7' || game == '8' ||
           game == 'A' || game == 'B' || game == 'F';
}

inline bool IsSpectatorStageEvent(const std::uint8_t* data, std::size_t size) {
    return data && size >= 4 && data[0] == 'E' &&
           IsCooperativeSpectatorGame(data[1]) && data[2] == 'S' && data[3] == 'V';
}

namespace SpectatorStageWire {
inline void Put(std::vector<std::uint8_t>& bytes, std::size_t at,
                std::uint64_t value, unsigned count = 4) {
    for (unsigned i = 0; i < count; ++i) bytes[at + i] = std::uint8_t(value >> (8 * i));
}
inline std::uint64_t Get(const std::uint8_t* bytes, std::size_t at, unsigned count = 4) {
    std::uint64_t value = 0;
    for (unsigned i = 0; i < count; ++i) value |= std::uint64_t(bytes[at + i]) << (8 * i);
    return value;
}
inline bool Valid(const SpectatorStageEvent& event) {
    if (!IsCooperativeSpectatorGame(event.game) || event.playerCount < 2 ||
        event.playerCount > MAX_PLAYERS || !event.sessionId || !event.gameplayAbi ||
        event.frame == INVALID_FRAME || event.stage > 65535 || !event.serial) return false;
    switch (event.kind) {
    case SpectatorEventKind::Checkpoint:
        return event.schema && !event.state.empty() &&
               event.state.size() <= SpectatorStageMaxStateBytes;
    case SpectatorEventKind::Node:
        return event.state.empty() && event.checkpoint &&
            ((event.node == SpectatorNodeKind::Stage && !event.bossSlots) ||
             (event.node == SpectatorNodeKind::Boss && event.bossSlots));
    case SpectatorEventKind::End:
    case SpectatorEventKind::Bootstrap: // Relay -> viewer only, never retained.
        return event.state.empty();
    }
    return false;
}
}

inline bool EncodeSpectatorStageEvent(const SpectatorStageEvent& event,
                                     std::vector<std::uint8_t>* output) {
    if (!output || !SpectatorStageWire::Valid(event)) return false;
    const auto extra = event.kind == SpectatorEventKind::Checkpoint || event.kind == SpectatorEventKind::Node ? 8u : 0u;
    std::vector<std::uint8_t> bytes(SpectatorStageHeaderBytes + extra + event.state.size());
    bytes[0] = 'E'; bytes[1] = event.game; bytes[2] = 'S'; bytes[3] = 'V';
    bytes[4] = 2; bytes[5] = std::uint8_t(event.kind); bytes[6] = event.playerCount;
    using namespace SpectatorStageWire;
    Put(bytes, 8, event.sessionId, 8); Put(bytes, 16, event.gameplayAbi);
    Put(bytes, 20, event.frame); Put(bytes, 24, event.stage); Put(bytes, 28, event.serial);
    if (event.kind == SpectatorEventKind::Checkpoint) {
        Put(bytes, 32, event.schema); Put(bytes, 36, event.state.size());
        std::copy(event.state.begin(), event.state.end(), bytes.begin() + 40);
    } else if (event.kind == SpectatorEventKind::Node) {
        Put(bytes, 32, event.checkpoint); bytes[36] = std::uint8_t(event.node);
        bytes[37] = event.bossSlots;
    }
    *output = std::move(bytes);
    return true;
}

inline bool DecodeSpectatorStageEvent(const std::uint8_t* data, std::size_t size,
                                     SpectatorStageEvent* output) {
    if (!output || !IsSpectatorStageEvent(data, size) || size < SpectatorStageHeaderBytes ||
        size > 40 + SpectatorStageMaxStateBytes || data[4] != 2 || data[7]) return false;
    using namespace SpectatorStageWire;
    SpectatorStageEvent event;
    event.game = data[1]; event.kind = SpectatorEventKind(data[5]); event.playerCount = data[6];
    event.sessionId = Get(data, 8, 8); event.gameplayAbi = std::uint32_t(Get(data, 16));
    event.frame = std::uint32_t(Get(data, 20)); event.stage = std::uint32_t(Get(data, 24));
    event.serial = std::uint32_t(Get(data, 28));
    if (event.kind == SpectatorEventKind::Checkpoint) {
        if (size < 40 || Get(data, 36) != size - 40) return false;
        event.schema = std::uint32_t(Get(data, 32));
        event.state.assign(data + 40, data + size);
    } else if (event.kind == SpectatorEventKind::Node) {
        if (size != 40 || data[38] || data[39]) return false;
        event.checkpoint = std::uint32_t(Get(data, 32));
        event.node = SpectatorNodeKind(data[36]); event.bossSlots = data[37];
    } else if ((event.kind != SpectatorEventKind::End && event.kind != SpectatorEventKind::Bootstrap) || size != 32) return false;
    if (!Valid(event)) return false;
    *output = std::move(event);
    return true;
}
}
