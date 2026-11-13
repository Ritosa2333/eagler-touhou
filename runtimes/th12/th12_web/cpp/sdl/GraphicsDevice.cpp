#include "GraphicsDevice.hpp"
#if defined(TH_ENABLE_THCRAP) || PORT_TITLE == 14
#include <SDL3/SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "../../../portable/sdl/third_party/stb_image.h"
#include <algorithm>
#include <cmath>
#endif
namespace th11::sdl {
#if PORT_TITLE == 14
bool GraphicsDevice::manual_image(AnmResource& file,u32 index,const u8* input,u32 size){
    if(index>=file.textures.size()||!input||size>32u*1024u*1024u){error="Invalid TH14 manual image";return false;}
    int width=0,height=0,channels=0;auto* decoded=stbi_load_from_memory(input,int(size),&width,&height,&channels,4);
    if(!decoded||width<=0||height<=0||width>16384||height>16384){stbi_image_free(decoded);error="TH14 manual PNG decode failed";return false;}
    // 47c8b0 uses D3DXLoadSurfaceFromFileInMemory with POINT filtering.
    // The shipped 1024-square pages fill the already allocated help surface.
    const u32 handle=texture(file,index);if(!handle){stbi_image_free(decoded);return false;}
    AnmTexture page=file.textures[index];page.kind=AnmTexture::Kind::Embedded;
    page.width=u16(textures.at(handle).image.width);page.height=u16(textures.at(handle).image.height);
    page.half_resolution=false;
    page.pixel_width=page.width;page.pixel_height=page.height;page.pixel_format=1;page.pixels.resize(size_t(page.width)*page.height*4);
    for(u32 y=0;y<page.height;++y)for(u32 x=0;x<page.width;++x){
        const u32 sx=std::min(u32(width-1),u32((u64(2*x+1)*width)/(2*page.width))),sy=std::min(u32(height-1),u32((u64(2*y+1)*height)/(2*page.height)));
        const auto* src=decoded+(size_t(sy)*width+sx)*4;auto* dest=page.pixels.data()+(size_t(y)*page.width+x)*4;
        dest[0]=src[2];dest[1]=src[1];dest[2]=src[0];dest[3]=src[3];
    }
    stbi_image_free(decoded);TextureImage next;if(!next.load(page)){error=next.failure;return false;}
    backend.flush();textures.at(handle).image=std::move(next);changed(handle);return true;
}
#endif
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
#if PORT_TITLE == 14
bool GraphicsDevice::initialize(u32 width,u32 height){
#else
bool GraphicsDevice::initialize(){
    constexpr u32 width=640,height=480;
#endif
    using namespace touhou::graphics;
    for(u32 id:{screen,depth}){auto& image=textures[id].image;image.width=width;image.height=height;image.pitch=width*(id==screen?4:2);image.format=id==screen?PixelFormat::Bgra8:PixelFormat::Depth16;if(id==screen)image.pixels.resize(image.pitch*image.height);}
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT,"#canvas");
    if(!backend.initialize()){error=backend.error();return false;}
#if PORT_TITLE == 14
    if(width!=640||height!=480)SDL_SetWindowSize(backend.native_window(),int(width),int(height));
#endif
    SDL_SetWindowTitle(backend.native_window(),kPortTitle==16?"东方天空璋":kPortTitle==14?"东方辉针城 · 移植验证":"Touhou 11");backend.state.target=screen;backend.state.depth=depth;initialize_game_pipeline();return true;
}
bool GraphicsDevice::prepare_texture(AnmResource& file,u32 index,u32& handle,bool low_color){
#if PORT_TITLE == 14
    auto& source=file.textures[index];
    if(source.kind==AnmTexture::Kind::RenderTarget){
        // Retail47d4b1..47d4dc replaces the chunk dimensions before
        //47d9e0 derives sprite UVs from the resulting surface descriptor.
        const auto& display=textures.at(screen).image;
        if(source.width!=display.width||source.height!=display.height){
            source.width=u16(display.width);source.height=u16(display.height);
            for(auto& sprite:file.sprites)if(sprite.texture==index){
                sprite.u0=float(double(sprite.x)/source.width);sprite.v0=float(double(sprite.y)/source.height);
                const float right=float(double(sprite.x)+sprite.width),bottom=float(double(sprite.y)+sprite.height);
                sprite.u1=float(double(right)/source.width);sprite.v1=float(double(bottom)/source.height);
            }
        }
    }
#else
    const auto& source=file.textures[index];
#endif
    Texture texture;
#if defined(TH_ENABLE_THCRAP)
    AnmTexture override_texture;
    const AnmTexture& selected=patched_texture(file,source,index,override_texture)?override_texture:source;
#else
    const AnmTexture& selected=source;
#endif
    const bool loaded=texture.image.load(selected,low_color
#if PORT_TITLE == 14
            ,texture_high_precision&&*texture_high_precision
#endif
    );
    if(!loaded){error="Unable to prepare ANM texture: "+source.name+" ("+texture.image.failure+")";return false;}
    handle=next_handle++;textures.emplace(handle,std::move(texture));backend.prepare(handle);return true;
}
bool GraphicsDevice::preload(AnmResource& file,bool low_color){
    auto found=resources.find(&file);
    if(found!=resources.end()&&found->second.size()==file.textures.size())return true;
    std::vector<u32> handles;
    if(found!=resources.end()){handles=std::move(found->second);resources.erase(found);}
    handles.reserve(file.textures.size());
    for(u32 index=u32(handles.size());index<file.textures.size();++index){
        u32 handle=0;
        if(!prepare_texture(file,index,handle,low_color)){
            backend.flush();for(const auto id:handles){backend.release(id);textures.erase(id);}return false;
        }
        handles.push_back(handle);
    }
    resources.emplace(&file,std::move(handles));return true;
}
#if PORT_TITLE == 14
bool GraphicsDevice::preload_chunk(AnmResource& file,u32 index,bool low_color){
    auto& handles=resources[&file];
    if(file.textures.empty()&&index==0)return true;
    if(index>=file.textures.size()||index>handles.size()){
        error="TH14 ANM texture chunks requested out of order";return false;
    }
    if(index<handles.size())return true;
    u32 handle=0;
    if(!prepare_texture(file,index,handle,low_color))return false;
    handles.push_back(handle);return true;
}
#endif
void GraphicsDevice::unload(const AnmResource& file){
    auto it=resources.find(&file);if(it==resources.end())return;backend.flush();for(u32 id:it->second){backend.release(id);textures.erase(id);}resources.erase(it);
}
u32 GraphicsDevice::texture(const AnmResource& file,u32 index){
    const auto it=resources.find(&file);if(it==resources.end()||index>=it->second.size()){error="ANM texture was not preloaded";return 0;}
    return it->second[index];
}
bool GraphicsDevice::select_target(const AnmResource* file,u32 index){
    const u32 handle=file?texture(*file,index):screen;if(!handle)return false;
    backend.flush();backend.state.target=handle;
    // The original target switch resets the device viewport to the surface.
    // CPU projection still uses the active scene camera until its next update.
    const auto& image=textures.at(handle).image;
    backend.viewport({0,0,image.width,image.height,0,1});return true;
}
bool GraphicsDevice::clear_target(u32 color,const GraphicsViewport* rect,u32 flags){
    if(rect){const i32 box[]={i32(rect->x),i32(rect->y),i32(rect->x+rect->width),i32(rect->y+rect->height)};backend.clear(flags,color,1,0,box,1);}
    else backend.clear(flags,color,1,0);
    return true;
}
}
