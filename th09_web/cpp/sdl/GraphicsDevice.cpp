#include "GraphicsDevice.hpp"
#include "../game/ImageResample.hpp"
#include "../game/RuntimeOverride.hpp"
#include <algorithm>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STBI_NO_STDIO
#include "../../../portable/sdl/third_party/stb_image.h"
namespace th09::sdl {
namespace {
// Mirrors thcrap's ANM PNG auto mode (also used by the TH10 Web adapter):
// each non-empty patch sprite is blended over an opaque original sprite, or
// replaces a non-opaque sprite so transparent pixels erase its old contents.
void override_embedded(TextureImage& image,const AnmTextureSource& source){
#ifdef TH_ENABLE_THCRAP
    if(source.name.empty()||source.sprite_rects.empty()||
       (source.name.size()>=15&&source.name.compare(source.name.size()-15,15,"ascii/ascii.png")==0))return;
    std::vector<u8> file;
    if(!RuntimeOverride::Read(source.name.c_str(),file)){
        if(source.name!="data/title/title02.png"||
           !RuntimeOverride::Read("data/title/title02.v1.50a.png",file))return;
    }
    i32 width=0,height=0,channels=0;
    u8* patch=stbi_load_from_memory(file.data(),i32(file.size()),&width,&height,&channels,4);
    if(!patch)return;
    TextureImage output;output.width=image.width;output.height=image.height;
    output.pitch=output.width*4;output.format=PixelFormat::Bgra8;
    output.pixels.resize(size_t(output.pitch)*output.height);
    if(!ImageResample::triangle(output,{0,0,i32(output.width),i32(output.height)},image,
                                {0,0,i32(image.width),i32(image.height)})){
        stbi_image_free(patch);return;
    }
    bool changed=false;
    for(const auto& rect:source.sprite_rects){
        const i32 left=i32(std::lround(rect.x)),top=i32(std::lround(rect.y));
        const i32 spriteWidth=i32(std::lround(rect.width)),spriteHeight=i32(std::lround(rect.height));
        if(left<0||top<0||spriteWidth<=0||spriteHeight<=0)continue;
        const i32 copyWidth=std::min({spriteWidth,width-left,i32(output.width)-left});
        const i32 copyHeight=std::min({spriteHeight,height-top,i32(output.height)-top});
        if(copyWidth<=0||copyHeight<=0)continue;
        bool replacementEmpty=true,destinationOpaque=true;
        for(i32 y=top;y<top+copyHeight;++y)for(i32 x=left;x<left+copyWidth;++x){
            const u8* p=patch+(size_t(y)*width+x)*4;
            const u8* d=output.pixels.data()+size_t(y)*output.pitch+x*4;
            if(p[3])replacementEmpty=false;if(d[3]!=255)destinationOpaque=false;
        }
        if(replacementEmpty)continue;
        changed=true;
        for(i32 y=top;y<top+copyHeight;++y)for(i32 x=left;x<left+copyWidth;++x){
            const u8* p=patch+(size_t(y)*width+x)*4;
            u8* d=output.pixels.data()+size_t(y)*output.pitch+x*4;
            if(!destinationOpaque){d[0]=p[2];d[1]=p[1];d[2]=p[0];d[3]=p[3];continue;}
            const i32 alpha=p[3];if(!alpha)continue;const i32 weight=255-alpha;
            d[0]=u8((d[0]*weight+p[2]*alpha)>>8);
            d[1]=u8((d[1]*weight+p[1]*alpha)>>8);
            d[2]=u8((d[2]*weight+p[0]*alpha)>>8);
            d[3]=alpha==255?u8(255):u8(std::min<i32>(d[3]+alpha,255));
        }
    }
    stbi_image_free(patch);
    if(changed)image=std::move(output);
#else
    (void)image;(void)source;
#endif
}
}
touhou::sdl::Surface GraphicsDevice::resolve(void* opaque,u32 id){auto& device=*static_cast<GraphicsDevice*>(opaque);const auto it=device.textures.find(id);if(it==device.textures.end())return {};auto& t=it->second;auto& i=t.image;return {id,i.width,i.height,i.format,i.pitch,i.pixels.data(),u32(i.pixels.size()),t.revision};}
bool GraphicsDevice::initialize(){
    for(u32 id:{screen,depth}){auto& i=textures[id].image;i.width=640;i.height=480;i.format=id==screen?PixelFormat::Bgra8:PixelFormat::Depth16;i.pitch=640*(id==screen?4:2);if(id==screen)i.pixels.resize(i.pitch*i.height);}
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT,"#canvas");
    if(!backend.initialize()){error=backend.error();return false;}backend.state.target=screen;backend.state.depth=depth;return true;
}
TextureAllocation GraphicsDevice::create(const AnmTextureSource& source,const u8* bytes,u32 count){
    Texture t;if(!t.image.load(source,bytes,count)){error="Invalid ANM texture: "+source.name;return {};}
    override_embedded(t.image,source);
    const u32 id=next++,width=t.image.width,height=t.image.height;textures.emplace(id,std::move(t));backend.prepare(id);return {id,width,height};
}
TextureAllocation GraphicsDevice::image(const u8* data,u32 size){
    i32 width=0,height=0,channels=0;u8* raw=stbi_load_from_memory(data,i32(size),&width,&height,&channels,4);if(!raw){error=stbi_failure_reason();return {};}
    Texture t;auto& i=t.image;i.width=width;i.height=height;i.pitch=width*4;i.format=PixelFormat::Bgra8;i.pixels.resize(size_t(i.pitch)*height);
    for(size_t p=0;p<i.pixels.size();p+=4){i.pixels[p]=raw[p+2];i.pixels[p+1]=raw[p+1];i.pixels[p+2]=raw[p];i.pixels[p+3]=raw[p+3];}stbi_image_free(raw);
    const u32 id=next++;textures.emplace(id,std::move(t));backend.prepare(id);return {id,u32(width),u32(height)};
}
void GraphicsDevice::destroy(u32 id){if(id<3)return;backend.flush();backend.release(id);textures.erase(id);}
TextureImage* GraphicsDevice::pixels(u32 id){auto it=textures.find(id);return it==textures.end()?nullptr:&it->second.image;}
void GraphicsDevice::changed(u32 id){auto it=textures.find(id);if(it!=textures.end())++it->second.revision;}
void GraphicsDevice::viewport(const Viewport& v){backend.viewport({v.x,v.y,v.width,v.height,v.near_depth,v.far_depth});}
void GraphicsDevice::transform(MatrixKind kind,const Matrix4& value){backend.transform(kind,&value);}
void GraphicsDevice::clear(bool color,bool depth,u32 rgba,float z){backend.clear((color?1u:0u)|(depth?2u:0u),rgba,z,0);}
void GraphicsDevice::state(const PipelineState& p,u32 texture,VertexLayout layout){backend.state.pipeline=p;backend.state.texture=texture;backend.state.layout=attributes(layout);}
void GraphicsDevice::draw(const PipelineState& p,u32 texture,Topology primitive,VertexLayout layout,const void* data,u32 vertices){
    const u32 min=primitive==Topology::Points?1:primitive==Topology::Lines||primitive==Topology::LineStrip?2:3;if(vertices<min)return;
    const u32 count=primitive==Topology::Triangles?vertices/3:primitive==Topology::Lines?vertices/2:primitive==Topology::LineStrip?vertices-1:primitive==Topology::Strip||primitive==Topology::Fan?vertices-2:vertices;
    state(p,texture,layout);backend.draw(primitive,count,data,stride(layout));
}
void GraphicsDevice::triangles(const PipelineState& p,u32 texture,const SpriteVertex* data,u32 vertices){if(vertices<3)return;state(p,texture,VertexLayout::ScreenColorUv);backend.draw_batch(vertices/3,data,sizeof(SpriteVertex));}
void GraphicsDevice::present(){backend.present(screen);}
}
