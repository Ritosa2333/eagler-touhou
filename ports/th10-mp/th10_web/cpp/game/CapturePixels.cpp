#include "CapturePixels.hpp"
namespace th10 {
static TextureRect bounds(CaptureRectangle r){return {r.left,r.top,wrapping_add(r.left,r.width),wrapping_add(r.top,r.height)};}
bool CapturePixels::restore_texture(i32 slot){
    auto& env=environment;auto& buffer=buffers();if(buffer.textures[slot])return true;
    if(env.render_target(*env.device,size(slot).width,size(slot).height,*env.format,&buffer.textures[slot])!=0
       &&env.offscreen_surface(*env.device,size(slot).width,size(slot).height,*env.format,&buffer.textures[slot])!=0)return false;
    return env.copy_surface(buffer.textures[slot],nullptr,buffer.surfaces[slot],nullptr,1)==0;
}
void CapturePixels::restore(i32 slot,i32 left,i32 top,CapturePoint destination){
    auto& env=environment;if(!buffers().surfaces[slot])return;void* back=nullptr;if(env.back_buffer(*env.device,&back)!=0)return;
    if(restore_texture(slot)){const TextureRect rectangle{left,top,size(slot).width,size(slot).height};env.update_surface(*env.device,buffers().textures[slot],rectangle,back,destination);}
    env.release_surface(back);
}
void CapturePixels::restore_rectangle(i32 slot,CaptureRectangle source,CapturePoint destination){
    auto& env=environment;if(!buffers().surfaces[slot])return;void* back=nullptr;if(env.back_buffer(*env.device,&back)!=0)return;
    if(restore_texture(slot)){const auto rectangle=bounds(source);env.update_surface(*env.device,buffers().textures[slot],rectangle,back,destination);}
    env.release_surface(back);
}
void CapturePixels::capture_texture(i32 file,i32 texture,CaptureRectangle source,CaptureRectangle destination){
    auto& env=environment;if(!manager.files[file]->textures[texture].handle)return;env.flush(manager);void* back=nullptr;if(env.back_buffer(*env.device,&back)!=0)return;
    void* surface=nullptr;if(env.texture_surface(manager.files[file]->textures[texture].handle,&surface)==0){const auto from=bounds(source),to=bounds(destination);env.copy_surface(surface,&to,back,&from,2);env.release_surface(surface);}env.release_surface(back);
}
void CapturePixels::copy_texture(i32 destination_file,i32 destination_texture,i32 source_file,i32 source_texture,const TextureRect* destination,const TextureRect* source){
    auto& env=environment;if(!manager.files[destination_file]->textures[destination_texture].handle||!manager.files[source_file]->textures[source_texture].handle)return;env.flush(manager);
    void* to=nullptr;if(env.texture_surface(manager.files[destination_file]->textures[destination_texture].handle,&to)!=0)return;
    void* from=nullptr;if(env.texture_surface(manager.files[source_file]->textures[source_texture].handle,&from)!=0){env.release_surface(to);return;}
    env.copy_surface(to,destination,from,source,0xffffffff);env.release_surface(to);env.release_surface(from);
}
void CapturePixels::capture_screen(i32 slot,CaptureRectangle source,CaptureRectangle destination){
    auto& env=environment;env.flush(manager);auto& buffer=buffers();if(buffer.textures[slot])buffer.release(slot,env);
    const auto from=bounds(source),to=bounds(destination);void* back=nullptr;if(env.back_buffer(*env.device,&back)!=0)return;
    size(slot).width=destination.width;size(slot).height=destination.height;
    if((env.render_target(*env.device,destination.width,destination.height,*env.format,&buffer.textures[slot])==0
        ||env.offscreen_surface(*env.device,size(slot).width,size(slot).height,*env.format,&buffer.textures[slot])==0)
       &&env.offscreen_surface(*env.device,size(slot).width,size(slot).height,*env.format,&buffer.surfaces[slot])==0
       &&env.copy_surface(buffer.textures[slot],&to,back,&from,0xffffffff)==0)
        env.copy_surface(buffer.surfaces[slot],nullptr,buffer.textures[slot],nullptr,0xffffffff);
    if(back)env.release_surface(back);
}
}
