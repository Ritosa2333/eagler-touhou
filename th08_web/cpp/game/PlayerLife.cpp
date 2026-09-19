#include "PlayerLife.hpp"
namespace th08 {
void PlayerLife::die(){
    // th08_dissolve @ 0044abe9: count every entry into the deathbomb window,
    // including hits later rescued by a deathbomb.
    actions.dissolve();
    context.pause=0;actions.update_integrity();actions.effect(6,movement.position,16,0xffffffff);
    state.state=2;state.timer.set(0);actions.sound(4,movement.position.x);context.replay_flags|=0x200;
    if(!(context.game_flags&0x180)){
        context.gauge=0;
        if(context.bombs<1)state.predead_count=2;
        else {
            state.predead_count=signed_bits(u32(context.bombs)*6);
            if(context.time_orbs>=context.last_spell_requirement)state.predead_count=wrapping_add(state.predead_count,7);
            if(state.predead_count>15)state.predead_count=15;
            if(context.time_spell){state.predead_count=wrapping_add(state.predead_count,state.predead_count);if(state.predead_count>30)state.predead_count=30;}
            if(context.character==0||context.character==4||context.character==5)state.predead_count=signed_bits(u32(state.predead_count)*9)/5;
            animation.color2.d3dColor=0xffffff|(u32(animation.color1.a)<<24);animation.flag17=1;
            state.predead_effect=actions.fixed_effect(59,movement.position,11,0xfff0404f);
            if(auto* e=state.predead_effect){
                e->interpCurrentTimers[0].set(0);e->interpEndTimers[0].set(state.predead_count);e->interpModes[0]=4;
                e->posInitial.x=128;e->posInitial.y=32;e->posFinal.x=8;e->posFinal.y=0;e->pos.x=128;e->pos.y=32;
                e->segments=64;e->angle=0;e->radius=128;e->width=15;e->frequency=6;e->ignore_pause=1;
            }
            if(context.time_spell)context.game_flags|=0x400;
        }
    }else {state.predead_count=2;state.auto_bomb=1;}
    actions.cancel_item_homing();
}
bool PlayerLife::resolve_death(const ShotProfile& profile){
    if(state.predead_count!=0){
        actions.add_time_orbs(-15);state.predead_count=wrapping_sub(state.predead_count,1);state.deathbomb=1;
        if(state.predead_count==0){
            if(state.predead_effect){state.predead_effect->active=0;state.predead_effect=nullptr;}
            actions.fixed_effect(12,movement.position,3,0xff4040ff);actions.effect(6,movement.position,16,0xffffffff);actions.sound(15,movement.position.x);
            context.game_flags&=~0x400u;actions.reset_screen_color();animation.flag17=0;context.replay_flags|=4;context.miss_control=0;state.deathbomb=0;
            actions.fail_spell();actions.add_deaths(1);context.hud_flags=(context.hud_flags&~0xc00u)|0x800;
            actions.add_time_orbs(context.time_orbs>5000?-500:wrapping_sub(0,context.time_orbs)/10);
            if(context.lives>0){
                if(!(context.cheats&8)){if(context.power<=16)actions.set_power(0);else actions.add_power(-16);}
                actions.item(2,movement.position,2);for(u32 i=0;i<5;++i)actions.item(0,movement.position,2);
                if(context.bombs>0&&(context.character==2||context.character==8||context.character==9))actions.item(3,movement.position,2);
                context.hud_flags=(context.hud_flags&~0x30u)|0x20;actions.cancel_item_homing();
            }else {if(!(context.cheats&8))actions.set_power(0);for(u32 i=0;i<5;++i)actions.item(4,movement.position,2);context.hud_flags=(context.hud_flags&~0x30u)|0x20;}
            actions.subtract_rank(1600);
        }
    }else {
        const float fraction=(state.timer.value()/number(30)).to_float();
        animation.scale.y=(number(3)*number(fraction)+number(1)).to_float();animation.scale.x=(number(1)-number(1)*number(fraction)).to_float();
        const i32 alpha=(number(255)-state.timer.value()*number(255)/number(30)).truncate_int();animation.color1.d3dColor=(u32(alpha)<<24)|0xffffff;
        animation.blendMode=1;movement.delta={};
        if(state.timer.current>=30){
            state.state=1;movement.position={Scalar::div(context.extent.x,2),Scalar::sub(context.extent.y,64),.2f};state.timer.set(0);animation.scale={3,3};
            actions.animation(((context.character<4&&!context.focused)||!(context.character&1))?0:5);
            if(context.lives>0){if(!(context.cheats&2))actions.add_lives(-1);context.hud_flags=(context.hud_flags&~3u)|2;actions.set_bombs(Scalar::truncate(profile.initial_bombs));context.hud_flags=(context.hud_flags&~12u)|8;return true;}
            context.game_over=1;
        }
    }
    return false;
}
void PlayerLife::respawn(const ShotProfile& profile){
    state.clear_frames=60;const float fraction=(number(1)-state.timer.value()/number(30)).to_float();
    animation.scale.y=(number(fraction)*number(2)+number(1)).to_float();animation.scale.x=(number(1)-number(fraction)*number(1)).to_float();
    animation.blendMode=1;movement.multiplier={1,1};animation.color1.d3dColor=(u32(signed_bits(u32(state.timer.current)*255)/30)<<24)|0xffffff;state.predead_count=0;
    if(state.timer.current>=30){state.state=3;animation.scale={1,1};animation.color1.d3dColor=-1;animation.blendMode=0;if(!(context.game_flags&0x4000))state.timer.set(240);state.predead_count=profile.deathbomb_limit;}
}
void PlayerLife::update_invincibility(const FrameTiming& timing){
    if(state.clear_frames!=0){state.clear_frames=wrapping_sub(state.clear_frames,1);actions.cancel_rectangle(movement.position,768,896,-1,0);}
    if(state.state==3){
        state.deathbomb=0;if(state.invincible_effect)state.invincible_effect->position=movement.position;state.timer.decrement(1,timing);
        if(state.timer.current<1){if(state.invincible_effect){state.invincible_effect->active=0;state.invincible_effect=nullptr;}state.state=0;state.timer.set(0);animation.color1.d3dColor=-1;}
        else animation.color1.d3dColor=state.timer.current%8<2?0xfff02020:0xffffffff;
    }else state.timer.tick(timing);
}
}
