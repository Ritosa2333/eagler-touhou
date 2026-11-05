#include "BulletEmitter.hpp"
#include "LaserBehavior.hpp"
#include "GameMath.hpp"
namespace th10 {
namespace {
void reset_timer(Timer& timer,u32& flags,const float* rate){if(!(flags&1)){flags|=1;timer.rate=rate;}timer.initialize(-1);}
}
// 0x405c80. The emitter is reused by ECL and by nested bullet programs.
void BulletEmitter::initialize() noexcept {__builtin_memset(this,0,sizeof(*this));turn_sound=-1;}
// 0x4073e0. A full pool ends both loops; the shooting sound still plays.
i32 BulletEmitter::fire(EnemyBulletManager& manager,BulletBehaviorEnvironment& env) const {
    const auto x=number(env.player_position->x)-number(position.x),y=number(env.player_position->y)-number(position.y);
    const float aim=(x==number(0.f)&&y==number(0.f)?number(1.57079637050628662109375f):angle_to(y,x)).to_float();
    bool full=false;
    for(i32 layer=0;layer<layers&&!full;++layer)for(i32 index=0;index<count;++index)if(manager.spawn(*this,index,layer,aim,env)){full=true;break;}
    if(flags&0x200)env.play_sound(shoot_sound,position.x);
    return 0;
}
// 0x4067d0. Pattern formulas preserve the original extended precision order,
// random draw order, and the distinction between raw and stored angles.
i32 EnemyBulletManager::spawn(const BulletEmitter& e,i32 index,i32 layer,float aim,BulletBehaviorEnvironment& env){
    auto* bullet=cursor;i32 searched=0;
    while(searched<2000&&bullet->state){++bullet;if(bullet->state==5)bullet=pool;++searched;}
    if(searched>=2000)return 1;
    float speed=e.layers<=1?e.speed_start:(number(e.speed_start)-(number(e.speed_start)-number(e.speed_end))*Extended::from_int(layer)/Extended::from_int(e.layers)).to_float();
    float angle=0;
    const auto count=Extended::from_int(e.count),tau=number(6.283185482025146484375f),pi=number(3.1415927410125732421875f);
    switch(e.pattern){
    case 0:case 1:{
        auto step=e.count&1?Extended::from_int(wrapping_add(index,1)/2):Extended::from_int(index/2)+number(.5f);
        step=step*number(e.spread);if(index&1)step=step*number(-1.f);if(e.pattern==0)step=step+number(aim);angle=(step+number(e.angle)).to_float();break;
    }
    case 2:case 3:angle=(Extended::from_int(index)*tau/count+number(e.pattern==2?aim:0.f)+Extended::from_int(layer)*number(e.spread)+number(e.angle)).to_float();break;
    case 4:case 5:angle=(pi/count+number(e.pattern==4?aim:0.f)+Extended::from_int(index)*tau/count+Extended::from_int(layer)*number(e.spread)+number(e.angle)).to_float();break;
    case 6:angle=((number(e.angle)-number(e.spread))*env.rng->unit()+number(e.spread)).to_float();break;
    case 7:
        speed=((number(e.speed_start)-number(e.speed_end))*env.rng->unit()+number(e.speed_end)).to_float();
        angle=(Extended::from_int(index)*tau/count+Extended::from_int(layer)*number(e.spread)+number(e.angle)).to_float();break;
    case 8:
        angle=((number(e.angle)-number(e.spread))*env.rng->unit()+number(e.spread)).to_float();
        speed=((number(e.speed_start)-number(e.speed_end))*env.rng->unit()+number(e.speed_end)).to_float();break;
    default:break;
    }
    bullet->flags|=1;bullet->state=1;
    reset_timer(bullet->cancel_timer,bullet->cancel_timer_flags,env.default_rate);reset_timer(bullet->secondary_timer,bullet->secondary_timer_flags,env.default_rate);
    bullet->motion.speed=speed;bullet->motion.angle=add_angle(angle,0).to_float();bullet->motion.position=e.position;bullet->motion.position.z=.1f;
    const auto velocity=polar(angle,speed);bullet->motion.velocity.x=velocity.x;bullet->motion.velocity.y=velocity.y;
    bullet->active_features=e.flags;bullet->color=e.color;bullet->sprite_type=e.sprite_type;bullet->flags=(bullet->flags&~0xcu)|2;bullet->reserved_454=0;
    initialize_embedded_animation(*animation_file,bullet->animation,wrapping_add(env.sprite_scripts[e.sprite_type],e.color),*env.animations,env.manager->started_scripts);
    switch(env.cancel_types[e.sprite_type]){
    case 0:bullet->cancel_script=e.color*2+17;break;
    case 1:bullet->cancel_script=env.cancel_scripts[e.color];break;
    case 2:bullet->cancel_script=-1;break;
    case 3:bullet->cancel_script=29;break;
    case 4:bullet->cancel_script=19;break;
    default:break;
    }
    bullet->draw_layer=env.draw_layers[e.sprite_type];bullet->turn_sound=e.turn_sound;bullet->outside_delay=10;
    bullet->hitbox_height=bullet->cancel_size=env.hitbox_sizes[e.sprite_type];
    if(e.flags&0xe){
        bullet->animation.pending_interrupt=e.flags&2?7:e.flags&4?8:9;bullet->state=2;
        const auto x=number(bullet->motion.velocity.x)*number(4.f),y=number(bullet->motion.velocity.y)*number(4.f);const float z=Scalar::mul(bullet->motion.velocity.z,4.f);
        bullet->motion.position.x=(number(bullet->motion.position.x)-x).to_float();bullet->motion.position.y=(number(bullet->motion.position.y)-y).to_float();bullet->motion.position.z=Scalar::sub(bullet->motion.position.z,z);
    }else bullet->animation.pending_interrupt=2;
    __builtin_memcpy(bullet->commands,e.commands,sizeof(e.commands));bullet->spawn_flags=e.flags;bullet->active_features=0;bullet->command_index=e.command_start;
    env.run_commands(*bullet);bullet->animation.update(*env.animations);
    cursor=bullet+1;if(cursor->state==5)cursor=pool;
    return 0;
}
}
