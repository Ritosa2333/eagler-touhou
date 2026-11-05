#pragma once
namespace touhou::sdl {
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
