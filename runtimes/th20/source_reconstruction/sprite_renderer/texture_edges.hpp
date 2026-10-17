#pragma once
#include "../program_entry/device.hpp"
#include <cstdint>
namespace th20::source::sprite {
// Original451910: fixes RGB of alpha-zero texels using the mean RGB of the
// nonzero-alpha four-connected neighbors. Alpha and opaque texels stay intact.
// Supported formats: A8R8G8B8 (and UNKNOWN alias0), A1R5G5B5, A4R4G4B4,
// A8R3G3B2. Unsupported formats still perform original lock/unlock/release.
// TH_SDL3 textures are always A8R8G8B8, so only that branch executes on wasm.
void repair_transparent_texels(DeviceTexture&);
namespace texture_edge_detail {
void accumulate_argb1555(std::uint32_t* rgb,const std::uint16_t*,std::uint32_t& count); // 451640
void accumulate_argb4444(std::uint32_t* rgb,const std::uint16_t*,std::uint32_t& count); // 451700
void accumulate_argb8332(std::uint32_t* rgb,const std::uint16_t*,std::uint32_t& count); // 4517c0
void accumulate_argb8888(std::uint32_t* rgb,const std::uint8_t*,std::uint32_t& count);  // 451880
}
}
