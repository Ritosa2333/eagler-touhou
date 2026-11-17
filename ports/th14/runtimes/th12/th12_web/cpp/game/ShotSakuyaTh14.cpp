#include "ShotManager.hpp"
#include <cmath>
#include <algorithm>
namespace th11 {
#if PORT_TITLE == 14
// Retail 452f30 limits the total type-5 knives separately from attached knives.
i32 shot_th14_sakuya_spawn(ShotState& s,ShotPlayer& p,const float* rate)noexcept{
    if(!s.spec||p.th14_cached_power>=signed_bits(s.spec->native_tail[1]))return -1;
    p.th14_cached_power=wrapping_add(p.th14_cached_power,1);
    s.th14_shot_flags&=0xffffffc3u;s.target=nullptr;s.th14_target_id=0;
    s.th14_steering.set(0,rate);s.th14_decay.set(0,rate);return 0;
}
namespace {
Enemy* find(EnemyLink* first,u32 id){for(auto* n=first;n;n=n->next)if(u32(n->value->state.instance_id)==id)return n->value;return nullptr;}
Enemy* nearest(EnemyLink* first,Vec3 p){Enemy* found=nullptr;float limit=600.f*600.f;for(auto* n=first;n;n=n->next){auto& e=n->value->state;if(e.flags&0xc000021u)continue;const float x=p.x-e.current.position.x,y=p.y-e.current.position.y,d=x*x+y*y;if(d<limit){limit=d;found=n->value;}}return found;}
float tail_float(const ShotSpec& s,u32 n){float v;const u32 bits=s.native_tail[n];std::memcpy(&v,&bits,4);return v;}
}
void shot_th14_sakuya_slowdown(EnemyState& e,float delta,u32 flags,i32 spell)noexcept{
    if(e.flags&0x800000){e.th14_knife_slowdown+=delta/6.f;if(e.th14_knife_slowdown>=.33000001311302185f)e.th14_knife_slowdown=.33000001311302185f;
        if((flags&1)&&(spell==0x77||spell==0x71||spell==0x60||spell==0x61||spell==0x62||spell==0x63))e.th14_knife_slowdown=0;
    }else e.th14_knife_slowdown+=delta;
}
float enemy_th14_knife_rate(float rate,float slowdown)noexcept{return slowdown>0?std::clamp(rate-slowdown*rate,0.f,1.f):rate;}
bool shot_th14_sakuya_update(ShotState& s,ShotPlayer& p,Th14DamageArea& area,AnmVm* vm,EnemyLink* enemies,bool focused,const float* rate,Th14SakuyaUpdate& out)noexcept{
    out={};if(!s.spec)return false;const u32 phase=(s.th14_shot_flags>>2)&15;
    if(!phase){const i32 age=s.timer.current,threshold=signed_bits(s.spec->native_tail[0]);
        if(age<threshold){
            if(!p.th14_enemy_manager){s.target=nullptr;s.th14_homing_target_retired=false;}
            // Retail452ff0 retains the old pointer until421a60 rejects its ID,
            // then skips acquisition/aiming for this update. The enemy object
            // is already freed here, so retain that validation frame as a flag.
            else if(s.th14_homing_target_retired){s.th14_homing_target_retired=false;s.target=nullptr;s.th14_target_id=0;}
            else if(!s.target){s.target=nearest(enemies,s.movement.position);if(s.target)s.th14_target_id=u32(s.target->state.instance_id);}
            if(s.target){if(!find(enemies,s.th14_target_id)){s.target=nullptr;s.th14_target_id=0;}
                else{const auto q=s.target->state.current.position;const float x=q.x-s.movement.position.x,y=q.y-s.movement.position.y;s.movement.angle=normalize_angle(float(std::atan2(double(y),double(x))));}}
        }else if(age==threshold){out.interrupt=2;s.movement.speed=36;}
    }else if(phase==1){
        if(p.th14_attached_count>=signed_bits(s.spec->native_tail[1]))s.th14_decay.tick();
        if(!find(enemies,s.th14_target_id))s.target=nullptr;
        if(s.target&&focused&&p.state==1&&!p.th14_dialogue_blocked&&s.th14_decay.current<signed_bits(s.spec->native_tail[3])){
            const auto q=s.target->state.current.position,o=s.th14_attachment_offset;s.movement.position={q.x+o.x,q.y+o.y,q.z+o.z};
        }else{
            const i32 count=s.target?s.target->state.th14_attached_knives:0;out.level=count<10?0:count<14?1:count<18?2:count<22?3:4;
            constexpr float scales[]={.800000011920929f,1.f,1.2999999523162842f,1.600000023841858f,2.f};
            if(!p.power_step)return false;constexpr i32 damage[]={4,9,9,10,11};out.explode=true;out.sound=out.level<p.power/p.power_step?22:72;
            if(vm){vm->flags|=8;vm->scale={scales[out.level],scales[out.level]};}else s.animation=0;
            s.th14_shot_flags=(s.th14_shot_flags&0xffffffcbu)|8;s.th14_steering.set(0,rate);s.damage=damage[out.level];s.th14_area_radius=s.th14_area_growth=0;s.th14_area_flags|=3;
        }
    }else{
        AnmVm* child=nullptr;if(vm)for(auto* n=&vm->child;n;n=n->next)if(n->value&&n->value!=vm&&n->value->script_index==14){child=n->value;break;}
        if(child){area.flags|=2;s.th14_area_flags|=2;area.radius=(child->scale.x*64.f)*.5f*vm->scale.x;area.growth=0;s.th14_area_radius=area.radius;s.th14_area_growth=0;}
        else{
            area.flags&=~1u;s.th14_area_flags&=~1u;s.th14_area_slot=256;s.state=0;s.animation=0;
            p.th14_cached_power=wrapping_sub(p.th14_cached_power,1);p.th14_attached_count=wrapping_sub(p.th14_attached_count,1);out.retired=true;
            if(s.target&&find(enemies,s.th14_target_id)){out.release=s.target;out.slowdown=-tail_float(*s.spec,5);s.target->state.th14_attached_knives=wrapping_sub(s.target->state.th14_attached_knives,1);}return true;
        }
    }
    s.th14_steering.tick();return true;
}
bool shot_th14_sakuya_hit(ShotState& s,ShotPlayer& p,Th14DamageArea& area,Vec3 target,const Vec2* size,float radius,EnemyLink* enemies,Th14SakuyaHit& out,Enemy* hit_target)noexcept{
    out={};if(!s.spec)return false;const u32 phase=(s.th14_shot_flags>>2)&15;
    if(!phase){
        out.interrupt=6;out.sound=64;s.th14_shot_flags=(s.th14_shot_flags&0xffffffc7u)|4;s.movement.speed=0;
        p.th14_attached_count=wrapping_add(p.th14_attached_count,1);area.flags&=~1u;s.th14_area_flags&=~1u;
        if(!size){const auto origin=s.movement.position;const float expanded=area.size.y*.5f+radius,dx=target.x-origin.x,dy=target.y-origin.y;
            const float sn=float(std::sin(-double(s.movement.angle))),cs=float(std::cos(-double(s.movement.angle)));
            const float x=dx*cs-dy*sn,y=dy*cs+dx*sn;float distance=origin.x;
            if(std::abs(y)<=expanded&&x>=-expanded&&(x>=0||x*x+y*y<=expanded*expanded)&&expanded!=0){const float t=y/expanded;distance=x-float(std::sqrt(double(1.f-t*t)))*expanded;}
            const float amount=radius*.699999988079071f+distance,sn2=float(std::sin(double(s.movement.angle))),cs2=float(std::cos(double(s.movement.angle)));
            s.movement.position={origin.x+amount*cs2,origin.y+amount*sn2,origin.z+amount*0.f};
        }
        // 453798 reads the actual hit enemy from the callback argument. A
        // constructor can receive damage before joining the manager list.
        s.target=hit_target?hit_target:find(enemies,area.argument);if(s.target){auto& e=s.target->state;s.th14_target_id=u32(e.instance_id);const auto q=e.current.position,at=s.movement.position;s.th14_attachment_offset={at.x-q.x,at.y-q.y,at.z-q.z};
            out.attachment=s.target;out.slowdown=tail_float(*s.spec,5);e.th14_attached_knives=wrapping_add(e.th14_attached_knives,1);}
    }else if(phase==2&&s.timer.current!=s.timer.previous&&s.timer.current%2==0){
        if(!p.power_step)return false;const i32 power=p.power/p.power_step;constexpr u32 reward[]={15,10,12,14,18};if(u32(power)>=5)return false;
        p.th14_focused_hits+=reward[power];out.damage=area.damage;
    }return true;
}
#endif
}
