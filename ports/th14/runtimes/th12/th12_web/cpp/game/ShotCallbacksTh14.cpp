#include "ShotManager.hpp"
#include <cmath>
#include <algorithm>
namespace th11 {
#if PORT_TITLE == 14
// Retail callback 4, 452c10, uses the visual RNG at 4db510.
void shot_th14_spread_spawn(ShotState& s,Rng& visual)noexcept{
    s.movement.angle=normalize_angle(s.movement.angle+visual.signed_unit()*.12217304855585098f);
    s.th14_shot_flags&=0xffffffc3u;
}
// Retail callback 4, 452e00. The acceleration is per invocation, even
// if the shot timer has not advanced, and stops during the hit animation.
void shot_th14_accelerating_update(ShotState& s)noexcept{
    if(s.state==1)s.movement.speed+=1.f;
}
// 452d20 uses the same spread angle as 452c10 without clearing flags.
void shot_th14_flame_spawn(ShotState& s,Rng& visual)noexcept{
    s.movement.angle=normalize_angle(s.movement.angle+visual.signed_unit()*.12217304855585098f);
}
// 452ca0 transforms older shots on a 1-in-6 visual RNG sample. The
// damage write is to the area, before the common update copies shot damage.
i32 shot_th14_flame_update(ShotState& s,Th14DamageArea* area,Rng& visual)noexcept{
    s.movement.speed=(s.movement.speed-2.5f)*.9200000166893005f+2.5f;
    if(s.timer.current<36||(s.th14_shot_flags&0x3c)||visual.next32()%6)return 0;
    s.th14_shot_flags=(s.th14_shot_flags&0xffffffc7u)|4;
    if(area)area->damage/=2;return 1;
}
// 452da0 does not test state or timer advancement before slowing down.
i32 shot_th14_small_flame_update(ShotState& s)noexcept{
    s.movement.speed=(s.movement.speed-1.f)*.9300000071525574f+1.f;
    return s.timer.current==3?1:0;
}
// 452e20 transfers the shot's movement to a separately allocated circle.
// This callback's ordinary projectiles use native movement mode zero.
bool shot_th14_explosive_hit(ShotState& s,ShotPlayer& player,Th14DamageArea& area)noexcept{
    if(!s.spec||(s.movement.flags&15))return false;
    area.period=4;s.state=2;s.th14_area_flags&=~1u;s.th14_area_slot=256;s.movement.speed=2;
    area.movement=s.movement;area.position=s.movement.position;area.moving=true;
    player.th14_focused_hits+=s.spec->native_tail[0];player.th14_hits_184bc+=s.spec->native_tail[1];return true;
}
// 4523a0 / 4523f0: the beam follows the fixed-point option record. Its
// collision rectangle has its own center; the ANM stays at the beam origin.
void shot_th14_beam_spawn(ShotState& s,Th14DamageArea& area)noexcept{
    s.th14_beam_length=0;area.period=1;area.size.x=0;s.th14_shot_flags&=~1u;
}
bool shot_th14_beam_update(ShotState& s,const ShotPlayer& player,Th14DamageArea& area,AnmVm* vm,bool firing,u32& cached_power,Th14BeamUpdate& out)noexcept{
    out={};if(!s.spec||s.spec->option<1||s.spec->option>8)return false;
    const auto& option=player.options[s.spec->option-1];
    s.movement.position={float(option.x)*.0078125f,float(option.y)*.0078125f,0};
    s.movement.angle=normalize_angle(s.spec->angle);
    if(s.state==2)return true;
    out.pan=i32((player.position.x*1000.f)/192.f);out.pan_active=true;
    if(s.th14_beam_length<512.f){s.th14_beam_length+=18.f;area.size.x=s.th14_beam_length;}
    const auto delta=polar(s.movement.angle,s.th14_beam_length*.5f);
    area.position={s.movement.position.x+delta.x,s.movement.position.y+delta.y,0};
    if(vm){vm->flags|=8|16;vm->sprite_size.x=s.th14_beam_length;vm->texture_scale.x=s.th14_beam_length*.001953125f;}
    else s.animation=0;
    if(!player.power_step)return false;
    if(s.state==1&&(!firing||s.spec->option-1>=player.option_count||cached_power!=u32(player.power/player.power_step)||player.focused||player.th14_dialogue_blocked||!player.th14_enemy_manager)){
        area.flags&=~1u;s.th14_area_flags&=~1u;s.state=2;out.interrupt=1;cached_power=0;out.stop=true;
    }
    if(!s.hit_this_frame&&s.state==1&&s.hit_last_frame==1){out.interrupt=3;s.hit_last_frame=0;}
    s.hit_this_frame=0;return true;
}
// 405b10 intersects a finite +/-1000 line with the four rectangle edges.
// Preserve the original float slope arithmetic and edge order, including
// sorting from the negative line endpoint rather than the beam origin.
bool shot_th14_beam_intersection(Vec3 origin,Vec3 center,Vec2 size,float angle,float beam_angle,Vec2& first)noexcept{
    const float hx=size.x*-.5f,hy=size.y*-.5f;
    std::array<Vec2,4> corners={Vec2{hx,hy},Vec2{hx,-hy},Vec2{-hx,-hy},Vec2{-hx,hy}};
    const float sn=float(std::sin(double(angle))),cs=float(std::cos(double(angle)));
    for(auto& p:corners){if(beam_angle!=0)p={p.x*cs-p.y*sn,p.y*cs+p.x*sn};p.x+=center.x;p.y+=center.y;}
    const float s=float(std::sin(double(beam_angle))),c=float(std::cos(double(beam_angle)));
    const float dx=s*0.f-c*1000.f,dy=c*0.f+s*1000.f;
    // The retail helper's sin/cos return order yields x=cos, y=sin.
    const Vec2 plus{origin.x-dx,origin.y+dy},minus{origin.x+dx,origin.y-dy};
    std::array<Vec2,2> points{};u32 count=0;
    const auto side=[](Vec2 a,Vec2 b,Vec2 p){return (b.x-p.x)*(b.y-a.y)+(p.y-b.y)*(b.x-a.x);};
    for(u32 i=0;i<4&&count<2;++i){const auto q=corners[i],r=corners[(i+1)%4];
        const float u=side(plus,minus,q),v=side(plus,minus,r);if(u*v>0)continue;
        if(u==0&&v==0){auto aa=plus,bb=minus,qq=q,rr=r;if(bb.x>aa.x)std::swap(aa,bb);if(qq.x>rr.x)std::swap(qq,rr);
            if(rr.x<bb.x||rr.y<bb.y||aa.x<qq.x||aa.y<qq.y)continue;
        }else if(side(q,r,plus)*side(q,r,minus)>0)continue;
        const float line_dx=plus.x-minus.x,line_dy=plus.y-minus.y,edge_dx=r.x-q.x,edge_dy=r.y-q.y;
        const bool lv=std::abs(line_dx)<.01f,ev=std::abs(edge_dx)<.01f;
        const float ls=lv?0:line_dy/line_dx,li=lv?minus.x:minus.y-(line_dy*minus.x)/line_dx;
        const float es=ev?0:edge_dy/edge_dx,ei=ev?q.x:q.y-(q.x*edge_dy)/edge_dx;
        Vec2 p;
        if(!lv&&!ev){const float d=ei-li,den=ls-es;p={d/den,(ls*d)/den+li};}
        else if(!lv)p={q.x,ls*q.x+li};
        else if(!ev)p={minus.x,minus.x*es+ei};
        else{if(std::abs(minus.x-q.x)>=.001f)continue;p=minus;}
        points[count++]=p;
    }
    if(!count)return false;first=points[0];
    if(count==2){const auto distance=[&](Vec2 p){const float x=minus.x-p.x,y=minus.y-p.y;return x*x+y*y;};if(distance(points[1])<=distance(points[0]))first=points[1];}
    return true;
}
bool shot_th14_beam_hit(ShotState& shot,ShotPlayer& player,Th14DamageArea& area,Vec3 target,const Vec2* size,float radius,float angle,Th14BeamHit& out)noexcept{
    out={};if(!player.th14_native_stack)return false;
    shot.hit_this_frame=1;if(!shot.hit_last_frame){out.interrupt=true;shot.hit_last_frame=1;}
    const auto origin=shot.movement.position;const float dx=target.x-origin.x,dy=target.y-origin.y;
    if(!size){const float r=area.size.y*.5f+radius,s=float(std::sin(-double(shot.movement.angle))),c=float(std::cos(-double(shot.movement.angle)));
        const float x=dx*c-dy*s,y=dy*c+dx*s;
        if(r==0)return false;
        if(std::abs(y)>r||x<-r||(x<0&&x*x+y*y>r*r)){
            const u32 bits=NativeStackTh14::fallback_bits(player.th14_native_stack->beam_fallback_scratch());
            std::memcpy(&shot.th14_beam_length,&bits,4);
        }else{
            const float t=y/r;shot.th14_beam_length=std::max(0.f,x-float(std::sqrt(double(1.f-t*t)))*r+8.f);
        }
        area.size.x=shot.th14_beam_length;
    }else{Vec2 p;const Vec2 expanded{size->x+area.size.y,size->y+area.size.y};
        if(shot_th14_beam_intersection(origin,target,expanded,angle,shot.movement.angle,p)){
            const float x=p.x-origin.x,y=p.y-origin.y,aim=float(std::atan2(double(y),double(x)));float d=aim-shot.movement.angle;
            player.th14_native_stack->rectangle_intersection_y(y);
            if(d>3.1415927410125732f)d=aim-(shot.movement.angle+6.2831854820251465f);
            else if(shot.movement.angle-aim>3.1415927410125732f)d=aim-(shot.movement.angle-6.2831854820251465f);
            shot.th14_beam_length=std::abs(d)<1.5707963705062866f?float(std::sqrt(double(y*y+x*x)))+8.f:8.f;
            shot.th14_beam_length=std::max(0.f,shot.th14_beam_length);area.size.x=shot.th14_beam_length;
        }else{shot.th14_beam_length=std::max(0.f,float(std::sqrt(double(dx*dx+dy*dy)))-24.f);area.size.x=shot.th14_beam_length+16.f;}
    }
    if(shot.timer.current!=shot.timer.previous){if(shot.timer.current%2==0){out.effect=true;out.angle=shot.movement.angle;
        const auto endpoint=polar(out.angle,shot.th14_beam_length),travel=polar(out.angle,64);out.position={origin.x+endpoint.x,origin.y+endpoint.y,0};out.travel={travel.x,travel.y,0};}
        if(shot.timer.current%4==0){player.th14_plain_hits+=2;out.damage=area.damage;}
    }return true;
}
#endif
}
