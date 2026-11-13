#include "EnemyCommands.hpp"
#include "EnemyManager.hpp"
#include "ScreenDeformation.hpp"
#include "Stage.hpp"
#include "Dialogue.hpp"
#include "SpellController.hpp"
#include "LaserManager.hpp"
#include "BulletManager.hpp"
#include "AnmVm.hpp"
#include "EnemyCallbacks.hpp"
#include <cmath>
namespace th11 {
void enemy_th14_death_metadata(EnemyState& e)noexcept{
    // Retail 4220d0..422181; keep explicit ECL overrides from the first frame.
    e.death_sound=i32(u32(e.instance_id)&1)+3;
    if(e.death_animation)return;
    e.death_animation=43;e.death_animation_file=1;
    if(e.bound_animation_file!=2)return;
    switch(e.animation_script){
    case 5:case 25:case 53:case 79:e.death_animation=39;break;
    case 10:case 56:case 83:e.death_animation=47;break;
    case 15:case 91:e.death_animation=51;break;
    case 30:e.death_animation=57;break;
    case 35:e.death_animation=56;break;
    case 40:e.death_animation=55;break;
    }
}
bool enemy_th14_clear_drops(EnemyState& e)noexcept{
    // 424cb0: the next list link is saved before invoking death callbacks.
    if(((e.flags&0xa0)||(e.flags&0xc00400))&&!(e.flags&0x100))return false;
    e.drops.primary=0;for(u32 n=0;n<13;++n)e.drops.counts[n]=0;
    e.drops.spread={32,32};return true;
}
i32 enemy_th14_bomb_damage(const EnemyState& e,i32 damage,i32 bomb_state,bool& resistant_sound)noexcept{
    resistant_sound=false;
#if PORT_TITLE == 14
    if(bomb_state==1&&e.th14_bomb_damage_ratio<1.f){
        resistant_sound=damage!=0&&e.th14_bomb_damage_ratio<=0.f;
        return truncate_int(float(float(damage)*e.th14_bomb_damage_ratio));
    }
#endif
    return damage;
}
i32 enemy_th14_hit_sound(const EnemyState& e,u32 spell_flags)noexcept{
    if(e.damage_reserved>=0)return e.damage_reserved;
    const bool low=(e.flags&0x40800000)&&((spell_flags&9)!=9)&&e.interrupt_health<((spell_flags&1)?200:900);
    return low?35:34;
}
i32 enemy_th14_hit_flash(EnemyState& e,AnmVm& vm,u32 spell_flags)noexcept{
    // Retail 423ee5..42407c uses native color-mode bits and TH14 enemy flags.
    auto clear=[&](){vm.th13_native_flags&=~0x60000u;};
    auto tint=[&](u32 color){vm.th13_native_flags=(vm.th13_native_flags&~0x60000u)|0x20000u;vm.secondary_color=color;};
    if(e.damage_flash){clear();e.damage_flash=wrapping_add(e.damage_flash,-1);return -1;}
    const bool flash_frame=e.timer_2bc.current%4==0;
    if(e.flags&0x80000000u){if(flash_frame)tint(0xffff00ff);else clear();}
    if(e.flags&0x200000u){tint(0xff0000ff);e.damage_flash=4;return enemy_th14_hit_sound(e,spell_flags);}
    if(!flash_frame)clear();
    else if((e.flags&0x40800000u)&&((spell_flags&9)!=9)&&e.interrupt_health<((spell_flags&1)?100:500))tint(0xff0000ff);
    return -1;
}
namespace {
float motion_easing(float t,i32 mode)noexcept{
    // Retail 42cfb0 use the same 4041a0 sine/reverse-parabola
    // table as bullet interpolation, after rounding elapsed/duration.
    if(mode>=18&&mode<=31)return easing(t,1,InterpolationMode(mode));
    const auto power=[](float x,u32 n){float result=x;for(u32 i=1;i<n;++i)result*=x;return result;};
    if(mode>=1&&mode<=3)return power(t,u32(mode+1));
    if(mode>=4&&mode<=6)return 1.f-power(1.f-t,u32(mode-2));
    if(mode>=9&&mode<=11){t*=2.f;return t<1.f?power(t,u32(mode-7))*.5f:(2.f-power(2.f-t,u32(mode-7)))*.5f;}
    if(mode>=12&&mode<=14){t*=2.f;return t<1.f?.5f-power(1.f-t,u32(mode-10))*.5f:power(t-1.f,u32(mode-10))*.5f+.5f;}
    if(mode==15)return 0;if(mode==16)return 1;return t;
}
float motion_value(FloatInterpolator& v,i32 mode,float t)noexcept{
    if(mode==7){v.start[0]+=v.end[0];return v.start[0];}
    if(mode==17){v.start[0]+=v.final_tangent[0];v.final_tangent[0]+=v.end[0];return v.start[0];}
    if(mode==8){
        const float twice=t*2.f,m=t-1.f,r=1.f-t;
        const float a=((3.f-twice)*(t*t))*v.end[0],b=((twice+1.f)*(m*m))*v.start[0];
        const float c=((r*r)*t)*v.initial_tangent[0],d=((m*t)*t)*v.final_tangent[0];return ((a+b)+c)+d;
    }
    return motion_easing(t,mode)*(v.end[0]-v.start[0])+v.start[0];
}

}
float enemy_th14_motion_sample(FloatInterpolator& v,const float* rate)noexcept{
    const i32 mode=i32(v.mode);
    if(v.duration>0){v.timer.tick();if(v.timer.current>=v.duration){v.timer.set(v.duration,rate);v.duration=0;return mode==7||mode==17?v.start[0]:v.end[0];}}
    else if(v.duration==0)return mode==7||mode==17?v.start[0]:v.end[0];
    return motion_value(v,mode,v.timer.fractional/float(v.duration));
}
Vec3 enemy_th14_position_sample(Vec3Interpolator& v,const float* rate,const i32* axis_modes)noexcept{
    float result[3];
    if(axis_modes){
        // Retail 42cd40 completes according to the retained common mode,
        // but samples each live axis with its own +54/+58/+5c mode.
        bool complete=v.duration==0;
        if(v.duration>0){v.timer.tick();if(v.timer.current>=v.duration){v.timer.set(v.duration,rate);v.duration=0;complete=true;}}
        if(complete){const float* values=i32(v.mode)==7||i32(v.mode)==17?v.start:v.end;return{values[0],values[1],values[2]};}
        const float t=v.timer.fractional/float(v.duration);
        for(u32 n=0;n<3;++n){
            FloatInterpolator axis;axis.start[0]=v.start[n];axis.end[0]=v.end[n];axis.initial_tangent[0]=v.initial_tangent[n];axis.final_tangent[0]=v.final_tangent[n];
            result[n]=motion_value(axis,axis_modes[n],t);v.start[n]=axis.start[0];v.final_tangent[n]=axis.final_tangent[0];
        }
        return{result[0],result[1],result[2]};
    }
    // 42cd40 shares the scalar easing arithmetic and one timer across all axes.
    // Sampling copies keeps mode 7/17's accumulated start/tangent per axis.
    Timer timer=v.timer;i32 duration=v.duration;
    for(u32 n=0;n<3;++n){
        FloatInterpolator axis;axis.start[0]=v.start[n];axis.end[0]=v.end[n];
        axis.initial_tangent[0]=v.initial_tangent[n];axis.final_tangent[0]=v.final_tangent[n];
        axis.timer=v.timer;axis.duration=v.duration;axis.mode=v.mode;
        result[n]=enemy_th14_motion_sample(axis,rate);
        v.start[n]=axis.start[0];v.final_tangent[n]=axis.final_tangent[0];
        timer=axis.timer;duration=axis.duration;
    }
    v.timer=timer;v.duration=duration;return {result[0],result[1],result[2]};
}
Vec2 enemy_th14_radius_sample(Vec2Interpolator& v,const float* rate)noexcept{
    float result[2];Timer timer=v.timer;i32 duration=v.duration;
    for(u32 n=0;n<2;++n){
        FloatInterpolator axis;axis.start[0]=v.start[n];axis.end[0]=v.end[n];
        axis.initial_tangent[0]=v.initial_tangent[n];axis.final_tangent[0]=v.final_tangent[n];
        axis.timer=v.timer;axis.duration=v.duration;axis.mode=v.mode;
        result[n]=enemy_th14_motion_sample(axis,rate);
        v.start[n]=axis.start[0];v.final_tangent[n]=axis.final_tangent[0];timer=axis.timer;duration=axis.duration;
    }
    v.timer=timer;v.duration=duration;return {result[0],result[1]};
}
void enemy_th14_ellipse_position(Movement& m)noexcept{
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
#if PORT_TITLE == 14
void enemy_th14_wave_position(Movement& m,EnemyState::Th14WaveMovement& wave,const float* rate)noexcept{
    // Retail 403f67..4040d7: drift the anchor, add the perpendicular
    // sine displacement, derive heading before the common 1/100 snap.
    const Vec3 previous=m.position;
    wave.anchor={float(wave.anchor.x+m.velocity.x),float(wave.anchor.y+m.velocity.y),float(wave.anchor.z+m.velocity.z)};
    const float heading=normalize_angle(float(m.ellipse_angle+1.5707963705062866f));
    const float amplitude=float(float(float(std::sin(double(wave.phase)))*m.radius) * *rate);
    const Vec2 p=polar(normalize_angle(heading),amplitude);
    m.position={float(wave.anchor.x+p.x),float(wave.anchor.y+p.y),float(wave.anchor.z+0.f)};
    const float dx=float(m.position.x-previous.x),dy=float(m.position.y-previous.y);
    m.angle=normalize_angle(float(std::atan2(double(dy),double(dx))));m.snap_position();
}
#endif
i32 enemy_th14_movement_command(EnemyState& e,EclContext& c,EnemyGlobals& g)noexcept{
#if PORT_TITLE == 14
    u16 op=c.instruction->opcode;auto f=[&](u32 n){return float(c.float_argument(n,g));};auto i=[&](u32 n){return c.integer_argument(n,g);};
    if(op==412||op==413){
        constexpr float pi=3.1415927410125732f,half_pi=1.5707963705062866f;
        const float half_x=e.clamp_size.x*.25f,half_y=e.clamp_size.y*.25f;
        const float x=e.current.position.x,y=e.current.position.y;float angle;
        auto& rng=g.environment.rng();
        if(x<e.clamp_center.x-half_x)angle=(rng.signed_unit()*pi)/3.f;
        else if(x>e.clamp_center.x+half_x)angle=normalize_angle((rng.signed_unit()*pi)/3.f+pi);
        else{
            angle=(rng.signed_unit()*pi)*.25f;const u32 choice=rng.next16()%3;
            if(g.environment.player_position.x>x?choice==0:choice!=0)angle+=pi;
        }
        if(y<e.clamp_center.y-half_y)angle=std::abs(angle);
        else if(y>e.clamp_center.y+half_y)angle=-std::abs(angle);
        if(std::abs(float(angle+half_pi))<.05f)angle=angle<-half_pi?-1.6207963228225708f:-1.5207964181900024f;
        else if(double(std::abs(float(angle-half_pi)))<.05)angle=angle<half_pi?1.5207964181900024f:1.6207963228225708f;
        const i32 mode=i(1);const float speed=f(2);const i32 duration=i(0);
        const u32 index=op==412?0:2;auto& a=e.motion_interpolation[index];auto& s=e.motion_interpolation[index+1];
        a.duration=duration;a.mode=InterpolationMode(mode);a.start[0]=a.end[0]=angle;
        a.initial_tangent[0]=a.final_tangent[0]=0;a.timer.set(0,&g.environment.rate);
        s.duration=i(0);s.start[0]=speed;s.end[0]=0;s.initial_tangent[0]=s.final_tangent[0]=0;s.timer.set(0,&g.environment.rate);
        // Retail 42756d writes the angle mode a second time; speed mode persists.
        (index==0?e.absolute:e.relative).flags&=~15u;return 0;
    }
    if(op==425||op==426||op==427){
        // 4261c6: Hermite position, arguments tangent0, endpoint, tangent1.
        auto& m=op==425?e.absolute:e.relative;auto& v=op==425?e.absolute_position:e.relative_position;
        const float x=f(3),y=f(4),tx=f(1),ty=f(2),ex=f(5),ey=f(6);const i32 duration=i(0);
        (op==425?e.th14_absolute_position_modes:e.th14_relative_position_modes).flags&=~1u;
        v.duration=duration;v.mode=InterpolationMode::Hermite;
        v.start[0]=m.position.x;v.start[1]=m.position.y;v.start[2]=m.position.z;
        v.end[0]=x>-999999.f?x:m.position.x;v.end[1]=y>-999999.f?y:m.position.y;v.end[2]=0;
        v.initial_tangent[0]=tx;v.initial_tangent[1]=ty;v.initial_tangent[2]=0;
        v.final_tangent[0]=ex;v.final_tangent[1]=ey;v.final_tangent[2]=0;
        v.timer.set(0,&g.environment.rate);m.flags&=~15u;return 0;
    }
    if(op==401||op==403||(op>=434&&op<=439)){
        const bool absolute=op==401||op==434||op==436||op==438;
        const bool separate=op==434||op==435||op==438||op==439;
        const bool offset=op>=436;
        auto& m=absolute?e.absolute:e.relative;auto& v=absolute?e.absolute_position:e.relative_position;
        auto& modes=absolute?e.th14_absolute_position_modes:e.th14_relative_position_modes;
        float x=f(separate?3:2),y=f(separate?4:3);const i32 duration=i(0);
        if(duration<=0){v.duration=0;return 0;}
        if(offset){x=(e.flags&0x80000)?float(m.position.x-x):float(m.position.x+x);y=float(m.position.y+y);}
        v.duration=i(0);
        if(separate){modes.flags|=1;modes.axis[0]=i(1);modes.axis[1]=i(2);}
        else{v.mode=InterpolationMode(i(1));modes.flags&=~1u;}
        v.start[0]=m.position.x;v.start[1]=m.position.y;v.start[2]=m.position.z;
        v.end[0]=x>-999999.f?x:m.position.x;v.end[1]=y>-999999.f?y:m.position.y;v.end[2]=0;
        for(u32 n=0;n<3;++n)v.initial_tangent[n]=v.final_tangent[n]=0;
        v.timer.set(0,&g.environment.rate);m.flags&=~15u;return 0;
    }
    if(op==424){e.flags=(e.flags&~0x80000u)|((u32(i(0))&1u)<<19);return 0;}
    if(op==414||op==415){
        // Retail 427199/4271c2 copies the player's raw XYZ; it does not
        // combine movement, reset velocity or alter the movement nibble.
        (op==414?e.absolute:e.relative).position=g.environment.player_position;return 0;
    }
    if(op==418||op==419){
        // Retail 426d48: write velocity XY and immediately refresh the
        // current movement mode. This command does not change the nibble.
        auto& m=op==418?e.absolute:e.relative;const float x=f(0),y=f(1);
        if(x>-999999.f)m.velocity.x=x;if(y>-999999.f)m.velocity.y=y;
        const u32 mode=m.flags&15;
        if(mode==0)m.update();
        else if(mode==2){const auto p=polar(m.angle,m.radius);m.position={float(m.velocity.x+p.x),float(m.velocity.y+p.y),float(m.velocity.z+0.f)};m.snap_position();}
        else if(mode==3)enemy_th14_ellipse_position(m);
        else if(mode==4){auto& wave=op==418?e.th14_absolute_wave:e.th14_relative_wave;wave.anchor=m.position;enemy_th14_wave_position(m,wave,&g.environment.rate);}
        else m.snap_position();
        e.combine_movement();
        // 426db8 falls through into 426dbd. The original then configures
        // the relative ellipse with six float reads (including words beyond
        // a conventional two-argument record). Preserve that retail tail.
        op=422;
    }
    if(op==420||op==422){
        auto& m=op==420?e.absolute:e.relative;
        const float values[]={f(0),f(1),f(2),f(3),f(4),f(5)};
        if((m.flags&15)!=2)m.velocity=m.position;
        if(values[0]>-999999.f)m.angle=normalize_angle(values[0]);
        if(values[1]>-999999.f)m.speed=values[1];if(values[2]>-999999.f)m.radius=values[2];if(values[3]>-999999.f)m.radial_velocity=values[3];
        if(values[4]>-999999.f)m.ellipse_angle=normalize_angle(values[4]);if(values[5]>-999999.f)m.ellipse_ratio=values[5];
        m.flags=(m.flags&~12u)|3u;enemy_th14_ellipse_position(m);e.combine_movement();return 0;
    }
    if(op==421||op==423){
        // Retail 426eee configures speed, radius/radial and ellipse bands.
        // Its ellipse band at +510/+554 is not sampled by 4230d0; preserve
        // that native behavior rather than ticking an invented consumer.
        auto& m=op==421?e.absolute:e.relative;
        auto& speed=e.motion_interpolation[op==421?1:3];
        auto& radius=op==421?e.absolute_radius:e.relative_radius;
        auto& ellipse=op==421?e.absolute_ellipse:e.relative_ellipse;
        const float values[]={f(2),f(3),f(4),f(5),f(6)};const i32 duration=i(0),mode=i(1);
        if(duration<=0){speed.duration=radius.duration=ellipse.duration=0;return 0;}
        speed.start[0]=m.speed;speed.end[0]=values[0]>-999999.f?values[0]:m.speed;
        speed.initial_tangent[0]=speed.final_tangent[0]=0;speed.duration=duration;speed.mode=InterpolationMode(mode);speed.timer.set(0,&g.environment.rate);
        const auto configure=[&](Vec2Interpolator& v,Vec2 start,Vec2 target){
            v.start[0]=start.x;v.start[1]=start.y;v.end[0]=target.x;v.end[1]=target.y;
            // Original 4fd2e8/4fd2ec are the zero-initialized default Vec2.
            for(u32 n=0;n<2;++n)v.initial_tangent[n]=v.final_tangent[n]=0;
            v.duration=duration;v.mode=InterpolationMode(mode);v.timer.set(0,&g.environment.rate);
        };
        configure(radius,{m.radius,m.radial_velocity},{values[1]>-999999.f?values[1]:m.radius,values[2]>-999999.f?values[2]:m.radial_velocity});
        configure(ellipse,{m.ellipse_angle,m.ellipse_ratio},{values[3]>-999999.f?values[3]:m.ellipse_angle,values[4]>-999999.f?values[4]:m.ellipse_ratio});
        m.velocity=m.position;m.flags=(m.flags&~12u)|3u;enemy_th14_ellipse_position(m);e.combine_movement();return 0;
    }
    if(op==440||op==442){
        auto& m=op==440?e.absolute:e.relative;float angle=f(0);
        if(e.flags&0x80000){constexpr float h=1.57079637050628662109375f;angle=normalize_angle(float(h-normalize_angle(float(angle-h))));}
        m.angle=normalize_angle(angle);m.flags&=~15u;return 0;
    }
    if(op==444||op==446){auto& m=op==444?e.absolute:e.relative;const float speed=f(0);if(speed>-999999.f)m.speed=speed;m.flags&=~15u;return 0;}
    if(op==441||op==443){
        const u32 index=op==441?0:2;auto& m=index==0?e.absolute:e.relative;auto& v=e.motion_interpolation[index];
        float target=f(2);const i32 duration=i(0);if(duration<=0){v.duration=0;return 0;}
        const i32 mode=i(1);
        if(target>-999999.f){if(e.flags&0x80000){
            if(mode==7)target=-target;else{constexpr float h=1.57079637050628662109375f;target=normalize_angle(float(h-normalize_angle(float(target-h))));}
        }}else target=mode==7?0.f:m.angle;
        // Native 4268ad adjusts the opposite endpoint from the paired command.
        float from=m.angle;constexpr float pi=3.1415927410125732421875f,tau=6.283185482025146484375f;
        if(std::abs(float(from-target))>=pi){if(target>from)target+=tau;else from+=tau;}
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
        auto& m=op==445?e.absolute:e.relative;auto& v=e.motion_interpolation[op==445?1:3];
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
        auto& angle=e.motion_interpolation[index];auto& speed=e.motion_interpolation[index+1];
        float to_angle=f(2),to_speed=f(3);const i32 duration=i(0);
        if(duration<=0){angle.duration=speed.duration=0;return 0;}
        const i32 mode=i(1);
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
    // 0x426b7f, the sibling: `mov esi,0x199` and the same `cmp cx,si` pick the record
    // (+0x88 for 409, +0xcc for 411) at 0x26b84, and two more selects of the same shape route the
    // interpolator records to +0x3f8/+0x488 (409) or +0x458/+0x4cc (411) at 0x26b99, 0x26bb1.
    // The two integer arguments (0x2c1d0 at 0x26c6e and 0x26c7b) are read before the branch, and
    // the three float arguments (0x2c230 at 0x26bc3, 0x26bd2, 0x26be1) are the interpolator ends
    // under the -999999.0 double of 0x4c1c78: 0x26bf8 keeps the record's own +0x18 when the first
    // is the sentinel, 0x26c35 the own +0x20, 0x26c1d the own +0x24. The frame's state changes
    // only while that first count is positive (0x26c8e `test ecx,ecx; jg`): 0x26d2b computes
    // (flags & 0xfffffff2) | 2 on the record, 0x26d37 calls the same 0x4180 the 408 path calls
    // (mode 2, 0x403e43) and 0x26d3e the same 0x23750 sum, i.e. the record's own angle and radius
    // bake one position step and the coordinate sum runs; a non-positive count only zeroes
    // +0x28/+0x3c of the interpolator records (0x26c92, 0x26c99) and jumps to the ECL tail at
    // 0x29c28, leaving the four compared records untouched.
    if(op==409||op==411){
        auto& m=op==409?e.absolute:e.relative;
        auto& speed=e.motion_interpolation[op==409?1:3];
        auto& radius=op==409?e.absolute_radius:e.relative_radius;
        const float to_speed=f(2),to_radius=f(3),to_radial=f(4);
        const i32 duration=i(0),mode=i(1);
        if(duration>0){
            speed.duration=duration;speed.mode=InterpolationMode(mode);
            speed.start[0]=m.speed;speed.end[0]=to_speed>-999999.f?to_speed:m.speed;
            speed.initial_tangent[0]=speed.final_tangent[0]=0;speed.timer.set(0,&g.environment.rate);
            radius.duration=duration;radius.mode=InterpolationMode(mode);
            radius.start[0]=m.radius;radius.start[1]=m.radial_velocity;
            radius.end[0]=to_radius>-999999.f?to_radius:m.radius;
            radius.end[1]=to_radial>-999999.f?to_radial:m.radial_velocity;
            for(u32 n=0;n<2;++n)radius.initial_tangent[n]=radius.final_tangent[n]=0;
            radius.timer.set(0,&g.environment.rate);
            m.flags=(m.flags&0xfffffff2u)|2u;
            const Vec2 p=polar(m.angle,m.radius);
            m.position={float(double(m.velocity.x)+p.x),float(double(m.velocity.y)+p.y),float(double(m.velocity.z)+0.)};
            m.snap_position();
            e.combine_movement();
        }else speed.duration=radius.duration=0;
        return 0;
    }
#else
    (void)e;(void)c;(void)g;
#endif
    return -2;
}
i32 enemy_th14_visibility(EnemyState& e)noexcept{
    const float half_x=float(e.visual_size.x*.5f),half_y=float(e.visual_size.y*.5f);
    if(float(e.current.position.x+half_x)<-192.f||float(e.current.position.x-half_x)>192.f)return (e.flags&0x10000)&&!(e.flags&4)?-1:0;
    if(float(e.current.position.y+half_y)<0.f||float(e.current.position.y-half_y)>448.f)return (e.flags&0x10000)&&!(e.flags&8)?-1:0;
    e.flags|=0x10000;return 0;
}
i32 enemy_th14_state_command(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& world)noexcept{
#if PORT_TITLE == 14
    const u16 op=c.instruction->opcode;
    if(op==556){
        const auto* name=reinterpret_cast<const char*>(c.instruction)+20;
        if(c.instruction->length<=20||!std::memchr(name,0,c.instruction->length-20))return -2;
        e.th14_death_script=name;return 0; // 4287ac does not resolve the length word.
    }
    if(op==565){e.th14_bomb_damage_ratio=float(c.float_argument(0,g));return 0;} // 427704.
    if(op==544){e.flags=(e.flags&~0x8000000u)|((u32(c.integer_argument(0,g))&1u)<<27);return 0;} // 428a27.
    if(op==553){e.damage_reserved=c.integer_argument(0,g);return 0;} // 42537c, hit-sound override +3fe8.
    auto i=[&](u32 n){return c.integer_argument(n,g);};auto f=[&](u32 n){return float(c.float_argument(n,g));};
    switch(op){
    case 615:case 616:{
        const float radius=f(0);if(!world.bullets||!world.lasers)return -2;
        if(!world.bullets->cancel_circle_th14(e.current.position,radius,op==615))return -2;
        return world.lasers->cancel_circle(e.current.position,radius,op==615?1u:0u,true)<0?-2:0;
    }
    case 704:case 705:case 706:case 707:case 708:case 709:case 710:{
        if(!world.lasers)return -2;auto* laser=world.lasers->find(i(0));
        if(op==710){
            while(laser){if(!world.lasers->cancel_object(*laser,false,false))return -2;laser->id=0;laser=world.lasers->find(i(0));}return 0;
        }
        // 704/705 read both float arguments even when lookup misses;
        // 706..709 read their second argument only for a found object.
        const float x=(laser||op<=705)?f(1):0;const float y=op<=705?f(2):0;if(!laser)return 0;
        if(op==704){laser->position={x,y,0};return 0;}
        if(op==706){laser->speed=x;return 0;}if(op==707){laser->width=x;return 0;}if(op==708){laser->angle=x;return 0;}
        u8* request=nullptr;
        if(laser->type==1)request=reinterpret_cast<Th14LaserInfinite*>(laser)->request;
        else if(laser->type==2)request=reinterpret_cast<Th14LaserCurve*>(laser)->path.request;
        else if(laser->type==3)request=reinterpret_cast<Th14LaserShell*>(laser)->request;
        else return -2;
        if(op==705){const Vec3 velocity{x,y,0};std::memcpy(request+0xc,&velocity,12);}
        else std::memcpy(request+0x1c,&x,4);return 0;
    }
    case 522:case 537:case 538:case 539:{
        const auto* instruction=c.instruction;
        if(!world.spells||!world.bosses[0]||instruction->length<32)return -2;
        const i32 size=instruction->argument<i32>(3);
        if(size<1||size>64||u32(size)>u32(instruction->length)-32)return -2;
        char name[64]{};u8 key=0x77,step=7;const auto* source=reinterpret_cast<const u8*>(instruction)+32;
        for(i32 n=0;n<size;++n){name[n]=char(source[n]^key);key=u8(key+step);step=u8(step+0x10);}
        if(!std::memchr(name,0,u32(size)))return -2;
        i32 id=i(0);if(op!=522)id=wrapping_add(id,wrapping_sub(g.environment.difficulty,i32(op)-537));
        // 428949 reads the unused third word before the duration word.
        (void)i(2);const i32 duration=i(1);world.spells->run_flags=world.th14_run_flags;
        if(!world.spells->begin(id,duration,name,world.bosses[0]->current.position,world.spells->bomb_active))return -2;
        e.health_flags|=1;e.scaled_health=signed_bits(u32(e.health)*7);e.timer_2bc.set(0,&g.environment.rate);return 0;
    }
    case 557:{
        if(!world.stage)return -2;
        const i32 duration=i(0),mode=i(1);const u32 color=u32(i(2));
        const float far_distance=f(4),near_distance=f(3);if(mode!=0)return -2;
        auto& s=world.stage->state;auto& v=s.fog_interpolation;
        v.start=s.camera.fog;v.end.set(color,near_distance,far_distance);
        v.duration=duration;v.mode=InterpolationMode(mode);v.timer.set(0,&g.environment.rate);return 0;
    }
    case 500:e.hitbox.x=f(0);e.hitbox.y=f(1);return 0;
    case 501:e.collision_box.x=f(0);e.collision_box.y=f(1);return 0;
    case 502:case 503:return enemy_th14_flags(e,op,u32(i(0)),world);
    case 504:e.flags|=0x20000;e.clamp_center.x=f(0);e.clamp_center.y=f(1);e.clamp_size.x=f(2);e.clamp_size.y=f(3);return 0;
    case 505:e.flags&=~0x20000u;return 0;
    case 506:std::memset(e.drops.counts,0,13*4);return 0;
    case 507:{const i32 index=i(0),value=i(1);if(u32(index)>13)return -2;if(index==0)e.drops.primary=value;else e.drops.counts[index-1]=value;return 0;}
    case 508:e.drops.spread.x=f(0);e.drops.spread.y=f(1);return 0;
    case 510:e.drops.primary=i(0);return 0;
    case 511:{const i32 value=i(0);e.health=e.max_health=e.interrupt_health=value;e.scaled_health=signed_bits(u32(value)*7);if(e.flags&0x800000)e.flags|=0x40000000;return 0;}
    case 512:{
        const i32 slot=i(0);if(slot>=i32(world.boss_capacity))return -2;
        world.manager_flags&=~1u;
        if(slot<0){
            if(e.flags&0x800000){if(u32(e.boss_slot)>=world.boss_capacity)return -2;world.bosses[e.boss_slot]=nullptr;}
            e.flags&=~0x800000u;
        }else{e.flags|=0x800000;world.bosses[slot]=&e;e.boss_slot=slot;}
        g.environment.boss=world.bosses[0];return 0;
    }
    case 513:e.timer_2bc.set(0,&g.environment.rate);return 0;
    case 520:
        // 42886e waits on the first three manager boss slots, not all eight.
        return world.bosses[0]||world.bosses[1]||world.bosses[2]?-1:0;
    case 519:return world.dialogue&&world.dialogue->dialogue_waiting()?-1:0;
    case 514:{
        const i32 time=i(2),health=i(1),slot=i(0);if(u32(slot)>=8)return -2;
        auto& interrupt=e.interrupts[slot];interrupt.health=health;
        if((world.th14_run_flags&0x30)==0x20&&(e.flags&0x800000)){
            // Retail4286d5 replaces the regular next phase with the two
            // shipped spell-practice exit routines, regardless of health.
            interrupt.health=0;interrupt.time=time;interrupt.health_script="BossDead";interrupt.timeout_script="BossEscape";return 0;
        }
        if(health>=0){
            const auto* command=c.instruction;
            if(command->length<=32)return -2;
            const char* name=reinterpret_cast<const char*>(command)+32;
            if(!std::memchr(name,0,command->length-32))return -2;
            interrupt.time=time;interrupt.health_script=interrupt.timeout_script=name;
        }
        return 0;
    }
    case 521:{
        const i32 slot=i(0);if(u32(slot)>=8||c.instruction->length<=24)return -2;
        const char* name=reinterpret_cast<const char*>(c.instruction)+24;
        if(!std::memchr(name,0,c.instruction->length-24))return -2;
        e.interrupts[slot].timeout_script=name;return 0;
    }
    case 629:{
        if(!world.deformations)return -2;
        if(e.deformation&&!world.deformations->release_deformation(e))return -2;
        e.deformation=nullptr;e.deformation_target=f(0);e.deformation_radius=16;e.deformation_color=u32(i(1));
        e.deformation_phase_x=e.deformation_phase_y=normalize_angle(0);
        return e.deformation_target>0&&!world.deformations->create_deformation(e)?-2:0;
    }
    case 630:return world.stage&&world.stage->interrupt(i(0))?0:-2; // 429940 -> 410470, STD marker seek.
    case 527:{
        // 42759b evaluates type, fraction, segment, then 421b90 writes the row.
        const i32 type=i(2);const float health=float(e.max_health),value=f(1)/health;
        const i32 slot=i(0);if(u32(slot)>=4||u32(e.boss_slot)>=3)return -2;
        world.th14_health_segments[e.boss_slot][slot]={value,type};return 0;
    }
    case 524:{const i32 section=i(0);if(world.stage_section!=section)world.section_frame=0;world.stage_section=section;return 0;}
    case 540:world.remaining_phases=i(0);return 0; // 429708, game +158.
    case 542:if(!world.spells)return -2;world.spells->invalidate_capture();return 0; // 4289fb, spell +7c bit 3.
    case 567:if(!world.spells)return -2;world.spells->title_orientation(i(0));return 0; // 428a13 -> 421c90.
    case 568:e.health_flags=(e.health_flags&~1u)|(u32(i(0))&1u);return 0; // 4289cb.
    case 564:e.animation_angle=f(0);return 0; // 4276ee: shared +120, raw f32.
    case 548:{
        // 429812 reads all four counts, selects by difficulty, then rewinds
        // the current ECL thread using a signed-int-to-f32 conversion.
        const i32 counts[]={i(0),i(1),i(2),i(3)};
        const u32 difficulty=u32(g.environment.difficulty);
        c.time-=float(counts[difficulty<3?difficulty:3]);return 0;
    }
    case 632:
        {const i32 value=i(0);if(value<0||value>6)return -2;
        e.tick_callback=EnemyTick(value);e.callbacks[0]=0;return 0;}
    case 633:{const i32 value=i(0);if(u32(value)>1)return -2;e.damage_callback=EnemyDamage(value);return 0;}
    case 547:{const float value=f(0);g.environment.rate=value;if(world.th14_rate_target)th14_rate_store(world.th14_rate_target,value);return 0;}
    case 637:{const i32 value=i(0);i32 out=0;return world.callbacks&&world.callbacks->invoke_th14(e,value,out)?0:-2;}
    case 535:{
        const i32 values[]={i(1),i(2),i(3),i(4)};
        auto* destination=c.integer_reference(0,g);if(!destination)return -2;
        const u32 difficulty=u32(g.environment.difficulty);*destination=values[difficulty<4?difficulty:3];return 0;
    }
    case 536:{
        const float values[]={f(1),f(2),f(3),f(4)};
        auto* destination=c.float_reference(0,g);if(!destination)return -2;
        const u32 difficulty=u32(g.environment.difficulty);*destination=values[difficulty<4?difficulty:3];return 0;
    }
    // 0x4285eb (700) and 0x42864d (701): the demo's own records of the pair sit in block 0x584
    // +0x17c with `length=36` and five parameters. Integer argument 0 indexes the slot table at
    // native +0x5a0 with the machine's own stride (`lea esi,[eax*8]; sub esi,eax; shl esi,7`),
    // and 700 stores its float arguments 1..4 at slot +0/+4/+8 and +0x338 while 701 stores its
    // integer arguments 1..5 at +0x348..+0x358. Neither handler reads anything else, and there is
    // no bounds check in the machine; the port refuses indices its sixteen slots cannot hold.
    case 700:{
        const u32 index=u32(i(0));if(index>=16u)return -2;auto& record=e.th14_emitters[index];
        record.position.x=f(1);record.position.y=f(2);record.position.z=f(3);
        const u32 fourth=float_bits(f(4));std::memcpy(&record.values_340[0],&fourth,4);return 0;
    }
    case 701:{
        const u32 index=u32(i(0));if(index>=16u)return -2;auto& record=e.th14_emitters[index];
        for(u32 n=0;n<5;++n)record.values_340[4+n]=u32(i(n+1));return 0;
    }
    // 0x428a49 (702), the pair's first consumer: the demo's record is block 0x584 +0x260,
    // `length=20` with one parameter, and the handler memsets a 0x354-byte stack request, copies the
    // slot's body (0xc6 dwords = 0x318 bytes, from the record's +0x20, measured as a `rep movsd`
    // whose source is the same +0x5a0 + index*0x380) into the request at +0x34, and then fills the
    // request's other fields. The two positions it can build are `base + offset` with z forced to
    // zero, when the offset triple's z is above [0x4c19e8] (bits 0x3f666666 - the comparison at
    // 0x28a95 loads it from the image, and `comiss`/`jbe` means "above" goes the zero-z way), and
    // `base + the entity's own position at +0x44` otherwise; `base` and `offset` are the
    // twelve-byte triples at native +0x3dd8 and +0x3e98 indexed by the same argument. The request's
    // +0x24 and +0x28 are the two words before the record (`+0x5a0 + index*0x380 - 8`), and
    // +0x30 has bit 0 set. The machine then calls the bullet spawner at 0x43a750 with the request;
    // its kind-0 request now feeds the independent TH14 straight-laser consumer.
    case 702:case 703:case 711:case 713:{
        const u32 index=u32(i(0));if(index>=16u)return -2;
        u8* request=e.th14_request.bytes;
        // The measured reads are all relative to the record's positioned part at +0x5a0, so the
        // pointer this transcription uses is the record's +8; the copy source at +0x20 here is the
        // machine's +0x5c0.
        const u8* whole=reinterpret_cast<const u8*>(&e.th14_emitters[index]);
        const u8* record=whole+8;
        std::memset(request,0,op==703?0x374:0x354);std::memcpy(request+(op==703?0x5c:op==711?0x2c:op==713?0x3c:0x34),record+0x20,0x318);
        const auto read=[record](u32 at){float value;std::memcpy(&value,record+at,4);return value;};
        const auto store=[request](u32 at,float value){std::memcpy(request+at,&value,4);};
        const auto store_word=[request](u32 at,u32 value){std::memcpy(request+at,&value,4);};
        const auto reader=[request](u32 at){u32 value;std::memcpy(&value,request+at,4);return value;};
        const auto& base=e.th14_base_vectors[index];const auto& offset=e.th14_offset_vectors[index];
        // [0x4c19e8] is not a guess: its four bytes are 0x3f666666.
        u32 bits=0x3f666666u;float limit;std::memcpy(&limit,&bits,4);
        float x,y,z;
        if(offset.z>limit)x=base.x+offset.x,y=base.y+offset.y,z=0;
        else{const Vec3& position=e.current.position;x=base.x+position.x;y=base.y+position.y;z=base.z+position.z;}
        store(0x00,x);store(0x04,y);store(0x08,z);
        store(op==703||op==713?0x18:0x0c,normalize_angle(read(0x0c)));
        if(op==703){
            // Retail 428c3b -> 43a750(kind 1). Unlike 702/711 this
            // request retains all 18 native 0x2c-byte ETEX records at +0x5c.
            const auto source_word=[record](u32 at){u32 v;std::memcpy(&v,record+at,4);return v;};
            const struct{u32 at,source;} fields[]={{0x20,4},{0x24,0},{0x28,0x338},{0x2c,0x14},{0x30,0x348},{0x34,0x34c},{0x38,0x350},{0x3c,0x354},{0x40,0x368},{0x44,0x36c},{0x4c,0x1c}};
            for(const auto field:fields)store_word(field.at,source_word(field.source));
            u32 sprite,color;std::memcpy(&sprite,whole,4);std::memcpy(&color,whole+4,4);
            store_word(0x48,u32(i(1)));store_word(0x50,sprite);store_word(0x54,color);store_word(0x58,source_word(0x358)|2u);
            if(!world.lasers)return -2;world.lasers->spawn_th14_infinite(request);return world.lasers->last_error?-2:0;
        }
        if(op==713){
            u32 color;std::memcpy(&color,whole+4,4);store(0x20,read(0x08));store(0x24,read(0x338));store_word(0x28,u32(i(1)));store_word(0x2c,color);store(0x30,read(0x1c));store(0x34,read(0x348));
            if(!world.lasers)return -2;world.lasers->spawn_th14_shell(request);return world.lasers->last_error?-2:0;
        }
        if(op==711){store(0x10,read(0x338));store(0x14,read(0x14));store(0x20,read(0x348));store(0x24,read(0x1c));}
        else{const struct{const u32 at,source;}named[]={{0x10,0x04},{0x14,0x00},{0x18,0x08},{0x1c,0x338},{0x20,0x14},{0x2c,0x1c}};for(const auto& field:named)store(field.at,read(field.source));}
        // The two words at +0x24/+0x28 are the record's own first two (sprite and colour).
        u32 first=0,second=0;std::memcpy(&first,whole,4);std::memcpy(&second,whole+4,4);
        const u32 shift=op==711?8u:0u;store_word(op==711?0x18:0x24,first);store_word(op==711?0x1c:0x28,second);store_word(0x30-shift,reader(0x30-shift)|1u);
        u32 tail=0,head=0;std::memcpy(&tail,record+0x368,4);std::memcpy(&head,record+0x36c,4);
        store_word(0x34c-shift,tail);store_word(0x350-shift,head);
        if(op==711){if(!world.lasers)return -2;world.lasers->spawn_th14_curve(request);return world.lasers->last_error?-2:0;}
        if(!world.lasers)return -2;world.lasers->spawn_th14_line(request);return world.lasers->last_error?-2:0;
    }
    default:return -2;
    }
#else
    (void)e;(void)c;(void)g;(void)world;return -2;
#endif
}
bool enemy_th14_spawn_parameters(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemySpawn& value,const char*& name)noexcept{
#if PORT_TITLE == 14
    const auto* instruction=c.instruction;const u16 op=instruction->opcode;
    if(op!=300&&op!=301&&op!=304&&op!=305&&op!=309&&op!=310&&op!=311&&op!=312&&op!=321)return false;
    if(instruction->length<20)return false;
    const u32 bytes=instruction->argument();
    // The original names are padded to a whole word; reject malformed lengths.
    if(!bytes||(bytes&3)||bytes>u32(instruction->length)-20)return false;
    const u32 start=20+bytes;
    if(u32(instruction->length)-start<20)return false;
    name=reinterpret_cast<const char*>(instruction)+20;
    if(!std::memchr(name,0,bytes))return false;
    auto raw=[&](u32 index){u32 v;std::memcpy(&v,reinterpret_cast<const u8*>(instruction)+start+(index-1)*4,4);return v;};
    auto f=[&](u32 index){const float v=float_bits(raw(index));return instruction->references&(1u<<index)?float(c.resolve_float(v,g)):v;};
    auto i=[&](u32 index){const i32 v=signed_bits(raw(index));return instruction->references&(1u<<index)?c.resolve_integer(v,g):v;};
    value={};value.position.x=f(1);value.position.y=f(2);
    if(op==300||op==309||op==321||op==311||op==304){value.position.x+=e.current.position.x;value.position.y+=e.current.position.y;}
    value.mirrored=op==311||op==304||op==312||op==305;
    if(e.flags&0x80000){value.position.x=-value.position.x;value.mirrored^=1;}
    value.health=i(3);value.score=i(4);value.drop=i(5);
    std::memcpy(value.integers,e.integers,48);
    value.parent_instance_id=u32(e.instance_id);
    return true;
#else
    (void)e;(void)c;(void)g;(void)value;(void)name;return false;
#endif
}
i32 enemy_th14_flags(EnemyState& e,u16 opcode,u32 bits,EnemyCommandEnvironment& world)noexcept{
#if PORT_TITLE == 14
    if(opcode!=502&&opcode!=503)return -2;
    // Retail 42771a/427753 updates +4054, then conditionally walks +124..+160.
    e.flags=opcode==502?e.flags|bits:e.flags&~bits;
    const bool visible=(e.flags&32)==0;
    if(opcode==502?visible:!visible)return 0;
    auto apply=[&](u32 id){if(!id)return true;if(!world.animation_visibility)return false;world.animation_visibility(id,visible,world.animation_user);return true;};
    for(const auto id:e.animations)if(!apply(id))return -2;
    for(const auto id:e.additional_animations)if(!apply(id))return -2;
    return 0;
#else
    (void)e;(void)opcode;(void)bits;(void)world;return -2;
#endif
}
}
