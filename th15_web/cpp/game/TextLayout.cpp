#include "TextLayout.hpp"
namespace th15 {
bool TextLayout::dialogue(AnmVm& vm,const DialogueText& request,bool fallback,TextLayout& output){
    if(!vm.sprite||!vm.resource||request.font<0||request.font>8)return false;const auto& sprite=*vm.sprite;output.resource=vm.sprite_resource?vm.sprite_resource:vm.resource;output.texture=sprite.texture;output.region={truncate_int(sprite.x),truncate_int(sprite.y),truncate_int(float(sprite.x+sprite.width)),truncate_int(float(sprite.y+sprite.height))};
    constexpr i32 heights[]={17,17,17,17,21,21,21,21,14};auto& style=output.style;style.height=heights[request.font];style.font=request.font;style.offset=request.right_aligned?truncate_int(float(sprite.width-float((style.height*2-1)*i32(request.bytes.size())/2))):(request.offset_pixels?request.offset:wrapping_mul(request.offset,2));style.spacing=wrapping_mul(request.style,2);style.color=request.color;style.outline=request.secondary_color;style.shadow=(vm.visual.render_flags&0x1000)==0;style.fallback=fallback;return true;
}
}
