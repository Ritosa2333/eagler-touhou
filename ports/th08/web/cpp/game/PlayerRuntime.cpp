#include "PlayerRuntime.hpp"
#include <cmath>

namespace th08 {
void PlayerRuntime::sync_player() noexcept {auto& p=combat.player_state();p.position=state.position;p.focused=state.focused;p.alive=state.alive!=0;p.hitbox=2.5f;p.grazebox=state.focused?24.0f:28.0f;}
void PlayerRuntime::reset(PlayerTeam team) noexcept {state=PlayerRuntimeState{};state.team=team;input=InputFrame{};combat.player_state().position=state.position;sync_player();}
void PlayerRuntime::shoot() noexcept {
    const u32 count=1+(state.power>=32)+(state.power>=64)+(state.power>=96)+(state.power>=128);const float spread=0.08f;
    for(u32 i=0;i<count;++i){const float center=(float(count)-1)*0.5f,angle=-1.57079632679f+(float(i)-center)*spread;Vec3 position={state.position.x+(float(i)-center)*4,state.position.y-8,0};combat.spawn_bullet(u8(0x40+u8(state.team)),position,angle,11.0f,0);}
    state.shot_cooldown=5;
}
void PlayerRuntime::damage() noexcept {if(!state.alive||state.invincible)return;state.alive=0;++state.deaths;if(state.lives>0)--state.lives;state.invincible=120;state.position={128,400,0};if(state.lives==0)state.alive=0;sync_player();}
void PlayerRuntime::bomb() noexcept {if(!state.alive||!state.bombs||state.invincible)return;--state.bombs;++state.bombs_used;state.invincible=180;combat.cancel_bullets();sync_player();}
void PlayerRuntime::step(u16 buttons,float rate) noexcept {
    input.update(buttons);const float step=rate<=0?1:rate;state.focused=(input.current&InputButton::Focus)!=0;
    const float speed=state.focused?focus_speed:normal_speed;float dx=0,dy=0;if(input.current&InputButton::Left)dx-=speed;if(input.current&InputButton::Right)dx+=speed;if(input.current&InputButton::Up)dy-=speed;if(input.current&InputButton::Down)dy+=speed;
    state.position.x+=dx*step;state.position.y+=dy*step;if(state.position.x<16)state.position.x=16;if(state.position.x>240)state.position.x=240;if(state.position.y<16)state.position.y=16;if(state.position.y>464)state.position.y=464;
    if(state.invincible>0)state.invincible-=u32(state.invincible<step?state.invincible:step);if(state.shot_cooldown>0)state.shot_cooldown-=u32(state.shot_cooldown<step?state.shot_cooldown:step);
    if(input.current&InputButton::Bomb&&input.pressed(InputButton::Bomb))bomb();if(state.alive&&(input.current&InputButton::Shoot)&&state.shot_cooldown==0)shoot();sync_player();
}
}
