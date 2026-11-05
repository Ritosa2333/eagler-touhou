#include "menu_animation.hpp"
#include "binding.hpp"
#include "pool.hpp"
#include "loading_interrupt.hpp"
#include "anm_vm.hpp"
#include "../core_scheduler/scheduler.hpp"
namespace th20::source::sprite {
void set_animation_texture_rectangle(Animation& animation,const SpriteData& sprite,float x,float y,float width,float height){
    auto& b=animation.base;
    float value=sprite.u0+x/sprite.texture_extent_20;b.vectors_378[2].x=value;b.vectors_378[0].x=value;
    value=sprite.u0+(x+width)/sprite.texture_extent_20;b.vectors_378[3].x=value;b.vectors_378[1].x=value;
    value=sprite.v0+y/sprite.texture_extent_1c;b.vectors_378[1].y=value;b.vectors_378[0].y=value;
    value=sprite.v0+(y+height)/sprite.texture_extent_1c;b.vectors_378[3].y=value;b.vectors_378[2].y=value;
    b.vector_398={b.vectors_378[1].x-b.vectors_378[0].x,b.vectors_378[2].y-b.vectors_378[0].y};b.vector_70={width,height};
    constexpr Matrix4 identity{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};b.matrix_3b8=b.matrix_3f8=identity;
    b.matrix_3b8.elements[0]=b.vector_70.x/256.f;b.matrix_3b8.elements[5]=b.vector_70.y/256.f;
    b.matrix_3f8.elements[0]=(b.vector_70.x/sprite.texture_extent_20)*sprite.scale_50;b.matrix_3f8.elements[5]=(b.vector_70.y/sprite.texture_extent_1c)*sprite.scale_54;animation.matrix_57c=b.matrix_3b8;
    b.fields_3a0[2]=static_cast<unsigned>(recovered::truncate32(sprite.left+x));b.fields_3a0[3]=static_cast<unsigned>(recovered::truncate32(sprite.top+y));b.fields_3a0[4]=static_cast<unsigned>(recovered::truncate32((sprite.left+x)+width));b.fields_3a0[5]=static_cast<unsigned>(recovered::truncate32((sprite.top+y)+height));
}
void set_animation_texture_rectangle(Controller& controller,Animation& animation,float x,float y,float width,float height){set_animation_texture_rectangle(animation,current_sprite(controller,animation),x,y,width,height);}
Animation* find_animation_child(Animation& parent,int script,int occurrence){
    auto* current=parent.links[3].value?&parent.links[3]:nullptr;
    for(;current;current=current->next){
        auto* child=current->value;if(!child||child==&parent)continue;
        if(static_cast<std::int16_t>(child->base.field_440)==script||script==-1){if(occurrence==0)return child;occurrence=recovered::signed_bits(static_cast<unsigned>(occurrence)-1);}
        auto* nested=child->links[3].value?&child->links[3]:nullptr;
        if(nested&&nested->next)if(auto* match=find_animation_child(*child,script,occurrence))return match;
        if(static_cast<std::int16_t>(parent.base.field_440)==-2&&!current->next)return child;
    }
    return nullptr;
}
Animation* find_animation_child(Controller& controller,std::uint32_t& handle,int script,int occurrence){auto* animation=resolve_animation_handle(controller,handle);return animation?find_animation_child(*animation,script,occurrence):nullptr;}
void execute_animation_interrupt(Controller& controller,std::uint32_t handle,int event){
    auto* animation=find_animation(controller,handle);if(!animation)return;
    auto* first=animation->links[3].value?&animation->links[3]:nullptr;
    scheduler::Iterator iterator(reinterpret_cast<scheduler::Link*>(first));
    if(iterator.current)iterator.current->iterator=nullptr;if(iterator.next)iterator.next->iterator=nullptr;
    for(;iterator.current;iterator.advance()){
        auto* value=reinterpret_cast<Animation*>(iterator.current->value);set_animation_interrupt(*value,event);
        // Fetch the current link again: callbacks may replace its value.
        value=reinterpret_cast<Animation*>(iterator.current->value);execute_animation(*value);
    }
}
}
