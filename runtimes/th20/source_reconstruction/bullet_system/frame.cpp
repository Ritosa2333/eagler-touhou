#include "bullet.hpp"
#include "../program_entry/program_entry.hpp"
#include "../gameplay/gameplay.hpp"
#include "../runtime_state/state.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/dispatch.hpp"
#include "../ecl_vm/math.hpp"
namespace th20::source::bullet {
namespace pe=program_entry;namespace n=recovered;
int Controller::update(){
    pe::sprite_controller->field_6c4=static_cast<std::uint32_t>(view_index);
    for(auto& field:fields_14)field=0;
    scheduler::Iterator iterator(active.sentinel.next);
    for(;iterator.current;iterator.advance()){
        auto& b=*reinterpret_cast<Bullet*>(iterator.current->value);
        if(!(gameplay::controller&&(gameplay::controller->game_flags&0x800u))){
            if((b.flags&0x100u)&&(b.state==1||(b.state==2&&b.timer_4f8.current>=8)))unrecovered::player_hit_test_00484a20(b,1);
            else if(update_bullet(b))continue;
        }
        if(!(b.flags&0x200u)){
            auto& first=fields_14[b.field_44];auto& last=fields_14[6+b.field_44];
            if(!first)first=reinterpret_cast<std::uint32_t>(&b);
            else if(last)reinterpret_cast<Bullet*>(last)->field_60=reinterpret_cast<std::uint32_t>(&b);
            last=reinterpret_cast<std::uint32_t>(&b);b.field_60=0;
        }
        ++fields_14[12];n::timer_tick(b.timer_4f8,state::timer_rate);
    }
    return 1;
}
int update_bullet(Bullet& b){
    if(!(b.flags&0x400u)&&unrecovered::advance_bullet_00485b60(b))return -1;
    if(b.animation_handle){auto* a=sprite::find_animation(*pe::sprite_controller,b.animation_handle);if(a)a->vector_5bc=b.position;}
    if((b.state==1||(b.state==2&&(b.timer_4f8.current<8||unrecovered::player_hit_test_00484a20(b,0)!=1)))&&!(b.flags&0x200u))unrecovered::player_hit_test_00484a20(b,0);
    b.flags&=~0x400u;return 0;
}
int Controller::draw(){
    auto& sprites=*pe::sprite_controller;sprite::configure_animation_layer(sprites,12,view_index);
    for(unsigned group=0;group<6;++group)for(auto* b=reinterpret_cast<Bullet*>(fields_14[group]);b;b=reinterpret_cast<Bullet*>(b->field_60)){
        auto& animation=*b->animation;animation.vector_5bc=b->position;
        if((animation.base.flags[2]>>21)&7u){animation.base.vector_38.z=ecl::math::wrap_angle(n::add32(b->angle,1.5707963705062866f));animation.base.flags[1]|=2u;}
        if(b->flags&64u){animation.base.vector_58={b->scale,b->scale};animation.base.flags[1]|=4u;}
        sprite::draw_animation(sprites,animation);
    }
    return 1;
}
}
