#pragma once
#include "Types.hpp"
namespace th075 {
// Original bitmap payload and 1555 palettes, decoded without a Windows API.
struct BitmapData {
    u32 width=0,height=0,sourceStride=0,packedSize=0;u8 depth=0;
    std::array<u16,256> palette{};std::vector<u8> storage;
    bool load(ByteReader&,const std::array<u16,256>&);
    bool decode(std::vector<u8>&,u32 outputStride=0)const;
    bool encode_runs();
    u32 pixel_bytes()const{return depth>=24?4:2;}
};
struct TexturePack {
    std::vector<BitmapData> images;
    u8 paletteCount=0,selectedPalette=0;
    std::string error;
    bool load(const void*,std::size_t,u8 requestedPalette=1);
};
}
