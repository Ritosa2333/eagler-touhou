#include "Player.hpp"
namespace th15 {
bool Player::finish_stage_options(){
    if(!initialized||!error.empty()){if(error.empty())error="Player options unavailable at stage completion";return false;}
    motion.behavior_flags|=2;
    for(const auto& pair:visuals.option_handles)for(auto handle:pair)if(!animations.interrupt(handle,3)){error=animations.error;return false;}
    motion.collapse_frame=0;return true;
}
Player::Player(const ShtResource& s,AnmManager& a,EffectManager& e,Rng& game,Rng& visual,PlayerLifeSession& state,PlayerSpellStatus& b,PlayerHost& w,i32 ch,i32 id,i32 effect):world(w),animations(a),character(ch),resource(s),visuals(a,id,effect),effects(e),session(state),spell(b),shot_context{motion},shots(resource,shot_context,damage,a,game,visual,id,effect),options(resource,motion,visuals,a,id,ch),life(motion,visuals,effects,state,b),frame(motion,visuals,a,life,damage,shots,shot_context,*this){
    shots.sound=[this](i32 id,float pan,PlayerShots::SoundAction action){return world.sound(id,pan,action);};shots.item=[this](i32 kind,const Vec3& p,float angle,float speed){return world.item(kind,p,angle,speed);};
    life.sound=[this](i32 id){return world.hit_sound(id);};life.life_hud=[this](i32 lives,i32 pieces){return world.life_hud(lives,pieces);};
    shots.track_effect=[this](u32 handle){effects.track(handle);};
}
bool Player::initialize(){
    if(initialized){error="Player already initialized";return false;}if(character<0||character>3||resource.groups.size()!=10){error="Invalid player resource or character";return false;}
    constexpr float hitbox[]={2,3,3,3},attraction[]={5,6,5,5};resource.header.power_step=100;resource.header.hitbox=hitbox[character];resource.header.attraction_diameter=60;resource.header.attraction_speed=attraction[character];session.power_step=100;motion.configure(resource.header);
    if(!visuals.initialize()){error=animations.error;return false;}motion.position_fixed={0,51200};motion.position={0,400,0};motion.snap_options();motion.option_follow_percentage=30;motion.enlargement=motion.movement_scale=1;
    for(auto& option:motion.options)option.position.y=-51200;motion.barrier_timer={0,0,0,0,0};life.age={0,0,0,0,0};life.age.set(0);life.invulnerability={0,0,0,0,0};life.invulnerability.set(0);frame.input_age={0,0,0,0,0};frame.ready_age={0,0,0,0,0};frame.ready_age.set(0);
    for(auto& source:damage.sources)source.lifetime={0,0,0,0,0};for(auto& shot:shots.shots){shot.age=shot.behavior=shot.auxiliary={0,0,0,0,0};}
    auto& b=frame.bounds;b.hit_half_size={float(resource.header.hitbox*.5f),float(resource.header.hitbox*.5f),5};b.item_half_size={30,30,5};b.graze_half_size={50,50,5};const auto& p=motion.position;
    b.hit={{float(p.x-b.hit_half_size.x),float(p.y-b.hit_half_size.y),float(p.z-5)},{float(p.x+b.hit_half_size.x),float(p.y+b.hit_half_size.y),float(p.z+5)}};
    b.item_near={{float(p.x-30),float(p.y-30),float(p.z-5)},{float(p.x+30),float(p.y+30),float(p.z+5)}};b.graze={{float(p.x-50),float(p.y-50),float(p.z-5)},{float(p.x+50),float(p.y+50),float(p.z+5)}};b.item_full=b.graze;frame.collision.radius=resource.header.hitbox;initialized=true;return true;
}
bool Player::update(const PlayerFrameContext& context,float& rate){if(!initialized){error="Player not initialized";return false;}if(!error.empty())return false;if(!frame.update(context,rate)){error=frame.error;return false;}return true;}
bool Player::reset_for_stage(){
    if(!initialized){error="Player not initialized";return false;}if(!error.empty())return false;
    life.state=1;frame.shooting.shot.set(-1);frame.shooting.continuous.set(-1);life.age.set(0);frame.input_age.set(0);frame.ready_age.set(0);
    if(character==3)resource.header.hitbox=3;motion.behavior_flags&=~9u;
    if(!animations.retire(visuals.focus_handle)||!animations.retire(visuals.barrier_handle)){error=animations.error;return false;}
    if(!world.life_hud(session.extra_lives,session.life_pieces)){error="Stage life HUD reset failed";return false;}
    motion.behavior_flags&=~2u;for(const auto& pair:visuals.option_handles)for(auto handle:pair)if(!animations.interrupt(handle,2)){error=animations.error;return false;}motion.collapse_frame=0;
    if(!configure_options()){error=options.error;return false;}motion.behavior_flags&=~4u;motion.movement_scale=1;
    for(u32 i=1;i<5;i++)shot_context.laser_power[i]=0;shots.reward_counters={};damage.cursor=0;frame.bounds.enlargement.duration=0;motion.enlargement=1;return true;
}
}
