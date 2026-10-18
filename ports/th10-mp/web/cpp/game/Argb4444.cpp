#include "Argb4444.hpp"
namespace th10 {
void Argb4444::unpack(const u8* source,float* rgba,u32 width) noexcept {
    float levels[16];for(i32 i=0;i<16;++i)levels[i]=(Extended::from_int(i)*number(0x1.111112p-4f)).to_float();
    for(u32 i=0;i<width;++i){u16 value;std::memcpy(&value,source+i*2,2);rgba[i*4]=levels[(value>>8)&15];rgba[i*4+1]=levels[(value>>4)&15];rgba[i*4+2]=levels[value&15];rgba[i*4+3]=levels[value>>12];}
}
bool Argb4444::pack(const float* rgba,u8* destination,u32 width) noexcept {
    // With an ordinary float32 input in this interval, the half-step threshold
    // cannot cross an integer boundary through the DLL's intermediate stores.
    // Exceptional values, diffusion and nonuniform dithering use their own path.
    for(u32 i=0;i<width*4;++i)if(!(rgba[i]>=-1&&rgba[i]<=1))return false;
    const auto quantize=[](float value){const i32 n=static_cast<i32>(static_cast<double>(value)*15+0.5);return n<0?0:n>15?15:n;};
    for(u32 i=0;i<width;++i){const auto* p=rgba+i*4;const u16 n=(quantize(p[3])<<12)|(quantize(p[0])<<8)|(quantize(p[1])<<4)|quantize(p[2]);std::memcpy(destination+i*2,&n,2);}return true;
}
}
