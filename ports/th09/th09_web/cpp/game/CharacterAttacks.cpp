#include "CharacterAttacks.hpp"
#include "GameMath.hpp"
#include "AttackTravel.hpp"
#include <cmath>
namespace th09 {
namespace {
constexpr float pi=3.1415927410125732f;
enum class TravelDestination {top,script_nearby,script_exact,bottom,youmu,lyrica,merlin,lunasa,komachi};
Vec3 polar(float angle,float radius){return {float(std::cos(double(angle))*double(radius)),float(std::sin(double(angle))*double(radius)),0};}
template<class T> T& allocate_travel(AttackActor& a,u32 animation_count=1){
    a.animations.resize(animation_count);std::memset(a.animations.data(),0,animation_count*sizeof(AnmVm));a.state=std::make_unique<T>();return static_cast<T&>(*a.state);
}
void random_tangents(TravelAttackState& t,Rng& r){
    float radius=float(r.range(192)),angle=float(r.signed_unit())*pi;t.start_tangent=polar(angle,radius);
    radius=float(r.range(192));angle=float(r.signed_unit())*pi;t.end_tangent=polar(angle,radius);
}
void screen_endpoints(AttackActor& a,TravelAttackState& t,CharacterAttackServices& s){
    t.origin=s.geometry[a.source_side].to_screen(a.position);t.origin.z=0;
    t.destination=s.geometry[a.destination_side].to_screen(t.target);t.destination.z=0;
}
template<class State=TravelAttackState> bool initialize_travel(AttackActor& a,CharacterAttackServices& s,TravelDestination kind,u32 animation_count=1){
    auto& t=allocate_travel<State>(a,animation_count);
    if(!s.start_animation(a,0,a.source_side,AttackAnimationResource::shared_effects,37))return true;
    if(kind!=TravelDestination::youmu)t.velocity=polar(float(s.random.range(-pi)),2);a.layer=2;
    if(kind==TravelDestination::script_nearby||kind==TravelDestination::script_exact)a.time.reset(90);
    random_tangents(t,s.random);
    t.origin=s.geometry[a.source_side].to_screen(a.position);t.origin.z=0;
    if(kind==TravelDestination::script_nearby){
        t.target.x=float(s.random.signed_unit())*64.f+a.position.x;
        // The original script-triggered variant uses source X for both axes.
        t.target.y=float(s.random.signed_unit())*64.f+a.position.x;
    }else if(kind==TravelDestination::script_exact)t.target={a.position.x,a.position.y,0};
    else if(kind==TravelDestination::youmu){
        t.target.x=float(s.random.signed_unit())*128.f+s.players[a.destination_side].x;
        t.target.y=float(s.random.signed_unit())*128.f+s.players[a.destination_side].y;
        if(t.target.y>432)t.target.y=432;if(t.target.x<-128)t.target.x=-128;else if(t.target.x>128)t.target.x=128;
    }else {
        t.target.x=float(s.random.signed_unit())*(s.geometry[a.source_side].width*.5f-8.f);
        if(kind==TravelDestination::bottom)t.target.y=448;
        else if(kind==TravelDestination::lyrica)t.target.y=float(s.random.range(200))+128.f;
        else if(kind==TravelDestination::merlin)t.target.y=float(s.random.range(128))+32.f;
        else if(kind==TravelDestination::lunasa)t.target.y=float(s.random.range(128))+200.f;
        else if(kind==TravelDestination::komachi)t.target.y=float(s.random.range(144))+32.f;
        else t.target.y=float(s.random.range(128));
    }
    t.destination=s.geometry[kind==TravelDestination::script_nearby?a.source_side:a.destination_side].to_screen(t.target);t.destination.z=0;
    if(kind==TravelDestination::komachi){t.speed=1;t.heading=float(s.random.signed_unit())*pi;t.turn=s.random.below(2)?0.10471975803375244f:-0.10471975803375244f;}
    return false;
}
bool initialize_reimu(AttackActor& a,AttackServices& s){return initialize_travel(a,static_cast<CharacterAttackServices&>(s),TravelDestination::top);}
bool initialize_reimu_script(AttackActor& a,AttackServices& s){return initialize_travel(a,static_cast<CharacterAttackServices&>(s),TravelDestination::script_nearby);}
bool initialize_marisa(AttackActor& a,AttackServices& s){return initialize_travel(a,static_cast<CharacterAttackServices&>(s),TravelDestination::bottom);}
bool initialize_youmu(AttackActor& a,AttackServices& s){return initialize_travel(a,static_cast<CharacterAttackServices&>(s),TravelDestination::youmu);}
bool initialize_lyrica(AttackActor& a,AttackServices& s){return initialize_travel(a,static_cast<CharacterAttackServices&>(s),TravelDestination::lyrica);}
bool initialize_merlin(AttackActor& a,AttackServices& s){return initialize_travel(a,static_cast<CharacterAttackServices&>(s),TravelDestination::merlin);}
bool initialize_lunasa(AttackActor& a,AttackServices& s){return initialize_travel(a,static_cast<CharacterAttackServices&>(s),TravelDestination::lunasa);}
bool initialize_komachi(AttackActor& a,AttackServices& s){return initialize_travel(a,static_cast<CharacterAttackServices&>(s),TravelDestination::komachi);}
bool initialize_medicine(AttackActor& a,AttackServices& s){return initialize_travel<MedicineAttackState>(a,static_cast<CharacterAttackServices&>(s),TravelDestination::top,4);}
bool initialize_medicine_script(AttackActor& a,AttackServices& s){return initialize_travel<MedicineAttackState>(a,static_cast<CharacterAttackServices&>(s),TravelDestination::script_exact,4);}
bool initialize_cirno_common(AttackActor& a,CharacterAttackServices& s,bool scripted){
    auto& t=allocate_travel<CirnoAttackState>(a);if(!s.start_animation(a,0,a.source_side,AttackAnimationResource::shared_effects,37))return true;
    a.layer=2;
    if(scripted){t.acceleration={float(s.random.signed_unit())*.001f,.025f,0};a.time.reset(40);}else t.acceleration={0,.028f,0};
    random_tangents(t,s.random);
    t.target=scripted?Vec3{a.position.x,a.position.y,0}:Vec3{float(s.random.signed_unit())*32.f+s.players[a.destination_side].x,16,0};
    screen_endpoints(a,t,s);return false;
}
bool initialize_cirno(AttackActor& a,AttackServices& s){return initialize_cirno_common(a,static_cast<CharacterAttackServices&>(s),false);}
bool initialize_cirno_script(AttackActor& a,AttackServices& s){return initialize_cirno_common(a,static_cast<CharacterAttackServices&>(s),true);}
bool initialize_tewi_common(AttackActor& a,CharacterAttackServices& s,bool scripted){
    auto& t=allocate_travel<TewiAttackState>(a);if(!s.start_animation(a,0,a.source_side,AttackAnimationResource::shared_effects,37))return true;
    if(scripted)a.time.reset(90);t.velocity=polar(float(s.random.range(-pi)),2);a.layer=2;random_tangents(t,s.random);
    if(scripted)t.target={a.position.x,a.position.y,0};
    else {t.target.x=float(s.random.signed_unit())*(s.geometry[a.source_side].width*.5f-64.f);t.target.y=float(s.random.range(128));}
    screen_endpoints(a,t,s);return false;
}
bool initialize_tewi(AttackActor& a,AttackServices& s){return initialize_tewi_common(a,static_cast<CharacterAttackServices&>(s),false);}
bool initialize_tewi_script(AttackActor& a,AttackServices& s){return initialize_tewi_common(a,static_cast<CharacterAttackServices&>(s),true);}
bool initialize_aya(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=allocate_travel<AyaAttackState>(a);
    if(!s.start_animation(a,0,a.source_side,AttackAnimationResource::shared_effects,37))return true;
    s.random.range(-pi);a.layer=2;random_tangents(t,s.random);
    t.target.x=float(s.random.signed_unit())*(s.geometry[a.source_side].width*.5f-8.f);t.target.y=float(s.random.range(128));screen_endpoints(a,t,s);
    t.velocity.y=float(s.random.range(2))+.5f;return false;
}
SakuyaAttackState& allocate_sakuya(AttackActor& a){
    a.animations.resize(5);std::memset(a.animations.data(),0,5*sizeof(AnmVm));a.state=std::make_unique<SakuyaAttackState>();return static_cast<SakuyaAttackState&>(*a.state);
}
bool initialize_sakuya(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=allocate_sakuya(a);
    for(u32 i=0;i<5;++i)if(!s.start_animation(a,i,a.source_side,AttackAnimationResource::character_shots,i32(i)+9))return true;
    const auto target=s.geometry[a.destination_side].to_screen(s.players[a.destination_side]);a.position=s.geometry[a.source_side].to_screen(a.position);a.layer=2;
    const float dx=target.x-a.position.x,dy=target.y-a.position.y;a.angle=float(std::atan2(double(dy),double(dx)));t.velocity=polar(a.angle,3.5f);
    s.spawn_attack(4,a.source_side,a.position,nullptr,&a);t.history.fill(a.position);return false;
}
bool initialize_sakuya_collision(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=allocate_sakuya(a);
    for(u32 i=0;i<5;++i)if(!s.start_animation(a,i,a.source_side,AttackAnimationResource::character_shots,i32(i)+14))return true;
    if(!a.parent||!a.parent->state)return true;t.parent=a.parent;a.position=s.geometry[a.destination_side].to_local(a.position);a.layer=a.destination_side;a.angle=a.parent->angle;
    t.velocity=static_cast<SakuyaAttackState&>(*a.parent->state).velocity;t.history.fill(a.position);return false;
}
void advance_trail(AttackActor& a,SakuyaAttackState& t){
    for(u32 i=31;i>0;--i)t.history[i]=t.history[i-1];t.history[0]=a.position;
    for(u32 i=1;i<5;++i){const auto& p=t.history[i*6];a.animations[i].pos2={p.x-a.position.x,p.y-a.position.y,p.z-a.position.z};}
}
void interrupt_trail(AttackActor& a,SakuyaAttackState& t){a.time.reset();t.phase=1;for(auto& v:a.animations)v.pendingInterrupt=1;}
bool trail_outside(const Vec3& p){return p.x<-32||672<p.x||p.y<-32||512<p.y;}
bool update_sakuya(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<SakuyaAttackState&>(*a.state);
    if(t.phase==0){
        advance_trail(a,t);const auto& g=s.geometry[a.destination_side];
        if(s.probe_cancellation(a.destination_side,g.to_local(a.position),{12,12,0})==2||s.probe_cancellation(a.destination_side,g.to_local(t.history[12]),{12,12,0})==2||s.probe_cancellation(a.destination_side,g.to_local(t.history[24]),{12,12,0})==2)interrupt_trail(a,t);
        else if(a.time.current>=30){a.position={t.velocity.x+a.position.x,t.velocity.y+a.position.y,t.velocity.z+a.position.z};if(trail_outside(t.history[31]))return true;}
    }else if(t.phase==1&&a.time.current>=15)return true;return false;
}
bool update_sakuya_collision(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<SakuyaAttackState&>(*a.state);
    if(t.phase==0){
        advance_trail(a,t);const auto parent=t.parent?static_cast<SakuyaAttackState*>(t.parent->state.get()):nullptr;
        if(!parent||parent->phase!=0)interrupt_trail(a,t);
        else if(a.time.current>=30){
            a.position={t.velocity.x+a.position.x,t.velocity.y+a.position.y,t.velocity.z+a.position.z};s.collide_player(a.destination_side,a.position,12);
            for(u32 i=1;i<5;++i)s.collide_player(a.destination_side,t.history[i*6],12);
            if(trail_outside(parent->history[24]))return true;
        }
    }else if(t.phase==1&&a.time.current>=15)return true;return false;
}
void travel_position(AttackActor& a,const TravelAttackState& s,float duration=90){
    const float t=a.time.value()/duration,m=t-1.f,r=1.f-t;
    const float ca=((t+t+1.f)*m)*m,cb=((3.f-(t+t))*t)*t,cc=(r*r)*t,cd=(m*t)*t;
    // The two axes have different evaluation order in the original helper.
    a.position.x=((cb*s.destination.x+ca*s.origin.x)+cd*s.end_tangent.x)+cc*s.start_tangent.x;
    a.position.y=((cd*s.end_tangent.y+cb*s.destination.y)+cc*s.start_tangent.y)+ca*s.origin.y;
}
bool update_reimu(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<TravelAttackState&>(*a.state);
    switch(t.phase){
    case 0:
        travel_position(a,t);
        if(a.time.current>90){
            if(!s.start_animation(a,0,a.source_side,AttackAnimationResource::character_shots,9))return true;
            ++t.phase;a.position=t.target;a.layer=a.destination_side;a.time.reset();
        }break;
    case 1:
        if(a.time.current>20){
            if(s.probe_cancellation(a.destination_side,a.position,{24,24,0})==2){t.phase=2;a.time.reset();a.animations[0].pendingInterrupt=1;return false;}
            a.position={t.velocity.x+a.position.x,t.velocity.y+a.position.y,t.velocity.z+a.position.z};
            if((t.velocity.x<0&&a.position.x<-144)||(t.velocity.x>0&&144<a.position.x))t.velocity.x=-t.velocity.x;
            t.velocity.y+=.04f;if(t.velocity.y>3)t.velocity.y=3;
            s.collide_player(a.destination_side,a.position,18);if(a.position.y>=480)return true;
        }break;
    case 2:if(a.time.current>20)return true;break;
    default:break;
    }
    return false;
}
bool update_marisa(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);const auto& t=static_cast<TravelAttackState&>(*a.state);
    if(t.phase==0){
        travel_position(a,t);
        if(a.time.current>90){
            EnemySpawn request;request.script=8;request.position=t.target;request.life=20;request.item=-2;request.score=1000;request.character_program=1;
            EclLocals locals;locals.floats[0]=-1.5707963705062866f;s.spawn_enemy(a.destination_side,request,locals);return true;
        }
    }return false;
}
bool arrive(AttackActor& a,CharacterAttackServices& s,TravelAttackState& t,i32 script,i32 duration=90){
    travel_position(a,t,float(duration));if(a.time.current<=duration)return false;
    if(script>=0)s.start_animation(a,0,a.source_side,AttackAnimationResource::character_shots,script);
    ++t.phase;a.position=t.target;a.layer=a.destination_side;a.time.reset();return true;
}
void interrupt(AttackActor& a,TravelAttackState& t){t.phase=2;a.time.reset();a.animations[0].pendingInterrupt=1;}
bool update_youmu(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<TravelAttackState&>(*a.state);
    if(t.phase==0)arrive(a,s,t,9,180);
    else if(t.phase==1){
        if(a.time.current!=a.time.previous&&a.time.current%2==0)s.effect(a.destination_side,9,a.position);
        if(a.time.current>20){
            if(s.probe_cancellation(a.destination_side,a.position,{24,24,0})==2)interrupt(a,t);
            else s.collide_player(a.destination_side,a.position,14);
            if(a.time.current>300)interrupt(a,t);
        }
    }else if(t.phase==2&&a.time.current>20)return true;return false;
}
bool update_lyrica(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<TravelAttackState&>(*a.state);
    if(t.phase==0)arrive(a,s,t,8);
    else if(t.phase==1&&a.time.current>20){
        BulletEmission b;b.position=a.position;b.sprite=19;b.count=8;b.layers=1;b.pattern=5;b.angle=float(s.random.signed_unit())*pi;b.speed=.2f;b.flags=0x10;
        b.extras[0]={.01666666753590107f,-999,120,0,0x10,0};s.emit_bullets(a.destination_side,false,b);
        b.pattern=3;b.color=4;b.extras[0]={.01600000075995922f,-999,100,0,0x10,0};s.emit_bullets(a.destination_side,false,b);return true;
    }else if(t.phase==2&&a.time.current>20)return true;return false;
}
bool update_merlin(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<TravelAttackState&>(*a.state);
    if(t.phase==0)arrive(a,s,t,8);
    else if(t.phase==1&&a.time.current>20){
        if(a.time.current!=a.time.previous&&a.time.current%3==0){
            BulletEmission b;b.position=a.position;b.position.y+=24;b.sprite=19;b.color=1;b.count=b.layers=1;b.pattern=3;b.angle=1.5707963705062866f;b.speed=1.2f;b.flags=0x20;
            b.extras[0]={.010833333246409893f,float(s.random.signed_unit())*.015707964077591896f,120,0,0x20,0};s.emit_bullets(a.destination_side,false,b);
        }if(a.time.current>60)return true;
    }else if(t.phase==2&&a.time.current>20)return true;return false;
}
bool update_lunasa(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<TravelAttackState&>(*a.state);
    if(t.phase==0)arrive(a,s,t,8);
    else if(t.phase==1&&a.time.current>20){
        if(a.time.current!=a.time.previous&&a.time.current%3==0){
            BulletEmission b;b.position=a.position;b.sprite=19;b.color=2;b.count=b.layers=1;b.pattern=2;b.speed=.2f;b.flags=0x10;
            b.extras[0]={.006666666828095913f,-999,120,0,0x10,0};s.emit_bullets(a.destination_side,false,b);
        }if(a.time.current>50)return true;
    }else if(t.phase==2&&a.time.current>20)return true;return false;
}
bool update_komachi(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<TravelAttackState&>(*a.state);
    if(t.phase==0){if(arrive(a,s,t,-1))a.animations[0].flag1=0;}
    else if(t.phase==1){
        if(a.time.current>=30)return true;
        if(a.time.current!=a.time.previous&&a.time.current%2==0){
            BulletEmission b;b.position=a.position;b.sprite=21;b.color=i16(t.emissions%3);b.pattern=5;b.count=8;b.layers=1;b.speed=t.speed;b.angle=t.heading;s.emit_bullets(a.destination_side,true,b);
            ++t.emissions;t.heading=t.turn+t.heading;t.speed+=.2f;
        }
    }else if(t.phase==2&&a.time.current>20)return true;return false;
}
bool update_cirno(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<CirnoAttackState&>(*a.state);
    if(t.phase==0)arrive(a,s,t,9,40);
    else if(t.phase==1&&a.time.current>12){
        if(s.probe_cancellation(a.destination_side,a.position,{6,32,0})==2){interrupt(a,t);return false;}
        a.position={t.velocity.x+a.position.x,t.velocity.y+a.position.y,t.velocity.z+a.position.z};
        t.velocity={t.acceleration.x+t.velocity.x,t.acceleration.y+t.velocity.y,t.acceleration.z+t.velocity.z};if(t.velocity.y>4.5f)t.velocity.y=4.5f;
        s.collide_player_box(a.destination_side,a.position,{6,32,0});if(a.position.y>=480)return true;
    }else if(t.phase==2&&a.time.current>12)return true;return false;
}
bool update_tewi(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<TewiAttackState&>(*a.state);
    if(t.phase==0){
        if(arrive(a,s,t,-1)){
            const bool positive=a.position.x>=0;s.start_animation(a,0,a.source_side,AttackAnimationResource::character_shots,positive?12:11);
            t.horizontal_acceleration=positive?.03f:-.03f;t.velocity.x=0;t.velocity.y=2;
        }
    }else if(t.phase==1&&a.time.current>20){
        if(s.probe_cancellation(a.destination_side,a.position,{24,24,0})==2){interrupt(a,t);return false;}
        a.position={t.velocity.x+a.position.x,t.velocity.y+a.position.y,t.velocity.z+a.position.z};t.velocity.x=t.horizontal_acceleration+t.velocity.x;
        if(a.position.x<-144||144<a.position.x)t.velocity.x*= -1.f;
        if(a.position.y>=464)return true;s.collide_player(a.destination_side,a.position,13);
    }else if(t.phase==2&&a.time.current>20)return true;return false;
}
bool update_aya(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<AyaAttackState&>(*a.state);
    if(t.phase==0){
        travel_position(a,t);
        if(a.time.current>90){t.variant=s.random.below(4);s.start_animation(a,0,a.source_side,AttackAnimationResource::character_shots,i32(t.variant)+9);++t.phase;a.position=t.target;a.layer=a.destination_side;a.time.reset();}
    }else if(t.phase==1&&a.time.current>20){
        if(s.probe_cancellation(a.destination_side,a.position,{24,24,0})==2){interrupt(a,t);return false;}
        a.position={t.velocity.x+a.position.x,t.velocity.y+a.position.y,t.velocity.z+a.position.z};s.collide_player(a.destination_side,a.position,t.variant<2?28.f:18.f);
        if(a.position.y>=512)return true;
    }else if(t.phase==2&&a.time.current>20)return true;return false;
}
bool update_medicine(AttackActor& a,AttackServices& services){
    auto& s=static_cast<CharacterAttackServices&>(services);auto& t=static_cast<MedicineAttackState&>(*a.state);
    if(t.phase==0){
        travel_position(a,t);if(a.time.current<=90)return false;
        for(u32 i=0;i<4;++i)s.start_animation(a,i,a.source_side,AttackAnimationResource::character_shots,i?10:9);
        ++t.phase;a.position=t.target;a.layer=a.destination_side;a.time.reset();
        const float dx=s.players[a.destination_side].x-a.position.x,dy=s.players[a.destination_side].y-a.position.y;
        t.movement_heading=dx==0&&dy==0?1.5707963705062866f:float(std::atan2(double(dy),double(dx)));t.movement_speed=.2f;t.has_bounced=0;
        t.history.fill(a.position);t.directions.fill(a.angle);return false;
    }
    if(t.phase==2)return a.time.current>20;if(t.phase!=1)return false;
    for(u32 i=15;i>0;--i){t.history[i]=t.history[i-1];t.directions[i]=t.directions[i-1];}
    t.history[0]=a.position;t.directions[0]=t.movement_heading;
    a.animations[0].rotation.z=t.movement_heading;a.animations[0].updateRotation=1;
    for(u32 i=1;i<4;++i){const u32 h=2*i-1;const auto& pos=t.history[h];a.animations[i].pos2={pos.x-a.position.x,pos.y-a.position.y,pos.z-a.position.z};a.animations[i].rotation.z=t.directions[h];a.animations[i].updateRotation=1;}
    if(a.time.current<=20)return false;
    t.velocity=polar(t.movement_heading,t.movement_speed);t.movement_speed+=.1f;
    if(s.probe_cancellation(a.destination_side,a.position,{24,24,0})==2){t.phase=2;a.time.reset();for(auto& v:a.animations)v.pendingInterrupt=1;return false;}
    a.position={t.velocity.x+a.position.x,t.velocity.y+a.position.y,t.velocity.z+a.position.z};
    if(!t.has_bounced&&(a.position.x<-144||144<a.position.x)){t.velocity.x=-t.velocity.x;t.movement_heading=float(add_angle(pi-t.movement_heading,0));t.has_bounced=1;}
    const auto forward=polar(t.movement_heading,t.movement_speed);s.collide_player(a.destination_side,{a.position.x+forward.x,a.position.y+forward.y,a.position.z+forward.z},12);
    return a.position.y>=480;
}
}
void interpolate_attack_travel(AttackActor& a,const TravelAttackState& t,float duration){travel_position(a,t,duration);}
void draw_character_attacks(AttackQueue& queue,CharacterAttackServices& s,bool cross_field){
    for(i32 layer=cross_field?2:0;layer<(cross_field?3:2);++layer){
        s.begin_attack_layer(layer);
        for(auto a:queue.draw_lists[layer]){
            for(auto& v:a->animations){
                if(v.type){v.rotation.z=a->angle;v.updateRotation=1;}
                v.pos={v.pos2.x+a->position.x,v.pos2.y+a->position.y,0};if(layer!=2)v.pos=s.geometry[layer].to_screen(v.pos);v.pos.z=.06f;s.draw_animation(v);
            }
            if(a->behavior->draw)a->behavior->draw(*a,s);
        }
    }
}
std::array<AttackBehavior,27> character_attack_behaviors(){
    std::array<AttackBehavior,27> result{};
    result[0]={initialize_reimu,update_reimu,nullptr,nullptr};
    result[1]={initialize_reimu_script,update_reimu,nullptr,nullptr};
    result[2]={initialize_marisa,update_marisa,nullptr,nullptr};
    result[3]={initialize_sakuya,update_sakuya,nullptr,nullptr};
    result[4]={initialize_sakuya_collision,update_sakuya_collision,nullptr,nullptr};
    result[5]={initialize_cirno,update_cirno,nullptr,nullptr};
    result[10]={initialize_cirno_script,update_cirno,nullptr,nullptr};
    result[13]={initialize_youmu,update_youmu,nullptr,nullptr};
    result[15]={initialize_lyrica,update_lyrica,nullptr,nullptr};
    result[16]={initialize_tewi,update_tewi,nullptr,nullptr};
    result[17]={initialize_aya,update_aya,nullptr,nullptr};
    result[18]={initialize_medicine,update_medicine,nullptr,nullptr};
    result[20]={initialize_komachi,update_komachi,nullptr,nullptr};
    result[23]={initialize_tewi_script,update_tewi,nullptr,nullptr};
    result[24]={initialize_medicine_script,update_medicine,nullptr,nullptr};
    result[25]={initialize_merlin,update_merlin,nullptr,nullptr};
    result[26]={initialize_lunasa,update_lunasa,nullptr,nullptr};add_mystia_attack_behaviors(result);add_field_attack_behaviors(result);return result;
}
}
