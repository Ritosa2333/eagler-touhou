#include "BulletState.hpp"
namespace th15 {
namespace {
float reward_angle(Rng& random){return float(float(random.signed_unit()*.1745329201221466064453125f)-1.57079637050628662109375f);}
}
void cancellation_reward(const Vec3& position,i32 kind,Rng& random,BulletCancellationRewards& rewards,CancellationRewardHost& host){
    if(!kind||float(position.x+32.f)<=-192.f||192.f<=float(position.x-32.f)||float(position.y+32.f)<=0.f||448.f<=float(position.y-32.f))return;
    rewards.count=wrapping_add(rewards.count,1);
    if(kind==3)host.item(10,position,reward_angle(random),2.2f);
    else if(kind==2){if(rewards.count%5==0&&!rewards.spell_active)host.item(1,position,-1.57079637050628662109375f,2.2f);if(rewards.spell_active)return;}
    else if(kind!=1)return;
    host.item(10,position,reward_angle(random),2.2f);
}
bool BulletState::cancel(i32 kind,float rate,Rng& random,BulletCancellationRewards& rewards,BulletFrameHost& frame_host,BulletContactHost& contact_host,std::string& error){
    if(!contact_host.hit_animation()){error="Bullet cancel animation callback unavailable";return false;}hit_interrupt=1;frame_host.animation_finished(false);
    if(overlay_active&&!contact_host.overlay_interrupt(1)){error="Bullet cancel overlay callback unavailable";return false;}
    if(!(flags&512)){
        if(cancellation_script>=0)contact_host.cancellation_effect(cancellation_script,motion.position,{});
        contact_host.play(71);cancellation_reward(motion.position,kind,random,rewards,contact_host);
    }
    motion.position={float(motion.position.x+float(float(motion.velocity.x*rate)*.5f)),float(motion.position.y+float(float(motion.velocity.y*rate)*.5f)),float(motion.position.z+float(float(motion.velocity.z*rate)*.5f))};phase=4;spawn.set(0);return true;
}
}
