#include "RoaringSystem.hpp"
#include <cmath>
namespace th17 {
RoaringSystem::~RoaringSystem(){for(u32* h:{&visuals.hud,&visuals.outer_timer,&visuals.inner_timer,&visuals.mode_effect,&visuals.break_effect,&visuals.animal_effect,&visuals.reward_effect})animations.retire(*h);for(auto& h:visuals.otter_orbs)animations.retire(h);}
bool RoaringSystem::interrupt(u32 handle,i32 label,bool immediate){if(animations.interrupt(handle,label,immediate))return true;error=animations.error;return false;}
bool RoaringSystem::create(u32& handle,i32 script,const Vec3& p){handle=animations.create(resource,script,-1,0,p);if(handle)return true;error=animations.error;return false;}
bool RoaringSystem::initialize_hud(){return create(visuals.hud,13);}
bool RoaringSystem::enter_stage(){
    if(!visuals.hud){if(!create(visuals.hud,29)||!interrupt(visuals.hud,34,true))return false;for(i32 i=0;i<5;i++)if(!display_slot(i))return false;}
    state.flags&=~0x201u;if(!(state.flags&2))return true;
    if(!animations.retire(visuals.outer_timer)||!create(visuals.outer_timer,30)||!animations.retire(visuals.inner_timer)||!create(visuals.inner_timer,31)||!scale_timers()){if(error.empty())error=animations.error;return false;}
    if(!animations.retire(visuals.animal_effect))return false;
    if(state.kind!=4&&!create(visuals.animal_effect,state.kind+40,player.position))return false;
    return animations.retire(visuals.mode_effect)&&create(visuals.mode_effect,32,player.position);
}
bool RoaringSystem::display_slot(i32 index){
    auto* root=animations.registry.find(visuals.hud);if(!root){visuals.hud=0;return true;}
    auto* icon=animations.registry.find_child_script(*root,index+14,0);auto* border=animations.registry.find_child_script(*root,index+24,0);if(!icon)return true;if(!border){error="Animal inventory border animation missing";return false;}
    const auto send=[&](AnmVm& vm,i32 label){return interrupt(animations.registry.handle(vm),label,true);};
    if(index<state.count)return send(*icon,4)&&send(*icon,state.inventory[u32(index)]+6)&&send(*border,4);
    return send(*icon,5)&&send(*border,5);
}
bool RoaringSystem::create_orbs(){for(u32 i=0;i<3;i++){if(!animations.retire(visuals.otter_orbs[i])||!create(visuals.otter_orbs[i],i32(i)+37,player.position)){error=animations.error;return false;}}return true;}
bool RoaringSystem::scale_timers(){
    auto* outer=animations.registry.find(visuals.outer_timer);auto* inner=animations.registry.find(visuals.inner_timer);if(!outer||!inner){error="Roaring timer animation missing";return false;}
    outer->visual.flags|=8;outer->visual.sprite_size.x=float(float(state.duration)*.125f+2.f);inner->visual.flags|=8;inner->visual.sprite_size.x=float(state.countdown.fractional*.125f);return true;
}
bool RoaringSystem::activate(){
    i32 mode=1,matched=0,occurrences=0;
    for(;mode<4;mode++){occurrences=0;for(i32 value:state.inventory)occurrences+=value==mode;if(occurrences>2)break;matched++;}
    if(occurrences<3){state.kind=4;state.duration=1140;}else{state.kind=matched+1;constexpr i32 durations[]={0,0,0,840,1140,1500};state.duration=durations[occurrences];if(matched==session.animal)state.duration=wrapping_add(state.duration,480);}
    state.countdown.set(state.duration);if(!interrupt(visuals.hud,matched+28,true))return false;state.flags|=2;state.extension=180;
    if(!animations.retire(visuals.outer_timer)||!create(visuals.outer_timer,30)||!animations.retire(visuals.inner_timer)||!create(visuals.inner_timer,31)||!scale_timers()){if(error.empty())error=animations.error;return false;}
    if(state.kind==2){if(!create_orbs())return false;}else player.behavior_flags|=0x40;
    if(state.kind!=4&&!create(visuals.animal_effect,state.kind+40,player.position))return false;
    if(!animations.retire(visuals.mode_effect)||!create(visuals.mode_effect,32,player.position)){if(error.empty())error=animations.error;return false;}
    if(!host.sound(77,true)){error="Roaring start audio failed";return false;}
    state.flags&=~0xf0u;for(i32 value:state.inventory)if(value>=8&&value<=14)state.flags=(state.flags&~0xf0u)|(u32(value-7)<<4);state.reserve_count=0;return true;
}
bool RoaringSystem::collect(i32 kind){
    error.clear();if(kind<1||kind>14||state.count<0||state.count>5||state.reserve_count<0||state.reserve_count>5){error="Invalid animal inventory state";return false;}
    if(state.count<5){const i32 slot=state.count;state.inventory[u32(slot)]=kind;state.count++;if(!display_slot(slot))return false;if(state.count>4)return activate();}
    else if(!(state.flags&4)){if(kind>=4&&kind<=7){Vec3 primary=player.position,scatter=player.position;primary.y=float(primary.y-80.f);scatter.y=float(scatter.y-80.f);if(!item_reward(kind,primary,scatter))return false;}
        const float rate=animations.rate;state.countdown.add(rate>.99f&&rate<1.01f?float(state.extension):float(float(state.extension)*rate));if(state.extension>30)state.extension=wrapping_mul(state.extension,2)/3;if(state.countdown.fractional>float(state.duration))state.countdown.set(state.duration);}
    else if(state.reserve_count<5)state.reserve[u32(state.reserve_count++)]=kind;return true;
}
bool RoaringSystem::break_roaring(bool death){
    error.clear();if(!create(visuals.break_effect,33,player.position)||!host.sound(44,true)||!interrupt(visuals.mode_effect,2)||!interrupt(visuals.outer_timer,1)||!interrupt(visuals.inner_timer,1)||!interrupt(visuals.animal_effect,1))return false;
    visuals.outer_timer=visuals.inner_timer=visuals.animal_effect=0;if(state.kind==2)for(auto& h:visuals.otter_orbs){if(!interrupt(h,1))return false;h=0;}
    state.flags=(state.flags&~8u)|(death?8u:0u)|4;state.countdown.set(40);visuals.damage_source=damage.circle(player.position,8,0,40,5);if(state.kind==3)player.behavior_flags|=0x40;
    if(!(state.flags&0x800)&&!death&&(spell.flags&1)){if(spell.frame>=60){spell.bonus=0;spell.flags&=~0x22u;}else if(session.bomb_state==1)spell.flags|=0x20;}
    invulnerability.set(60);break_count=wrapping_add(break_count,1);break_chain=0;return true;
}
bool RoaringSystem::item_reward(i32 kind,Vec3& primary,Vec3& scatter){
    if(kind==4||kind==5){if(!host.item(kind==4?6:4,primary,-1.5707963705062866f,2.2f,10,-1)){error="Roaring fragment reward failed";return false;}primary.y=float(primary.y-30.f);}
    else if(kind==6||kind==7){for(i32 i=0;i<30;i++){const float radius=float(i*2),angle=float(double(random.signed_unit())*double(3.1415927410125732421875f));Vec3 p={float(scatter.x+float(std::cos(double(angle))*double(radius))),float(scatter.y+float(std::sin(double(angle))*double(float(radius*.5f)))),0};if(!host.item(kind==6?1:2,p,-1.5707963705062866f,2.2f,10+i*2,62)){error="Roaring scattered reward failed";return false;}}scatter.y=float(scatter.y-30.f);}
    return true;
}
bool RoaringSystem::reward_items(){Vec3 primary=player.position,scatter=player.position;primary.y=float(primary.y-80.f);scatter.y=float(scatter.y-80.f);for(i32 kind:state.inventory)if(!item_reward(kind,primary,scatter))return false;return true;}
bool RoaringSystem::update_rewards(float rate){
    if(auto* vm=animations.registry.find(visuals.reward_effect))vm->visual.translation={float(player.position.x*2.f),float(player.position.y*2.f),float(player.position.z*2.f)};
    state.reward_age.tick(&rate);if(state.flags&0x200)state.reward_age.decrement(&rate);if(state.reward_age.current<120)return true;
    if(!animations.retire(visuals.reward_effect)){error=animations.error;return false;}
    Vec3 p=player.position;p.y=float(p.y-64.f);std::array<u32,5> angles;
    if(p.y<96.f){
        if(p.x<-80.f)angles={0x3f060a92,0x3f490fdb,0x3f29c91f,0x3f860a92,0x3f685696};
        else if(p.x>80.f)angles={0x40278d36,0x4016cbe4,0x401e9d93,0x40060a92,0x400efa36};
        else angles={0x3f060a92,0x4016cbe4,0x3f5f66f4,0x40278d36,0x400074cc};
    }else{
        if(p.x<-80.f)angles={0xbf060a92,0xbf490fdb,0xbf29c91f,0xbf860a92,0xbf685696};
        else if(p.x>80.f)angles={0x406a9280,0x407b53d2,0x40738223,0x40860a92,0x408192c0};
        else angles={0xc0278d36,0xbf490fdb,0xc011361e,0xbf060a92,0xbf91361e};
    }
    if(!host.sound(62,false)){error="Roaring reward audio failed";return false;}
    const u32 rare=state.flags>>4&15;const auto rotating=[&](){return i32(random.next32()%3)+15;};
    const auto spawn=[&](i32 kind,u32 angle){if(host.spirit(kind,p,float_from_bits(angles[angle])))return true;error="Roaring animal reward failed";return false;};
    if(rare==0)return spawn(rotating(),0)&&spawn(rotating(),3);
    if(rare==3){if(!spawn(6,0)||!spawn(7,3))return false;}
    else if(!spawn(rotating(),0)||!spawn(rotating(),3))return false;
    p.y=float(p.y-32.f);if(!spawn(5,2)||!spawn(rare==2||rare==3?4:rotating(),1))return false;
    return spawn(rare==3?4:rotating(),4);
}
bool RoaringSystem::finish(){
    if(!(state.flags&4)){
        if(!animations.retire(visuals.reward_effect)||!create(visuals.reward_effect,44,{float(player.position.x*2.f),float(player.position.y*2.f),float(player.position.z*2.f)})){if(error.empty())error=animations.error;return false;}
        state.reward_age.set(0);
    }
    if(state.kind==2)for(auto& h:visuals.otter_orbs){if(!interrupt(h,1))return false;h=0;}
    if(!host.sound(66,true)||!interrupt(visuals.mode_effect,1))return false;visuals.mode_effect=0;
    if(!reward_items())return false;state.flags&=~6u;
    if(!interrupt(visuals.outer_timer,1)||!interrupt(visuals.inner_timer,1)||!interrupt(visuals.animal_effect,1))return false;
    visuals.outer_timer=visuals.inner_timer=visuals.break_effect=visuals.animal_effect=0;if(state.kind==3)player.behavior_flags|=0x40;
    state.count=0;for(i32 i=0;i<5;i++){state.inventory[u32(i)]=0;if(!display_slot(i))return false;}
    for(i32 i=0;i<state.reserve_count;i++){if(!collect(state.reserve[u32(i)]))return false;state.reserve[u32(i)]=0;if(!display_slot(i))return false;}state.reserve_count=0;return true;
}
bool RoaringSystem::update(float rate,bool hud_collect,u32 held){
    error.clear();animations.rate=rate;
    if(!(state.flags&1)){if(player.position.y>400.f&&player.position.x<-64.f){if(!interrupt(visuals.hud,3))return false;state.flags|=1;}}
    else if(player.position.y<384.f||player.position.x>-64.f){if(!interrupt(visuals.hud,2))return false;state.flags&=~1u;}
    if(visuals.reward_effect&&!update_rewards(rate))return false;if(!(state.flags&2))return true;
    const auto pause=[&](auto&& self,AnmVm& vm,bool paused)->void{
        vm.visual.flags=paused?(vm.visual.flags&~2u)|0x100000:(vm.visual.flags&~0x100000u)|2;
        for(auto* child:animations.registry.children(vm))self(self,*child,paused);
    };
    if((!hud_collect&&!(state.flags&0x200))||(state.flags&4)){
        state.countdown.decrement(&rate);state.flags&=~0x400u;
        if(host.active_player_shot()||visuals.hit_delay){state.countdown.decrement(&rate);state.flags|=0x400;}
        if(state.flags&0x400){if(auto* vm=animations.registry.find(visuals.inner_timer)){if(animations.tick_instance(*vm)<0){error=animations.error;return false;}}else visuals.inner_timer=0;}
        if(state.flags&0x100){if(auto* vm=animations.registry.find(visuals.mode_effect))pause(pause,*vm,false);state.flags&=~0x100u;if(state.kind==2&&!create_orbs())return false;}
    }else{
        if(!(state.flags&0x100)&&state.kind==2)for(auto& h:visuals.otter_orbs){if(!interrupt(h,1))return false;h=0;}
        if(auto* vm=animations.registry.find(visuals.mode_effect))pause(pause,*vm,true);state.flags|=0x100;
    }
    if(visuals.hit_delay)visuals.hit_delay=wrapping_add(visuals.hit_delay,-1);
    if(!(state.flags&4)){
        const Vec3 scaled{float(std::floor(double(float(player.position.x*2.f)))),float(std::floor(double(float(player.position.y*2.f)))),float(player.position.z*2.f)};
        for(u32 handle:{visuals.outer_timer,visuals.inner_timer})if(auto* vm=animations.registry.find(handle))vm->visual.translation=scaled;
        for(u32 handle:{visuals.mode_effect,visuals.animal_effect})if(auto* vm=animations.registry.find(handle))vm->visual.translation=player.position;
        if(!(state.flags&0x100)){
            if(state.kind==2){
                float z=scaled.z;for(auto& h:visuals.otter_orbs){auto* vm=animations.registry.find(h);if(!vm){h=0;continue;}vm->visual.translation=player.position;
                    const float angle=float(vm->variables.rotation.z-.34906584f);z=float(player.position.z+z);
                    const Vec3 p{float(float(std::cos(double(angle))*96.0)+player.position.x),float(player.position.y+float(std::sin(double(angle))*96.0)),z};
                    if(!host.cancel_bullets(p,28.f,1)||!host.cancel_lasers(p,28.f,1,true)){error="Otter barrier cancellation failed";return false;}
                    auto* source=damage.find(damage.circle(p,28.f,0.f,2,5));if(!source){error="Otter barrier damage source unavailable";return false;}source->callback_kind=4;
                }
            }
            if((held&2)&&!break_roaring(false))return false;
        }
    }
    if(state.countdown.current<1)return finish();
    if(!(state.flags&4))return scale_timers();
    if(auto* source=damage.find(visuals.damage_source)){
        const float t=float(1.f-float(1.f-float(state.countdown.fractional/40.f)));
        source->radius=float(float(1.f-float(float(float(t*t)*t)*t))*200.f);const i32 reward=i32(~(state.flags>>3)&1);
        if(!host.cancel_bullets(source->motion.position,source->radius,reward)||!host.cancel_lasers(source->motion.position,source->radius,reward,true)){error="Roaring break cancellation failed";return false;}
    }
    return true;
}
}
