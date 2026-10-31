#include "TextLayout.hpp"
namespace th17 {
bool TextLayout::prepare(AnmVm& vm,const TextureText& request,bool fallback,TextLayout& output){
 if(!vm.sprite||!vm.resource||request.font<0||request.font>8)return false;const auto& sprite=*vm.sprite;
 output.resource=vm.sprite_resource?vm.sprite_resource:vm.resource;output.texture=sprite.texture;output.region={truncate_int(sprite.x),truncate_int(sprite.y),truncate_int(float(sprite.x+sprite.width)),truncate_int(float(sprite.y+sprite.height))};
 constexpr i32 heights[]={17,17,17,17,21,21,21,21,14};auto& style=output.style;style.height=heights[request.font];style.font=request.font;style.offset=wrapping_mul(request.x,2);
 const u32 length=u32(wrapping_mul(style.height*2-1,i32(request.bytes.size())));
 if(request.right_aligned)style.offset=truncate_int(float(sprite.width-float(length>>1)));
 else if(request.centered)style.offset=wrapping_sub(truncate_int(sprite.width)/2,i32(length>>2));
 style.spacing=wrapping_mul(request.spacing,2);style.color=request.color;style.outline=(vm.visual.render_flags&0x1000)?0:request.outline;style.shadow=(vm.visual.render_flags&0x1000)==0;style.fallback=fallback;return true;
}
}
