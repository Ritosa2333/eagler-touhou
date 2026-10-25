#include "AnmMaterial.hpp"
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <random>
using namespace touhou::graphics;
unsigned long long checks=0,matches=0;
PipelineState old_material(PipelineState p,unsigned flags,unsigned render){
 const unsigned mode=flags>>5&15;p.alphaTest=true;
 switch(mode){
 case 0:p.sourceBlend=BlendFactor::SourceAlpha;p.destinationBlend=BlendFactor::InverseSourceAlpha;p.blendEquation=BlendEquation::Add;break;
 case 1:p.sourceBlend=BlendFactor::SourceAlpha;p.destinationBlend=BlendFactor::One;p.blendEquation=BlendEquation::Add;break;
 case 2:p.sourceBlend=BlendFactor::SourceAlpha;p.destinationBlend=BlendFactor::One;p.blendEquation=BlendEquation::ReverseSubtract;break;
 case 3:p.alphaTest=false;p.sourceBlend=BlendFactor::One;p.destinationBlend=BlendFactor::Zero;p.blendEquation=BlendEquation::Add;break;
 case 4:p.sourceBlend=BlendFactor::InverseDestinationColor;p.destinationBlend=BlendFactor::InverseSourceColor;p.blendEquation=BlendEquation::Add;break;
 case 5:p.sourceBlend=BlendFactor::DestinationColor;p.destinationBlend=BlendFactor::Zero;p.blendEquation=BlendEquation::Add;break;
 case 6:p.sourceBlend=BlendFactor::InverseSourceColor;p.destinationBlend=BlendFactor::InverseSourceAlpha;p.blendEquation=BlendEquation::Add;break;
 case 7:p.sourceBlend=BlendFactor::DestinationAlpha;p.destinationBlend=BlendFactor::InverseDestinationAlpha;p.blendEquation=BlendEquation::Add;break;
 case 8:p.sourceBlend=BlendFactor::SourceAlpha;p.destinationBlend=BlendFactor::One;p.blendEquation=BlendEquation::Minimum;break;
 case 9:p.sourceBlend=BlendFactor::SourceAlpha;p.destinationBlend=BlendFactor::One;p.blendEquation=BlendEquation::Maximum;break;
 default:break;
 }
 p.minFilter=p.magFilter=render&0x800?Filter::Nearest:Filter::Linear;const unsigned u=render&3,v=flags>>30;
 if(u<3)p.addressU=u==0?Address::Repeat:u==1?Address::Clamp:Address::Mirror;if(v<3)p.addressV=v==0?Address::Repeat:v==1?Address::Clamp:Address::Mirror;
 p.textureTransform=false;p.color.operation=p.alpha.operation=ColorOperation::Multiply;
 p.color.first=p.alpha.first={ArgumentSource::Texture};p.color.second=p.alpha.second={ArgumentSource::Diffuse};return p;
}
void check(const PipelineState& p,unsigned flags,unsigned render){
 const bool expected=p.compatible(old_material(p,flags,render)),actual=th15::anm_material_matches(p,flags,render);
 ++checks;matches+=actual;if(expected!=actual){std::fprintf(stderr,"FAIL material predicate flags=%x render=%x expected=%d actual=%d\n",flags,render,expected,actual);std::exit(1);}
}
int main(){
 std::mt19937 rng(0x735139);
 for(unsigned mode=0;mode<16;++mode)for(unsigned u=0;u<4;++u)for(unsigned v=0;v<4;++v)for(unsigned filter=0;filter<2;++filter){
  const unsigned flags=(mode<<5)|(v<<30),render=u|(filter<<11);PipelineState p;
  // Controlled and intentionally untouched values, including non-finite fog.
  p=old_material(p,flags,render);check(p,flags,render);
  for(unsigned mutate=0;mutate<32;++mutate){auto q=p;
   switch(mutate){
    case 0:q.alphaTest=!q.alphaTest;break;case 1:q.textureTransform=!q.textureTransform;break;
    case 2:q.minFilter=Filter(unsigned(q.minFilter)^1);break;case 3:q.magFilter=Filter(unsigned(q.magFilter)^1);break;
    case 4:q.addressU=Address((unsigned(q.addressU)+1)%3);break;case 5:q.addressV=Address((unsigned(q.addressV)+1)%3);break;
    case 6:q.sourceBlend=BlendFactor((unsigned(q.sourceBlend)+1)%11);break;case 7:q.destinationBlend=BlendFactor((unsigned(q.destinationBlend)+1)%11);break;
    case 8:q.blendEquation=BlendEquation((unsigned(q.blendEquation)+1)%5);break;
    case 9:q.color.operation=ColorOperation::First;break;case 10:q.alpha.operation=ColorOperation::First;break;
    case 11:q.color.first.source=ArgumentSource::Specular;break;case 12:q.alpha.first.source=ArgumentSource::Specular;break;
    case 13:q.color.second.source=ArgumentSource::Specular;break;case 14:q.alpha.second.source=ArgumentSource::Specular;break;
    case 15:q.color.first.complement=true;break;case 16:q.color.first.alphaOnly=true;break;
    case 17:q.alpha.first.complement=true;break;case 18:q.alpha.first.alphaOnly=true;break;
    case 19:q.color.second.complement=true;break;case 20:q.color.second.alphaOnly=true;break;
    case 21:q.alpha.second.complement=true;break;case 22:q.alpha.second.alphaOnly=true;break;
    case 23:q.fogNear=std::numeric_limits<float>::quiet_NaN();break;
    case 24:q.fogFar=std::numeric_limits<float>::quiet_NaN();break;
    case 25:q.fogDensity=std::numeric_limits<float>::quiet_NaN();break;
    case 26:q.fogNear=std::numeric_limits<float>::infinity();break;
    case 27:q.fogFar=-std::numeric_limits<float>::infinity();break;
    case 28:q.fogDensity=-0.f;break;case 29:q.colorMask=3;break;
    case 30:q.textureFactor=0x12345678;break;case 31:q.depthTest=!q.depthTest;q.separateAlphaBlend=!q.separateAlphaBlend;break;
   }check(q,flags,render);
  }
 }
 for(unsigned n=0;n<250000;++n){
  unsigned flags=rng(),render=rng();PipelineState p=old_material({},flags,render);
  if(n&1){p.sourceBlend=BlendFactor(rng()%16);p.destinationBlend=BlendFactor(rng()%16);p.blendEquation=BlendEquation(rng()%8);}
  p.fogNear=float(rng()%1000);p.fogFar=float(rng()%1000);p.fogDensity=float(rng()%1000);
  if(n%7==0)p.alpha.first.alphaOnly=true;if(n%11==0)p.textureTransform=true;
  check(p,flags,render);
 }
 std::printf("ANM_MATERIAL_CONTRACT {\"checks\":%llu,\"matches\":%llu,\"reference\":\"former-material-pipeline-update\"}\n",checks,matches);
}
