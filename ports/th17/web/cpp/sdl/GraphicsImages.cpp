#include "GraphicsDevice.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#define STBI_MAX_DIMENSIONS 4096
#include "../../../portable/sdl/third_party/stb_image.h"
#include <algorithm>
#include <cmath>
namespace th17::sdl {
#if TH17_MULTIPLAYER
bool GraphicsDevice::layout_room_front(TextureImage& image){
    if(image.width!=1024||image.height!=1024||image.format!=touhou::graphics::PixelFormat::Bgra8)return false;
    const auto pixel=[&](u32 x,u32 y){return image.pixels.data()+size_t(y)*image.pitch+x*4;};
    // The retail right frame and logo share one sprite. Continue its own
    // background into the logo area so the extra HUD rows stay legible.
    constexpr u32 panel_left=64,panel_width=448,logo_top=576,panel_bottom=960;
    for(u32 y=logo_top;y<panel_bottom;++y)
        std::memcpy(pixel(panel_left,y),pixel(panel_left,2*logo_top-1-y),panel_width*4);

    constexpr u32 graze_x=656,graze_y=36,graze_w=160,graze_h=36;
    for(u32 y=0;y<graze_h;++y)for(u32 x=0;x<graze_w;++x)
        if(pixel(graze_x+x,graze_y+y)[3])return true;

    // TH17 keeps the Graze script but ships its atlas rectangle transparent.
    // Fill it with the original TH15 label, below any localized replacement.
    static constexpr u8 graze_png[]={
#include "RoomGrazeTexture.inc"
    };
    int width=0,height=0;
    auto* rgba=stbi_load_from_memory(graze_png,sizeof(graze_png),&width,&height,nullptr,4);
    if(rgba&&width==144&&height==graze_h){
        for(int y=0;y<height;++y)for(int x=0;x<width;++x){
            const auto* from=rgba+(y*width+x)*4;
            auto* to=pixel(graze_x+x,graze_y+y);
            to[0]=from[2];to[1]=from[1];to[2]=from[0];to[3]=from[3];
        }
    }
    stbi_image_free(rgba);
    return true;
}
#endif
// TH10 AnimationResources::override_embedded uses thcrap's per-sprite "auto"
// replacement rule: opaque originals are blended with the PNG, while sprites
// with transparency are replaced (including transparent pixels). Keep all
// unmentioned atlas regions from the retail THTX bitmap.
bool GraphicsDevice::patch_texture(const AnmResource& file,const AnmTexture& source,u32 index,AnmTexture& result){
    if(source.kind!=AnmTexture::Kind::Embedded||source.name.empty()||
       source.name.front()=='/'||source.name.find("..")!=std::string::npos||
       source.name.find(':')!=std::string::npos)return false;
    std::string name=source.name;
    for(char& c:name)if(c=='\\')c='/';
    const std::string path="/thcrap/th17/"+name;
    size_t bytes=0;auto* input=static_cast<u8*>(SDL_LoadFile(path.c_str(),&bytes));
    if(!input)return false;
    int width=0,height=0,channels=0;
    auto* image=bytes<=32u*1024u*1024u?stbi_load_from_memory(input,int(bytes),&width,&height,&channels,4):nullptr;
    SDL_free(input);
    if(!image)return false;
    // These atlases share fixed glyph IDs with AsciiText. Accept a patch only
    // at the authored scale and apply it to the original sprite rectangles.
    const bool ascii=name=="ascii/ascii.png"||name=="ascii/ascii_1280.png";
    if(ascii&&(width!=source.pixel_width||height!=source.pixel_height)){stbi_image_free(image);return false;}
    std::vector<u8> original;
    if(!source.rgba(original)){stbi_image_free(image);return false;}
    const u32 atlas_width=source.pixel_width,atlas_height=source.pixel_height;
    // thcrap anm.cpp patches the complete entry for zero/one sprite. PNGs can
    // cover more than its lone displayed sprite (e.g. signature and logos).
    std::vector<AnmSprite> regions;
    for(const auto& sprite:file.sprites)if(sprite.texture==index)regions.push_back(sprite);
    if(regions.size()<=1){AnmSprite whole;whole.texture=index;whole.width=float(atlas_width);whole.height=float(atlas_height);regions={whole};}
    const auto authored=original;
    bool changed=false;
    for(const auto& sprite:regions){
        if(sprite.texture!=index)continue;
        const i32 left=i32(std::lround(sprite.x))%atlas_width,top=i32(std::lround(sprite.y))%atlas_height;
        const i32 patch_left=source.source_x+left,patch_top=source.source_y+top;
        const i32 sw=i32(std::lround(sprite.width)),sh=i32(std::lround(sprite.height));
        if(left<0||top<0||patch_left<0||patch_top<0||sw<=0||sh<=0||patch_left>=width||patch_top>=height||
           left>=i32(atlas_width)||top>=i32(atlas_height))continue;
        const i32 w=std::min({sw,width-patch_left,i32(atlas_width)-left});
        const i32 h=std::min({sh,height-patch_top,i32(atlas_height)-top});
        bool empty=true,opaque=true;
        for(i32 y=0;y<h;++y)for(i32 x=0;x<w;++x){
            const auto* patch=image+(size_t(patch_top+y)*width+patch_left+x)*4;
            const auto* base=authored.data()+(size_t(top+y)*atlas_width+left+x)*4;
            if(patch[3])empty=false;
            if(base[3]!=255)opaque=false;
        }
        if(empty)continue;
        for(i32 y=0;y<h;++y)for(i32 x=0;x<w;++x){
            const auto* patch=image+(size_t(patch_top+y)*width+patch_left+x)*4;
            auto* base=original.data()+(size_t(top+y)*atlas_width+left+x)*4;
            if(!opaque){std::copy(patch,patch+4,base);continue;}
            const i32 alpha=patch[3];if(!alpha)continue;
            for(int c=0;c<3;++c)base[c]=u8((base[c]*(255-alpha)+patch[c]*alpha)>>8);
            base[3]=alpha==255?u8(255):u8(std::min<i32>(base[3]+alpha,255));
        }
        changed=true;
    }
    stbi_image_free(image);
    if(!changed)return false;
    result=source;result.pixel_format=1;result.pixels.resize(original.size());
    for(size_t p=0;p<original.size();p+=4){
        result.pixels[p]=original[p+2];result.pixels[p+1]=original[p+1];
        result.pixels[p+2]=original[p];result.pixels[p+3]=original[p+3];
    }
    return true;
}
bool GraphicsDevice::clear_image(u32 handle){auto* target=pixels(handle);if(!target||target->pixels.empty()){error="Image upload surface unavailable";return false;}backend.flush();std::fill(target->pixels.begin(),target->pixels.end(),u8(0));changed(handle);return true;}
bool GraphicsDevice::upload_png(u32 handle,const u8* bytes,u32 size){auto* target=pixels(handle);if(!target||!bytes||!size||size>64*1024*1024){error="Invalid image upload";return false;}i32 width=0,height=0,channels=0;auto* rgba=stbi_load_from_memory(bytes,i32(size),&width,&height,&channels,4);if(!rgba){error=stbi_failure_reason();return false;}
 // BGRA and ARGB4444 image targets need only one final image allocation. Preserve
 // the original top-left crop and transparent padding without an intermediate
 // ANM pixel vector followed by another full TextureImage copy.
 if(target->format==touhou::graphics::PixelFormat::Bgra8||target->format==touhou::graphics::PixelFormat::Argb4444){
  TextureImage next;next.width=target->width;next.height=target->height;const u32 stride=target->format==touhou::graphics::PixelFormat::Bgra8?4:2;next.pitch=next.width*stride;next.packed_format=target->packed_format;next.format=target->format;next.pixels.resize(size_t(next.pitch)*next.height,0);
  const u32 w=std::min<u32>(width,next.width),h=std::min<u32>(height,next.height);
  for(u32 y=0;y<h;y++)for(u32 x=0;x<w;x++){const auto* from=rgba+(size_t(y)*width+x)*4;auto* to=next.pixels.data()+size_t(y)*next.pitch+x*stride;if(stride==4){to[0]=from[2];to[1]=from[1];to[2]=from[0];to[3]=from[3];}else{const u16 value=u16(((u32(from[2])*15+127)/255)|((u32(from[1])*15+127)/255)<<4|((u32(from[0])*15+127)/255)<<8|((u32(from[3])*15+127)/255)<<12);std::memcpy(to,&value,2);}}
  stbi_image_free(rgba);backend.flush();*target=std::move(next);changed(handle);return true;
 }
 // Original no-filter image uploads copy the top-left source region; oversized
 // manual PNGs are cropped to their 768x1024 atlas, rather than rescaled.
 AnmTexture image;image.width=target->width;image.height=target->height;image.format=target->packed_format;image.pixel_format=1;image.pixel_width=std::min<u32>(width,target->width);image.pixel_height=std::min<u32>(height,target->height);image.pixels.resize(size_t(image.pixel_width)*image.pixel_height*4);
 for(u32 y=0;y<image.pixel_height;y++)for(u32 x=0;x<image.pixel_width;x++){const u32 from=(y*u32(width)+x)*4,to=(y*u32(image.pixel_width)+x)*4;image.pixels[to]=rgba[from+2];image.pixels[to+1]=rgba[from+1];image.pixels[to+2]=rgba[from];image.pixels[to+3]=rgba[from+3];}stbi_image_free(rgba);TextureImage next;if(!next.load(image)){error="Unable to convert image upload";return false;}backend.flush();*target=std::move(next);changed(handle);return true;}
}
