#include "GraphicsDevice.hpp"
namespace th17::sdl {
touhou::sdl::Surface GraphicsDevice::resolve(void* owner,u32 id){
    auto& device=*static_cast<GraphicsDevice*>(owner);auto it=device.textures.find(id);if(it==device.textures.end())return {};
    auto& t=it->second;auto& image=t.image;return {id,image.width,image.height,image.format,image.pitch,image.pixels.data(),u32(image.pixels.size()),t.revision,t.renderScale};
}
bool GraphicsDevice::initialize(){
    using namespace touhou::graphics;
    backend.staged_uploads=true;
    backend.additive_batches=true;
    backend.lazy_depth_clears=true;
    for(u32 id:{screen,depth}){textures[id].renderScale=render_scale;auto& image=textures[id].image;image.width=640;image.height=480;image.pitch=640*(id==screen?4:2);image.format=id==screen?PixelFormat::Bgra8:PixelFormat::Depth16;if(id==screen)image.pixels.resize(image.pitch*image.height);}
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT,"#canvas");
    if(!backend.initialize()){error=backend.error();return false;}SDL_SetWindowTitle(SDL_GL_GetCurrentWindow(),"Touhou 17");SDL_SetWindowSize(SDL_GL_GetCurrentWindow(),640*render_scale,480*render_scale);backend.state.target=screen;backend.state.depth=depth;configure_game(1);return true;
}
bool GraphicsDevice::preload(AnmResource& file,bool low_color,const AnmResource* detailed){
    if(resources.find(&file)!=resources.end())return true;
    if(detailed){
        // The caller normalizes detailed glyph rectangles to logical pixels
        // before binding templates. Check the actual atlas UVs before upload,
        // including the repacked small digits and the separate loading image.
        if(detailed->textures.size()!=file.textures.size()||detailed->sprites.size()!=file.sprites.size()){error="Detailed atlas layout mismatch";return false;}
        for(u32 i=0;i<file.sprites.size();i++){const auto& a=file.sprites[i];const auto& b=detailed->sprites[i];if(a.texture!=b.texture||a.u0!=b.u0||a.v0!=b.v0||a.u1!=b.u1||a.v1!=b.v1){error="Detailed atlas glyph UV mismatch";return false;}}
        for(u32 i=0;i<file.textures.size();i++)if(file.textures[i].kind!=AnmTexture::Kind::Embedded||detailed->textures[i].kind!=AnmTexture::Kind::Embedded){error="Detailed atlas must contain immutable images";return false;}
    }
    // Bounds were computed from the original embedded image. A replacement
    // atlas must not inherit the old image's transparent-padding certificate.
    if(detailed)file.alpha_bounds.clear();
    std::vector<u32> handles;handles.reserve(file.textures.size());
    for(u32 index=0;index<file.textures.size();index++){const auto& source=detailed?detailed->textures[index]:file.textures[index];Texture texture;texture.resource=&file;texture.renderScale=source.kind==AnmTexture::Kind::RenderTarget?render_scale:1;
        if(!texture.image.load(source,low_color)){error="Unable to prepare ANM texture: "+source.name;for(const auto id:handles){backend.release(id);textures.erase(id);}return false;}
        const auto id=next_handle++;textures.emplace(id,std::move(texture));handles.push_back(id);backend.prepare(id);
    }
    resources.emplace(&file,std::move(handles));return true;
}
void GraphicsDevice::unload(const AnmResource& file){
    if(last_resource==&file){last_resource=nullptr;last_resource_handles=nullptr;}
    auto it=resources.find(&file);if(it==resources.end())return;backend.flush();for(u32 id:it->second){backend.release(id);textures.erase(id);}resources.erase(it);
}
u32 GraphicsDevice::texture(const AnmResource& file,u32 index){
    // Adjacent particles normally use the same bank. Its immutable handle
    // vector stays valid until unload; avoid a tree lookup for every sprite.
    if(last_resource!=&file){const auto it=resources.find(&file);if(it==resources.end()){error="ANM texture was not preloaded";return 0;}last_resource=&file;last_resource_handles=&it->second;}
    if(index>=last_resource_handles->size()){error="ANM texture was not preloaded";return 0;}return (*last_resource_handles)[index];
}
bool GraphicsDevice::select_target(const AnmResource* file,u32 index){
    const u32 handle=file?texture(*file,index):screen;if(!handle)return false;
    if(file)const_cast<AnmResource*>(file)->alpha_bounds.clear();
    backend.flush();backend.state.target=handle;
    // The original target switch resets the device viewport to the surface.
    // CPU projection still uses the active scene camera until its next update.
    const auto& image=textures.at(handle).image;
    backend.viewport({0,0,image.width,image.height,0,1});return true;
}
bool GraphicsDevice::clear_target(u32 color,const GraphicsViewport* rect){
    if(rect){const i32 box[]={i32(rect->x),i32(rect->y),i32(rect->x+rect->width),i32(rect->y+rect->height)};backend.clear(1,color,1,0,box,1);}
    else backend.clear(1,color,1,0);
    return true;
}
bool GraphicsDevice::clear_depth(const GraphicsViewport* rect){
    if(rect){const i32 box[]={i32(rect->x),i32(rect->y),i32(rect->x+rect->width),i32(rect->y+rect->height)};backend.clear(2,0,1,0,box,1);}
    else backend.clear(2,0,1,0);
    return true;
}
bool GraphicsDevice::copy_surface(u32 source,const i32* region,u32 target,const i32* point){
    if(!textures.count(source)||!textures.count(target)||source==target){error="Invalid GPU surface copy";return false;}
    backend.copy(source,region,target,point);changed(target);return true;
}
bool GraphicsDevice::resample_surface(u32 source,const i32* region,u32 target,const i32* destination){
    if(!textures.count(source)||!textures.count(target)||source==target){error="Invalid GPU surface resize";return false;}
    if(!backend.resample(source,region,target,destination,nullptr,0,0)){error="GPU surface resize failed";return false;}
    changed(target);return true;
}
}
