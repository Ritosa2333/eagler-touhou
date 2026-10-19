#include "BulletEmitter.hpp"
#include "LaserBehavior.hpp"
#include "GameMath.hpp"
namespace th10 {
namespace {
void seek(ProjectileModifier& modifier,i32 value,const float* rate){
    if(!(modifier.timer_flags&1)){modifier.timer_flags|=1;modifier.timer.rate=rate;}
    modifier.timer.previous=wrapping_add(value,-1);modifier.timer.current=value;modifier.timer.fractional=Extended::from_int(value).to_float();
}
float argument_angle(float value,const EnemyBullet& bullet,const BulletBehaviorEnvironment& env){
    if(value<=-990.f)return bullet.motion.angle;
    if(value>=990.f){const auto x=number(env.player_position->x)-number(bullet.motion.position.x),y=number(env.player_position->y)-number(bullet.motion.position.y);return (x==number(0.f)&&y==number(0.f)?number(1.57079637050628662109375f):angle_to(y,x)).to_float();}
    return value;
}
}
// 0x406d90. These commands share their binary format with laser commands, but
// have different reflection, aiming, cancellation and nested emission behavior.
void EnemyBullet::process_commands(BulletBehaviorEnvironment& env){
    while(command_index<18){
        auto& command=commands[command_index];const auto type=command.type;
        if(!type||(!command.concurrent&&active_features))return;
        switch(type){
        case 1:active_features|=1;seek(modifiers[0],0,env.default_rate);modifiers[0].vector.z=0;break;
        case 0x10:{
            active_features|=type;auto& m=modifiers[1];m.first=command.floating(0);m.second=argument_angle(command.floating(1),*this,env);seek(m,0,env.default_rate);m.duration=command.integer(2);
            const auto acceleration=polar(m.second,m.first);m.vector.x=acceleration.x;m.vector.y=acceleration.y;
            if(command_index&&turn_sound>=0)env.play_turn_sound(turn_sound);break;
        }
        case 0x20:{
            active_features|=type;auto& m=modifiers[2];m.first=command.floating(0);m.second=command.floating(1);seek(m,0,env.default_rate);m.duration=command.integer(2);
            if(command_index&&turn_sound>=0)env.play_turn_sound(turn_sound);break;
        }
        case 0x40:case 0x80:case 0x100:{
            active_features|=type;auto& m=modifiers[3];m.second=argument_angle(command.floating(0),*this,env);m.first=command.floating(1)>-999.f?command.floating(1):motion.speed;
            seek(m,0,env.default_rate);m.duration=command.integer(2);m.count=command.integer(3);m.iteration=0;break;
        }
        case 0x400:case 0x800:case 0x8000000:active_features|=type;modifiers[4].first=command.floating(0);modifiers[4].count=command.integer(2);modifiers[4].duration=0;break;
        case 0x1000:cancel_protection=command.arguments[2];break;
        case 0x2000:outside_delay=command.integer(2);break;
        case 0x4000:
#ifdef TH_ENABLE_THPRAC
            // 0x406e03: with "Real bullet sprites" on, jump to the loop
            // increment so this embedded-animation setup is skipped.
            if(env.real_bullet_sprite&&*env.real_bullet_sprite)break;
#endif
            initialize_embedded_animation(*env.effect_file,animation,wrapping_add(env.sprite_scripts[command.integer(2)],command.integer(3)),*env.animations,env.manager->started_scripts);break;
        case 0x8000:active_features|=type;seek(modifiers[5],command.integer(2),env.default_rate);break;
        case 0x10000:cancel(env);break;
        case 0x20000:env.play_sound(command.integer(2),motion.position.x);break;
        case 0x100000:case 0x200000:active_features|=type;seek(modifiers[type==0x100000?6:7],command.integer(2),env.default_rate);break;
        case 0x400000:{
            BulletEmitter emitter;emitter.initialize();emitter.position=motion.position;
            emitter.color=static_cast<std::int8_t>(command.arguments[2]>>8);emitter.command_start=command.arguments[2]&0xff;emitter.count=static_cast<std::int16_t>(command.arguments[3]);
            emitter.speed_start=command.floating(0);emitter.speed_end=command.floating(1);const bool cancel_self=command.arguments[2]&0x80000000;
            command_index=wrapping_add(command_index,1);const auto& next=commands[command_index];emitter.layers=static_cast<std::int16_t>(next.arguments[2]);emitter.flags=next.arguments[3];emitter.angle=next.floating(0);emitter.spread=next.floating(1);
            __builtin_memcpy(emitter.commands,commands,sizeof(commands));env.emit(emitter);command_index=wrapping_add(command_index,1);
            if(cancel_self)cancel(env);else continue; // Cancel also consumes the entry after the parameter pair.
            break;
        }
        case 0x1000000:id=command.integer(2);break;
        case 0x2000000:command_index=command.integer(2);continue;
        case 0x4000000:{active_features|=type;auto& m=modifiers[8];m.first=command.floating(0);m.second=command.floating(1);seek(m,0,env.default_rate);m.duration=command.integer(2);break;}
        default:break;
        }
        command_index=wrapping_add(command_index,1);
    }
}
}
