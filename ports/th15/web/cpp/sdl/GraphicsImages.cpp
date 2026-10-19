#include "GraphicsDevice.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#define STBI_MAX_DIMENSIONS 4096
#include "../../../portable/sdl/third_party/stb_image.h"
#include <algorithm>
namespace th15::sdl {
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
