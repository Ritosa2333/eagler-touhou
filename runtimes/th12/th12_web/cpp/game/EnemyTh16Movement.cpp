#include "EnemyCommands.hpp"
#include <cmath>
namespace th11 {
namespace {
float motion_easing(float t,i32 mode)noexcept{
    const auto power=[](float x,u32 n){float result=x;for(u32 i=1;i<n;++i)result*=x;return result;};
    if(mode>=1&&mode<=3)return power(t,u32(mode+1));
    if(mode>=4&&mode<=6)return 1.f-power(1.f-t,u32(mode-2));
    if(mode>=9&&mode<=11){t*=2.f;return t<1.f?power(t,u32(mode-7))*.5f:(2.f-power(2.f-t,u32(mode-7)))*.5f;}
    if(mode>=12&&mode<=14){t*=2.f;return t<1.f?.5f-power(1.f-t,u32(mode-10))*.5f:power(t-1.f,u32(mode-10))*.5f+.5f;}
    if(mode==15)return 0;if(mode==16)return 1;return t;
}
}
float enemy_th16_motion_sample(FloatInterpolator& v,const float* rate)noexcept{
    const i32 mode=i32(v.mode);
    if(v.duration>0){v.timer.tick();if(v.timer.current>=v.duration){v.timer.set(v.duration,rate);v.duration=0;return mode==7||mode==17?v.start[0]:v.end[0];}}
    else if(v.duration==0)return mode==7||mode==17?v.start[0]:v.end[0];
    if(mode==7){v.start[0]+=v.end[0];return v.start[0];}
    if(mode==17){v.start[0]+=v.final_tangent[0];v.final_tangent[0]+=v.end[0];return v.start[0];}
    const float t=v.timer.fractional/float(v.duration);
    if(mode==8){
        const float twice=t*2.f,m=t-1.f,r=1.f-t;
        const float a=((3.f-twice)*(t*t))*v.end[0],b=((twice+1.f)*(m*m))*v.start[0];
        const float c=((r*r)*t)*v.initial_tangent[0],d=((m*t)*t)*v.final_tangent[0];return ((a+b)+c)+d;
    }
    return motion_easing(t,mode)*(v.end[0]-v.start[0])+v.start[0];
}
Vec3 enemy_th16_position_sample(Vec3Interpolator& v,const float* rate)noexcept{
    float result[3];
    // 42cd40 shares the scalar easing arithmetic and one timer across all axes.
    // Sampling copies keeps mode 7/17's accumulated start/tangent per axis.
    Timer timer=v.timer;i32 duration=v.duration;
    for(u32 n=0;n<3;++n){
        FloatInterpolator axis;axis.start[0]=v.start[n];axis.end[0]=v.end[n];
        axis.initial_tangent[0]=v.initial_tangent[n];axis.final_tangent[0]=v.final_tangent[n];
        axis.timer=v.timer;axis.duration=v.duration;axis.mode=v.mode;
        result[n]=enemy_th16_motion_sample(axis,rate);
        v.start[n]=axis.start[0];v.final_tangent[n]=axis.final_tangent[0];
        timer=axis.timer;duration=axis.duration;
    }
    v.timer=timer;v.duration=duration;return {result[0],result[1],result[2]};
}
void enemy_th16_ellipse_position(Movement& m)noexcept{
    constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;
    float angle=m.angle-m.ellipse_angle;
    if(angle>pi)angle=m.angle-float(m.ellipse_angle+tau);
    else if(m.ellipse_angle-m.angle>pi)angle=m.angle-float(m.ellipse_angle-tau);
    // 407210 and 403590 each normalize with the retail iteration limit.
    const auto p=polar(normalize_angle(normalize_angle(angle)),m.radius);const float x=m.ellipse_ratio*p.x;
    const float c=float(std::cos(double(m.ellipse_angle))),s=float(std::sin(double(m.ellipse_angle)));
    const float rx=float(c*x)-float(s*p.y),ry=float(c*p.y)+float(s*x);
    m.position={float(m.velocity.x+rx),float(m.velocity.y+ry),float(m.velocity.z+0.f)};m.snap_position();
}
i32 enemy_th16_movement_command(EnemyState& e,EclContext& c,EnemyGlobals& g)noexcept{
#if PORT_TITLE == 16
    const u16 op=c.instruction->opcode;auto f=[&](u32 n){return float(c.float_argument(n,g));};auto i=[&](u32 n){return c.integer_argument(n,g);};
    if(op==412||op==413){
        // Own41ff77..420241: bounds bias and native4a6d88 random angle.
        constexpr float pi=3.1415927410125732f,half_pi=1.5707963705062866f;
        auto& rng=g.environment.th16_visual_random?*g.environment.th16_visual_random:g.environment.random;
        const auto random_angle=[&](){return float(double(rng.signed_unit())*double(pi));};
        const float half_x=e.clamp_size.x*.25f,half_y=e.clamp_size.y*.25f;
        const float x=e.current.position.x,y=e.current.position.y;float angle;
        if(x<e.clamp_center.x-half_x)angle=random_angle()/3.f;
        else if(x>e.clamp_center.x+half_x)angle=normalize_angle(random_angle()/3.f+pi);
        else{angle=random_angle()*.25f;const u32 choice=rng.next16()%3;if(g.environment.player_position.x>x?choice==0:choice!=0)angle+=pi;}
        if(y<e.clamp_center.y-half_y)angle=std::abs(angle);
        else if(y>e.clamp_center.y+half_y)angle=-std::abs(angle);
        if(std::abs(float(angle+half_pi))<.05f)angle=angle<-half_pi?-1.6207963228225708f:-1.5207964181900024f;
        else if(double(std::abs(float(angle-half_pi)))<.05)angle=angle<half_pi?1.5207964181900024f:1.6207963228225708f;
        const i32 mode=i(1);const float speed=f(2);const i32 duration=i(0);if(mode<0||mode>17)return -2;
        const u32 index=op==412?0:2;auto& a=e.th16_motion_interpolation[index];auto& s=e.th16_motion_interpolation[index+1];
        a.duration=duration;a.mode=InterpolationMode(mode);a.start[0]=a.end[0]=angle;a.initial_tangent[0]=a.final_tangent[0]=0;a.timer.set(0,&g.environment.rate);
        s.duration=i(0);s.start[0]=speed;s.end[0]=0;s.initial_tangent[0]=s.final_tangent[0]=0;s.timer.set(0,&g.environment.rate);
        //420218 writes the angle mode again; the speed band's mode persists.
        (index==0?e.absolute:e.relative).flags&=~15u;return 0;
    }
    if(op==425||op==426){
        // 41ed90: Hermite position, arguments tangent0, endpoint, tangent1.
        auto& m=op==425?e.absolute:e.relative;auto& v=op==425?e.absolute_position:e.relative_position;
        const float x=f(3),y=f(4),tx=f(1),ty=f(2),ex=f(5),ey=f(6);const i32 duration=i(0);
        v.duration=duration;v.mode=InterpolationMode::Hermite;
        v.start[0]=m.position.x;v.start[1]=m.position.y;v.start[2]=m.position.z;
        v.end[0]=x>-999999.f?x:m.position.x;v.end[1]=y>-999999.f?y:m.position.y;v.end[2]=0;
        v.initial_tangent[0]=tx;v.initial_tangent[1]=ty;v.initial_tangent[2]=0;
        v.final_tangent[0]=ex;v.final_tangent[1]=ey;v.final_tangent[2]=0;
        v.timer.set(0,&g.environment.rate);m.flags&=~15u;return 0;
    }
    if(op==401||op==403){
        auto& m=op==401?e.absolute:e.relative;auto& v=op==401?e.absolute_position:e.relative_position;
        const float x=f(2),y=f(3);const i32 duration=i(0);
        if(duration<=0){v.duration=0;return 0;}
        const i32 mode=i(1);if(mode<0||mode>17)return -2;
        v.duration=i(0);v.mode=InterpolationMode(mode);
        v.start[0]=m.position.x;v.start[1]=m.position.y;v.start[2]=m.position.z;
        v.end[0]=x>-999999.f?x:m.position.x;v.end[1]=y>-999999.f?y:m.position.y;v.end[2]=0;
        for(u32 n=0;n<3;++n)v.initial_tangent[n]=v.final_tangent[n]=0;
        v.timer.set(0,&g.environment.rate);m.flags&=~15u;return 0;
    }
    if(op==424){e.flags=(e.flags&~0x80000u)|((u32(i(0))&1u)<<19);return 0;}
    if(op==420||op==422){
        auto& m=op==420?e.absolute:e.relative;
        const float values[]={f(0),f(1),f(2),f(3),f(4),f(5)};
        if((m.flags&15)!=2)m.velocity=m.position;
        if(values[0]>-999999.f)m.angle=normalize_angle(values[0]);
        if(values[1]>-999999.f)m.speed=values[1];if(values[2]>-999999.f)m.radius=values[2];if(values[3]>-999999.f)m.radial_velocity=values[3];
        if(values[4]>-999999.f)m.ellipse_angle=normalize_angle(values[4]);if(values[5]>-999999.f)m.ellipse_ratio=values[5];
        m.flags=(m.flags&~12u)|3u;enemy_th16_ellipse_position(m);e.combine_movement();return 0;
    }
    if(op==440||op==442){
        auto& m=op==440?e.absolute:e.relative;float angle=f(0);
        if(e.flags&0x80000){constexpr float h=1.57079637050628662109375f;angle=normalize_angle(float(h-normalize_angle(float(angle-h))));}
        m.angle=normalize_angle(angle);m.flags&=~15u;return 0;
    }
    if(op==444||op==446){auto& m=op==444?e.absolute:e.relative;const float speed=f(0);if(speed>-999999.f)m.speed=speed;m.flags&=~15u;return 0;}
    if(op==441||op==443){
        const u32 index=op==441?0:2;auto& m=index==0?e.absolute:e.relative;auto& v=e.th16_motion_interpolation[index];
        float target=f(2);const i32 duration=i(0);if(duration<=0){v.duration=0;return 0;}
        const i32 mode=i(1);if(mode>=18&&mode<=31)return -2;
        if(target>-999999.f){if(e.flags&0x80000){
            if(mode==7)target=-target;else{constexpr float h=1.57079637050628662109375f;target=normalize_angle(float(h-normalize_angle(float(target-h))));}
        }}else target=mode==7?0.f:m.angle;
        // 41f4d8..41f55f keeps the start and wraps the displacement.
        const float from=m.angle;constexpr float pi=3.1415927410125732421875f,tau=6.283185482025146484375f;
        float delta=float(target-from);
        if(delta>pi)delta=float(target-float(from+tau));
        else if(float(from-target)>pi)delta=float(target-float(from-tau));
        target=float(delta+from);
        v.duration=i(0);v.mode=InterpolationMode(mode);v.start[0]=from;v.end[0]=target;
        v.initial_tangent[0]=v.final_tangent[0]=0;v.timer.set(0,&g.environment.rate);m.flags&=~15u;return 0;
    }
    // 0x426960, the handler the th14 demo stops on with its own 0x1bd records (445 with 447 share
    // group 42). `cmp cx,si` with si=0x1bd picks the movement record (+0x88, else +0xcc) and then
    // the scalar band at +0x3f8 (else +0x458): the first and the third of the four interpolation
    // records the frame function samples, and the same pair the sibling 0x426b7f fills. Integer
    // argument 0 is the duration - a non-positive one only stores 0 and returns - integer argument 1
    // the mode, and the one float argument goes through the -999999 sentinel against the record's own
    // +0x18 speed (0.0 in mode 7), while the start is that same speed and both tangents are
    // cleared. Only in the positive-duration path does 0x14350 reset the timer and does
    // `and [eax+0x40],0xfffffff0` clear the record's movement nibble.
    if(op==445||op==447){
        auto& m=op==445?e.absolute:e.relative;auto& v=e.th16_motion_interpolation[op==445?1:3];
        const i32 duration=i(0);
        if(duration<=0){v.duration=0;return 0;}
        const i32 mode=i(1);const float value=f(2);
        v.duration=u32(duration);v.mode=InterpolationMode(mode);v.start[0]=m.speed;
        v.end[0]=value>-999999.f?value:(mode==7?0.f:m.speed);
        v.initial_tangent[0]=v.final_tangent[0]=0;v.timer.set(0,&g.environment.rate);
        m.flags&=~15u;return 0;
    }
    if(op==405||op==407||op==429||op==431){
        const u32 index=op==405||op==429?0:2;auto& m=index==0?e.absolute:e.relative;
        auto& angle=e.th16_motion_interpolation[index];auto& speed=e.th16_motion_interpolation[index+1];
        float to_angle=f(2),to_speed=f(3);const i32 duration=i(0);
        if(duration<=0){angle.duration=speed.duration=0;return 0;}
        const i32 mode=i(1);if(mode>=18&&mode<=31)return -2;
        const bool mirrored=(e.flags&0x80000)&&(op==405||op==407);
        if(mode==7){if(to_angle>-999999.f){if(mirrored)to_angle=-to_angle;}else to_angle=0;if(!(to_speed>-999999.f))to_speed=0;}
        else{
            if(to_angle>-999999.f){if(mirrored){constexpr float h=1.57079637050628662109375f;to_angle=normalize_angle(float(h-normalize_angle(float(to_angle-h))));}}
            else to_angle=m.angle;
            if(!(to_speed>-999999.f))to_speed=m.speed;
        }
        float from=m.angle;constexpr float pi=3.1415927410125732421875f,tau=6.283185482025146484375f;
        if(std::abs(float(from-to_angle))>=pi){if(to_angle>from)from+=tau;else to_angle+=tau;}
        auto setup=[&](FloatInterpolator& v,float begin,float end){v.duration=i(0);v.mode=InterpolationMode(mode);v.start[0]=begin;v.end[0]=end;v.initial_tangent[0]=v.final_tangent[0]=0;v.timer.set(0,&g.environment.rate);};
        setup(angle,from,to_angle);setup(speed,m.speed,to_speed);m.flags&=~15u;return 0;
    }
    if(op==404||op==406||op==428||op==430){
        auto& m=op==404||op==428?e.absolute:e.relative;
        float angle=f(0);const float speed=f(1);
        if(angle>-999999.f){
            if((e.flags&0x80000)&&(op==404||op==406)){
                constexpr float half_pi=1.57079637050628662109375f;
                angle=normalize_angle(float(half_pi-normalize_angle(float(angle-half_pi))));
            }
            m.angle=normalize_angle(angle);
        }
        if(speed>-999999.f)m.speed=speed;m.flags&=~15u;return 0;
    }
    if(op==400||op==402){
        auto& m=op==400?e.absolute:e.relative;const float x=f(0),y=f(1);
        if(x>-999999.f)m.position.x=x;if(y>-999999.f)m.position.y=y;m.flags&=~15u;
        e.current.position={float(e.relative.position.x+e.absolute.position.x),float(e.relative.position.y+e.absolute.position.y),float(e.relative.position.z+e.absolute.position.z)};
        e.combine_movement();return 0;
    }
    if(op==416||op==417){
        auto& p=op==416?e.absolute.position:e.relative.position;const float x=f(0),y=f(1),z=f(2);
        p={float(p.x+x),float(p.y+y),float(p.z+z)};e.combine_movement();return 0;
    }
    // 0x426a74: 0x198 selects the record at enemy+0x88 (absolute) with `cmp cx,ax`, and everything
    // else the one at +0xcc (relative); the record-relative offsets below are the mode table's own.
    // 0x26ae4 skips 0x26aef's `[esi+0x34..0x3c] <- [esi+0x00..0x08]` only when the mode nibble is
    // already 2. 0x26b0e stores the angle through 0x4154a0 (which normalizes it) under the
    // -999999.0f sentinel at 0x4c1c78; 0x26b35, 0x26b49 and 0x26b5b store speed, radius and radial
    // velocity with the same sentinel. 0x26b6b sets the nibble to 2, and 0x4180's 0x3df0 then
    // walks mode 2 (0x403e43: p = polar(angle, radius), position = velocity + (p.x, p.y, 0)) with
    // the 1/100 snap of 0x4c1b54 feeding 0x3eaa -> 0x40dc. The shared tail is 0x23750, the
    // coordinate sum the port already runs as combine_movement.
    if(op==408||op==410){
        auto& m=op==408?e.absolute:e.relative;
        if((m.flags&0xfu)!=2u)m.velocity=m.position;
        const float angle=f(0),speed=f(1),radius=f(2),radial=f(3);
        if(double(angle)>-999999.0)m.angle=normalize_angle(angle);
        if(speed>-999999.f)m.speed=speed;
        if(radius>-999999.f)m.radius=radius;
        if(radial>-999999.f)m.radial_velocity=radial;
        m.flags=(m.flags&0xfffffff2u)|2u;
        const Vec2 p=polar(m.angle,m.radius);
        m.position={float(double(m.velocity.x)+p.x),float(double(m.velocity.y)+p.y),float(double(m.velocity.z)+0.)};
        m.snap_position();
        e.combine_movement();return 0;
    }

#else
    (void)e;(void)c;(void)g;
#endif
    return -2;
}
bool enemy_th16_direction_request(const EnemyState& e,i32& direction,i32& script)noexcept{
    if(!(e.flags&0x100000))return false;
    direction=e.current.velocity.x<-.03f?-1:e.current.velocity.x>.03f?1:0;
    if(direction==e.direction)return false;
    const i32 variant=e.direction==-1?(direction?2:3):e.direction==0?(direction==-1?1:2):e.direction==1?(direction?1:4):0;
    script=wrapping_add(e.base_animation,variant);return true;
}
i32 enemy_th16_visibility(EnemyState& e)noexcept{
    const float half_x=float(e.visual_size.x*.5f),half_y=float(e.visual_size.y*.5f);
    if(float(e.current.position.x+half_x)<-192.f||float(e.current.position.x-half_x)>192.f)return (e.flags&0x10000)&&!(e.flags&4)?-1:0;
    if(float(e.current.position.y+half_y)<0.f||float(e.current.position.y-half_y)>448.f)return (e.flags&0x10000)&&!(e.flags&8)?-1:0;
    e.flags|=0x10000;return 0;
}
}
