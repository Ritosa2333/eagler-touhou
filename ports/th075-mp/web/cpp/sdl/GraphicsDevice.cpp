#include "GraphicsDevice.hpp"
namespace th075::sdl {
static u32 power_of_two(u32 value){u32 n=1;while(n<value)n*=2;return n;}
touhou::sdl::Surface GraphicsDevice::resolve(void* owner,u32 id){
    const auto found=static_cast<GraphicsDevice*>(owner)->images.find(id);if(found==static_cast<GraphicsDevice*>(owner)->images.end())return {};
    auto& image=found->second;return {id,image.size.width,image.size.height,image.format,image.pitch,image.bytes.data(),u32(image.bytes.size()),image.revision,image.scale};
}
bool GraphicsDevice::initialize(u32 scale){
    if(scale<1||scale>2){error="Unsupported render scale";return false;}
    for(const u32 id:{screen,depthBuffer}){auto& image=images[id];image.size={640,480};image.pitch=640*(id==screen?4:2);image.format=id==screen?PixelFormat::Bgra8:PixelFormat::Depth16;image.scale=scale;if(id==screen)image.bytes.resize(image.pitch*480);}
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT,"#canvas");
    if(!renderer.initialize()){error=renderer.error();return false;}
    SDL_SetWindowTitle(SDL_GL_GetCurrentWindow(),"Touhou 07.5");SDL_SetWindowSize(SDL_GL_GetCurrentWindow(),640*scale,480*scale);renderer.state.target=screen;renderer.state.depth=depthBuffer;
    renderer.state.layout=attributes(VertexLayout::ScreenColorUv);renderer.state.stride=sizeof(ScreenVertex);
    auto& p=pipeline();p.blend=true;p.alphaTest=true;p.alphaReference=1;p.alphaCompare=Compare::GreaterEqual;
    p.sourceBlend=BlendFactor::SourceAlpha;p.destinationBlend=BlendFactor::InverseSourceAlpha;p.depthTest=false;p.depthWrite=true;
    p.color={ColorOperation::Multiply,{ArgumentSource::Texture},{ArgumentSource::Diffuse}};p.alpha={ColorOperation::Multiply,{ArgumentSource::Texture},{ArgumentSource::Diffuse}};
    p.minFilter=p.magFilter=Filter::Nearest;p.addressU=p.addressV=Address::Repeat;return true;
}
u32 GraphicsDevice::upload(const BitmapData& source){
    flush_vertices();Image image;image.size={power_of_two(source.width),power_of_two(source.height)};image.format=source.depth>=24?PixelFormat::Bgra8:PixelFormat::Argb1555;
    image.pitch=image.size.width*source.pixel_bytes();
    if(!source.decode(image.bytes,image.size.width)){error="Unable to decode original texture";return 0;}
    image.bytes.resize(std::size_t(image.pitch)*image.size.height,0);
    const u32 handle=nextHandle++;images.emplace(handle,std::move(image));renderer.prepare(handle);return handle;
}
u32 GraphicsDevice::create_surface(u32 width,u32 height,u32 scale){if(!width||!height||width>4096||height>4096)return 0;flush_vertices();Image image;image.size={width,height};image.scale=scale;image.pitch=width*4;image.bytes.resize(std::size_t(image.pitch)*height);const auto handle=nextHandle++;images.emplace(handle,std::move(image));renderer.prepare(handle);return handle;}
bool GraphicsDevice::update_pixels(u32 handle,const void* data,u32 bytes){const auto it=images.find(handle);if(it==images.end()||!data||bytes!=it->second.bytes.size())return false;flush_vertices();renderer.flush();auto& image=it->second;std::memcpy(image.bytes.data(),data,bytes);++image.revision;renderer.prepare(handle);return true;}
u32 GraphicsDevice::snapshot(){flush_vertices();const auto handle=create_surface(640,480,images.at(screen).scale);const i32 rect[4]{0,0,640,480},point[2]{0,0};renderer.copy(screen,rect,handle,point);return handle;}
bool GraphicsDevice::preload(const TexturePack& pack,std::vector<u32>& handles){
    std::vector<u32> added;added.reserve(pack.images.size());for(const auto& image:pack.images){const u32 handle=upload(image);if(!handle){unload(added);return false;}added.push_back(handle);}handles=std::move(added);return true;
}
void GraphicsDevice::unload(const std::vector<u32>& handles){flush_vertices();renderer.flush();for(const auto handle:handles){if(handle==screen||handle==depthBuffer)continue;renderer.release(handle);images.erase(handle);}}
TextureSize GraphicsDevice::texture_size(u32 handle){const auto found=images.find(handle);return found==images.end()?TextureSize{}:found->second.size;}
void GraphicsDevice::flush_vertices(){
    if(vertices.empty())return;const auto next=renderer.state;renderer.state=batchState;
    renderer.draw_batch(u32(vertices.size()/3),vertices.data(),sizeof(ScreenVertex));
    renderer.state=next;vertices.clear();
}
void GraphicsDevice::draw(u32 texture,Topology topology,const ScreenVertex* source,u32 count){
    if(suppressed||count<3)return;renderer.state.texture=texture;renderer.state.layout=attributes(VertexLayout::ScreenColorUv);renderer.state.stride=sizeof(ScreenVertex);
    const auto& state=renderer.state;
    const bool same=batchState.texture==texture&&batchState.target==state.target&&batchState.depth==state.depth&&batchState.layout==state.layout&&batchState.pipeline.compatible(state.pipeline)&&batchState.matrix==state.matrix&&std::memcmp(&batchState.viewport,&state.viewport,sizeof(state.viewport))==0;
    if(!vertices.empty()&&!same)flush_vertices();if(vertices.empty())batchState=renderer.state;
    if(topology==Topology::Triangles)vertices.insert(vertices.end(),source,source+count);
    else for(u32 i=0;i<count-2;i++){
        const u32 a=topology==Topology::Fan?0:(i&1)?i+1:i,b=topology==Topology::Fan?i+1:(i&1)?i:i+1;
        vertices.push_back(source[a]);vertices.push_back(source[b]);vertices.push_back(source[i+2]);
    }
    if(vertices.size()*sizeof(ScreenVertex)>=1048576)flush_vertices();
}
}
