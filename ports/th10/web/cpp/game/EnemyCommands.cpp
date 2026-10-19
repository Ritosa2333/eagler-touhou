#include "EnemyVariables.hpp"
namespace th10 {
namespace {
bool supplied(Extended value){return number(-999999.0f)<value;}
bool supplied(float value){return supplied(number(value));}
float mirror_angle(float value){
    const auto half_pi=number(1.5707963705062866f);
    const auto shifted=normalize_angle((number(value)-half_pi).to_float());
    return normalize_angle((half_pi-shifted).to_float()).to_float();
}
template<unsigned N> void restart(Interpolator<float,N>& interpolation,const float* rate){
    if(!(interpolation.flags&1)){interpolation.timer.rate=rate;interpolation.flags|=1;}
    interpolation.timer.initialize(-1);
}
void initialize(Vec2Interpolator& interpolation,Vec2 start,Vec2 end,i32 duration,InterpolationMode mode,EnemyEnvironment& env){
    interpolation.duration=duration;interpolation.mode=mode;
    interpolation.start[0]=start.x;interpolation.start[1]=start.y;interpolation.end[0]=end.x;interpolation.end[1]=end.y;
    interpolation.initial_tangent[0]=interpolation.final_tangent[0]=env.default_tangent2->x;
    interpolation.initial_tangent[1]=interpolation.final_tangent[1]=env.default_tangent2->y;
    restart(interpolation,env.default_rate);
}
Extended absolute_value(Extended value){return value<number(0.0f)?-value:value;}
}
// ECL enemy movement commands, the 0x118..0x12b cases of 0x40e770.
bool EnemyState::movement_command(EclContext& context,EclGlobals& globals,EnemyEnvironment& env){
    const auto opcode=context.instruction->opcode;
    const auto integer=[&](u32 index){return context.integer_argument(index,globals);};
    const auto floating=[&](u32 index){return context.float_argument(index,globals);};
    switch(opcode){
    case 0x118:case 0x11a:{
        auto& movement=opcode==0x118?absolute:relative;
        const float x=floating(0).to_float();const auto y=floating(1);
        if(supplied(x))movement.position.x=x;
        if(supplied(y))movement.position.y=y.to_float();
        movement.flags&=~1u;
        current.position={Scalar::add(relative.position.x,absolute.position.x),
            Scalar::add(relative.position.y,absolute.position.y),
            Scalar::add(relative.position.z,absolute.position.z)};
        return true;
    }
    case 0x119:case 0x11b:{
        auto& movement=opcode==0x119?absolute:relative;
        auto& interpolation=opcode==0x119?absolute_position:relative_position;
        const float x=floating(2).to_float(),y=floating(3).to_float();
        interpolation.duration=integer(0);
        std::memcpy(interpolation.initial_tangent,env.default_tangent,12);
        std::memcpy(interpolation.final_tangent,env.default_tangent,12);
        interpolation.mode=static_cast<InterpolationMode>(integer(1));
        std::memcpy(interpolation.start,&movement.position,12);
        interpolation.end[0]=supplied(x)?x:movement.position.x;
        interpolation.end[1]=supplied(y)?y:movement.position.y;
        interpolation.end[2]=0;
        restart(interpolation,env.default_rate);movement.flags&=~1u;return true;
    }
    case 0x11c:case 0x11e:{
        auto& movement=opcode==0x11c?absolute:relative;
        const float angle=floating(0).to_float(),speed=floating(1).to_float();
        if(supplied(angle))movement.set_angle(flags&0x800?mirror_angle(angle):angle);
        if(supplied(speed))movement.speed=speed;
        movement.flags&=~1u;return true;
    }
    case 0x11d:case 0x11f:{
        auto& movement=opcode==0x11d?absolute:relative;
        auto& interpolation=opcode==0x11d?absolute_angle:relative_angle;
        const float angle=floating(2).to_float(),speed=floating(3).to_float();
        const auto mode=static_cast<InterpolationMode>(integer(1));interpolation.mode=mode;
        Vec2 end;
        if(mode==InterpolationMode::Velocity){end={supplied(angle)?angle:0.0f,supplied(speed)?speed:0.0f};}
        else{end={supplied(angle)?(flags&0x800?mirror_angle(angle):angle):movement.angle,supplied(speed)?speed:movement.speed};}
        Vec2 start{movement.angle,movement.speed};
        if(!(absolute_value(number(start.x)-number(end.x))<number(3.1415927410125732f))){
            if(number(start.x)<number(end.x))start.x=Scalar::add(start.x,6.2831854820251465f);
            else end.x=Scalar::add(end.x,6.2831854820251465f);
        }
        const auto duration=integer(0);initialize(interpolation,start,end,duration,mode,env);
        movement.flags&=~1u;return true;
    }
    case 0x120:case 0x122:{
        auto& movement=opcode==0x120?absolute:relative;
        const float angle=floating(0).to_float(),speed=floating(1).to_float(),radius=floating(2).to_float(),radial=floating(3).to_float();
        if(!(movement.flags&1))movement.velocity=movement.position;
        if(supplied(angle))movement.set_angle(angle);
        if(supplied(speed))movement.speed=speed;
        if(supplied(radius))movement.radius=radius;
        if(supplied(radial))movement.radial_velocity=radial;
        movement.flags|=1;return true;
    }
    case 0x121:case 0x123:{
        auto& movement=opcode==0x121?absolute:relative;
        auto& angle_interpolation=opcode==0x121?absolute_angle:relative_angle;
        auto& radius_interpolation=opcode==0x121?absolute_radius:relative_radius;
        const float angle=floating(2).to_float(),speed=floating(3).to_float(),radius=floating(4).to_float();
        const auto radial=floating(5);
        const Vec2 angle_end{supplied(angle)?angle:movement.angle,supplied(speed)?speed:movement.speed};
        const Vec2 radius_end{supplied(radius)?radius:movement.radius,supplied(radial)?radial.to_float():movement.radial_velocity};
        const Vec2 angle_start{movement.angle,movement.speed},radius_start{movement.radius,movement.radial_velocity};
        const auto duration=integer(0);const auto mode=static_cast<InterpolationMode>(integer(1));
        initialize(angle_interpolation,angle_start,angle_end,duration,mode,env);
        initialize(radius_interpolation,radius_start,radius_end,duration,mode,env);
        movement.velocity=movement.position;movement.flags|=1;return true;
    }
    case 0x124:case 0x125:{
        auto& movement=opcode==0x124?absolute:relative;
        auto& interpolation=opcode==0x124?absolute_angle:relative_angle;
        const auto quarter=number(Scalar::mul(clamp_size.x,0.25f));
        const auto x=number(current.position.x),center=number(clamp_center.x),pi=number(3.1415927410125732f);
        Extended angle;
        if(x<center-quarter)angle=env.script_rng->signed_unit()*number(1.0471975803375244f);
        else if(quarter+center<x)angle=normalize_angle((env.script_rng->signed_unit()*pi*number(0.3333333432674408f)+pi).to_float());
        else if(x<number(env.player_position->x))angle=env.script_rng->signed_unit()*number(1.5707963705062866f);
        else angle=normalize_angle((env.script_rng->signed_unit()*pi*number(0.5f)+pi).to_float());
        float target=angle.to_float();
        const auto y=number(current.position.y),ycenter=number(clamp_center.y),yquarter=number(Scalar::mul(clamp_size.y,0.25f));
        if(y<ycenter-yquarter)target=absolute_value(number(target)).to_float();
        else if(yquarter+ycenter<y)target=(-absolute_value(number(target))).to_float();
        const auto mode=static_cast<InterpolationMode>(integer(1));interpolation.mode=mode;
        const float speed=floating(2).to_float();const auto duration=integer(0);
        initialize(interpolation,{target,speed},{target,0.0f},duration,mode,env);
        movement.flags&=~1u;return true;
    }
    case 0x126:absolute.position=env.boss->state.current.position;return true;
    case 0x127:relative.position=env.boss->state.current.position;return true;
    case 0x128:case 0x129:{
        auto& movement=opcode==0x128?absolute:relative;
        const float x=floating(0).to_float(),y=floating(1).to_float();const auto z=floating(2);
        movement.position.x=Scalar::add(x,movement.position.x);
        movement.position.y=Scalar::add(y,movement.position.y);
        movement.position.z=(z+number(movement.position.z)).to_float();return true;
    }
    case 0x12a:case 0x12b:{
        auto& movement=opcode==0x12a?absolute:relative;
        const float x=floating(0).to_float();const auto y=floating(1);
        if(supplied(x))movement.velocity.x=x;
        if(supplied(y))movement.velocity.y=y.to_float();
        // Both original cases also copy the boss's position to absolute.
        absolute.position=env.boss->state.current.position;return true;
    }
    default:return false;
    }
}
}
