struct Uniform {sourceRect:vec4i,destinationRect:vec4i,sourceMask:vec4u,destinationMask:vec4u,flags:vec4u};
@group(0) @binding(0) var<uniform> u:Uniform;
@group(0) @binding(1) var sourceImage:texture_2d<f32>;
@group(0) @binding(2) var<storage,read> weights:array<vec2f>;
@vertex fn vs(@builtin(vertex_index) id:u32)->@builtin(position) vec4f {
 let p=vec2f(f32((id<<1u)&2u),f32(id&2u));return vec4f(p*2.0-1.0,0,1);
}
// Preserve the separately rounded positive binary32 operations used by the
// existing capture writer, including drivers that fuse native multiply/add.
fn multiply(a:f32,b:f32)->f32 {
 if(a==0.0||b==0.0){return 0;}
 let ab=bitcast<u32>(a);let bb=bitcast<u32>(b);
 let am=(ab&0x7fffffu)|0x800000u;let bm=(bb&0x7fffffu)|0x800000u;
 let lowProduct=(am&65535u)*(bm&65535u);let cross=(am>>16)*(bm&65535u)+(am&65535u)*(bm>>16);
 let low=lowProduct+(cross<<16);let high=(am>>16)*(bm>>16)+(cross>>16)+u32(low<lowProduct);
 let shift=select(23u,24u,(high&0x8000u)!=0u);
 var mantissa=(low>>shift)|(high<<(32u-shift));let tail=low&((1u<<shift)-1u);let midpoint=1u<<(shift-1u);
 mantissa+=u32(tail>midpoint||(tail==midpoint&&(mantissa&1u)!=0u));
 var exponent=((ab>>23)&255u)+((bb>>23)&255u)-127u+shift-23u;
 if(mantissa==0x1000000u){mantissa>>=1;exponent+=1;}
 return bitcast<f32>((exponent<<23)|(mantissa&0x7fffffu));
}
fn add(a:f32,b:f32)->f32 {
 if(a==0.0){return b;}if(b==0.0){return a;}
 var ab=bitcast<u32>(a);var bb=bitcast<u32>(b);
 if(ab<bb){let temporary=ab;ab=bb;bb=temporary;}
 var exponent=(ab>>23)&255u;let delta=exponent-((bb>>23)&255u);
 if(delta>27u){return max(a,b);}
 let am=((ab&0x7fffffu)|0x800000u)<<3;var bm=((bb&0x7fffffu)|0x800000u)<<3;
 if(delta!=0u){bm=(bm>>delta)|u32((bm&((1u<<delta)-1u))!=0u);}
 var sum=am+bm;if((sum&0x8000000u)!=0u){sum=(sum>>1)|(sum&1u);exponent+=1;}
 var mantissa=sum>>3;let tail=sum&7u;mantissa+=u32(tail>4u||(tail==4u&&(mantissa&1u)!=0u));
 if(mantissa==0x1000000u){mantissa>>=1;exponent+=1;}
 return bitcast<f32>((exponent<<23)|(mantissa&0x7fffffu));
}
fn vector_multiply(a:f32,b:vec4f)->vec4f{return vec4f(multiply(a,b.r),multiply(a,b.g),multiply(a,b.b),multiply(a,b.a));}
fn vector_add(a:vec4f,b:vec4f)->vec4f{return vec4f(add(a.r,b.r),add(a.g,b.g),add(a.b,b.b),add(a.a,b.a));}
fn channel(value:f32,mask:u32)->u32 {
 if(value<=0.0||mask==0u){return 0;}if(value>=1.0){return mask;}
 let bits=bitcast<u32>(value);let shift=150-i32((bits>>23)&255u);
 var product=((bits&0x7fffffu)|0x800000u)*mask;
 var discardBits=0u;var high=product>>24;
 while(high!=0u){discardBits+=1;high>>=1;}
 if(discardBits>0u){product=(product>>discardBits)<<discardBits;}
 if(shift>32){return 0;}if(shift==32){return u32(product>=0x80000000u);}
 return (product>>u32(shift))+u32((product&((1u<<u32(shift))-1u))>=(1u<<u32(shift-1)));
}
fn original_unit(mask:u32)->f32 {
 var bits=0x3f800000u;
 if(mask==255u){bits=0x3b808081u;}else if(mask==31u){bits=0x3d042108u;}
 else if(mask==63u){bits=0x3c820821u;}else if(mask==15u){bits=0x3d888889u;}
 return bitcast<f32>(bits);
}
fn original_pixel(at:vec2i)->vec4f {
 let p=textureLoad(sourceImage,at,0);let integers=floor(p*vec4f(u.sourceMask)+0.5);
 let value=vec4f(multiply(integers.r,original_unit(u.sourceMask.r)),multiply(integers.g,original_unit(u.sourceMask.g)),multiply(integers.b,original_unit(u.sourceMask.b)),multiply(integers.a,original_unit(u.sourceMask.a)));
 return select(vec4f(1),value,u.sourceMask>vec4u(0));
}
fn expand(value:u32,mask:u32)->f32 {
 var byte=value;
 if(mask==0u){byte=255u;}else if(mask==31u){byte=(value<<3u)|(value>>2u);}
 else if(mask==63u){byte=(value<<2u)|(value>>4u);}
 else if(mask==15u){byte=value*17u;}else if(mask==1u){byte=value*255u;}
 return f32(byte)/255.0;
}
@fragment fn fs(@builtin(position) position:vec4f)->@location(0) vec4f {
 let at=vec2i(position.xy)-u.destinationRect.xy;var value=vec4f(0);
 if(u.flags.x!=0u) {
  let row=u.destinationRect.z+at.y;let width=i32(u.flags.y);
  let nx=i32(weights[at.x*width].x);let ny=i32(weights[row*width].x);
  for(var y=1;y<=ny;y+=1){let wy=weights[row*width+y];
   for(var x=1;x<=nx;x+=1){let wx=weights[at.x*width+x];
    let weight=multiply(wx.y,wy.y);
    value=vector_add(vector_multiply(weight,original_pixel(u.sourceRect.xy+vec2i(i32(wx.x),i32(wy.x)))),value);
   }
  }
 }else{value=original_pixel(u.sourceRect.xy+at*u.sourceRect.zw/u.destinationRect.zw);}
 let mask=u.destinationMask;let packed=vec4u(channel(value.r,mask.r),channel(value.g,mask.g),channel(value.b,mask.b),channel(value.a,mask.a));
 return vec4f(expand(packed.r,mask.r),expand(packed.g,mask.g),expand(packed.b,mask.b),expand(packed.a,mask.a));
}
