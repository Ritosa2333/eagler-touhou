#include "TexturePlatform.hpp"
namespace th10 {
namespace {
constexpr u32 formats[]={0,21,25,23,20,26},sizes[]={4,4,2,2,3,2};
u32 checked_format(i32 format){if(format<0||format>=6)__builtin_trap();return format;}
u32 display_format(i32 format,const TexturePlatform& platform){auto index=checked_format(format);if(*platform.display_flags&1){if(formats[index]==0||formats[index]==21)index=5;else if(formats[index]==20)index=3;}return index;}
template<class T>T read(const u8* bytes){T value;__builtin_memcpy(&value,bytes,sizeof(value));return value;}
}
// 0x447050 does not apply the optional 16-bit display substitution and ignores
// the platform creation result. The encoded/embedded paths do check creation.
i32 AnmTexture::create_empty(i32 width,i32 height,i32 format,TexturePlatform& env){const auto index=checked_format(format);env.create_surface_texture(*this,width,height,formats[index]);bytes_per_pixel=sizes[index];return 0;}
// 0x446eb0.
i32 AnmTexture::create_encoded(i32 width,i32 height,i32 format,u32 key,TexturePlatform& env){const auto index=display_format(format,env);if(env.decode_source_texture(*this,width,height,formats[index],key))return -1;repair_edges(env);bytes_per_pixel=sizes[index];return 0;}
// 0x446f40. Signed dimensions in the embedded THTX header determine the source
// rectangle and row pitch. Upload errors are intentionally not propagated.
i32 AnmTexture::create_embedded(const u8* data,i32 width,i32 height,i32 format,TexturePlatform& env){
    const auto index=display_format(format,env),source_format=checked_format(read<std::int16_t>(data+6));
    const i32 source_width=read<std::int16_t>(data+8),source_height=read<std::int16_t>(data+10);
    if(env.create_surface_texture(*this,width,height,formats[index]))return -1;auto* surface=env.get_surface(handle);
    const TextureRect rectangle{0,0,source_width,source_height};env.upload_surface(surface,data+16,formats[source_format],static_cast<i32>(static_cast<u32>(source_width)*sizes[source_format]),rectangle);
    bytes_per_pixel=sizes[index];if(surface)env.release_surface(surface);return 0;
}
// 0x4465b0. All formats take the same acquire/describe/lock/unlock/release path.
void AnmTexture::repair_edges(TexturePlatform& env){auto* surface=env.get_surface(handle);const auto description=env.describe_surface(surface);const auto lock=env.lock_surface(surface);PixelSurface view{description.format,description.width,description.height,lock.pitch,lock.pixels};view.repair_edges();env.unlock_surface(surface);env.release_surface(surface);}
// Surface padding differs from the tightly packed temporary font bitmap.
// A4 colors here retain their full value, while the font path halves them.
void PixelSurface::repair_edges() noexcept {
    if(format!=0&&format!=21&&format!=25&&format!=26)return;
    const bool wide=format==0||format==21;const i32 size=wide?4:2,stride=(pitch/size)*size;const u32 mask=format==25?31:15,shift=format==25?5:4,alpha=format==25?0x8000:0xf000;
    for(u32 y=0;y<height;++y){auto* pixel=pixels+static_cast<i32>(y*static_cast<u32>(pitch));for(u32 x=0;x<width;++x,pixel+=size){
        if(wide?pixel[3]!=0:(read<u16>(pixel)&alpha)!=0)continue;u32 red=0,green=0,blue=0,count=0;
        auto sample=[&](const u8* source){if(wide){if(!source[3])return;red+=source[2];green+=source[1];blue+=source[0];}else{const auto value=read<u16>(source);if(!(value&alpha))return;red+=(value>>(shift*2))&mask;green+=(value>>shift)&mask;blue+=value&mask;}++count;};
        if(x)sample(pixel-size);if(x<width-1)sample(pixel+size);if(y)sample(pixel-stride);if(y<height-1)sample(pixel+stride);
        if(count>1){red/=count;green/=count;blue/=count;}
        if(wide){pixel[2]=red;pixel[1]=green;pixel[0]=blue;}else{const u16 value=(red<<(shift*2))|(green<<shift)|blue;__builtin_memcpy(pixel,&value,2);}
    }}
}
}
