#include "EnemyCallbacks.hpp"
#include "BulletManager.hpp"
#include "LaserManager.hpp"
#include "GraphicsMath.hpp"
#include <cmath>
namespace th11 {
namespace {
constexpr double pi=3.1415927410125732421875,tau=6.283185482025146484375,half_pi=1.57079637050628662109375;
Vec3 add(Vec3 a,Vec3 b){return{float(double(a.x)+b.x),float(double(a.y)+b.y),float(double(a.z)+b.z)};}
bool active(const BulletState& b){return (b.flags&1)&&b.state==1;}
float distance2(Vec3 a,Vec3 b){const float x=float(double(a.x)-b.x),y=float(double(a.y)-b.y);return float(double(x)*x+double(y)*y);}
float angle_difference(float a,float b){return float(double(a)-b>pi?double(a)-(double(b)+tau):double(b)-a>pi?double(a)-(double(b)-tau):double(a)-b);}
float arc_start(const AnmVm& v){return normalize_angle(float(double(v.rotation.x)*.015625+(double(v.rotation.z)-double(v.rotation.x)*.5)));}
float arc_next(const AnmVm& v,float angle){return normalize_angle(float(double(float(double(v.rotation.x)*.03125))+angle));}
Vec3 arc_point(const EnemyState& e,const AnmVm& v,float angle){const auto p=polar(angle,v.scale.y);return add(e.current.position,{p.x,p.y,0});}
}
bool EnemyCallbacks::invoke_tick(EnemyState& e,EnemyTick kind,i32& result){
    result=0;
    switch(kind){
    case EnemyTick::None:return true;
    case EnemyTick::RotateWarningLasers:return commands.lasers&&commands.lasers->update_warning(e.floats[0]);
    case EnemyTick::KillMarkedEnemies:case EnemyTick::SignalMarkedEnemies:case EnemyTick::ReleaseMarkedEnemies:return marked_enemies(kind);
    case EnemyTick::AttractPlayerAndBullets:return attract(e);
    case EnemyTick::SpawnEnemiesFromBullets:return spawn_from_bullets(e);
    case EnemyTick::BoundaryEmitter:return boundary_emitter(e);
    case EnemyTick::BulletEmitter:return bullet_emitter(e);
    case EnemyTick::HideIndicator:return world.callback_indicator(3,false);
    case EnemyTick::ResetIndicator:return world.callback_indicator(2,true);
    case EnemyTick::TransformCircle:return transform(e,false);
    case EnemyTick::TransformAngles:return transform(e,true);
    }
    return false;
}
bool EnemyCallbacks::marked_enemies(EnemyTick kind){
    for(auto* node=world.callback_enemies();node;){auto& e=node->value->state;node=node->next;if(!(e.flags&0x400))continue;
        if(kind==EnemyTick::SignalMarkedEnemies)e.integers[1]=1;
        else if(kind==EnemyTick::ReleaseMarkedEnemies)e.flags=(e.flags&~0x400u)|0x100;
        else if(e.health_flags&1){e.scaled_health=wrapping_sub(e.scaled_health,99999);e.health=wrapping_add(wrapping_sub(e.scaled_health,signed_bits(u32(e.health_baseline)*7))/7,e.health_baseline);}
        else e.health=wrapping_sub(e.health,99999);
    }return true;
}
bool EnemyCallbacks::attract(EnemyState& e){
    if(!commands.bullets)return false;auto& manager=*commands.bullets;
    if(e.integers[0]==1){
#ifdef TH11_MULTIPLAYER
        if(world.multiplayer_world()){if(!world.multiplayer_attract_players(e))return false;}
        else{
#endif
        const auto p=environment.player_position;auto d=GraphicsMath::normalize({float(double(e.current.position.x)-p.x),float(double(e.current.position.y)-p.y),0});
        d={float(double(d.x)*e.floats[3]),float(double(d.y)*e.floats[3]),float(double(d.z)*e.floats[3])};
        if(!world.callback_move_player(add(p,d)))return false;
#ifdef TH11_MULTIPLAYER
        }
#endif
    }
    for(u32 i=0;i<BulletManager::capacity;++i){auto& b=manager.at(i);if(!active(b))continue;if(u32(b.sprite)>=bullet_appearances.size())return false;
        if(bullet_appearances[b.sprite].script!=0x22)continue;
        b.speed=-e.floats[3];const auto v=polar(b.angle,b.speed);b.velocity={v.x,v.y,0};
        if(distance2(e.current.position,b.position)<double(e.floats[0])*e.floats[0]*.25&&!manager.cancel(b))return false;
    }return true;
}
bool EnemyCallbacks::spawn_from_bullets(EnemyState& e){
    if(!commands.bullets)return false;
    for(u32 i=0;i<BulletManager::capacity;++i){auto& b=commands.bullets->at(i);if(!active(b))continue;if(u32(b.sprite)>=bullet_appearances.size())return false;
        if(bullet_appearances[b.sprite].script==0x40&&!world.callback_spawn(e,b.position))return false;
    }return true;
}
bool EnemyCallbacks::fire(EnemyState& e,u32 slot){
    auto& b=*commands.bullets;b.exclusion_squared=e.exclusion_distance;const bool ok=b.fire(e.emitters[slot]);b.exclusion_squared=0;return ok;
}
bool EnemyCallbacks::boundary_emitter(EnemyState& e){
    if(!commands.bullets||!e.integers[1])return false;
    const float angle=e.floats[0];auto p=polar(angle,e.floats[1]);auto& emitter=e.emitters[0];emitter.position=add(e.current.position,{p.x,p.y,0});
    float length=0;bool clamped=true;
    if(emitter.position.x>=184)length=float(184./float(std::cos(double(angle))));
    else if(emitter.position.x<=-184)length=float(184./float(std::cos(double(float(angle>=0?pi-angle:pi+angle)))));
    else if(emitter.position.y<=32)length=float(192./float(std::cos(double(float(double(angle)+half_pi)))));
    else if(emitter.position.y>=432)length=float(208./float(std::cos(double(float(double(angle)-half_pi)))));
    else clamped=false;
    if(clamped){p=polar(angle,length);emitter.position=add(e.current.position,{p.x,p.y,0});}
    const i32 elapsed=e.lifetime.current%e.integers[1];
    const double dx=double(emitter.position.x)-environment.player_position.x,dy=double(emitter.position.y)-environment.player_position.y;
    if((!elapsed||(float(dx*dx+dy*dy)<576&&(elapsed==3||elapsed==6)))&&!fire(e,0))return false;
    for(u32 i=0;i<BulletManager::capacity;++i){auto& b=commands.bullets->at(i);if(!active(b)||b.color!=e.integers[0])continue;
        const auto local=rotate({float(double(e.current.position.x)-b.position.x),float(double(e.current.position.y)-b.position.y)},float(pi-angle));
        if(local.x>=0&&local.x<=295&&local.y>=-8&&local.y<=8&&!commands.bullets->cancel(b))return false;
    }return true;
}
bool EnemyCallbacks::bullet_emitter(EnemyState& e){
    if(!commands.bullets||!e.integers[1])return false;if(e.lifetime.current%e.integers[1])return true;
    for(u32 i=0;i<BulletManager::capacity;++i){auto& b=commands.bullets->at(i);if(!active(b))continue;if(u32(b.sprite)>=bullet_appearances.size())return false;
        if(bullet_appearances[b.sprite].script==0xbf){e.emitters[1].position=add(b.position,e.emitter_offset[1]);if(!fire(e,1))return false;}
    }return true;
}
bool EnemyCallbacks::transform(EnemyState& e,bool angular){
    if(!commands.bullets||!world.callback_indicator(0,true))return false;auto& manager=*commands.bullets;
    for(u32 i=0;i<BulletManager::capacity;++i){auto& b=manager.at(i);if(!active(b))continue;
        bool inside,alternate=false;
        if(angular){alternate=b.color==6||b.color==1;b.sprite=i16(alternate);inside=std::abs(angle_difference(b.angle,e.floats[alternate?0:1]))<=e.floats[2];}
        else {const double radius=double(e.deformation_radius)-32,x=double(b.position.x)-e.current.position.x,y=double(b.position.y)-e.current.position.y;inside=float(x*x+y*y)<radius*radius;}
        if(inside==(b.reserved_4c0!=0))continue;
        if(inside&&!manager.sound(24,b.position.x,true))return false;b.reserved_4c0=inside?1:0;
        const i32 color=angular?(inside?(alternate?1:0):(alternate?6:2)):(inside?(b.color==6?1:b.color==13?3:0):(b.color==1?6:b.color==3?13:2));
        // The original resets animation state before binding the new appearance.
        b.vm.initialize();if(!manager.change_appearance(b,inside?28:1,color,true))return false;
        if(inside){const float size=float(double(bullet_appearances[28].hitbox)*(angular?1.600000023841858:1.2000000476837158));b.hitbox={size,size};}
    }return true;
}
bool EnemyCallbacks::damage_arc(EnemyState& e,i32& result){
    auto* vm=world.callback_animation(e.animations[0]);if(!vm||!e.max_health)return false;
    vm->flags|=4;vm->rotation.x=float(double(e.health)*5.890486240386963/e.max_health+0.39269909262657166);
    float angle=arc_start(*vm);result=0;
    for(u32 i=0;i<31;++i){i32 damage=0;if(!world.callback_damage(arc_point(e,*vm,angle),{16,16},damage))return false;result=wrapping_add(result,damage);angle=arc_next(*vm,angle);}
    if(result>=30){result=wrapping_add(wrapping_sub(signed_bits(u32(result)*3),90)/5,30);if(result>=70){result=wrapping_add(wrapping_sub(result,50)/5,50);if(result>=80)result=80;}}
    return true;
}
bool EnemyCallbacks::collision_arc(EnemyState& e){
    auto* vm=world.callback_animation(e.animations[0]);if(!vm||!e.max_health)return false;
    float angle=arc_start(*vm);const float width=float(double(e.health)*16/e.max_health+4);
    for(u32 i=0;i<32;++i){if(!world.callback_collision(arc_point(e,*vm,angle),angle,width,12))return false;angle=arc_next(*vm,angle);}return true;
}
}
