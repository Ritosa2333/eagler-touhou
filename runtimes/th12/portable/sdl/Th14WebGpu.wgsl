struct Uniform {
 factor:vec4f, fogColor:vec4f, flags:vec4u, alpha:vec4f,
 color:vec4u, args:vec4u, clipMatrix:mat4x4f, eyeMatrix:mat4x4f,
 uvMatrix:mat4x4f, linearFog:vec4f, vertexFlags:vec4u,
 viewport:vec4f, fogParams:vec4f, extra:vec4u,
};
@group(0) @binding(0) var<uniform> u:Uniform;
@group(1) @binding(0) var tex:texture_2d<f32>;
@group(1) @binding(1) var texSampler:sampler;
struct Input {
 @location(0) position:vec4f, @location(1) color:vec4f,
 @location(2) uv:vec2f, @location(3) fog:f32,
 @location(4) eligible:f32, @location(5) specular:vec4f,
};
struct InstanceInput {
 @location(0) position:vec4f, @location(1) color:vec4f,
 @location(2) uv:vec2f, @location(3) fog:f32,
 @location(4) eligible:f32, @location(5) specular:vec4f,
 @location(6) clip0:vec4f, @location(7) clip1:vec4f,
 @location(8) clip2:vec4f, @location(9) clip3:vec4f,
 @location(10) eye0:vec4f, @location(11) eye1:vec4f,
 @location(12) eye2:vec4f, @location(13) eye3:vec4f,
 @location(14) factor:vec4f, @location(15) primitiveEligible:f32,
};
struct Output {
 @builtin(position) position:vec4f, @location(0) color:vec4f,
 @location(1) uv:vec2f, @location(2) fog:f32,
 @location(3) specular:vec4f,
 @location(4) @interpolate(flat) factor:vec4f,
};
fn round_even(v:vec2f)->vec2f {
 let a=floor(v);let fraction=v-a;
 return select(select(a,a+1.0,fraction>vec2f(0.5)),
  select(a,a+1.0,(vec2i(a)&vec2i(1))!=vec2i(0)),fraction==vec2f(0.5));
}
fn vertex(v:Input,clip:mat4x4f,eyeMatrix:mat4x4f,factor:vec4f,eligible:f32)->Output {
 var o:Output;o.position=v.position;o.color=v.color.bgra;o.factor=factor;
 o.uv=v.uv;o.fog=v.fog;o.specular=v.specular.bgra;
 if(u.vertexFlags.x==0u) {
  let original=vec4f(v.position.xyz,1);o.position=clip*original;
  if(eligible>0.5) {
   let half_viewport=u.viewport.zw*0.5;
   // Round in screen coordinates. Rounding before the viewport translation
   // loses binary32 boundary decisions in subviewports.
   let pixel=round_even((((vec2f(o.position.x,-o.position.y)*half_viewport)/o.position.w+half_viewport)+u.viewport.xy)*256.0)/256.0-u.viewport.xy;
   o.position.x=(pixel.x/half_viewport.x-1.0)*o.position.w;
   o.position.y=-(pixel.y/half_viewport.y-1.0)*o.position.w;
  }
  if(u.vertexFlags.y!=0u){o.uv=(u.uvMatrix*vec4f(v.uv,1,0)).xy;}
  o.fog=1;
  if(u.flags.x!=0u) {
   let eye=eyeMatrix*original;var z=abs(eye.z);
   if(u.extra.y!=0u){z=length(eye.xyz);}
   if(u.vertexFlags.z!=0u){o.fog=u.linearFog.x-z*u.linearFog.y;}
   else if(u.extra.z==1u){o.fog=exp(-u.fogParams.z*z);}
   else if(u.extra.z==2u){o.fog=exp(-pow(u.fogParams.z*z,2.0));}
   else{o.fog=(u.fogParams.y-z)/(u.fogParams.y-u.fogParams.x);}
  }
 }
 return o;
}
@vertex fn vs(v:Input)->Output {
 return vertex(v,u.clipMatrix,u.eyeMatrix,u.factor,v.eligible);
}
@vertex fn vsInstanced(v:InstanceInput)->Output {
 return vertex(Input(v.position,v.color,v.uv,v.fog,v.eligible,v.specular),
   mat4x4f(v.clip0,v.clip1,v.clip2,v.clip3),mat4x4f(v.eye0,v.eye1,v.eye2,v.eye3),v.factor,v.primitiveEligible);
}
fn argument(value:u32,t:vec4f,v:Output)->vec4f {
 var result=v.color;let source=value&15u;
 if(source==1u){result=t;}else if(source==2u){result=v.factor;}
 else if(source==3u){result=v.specular;}
 if((value&16u)!=0u){result=1.0-result;}
 if((value&32u)!=0u){result=vec4f(result.a);}return result;
}
fn operation(mode:u32,a:vec4f,b:vec4f,t:vec4f,v:Output)->vec4f {
 if(mode==1u){return a;}if(mode==2u){return b;}
 if(mode==3u){return a*b;}if(mode==4u){return a*b*2.0;}
 if(mode==5u){return a*b*4.0;}if(mode==6u){return a+b;}
 if(mode==7u){return a+b-0.5;}if(mode==8u){return (a+b-0.5)*2.0;}
 if(mode==9u){return a-b;}if(mode==10u){return a+b*(1.0-a);}
 if(mode==11u||mode==15u){return b*(1.0-v.color.a)+a*v.color.a;}
 if(mode==12u){return b*(1.0-t.a)+a*t.a;}
 if(mode==13u){return b*(1.0-v.factor.a)+a*v.factor.a;}
 if(mode==14u){return a+b*(1.0-t.a);}return v.color;
}
struct FragmentOut {@location(0) color:vec4f,@builtin(frag_depth) depth:f32};
fn fragment(v:Output)->FragmentOut {
 let t=textureSample(tex,texSampler,v.uv);var c=v.color;
 if(u.flags.w!=0u) {
  if(u.color.x!=0u){c=operation(u.color.x,argument(u.color.z,t,v),argument(u.color.w,t,v),t,v);}
  c.a=v.color.a;
  if(u.color.y!=0u){c.a=operation(u.color.y,argument(u.args.x,t,v),argument(u.args.y,t,v),t,v).a;}
 }
 c=clamp(c,vec4f(0),vec4f(1));
 if(u.flags.y!=0u) {
  var accepted=true;let f=u.flags.z;let ge=c.a>=u.alpha.x;let gt=c.a>=u.alpha.y;
  if(f==0u){accepted=false;}else if(f==1u){accepted=!ge;}
  else if(f==2u){accepted=ge&&!gt;}else if(f==3u){accepted=!gt;}
  else if(f==4u){accepted=gt;}else if(f==5u){accepted=!ge||gt;}
  else if(f==6u){accepted=ge;}if(!accepted){discard;}
 }
 if(u.flags.x!=0u){let fog=clamp(v.fog,0.0,1.0);let rest=u.fogColor.rgb-u.fogColor.rgb*fog;c=vec4f(c.rgb*fog+rest,c.a);}
 var depth=v.position.z;
 if(u.extra.x!=0u){let q=floor(depth*1048576.0)/1048576.0;let scaled=q*65536.0;depth=(floor(scaled)+ceil(fract(scaled)-q-0.5))/65535.0;}
 return FragmentOut(c,depth);
}
@fragment fn fs(v:Output)->FragmentOut{return fragment(v);}
@fragment fn fsColor(v:Output)->@location(0) vec4f{return fragment(v).color;}
