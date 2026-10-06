#include "GraphicsDevice.hpp"
namespace th15::sdl {
touhou::sdl::Surface GraphicsDevice::resolve(void* owner,u32 id){
    auto& device=*static_cast<GraphicsDevice*>(owner);auto it=device.textures.find(id);if(it==device.textures.end())return {};
    auto& t=it->second;auto& image=t.image;return {id,image.width,image.height,image.format,image.pitch,image.pixels.data(),u32(image.pixels.size()),t.revision,t.renderScale};
}
bool GraphicsDevice::initialize(){
    using namespace touhou::graphics;
    for(u32 id:{screen,depth}){textures[id].renderScale=render_scale;auto& image=textures[id].image;image.width=640;image.height=480;image.pitch=640*(id==screen?4:2);image.format=id==screen?PixelFormat::Bgra8:PixelFormat::Depth16;if(id==screen)image.pixels.resize(image.pitch*image.height);}
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT,"#canvas");
    if(!backend.initialize()){error=backend.error();return false;}SDL_SetWindowTitle(SDL_GL_GetCurrentWindow(),"Touhou 15");SDL_SetWindowSize(SDL_GL_GetCurrentWindow(),640*render_scale,480*render_scale);backend.state.target=screen;backend.state.depth=depth;configure_game(1);return true;
}
bool GraphicsDevice::preload(AnmResource& file,bool low_color){
    if(resources.find(&file)!=resources.end())return true;
    std::vector<u32> handles;handles.reserve(file.textures.size());
    for(const auto& source:file.textures){Texture texture;texture.renderScale=source.kind==AnmTexture::Kind::RenderTarget?render_scale:1;
        if(!texture.image.load(source,low_color)){error="Unable to prepare ANM texture: "+source.name;for(const auto id:handles){backend.release(id);textures.erase(id);}return false;}
        const auto id=next_handle++;textures.emplace(id,std::move(texture));handles.push_back(id);backend.prepare(id);
    }
    resources.emplace(&file,std::move(handles));return true;
}
void GraphicsDevice::unload(const AnmResource& file){
    presentation.reset();
    auto it=resources.find(&file);if(it==resources.end())return;backend.flush();for(u32 id:it->second){backend.release(id);textures.erase(id);}resources.erase(it);
}
u32 GraphicsDevice::texture(const AnmResource& file,u32 index){
    const auto it=resources.find(&file);if(it==resources.end()||index>=it->second.size()){error="ANM texture was not preloaded";return 0;}return it->second[index];
}
bool GraphicsDevice::select_target(const AnmResource* file,u32 index){
    const u32 handle=file?texture(*file,index):screen;if(!handle)return false;
    backend.flush();backend.state.target=handle;
    // The original target switch resets the device viewport to the surface.
    // CPU projection still uses the active scene camera until its next update.
    const auto& image=textures.at(handle).image;
    backend.viewport({0,0,image.width,image.height,0,1});return true;
}
bool GraphicsDevice::clear_target(u32 color,const GraphicsViewport* rect){
    if(rect){const i32 box[]={i32(rect->x),i32(rect->y),i32(rect->x+rect->width),i32(rect->y+rect->height)};presentation.clear(backend.state,3,color,box);backend.clear(3,color,1,0,box,1);}
    else {presentation.clear(backend.state,3,color,nullptr);backend.clear(3,color,1,0);}
    return true;
}
bool GraphicsDevice::clear_depth(const GraphicsViewport* rect){
    if(rect){const i32 box[]={i32(rect->x),i32(rect->y),i32(rect->x+rect->width),i32(rect->y+rect->height)};presentation.clear(backend.state,2,0,box);backend.clear(2,0,1,0,box,1);}
    else {presentation.clear(backend.state,2,0,nullptr);backend.clear(2,0,1,0);}
    return true;
}
bool GraphicsDevice::copy_surface(u32 source,const i32* region,u32 target,const i32* point){
    presentation.reset();
    if(!textures.count(source)||!textures.count(target)||source==target){error="Invalid GPU surface copy";return false;}
    // CPU font atlases already contain authored glyph pixels. Promote only
    // GPU capture destinations, avoiding fourfold atlas storage on mobile.
    auto& capture=textures.at(target);if(capture.renderScale!=render_scale){backend.release(target);capture.renderScale=render_scale;}
    backend.copy(source,region,target,point);changed(target);return true;
}
bool GraphicsDevice::resample_surface(u32 source,const i32* region,u32 target,const i32* destination){
    presentation.reset();
    if(!textures.count(source)||!textures.count(target)||source==target){error="Invalid GPU surface resize";return false;}
    auto& capture=textures.at(target);if(capture.renderScale!=render_scale){backend.release(target);capture.renderScale=render_scale;}
    if(!backend.resample(source,region,target,destination,nullptr,0,0)){error="GPU surface resize failed";return false;}
    changed(target);return true;
}
}
