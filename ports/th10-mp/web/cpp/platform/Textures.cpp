#include "Textures.hpp"
namespace th10::browser {
namespace {
u32 address(const void* value){return static_cast<u32>(reinterpret_cast<uintptr_t>(value));}
struct Format {u32 bytes,red,green,blue,alpha,red_shift,green_shift,blue_shift,alpha_shift,unused;};
Format format(u32 value){switch(value){
    case 20:return {3,255,255,255,0,16,8,0,0,0};
    case 21:return {4,255,255,255,255,16,8,0,24,0};
    case 22:return {4,255,255,255,0,16,8,0,0,0};
    case 23:return {2,31,63,31,0,11,5,0,0,0};
    case 24:return {2,31,31,31,0,10,5,0,0,0};
    case 25:return {2,31,31,31,1,10,5,0,15,0};
    case 26:return {2,15,15,15,15,8,4,0,12,0};
    default:return {};
}}
u32 quantize(u32 pixel,u32 mask,u32 shift,u32 destination){if(!mask)return destination;return (((pixel>>shift)&mask)*destination+mask/2)/mask;}
}
i32 PixelCopy::copy(PixelSurface& output,const TextureRect& target,const u8* source,u32 source_format,i32 source_pitch,const TextureRect& region) noexcept {
    const auto in=format(source_format),out=format(output.format);constexpr i32 invalid=static_cast<i32>(0x8876086cu);
    if(!in.bytes||!out.bytes||!source||!output.pixels||target.left<0||target.top<0||target.right<=target.left||target.bottom<=target.top||static_cast<u32>(target.right)>output.width||static_cast<u32>(target.bottom)>output.height||region.left<0||region.top<0||region.right<=region.left||region.bottom<=region.top||region.right-region.left!=target.right-target.left||region.bottom-region.top!=target.bottom-target.top||source_pitch<=0||output.pitch<=0||static_cast<u64>(region.right)*in.bytes>static_cast<u32>(source_pitch)||static_cast<u64>(target.right)*out.bytes>static_cast<u32>(output.pitch))return invalid;
    const u32 width=target.right-target.left,height=target.bottom-target.top;
    for(u32 y=0;y<height;++y){const auto* row=source+(region.top+y)*source_pitch+region.left*in.bytes;auto* to=output.pixels+(target.top+y)*output.pitch+target.left*out.bytes;
        if(source_format==output.format){std::memmove(to,row,width*in.bytes);continue;}
        for(u32 x=0;x<width;++x){u32 pixel=0;std::memcpy(&pixel,row+x*in.bytes,in.bytes);
            const u32 value=out.unused|(quantize(pixel,in.red,in.red_shift,out.red)<<out.red_shift)|(quantize(pixel,in.green,in.green_shift,out.green)<<out.green_shift)|(quantize(pixel,in.blue,in.blue_shift,out.blue)<<out.blue_shift)|(quantize(pixel,in.alpha,in.alpha_shift,out.alpha)<<out.alpha_shift);
            std::memcpy(to+x*out.bytes,&value,out.bytes);
        }
    }
    return 0;
}
i32 Textures::create_surface_texture(AnmTexture& texture,u32 width,u32 height,u32 pixel_format){
    if(!width||!height||width>4096||height>4096)return static_cast<i32>(0x8876086cu);
    if(pixel_format==0)pixel_format=21;if(!format(pixel_format).bytes)return static_cast<i32>(0x8876086cu);
    return device.create_texture(width,height,pixel_format,texture.handle);
}
i32 Textures::decode_source_texture(AnmTexture&,u32,u32,u32,u32){
    // Every supplied ANM stores THTX pixels or declares an empty texture.
    // Fail explicitly for external encoded images until that API is supported.
    return static_cast<i32>(0x88760b59u);
}
void* Textures::get_surface(void* texture){return device.texture_surface(texture);}
TextureDescription Textures::describe_surface(void* surface){return device.describe_surface(surface);}
TextureLock Textures::lock_surface(void* surface){return device.map_surface(surface);}
void Textures::unlock_surface(void* surface){device.unmap_surface(surface);}
void Textures::release_surface(void* surface){device.release_resource(surface);}
i32 Textures::upload_surface(void* surface,const u8* bytes,u32 pixel_format,i32 pitch,const TextureRect& rectangle){const auto desc=describe_surface(surface);const auto lock=lock_surface(surface);PixelSurface output{desc.format,desc.width,desc.height,lock.pitch,lock.pixels};const auto result=PixelCopy::copy(output,rectangle,bytes,pixel_format,pitch,rectangle);unlock_surface(surface);return result;}
}
