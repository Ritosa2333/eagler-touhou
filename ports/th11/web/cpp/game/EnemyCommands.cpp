#include "EnemyCommands.hpp"
#include <cmath>
namespace th11 {
namespace {
constexpr float missing=-999999.f,pi=3.1415927410125732421875f,half_pi=1.57079637050628662109375f,tau=6.283185482025146484375f;
float keep(float value,float previous){return value<=missing?previous:value;}
Vec3 add(Vec3 a,Vec3 b){return {float(double(a.x)+b.x),float(double(a.y)+b.y),float(double(a.z)+b.z)};}
float mirror(float angle){return normalize_angle(float(double(half_pi)-normalize_angle(float(double(angle)-half_pi))));}
template<u32 N>void setup(Interpolator<float,N>& p,i32 duration,i32 mode,const float* rate){
    p.duration=duration;p.mode=InterpolationMode(mode);
    for(u32 i=0;i<N;++i)p.initial_tangent[i]=p.final_tangent[i]=0;
    p.timer.set(0,rate);
}
void endpoints(Vec2Interpolator& p,Vec2 start,Vec2 end){p.start[0]=start.x;p.start[1]=start.y;p.end[0]=end.x;p.end[1]=end.y;}
void endpoints(Vec3Interpolator& p,Vec3 start,Vec3 end){p.start[0]=start.x;p.start[1]=start.y;p.start[2]=start.z;p.end[0]=end.x;p.end[1]=end.y;p.end[2]=end.z;}
}
// Hand reconstruction of movement cases in the original 0x412e30 dispatcher.
// Argument evaluation order is explicit because references can consume RNG.
bool enemy_movement_command(EnemyState& e,EclContext& c,EnemyGlobals& g)noexcept{
    const u16 op=c.instruction->opcode;auto& world=g.environment;const float* rate=&world.rate;
    auto f=[&](u32 index){return float(c.float_argument(index,g));};
    auto i=[&](u32 index){return c.integer_argument(index,g);};
    switch(op){
    case 0x118:case 0x11a:{
        auto& m=op==0x118?e.absolute:e.relative;const float x=f(0),y=f(1);
        if(x>missing)m.position.x=x;if(y>missing)m.position.y=y;m.flags&=~1u;
        e.current.position=add(e.relative.position,e.absolute.position);e.combine_movement();break;
    }
    case 0x119:case 0x11b:{
        auto& m=op==0x119?e.absolute:e.relative;auto& p=op==0x119?e.absolute_position:e.relative_position;
        const float x=f(2),y=f(3);const i32 duration=i(0),mode=i(1);
        endpoints(p,m.position,{keep(x,m.position.x),keep(y,m.position.y),0});setup(p,duration,mode,rate);m.flags&=~1u;break;
    }
    case 0x11c:case 0x11e:{
        auto& m=op==0x11c?e.absolute:e.relative;float angle=f(0);const float speed=f(1);
        if(angle>missing){if(e.flags&0x8000)angle=mirror(angle);m.angle=normalize_angle(angle);}
        if(speed>missing)m.speed=speed;m.flags&=~1u;break;
    }
    case 0x11d:case 0x11f:{
        auto& m=op==0x11d?e.absolute:e.relative;auto& p=op==0x11d?e.absolute_angle:e.relative_angle;
        const float angle=f(2),speed=f(3);const i32 mode=i(1);
        Vec2 start{m.angle,m.speed},end;
        if(mode==7)end={keep(angle,0),keep(speed,0)};
        else end={angle<=missing?m.angle:(e.flags&0x8000)?mirror(angle):angle,keep(speed,m.speed)};
        if(std::fabs(float(double(start.x)-end.x))>=pi){if(end.x<=start.x)end.x=float(double(end.x)+tau);else start.x=float(double(start.x)+tau);}
        const i32 duration=i(0);endpoints(p,start,end);setup(p,duration,mode,rate);m.flags&=~1u;break;
    }
    case 0x120:case 0x122:{
        auto& m=op==0x120?e.absolute:e.relative;const float angle=f(0),speed=f(1),radius=f(2),radial=f(3);
        if(!(m.flags&1))m.velocity=m.position;
        if(angle>missing)m.angle=normalize_angle(angle);if(speed>missing)m.speed=speed;
        if(radius>missing)m.radius=radius;if(radial>missing)m.radial_velocity=radial;
        m.flags|=1;m.refresh_position();e.combine_movement();break;
    }
    case 0x121:case 0x123:{
        auto& m=op==0x121?e.absolute:e.relative;auto& a=op==0x121?e.absolute_angle:e.relative_angle;auto& r=op==0x121?e.absolute_radius:e.relative_radius;
        const float speed=f(2),radius=f(3),radial=f(4);
        const Vec2 a0{m.angle,m.speed},a1{0,keep(speed,m.speed)},r0{m.radius,m.radial_velocity},r1{keep(radius,m.radius),keep(radial,m.radial_velocity)};
        const i32 duration=i(0),mode=i(1);endpoints(a,a0,a1);endpoints(r,r0,r1);setup(a,duration,mode,rate);setup(r,duration,mode,rate);
        m.flags|=1;m.refresh_position();e.combine_movement();break;
    }
    case 0x124:case 0x125:{
        auto& m=op==0x124?e.absolute:e.relative;auto& p=op==0x124?e.absolute_angle:e.relative_angle;
        const double random=float(double(world.rng().signed_unit())*pi);float angle;
        if(e.current.position.x<double(e.clamp_center.x)-double(e.clamp_size.x)*.25)angle=float(random/3);
        else if(e.current.position.x>double(e.clamp_center.x)+double(e.clamp_size.x)*.25)angle=normalize_angle(float(random/3+pi));
        else if(e.current.position.x<world.player_position.x)angle=float(random*.5);
        else angle=normalize_angle(float(random*.5+pi));
        if(e.current.position.y<double(e.clamp_center.y)-double(e.clamp_size.y)*.25)angle=std::fabs(angle);
        else if(e.current.position.y>double(e.clamp_center.y)+double(e.clamp_size.y)*.25)angle=-std::fabs(angle);
        const i32 mode=i(1);const float speed=f(2);const i32 duration=i(0);
        endpoints(p,{angle,speed},{angle,0});setup(p,duration,mode,rate);m.flags&=~1u;break;
    }
    case 0x126:case 0x127:{if(!world.boss)__builtin_trap();(op==0x126?e.absolute:e.relative).position=world.boss->current.position;break;}
    case 0x128:case 0x129:{
        auto& m=op==0x128?e.absolute:e.relative;const float x=f(0),y=f(1),z=f(2);m.position=add(m.position,{x,y,z});e.combine_movement();break;
    }
    case 0x12a:case 0x12b:{
        auto& m=op==0x12a?e.absolute:e.relative;const float x=f(0),y=f(1);
        if(x>missing)m.velocity.x=x;if(y>missing)m.velocity.y=y;m.refresh_position();e.combine_movement();
        // The original falls through at 0x41444f and selects relative motion.
        [[fallthrough]];
    }
    case 0x12c:case 0x12e:{
        auto& m=op==0x12c?e.absolute:e.relative;
        const float angle=f(0),speed=f(1),radius=f(2),radial=f(3),axis=f(4),ratio=f(5);
        if(!(m.flags&1))m.velocity=m.position;
        if(angle>missing)m.angle=normalize_angle(angle);if(speed>missing)m.speed=speed;
        if(radius>missing)m.radius=radius;if(radial>missing)m.radial_velocity=radial;
        if(axis>missing)m.ellipse_angle=normalize_angle(axis);if(ratio>missing)m.ellipse_ratio=ratio;
        m.flags|=3;m.refresh_position();e.combine_movement();break;
    }
    case 0x12d:case 0x12f:{
        auto& m=op==0x12d?e.absolute:e.relative;auto& a=op==0x12d?e.absolute_angle:e.relative_angle;
        auto& r=op==0x12d?e.absolute_radius:e.relative_radius;auto& v=op==0x12d?e.absolute_ellipse:e.relative_ellipse;
        const float angle=f(2),speed=f(3),radius=f(4),radial=f(5),axis=f(6),ratio=f(7);
        const Vec2 a0{m.angle,m.speed},a1{keep(angle,m.angle),keep(speed,m.speed)},r0{m.radius,m.radial_velocity},r1{keep(radius,m.radius),keep(radial,m.radial_velocity)},v0{m.ellipse_angle,m.ellipse_ratio},v1{keep(axis,m.ellipse_angle),keep(ratio,m.ellipse_ratio)};
        const i32 duration=i(0),mode=i(1);endpoints(a,a0,a1);endpoints(r,r0,r1);endpoints(v,v0,v1);
        setup(a,duration,mode,rate);setup(r,duration,mode,rate);setup(v,duration,mode,rate);
        m.velocity=m.position;m.flags|=3;m.refresh_position();e.combine_movement();break;
    }
    case 0x130:e.flags=(e.flags&~0x8000u)|((u32(i(0))&1)<<15);break;
    case 0x131:case 0x132:{
        auto& m=op==0x131?e.absolute:e.relative;auto& p=op==0x131?e.absolute_position:e.relative_position;
        const float x=f(3),y=f(4),tx=f(1),ty=f(2),ux=f(5),uy=f(6);const i32 duration=i(0);
        endpoints(p,m.position,{keep(x,m.position.x),keep(y,m.position.y),0});setup(p,duration,8,rate);
        p.initial_tangent[0]=tx;p.initial_tangent[1]=ty;p.final_tangent[0]=ux;p.final_tangent[1]=uy;m.flags&=~1u;break;
    }
    case 0x133:
        e.absolute.position=add(e.relative.position,e.absolute.position);e.relative.position={};
        e.absolute.speed=e.relative.speed=0;e.absolute.velocity=e.relative.velocity={};e.absolute.flags&=~3u;e.relative.flags&=~3u;
        e.absolute_position.duration=e.relative_position.duration=e.absolute_angle.duration=e.relative_angle.duration=e.absolute_radius.duration=e.relative_radius.duration=e.absolute_ellipse.duration=e.relative_ellipse.duration=0;break;
    default:return false;
    }
    return true;
}
}
