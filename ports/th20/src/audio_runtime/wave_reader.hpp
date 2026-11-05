#pragma once
#include "audio.hpp"

namespace th20::source::audio {
// 0x4596d0: the old MMIO prefix is initialized by several specific stores;
// streaming paths use +8 remaining and +2c initial_remaining within that prefix.
struct WaveReader {
    std::uint32_t retained_00;
    std::uint32_t retained_04;
    std::uint32_t remaining;
    std::uint8_t retained_0c[0x2c-0xc];
    std::uint32_t initial_remaining;
    std::uint8_t retained_30[0x78-0x30];
    std::uint32_t file_mode;                  // +78
    std::uint32_t memory_mode;                // +7c
    const std::uint8_t* memory_start;         // +80
    const std::uint8_t* memory_current;       // +84
    std::uint32_t memory_size;                // +88
    HANDLE file;                             // +8c
    TrackFormat* track;                      // +90
    const char* filename;                    // +94, borrowed from SoundInf::music_file
    std::uint32_t saved_position;            // +98
    std::uint32_t looped;                    // +9c

    WaveReader() noexcept;                   // source value storage starts zero, 0x4594d0/0x4596d0
    ~WaveReader();                           // 0x459920
    HRESULT open_file(const char*,TrackFormat*,std::uint32_t mode,std::uint32_t base_offset); // 0x45ab40
    HRESULT open_memory(const std::uint8_t*,std::uint32_t,TrackFormat*,int); // 0x45ac90
    HRESULT reopen(TrackFormat*,std::uint32_t,std::uint32_t base_offset);   // 0x45af40
    HRESULT read(void*,std::uint32_t,std::uint32_t*);                      // 0x45aff0
    HRESULT reset(bool,std::uint32_t,std::uint32_t base_offset);          // 0x45b5f0
    HRESULT close();                                                     // 0x459a30
    std::uint32_t tell() const;                                           // 0x45bcc0
};
static_assert(sizeof(WaveReader)==0xa0);
}
