#include "Captures.hpp"
#include "../game/Presentation.hpp"
#include <cstdlib>
namespace th10::browser {
namespace {u32 address(const void* p){return static_cast<u32>(reinterpret_cast<uintptr_t>(p));}}
Captures::Captures(AnimationEngine& owner):engine(owner),graphics(owner.device),textures(graphics,texture_flags),native_device(reinterpret_cast<void*>(static_cast<uintptr_t>(graphics.handle))){device=&native_device;format=&surface_format;void* back=nullptr;if(back_buffer(native_device,&back)==0){surface_format=textures.describe_surface(back).format;release_surface(back);}}
Captures::~Captures(){for(i32 slot=0;slot<32;++slot)release_slot(slot);}
CapturePixels Captures::pixels(){return {engine.manager,*this};}
void Captures::release_slot(i32 slot){pixels().buffers().release(slot,*this);}
void Captures::process(){auto& request=*reinterpret_cast<CaptureRequests*>(engine.manager.header);if(request.texture.target_file>=0){const auto r=request.texture;pixels().capture_texture(r.target_file,r.flags,r.source,r.destination);request.texture.target_file=-1;}if(request.texture.reserved_000>=0){pixels().capture_screen(request.texture.reserved_000,request.screen_source,request.screen_destination);request.texture.reserved_000=-1;}}
void Captures::flush(AnmManager&){engine.flush();}
i32 Captures::back_buffer(void*,void** output){*output=graphics.back_surface();return 0;}
i32 Captures::texture_surface(void* texture,void** output){*output=graphics.texture_surface(texture);return 0;}
i32 Captures::render_target(void*,i32 width,i32 height,u32 format,void** output){return graphics.create_surface(width,height,format,0,true,*output);}
i32 Captures::offscreen_surface(void*,i32 width,i32 height,u32 format,void** output){return graphics.create_surface(width,height,format,2,false,*output);}
i32 Captures::copy_surface(void* destination,const TextureRect* target,void* source,const TextureRect* region,u32 filter){
    // Preserve the device's exact-copy path before falling back to CPU pixels.
    if(graphics.copy_surface(source,region,destination,target,filter==2?1u:filter==1?0u:2u)==0)return 0;
    const auto src=textures.describe_surface(source),dst=textures.describe_surface(destination);const auto from=region?*region:TextureRect{0,0,static_cast<i32>(src.width),static_cast<i32>(src.height)},to=target?*target:TextureRect{0,0,static_cast<i32>(dst.width),static_cast<i32>(dst.height)};
    auto in=textures.lock_surface(source),out=textures.lock_surface(destination);PixelSurface input{src.format,src.width,src.height,in.pitch,in.pixels},output{dst.format,dst.width,dst.height,out.pitch,out.pixels};i32 result;
    if(filter==1)result=PixelCopy::copy(output,to,input.pixels,input.format,input.pitch,from);
    else if(filter==2)result=TextureResample::point(output,to,input,from);
    else result=TextureResample::triangle(output,to,input,from,true,true,filter==0xffffffff||(filter&0x80000));
    textures.unlock_surface(destination);textures.unlock_surface(source);return result;
}
void Captures::update_surface(void*,void* source,const TextureRect& region,void* destination,const CapturePoint& point){graphics.update_surface(source,region,destination,point.x,point.y);}
void Captures::release_surface(void* surface){graphics.release_resource(surface);}
void Captures::free_pixels(void* bytes){std::free(bytes);}
}
