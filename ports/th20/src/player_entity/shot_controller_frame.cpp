#include "shot_controller_frame.hpp"
#include "power.hpp"
namespace th20::source::player_entity {
int update_shooting(ShotController& owner,ShotControllerServices& env){
    if(env.input_blocked())return 0;
    const int slot=env.input_slot(owner.view_index);
    if(env.held(slot,1))owner.field_1255c|=2;
    auto& player=*static_cast<Player*>(owner.context->objects_04[0]);
    if(player.state!=1){owner.field_12578=0;owner.byte_1257c=0;recovered::timer_set(owner.timer_12400,-1);return 0;}
    const float rate=env.frames().callbacks().firing().clock_rate();
    if(owner.timer_12400.current>=0 || (env.held(slot,1)&&!(owner.field_1255c&4u))){
        if(owner.timer_12400.current<0){if(owner.timer_12410.current<0)recovered::timer_set(owner.timer_12410,0);recovered::timer_set(owner.timer_12400,0);}
        if(owner.timer_12400.current!=owner.timer_12400.previous)
            env.shoot_weapons(*owner.context,owner.timer_12400.current,owner.timer_12410.current,power_level(*owner.context->current_player));
        if(owner.timer_12400.current>=14){if(!env.held(slot,1)||(owner.field_1255c&4u))recovered::timer_set(owner.timer_12400,-1);else recovered::timer_add(owner.timer_12400,-14,&rate);}
        else recovered::timer_tick(owner.timer_12400,&rate);
    }
    if(owner.timer_12410.current>=0){
        if(owner.timer_12410.current>=119){if(!env.held(slot,1)||(owner.field_1255c&4u))recovered::timer_set(owner.timer_12410,-1);else recovered::timer_add(owner.timer_12410,-119,&rate);}
        else recovered::timer_tick(owner.timer_12410,&rate);
    }
    return 0;
}
void update_shot_controller(ShotController& owner,ShotControllerServices& env){
    owner.field_1255c&=~2u;auto& frames=env.frames();auto& player=*static_cast<Player*>(owner.context->objects_04[0]);
    if(env.world_allows_shooting()&&owner.timer_12420.current>=20&&!(player.entity_flags&4u)&&((player.entity_flags>>6)&3u)==0&&!(player.entity_flags&16u))update_shooting(owner,env);
    else{recovered::timer_set(owner.timer_12400,-1);recovered::timer_set(owner.timer_12410,-1);owner.field_12578=0;owner.byte_1257c=0;frames.callbacks().stop_sound(30);frames.callbacks().stop_sound(55);}
    env.activate_weapons(*owner.context,owner.timer_12400.current==-1);
    std::uint32_t count=0;for(scheduler::Iterator iterator(owner.active.sentinel.next);iterator.current;iterator.advance()){update_shot(*reinterpret_cast<Shot*>(iterator.current->value),frames);++count;}
    owner.field_12464=count;owner.field_1255c&=~1u;
    const float rate=frames.callbacks().firing().clock_rate();recovered::timer_tick(owner.timer_12420,&rate);recovered::timer_tick(owner.timer_12580,&rate);
}
}
