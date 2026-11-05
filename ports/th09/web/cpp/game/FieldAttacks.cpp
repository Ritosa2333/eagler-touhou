#include "AttackTravel.hpp"
#include "GameMath.hpp"
#include <cmath>
#include <algorithm>
namespace th09 {
namespace {
constexpr float pi=3.1415927410125732f,sector=.20268340480327606f;
Vec3 polar(float a,float r){return {float(std::cos(double(a))*double(r)),float(std::sin(double(a))*double(r)),0};}
Vec3 plus(const Vec3& a,const Vec3& b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
template<class T>T& allocate(AttackActor& a){a.animations.resize(1);std::memset(a.animations.data(),0,sizeof(AnmVm));a.state=std::make_unique<T>();return static_cast<T&>(*a.state);}
void tangents(TravelAttackState& t,CharacterAttackServices& s){
    float r=float(s.random.range(192)),angle=float(s.random.signed_unit())*pi;t.start_tangent=polar(angle,r);
    r=float(s.random.range(192));angle=float(s.random.signed_unit())*pi;t.end_tangent=polar(angle,r);
}
void endpoints(AttackActor& a,TravelAttackState& t,const CharacterAttackServices& s){t.origin=s.geometry[a.source_side].to_screen(a.position);t.destination=s.geometry[a.destination_side].to_screen(t.target);}
bool initialize_reisen_common(AttackActor& a,CharacterAttackServices& s,bool scripted){
    auto& t=allocate<ReisenAttackState>(a);if(!s.start_animation(a,0,a.source_side,AttackAnimationResource::shared_effects,37))return true;
    if(scripted&&!a.extra_position)return true;
    t.velocity=polar(scripted?a.extra_position->z:float(s.random.range(.19634954631328583f))+1.5707963705062866f,2);
    a.layer=2;tangents(t,s);
    if(scripted)t.target={a.extra_position->x,a.extra_position->y,0};
    else {t.target.x=float(s.random.signed_unit())*(s.geometry[a.source_side].width*.5f-8.f);t.target.y=float(s.random.range(128));}
    endpoints(a,t,s);return false;
}
bool initialize_reisen(AttackActor& a,AttackServices& s){return initialize_reisen_common(a,static_cast<CharacterAttackServices&>(s),false);}
bool initialize_reisen_script(AttackActor& a,AttackServices& s){return initialize_reisen_common(a,static_cast<CharacterAttackServices&>(s),true);}
bool arrive(AttackActor& a,TravelAttackState& t,CharacterAttackServices& s,i32 script){
    interpolate_attack_travel(a,t);if(a.time.current<=90)return false;
    ++t.phase;a.position=t.target;a.layer=a.destination_side;a.time.reset();s.start_animation(a,0,a.source_side,AttackAnimationResource::character_shots,script);return true;
}
bool update_reisen(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<ReisenAttackState&>(*a.state);
    switch(t.phase){
    case 0:arrive(a,t,s,9);break;
    case 1:{
        const float dx=s.players[a.destination_side].x-a.position.x,dy=s.players[a.destination_side].y-a.position.y;
        a.position=plus(t.velocity,a.position);
        if(s.probe_cancellation(a.destination_side,a.position,{24,24,0})==2||a.position.y>480)return true;
        if(dx*dx+dy*dy<9216.f){s.play_sound(16,a.destination_side?500:-500);auto& v=a.animations[0];v.visible=0;v.currentInstruction=nullptr;v.color1.d3dColor=-1;++t.phase;a.time.reset();}break;
    }
    case 2:{
        const float remaining=1.f-a.time.value()*.02083333395421505f;t.radius=(1.f-remaining*remaining)*64.f;
        if(a.time.current>1){
            if(s.probe_cancellation(a.destination_side,a.position,{24,24,0})==2){++t.phase;a.time.reset();}
            else s.collide_player(a.destination_side,a.position,t.radius*.8f);
            if(a.time.current>=30){++t.phase;a.time.reset();}
        }break;
    }
    case 3:
        for(u32 ring=0;ring<4;++ring){t.rings[ring][0].position.z=0;for(u32 j=0;j<32;++j)t.jitter[ring][j]=float(s.random.signed_unit())*a.time.value();}
        return a.time.current>=30;
    }return false;
}
void draw_reisen(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<ReisenAttackState&>(*a.state);
    if(t.phase>1){
        const auto& g=s.geometry[a.destination_side];const u8 alpha=t.phase==2?255:u8(255-a.time.current*255/30);const u32 color=u32(alpha)<<24;
        float angle=-pi;t.fan[0]={g.to_screen(a.position),1,(u32(alpha)<<23)|0xffffff};
        for(u32 j=1;j<33;++j){if(angle>=pi)angle-=2*pi;t.fan[j]={g.to_screen(plus(polar(angle,t.radius),a.position)),1,color|0xff4040};angle+=sector;}
        a.animations[0].color1.d3dColor=-1;s.draw_color_fan(a.animations[0],t.fan.data(),33);
        for(u32 ring=0;ring<4;++ring){
            for(u32 j=0;j<32;++j){if(angle>=pi)angle-=2*pi;t.rings[ring][j]={g.to_screen(plus(polar(angle,t.jitter[ring][j]+t.radius),a.position)),1,color|0xffffff};angle+=sector;}
            s.draw_additive_lines(t.rings[ring].data(),32);angle+=.050670851200819016f;
        }
    }t.draw_flag=0;
}
bool initialize_field(AttackActor& a,CharacterAttackServices& s,bool eiki){
    auto& t=allocate<FieldAttackState>(a);if(!s.start_animation(a,0,a.source_side,AttackAnimationResource::shared_effects,37))return true;
    t.velocity=polar(float(s.random.range(-pi)),2);a.layer=2;tangents(t,s);
    t.target.x=float(s.random.signed_unit())*(s.geometry[a.source_side].width*.5f-24.f);
    t.target.y=eiki?float(s.random.range(128))+128.f:448.f-float(s.random.range(256));endpoints(a,t,s);
    t.world[0]=t.target;t.vertices[0].reciprocal_w=1;t.vertices[0].uv={.5f,.5f};if(eiki)t.uv_angle=-pi;
    float angle=-pi,velocity=float(s.random.signed_unit())*.06666667014360428f;
    for(u32 j=0;j<32;++j){
        if(angle>=pi)angle-=2*pi;auto& v=t.vertices[j+1];v.reciprocal_w=1;const auto uv=polar(angle,.5f);v.uv={uv.x+.5f,uv.y+.5f};
        t.radii[j]=float(s.random.signed_unit())*8.f+80.f;t.radial_velocity[j]=velocity;
        velocity=std::clamp(float(s.random.signed_unit())*.03333333507180214f+velocity,-.06666667014360428f,.06666667014360428f);
        t.world[j+1]=plus(polar(angle,t.radii[j]),t.target);angle+=sector;
    }
    if(!eiki){t.uv_velocity.x=float(s.random.signed_unit())*.008333333767950535f;t.uv_velocity.y=float(s.random.signed_unit())*.008333333767950535f;}
    return false;
}
bool initialize_yuuka(AttackActor& a,AttackServices& s){return initialize_field(a,static_cast<CharacterAttackServices&>(s),false);}
bool initialize_eiki(AttackActor& a,AttackServices& s){return initialize_field(a,static_cast<CharacterAttackServices&>(s),true);}
void advance_uv(FieldAttackState& t,u32 vertex,bool vertical){
    auto& uv=t.vertices[vertex].uv;float& coordinate=vertical?uv.y:uv.x;
    // Both axes use X's velocity in the original, including its wrap rule.
    coordinate+=t.uv_velocity.x;if(coordinate<0)for(auto& v:t.vertices){if(vertical)v.uv.y+=1;else v.uv.x+=1;}
}
bool update_yuuka(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<FieldAttackState&>(*a.state);
    if(t.phase==0){if(arrive(a,t,s,9))a.animations[0].flag1=0;}
    else if(t.phase==1){
        advance_uv(t,0,false);advance_uv(t,0,true);float angle=-pi;
        for(u32 j=0;j<32;++j){advance_uv(t,j+1,false);advance_uv(t,j+1,true);t.radii[j]=t.radial_velocity[j]+t.radii[j];t.world[j+1]=plus(polar(angle,t.radii[j]),t.target);angle+=sector;}
        if(a.time.current<300&&a.time.current>20){const float dx=s.players[a.destination_side].x-a.position.x,dy=s.players[a.destination_side].y-a.position.y;if(dx*dx+dy*dy<=4096.f)s.scale_player_effect(a.destination_side,.4f);}
        return !a.animations[0].visible;
    }else if(t.phase==2)return a.time.current>20;return false;
}
void reflect_bullets(AttackActor& a,CharacterAttackServices& s,bool secondary){
    auto& pool=s.bullet_manager(a.destination_side).pool;const u32 begin=secondary?BulletManager::second_begin:0,count=secondary?BulletManager::second_capacity:BulletManager::first_capacity;
    for(u32 j=begin;j<begin+count;++j){auto& b=pool[j];if(!b.state||b.homing||b.base_sprite==0x14a)continue;
        const float dx=b.position.x-a.position.x,dy=b.position.y-a.position.y;if(dx*dx+dy*dy>1024.f)continue;
        b.homing=1;BulletEmission shot;shot.sprite=22;shot.position=b.position;shot.speed=b.speed+(secondary?.7f:1.f);shot.angle=b.direction;shot.count=shot.layers=shot.pattern=1;
        if(auto result=s.emit_bullets(a.destination_side,secondary,shot))result->homing=1;
    }
}
bool update_eiki(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<FieldAttackState&>(*a.state);
    if(t.phase==0){if(arrive(a,t,s,11))a.animations[0].flag1=0;}
    else if(t.phase==1){
        float uv_angle=t.uv_angle,angle=-pi;
        for(u32 j=0;j<32;++j){const auto uv=polar(uv_angle,.5f);t.vertices[j+1].uv={uv.x+.5f,uv.y+.5f};uv_angle=float(add_angle(uv_angle,sector));
            t.radii[j]=t.radial_velocity[j]+t.radii[j];t.world[j+1]=plus(polar(angle,t.radii[j]),t.target);angle+=sector;}
        t.uv_angle=float(add_angle(t.uv_angle,.05235987901687622f));reflect_bullets(a,s,false);reflect_bullets(a,s,true);return !a.animations[0].visible;
    }else if(t.phase==2)return a.time.current>20;return false;
}
void draw_field(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<FieldAttackState&>(*a.state);
    if(t.phase>0){const auto& g=s.geometry[a.destination_side];auto& center=t.vertices[0];center.position=g.to_screen(t.world[0]);center.color=(u32(a.animations[0].color1.a)<<24)|0xffffff;
        for(u32 j=1;j<32;++j){t.vertices[j].position=g.to_screen(t.world[j]);t.vertices[j].color=0x8080ff;}
        t.vertices[32]=t.vertices[1];s.draw_texture_fan(a.animations[0],t.vertices.data(),33);
    }t.draw_flag=0;
}
}
void add_field_attack_behaviors(std::array<AttackBehavior,27>& result){
    result[14]={initialize_reisen,update_reisen,draw_reisen,nullptr};result[22]={initialize_reisen_script,update_reisen,draw_reisen,nullptr};
    result[19]={initialize_yuuka,update_yuuka,draw_field,nullptr};result[21]={initialize_eiki,update_eiki,draw_field,nullptr};
}
}
