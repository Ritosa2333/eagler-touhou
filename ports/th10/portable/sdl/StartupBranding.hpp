// Generated from eagler-common/browser/StartupBranding.hpp.
#pragma once
#include "Renderer.hpp"
#include <algorithm>

// The shell supplies only a transparent credit layer, never retail pixels.
namespace touhou::sdl::startup_branding {
inline std::vector<u8> load() {
    size_t size=0;auto* data=static_cast<u8*>(SDL_LoadFile("/eagler-startup.rgba",&size));
    std::vector<u8> pixels;
    if(data&&size==1280u*960u*4u){pixels.assign(data,data+size);for(size_t i=0;i<size;i+=4)std::swap(pixels[i],pixels[i+2]);}
    SDL_free(data);return pixels;
}
inline void draw(Renderer& renderer,u32 texture,u32 target,u32 tint=0xffffffffu) {
    if(!texture||!(tint>>24))return;
    renderer.flush();const auto saved=renderer.state;
    auto& state=renderer.state;state.target=target;state.depth=0;
    state.viewport={0,0,640,480,0,1};state.texture=texture;
    state.layout=attributes(VertexLayout::ScreenColorUv);
    state.pipeline=PipelineState{};auto& p=state.pipeline;
    p.depthWrite=false;p.blend=true;p.sourceBlend=BlendFactor::SourceAlpha;p.destinationBlend=BlendFactor::InverseSourceAlpha;
    p.color.operation=p.alpha.operation=ColorOperation::Multiply;
    p.color.second=p.alpha.second={ArgumentSource::Diffuse};p.addressU=p.addressV=Address::Clamp;
    struct Vertex{float x,y,z,w;u32 color;float u,v;};
    const u32 color=tint;
    const Vertex quad[]={{0,0,0,1,color,0,0},{640,0,0,1,color,1,0},{0,480,0,1,color,0,1},{640,480,0,1,color,1,1}};
    renderer.draw(Topology::Strip,2,quad,sizeof(Vertex));renderer.flush();renderer.state=saved;
}
}
