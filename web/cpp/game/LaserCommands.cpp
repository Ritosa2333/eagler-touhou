#include "LaserBehavior.hpp"
#include "GameMath.hpp"
namespace th10 {
namespace {
void seek(Timer& timer,u32& flags,i32 value,const float* rate){
    if(!(flags&1)){flags|=1;timer.rate=rate;}
    timer.previous=wrapping_add(value,-1);timer.current=value;timer.fractional=Extended::from_int(value).to_float();
}
Extended aim(const Vec3& from,const Vec3& to){
    const auto x=number(to.x)-number(from.x),y=number(to.y)-number(from.y);
    return x==number(0.f)&&y==number(0.f)?number(1.57079637050628662109375f):angle_to(y,x);
}
}
// 0x41ca80. The 18-entry behavior program can modify itself and its cursor.
// Most optional feature handlers in this executable are empty virtual methods;
// their flags and argument setup still affect command sequencing.
void StraightLaser::process_commands(LaserBehaviorEnvironment& env){
    while(base.command_index<18){
        auto& command=parameters.commands[base.command_index];const auto type=command.type;
        if(!type||(!command.concurrent&&base.active_features))return;
        switch(type){
        case 1:base.active_features|=1;seek(base.acceleration_timer,base.acceleration_timer_flags,0,env.rate);base.acceleration_step=0;break;
        case 0x10:{
            base.active_features|=0x10;base.acceleration=command.floating(0);
            const float value=command.floating(1);
            if(value<=-990.f)base.acceleration_angle=base.angle;
            else if(value>=990.f)base.acceleration_angle=aim(base.position,*env.player_position).to_float();
            else base.acceleration_angle=value;
            seek(base.vector_timer,base.vector_timer_flags,0,env.rate);base.vector_duration=command.integer(2);
            const auto acceleration=polar(base.acceleration_angle,base.acceleration);base.vector_acceleration.x=acceleration.x;base.vector_acceleration.y=acceleration.y;
            if(base.command_index&&base.turn_sound>=0)env.play_turn_sound(base.turn_sound);break;
        }
        case 0x20:
            base.active_features|=0x20;base.angular_acceleration=command.floating(0);base.angular_speed=command.floating(1);
            seek(base.angular_timer,base.angular_timer_flags,0,env.rate);base.angular_duration=command.integer(2);
            if(base.command_index&&base.turn_sound>=0)env.play_turn_sound(base.turn_sound);break;
        case 0x40:case 0x80:case 0x100:
            base.active_features|=type;base.turn_angle=command.floating(0);base.turn_speed=command.floating(1)>-999.f?command.floating(1):base.speed;
            seek(base.turn_timer,base.turn_timer_flags,0,env.rate);base.turn_duration=command.integer(2);base.turn_count=command.integer(3);base.turn_completed=0;break;
        case 0x400:case 0x800:case 0x8000000:
            if(command.integer(2)>0){
                base.active_features|=type;const float speed=command.floating(0);base.reflection_speed=speed>=0.f?speed:base.speed;
                --command.arguments[2];base.reflection_count=base.reflection_state=0;
            }break;
        case 0x4000:initialize_embedded_animation(*env.bullet_file,beam,wrapping_add(env.sprite_scripts[command.integer(2)],command.integer(3)),*env.animations,*env.started_animations);break;
        case 0x8000:base.active_features|=type;seek(base.feature_delay,base.feature_delay_flags,command.integer(2),env.rate);break;
        case 0x10000:base.state=3;break;
        case 0x20000:env.play_sound(command.integer(2),base.position.x);break;
        case 0x100000:case 0x200000:base.active_features|=type;seek(base.size_timer,base.size_timer_flags,command.integer(2),env.rate);break;
        case 0x1000000:base.id=command.arguments[2];break;
        case 0x2000000:base.command_index=command.integer(2);continue;
        case 0x4000000:
            base.active_features|=type;base.blend_start=command.floating(0);base.blend_end=command.floating(1);
            seek(base.blend_timer,base.blend_timer_flags,0,env.rate);base.blend_duration=command.integer(2);break;
        default:break;
        }
        base.command_index=wrapping_add(base.command_index,1);
    }
}
// 0x41d2c0. Speed and velocity are both accumulated, then the angle is derived
// from non-negligible velocity. The timer advances even on the completion frame.
void StraightLaser::accelerate(LaserBehaviorEnvironment& env){
    if(base.vector_timer.current>=base.vector_duration)base.active_features&=~0x10u;
    else{
        base.speed=(number(*env.rate)*number(base.acceleration)+number(base.speed)).to_float();
        const auto x=number(*env.rate)*number(base.vector_acceleration.x),y=number(*env.rate)*number(base.vector_acceleration.y);
        const float z=Scalar::mul(*env.rate,base.vector_acceleration.z);
        base.velocity.x=(x+number(base.velocity.x)).to_float();base.velocity.y=(y+number(base.velocity.y)).to_float();base.velocity.z=Scalar::add(z,base.velocity.z);
        auto vx=number(base.velocity.x),vy=number(base.velocity.y);if(vx<number(0.f))vx=-vx;if(vy<number(0.f))vy=-vy;
        if(number(.0001f)<vx||number(.0001f)<vy)base.angle=angle_to(number(base.velocity.y),number(base.velocity.x)).to_float();
    }
    base.vector_timer.tick();
}
// 0x41d170. Each interval slows the laser's velocity before applying a turn.
void StraightLaser::turn(LaserBehaviorEnvironment& env){
    float speed;
    if(base.turn_timer.current>=base.turn_duration){
        if(base.turn_sound>=0)env.play_turn_sound(base.turn_sound);
        base.turn_completed=wrapping_add(base.turn_completed,1);if(base.turn_completed>=base.turn_count)base.active_features&=~0x40u;
        base.angle=Scalar::add(base.turn_angle,base.angle);speed=base.speed=base.turn_speed;
        seek(base.turn_timer,base.turn_timer_flags,0,env.rate);
    }else speed=(number(base.speed)-number(base.speed)*number(base.turn_timer.fractional)/Extended::from_int(base.turn_duration)).to_float();
    const auto velocity=polar(base.angle,speed);base.velocity.x=velocity.x;base.velocity.y=velocity.y;base.turn_timer.tick();
}
// 0x41cfd0. Reflection creates a new laser from the endpoint and consumes the
// parent's reflection flags. At a corner two children can be emitted in order.
void StraightLaser::reflect(LaserBehaviorEnvironment& env){
    const auto direction=polar(base.angle,base.length);
    const Vec3 end{Scalar::add(direction.x,base.position.x),Scalar::add(direction.y,base.position.y),0};
    if(!laser_outside_playfield(end,0,0))return;
    if(base.turn_sound>=0)env.play_turn_sound(base.turn_sound);bool reflected=false;
    if(end.x< -192.f||end.x>=192.f){
        parameters.position=end;parameters.angle=normalize_angle((-number(base.angle)-number(3.1415927410125732421875f)).to_float()).to_float();parameters.speed=base.reflection_speed;
        env.spawn_straight(parameters);reflected=true;
    }
    if(!(base.active_features&0x8000000)&&(end.y<0.f||(end.y>=448.f&&(base.active_features&0x400)))){
        parameters.position=end;parameters.angle=(-number(base.angle)).to_float();parameters.speed=base.reflection_speed;env.spawn_straight(parameters);reflected=true;
    }
    if(reflected)base.active_features&=~0x8000c00u;
}
}
