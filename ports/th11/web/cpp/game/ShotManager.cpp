#include "ShotManager.hpp"
#include <algorithm>
#include <cmath>
namespace th11 {
namespace {
constexpr double pi=3.1415927410125732421875,tau=6.283185482025146484375;
Vec3 screen(Vec3 p){return {float((double(p.x)+32)+192),float(double(p.y)+16),p.z};}
template<class F>void family(AnmVm* vm,F f){if(!vm)return;f(*vm);if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)f(*n->value);}
float angle_delta(float target,float current){const double d=double(target)-current;if(d>=pi)return float(double(target)-(double(current)+tau));if(-d>pi)return float(double(target)-(double(current)-tau));return float(d);}
bool outside(Vec3 p,float x,float y){return double(p.x)+x<=-192||double(p.x)-x>=192||double(p.y)+y<=0||double(p.y)-y>=448;}
}
ShotManager::ShotManager(ShtResource& s,AnmResource& a,AnmManager& m,ShotWorld& w,u16 file):resource(s),animation_resource(a),animations(m),world(w),file_id(file){schedule.timer.set(-1,&m.rate);}
void ShotManager::interrupt(u32 id,i16 label){family(animations.find(id),[&](AnmVm& a){a.pending_interrupt=label;});}
void ShotManager::erase(u32 id){family(animations.find(id),[](AnmVm& a){a.flags|=0x4000000;});}
Enemy* ShotManager::nearest(Vec3 p,float radius)const noexcept{
    Enemy* result=nullptr;float distance=float(double(radius)*radius);
    for(auto* node=world.enemies;node;node=node->next){auto* e=node->value;if(e->state.flags&0xc00021)continue;
        const auto q=e->state.current.position;const double dx=double(p.x)-q.x,dy=double(p.y)-q.y;const float d=float(dx*dx+dy*dy);
        if(d<distance){result=e;distance=d;}
    }return result;
}
bool ShotManager::on_spawn(ShotState& s,i32){
    switch(s.spec->spawn){
    case ShotSpawn::None:return true;
    case ShotSpawn::Homing:s.target=nullptr;return true;
    case ShotSpawn::ConvertedLaser:
        s.target=locked_target;
        if(s.target&&std::abs(s.target->state.current.position.x)>224)s.target=nullptr;
        return true;
    case ShotSpawn::OptionRotation:{const i32 option=s.spec->option;if(option<1||option>8)return false;auto* a=animations.find(player.options[option-1].animation);if(!a)return false;s.movement.angle=normalize_angle(a->floats[0]);return true;}
    case ShotSpawn::RandomAcceleration:s.acceleration=float(double(animations.script_rng.signed_unit())*0.0020000000949949026);return true;
    }return false;
}
i32 ShotManager::spawn(const ShotSpec& spec,i32 firing_frame,const Vec3& origin){
    if(spec.option<0||spec.option>8){last_error=1;return -2;}
    if(spec.type==4&&laser_active[spec.option])return -1;
    u32 index=0;while(index<shots.size()&&shots[index].state)++index;if(index==shots.size())return -1;
    auto& s=shots[index];s.state=1;s.spec=&spec;s.timer.set(0,&animations.rate);s.damage=spec.damage;
    s.movement.position=origin;
    if(spec.option){const auto& o=player.options[spec.option-1];s.movement.position={float(double(o.x)/128),float(double(o.y)/128),0};}
    if(spec.type==4)laser_active[spec.option]=1;
    s.movement.speed=spec.speed;
    if(spec.angle>=1000&&spec.option){const float random=animations.script_rng.signed_unit();s.movement.angle=normalize_angle(float((double(random)*pi/12)+player.options[spec.option-1].angle));s.movement.speed=float(double(animations.script_rng.signed_unit())*2+spec.speed);}
    else s.movement.angle=normalize_angle(spec.angle>=995&&spec.option?player.options[spec.option-1].angle:spec.angle);
    s.movement.update_velocity();
    s.movement.position.x=float(double(spec.offset.x)-s.movement.velocity.x+s.movement.position.x);
    s.movement.position.y=float(double(spec.offset.y)-s.movement.velocity.y+s.movement.position.y);
    auto* vm=animations.create(animation_resource,i32(spec.animation)+5,file_id,22);if(!vm){last_error=2;return -2;}s.animation=vm->id;
    if(vm->flags&0x8000000){vm->rotation.z=spec.angle;vm->flags|=4;}
    s.extra_animation=0;if(!on_spawn(s,firing_frame)){last_error=3;return -2;}
    if(spec.sound>=0&&!world.sound(spec.sound,s.movement.position.x)){last_error=4;return -2;}
    vm->position=screen(s.movement.position);return i32(index);
}
i32 ShotManager::spawn_converted_laser(Vec3 origin,float angle){
    converted_laser_spec.angle=angle;
    return spawn(converted_laser_spec,0,origin);
}
bool ShotManager::fire_frame(i32 frame){
    const i32 group=resource.group_index(player.power,player.power_step,player.character,player.subtype,player.focused,player.weapon_mode);
    if(group<0){last_error=5;return false;}
    for(const auto& spec:resource.groups[group].shots)if(spec.due(frame)&&spawn(spec,frame,player.position)==-2)return false;
    return true;
}
bool ShotManager::fire(){
    if(player.state!=1){locked_target=nullptr;target_locked=0;return true;}
    const i32 frame=schedule.begin(player.state,player.warp_state,player.held,&animations.rate);
    if(frame>=0&&!fire_frame(frame))return false;
    if(schedule.timer.current>=0)schedule.finish(player.held,&animations.rate);
    return true;
}
bool ShotManager::on_update(ShotState& s){
    auto& v=s.movement;
    switch(s.spec->update){
    case ShotUpdate::None:return true;
    case ShotUpdate::Homing:{
        if(s.state==2)return true;
        if(!s.target){Vec3 p=v.position;p.y=float(double(p.y)-128);s.target=nearest(p,80);}
        if(s.target&&(s.target->state.flags&0xc00021))s.target=nullptr;
        if(!s.target){v.speed=std::min(float(double(v.speed)+0.10000000149011612),16.f);return true;}
        const auto p=s.target->state.current.position;float delta=angle_delta(float(std::atan2(double(float(double(p.y)-v.position.y)),double(float(double(p.x)-v.position.x)))),v.angle);
        if(s.timer.current>=40){v.speed=float(double(v.speed)+0.20000000298023224);return true;}
        if(std::abs(delta)>=2.094395160675049f)delta=0;
        if(std::abs(delta)>=0.7853981852531433)v.speed=std::max(float(double(v.speed)-0.30000001192092896),4.f);
        else if(std::abs(delta)<=0.2617993950843811f)v.speed=std::min(float(double(v.speed)+0.10000000149011612),16.f);
        v.angle=normalize_angle(float(double(delta)*0.03999999910593033+v.angle));return true;
    }
    case ShotUpdate::ConvertedLaser:{
        if(s.state==2)return true;
        if(s.target&&(s.target->state.flags&0xc00021))s.target=nullptr;
        if(!s.target)v.speed=std::min(float(double(v.speed)+0.10000000149011612),16.f);
        else {
            const auto p=s.target->state.current.position;
            const float delta=angle_delta(float(std::atan2(double(float(double(p.y)-v.position.y)),double(float(double(p.x)-v.position.x)))),v.angle);
            if(s.timer.current>=240)v.speed=float(double(v.speed)+0.20000000298023224);
            else {
                if(std::abs(delta)>=0.7853981852531433)v.speed=std::max(float(double(v.speed)-0.30000001192092896),4.f);
                else if(std::abs(delta)<0.2617993950843811f)v.speed=std::min(float(double(v.speed)+0.10000000149011612),16.f);
                v.angle=normalize_angle(float(double(delta)*0.10000000149011612+v.angle));
            }
        }
        if(s.timer.current>=120){erase(s.animation);s.animation=0;}
        return true;
    }
    case ShotUpdate::Accelerating:
        if(s.state==2)return true;
        v.velocity.y=float(double(v.velocity.y)-(player.special_active?0.6000000238418579:0.3799999952316284));
        v.velocity.x=float(double(v.velocity.x)+s.acceleration);
        v.angle=normalize_angle(float(std::atan2(double(v.velocity.y),double(v.velocity.x))));
        v.speed=float(std::sqrt(double(float(double(v.velocity.x)*v.velocity.x+double(v.velocity.y)*v.velocity.y))));return true;
    case ShotUpdate::Attached:{
        const i32 n=s.spec->option;if(n<1||n>8)return false;const auto& o=player.options[n-1];
        v.position={float(double(o.x)/128),float(double(o.y)/128),0};
        v.position.x=float(double(s.spec->offset.x)-v.velocity.x+v.position.x);v.position.y=float(double(s.spec->offset.y)-v.velocity.y+v.position.y);return true;
    }
    }return false;
}
bool ShotManager::update(){
    for(auto& s:shots){if(!s.state)continue;if(!s.spec){last_error=6;return false;}
        if(s.spec->type==4&&s.state==1&&(schedule.timer.current<0||s.spec->option-1>=player.option_count||player.bomb_active||!player.enemies_present)){
            interrupt(s.animation,1);interrupt(s.extra_animation,1);s.state=2;laser_active[s.spec->option]=0;
        }
        if(s.spec->type==4&&!s.hit_this_frame&&s.state==1&&s.hit_last_frame==1){interrupt(s.animation,3);s.hit_last_frame=0;}
        s.hit_this_frame=0;if(!on_update(s)){last_error=7;return false;}
        s.movement.update_velocity();s.movement.update();
        auto* a=animations.find(s.animation);
        if(!a){s.state=0;erase(s.extra_animation);s.animation=s.extra_animation=0;continue;}
        if(s.spec->type!=4&&s.timer.current>=10){
            if(!a->sprite){last_error=8;return false;}
            const float width=float(double(a->sprite->width)*a->scale.x),height=float(double(a->sprite->height)*a->scale.y);const auto p=s.movement.position;
            if(outside(p,width,height)&&!(s.spec->type==2&&p.x>-224&&p.x<224&&p.y>=-32)){erase(s.animation);s.animation=0;s.state=0;continue;}
        }
        a->position=screen(s.movement.position);
        if(s.extra_animation){auto* extra=animations.find(s.extra_animation);if(extra)extra->position=a->position;else s.extra_animation=0;}
        if(a->flags&0x8000000){a->rotation.z=s.movement.angle;a->flags|=4;}
        s.timer.tick();
    }return true;
}
void ShotManager::invalidate_target(Enemy* e)noexcept{if(locked_target==e){locked_target=nullptr;target_locked=0;}for(auto& s:shots)if(s.target==e)s.target=nullptr;}
bool ShotManager::damage(const Vec3& p,const Vec2& size,bool frame_advanced,GameEconomy& economy,i32& result){
    result=0;if(!frame_advanced)return true;
    const float left=float(double(p.x)-double(size.x)*.5),right=float(double(p.x)+double(size.x)*.5);
    const float top=float(double(p.y)-double(size.y)*.5),bottom=float(double(p.y)+double(size.y)*.5);
    for(auto& s:shots){if(!s.state||s.state==2)continue;if(!s.spec){last_error=6;return false;}const auto& spec=*s.spec;
        const auto q=s.movement.position;
        const float l=float(double(q.x)-double(spec.hitbox.x)*.5),r=float(double(q.x)+double(spec.hitbox.x)*.5);
        const float t=float(double(q.y)-double(spec.hitbox.y)*.5),b=float(double(q.y)+double(spec.hitbox.y)*.5);
        if(!(t<=bottom&&l<=right&&top<=b&&left<=r))continue;
        if(spec.type==4?bottom<0:t<0)continue;
        if(spec.extra_callback){last_error=9;return false;}
        if(!s.hit_last_frame){interrupt(s.animation,2);s.hit_last_frame=1;}s.hit_this_frame=1;
        if(spec.type!=4||(s.timer.previous!=s.timer.current&&s.timer.current%4==0))result=wrapping_add(result,s.damage);
        if(spec.type!=1&&spec.type!=4){
            auto* old=animations.find(s.animation);if(!old){last_error=10;return false;}const float rotation=old->rotation.z;erase(s.animation);s.animation=0;
            const bool boosted=spec.type==2&&player.special_active;
            auto* a=animations.create(animation_resource,i32(spec.hit_animation)+(boosted?6:5),file_id,22);
            if(!a){last_error=2;return false;}s.animation=a->id;
            if(boosted)result=wrapping_add(result,s.damage/3);
            a->flags|=4;a->rotation.z=rotation;s.movement.position.z=.1f;s.state=2;s.movement.speed=float(double(s.movement.speed)*.125);a->position=screen(s.movement.position);
        }
        if(spec.type==1&&s.damage>1)s.damage=std::max(s.damage/10,1);
        else if(spec.type==2){if(!world.sound(50,s.movement.position.x)){last_error=4;return false;}}
        else if(spec.type==3)damage_areas.circle(s.movement.position,32,1.4f,13,s.damage/3,&animations.rate);
    }
    const i32 combination=player.character*3+player.subtype;
    if(player.special_active&&combination>=2&&combination<=4){i32 special=0;if(!world.special_damage(p,size,special)){last_error=11;return false;}result=wrapping_add(result,special);}
    if(!damage_areas.collect(p,size,result)){last_error=12;return false;}
    if(result>=30){result=wrapping_add(signed_bits(u32(result)*3-90)/5,30);if(result>=70)result=std::min(wrapping_add(wrapping_add(result,-50)/5,50),80);}
    if(result)economy.add_score(wrapping_add(result/10,10));
    return true;
}
}
