#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer spectator stage scene must not enter an ordinary build
#endif

#include <cstdint>
#include <vector>

namespace th10::browser { struct World; }
namespace Netplay::SpectatorScalars { class Writer; class Reader; }

namespace th10::multiplayer {
struct SpectatorStageState;

// The byte payload is a bounded LZSS stream of explicitly visited scalar
// fields. It is not a dump of a Stage, ANM VM, allocator, or resource graph.
struct SpectatorStageAnmHandle {
    std::uint32_t sourceId=0;
    std::uint32_t targetId=0;
};
struct SpectatorStageSceneSnapshot {
    bool present=false;
    std::uint32_t uncompressedBytes=0;
    std::uint32_t checksum=0;
    std::vector<std::uint8_t> compressed;
    // Runtime-only remaps produced while restoring; never encoded on the wire.
    mutable std::vector<SpectatorStageAnmHandle> resolvedHandles;
};

bool CaptureSpectatorStageScene(browser::World&,SpectatorStageState&);
bool RestoreSpectatorStageScene(browser::World&,const SpectatorStageState&);
std::uint32_t ResolveSpectatorStageAnm(const browser::World&,
                                      const SpectatorStageState&,
                                      std::uint32_t sourceId);

bool ValidateSpectatorStageScene(const SpectatorStageSceneSnapshot&);
bool PackSpectatorStageFields(const std::vector<std::uint8_t>& raw,
                              std::uint32_t& uncompressedBytes,
                              std::uint32_t& checksum,
                              std::vector<std::uint8_t>& compressed);
bool UnpackSpectatorStageFields(std::uint32_t rawSize,
                                std::uint32_t checksum,
                                const std::vector<std::uint8_t>& compressed,
                                std::vector<std::uint8_t>& raw);
bool WriteSpectatorStageScene(Netplay::SpectatorScalars::Writer&,
                              const SpectatorStageSceneSnapshot&);
bool ReadSpectatorStageScene(Netplay::SpectatorScalars::Reader&,
                             SpectatorStageSceneSnapshot&);
}
