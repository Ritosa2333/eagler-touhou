#pragma once
namespace touhou::sdl {
#if defined(PORT_TITLE) && PORT_TITLE == 14
// Preserve the original model/eye path for fog. Clip matrices are composed in
// C++ for each original world; only the homogeneous half-pixel bias changes.
inline constexpr const char* vertexSource=R"GLSL(#version 300 es
precision highp float;
layout(location=0) in vec4 position;
layout(location=1) in vec4 diffuse;
layout(location=2) in vec2 uv;
layout(location=3) in vec4 specular;
layout(location=4) in vec4 instanceWorld0;
layout(location=5) in vec4 instanceWorld1;
layout(location=6) in vec4 instanceWorld2;
layout(location=7) in vec4 instanceWorld3;
layout(location=8) in vec4 instanceFactor;
layout(location=9) in vec4 instanceClip0;
layout(location=10) in vec4 instanceClip1;
layout(location=11) in vec4 instanceClip2;
layout(location=12) in vec4 instanceClip3;
layout(location=13) in float primitiveViewportEligible;
layout(location=14) in vec4 clipEdgeB;
layout(location=15) in vec3 clipAttributesB;
uniform mat4 world,view,projection,textureMatrix,clipMatrix;
uniform vec4 factor;
uniform vec4 viewport;
uniform bool transformed,textureTransform,fogEnabled,rangeFog;
uniform bool instanced;
uniform int fogMode;
uniform vec3 fogParams;
out vec4 vColor,vSpecular;
out vec2 vUV;
out float vFog;
flat out vec4 vFactor;
// Clip interpolation rounds both weights and each product independently.
// Integer limbs preserve binary32 nearest-even products on GLES backends
// whose native floating multiply may choose a different permitted rounding.
// Derived from the shared TH143 FloatMultiplyNearestV2 reference.
uint multiplyNearestBits(uint ua,uint ub){
 uint sign=(ua^ub)&0x80000000u;
 int ea=int((ua>>23)&255u),eb=int((ub>>23)&255u);
 uint ma=ua&0x7fffffu,mb=ub&0x7fffffu;
 if(ea==255||eb==255){
  if((ea==255&&ma!=0u)||(eb==255&&mb!=0u)||(ea==255&&eb==0&&mb==0u)||(eb==255&&ea==0&&ma==0u))return 0x7fc00000u;
  return sign|0x7f800000u;
 }
 if((ea==0&&ma==0u)||(eb==0&&mb==0u))return sign;
 if(ea==0){ea=1;for(int i=0;i<23&&ma<0x800000u;++i){ma<<=1;--ea;}}else ma|=0x800000u;
 if(eb==0){eb=1;for(int i=0;i<23&&mb<0x800000u;++i){mb<<=1;--eb;}}else mb|=0x800000u;
 uint al=ma&65535u,ah=ma>>16,bl=mb&65535u,bh=mb>>16;
 uint p00=al*bl,p01=al*bh,p10=ah*bl,p11=ah*bh;
 uint middle=(p00>>16)+(p01&65535u)+(p10&65535u);
 uint lo=(p00&65535u)|(middle<<16),hi=p11+(p01>>16)+(p10>>16)+(middle>>16);
 int normalized=(hi&0x8000u)!=0u?1:0,exponent=ea+eb-127+normalized,shift=23+normalized;
 if(exponent>=255)return sign|0x7f800000u;
 if(exponent<=0)shift+=1-exponent;
 if(shift>48)return sign;
 uint mantissa,discarded,halfway;bool sticky=false;
 if(shift<32){mantissa=(lo>>uint(shift))|(hi<<uint(32-shift));discarded=lo&((1u<<uint(shift))-1u);halfway=1u<<uint(shift-1);}
 else if(shift==32){mantissa=hi;discarded=lo;halfway=0x80000000u;}
 else{int highShift=shift-32;mantissa=hi>>uint(highShift);discarded=hi&((1u<<uint(highShift))-1u);halfway=1u<<uint(highShift-1);sticky=lo!=0u;}
 if(discarded>halfway||(discarded==halfway&&(sticky||(mantissa&1u)!=0u)))++mantissa;
 if(exponent<=0)return sign|mantissa;
 if(mantissa==0x1000000u){mantissa>>=1;++exponent;}
 return sign|(uint(exponent)<<23)|(mantissa&0x7fffffu);
}
float multiplyNearest(float a,float b){return uintBitsToFloat(multiplyNearestBits(floatBitsToUint(a),floatBitsToUint(b)));}

float blendNative(float a,float b,float wa,float wb){return multiplyNearest(a,wa)+multiplyNearest(b,wb);}
void main(){
  mat4 model=instanced?mat4(instanceWorld0,instanceWorld1,instanceWorld2,instanceWorld3):world;
  vec4 eye=view*model*vec4(position.xyz,1.0);
  if(transformed){vec2 xy=(position.xy-viewport.xy+vec2(.5))/viewport.zw;gl_Position=vec4(xy.x*2.0-1.0,xy.y*2.0-1.0,position.z*2.0-1.0,1.0)/position.w;}
  else {mat4 composed=instanced?mat4(instanceClip0,instanceClip1,instanceClip2,instanceClip3):clipMatrix;gl_Position=composed*vec4(position.xyz,1.0);gl_Position.y=-gl_Position.y;vec2 numerator=gl_Position.xy*viewport.zw+vec2(gl_Position.w);gl_Position.xy=numerator/viewport.zw;if(primitiveViewportEligible>0.5){vec4 original=composed*vec4(position.xyz,1.0);vec2 halfViewport=viewport.zw*0.5;vec2 pixel=(vec2(original.x,-original.y)*halfViewport)/original.w+halfViewport;pixel=roundEven((pixel+viewport.xy)*256.0)/256.0-viewport.xy;gl_Position.xy=((pixel+vec2(0.5))/halfViewport-vec2(1.0))*original.w;}gl_Position.z=gl_Position.z*2.0-gl_Position.w;}
  vColor=diffuse.bgra;vSpecular=specular.bgra;
  vFactor=instanced?instanceFactor.bgra:factor;
  vUV=textureTransform&&!transformed?(textureMatrix*vec4(uv,1.0,0.0)).xy:uv;
  float z=rangeFog?length(eye.xyz):abs(eye.z);
  float fog=fogMode==1?exp(-fogParams.z*z):fogMode==2?exp(-pow(fogParams.z*z,2.0)):(fogParams.y-z)/(fogParams.y-fogParams.x);
  vFog=fogEnabled?(transformed?specular.a:fog):1.0;
  if(primitiveViewportEligible<-.5){
    vec4 q=position;vec2 mappedUv=uv;float mappedFog=specular.x;
    if(primitiveViewportEligible<-1.5){
      float r=1./(q.z-clipEdgeB.z),wa=multiplyNearest(-clipEdgeB.z,r),wb=multiplyNearest(q.z,r);
      q=vec4(blendNative(q.x,clipEdgeB.x,wa,wb),blendNative(q.y,clipEdgeB.y,wa,wb),0.,blendNative(q.w,clipEdgeB.w,wa,wb));
      mappedUv=vec2(blendNative(uv.x,clipAttributesB.x,wa,wb),blendNative(uv.y,clipAttributesB.y,wa,wb));
      mappedFog=blendNative(specular.x,clipAttributesB.z,wa,wb);
    }
    // Keep homogeneous W and the original integer viewport. Rescaling into
    // a power-of-two viewport changes interpolated UV/fog in the controls.
    gl_Position=vec4(vec2(q.x,-q.y)+q.w/viewport.zw,q.z,q.w);gl_Position.z=gl_Position.z*2.0-gl_Position.w;
    vUV=mappedUv;vFog=mappedFog;vColor=vec4(1.);vSpecular=vec4(1.);
  }
})GLSL";
#else
inline constexpr const char* vertexSource=R"GLSL(#version 300 es
precision highp float;
layout(location=0) in vec4 position;
layout(location=1) in vec4 diffuse;
layout(location=2) in vec2 uv;
layout(location=3) in vec4 specular;
layout(location=4) in vec4 instanceWorld0;
layout(location=5) in vec4 instanceWorld1;
layout(location=6) in vec4 instanceWorld2;
layout(location=7) in vec4 instanceWorld3;
layout(location=8) in vec4 instanceFactor;
uniform mat4 world,view,projection,textureMatrix;
uniform vec4 factor;
uniform vec4 viewport;
uniform bool transformed,textureTransform,fogEnabled,rangeFog;
uniform bool instanced;
uniform int fogMode;
uniform vec3 fogParams;
out vec4 vColor,vSpecular;
out vec2 vUV;
out float vFog;
flat out vec4 vFactor;
void main(){
  mat4 model=instanced?mat4(instanceWorld0,instanceWorld1,instanceWorld2,instanceWorld3):world;
  vec4 eye=view*model*vec4(position.xyz,1.0);
  if(transformed){vec2 xy=(position.xy-viewport.xy+vec2(.5))/viewport.zw;gl_Position=vec4(xy.x*2.0-1.0,xy.y*2.0-1.0,position.z*2.0-1.0,1.0)/position.w;}
  else {gl_Position=projection*eye;gl_Position.y=-gl_Position.y;gl_Position.xy+=gl_Position.w/viewport.zw;gl_Position.z=gl_Position.z*2.0-gl_Position.w;}
  vColor=diffuse.bgra;vSpecular=specular.bgra;
  vFactor=instanced?instanceFactor.bgra:factor;
  vUV=textureTransform&&!transformed?(textureMatrix*vec4(uv,1.0,0.0)).xy:uv;
  float z=rangeFog?length(eye.xyz):abs(eye.z);
  float fog=fogMode==1?exp(-fogParams.z*z):fogMode==2?exp(-pow(fogParams.z*z,2.0)):(fogParams.y-z)/(fogParams.y-fogParams.x);
  vFog=fogEnabled?(transformed?specular.a:fog):1.0;
})GLSL";
#endif
inline constexpr const char* fragmentSource=R"GLSL(#version 300 es
precision highp float;
in vec4 vColor,vSpecular;in vec2 vUV;in float vFog;
flat in vec4 vFactor;
uniform sampler2D tex;
uniform bool hasTexture,alphaTest,fogEnabled;
uniform bool depth16;
uniform vec4 fogColor;
uniform int colorOp,alphaOp,colorArg1,colorArg2,alphaArg1,alphaArg2,alphaFunc;
uniform float alphaRef;
out vec4 outColor;
vec4 arg(int value,vec4 textureColor){int source=value&15;vec4 c=source==1?textureColor:source==2?vFactor:source==3?vSpecular:vColor;if((value&16)!=0)c=1.0-c;if((value&32)!=0)c=vec4(c.a);return c;}
vec4 op(int mode,vec4 a,vec4 b,vec4 textureColor){
 if(mode==1)return a;if(mode==2)return b;if(mode==3)return a*b;if(mode==4)return a*b*2.0;if(mode==5)return a*b*4.0;if(mode==6)return a+b;if(mode==7)return a+b-.5;if(mode==8)return (a+b-.5)*2.0;if(mode==9)return a-b;if(mode==10)return a+b*(1.0-a);
 if(mode==11)return mix(b,a,vColor.a);if(mode==12)return mix(b,a,textureColor.a);if(mode==13)return mix(b,a,vFactor.a);if(mode==14)return a+b*(1.0-textureColor.a);if(mode==15)return mix(b,a,vColor.a);return vColor;
}
void main(){vec4 t=hasTexture?texture(tex,vUV):vec4(1.0);
 vec4 c=hasTexture&&colorOp!=0?op(colorOp,arg(colorArg1,t),arg(colorArg2,t),t):vColor;
 c.a=hasTexture&&alphaOp!=0?op(alphaOp,arg(alphaArg1,t),arg(alphaArg2,t),t).a:vColor.a;c=clamp(c,0.0,1.0);
 if(alphaTest){float a=c.a*255.0,ref=floor(alphaRef*255.0+.5);bool pass=alphaFunc==7||(alphaFunc==1&&a<ref)||(alphaFunc==2&&a==ref)||(alphaFunc==3&&a<=ref)||(alphaFunc==4&&a>ref)||(alphaFunc==5&&a!=ref)||(alphaFunc==6&&a>=ref);if(!pass)discard;}
 if(fogEnabled)c.rgb=mix(fogColor.rgb,c.rgb,clamp(vFog,0.0,1.0));outColor=c;
 // The game compares incoming D16 depth after quantization. Splitting the
 // product preserves values just below a half-unit (for example float 0.9).
 float scaled=gl_FragCoord.z*65536.0;
 gl_FragDepth=depth16?(floor(scaled)+floor(fract(scaled)-gl_FragCoord.z+0.5))/65535.0:gl_FragCoord.z;
})GLSL";
}
