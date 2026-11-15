#include "PlayerTh16Life.hpp"
#include "PlayerFrame.hpp"
#include <algorithm>
namespace th11 {
bool player_th16_hit(PlayerFrameState& state,PlayerSpellState& spell,Vec3 p,u32 flags,bool bomb,const float* rate,Th16PlayerHitServices& services){
 // Complete443f10. ANM construction/body binding/tick and audio are services.
 if(!(flags&8)&&!services.sound(2))return false;if(!services.effect(p,29))return false;
 if(spell.flags&1){if(spell.elapsed>=60){spell.bonus=0;spell.flags&=~0x22u;}else if(bomb)spell.flags|=0x20;}
 state.state_timer.set(0,rate);state.life_state=4;state.invincibility.set(6,rate);return services.reset_body();
}
bool player_th16_die(PlayerFrameState& state,PlayerSpellState& spell,GameEconomy& economy,PlayerOption* options,i32& main_count,Vec3 p,bool bomb,const float* rate,Th16PlayerDeathServices& services){
#if PORT_TITLE == 16
 // Complete443cd0. The shared effect-slot chooser, HUD and ANM are services.
 if(!services.effect(p,28))return false;economy.lives=wrapping_sub(economy.lives,1);economy.th16_bombs=3;
 if(!services.bombs(3,economy.th16_bomb_fragments))return false;
 if(economy.lives>=0&&!services.lives(economy.lives,economy.life_fragments))return false;
 if(!services.bombs(3,economy.th16_bomb_fragments))return false;
 state.life_state=2;state.state_timer.set(0,rate);state.invincibility.set(180,rate);if(!services.reset_body())return false;
 for(u32 i=0;i<4;++i){auto& option=options[i];option.state=0;if(!services.interrupt(option.animation,1)||!services.interrupt(option.auxiliary,1))return false;}main_count=0;
 if(spell.flags&1){if(spell.elapsed>=60){spell.bonus=0;spell.flags&=~0x22u;}else if(bomb)spell.flags|=0x20;}
 if(!services.enemy_death())return false;if(economy.th16_death_count<999999)++economy.th16_death_count;return true;
#else
 (void)state;(void)spell;(void)economy;(void)options;(void)main_count;(void)p;(void)bomb;(void)rate;(void)services;return false;
#endif
}
bool player_th16_death_drops(GameEconomy& economy,Vec3 p,PlayerFrameWorld& world){
 // Retail442724..442892: seven ordinary small-power drops, regardless of lives.
 economy.power=std::max(wrapping_sub(economy.power,economy.power_step/2),economy.power_step);
 const float dx=float(0.f-p.x),dy=float(float(p.y-224.f)-p.y);
 const float angle=dx==0&&dy==0?1.5707963705062866f:float(std::atan2(double(dy),double(dx)));
 for(i32 i=0;i<7;++i){const float offset=float(float(float(i)*3.1415927410125732f)/28.f);const float a=float(float(offset+angle)-.39269909262657166f);if(!world.spawn_item(1,p,0xffffff,a,3.f))return false;}return true;
}
bool player_th16_respawn_frame(PlayerFrameState& state,PlayerMotionState& motion,PlayerOption* options,PlayerFrameWorld& world){
 // Retail4425a3..442685: four main options, lasers before frame30, then
 // the own bullet circle followed by the same laser circle.
 const i32 age=state.state_timer.current;
 motion.y=wrapping_sub(0xf000,signed_bits(u32(age)*0x2800u)/60);
 motion.position.y=float(float(motion.y)*.0078125f);
 for(u32 n=0;n<4;++n)options[n].snap=1;
 if(age<30){const float radius=float(float(float(age)*512.f)/30.f)+64.f;
  return world.cancel_lasers(&state.death_position,radius,false,true)&&world.cancel_lasers(&state.death_position,float(radius*.25f),false,false);
 }
 return world.cancel_bullets(&motion.position,640.f,false)&&world.cancel_lasers(&motion.position,640.f,false,false);
}
}
