#pragma once
#include <array>
#include <cstdint>

namespace th08::audit {
// Synchronous host operations, independent of the asynchronously draining device.
enum class AudioOperation : std::uint32_t {
    Initialize, CreatePcm, AttachOgg, MusicFormat, ResourceChange, Stop, Seek,
    Pan, Volume, Play, Release, MidiOpen, MidiClose, MidiMessage, Pump,
    StreamWrite, Pause, MusicSpeed, Shutdown, MusicEnabled, DecodeMode,
    ClearSource, ManagerOperation, Count
};
inline constexpr auto audio_operation_count=static_cast<std::uint32_t>(AudioOperation::Count);
#if defined(TH_PRESENTATION_AUDIT)
inline std::array<std::uint32_t,audio_operation_count+2> audio_operations{{1,0}};
inline void audio_operation(AudioOperation operation){
    ++audio_operations[1];++audio_operations[2+static_cast<std::uint32_t>(operation)];
}
inline const std::uint32_t* audio_snapshot(){return audio_operations.data();}
#else
inline void audio_operation(AudioOperation){}
#endif
}
