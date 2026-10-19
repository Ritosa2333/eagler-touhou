#include "FontResources.hpp"
namespace th10 {
void RasterImage::initialize() noexcept {format=0xffffffff;width=height=0;device_context=previous_bitmap=bitmap_handle=0;pixels=nullptr;}
bool RasterImage::release(RasterEnvironment& env){
    if(!device_context)return false;
    env.select_object(device_context,previous_bitmap);env.delete_context(device_context);env.delete_object(bitmap_handle);initialize();return true;
}
bool RasterImage::create(i32 requested_width,i32 requested_height,u32 requested_format,RasterEnvironment& env){
    release(env);BitmapDescription description{};
    const auto* pixel_format=env.formats;while(pixel_format->format!=0xffffffff&&pixel_format->format!=requested_format)++pixel_format;
    if(requested_format==0xffffffff)return false;
    // Integer divisions truncate toward zero; the original multiplication wraps.
    const i32 row_bits=static_cast<i32>(pixel_format->bits*static_cast<u32>(requested_width));
    const i32 row_bytes=static_cast<i32>(static_cast<u32>(row_bits/8)+3);
    const i32 stride=(row_bytes/4)*4;
    description.size=sizeof(description);description.width=requested_width;description.height=static_cast<i32>(~static_cast<u32>(requested_height));description.planes=1;description.bits=pixel_format->bits;description.byte_size=static_cast<u32>(stride)*static_cast<u32>(requested_height);
    if(requested_format!=24&&requested_format!=22){description.compression=3;description.red=pixel_format->red;description.green=pixel_format->green;description.blue=pixel_format->blue;description.alpha=pixel_format->alpha;}
    u8* storage=nullptr;const auto bitmap=env.create_bitmap(description,&storage);if(!bitmap)return false;
    std::memset(storage,0,description.byte_size);const auto context=env.create_context();const auto previous=env.select_object(context,bitmap);
    previous_bitmap=previous;pitch=stride;pixels=storage;byte_size=static_cast<i32>(description.byte_size);device_context=context;bitmap_handle=bitmap;width=requested_width;height=requested_height;format=requested_format;return true;
}
bool RasterImage::create_compatible(i32 requested_width,i32 requested_height,u32 requested_format,RasterEnvironment& env){
    if(create(requested_width,requested_height,requested_format,env))return true;
    if(requested_format==25||requested_format==26)return create(requested_width,requested_height,21,env);
    if(requested_format==23)return create(requested_width,requested_height,22,env);
    return false;
}
void FontResources::initialize(FontEnvironment& env){
    if(!bitmap.create(1024,64,26,env))bitmap.create(1024,64,21,env);
    for(auto& value:bitmap.resource_fields)value=static_cast<u8>(random.next_word()>>9);
    for(u32 index=0;index<15;++index)fonts[14-index]=env.create_font(32+index*2,env.face,env.charset);
}
void FontResources::release(FontEnvironment& env){bitmap.release(env);for(u32 index=0;index<15;++index)env.delete_object(fonts[14-index]);}
}
