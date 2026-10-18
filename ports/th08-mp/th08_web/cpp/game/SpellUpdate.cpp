#include "SpellSystem.hpp"
#include "GameMath.hpp"
namespace th08 {
namespace {
void interpolate(AnmVm& vm,u32 kind,i32 duration,u8 mode){vm.interpCurrentTimers[kind].set(0);vm.interpEndTimers[kind].set(duration);vm.interpModes[kind]=mode;}
Vec3 sum(const Vec3& a,const Vec3& b){return {Scalar::add(a.x,b.x),Scalar::add(a.y,b.y),Scalar::add(a.z,b.z)};}
void approach(Vec3& current,const Vec3& target,float divisor){
    const float inverse=Scalar::div(1,divisor);
    const auto step=[&](float p,float q){const float difference=Scalar::sub(q,p),delta=Scalar::mul(inverse,difference);return Scalar::add(delta,p);};
    current={step(current.x,target.x),step(current.y,target.y),0};
}
}
bool SpellSystem::update(){
    auto& s=globals;if((s.game_flags&1024)||s.paused)return true;
    if(s.spell_flags&1){
        auto* enemy=s.spell_enemy;if(!enemy)return false;
        if(!(enemy->flags&1)||s.spell_enemy_index!=u32(enemy->pool_index)){s.spell_flags&=~1u;presentation.end_enemy();}
        u8 alpha=u8(s.spell_panel_color>>24);
        if(s.player.x>=64&&s.player.y<64){if(alpha>32)alpha=u8(alpha-4);}else if(alpha<128)alpha=u8(alpha+4);
        s.spell_panel_color=(s.spell_panel_color&0xffffff)|(u32(alpha)<<24);
        auto* effect=s.spell_effect;if(!effect)return false;
        if(s.spell_flags&4){
            if(!(s.spell_flags&2048)&&!(enemy->flags&0x8000000)){
                s.spell_bonus-=u32((Extended::from_int64(s.spell_bonus_decay/60)*number(anm.timing.rate)).truncate_int());s.spell_bonus-=s.spell_bonus%10;
            }
        }else if(effect->activeSpriteIndex==221){
            if(!effects.state.base_animation||effects.state.base_animation->SetSprite(effect,222))return false;effect->scaleFinal.x=4;effect->scale.x=4;
        }
        if(effect->frequency!=0){effect->height=effect->pos.y;if(effect->height==0)effect->frequency=0;}
        if(effect->interpEndTimers[AnmInterp_Pos].current==0){interpolate(*effect,AnmInterp_Pos,wrapping_sub(enemy->timeout,100),0);effect->posInitial.x=256;effect->posFinal.x=8;effect->posInitial.y=effect->posFinal.y=0;}
        if(!(s.spell_flags&64))approach(effect->center,sum(enemy->position,enemy->position_offset),16);
        effect->angle=add_angle(effect->angle,(s.spell_flags&1)&&(s.spell_flags&32)?-.031415928f:.015707964f);
    }else if(auto* effect=s.spell_reward_effect){
        if(effect->age.current==30){
            interpolate(*effect,AnmInterp_Scale,20,1);effect->scaleInitial.x=effect->width;effect->scaleFinal.x=64;
            interpolate(*effect,AnmInterp_Pos,100,4);effect->posInitial.x=effect->radius;effect->posFinal.x=0;effect->posInitial.y=effect->height;effect->posFinal.y=60;effect->pos.x=effect->radius;effect->pos.y=effect->height;
        }else if(effect->age.current==60){
            interpolate(*effect,AnmInterp_Scale,70,1);effect->scaleInitial.x=effect->width;effect->scaleFinal.x=0;
        }else if(effect->age.current==130){effect->active=0;if(!reward(true))return false;}
        if(s.spell_reward_effect){
            const bool slow=effect->age.current<=80;approach(effect->center,s.player,slow?16:4);effect->angle=add_angle(effect->angle,slow?-.015707964f:-.05235988f);effect->height=effect->pos.y;
            if(effect->age.current>8&&s.spell_time_items>0){
                float angle=((effect->age.value()-number(10))*number(6.2831855f)/number(40)-number(1.5707964f)).to_float();angle=add_angle(angle,0);
                const auto at=[&](float a){Vec3 p{(cosine(a)*number(128)).to_float(),(sine(a)*number(128)).to_float(),0};p=sum(p,effect->center);p.z=0;return p;};
                auto position=at(angle);i32 count=s.spell_time_items<8?s.spell_time_items:7;
                for(i32 i=0;i<count;i++)actions.item(position,10,0);s.spell_time_items=wrapping_sub(s.spell_time_items,count);
                angle=add_angle(angle,3.1415927f);position=at(angle);count=s.spell_time_items<8?s.spell_time_items:7;
                // The original emits six from the second side even when the
                // remaining counter is smaller; retain this replay-visible quirk.
                for(i32 i=0;i<6;i++)actions.item(position,10,0);s.spell_time_items=wrapping_sub(s.spell_time_items,count);if(s.spell_time_items<0)s.spell_time_items=0;
            }
        }
    }
    for(i32 slot:{0,2,4,6,10,1,3,5,7,8,9,11,13})anm.execute(s.spell_vms[slot]);
    s.spell_remaining.decrement(1,anm.timing);return !anm.invalid;
}
}
