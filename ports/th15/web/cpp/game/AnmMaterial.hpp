#pragma once
#include "../../../portable/sdl/GraphicsState.hpp"
namespace th15 {
// material() only changes this subset of the pipeline. The common 2D path
// already has these exact values; avoid copying and re-comparing the complete
// state while retaining the former update/flush path for every difference.
inline bool anm_material_matches(const touhou::graphics::PipelineState& p,
                                 std::uint32_t flags,std::uint32_t render_flags)noexcept{
 using namespace touhou::graphics;
 const auto mode=(flags>>5)&15u;
 if(p.alphaTest!=(mode!=3)||p.textureTransform)return false;
 const auto filter=render_flags&0x800?Filter::Nearest:Filter::Linear;
 if(p.minFilter!=filter||p.magFilter!=filter)return false;
 const auto u=render_flags&3u,v=flags>>30;
 if(u<3&&p.addressU!=(u==0?Address::Repeat:u==1?Address::Clamp:Address::Mirror))return false;
 if(v<3&&p.addressV!=(v==0?Address::Repeat:v==1?Address::Clamp:Address::Mirror))return false;
 const Argument texture{ArgumentSource::Texture},diffuse{ArgumentSource::Diffuse};
 if(p.color.operation!=ColorOperation::Multiply||p.alpha.operation!=ColorOperation::Multiply||
    !(p.color.first==texture)||!(p.alpha.first==texture)||
    !(p.color.second==diffuse)||!(p.alpha.second==diffuse))return false;
 // compatible() deliberately treats NaN as unequal even in an unchanged
 // field. Keep its flush behavior for malformed fog input, including NaN.
 if(!(p.fogNear==p.fogNear&&p.fogFar==p.fogFar&&p.fogDensity==p.fogDensity))return false;
 if(mode<10){
  constexpr BlendFactor source[]={BlendFactor::SourceAlpha,BlendFactor::SourceAlpha,
   BlendFactor::SourceAlpha,BlendFactor::One,BlendFactor::InverseDestinationColor,
   BlendFactor::DestinationColor,BlendFactor::InverseSourceColor,BlendFactor::DestinationAlpha,
   BlendFactor::SourceAlpha,BlendFactor::SourceAlpha};
  constexpr BlendFactor destination[]={BlendFactor::InverseSourceAlpha,BlendFactor::One,
   BlendFactor::One,BlendFactor::Zero,BlendFactor::InverseSourceColor,BlendFactor::Zero,
   BlendFactor::InverseSourceAlpha,BlendFactor::InverseDestinationAlpha,BlendFactor::One,BlendFactor::One};
  const auto equation=mode==2?BlendEquation::ReverseSubtract:mode==8?BlendEquation::Minimum:
                      mode==9?BlendEquation::Maximum:BlendEquation::Add;
  if(p.sourceBlend!=source[mode]||p.destinationBlend!=destination[mode]||p.blendEquation!=equation)return false;
 }
 return true;
}
}
