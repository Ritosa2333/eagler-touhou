#include "AnmRenderer.hpp"
#include "AnmOverlay.hpp"
namespace th15 {
using namespace touhou::graphics;
int AnmRenderer::overlay(AnmVm& vm){
 auto& effect=*vm.geometry.overlay;const bool masking=effect.mode==2||(effect.mode==0&&vm.environment&&vm.environment->render_target_has_alpha);
 if(masking){flush();auto& state=graphics.pipeline();state.alphaTest=false;state.blend=true;state.sourceBlend=BlendFactor::Zero;state.destinationBlend=BlendFactor::One;state.blendEquation=BlendEquation::Add;state.separateAlphaBlend=true;state.sourceAlphaBlend=BlendFactor::One;state.destinationAlphaBlend=BlendFactor::Zero;state.alphaBlendEquation=BlendEquation::Add;state.color.operation=state.alpha.operation=ColorOperation::First;state.color.first=state.alpha.first={ArgumentSource::Diffuse};state.textureTransform=false;
  const float x=effect.mode==2?128.f:0,y=effect.mode==2?16.f:0,right=effect.mode==2?512.f:float(vm.environment->screen_width),bottom=effect.mode==2?464.f:float(vm.environment->screen_height);
  const ColorVertex rectangle[]={{{x,y,0},1,0},{{right,y,0},1,0},{{x,bottom,0},1,0},{{right,bottom,0},1,0}};graphics.set_layout(VertexLayout::ScreenColor);graphics.primitives(Topology::Strip,2,rectangle,sizeof(ColorVertex));
  state.alphaTest=true;state.color.operation=state.alpha.operation=ColorOperation::Multiply;state.color.first=state.alpha.first={ArgumentSource::Texture};state.color.second=state.alpha.second={ArgumentSource::Diffuse};state.sourceAlphaBlend=BlendFactor::SourceAlpha;state.destinationAlphaBlend=BlendFactor::One;state.alphaBlendEquation=BlendEquation::Add;
 }
 for(u32 i=0;i<4;i++)if(draw(*effect.panels[i])==-2)return -2;
 if(masking){flush();graphics.pipeline().separateAlphaBlend=false;if(draw(*effect.panels[4])==-2)return -2;flush();}return 0;
}
}
