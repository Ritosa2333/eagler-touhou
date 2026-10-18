#include "PlayerResources.hpp"
namespace th10 {
static void set_timer(Timer& timer,u32& flags,i32 value,float* rate){if(!(flags&1)){timer.rate=rate;flags|=1;}timer.previous=wrapping_add(value,-1);timer.current=value;timer.fractional=Extended::from_int(value).to_float();}
static void bounds(PlayerBounds& out,const Vec3& position,const Vec3& size){out.minimum={Scalar::sub(position.x,size.x),Scalar::sub(position.y,size.y),Scalar::sub(position.z,size.z)};out.maximum={Scalar::add(size.x,position.x),Scalar::add(size.y,position.y),Scalar::add(size.z,position.z)};}
// 0x4246c0. The player constructor clears the complete object after member setup.
void PlayerResources::initialize() noexcept {std::memset(&player,0,sizeof(player));*environment.current=&player;}
// 0x4247f0. Profile speed conversion precedes collision/pickup overrides.
i32 PlayerResources::start(){
    auto& env=environment;
    player.animation_file=env.load_animations(env.game->character==0?"pl00.anm":"pl01.anm");
    if(!player.animation_file){env.report_error();return -1;}
    if(*env.cached_profile){player.profile=*env.cached_profile;*env.cached_profile=nullptr;}
    else if(env.load_profile(player,env.profile_names[env.game->character*3+env.game->shot_type])){env.report_error();return -1;}
    player.update_entry=(*env.chain)->add(env.update_callback,&player,16,false,false,*env.callbacks);
    player.draw_entry=(*env.chain)->add(env.draw_callback,&player,22,true,false,*env.callbacks);
    env.initialize_animation(player);
    player.position.x=0;player.position.y=400;player.fixed_position={0,40000};
    player.fast_speed=(number(player.profile->fast_speed)*number(100)).truncate_int();player.slow_speed=(number(player.profile->slow_speed)*number(100)).truncate_int();player.fast_diagonal=(number(player.profile->fast_diagonal)*number(100)).truncate_int();player.slow_diagonal=(number(player.profile->slow_diagonal)*number(100)).truncate_int();
    for(auto& point:player.position_history)point=player.fixed_position;
    set_timer(player.fire_timer,player.fire_timer_flags,-1,env.rate);
    const auto character=env.game->character;
    player.profile->collision_size=env.hitbox_sizes[character];player.profile->pickup_size=env.pickup_sizes[character];player.profile->item_attraction_speed=env.attraction_speeds[character];
    float half=Scalar::mul(player.profile->collision_size,.5f);player.hitbox_half_size={half,half,5};
    half=Scalar::mul(env.pickup_sizes[character],.5f);player.fast_pickup_size={half,half,5};
    half=Scalar::mul(env.focused_pickup_sizes[character],.5f);player.slow_pickup_size={half,half,5};
    bounds(player.collision_bounds,player.position,player.hitbox_half_size);bounds(player.pickup_bounds,player.position,player.fast_pickup_size);bounds(player.slow_pickup_bounds,player.position,player.slow_pickup_size);bounds(player.fast_pickup_bounds,player.position,player.slow_pickup_size);
    set_timer(player.state_timer,player.state_timer_flags,0,env.rate);set_timer(player.invulnerability,player.invulnerability_flags,120,env.rate);player.option_follow_speed=30;env.configure_options(player);return 0;
}
// 0x424d90. Activating gameplay resets firing/focus but retains invulnerability.
void PlayerResources::activate(){auto& env=environment;player.state=1;set_timer(player.fire_timer,player.fire_timer_flags,-1,env.rate);set_timer(player.state_timer,player.state_timer_flags,0,env.rate);set_timer(player.focus_timer,player.focus_timer_flags,0,env.rate);env.registry->delete_and_clear(player.focus_animation);env.display_lives(env.game->lives);}
// 0x424ed0. Retry keeps the parsed profile and removes only its visible VMs.
void PlayerResources::shutdown(){auto& env=environment;(*env.chain)->remove_locked(player.update_entry,*env.callbacks);(*env.chain)->remove_locked(player.draw_entry,*env.callbacks);*env.current=nullptr;if(!(env.game->flags&1)){auto*& file=*env.animation_slot;if(file){env.release_animations(*file);env.delete_object(file);file=nullptr;}if(player.profile){env.free_bytes(player.profile);player.profile=nullptr;}*env.cached_profile=nullptr;}else{env.registry->discard_file(player.animation_file);*env.cached_profile=player.profile;}if(player.animation.geometry)env.free_bytes(player.animation.geometry);player.animation.geometry=nullptr;}
Player* PlayerResources::create(PlayerResourceEnvironment& env){auto* player=env.allocate();if(!player)return nullptr;PlayerResources resources{*player,env};resources.initialize();if(resources.start()){resources.shutdown();env.delete_object(player);return nullptr;}return player;}
}
