#pragma once
namespace touhou::sdl {
inline constexpr const char* resampleVertex=R"GLSL(#version 300 es
void main(){vec2 p=vec2((gl_VertexID<<1)&2,gl_VertexID&2);gl_Position=vec4(p*2.0-1.0,0,1);}
)GLSL";
inline constexpr const char* resampleFragment=R"GLSL(#version 300 es
precision highp float;
precision highp int;
uniform sampler2D sourceImage,weights;
uniform ivec4 sourceRect,destinationRect;
uniform ivec4 sourceMask,destinationMask;
uniform bool triangle;
out vec4 outputColor;
// The original writer multiplies and adds under round-toward-zero. Retain
// that rounding using the positive f32 significand, without GPU double support.
// Positive normal f32 operations in integer arithmetic. Drivers may fuse
// native multiply/add even across local variables; captures require the
// original separately rounded operations, including on those GPUs.
float multiply(float a,float b){
 if(a==0.0||b==0.0)return 0.0;
 uint ab=floatBitsToUint(a),bb=floatBitsToUint(b),am=(ab&0x7fffffu)|0x800000u,bm=(bb&0x7fffffu)|0x800000u;
 uint lowProduct=(am&65535u)*(bm&65535u),cross=(am>>16)*(bm&65535u)+(am&65535u)*(bm>>16);
 uint low=lowProduct+(cross<<16),high=(am>>16)*(bm>>16)+(cross>>16)+uint(low<lowProduct);
 uint shift=(high&0x8000u)!=0u?24u:23u;
 uint mantissa=(low>>shift)|(high<<(32u-shift)),tail=low&((1u<<shift)-1u),midpoint=1u<<(shift-1u);
 mantissa+=uint(tail>midpoint||(tail==midpoint&&(mantissa&1u)!=0u));
 uint exponent=((ab>>23)&255u)+((bb>>23)&255u)-127u+shift-23u;
 if(mantissa==0x1000000u){mantissa>>=1;++exponent;}
 return uintBitsToFloat((exponent<<23)|(mantissa&0x7fffffu));
}
float add(float a,float b){
 if(a==0.0)return b;if(b==0.0)return a;
 uint ab=floatBitsToUint(a),bb=floatBitsToUint(b);if(ab<bb){uint t=ab;ab=bb;bb=t;}
 uint exponent=(ab>>23)&255u,delta=exponent-((bb>>23)&255u);
 if(delta>27u)return a>b?a:b;
 uint am=((ab&0x7fffffu)|0x800000u)<<3,bm=((bb&0x7fffffu)|0x800000u)<<3;
 if(delta!=0u)bm=(bm>>delta)|uint((bm&((1u<<delta)-1u))!=0u);
 uint sum=am+bm;if((sum&0x8000000u)!=0u){sum=(sum>>1)|(sum&1u);++exponent;}
 uint mantissa=sum>>3,tail=sum&7u;mantissa+=uint(tail>4u||(tail==4u&&(mantissa&1u)!=0u));
 if(mantissa==0x1000000u){mantissa>>=1;++exponent;}
 return uintBitsToFloat((exponent<<23)|(mantissa&0x7fffffu));
}
vec4 multiply(float a,vec4 b){return vec4(multiply(a,b.r),multiply(a,b.g),multiply(a,b.b),multiply(a,b.a));}
vec4 add(vec4 a,vec4 b){return vec4(add(a.r,b.r),add(a.g,b.g),add(a.b,b.b),add(a.a,b.a));}
uint channel(float value,uint mask){
 if(value<=0.0||mask==0u)return 0u;if(value>=1.0)return mask;
 uint bits=floatBitsToUint(value);int shift=150-int((bits>>23)&255u);
 uint product=((bits&0x7fffffu)|0x800000u)*mask;
 int discardBits=0;uint high=product>>24;while(high!=0u){++discardBits;high>>=1;}
 if(discardBits>0)product=(product>>discardBits)<<discardBits;
 if(shift>32)return 0u;
 if(shift==32)return uint(product>=0x80000000u);
 return (product>>shift)+uint((product&((1u<<shift)-1u))>=(1u<<(shift-1)));
}
float originalUnit(int mask){return uintBitsToFloat(mask==255?0x3b808081u:mask==31?0x3d042108u:mask==63?0x3c820821u:mask==15?0x3d888889u:0x3f800000u);}
vec4 originalPixel(ivec2 at){
 vec4 p=texelFetch(sourceImage,at,0);vec4 mask=vec4(sourceMask);
 // Restore original integer channels before the original normalization.
 vec4 integers=floor(p*mask+0.5);
 vec4 unit=vec4(originalUnit(sourceMask.r),originalUnit(sourceMask.g),originalUnit(sourceMask.b),originalUnit(sourceMask.a));
 return mix(vec4(1),vec4(multiply(integers.r,unit.r),multiply(integers.g,unit.g),multiply(integers.b,unit.b),multiply(integers.a,unit.a)),greaterThan(mask,vec4(0)));
}
void main(){
 ivec2 at=ivec2(gl_FragCoord.xy)-destinationRect.xy;
 vec4 value=vec4(0);
 if(triangle){
  int row=destinationRect.z+at.y;
  int nx=int(texelFetch(weights,ivec2(0,at.x),0).x),ny=int(texelFetch(weights,ivec2(0,row),0).x);
  for(int y=1;y<=ny;++y){vec2 wy=texelFetch(weights,ivec2(y,row),0).xy;
   for(int x=1;x<=nx;++x){vec2 wx=texelFetch(weights,ivec2(x,at.x),0).xy;
    float weight=multiply(wx.y,wy.y);
    vec4 contribution=multiply(weight,originalPixel(sourceRect.xy+ivec2(int(wx.x),int(wy.x))));
    value=add(contribution,value);
   }
  }
 }else{value=originalPixel(sourceRect.xy+at*sourceRect.zw/destinationRect.zw);}
 uvec4 mask=uvec4(destinationMask);
 uvec4 packed=uvec4(channel(value.r,mask.r),channel(value.g,mask.g),channel(value.b,mask.b),channel(value.a,mask.a));
 outputColor=mix(vec4(1),vec4(packed)/max(vec4(mask),vec4(1)),greaterThan(mask,uvec4(0)));
}
)GLSL";
}
