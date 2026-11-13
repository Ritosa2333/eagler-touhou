#include "ShotManager.hpp"
#include <cmath>
namespace th11 {
#if PORT_TITLE == 14
namespace {
Enemy* nearest(EnemyLink* first,Vec3 p,float radius){
    Enemy* found=nullptr;float limit=radius*radius;
    for(auto* n=first;n;n=n->next){auto* e=n->value;if(e->state.flags&0xc000021u)continue;const auto q=e->state.current.position;
        const float x=p.x-q.x,y=p.y-q.y,d=x*x+y*y;if(limit>d){found=e;limit=d;}}
    return found;
}
bool exists(EnemyLink* first,u32 id){if(!id)return false;for(auto* n=first;n;n=n->next)if(u32(n->value->state.instance_id)==id)return true;return false;}
bool scale(AnmVm& vm,i32 power){constexpr float scales[]={0,1,1.2000000476837158f,1.399999976158142f,1.75f};if(u32(power)>=5)return false;vm.flags|=8;vm.scale={scales[power],scales[power]};return true;}
float difference(float target,float angle){const float d=target-angle;if(d>3.1415927410125732f)return target-(angle+6.2831854820251465f);if(angle-target>3.1415927410125732f)return target-(angle-6.2831854820251465f);return d;}
float aim(Vec3 target,Vec3 p){const float x=target.x-p.x,y=target.y-p.y;return float(std::atan2(double(y),double(x)));}
}
void shot_th14_homing_update(ShotState& s,bool manager_present,EnemyLink* enemies)noexcept{
    if(s.state==2)return;
    if(!manager_present){s.target=nullptr;s.th14_homing_target_retired=false;}
    else if(s.th14_homing_target_retired){s.th14_homing_target_retired=false;s.target=nullptr;s.th14_target_id=0;}
    else if(!s.target){s.target=nearest(enemies,s.movement.position,256.f);if(s.target)s.th14_target_id=u32(s.target->state.instance_id);}
    if(s.target){
        if(!exists(enemies,s.th14_target_id)){s.th14_target_id=0;s.target=nullptr;}
        else if(s.target->state.flags&0xc000021u)s.target=nullptr;
    }
    auto& v=s.movement;
    if(!s.target){v.speed=std::min(v.speed+.10000000149011612f,16.f);return;}
    float turn=difference(aim(s.target->state.current.position,v.position),v.angle);
    if(s.timer.current>=60){v.speed+=.20000000298023224f;return;}
    const float magnitude=std::abs(turn);
    if(magnitude>=.7853981852531433f)v.speed=std::max(v.speed-.20000000298023224f,4.f);
    else if(magnitude<.2617993950843811f)v.speed=std::min(v.speed+.20000000298023224f,16.f);
    v.angle=normalize_angle(normalize_angle(v.angle+turn*.07999999821186066f));
}
void shot_th14_delayed_turn(ShotState& s)noexcept{
    if(s.timer.current==signed_bits(s.spec->native_tail[0]))s.movement.angle=-1.5707963705062866f;
}
bool shot_th14_focused_spawn(ShotState& s,const ShotPlayer& player,AnmVm& vm,EnemyLink* enemies,const float* rate)noexcept{
    s.target=nullptr;s.th14_target_id=0;s.th14_area_flags|=2;s.th14_area_radius=s.spec->hitbox.x;s.th14_area_growth=0;
    s.target=nearest(enemies,s.movement.position,160.f);
    if(s.target){s.th14_target_id=u32(s.target->state.instance_id);if(!(s.target->state.flags&0xc000021u))s.movement.angle=normalize_angle(aim(s.target->state.current.position,s.movement.position));}
    s.th14_saved_speed=s.movement.speed;s.th14_decay.set(10,rate);
    return player.power_step&&scale(vm,player.power/player.power_step);
}
i32 shot_th14_focused_update(ShotState& s,ShotPlayer& player,AnmVm& vm,EnemyLink* enemies,const ShtResource& resource,bool firing,const float* rate)noexcept{
    if(s.state==2)return 0;
    // 451cf8..451d49 searches only when the retained pointer was null on
    // entry. A destroyed target still reaches the later identity check at
    // 451e53..451fc7; preserve that ordering without a dangling C++ pointer.
    if(s.timer.current>=10&&!s.target&&!s.th14_homing_target_retired){s.target=nearest(enemies,s.movement.position,320.f);if(s.target)s.th14_target_id=u32(s.target->state.instance_id);}
    // 452033 clears the retained area pointer. Later VM retirement must
    // not clear a new shot's reused damage-area record.
    const auto terminate=[&](){s.state=2;s.th14_area_flags&=~1u;s.th14_area_slot=256;player.th14_cached_power=0;return 1;};
    if(player.th14_shooting_blocked||player.state==2||player.special_active==1)return terminate();
    if(player.focused&&firing)s.th14_steering.set(0,rate);
    else{s.th14_steering.tick();if((!s.target&&!s.th14_homing_target_retired)||s.th14_steering.current>=12)return terminate();}
    if(!player.power_step)return -2;const i32 power=player.power/player.power_step;
    if(player.th14_cached_power!=power){
        if(!scale(vm,power))return -2;const i32 group=resource.header.max_power_level+1+power;
        if(group<0||u32(group)>=resource.groups.size()||resource.groups[group].shots.size()<3)return -2;
        const auto& spec=resource.groups[group].shots[2];s.th14_area_radius=spec.hitbox.x;s.damage=spec.damage;player.th14_cached_power=power;
    }
    if(s.th14_homing_target_retired){s.th14_homing_target_retired=false;s.target=nullptr;s.th14_target_id=0;}
    else if(s.target&&(!exists(enemies,s.th14_target_id)||(s.target->state.flags&0xc000021u))){s.target=nullptr;s.th14_target_id=0;}
    if(s.th14_decay.current>4){
        float speed=s.th14_saved_speed;
        if(s.target){
            float turn=difference(aim(s.target->state.current.position,s.movement.position),s.movement.angle);
            const float x=s.movement.position.x-s.target->state.current.position.x,y=s.movement.position.y-s.target->state.current.position.y,d=x*x+y*y;
            if(s.timer.current<=30||d<=256.f){
                if(s.timer.current>30)speed*=.5f;
            }else if(d<=14400.f)speed=(speed-3.f)*.8999999761581421f+3.f;
            else{speed+=.15000000596046448f;if(speed>16.f){speed=16.f;turn*=.699999988079071f;goto apply;}}
            turn*=speed<1.f?.8999999761581421f:speed<4.f?.800000011920929f:.699999988079071f;
            apply:s.movement.angle=normalize_angle(s.movement.angle+turn);
        }else speed*=.8999999761581421f;
        s.th14_saved_speed=s.movement.speed=speed;
    }
    s.th14_decay.tick();return 0;
}
bool shot_th14_focused_hit(ShotState& s,ShotPlayer& player,Rng& rng,const float* rate,Th14FocusedHit& out)noexcept{
    out={};if(s.timer.current==s.timer.previous||s.timer.current%8)return true;
    if(!player.power_step)return false;const i32 power=player.power/player.power_step;
    constexpr u32 increments[]={15,50,100,150,200};if(u32(power)>=5)return false;
    s.movement.speed=0;
    const float radius=rng.unit()*s.th14_area_radius;const auto offset=polar(s.movement.angle,radius);
    out.position={s.movement.position.x+offset.x,s.movement.position.y+offset.y,s.movement.position.z};
    out.angle=normalize_angle(s.movement.angle+rng.signed_unit()*1.5707963705062866f);
    s.th14_saved_speed=s.timer.current<30?s.th14_saved_speed*.5f:-2.f;
    s.th14_decay.set(0,rate);player.th14_focused_hits+=increments[power];
    out.damage=s.damage;out.active=true;return true;
}
bool shot_th14_plain_hit(ShotState& s,ShotPlayer& player,Rng& rng,Th14PlainHit& out)noexcept{
    out={};if(!s.spec)return false;
    const u32 callback=s.spec->extra_callback;
    if(callback==5){
        (s.th14_was_focused?player.th14_focused_hits:player.th14_plain_hits)+=s.spec->native_tail[0];return true;
    }
    if(callback!=2&&callback!=7)return false;
    // 452240 / 453910 consume the angle sample before four independent
    // color samples. Both normalize once before and once after adding pi.
    const auto normalize=[](float angle){constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;u32 n=0;
        while(angle>pi){angle-=tau;if(n++>32)break;}
        while(-pi>angle){angle+=tau;if(n++>32)break;}return angle;};
    out.angle=normalize(normalize(s.movement.angle+rng.signed_unit()*.3490658402442932f)+3.1415927410125732f);
    out.position=s.movement.position;out.effect=true;
    const u32 red=(rng.next32()&(callback==2?127u:63u))+(callback==2?127u:32u);
    const u32 green=(rng.next32()&(callback==2?63u:127u))+(callback==2?64u:127u);
    const u32 blue=(rng.next32()&63u)+(callback==2?64u:32u);
    const u32 alpha=(rng.next32()&63u)+(callback==2?96u:56u);
    out.color=(alpha<<24)|(red<<16)|(green<<8)|blue;
    if(callback==2){constexpr u32 amounts[5]={15,5,7,10,15};if(!player.power_step)return false;
        const i32 power=player.power/player.power_step;if(u32(power)>=5)return false;player.th14_focused_hits+=amounts[power];}
    else{const i32 age=s.timer.current;player.th14_focused_hits+=age<2?60:age<4?50:age<6?40:age<10?35:30;}
    return true;
}
bool ShotManager::bind_th14_area(ShotState& s){
    u32 index=0;
    if(!th14_damage_rectangle(th14_areas,th14_area_cursor,s.movement.position,s.spec->hitbox,s.movement.angle,9999999,s.damage,&animations.rate,index)){last_error=15;return false;}
    auto& a=th14_areas[index];a.maximum=10000000;a.callback=1;
    s.th14_area_slot=index;s.th14_area_flags=a.flags;th14_area_owners[index]=&s;
    if(s.spec->type==2&&player.power_step)player.th14_cached_power=player.power/player.power_step;
    return true;
}
void ShotManager::sync_th14_area(ShotState& s)noexcept{
    if(s.th14_area_slot>=256)return;auto& a=th14_areas[s.th14_area_slot];
    // 4512af..4512ef writes geometry through a retained pointer, even when
    // another shot has reused it. Flags/radius belong to their callbacks.
    if(s.state&&(s.th14_shot_flags&1)){a.angle=s.movement.angle;a.damage=s.damage;a.position=s.movement.position;a.size=s.spec->hitbox;}
}
void ShotManager::update_th14_areas()noexcept{
    for(auto& a:th14_areas)th14_damage_tick(a);
}
bool ShotManager::th14_death_area(Vec3 p){
    u32 index=0;if(!th14_damage_circle(th14_areas,th14_area_cursor,p,32,16,30,150,&animations.rate,index)){last_error=15;return false;}
    // 450130/450270 clear the callback at+a0 but retain its argument at+9c.
    // Shot records have stable pool storage, including retired records.
    return true;
}
bool ShotManager::th14_bomb_circle(Vec3 p,float radius,float growth,i32 duration,i32 damage){
    u32 index=0;if(!th14_damage_circle(th14_areas,th14_area_cursor,p,radius,growth,duration,damage,&animations.rate,index)){last_error=15;return false;}
    th14_areas[index].flags|=4;return true;
}
bool ShotManager::th14_bomb_orb_area(Vec3 p,u32& index){
    if(!th14_damage_circle(th14_areas,th14_area_cursor,p,56,0,9999,15,&animations.rate,index)){last_error=15;return false;}
    auto& a=th14_areas[index];a.flags|=4;a.period=3;a.maximum=300;return true;
}
bool ShotManager::th14_bomb_rectangle(Vec3 p,Vec2 size,float angle,i32 duration,i32 damage){
    u32 index=0;if(!th14_damage_rectangle(th14_areas,th14_area_cursor,p,size,angle,duration,damage,&animations.rate,index)){last_error=15;return false;}
    th14_areas[index].flags|=4;return true;
}
bool ShotManager::th14_reimu_bomb_area(Vec3 p,Vec2 size,i32 duration,u32& index){
    if(!th14_damage_rectangle(th14_areas,th14_area_cursor,p,size,0,duration,300,&animations.rate,index)){last_error=15;return false;}
    auto& a=th14_areas[index];a.flags|=4;a.period=10;a.callback=2;return true;
}
void shot_th14_reimu_bomb_hit(const Th14DamageArea& a,Vec3 hit,Rng& rng,Vec3& position,float& angle,float polar_z)noexcept{
    // Retail 414020 consumes two signed script-RNG units, then aims the effect
    // from the damage-area center toward the unperturbed contact point.
    const float radius=rng.signed_unit()*16.f,scatter=rng.signed_unit()*3.1415927410125732f;
    position={hit.x+float(std::cos(double(scatter))*radius),hit.y+float(std::sin(double(scatter))*radius),float(hit.z+polar_z)};
    angle=float(std::atan2(double(hit.y-a.position.y),double(hit.x-a.position.x)));
}
Vec3 shot_th14_reward_position(const Th14DamageArea& area,Vec3 target)noexcept{
    // 451760 reads the damage-area center (+2c/+30), including the beam offset.
    return {(area.position.x+target.x)*.5f,(area.position.y+target.y)*.5f,0};
}
bool shot_th14_hit_rewards(ShotPlayer& player,Rng& rng,Vec3 p,ShotWorld& world)noexcept{
    if(!player.power_step)return false;const i32 power=player.power/player.power_step;if(u32(power)>=5)return false;
    while(signed_bits(player.th14_plain_hits)>=500){
        if(power<4){if(!world.th14_hit_item(1,p,-1.5707963705062866f,2.200000047683716f))return false;player.th14_plain_hits-=u32(200+power*60);}
        else{const float angle=rng.signed_unit()*.5235987901687622f-1.5707963705062866f;if(!world.th14_hit_item(9,p,angle,2.200000047683716f))return false;player.th14_plain_hits-=50;}
    }
    while(signed_bits(player.th14_focused_hits)>=100){
        const float speed=rng.unit()*1.2000000476837158f;
        const float angle=rng.signed_unit()*.5235987901687622f-1.5707963705062866f;
        if(!world.th14_hit_item(9,p,angle,speed+1.f))return false;player.th14_focused_hits-=100;
    }
    // 454091..454274 consumes the explosive-shot power counter after both
    // preceding reward loops, without drawing RNG, even when its pool is full.
    while(signed_bits(player.th14_hits_184bc)>=100){
        if(!world.th14_hit_item(1,p,-1.5707963705062866f,2.200000047683716f))return false;
        player.th14_hits_184bc-=100;
    }return true;
}
bool ShotManager::hit(u32 j,Th14DamageArea& area,const Vec3& p,const Vec2* size,float radius,float angle,i32& out){
    if(area.callback==2){Vec3 position;float rotation;shot_th14_reimu_bomb_hit(area,p,animations.script_rng,position,rotation,animations.th14_native_stack.damage_polar_z());
        animations.th14_native_stack.damage_polar_store(position.z);
        out=area.damage;if(!world.th14_reimu_bomb_hit_effect(position,rotation)){last_error=14;return false;}return true;}
    auto* owner=th14_area_owners[j];if(!owner||!owner->spec){last_error=6;return false;}auto& s=*owner;
    // 451760 calls 453a90 before the shot callback.
    const Vec3 midpoint=shot_th14_reward_position(area,p);
    animations.th14_native_stack.damage_hit_wrapper(midpoint.y);
    if(!shot_th14_hit_rewards(player,animations.visual_rng,midpoint,world)){last_error=14;return false;}
    if(s.spec->extra_callback==3){Th14BeamHit effect;
        if(!shot_th14_beam_hit(s,player,area,p,size,radius,angle,effect)){
            th14_beam_failure={s.movement.position.x,s.movement.position.y,s.movement.angle,
                area.size.x,area.size.y,p.x,p.y,radius,angle,size?size->x:0,size?size->y:0,float(s.timer.current)};
            last_error=9;return false;
        }
        if(effect.interrupt)interrupt(s.animation,2);
        if(effect.effect&&!world.th14_beam_hit_effect(effect.position,effect.angle,effect.travel)){last_error=2;return false;}
        out=effect.damage;return true;
    }
    if(s.spec->extra_callback==6){Th14SakuyaHit effect;
        if(!shot_th14_sakuya_hit(s,player,area,p,size,radius,world.enemies,effect,th14_damage_target)){last_error=9;return false;}
        if(effect.interrupt)interrupt(s.animation,i16(effect.interrupt));
        if(effect.sound&&!world.sound(effect.sound,s.movement.position.x)){last_error=4;return false;}
        if(effect.attachment){if(!world.th14_knife_slowdown(*effect.attachment,effect.slowdown)){last_error=14;return false;}auto& slowdown=effect.attachment->state.th14_knife_slowdown;if(slowdown>.8999999761581421f)slowdown=.8999999761581421f;}
        out=effect.damage;return true;
    }
    if(s.spec->extra_callback==1){
        Th14FocusedHit effect;if(!shot_th14_focused_hit(s,player,animations.script_rng,&animations.rate,effect)){last_error=9;return false;}
        out=effect.damage;if(!effect.active)return true;
        interrupt(s.animation,3);
        if(!world.th14_hit_effect(effect.position,effect.angle)){last_error=2;return false;}
        if(!world.sound(68,s.movement.position.x)){last_error=4;return false;}
        return true;
    }
    if(s.spec->extra_callback==4){
        if(s.movement.flags&15){last_error=9;return false;}
        u32 index=0;if(!th14_damage_circle(th14_areas,th14_area_cursor,s.movement.position,24,1,20,s.spec->damage,&animations.rate,index)){last_error=15;return false;}
        interrupt(s.animation,1);area.flags&=~1u;
        if(!shot_th14_explosive_hit(s,player,th14_areas[index])){last_error=9;return false;}th14_area_owners[index]=nullptr;
        if(!world.sound(65,s.movement.position.x)){last_error=4;return false;}out=s.damage;return true;
    }
    if(s.spec->extra_callback==8){player.th14_focused_hits+=s.spec->native_tail[3];}
    else if(s.spec->extra_callback){Th14PlainHit effect;
        if(!shot_th14_plain_hit(s,player,animations.script_rng,effect)){last_error=9;return false;}
        if(effect.effect&&!world.th14_plain_hit_effect(effect.position,effect.angle,effect.color)){last_error=2;return false;}
    }else (s.th14_was_focused?player.th14_focused_hits:player.th14_plain_hits)+=5;
    auto* vm=animations.find(s.animation);if(!vm){last_error=10;return false;}
    // 451855 interrupts only this VM;47f140's family broadcast belongs to
    // the explicit shot callbacks. Children keep running until root deletion.
    vm->interrupt(1);s.movement.position.z=.1f;s.state=2;s.movement.speed*=.125f;vm->position=s.movement.position;
    area.flags&=~1u;s.th14_area_flags&=~1u;s.th14_area_slot=256;out=s.damage;return true;
}
bool ShotManager::damage_th14(Vec3 p,const Vec2* size,float radius,float angle,bool advanced,GameEconomy& economy,i32& out,u32 argument,u32 stamp,Enemy* hit_target,bool probe,bool* bomb_contact){
    struct TargetScope {Enemy*& slot;Enemy* previous;~TargetScope(){slot=previous;}} scope{th14_damage_target,th14_damage_target};
    th14_damage_target=hit_target;
    out=0;if(!advanced)return true;
    if(player.special_active){
        i32 direct=0;
        // 451479 calls the active Bomb's damage virtual before traversing
        // the pool. All six TH14 implementations return zero; an active
        // Bomb does not invalidate the attached shot/death damage areas.
        if(!world.special_damage(p,size?*size:Vec2{},direct)||direct!=0){last_error=13;return false;}
    }
    for(const auto& a:damage_areas.areas)if(a.flags&1){last_error=13;return false;}
    // Retail 423a54 passes enemy+280 to451430. The collector's last hit
    // position supplies the direction used by the later424af0 death effect.
    Vec3 last_hit=hit_target?Vec3{hit_target->state.death_reference.x,hit_target->state.death_reference.y,0}:Vec3{};
    bool bomb=false;if(!th14_damage_collect(th14_areas,*this,p,size,radius,angle,advanced,probe,argument,stamp,signed_bits(resource.header.native_extra[0]),economy,out,bomb,hit_target?&last_hit:nullptr,&animations.th14_native_stack)){if(!last_error)last_error=12;return false;}
    if(bomb_contact)*bomb_contact=bomb;
    if(hit_target)hit_target->state.death_reference={last_hit.x,last_hit.y};
    return true;
}
#endif
}
