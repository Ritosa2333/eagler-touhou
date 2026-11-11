#include "AnmTextureDecode.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include "../../../portable/sdl/third_party/stb_image.h"
namespace th13 {
bool anm_decode_png(const u8* bytes, u32 size, u32& width, u32& height, std::vector<u8>& rgba){
    if(!bytes||!size||size>64u*1024u*1024u)return false;
    int w=0,h=0,channels=0;
    auto* pixels=stbi_load_from_memory(bytes,int(size),&w,&h,&channels,4);
    if(!pixels)return false;
    width=u32(w);height=u32(h);
    rgba.assign(pixels,pixels+std::size_t(w)*std::size_t(h)*4);
    stbi_image_free(pixels);return true;
}
}
