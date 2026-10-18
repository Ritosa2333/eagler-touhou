#include "GraphicsDevice.hpp"
#include "../../../portable/sdl/StartupBranding.hpp"
#if defined(TH_ENABLE_THCRAP)
#include <SDL3/SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "../../../portable/sdl/third_party/stb_image.h"
#include <algorithm>
#include <cmath>
#endif
namespace th11::sdl {
#if defined(TH_ENABLE_THCRAP)
namespace {
// TH10 AnimationResources::override_embedded uses thcrap's per-sprite "auto"
// replacement rule: opaque originals are blended with the PNG, while sprites
// with transparency are replaced (including transparent pixels). Keep all
// unmentioned atlas regions from the retail THTX bitmap.
bool patched_texture(const AnmResource& file,const AnmTexture& source,u32 index,AnmTexture& result){
    if(source.kind!=AnmTexture::Kind::Embedded||source.name.empty()||
       source.name.front()=='/'||source.name.find("..")!=std::string::npos||
       source.name.find(':')!=std::string::npos)return false;
    std::string name=source.name;
    for(char& c:name)if(c=='\\')c='/';
    // The original ASCII glyph atlas has fixed codepoint slots; translating
    // its PNG directly would invalidate those indices.
    if(name=="ascii/ascii.png")return false;
    const std::string path="/thcrap/th11/"+name;
    size_t bytes=0;auto* input=static_cast<u8*>(SDL_LoadFile(path.c_str(),&bytes));
    if(!input)return false;
    int width=0,height=0,channels=0;
    auto* image=bytes<=32u*1024u*1024u?stbi_load_from_memory(input,int(bytes),&width,&height,&channels,4):nullptr;
    SDL_free(input);
    if(!image||width<=0||height<=0||width>16384||height>16384){stbi_image_free(image);return false;}
    std::vector<u8> original;
    if(!source.rgba(original)){stbi_image_free(image);return false;}
    const u32 atlas_width=source.pixel_width,atlas_height=source.pixel_height;
    bool changed=false;
    for(const auto& sprite:file.sprites){
        if(sprite.texture!=index)continue;
        const i32 left=i32(std::lround(sprite.x)),top=i32(std::lround(sprite.y));
        const i32 sw=i32(std::lround(sprite.width)),sh=i32(std::lround(sprite.height));
        if(left<0||top<0||sw<=0||sh<=0||left>=width||top>=height||
           left>=i32(atlas_width)||top>=i32(atlas_height))continue;
        const i32 w=std::min({sw,width-left,i32(atlas_width)-left});
        const i32 h=std::min({sh,height-top,i32(atlas_height)-top});
        bool empty=true,opaque=true;
        for(i32 y=0;y<h;++y)for(i32 x=0;x<w;++x){
            const auto* patch=image+(size_t(top+y)*width+left+x)*4;
            const auto* base=original.data()+(size_t(top+y)*atlas_width+left+x)*4;
            if(patch[3])empty=false;
            if(base[3]!=255)opaque=false;
        }
        if(empty)continue;
        for(i32 y=0;y<h;++y)for(i32 x=0;x<w;++x){
            const auto* patch=image+(size_t(top+y)*width+left+x)*4;
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
}
#endif
touhou::sdl::Surface GraphicsDevice::resolve(void* owner,u32 id){
    auto& device=*static_cast<GraphicsDevice*>(owner);auto it=device.textures.find(id);if(it==device.textures.end())return {};
    auto& t=it->second;auto& image=t.image;return {id,image.width,image.height,image.format,image.pitch,image.pixels.data(),u32(image.pixels.size()),t.revision};
}
void GraphicsDevice::draw_startup_branding(u32 tint){
    if(!startup_credit){auto pixels=touhou::sdl::startup_branding::load();if(pixels.empty())return;
        startup_credit=next_handle++;auto& image=textures[startup_credit].image;
        image.width=1280;image.height=960;image.pitch=1280*4;image.format=touhou::graphics::PixelFormat::Bgra8;image.pixels=std::move(pixels);}
    touhou::sdl::startup_branding::draw(backend,startup_credit,screen,tint);
}
void GraphicsDevice::release_startup_branding(){
    if(startup_credit){backend.release(startup_credit);textures.erase(startup_credit);startup_credit=0;}
}
bool GraphicsDevice::initialize(){
    using namespace touhou::graphics;
    for(u32 id:{screen,depth}){auto& image=textures[id].image;image.width=640;image.height=480;image.pitch=640*(id==screen?4:2);image.format=id==screen?PixelFormat::Bgra8:PixelFormat::Depth16;if(id==screen)image.pixels.resize(image.pitch*image.height);}
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT,"#canvas");
    if(!backend.initialize()){error=backend.error();return false;}SDL_SetWindowTitle(SDL_GL_GetCurrentWindow(),"Touhou 11");backend.state.target=screen;backend.state.depth=depth;initialize_game_pipeline();return true;
}
bool GraphicsDevice::preload(AnmResource& file,bool low_color){
    if(resources.find(&file)!=resources.end())return true;
    std::vector<u32> handles;handles.reserve(file.textures.size());
    for(u32 index=0;index<file.textures.size();++index){const auto& source=file.textures[index];Texture texture;
#if defined(TH_ENABLE_THCRAP)
        AnmTexture override_texture;
        const AnmTexture& selected=patched_texture(file,source,index,override_texture)?override_texture:source;
#else
        const AnmTexture& selected=source;
#endif
        if(!texture.image.load(selected,low_color)){error="Unable to prepare ANM texture: "+source.name;for(const auto id:handles){backend.release(id);textures.erase(id);}return false;}
        const auto id=next_handle++;textures.emplace(id,std::move(texture));handles.push_back(id);backend.prepare(id);
    }
    resources.emplace(&file,std::move(handles));return true;
}
void GraphicsDevice::unload(const AnmResource& file){
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
    if(rect){const i32 box[]={i32(rect->x),i32(rect->y),i32(rect->x+rect->width),i32(rect->y+rect->height)};backend.clear(3,color,1,0,box,1);}
    else backend.clear(3,color,1,0);
    return true;
}
}
