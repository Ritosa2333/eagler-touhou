#pragma once
#ifdef TH_NATIVE_PLATFORM
#error Legacy graphics translation is for comparison fixtures, not the production game
#endif
#include "GraphicsState.hpp"
#include <cstring>
namespace touhou::graphics::legacy {
inline float floating(u32 value){float f;std::memcpy(&f,&value,4);return f;}
inline MatrixKind matrix(u32 value){return value==256?MatrixKind::World:value==2?MatrixKind::View:value==3?MatrixKind::Projection:MatrixKind::Texture;}
inline Topology topology(u32 value){return Topology(value-1);}
inline VertexAttributes vertices(u32 f){VertexAttributes v;v.screen=(f&14)==4;u32 offset=v.screen?16:12;if(f&16)offset+=12;if(f&32)offset+=4;if(f&64){v.diffuse=offset;offset+=4;}if(f&128){v.specular=offset;offset+=4;}v.uv=(f&0xf00)?offset:VertexAttributes::absent;return v;}
inline Argument argument(u32 v){return {((v&15)==2)?ArgumentSource::Texture:((v&15)==3)?ArgumentSource::Factor:((v&15)==4)?ArgumentSource::Specular:ArgumentSource::Diffuse,bool(v&16),bool(v&32)};}
inline void render(PipelineState& p,u32 key,u32 v){
    switch(key){
    case 7:p.depthTest=v;break;case 14:p.depthWrite=v;break;case 15:p.alphaTest=v;break;
    case 19:p.sourceBlend=BlendFactor(v-1);break;case 20:p.destinationBlend=BlendFactor(v-1);break;
    case 22:p.cull=Cull(v-1);break;case 23:p.depthCompare=Compare(v-1);break;case 24:p.alphaReference=v;break;
    case 25:p.alphaCompare=Compare(v-1);break;case 26:p.dither=v;break;case 27:p.blend=v;break;case 28:p.fog=v;break;
    case 34:p.fogColor=v;break;case 36:p.fogNear=floating(v);break;case 37:p.fogFar=floating(v);break;case 38:p.fogDensity=floating(v);break;
    case 48:p.rangeFog=v;break;case 60:p.textureFactor=v;break;case 140:p.fogMode=FogMode(v);break;case 168:p.colorMask=v;break;case 171:p.blendEquation=BlendEquation(v-1);break;
    }
}
inline void stage(PipelineState& p,u32 key,u32 v){
    switch(key){
    case 1:p.color.operation=ColorOperation(v-1);break;case 2:p.color.first=argument(v);break;case 3:p.color.second=argument(v);break;
    case 4:p.alpha.operation=ColorOperation(v-1);break;case 5:p.alpha.first=argument(v);break;case 6:p.alpha.second=argument(v);break;
    case 13:p.addressU=v==2?Address::Mirror:v>=3?Address::Clamp:Address::Repeat;break;
    case 14:p.addressV=v==2?Address::Mirror:v>=3?Address::Clamp:Address::Repeat;break;
    case 16:p.magFilter=v==1?Filter::Nearest:Filter::Linear;break;case 17:p.minFilter=v==1?Filter::Nearest:Filter::Linear;break;case 24:p.textureTransform=v;break;
    }
}
}
