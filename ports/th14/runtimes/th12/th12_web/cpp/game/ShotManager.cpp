#include "ShotManager.hpp"
#include "AnmGeometry.hpp"
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
ShotManager::ShotManager(ShtResource& s,AnmResource& a,AnmManager& m,ShotWorld& w,u16 file):resource(s),animation_resource(a),animations(m),world(w),file_id(file){schedule.timer.set(-1,&m.rate);
#if PORT_TITLE == 14
    player.th14_native_stack=&m.th14_native_stack;
#endif
#if PORT_TITLE == 16
    th16_schedule.regular.set(-1,&m.rate);th16_schedule.long_timer.set(-1,&m.rate);
#endif
}
#if PORT_TITLE == 14
void ShotManager::start_stage_th14()noexcept{
    th14_shot_pool=Th14ShotPool{};
    th14_area_cursor=0;
}
#endif
void ShotManager::interrupt(u32 id,i16 label){family(animations.find(id),[&](AnmVm& a){a.interrupt(label);});}
void ShotManager::erase(u32 id){family(animations.find(id),[](AnmVm& a){if constexpr(kPortTitle==14)a.flags2|=0x20;else a.flags|=0x4000000;});}
Enemy* ShotManager::nearest(Vec3 p,float radius)const noexcept{
    Enemy* result=nullptr;float distance=float(double(radius)*radius);
    for(auto* node=world.enemies;node;node=node->next){auto* e=node->value;if(e->state.flags&0xc00021)continue;
        const auto q=e->state.current.position;const double dx=double(p.x)-q.x,dy=double(p.y)-q.y;const float d=float(dx*dx+dy*dy);
        if(d<distance){result=e;distance=d;}
    }return result;
}
bool ShotManager::on_spawn(ShotState& s,i32){
#if PORT_TITLE == 14
    switch(u32(s.spec->spawn)){
    case 0:return true;
    case 1:s.target=nullptr;s.th14_target_id=0;return true;
    case 2:{auto* vm=animations.find(s.animation);if(!vm||!shot_th14_focused_spawn(s,player,*vm,world.enemies,&animations.rate))return false;
        // 451b31..451b3a writes both radius and growth through the retained
        // area pointer. Reused death/Bomb records can still have growth16.
        if(s.th14_area_slot<256){auto& a=th14_areas[s.th14_area_slot];a.flags|=2;a.radius=s.th14_area_radius;a.growth=s.th14_area_growth;}return true;}
    case 3:if(s.th14_area_slot>=256)return false;shot_th14_beam_spawn(s,th14_areas[s.th14_area_slot]);return world.sound(20,player.position.x);
    case 4:shot_th14_spread_spawn(s,animations.visual_rng);return true;
    case 5:shot_th14_flame_spawn(s,animations.visual_rng);return true;
    case 6:return shot_th14_sakuya_spawn(s,player,&animations.rate)==0;
    default:return false;
    }
#else
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
#endif
}
i32 ShotManager::spawn(const ShotSpec& spec,i32 firing_frame,const Vec3& origin){
#if PORT_TITLE == 16
    const ShtResource* families[]={&resource,th16_season_resource};
    for(u32 f=0;f<2;++f)if(families[f])for(u32 g=0;g<families[f]->groups.size();++g){const auto& list=families[f]->groups[g].shots;for(u32 n=0;n<list.size();++n)if(&list[n]==&spec)return spawn_th16(spec,(f?0x10000u:0u)|(g<<8)|n,firing_frame,origin);}
    last_error=1693;return -2;
#endif
    if(spec.option<0||spec.option>8){last_error=1;return -2;}
    const bool beam_type=spec.type==(kPortTitle==14?3:4);
    if(beam_type&&laser_active[spec.option])return -1;
#if PORT_TITLE == 14
    if(spec.type==2&&player.th14_cached_power)return -1;
#endif
#if PORT_TITLE == 14
    const u32 index=th14_shot_pool.peek();if(index>=shots.size())return -1;
#else
    u32 index=0;while(index<shots.size()&&shots[index].state)++index;if(index==shots.size())return -1;
#endif
    auto& s=shots[index];s.state=1;s.spec=&spec;s.timer.set(0,&animations.rate);s.damage=spec.damage;
#if PORT_TITLE == 14
    s.th14_steering.set(0,&animations.rate);s.th14_decay.set(0,&animations.rate);s.th14_area_flags=1;
    s.th14_was_focused=player.focused;s.th14_area_slot=256;
    s.th14_homing_target_retired=false;
    s.th14_shot_flags=(s.th14_shot_flags&~2u)|1u|(player.focused?2u:0u);
    s.movement={}; // Retail 4508f0 clears the old movement before filling it.
#endif
    s.movement.position=origin;
    if(spec.option){const auto& o=player.options[spec.option-1];
#if PORT_TITLE == 14
        s.movement.position={float(o.x)*.0078125f,float(o.y)*.0078125f,0};
#else
        s.movement.position={float(double(o.x)/128),float(double(o.y)/128),0};
#endif
    }
    if(beam_type){
#if PORT_TITLE == 14
        if(!player.power_step){last_error=1;return -2;}laser_active[spec.option]=u32(player.power/player.power_step);
#else
        laser_active[spec.option]=1;
#endif
    }
    s.movement.speed=spec.speed;
    if(spec.angle>=1000&&spec.option){
#if PORT_TITLE == 14
        const float random=animations.visual_rng.signed_unit();
        const float spread=float(random*float(pi))/12.f;
        s.movement.angle=normalize_angle(float(spread+player.options[spec.option-1].angle));
        s.movement.speed=float(float(animations.visual_rng.signed_unit()*2.f)+spec.speed);
#else
        const float random=animations.script_rng.signed_unit();s.movement.angle=normalize_angle(float((double(random)*pi/12)+player.options[spec.option-1].angle));s.movement.speed=float(double(animations.script_rng.signed_unit())*2+spec.speed);
#endif
    }
    else s.movement.angle=normalize_angle(spec.angle>=995&&spec.option?player.options[spec.option-1].angle:spec.angle);
#if PORT_TITLE == 14
    const auto velocity=polar(s.movement.angle,float(s.movement.speed*animations.rate));
    s.movement.velocity={velocity.x,velocity.y,0};
    s.movement.position.x=float(float(spec.offset.x-s.movement.velocity.x)+s.movement.position.x);
    s.movement.position.y=float(float(spec.offset.y-s.movement.velocity.y)+s.movement.position.y);
    const Vec3 initial_position{};const float initial_rotation=0;
    // Retail450cd3 binds the copied template; it retains the shot's ANM layer.
    auto* vm=animations.create(animation_resource,i32(spec.animation)+5,file_id,u32(-1),false,false,&initial_position,nullptr,nullptr,false,&initial_rotation);
#else
    s.movement.update_velocity();
    s.movement.position.x=float(double(spec.offset.x)-s.movement.velocity.x+s.movement.position.x);
    s.movement.position.y=float(double(spec.offset.y)-s.movement.velocity.y+s.movement.position.y);
    auto* vm=animations.create(animation_resource,i32(spec.animation)+5,file_id,22);
#endif
    if(!vm){last_error=2;return -2;}s.animation=vm->id;
#if PORT_TITLE == 14
    if(vm->flags2&0x40){vm->rotation.z=spec.angle;vm->flags|=4;}
#else
    if(vm->flags&0x8000000){vm->rotation.z=spec.angle;vm->flags|=4;}
#endif
    s.extra_animation=0;
#if PORT_TITLE == 14
    if(!bind_th14_area(s))return -2;
#endif
    if(!on_spawn(s,firing_frame)){
#if PORT_TITLE == 14
        if(u32(spec.spawn)==6){s.state=0;s.th14_area_flags&=~1u;if(s.th14_area_slot<256)th14_areas[s.th14_area_slot].flags&=~1u;if(auto* a=animations.find(s.animation))a->flags2|=0x20;s.animation=0;return -1;}
#endif
        last_error=3;return -2;
    }
#if PORT_TITLE == 14
    if(u32(spec.spawn))sync_th14_area(s);
#endif
    if(spec.sound>=0&&!world.sound(spec.sound,s.movement.position.x)){last_error=4;return -2;}
    vm->position=kPortTitle==14?s.movement.position:screen(s.movement.position);
#if PORT_TITLE == 14
    th14_shot_pool.consume();
#endif
    return i32(index);
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
#if PORT_TITLE == 16
    struct Frames final:Th16FireFrames {ShotManager& manager;Frames(ShotManager& m):manager(m){}bool fire(i32 a,i32 b)override{return manager.th16_fire_frames(a,b);}} frames(*this);
    const bool ok=th16_schedule.update(player.state,player.held,&animations.rate,frames);schedule.timer=th16_schedule.regular;
    if(player.state!=1){locked_target=nullptr;target_locked=0;}return ok;
#endif
    if(player.state!=1){locked_target=nullptr;target_locked=0;return true;}
    const i32 frame=schedule.begin(player.state,player.warp_state,player.held,&animations.rate);
    if(frame>=0&&!fire_frame(frame))return false;
    if(schedule.timer.current>=0)schedule.finish(player.held,&animations.rate);
    return true;
}
bool ShotManager::on_update(ShotState& s){
#if PORT_TITLE == 14
    if(u32(s.spec->update)==3){
        if(s.th14_area_slot>=256)return false;Th14BeamUpdate effect;
        if(!shot_th14_beam_update(s,player,th14_areas[s.th14_area_slot],animations.find(s.animation),schedule.timer.current>=0,laser_active[s.spec->option],effect))return false;
        if(effect.pan_active&&!world.th14_beam_pan(effect.pan))return false;
        if(effect.interrupt)interrupt(s.animation,i16(effect.interrupt));
        return !effect.stop||world.th14_beam_stop();
    }
    if(u32(s.spec->update)==4){shot_th14_accelerating_update(s);return true;}
    if(u32(s.spec->update)==5){if(shot_th14_flame_update(s,s.th14_area_slot<256?&th14_areas[s.th14_area_slot]:nullptr,animations.visual_rng))interrupt(s.animation,1);return true;}
    if(u32(s.spec->update)==6){if(shot_th14_small_flame_update(s))interrupt(s.animation,1);return true;}
    if(u32(s.spec->update)==7){
        if(s.th14_area_slot>=256)return false;Th14SakuyaUpdate effect;
        if(!shot_th14_sakuya_update(s,player,th14_areas[s.th14_area_slot],animations.find(s.animation),world.enemies,player.focused!=0,&animations.rate,effect))return false;
        if(effect.interrupt)interrupt(s.animation,i16(effect.interrupt));
        if(effect.explode){
            bool ok=true;family(animations.find(s.animation),[&](AnmVm& a){a.interrupt(7+effect.level);if(a.update(animations)==-2)ok=false;});if(!ok)return false;
            // The immediate ANM interrupt can replace the root scale.
            constexpr float scales[]={.800000011920929f,1.f,1.2999999523162842f,1.600000023841858f,2.f};
            if(auto* a=animations.find(s.animation)){a->flags|=8;a->scale={scales[effect.level],scales[effect.level]};}
            u32 index=0;if(!th14_damage_circle(th14_areas,th14_area_cursor,s.movement.position,0,0,60,s.damage,&animations.rate,index))return false;
            auto& a=th14_areas[index];a.period=2;a.callback=1;a.maximum=10000000;a.size={0,0};
            th14_area_owners[index]=&s;s.th14_area_slot=index;s.th14_area_flags=a.flags;
            if(!world.sound(effect.sound,s.movement.position.x))return false;
        }
        return !effect.release||world.th14_knife_slowdown(*effect.release,effect.slowdown);
    }
    if(u32(s.spec->update)==8){shot_th14_delayed_turn(s);return true;}
    if(u32(s.spec->update)==1){shot_th14_homing_update(s,player.th14_enemy_manager,world.enemies);return true;}
    if(u32(s.spec->update)==2){if(s.state==2)return true;auto* vm=animations.find(s.animation);if(!vm)return false;
        const i32 previous_power=player.th14_cached_power;const u32 previous_area=s.th14_area_slot;
        const i32 result=shot_th14_focused_update(s,player,*vm,world.enemies,resource,schedule.timer.current>=0,&animations.rate);
        if(previous_area<256){auto& area=th14_areas[previous_area];if(result==1)area.flags&=~1u;
            else if(result==0&&previous_power!=player.th14_cached_power)area.radius=s.th14_area_radius;}
        if(result==1)interrupt(s.animation,1);return result>=0;
    }
    if(u32(s.spec->update)>1)return false;
#endif
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
#if PORT_TITLE == 16
    return update_th16();
#endif
    for(auto& s:shots){if(!s.state)continue;if(!s.spec){last_error=6;return false;}
#if PORT_TITLE == 14
        struct ReturnRetiredSlot {
            Th14ShotPool& pool;ShotState& shot;u32 slot;
            ~ReturnRetiredSlot(){if(!shot.state)pool.recycle(slot);}
        } return_retired{th14_shot_pool,s,u32(&s-shots.data())};
#endif
        const bool native_beam=kPortTitle==14&&u32(s.spec->update)==3;
        if(kPortTitle!=14&&s.spec->type==4&&s.state==1&&(schedule.timer.current<0||s.spec->option-1>=player.option_count||player.bomb_active||!player.enemies_present)){
            interrupt(s.animation,1);interrupt(s.extra_animation,1);s.state=2;laser_active[s.spec->option]=0;
        }
        if(kPortTitle!=14&&s.spec->type==4&&!s.hit_this_frame&&s.state==1&&s.hit_last_frame==1){interrupt(s.animation,3);s.hit_last_frame=0;}
        if(!native_beam)s.hit_this_frame=0;if(!on_update(s)){last_error=7;return false;}if(!s.state)continue;
#if PORT_TITLE == 14
        if((s.movement.flags&15)==0){const auto v=polar(s.movement.angle,float(s.movement.speed*animations.rate));s.movement.velocity.x=v.x;s.movement.velocity.y=v.y;}
        else {last_error=16;return false;}
#else
        s.movement.update_velocity();
#endif
        s.movement.update();
        auto* a=animations.find(s.animation);
        if(!a){s.state=0;erase(s.extra_animation);s.animation=s.extra_animation=0;
#if PORT_TITLE == 14
            if(s.spec->type==5){player.th14_cached_power=wrapping_sub(player.th14_cached_power,1);if(s.th14_shot_flags&0x3c)player.th14_attached_count=wrapping_sub(player.th14_attached_count,1);}
            s.th14_area_flags&=~1u;if(s.th14_area_slot<256)th14_areas[s.th14_area_slot].flags&=~1u;
#endif
            continue;}
        if(s.spec->type!=(kPortTitle==14?3:4)&&(kPortTitle==14||s.timer.current>=10)){
#if PORT_TITLE == 14
            // 45115a computes the quad before45115f tests the ten-frame
            // visibility grace period. Keep that call order for young shots.
            Vec3 corners[4];if(!anm_quad_positions(*a,corners,animations.render_space)){last_error=8;return false;}
#if PORT_TITLE == 14
            if(a->drawing_mode()==1)animations.th14_native_stack.geometry_mode1_call();
#endif
            const float left=animations.render_space.play_origin.x-192.f,top=animations.render_space.play_origin.y;
            bool visible=s.timer.current<10;for(const auto p:corners)if(p.x>left&&p.x<left+384.f&&p.y>top&&p.y<top+448.f){visible=true;break;}
            if(!visible){erase(s.animation);s.animation=0;s.state=0;
#else
            if(!a->sprite){last_error=8;return false;}
            const float width=float(double(a->sprite->width)*a->scale.x),height=float(double(a->sprite->height)*a->scale.y);const auto p=s.movement.position;
            if(outside(p,width,height)&&!(s.spec->type==2&&p.x>-224&&p.x<224&&p.y>=-32)){erase(s.animation);s.animation=0;s.state=0;
#endif
#if PORT_TITLE == 14
                s.th14_area_flags&=~1u;if(s.th14_area_slot<256)th14_areas[s.th14_area_slot].flags&=~1u;if(s.spec->type==2)player.th14_cached_power=0;
                if(s.spec->type==5){player.th14_cached_power=wrapping_sub(player.th14_cached_power,1);if(s.th14_shot_flags&0x3c)player.th14_attached_count=wrapping_sub(player.th14_attached_count,1);}
#endif
                continue;}
        }
        a->position=kPortTitle==14?s.movement.position:screen(s.movement.position);
#if PORT_TITLE == 14
        sync_th14_area(s);
#endif
        if(s.extra_animation){auto* extra=animations.find(s.extra_animation);if(extra)extra->position=a->position;else s.extra_animation=0;}
#if PORT_TITLE == 14
        if(a->flags2&0x40){a->rotation.z=s.movement.angle;a->flags|=4;}
#else
        if(a->flags&0x8000000){a->rotation.z=s.movement.angle;a->flags|=4;}
#endif
        s.timer.tick();
    }return true;
}
void ShotManager::invalidate_target(Enemy* e)noexcept{if(locked_target==e){locked_target=nullptr;target_locked=0;}for(auto& s:shots){if(s.target==e){
#if PORT_TITLE == 14
    if(s.spec&&(u32(s.spec->update)==1||u32(s.spec->update)==2||u32(s.spec->update)==7))s.th14_homing_target_retired=true;
#endif
    s.target=nullptr;
    }
#if PORT_TITLE == 16
    if(s.th16.target_id==u32(e->state.instance_id))s.th16.target_id=0;
#endif
}}
bool ShotManager::damage(const Vec3& p,const Vec2& size,bool frame_advanced,GameEconomy& economy,i32& result){
#if PORT_TITLE == 16
    Th16DamageQuery query;query.position=p;query.size=&size;query.advanced=frame_advanced;return damage_th16(query,result);
#endif
#if PORT_TITLE == 14
    return damage_th14(p,&size,0,0,frame_advanced,economy,result);
#else
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
    const i32 combination=player.character*2+player.subtype;
    if(player.special_active&&combination>=2&&combination<=4){i32 special=0;if(!world.special_damage(p,size,special)){last_error=11;return false;}result=wrapping_add(result,special);}
    if(!damage_areas.collect(p,size,result)){last_error=12;return false;}
    if(result>=30){result=wrapping_add(signed_bits(u32(result)*3-90)/5,30);if(result>=70)result=std::min(wrapping_add(wrapping_add(result,-50)/5,50),80);}
    if(result)economy.add_score(wrapping_add(result/10,10));
    return true;
#endif
}
#if PORT_TITLE == 14
bool ShotManager::th14_empty_damage(bool frame_advanced,i32& result)const noexcept{
    result=0;if(!frame_advanced)return true;
    // TH14's six direct Bomb damage virtuals return zero. Only occupied
    // damage areas/shots prevent this empty-pool shortcut.
    for(const auto& s:shots)if(s.state)return false;
    for(const auto& a:damage_areas.areas)if(a.flags&1)return false;
    for(const auto& a:th14_areas)if(a.flags&1)return false;
    return true;
}
#endif
}
