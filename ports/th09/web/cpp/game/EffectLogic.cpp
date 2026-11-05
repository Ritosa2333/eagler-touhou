#include "EffectManager.hpp"
#include "GameMath.hpp"
#include <cmath>
#include <algorithm>
namespace th09 {
namespace {
constexpr float pi=3.1415927410125732f,half_pi=1.5707963705062866f;
Vec3 polar(float angle,float radius){return {float(double(radius)*std::cos(double(angle))),float(double(radius)*std::sin(double(angle))),0};}
bool transfer_kind(EffectKind k){return k>=EffectKind::bullet_transfer_left&&k<=EffectKind::spirit_transfer_right;}
bool ring_kind(EffectKind k){return k==EffectKind::ring||k==EffectKind::ring_alternate||(k>=EffectKind::protection_one&&k<=EffectKind::protection_three);}
void focus_base(EffectActor& a){a.anchor=a.position;a.angle=a.arguments.x;a.origin={0,0,1};a.destination={0,-1,0};a.thickness=a.arguments.z;a.cycle=1;a.segments=32;a.radius=16;a.dirty=1;}
bool focus_kind(EffectKind k){return k==EffectKind::reimu_field||(k>=EffectKind::marisa_field&&k<=EffectKind::youmu_field)||k>=EffectKind::mystia_field;}
void turn_focus(EffectActor& a,const EffectServices& s,float fraction){const float target=float(add_angle(s.player_angles[a.side],pi));float delta=target-a.angle;if(target<=a.angle){if(delta<-pi)delta+=2*pi;}else if(delta>pi)delta-=2*pi;a.angle=float(add_angle(a.angle,delta*fraction));}
void transfer_curve(EffectActor& a){
    const float t=a.time.value()/a.cycle,u=t-1.f;
    const float start=((1.f+(t+t))*u)*u,end=((3.f-(t+t))*t)*t,out=((1.f-t)*(1.f-t))*t,in=(u*t)*t;
    a.position.x=((in*a.anchor.x+end*a.destination.x)+out*a.start_tangent.x)+start*a.origin.x;
    a.position.y=((end*a.destination.y+in*a.anchor.y)+out*a.start_tangent.y)+start*a.origin.y;
}
}
bool initialize_effect(EffectActor& a,EffectServices& s){
    if(transfer_kind(a.kind)){
        a.origin=a.position;a.destination=s.geometry[s.coordinate_side].to_screen(a.arguments);a.destination.z=a.arguments.z;
        const float r1=float(s.random.range(256))+64;const float t1=float(s.random.signed_unit())*pi;a.start_tangent=polar(t1,r1);
        const float r2=float(s.random.range(256))+64;const float t2=float(s.random.signed_unit())*pi;a.anchor=polar(t2,r2);
        a.cycle=float(s.random.range(100))+60;return true;
    }
    if(ring_kind(a.kind)){a.anchor=a.position;a.angle=a.arguments.x;a.origin={0,0,1};a.destination={0,-1,0};a.radius=a.arguments.y;a.thickness=a.arguments.z;a.segments=24;a.dirty=1;a.texture.resize(258);return true;}
    if(a.kind==EffectKind::slash_left||a.kind==EffectKind::slash_right){const auto p=polar(a.arguments.x,256);a.position.x+=p.x;a.position.y+=p.y;a.animation->rotation.z=float(add_angle(a.arguments.x,half_pi));return true;}
    if(!focus_kind(a.kind))return true;
    if(a.kind==EffectKind::lyrica_shots){s.start_animation(*a.animation,a.side,true,7);a.animation->flags|=0x2000;a.animation->pos2={};return true;}
    focus_base(a);a.colors.resize(33);
    switch(a.kind){
    case EffectKind::marisa_field:a.spread=.01f;break;
    case EffectKind::sakuya_field:a.angle=float(add_angle(s.player_angles[a.side],pi));a.segments=16;a.radius=160;break;
    case EffectKind::lyrica_field:case EffectKind::merlin_field:case EffectKind::lunasa_field:a.radius=0;break;
    case EffectKind::medicine_field:a.angle=0;a.radius=32;a.colors.resize(198);break;
    case EffectKind::cirno_field:a.angle=-half_pi;a.segments=16;a.radius=128;break;
    case EffectKind::komachi_field:a.angle=-half_pi;break;
    case EffectKind::yuuka_field:a.radius=640;break;
    case EffectKind::reisen_burst:a.burst=std::make_unique<EffectBurst>();break;
    default:break;
    }return true;
}
bool update_effect(EffectActor& a,EffectServices& s){
    if(transfer_kind(a.kind)){
        const bool spirit=a.kind==EffectKind::spirit_transfer_left||a.kind==EffectKind::spirit_transfer_right;
        if(!spirit&&a.rotation!=0){a.hidden=1;if(a.time.value()<a.rotation)return true;s.play_sound(46,a.side?500:-500);a.rotation=0;a.time.reset();a.hidden=0;return true;}
        if(a.time.value()<a.cycle){transfer_curve(a);return true;}if(s.rewards_blocked)return false;const i32 side=1-a.transfer.source_side;
        if(spirit){
            if(s.spirit_counts[side]>=10)return false;if(auto enemy=s.spawn_spirit(side,a.transfer.source_kind,a.arguments)){
                auto& v=enemy->values;v.flags=(v.flags&~0x1c0u)|(u32(i32(a.transfer.target_kind)<<6)&0x1c0);v.acceleration=a.transfer.speed+v.acceleration;
                if(s.random.bounded32(s.characters[side]==7?4:5)){v.direction=float(s.random.signed_unit())*.015707964077353477f+half_pi;}
                else {const auto& p=s.players[side];const auto& e=v.resolved_position;const float aim=(p.x==e.x&&p.y==e.y)?half_pi:float(std::atan2(double(p.y-e.y),double(p.x-e.x)));v.direction=float(s.random.signed_unit())*.39269909262657166f+aim;}
            }return false;
        }
        BulletEmission b;b.position=a.arguments;b.sprite=a.transfer.source_kind;
        if(i16(a.transfer.owner_flags)>=2&&s.random.bounded32(5)==0)b.sprite=3;
        b.color=a.transfer.target_kind;b.pattern=1;b.count=b.layers=1;
        if(b.sprite==3)b.angle=float(s.random.signed_unit())*.13089969754219055f+half_pi;
        else if(s.random.bounded32(3)){b.color=5;b.pattern=0;b.angle=float(s.random.signed_unit())*.22439947724342346f;}
        else{b.color=3;b.angle=float(s.random.signed_unit())*.2617993950843811f+half_pi;}
        b.speed=float(s.random.signed_unit())*.4f+a.transfer.speed;if(s.characters[side]==10)b.speed*=.93f;if(b.sprite==3)b.speed+=.3f;b.speed=std::min(float(s.rank)*.1f+3.f,std::max(b.speed,.8f));b.owner_flags=u8(a.transfer.owner_flags+1);b.flags=4;s.emit_bullets(side,b);return false;
    }
    if(ring_kind(a.kind)){auto& v=*a.animation;a.segments=v.intVar0;a.cycle=float(v.intVar1);a.thickness=v.scale.x;a.radius=v.pos2.x;a.distortion=v.pos2.y;a.angle=v.rotation.z;a.rotation=v.rotation.y;a.dirty=1;return true;}
    if(a.kind==EffectKind::focus_aura){a.position=s.players[a.side];return true;}
    if(a.kind==EffectKind::lyrica_shots){s.fire_shots(a.side,2,a.time.current);return true;}
    if(a.kind==EffectKind::reisen_burst){auto& b=*a.burst;if(b.phase==0){const float t=1.f-a.time.value()*.02f;b.radius=(1.f-t*t)*64.f;if(a.time.current>=50){b.phase=1;a.time.reset();}}
        else {for(auto& ring:b.jitter)for(u32 i=0;i<32;++i)ring[i]=float(s.random.signed_unit())*a.time.value();if(a.time.current>=30)return false;}return true;}
    if(!focus_kind(a.kind))return true;a.dirty=1;
    auto grow=[&](float limit,float speed){if(a.radius<limit)a.radius+=speed;};
    switch(a.kind){
    case EffectKind::reimu_field:grow(96,4);break;
    case EffectKind::marisa_field:if(a.time.current>=30&&a.spread<.08f)a.spread+=.001f;break;
    case EffectKind::sakuya_field:if(a.spread<half_pi)a.spread+=.05235987901687622f;turn_focus(a,s,.25f);break;
    case EffectKind::youmu_field:grow(144,.7f);break;
    case EffectKind::mystia_field:grow(48,8);break;
    case EffectKind::tewi_field:grow(56,4);break;
    case EffectKind::reisen_field:grow(112,8);turn_focus(a,s,.125f);break;
    case EffectKind::lyrica_field:grow(16,.5f);break;
    case EffectKind::aya_field:grow(320,8);a.angle=0;break;
    case EffectKind::medicine_field:grow(48,4);a.angle=float(add_angle(a.angle,.05235987901687622f));break;
    case EffectKind::cirno_field:if(a.spread<half_pi)a.spread+=.03141592815518379f;a.angle=-half_pi;break;
    case EffectKind::komachi_field:{grow(128,6);const float dx=s.player_steps[a.side];a.angle+=(dx<0?-2.094395160675049f-a.angle:dx>0?-1.0471975803375244f-a.angle:-half_pi-a.angle)*(dx==0?.05f:.02f);break;}
    case EffectKind::yuuka_field:if(a.radius<=0)a.dirty=0;else a.radius-=32;break;
    case EffectKind::eiki_field:grow(96,16);break;
    case EffectKind::merlin_field:grow(24,.5f);break;
    case EffectKind::lunasa_field:grow(8,.5f);break;
    default:break;
    }
    a.position=s.players[a.side];if(a.kind==EffectKind::tewi_field)a.position.y-=a.radius*1.5f;return true;
}
}
