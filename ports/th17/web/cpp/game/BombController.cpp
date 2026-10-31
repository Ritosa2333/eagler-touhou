#include "BombController.hpp"
#include <algorithm>
namespace th17 {
bool BombController::fail(const std::string& message){if(error.empty())error=message;return false;}
bool BombController::sound(i32 id,bool queued){return context.world.sound(id,queued)||fail("Bomb audio unavailable");}
bool BombController::interrupt(u32 handle,i32 label){return context.animations.interrupt(handle,label)||fail(context.animations.error);}
u32 BombController::animation(i32 script,const Vec3& p){const auto h=context.animations.create(context.player_resource,script,-1,0,p);if(!h)fail(context.animations.error);return h;}
void BombController::invalidate_spell()noexcept{auto& s=context.spell;if(!(s.flags&1))return;if(s.frame<60){if(context.session.bomb_state==1)s.flags|=32;}else{s.bonus=0;s.flags&=~34u;}}
DamageSource* BombController::circle(const Vec3& p,float radius,float growth,i32 frames,i32 amount,i32* id){const i32 value=context.damage.circle(p,radius,growth,frames,amount);if(id)*id=value;auto* source=context.damage.find(value);if(!source){fail("Bomb circle allocation failed");return nullptr;}source->flags|=4;return source;}
DamageSource* BombController::rectangle(const Vec3& p,float angle,const Vec2& size,i32 frames,i32 amount){auto* source=context.damage.find(context.damage.rectangle(p,angle,size,frames,amount));if(!source){fail("Bomb rectangle allocation failed");return nullptr;}source->flags|=4;return source;}
bool BombController::allowed()const noexcept{return context.session.bomb_state!=1&&context.roaring_available&&!(context.roaring_flags&6)&&context.hud_available&&!context.hud_collect&&context.enemy_available&&context.enemy_count!=0;}
bool BombController::begin(){
    if(context.session.bomb_state!=0)return false;context.session.bomb_state=1;age.set(0);
    auto& s=context.session;s.bombs=std::clamp(wrapping_sub(s.bombs,1),0,8);
    if(context.hud_available&&!context.world.bomb_hud(s.bombs,s.bomb_pieces))return fail("Bomb HUD unavailable");
    if(context.spell.flags&1){if(context.spell.frame>=60)failed_spell=1;if(context.spell.id==100&&!context.world.cancel_bullets(context.motion.position,192,0,false))return fail("Bomb activation cancellation failed");}
    if(!sound(44)||!start())return false;s.enemy_chain=0;return true;
}
bool BombController::update(float frame_rate){rate=frame_rate;context.animations.rate=rate;if(age.current<0)age.tick(&rate);if(context.session.bomb_state==0)return true;bool finished=false;if(!frame(finished))return false;if(finished)context.session.bomb_state=0;else age.tick(&rate);return true;}
}
