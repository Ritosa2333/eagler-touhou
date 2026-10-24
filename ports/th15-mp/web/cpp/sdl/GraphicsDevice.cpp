#include "GraphicsDevice.hpp"
#include "../../../portable/sdl/StartupBranding.hpp"
#if defined(TH_ENABLE_THCRAP)
#include <SDL3/SDL.h>
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#define STBI_MAX_DIMENSIONS 4096
#include "../../../portable/sdl/third_party/stb_image.h"
#include <algorithm>
#include <cmath>
#endif
namespace th15::sdl {
#include "GraphicsLocalization.inc"
touhou::sdl::Surface GraphicsDevice::resolve(void* owner,u32 id){TH15_SYSTEM_SCOPE;
    auto& device=*static_cast<GraphicsDevice*>(owner);auto it=device.textures.find(id);if(it==device.textures.end())return {};
    auto& t=it->second;auto& image=t.image;return {id,image.width,image.height,image.format,image.pitch,image.pixels.data(),u32(image.pixels.size()),t.revision,t.renderScale};
}
void GraphicsDevice::draw_startup_branding(u32 tint){TH15_SYSTEM_SCOPE;
    if(!startup_credit){auto pixels=touhou::sdl::startup_branding::load();if(pixels.empty())return;
        startup_credit=next_handle++;auto& image=textures[startup_credit].image;
        image.width=1280;image.height=960;image.pitch=1280*4;image.format=touhou::graphics::PixelFormat::Bgra8;image.pixels=std::move(pixels);}
    touhou::sdl::startup_branding::draw(backend,startup_credit,screen,tint);
}
void GraphicsDevice::release_startup_branding(){TH15_SYSTEM_SCOPE;
    if(startup_credit){backend.release(startup_credit);textures.erase(startup_credit);startup_credit=0;}
}
bool GraphicsDevice::initialize(){TH15_SYSTEM_SCOPE;
    using namespace touhou::graphics;
    for(u32 id:{screen,depth}){textures[id].renderScale=render_scale;auto& image=textures[id].image;image.width=640;image.height=480;image.pitch=640*(id==screen?4:2);image.format=id==screen?PixelFormat::Bgra8:PixelFormat::Depth16;if(id==screen)image.pixels.resize(image.pitch*image.height);}
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT,"#canvas");
    if(!backend.initialize()){error=backend.error();return false;}SDL_SetWindowTitle(SDL_GL_GetCurrentWindow(),"Touhou 15");SDL_SetWindowSize(SDL_GL_GetCurrentWindow(),640*render_scale,480*render_scale);backend.state.target=screen;backend.state.depth=depth;configure_game(1);return true;
}
bool GraphicsDevice::preload(AnmResource& file,bool low_color){
    TH15_SYSTEM_SCOPE;
    if(resources.find(&file)!=resources.end())return true;
    if(!rollback::RequireConfirmed())return false;
    std::vector<u32> handles;handles.reserve(file.textures.size());
    for(u32 index=0;index<file.textures.size();index++){const auto& source=file.textures[index];Texture texture;texture.render_target=source.kind==AnmTexture::Kind::RenderTarget;texture.renderScale=texture.render_target?render_scale:1;
#if defined(TH_ENABLE_THCRAP)
        AnmTexture override_texture;const auto& selected=patched_texture(file,source,index,override_texture)?override_texture:source;
#else
        const auto& selected=source;
#endif
        bool loaded=false;{TH15_GAME_SCOPE;loaded=texture.image.load(selected,low_color);}
        if(!loaded){error="Unable to prepare ANM texture: "+source.name;for(const auto id:handles){backend.release(id);textures.erase(id);}return false;}
        const auto id=next_handle++;textures.emplace(id,std::move(texture));handles.push_back(id);backend.prepare(id);
    }
    resources.emplace(&file,std::move(handles));return true;
}
void GraphicsDevice::unload(const AnmResource& file){
    TH15_SYSTEM_SCOPE;
    if(resources.find(&file)==resources.end()||!rollback::RequireConfirmed())return;
    if(presentation.retain_authored){
        // A resource can retire after Draw. Preserve that completed compositor
        // once at the confirmed barrier before its command handles disappear.
        std::vector<u32> targets{screen};for(const auto& entry:textures)if(entry.second.render_target)targets.push_back(entry.first);
        rollback_base=backend.save_surfaces(targets);
    }
    presentation.reset();presentation.clear_authored();
    auto it=resources.find(&file);if(it==resources.end())return;backend.flush();for(u32 id:it->second){backend.release(id);textures.erase(id);}resources.erase(it);
}
u32 GraphicsDevice::texture(const AnmResource& file,u32 index){TH15_SYSTEM_SCOPE;
    const auto it=resources.find(&file);if(it==resources.end()||index>=it->second.size()){error="ANM texture was not preloaded";return 0;}return it->second[index];
}
bool GraphicsDevice::select_target(const AnmResource* file,u32 index){TH15_SYSTEM_SCOPE;
    const u32 handle=file?texture(*file,index):screen;if(!handle)return false;
    backend.flush();backend.state.target=handle;
    // The original target switch resets the device viewport to the surface.
    // CPU projection still uses the active scene camera until its next update.
    const auto& image=textures.at(handle).image;
    backend.viewport({0,0,image.width,image.height,0,1});return true;
}
bool GraphicsDevice::clear_target(u32 color,const GraphicsViewport* rect){TH15_SYSTEM_SCOPE;
    if(rect){const i32 box[]={i32(rect->x),i32(rect->y),i32(rect->x+rect->width),i32(rect->y+rect->height)};presentation.clear(backend.state,3,color,box);backend.clear(3,color,1,0,box,1);}
    else {presentation.clear(backend.state,3,color,nullptr);backend.clear(3,color,1,0);}
    return true;
}
bool GraphicsDevice::clear_depth(const GraphicsViewport* rect){TH15_SYSTEM_SCOPE;
    if(rect){const i32 box[]={i32(rect->x),i32(rect->y),i32(rect->x+rect->width),i32(rect->y+rect->height)};presentation.clear(backend.state,2,0,box);backend.clear(2,0,1,0,box,1);}
    else {presentation.clear(backend.state,2,0,nullptr);backend.clear(2,0,1,0);}
    return true;
}
bool GraphicsDevice::copy_surface(u32 source,const i32* region,u32 target,const i32* point){
    TH15_SYSTEM_SCOPE;if(!rollback::RequireConfirmed())return false;
    presentation.reset();
    if(!textures.count(source)||!textures.count(target)||source==target){error="Invalid GPU surface copy";return false;}
    // CPU font atlases already contain authored glyph pixels. Promote only
    // GPU capture destinations, avoiding fourfold atlas storage on mobile.
    auto& capture=textures.at(target);if(capture.renderScale!=render_scale){backend.release(target);capture.renderScale=render_scale;}
    backend.copy(source,region,target,point);changed(target);
#if TH15_MULTIPLAYER && TH15_WRITE_JOURNAL
    // Captures are confirmed barriers. Retain their native pixels once so an
    // atlas invalidation after a later undo also preserves pause/results art.
    backend.read(target);
#endif
    return true;
}
bool GraphicsDevice::resample_surface(u32 source,const i32* region,u32 target,const i32* destination){
    TH15_SYSTEM_SCOPE;if(!rollback::RequireConfirmed())return false;
    presentation.reset();
    if(!textures.count(source)||!textures.count(target)||source==target){error="Invalid GPU surface resize";return false;}
    auto& capture=textures.at(target);if(capture.renderScale!=render_scale){backend.release(target);capture.renderScale=render_scale;}
    if(!backend.resample(source,region,target,destination,nullptr,0,0)){error="GPU surface resize failed";return false;}
    changed(target);
#if TH15_MULTIPLAYER && TH15_WRITE_JOURNAL
    backend.read(target);
#endif
    return true;
}
void GraphicsDevice::rollback_restore(bool restore_base){
    TH15_SYSTEM_SCOPE;backend.cancel_present();presentation.reset();
    // Edited CPU images are journaled with gameplay. Refresh their device cache
    // after undo, including an image first edited in the discarded prediction.
    // Never-edited ANM textures retain the same bytes and GPU upload across undo.
    for(auto& entry:textures){auto& texture=entry.second;
#if TH15_MULTIPLAYER && TH15_WRITE_JOURNAL
        if(texture.cpu_mutated&&!texture.render_target&&!texture.image.pixels.empty()&&Netplay::WriteJournal::OwnedByGameHeap(texture.image.pixels.data())){
            backend.changed(entry.first,texture.revision,{});++texture.revision;
        }
#endif
    }
    if(restore_base)backend.restore_surfaces(rollback_base);
    error.clear();
}
}
