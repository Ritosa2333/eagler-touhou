#include "Item.hpp"
#include "GameMath.hpp"
namespace th10 {
// 0x404f30, used for standalone sprite VMs embedded in game objects.
void ItemEnvironment::initialize_animation(Item& item,i32 script){
    auto& vm=item.animation;vm.initialize();vm.position={};vm.script_position={};vm.child_position={};
    vm.text_settings[0]=vm.text_settings[1]=0x10;vm.script_index=static_cast<std::int16_t>(script);
    animation_file->initialize_script(vm,script,*animations,*started_animations);
}
namespace {
void initialize_motion(Item& item,float angle,float speed,const float* rate){
    const auto velocity=polar(angle,speed);item.velocity={velocity.x,velocity.y,0};
    if(!(item.timer_flags&1)){item.timer.rate=rate;item.timer_flags|=1;}
    item.timer.initialize(-1);item.motion_3c4=0;
}
}
// 0x41bb00. Regular and faith items have separate pools and full-pool behavior.
i32 ItemManager::spawn(const Vec3& position,i32 kind,u32 color,float angle,float speed,ItemEnvironment& env){
    if(kind==8){
        auto& item=faith[faith_cursor];faith_count=wrapping_add(faith_count,1);
        if(item.state==0){
            item.value_variant=faith_count<256?faith_cursor%4:faith_count<512?faith_cursor%8+4:faith_count<1024?faith_cursor%16+8:faith_cursor%32+16;
            item.kind=item.sprite_kind=8;item.state=5;item.position=position;initialize_motion(item,angle,speed,env.default_rate);
        }
        faith_cursor=wrapping_add(faith_cursor,1)%2048;return 0;
    }
    Item* item=nullptr;for(auto& candidate:regular)if(candidate.state==0){item=&candidate;break;}
    if(!item)return 0;
    item->state=1;item->position=position;
    if(number(item->position.x)<number(-192.0f))item->position.x=-192.0f;
    else if(number(192.0f)<number(item->position.x)||number(item->position.x)==number(192.0f))item->position.x=192.0f;
    initialize_motion(*item,angle,speed,env.default_rate);
    if(*env.power>=100){if(kind==1||kind==4)kind=9;else if(kind==10||kind==11)kind=5;}
    if(item->kind==3)env.spawn_effect(item->position,0x189);
    item->kind=item->sprite_kind=kind;
    if(kind==10)item->sprite_kind=1;else if(kind==11)item->sprite_kind=4;
    env.initialize_animation(*item,wrapping_add(item->sprite_kind,0x176));
    item->animation.color=color;return 0;
}
}
