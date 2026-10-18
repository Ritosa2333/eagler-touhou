#include "PlayerShots.hpp"
#include "GameMath.hpp"
namespace th08 {
namespace {
struct Bounds {Vec2 min,max;};
Bounds bounds(const Vec3& p,const Vec3& s){return {{(number(p.x)-number(s.x)*number(.5f)).to_float(),(number(p.y)-number(s.y)*number(.5f)).to_float()},{(number(s.x)*number(.5f)+number(p.x)).to_float(),(number(s.y)*number(.5f)+number(p.y)).to_float()}};}
bool overlap(const Bounds& a,const Bounds& b){return !(b.max.y<a.min.y)&&!(b.max.x<a.min.x)&&!(a.max.y<b.min.y)&&!(a.max.x<b.min.x);}
bool le(Extended a,Extended b){return a<b||a==b;}
}
bool PlayerShots::hit_callback(PlayerShot& shot,const Vec3& position){
    failure=Failure::None;
    if(shot.hit==ShotHit::Explosion){
        if(shot.state==2){
            if(shot.timer.current%2||(state.time_spell&&shot.timer.current%4))return true;
            shot.damage/=3;if(!shot.damage)shot.damage=1;
            shot.velocity.x=Scalar::mul(shot.velocity.x,.88f);shot.velocity.y=Scalar::mul(shot.velocity.y,.88f);
        }else{
            const float angle=(rng.range(1.5707963705062866f)-number(2.356194496154785f)).to_float();
            float size=0;switch(shot.animation.scriptIndex){case 12:size=48;break;case 14:size=64;break;case 16:size=80;break;case 18:size=96;break;case 20:size=128;break;}
            if(size){shot.size.x=shot.size.y=size;direction(shot,angle,6);}
        }
        if(shot.timer.current%6==0&&actions)actions->effect(5,position);
    }else if(shot.hit==ShotHit::Laser){
        ++state.effect_counter;if(!(state.effect_counter&7)&&actions)actions->effect(5,{shot.position.x,position.y,position.z});
    }
    return false;
}
i32 PlayerShots::damage(const Vec3& position,const Vec3& size,i32& time_items,i32* bomb_hit){
    failure=Failure::None;if(!state.collision_timer.changed())return 0;
    const auto target=bounds(position,size);i32 total=0;if(bomb_hit)*bomb_hit=0;
    for(auto& shot:state.shots){
        if(!shot.state||(shot.state!=1&&shot.kind!=3)||!overlap(bounds(shot.position,shot.size),target))continue;
        if((shot.kind==4||shot.kind==5)&&shot.timer.current%2)continue;
        if(hit_callback(shot,position))continue;
        i32 damage=shot.damage;if(state.bomb){damage/=5;if(!damage)damage=1;}total=wrapping_add(total,damage);
        if(state.time_item_threshold<=0){failure=Failure::InvalidInterval;return total;}
        while(time_items>=state.time_item_threshold){
            if(state.human_bonus){if(!shot.definition){failure=Failure::MissingDefinition;return total;}if(shot.definition->gauge<0&&actions)actions->item(7,shot.position,3);}
            time_items=wrapping_sub(time_items,state.time_item_threshold);
        }
        if(shot.kind!=4&&shot.kind!=5&&shot.kind!=6){
            if(shot.state==1){if(!actions){failure=Failure::MissingActions;return total;}const float angle=shot.animation.rotation.z;actions->animation(shot.animation,wrapping_add(shot.animation_index,11));shot.animation.rotation.z=angle;actions->effect(5,shot.position);shot.position.z=.1f;}
            shot.state=2;if(shot.kind!=3){shot.velocity.x=Scalar::div(shot.velocity.x,8);shot.velocity.y=Scalar::div(shot.velocity.y,8);}
        }
    }
    time_items=wrapping_add(time_items,total<51?total:50);
    for(auto& area:state.regions.damaging){
        if(!area.active)continue;if(!area.interval){failure=Failure::InvalidInterval;return total;}if(area.lifetime%area.interval)continue;
        bool hit=false;
        if(area.radius!=0){
            const auto dx=number(area.position.x)-number(position.x),dy=number(area.position.y)-number(position.y);
            hit=le(dx*dx+dy*dy,number(area.radius)*number(area.radius));
        }else if(area.angle==0){
            hit=!(number(target.max.x)<number(area.position.x)-number(area.dimensions.x)/number(2))
                &&!(number(area.dimensions.x)/number(2)+number(area.position.x)<number(target.min.x))
                &&!(number(target.max.y)<number(area.position.y)-number(area.dimensions.y)/number(2))
                &&!(number(area.dimensions.y)/number(2)+number(area.position.y)<number(target.min.y));
        }else{
            const float x=Scalar::sub(position.x,area.position.x),y=Scalar::sub(position.y,area.position.y);
            const float s=sine((-number(area.angle)).to_float()).to_float(),c=cosine((-number(area.angle)).to_float()).to_float();
            const float rx=(number(c)*number(x)-number(s)*number(y)).to_float(),ry=(number(s)*number(x)+number(c)*number(y)).to_float();
            hit=le(-number(area.dimensions.x)/number(2),number(size.x)/number(2)+number(rx))&&le(number(rx)-number(size.x)/number(2),number(area.dimensions.x)/number(2))
                &&le(-number(area.dimensions.y)/number(2),number(size.y)/number(2)+number(ry))&&le(number(ry)-number(size.y)/number(2),number(area.dimensions.y)/number(2));
        }
        if(!hit)continue;
        total=wrapping_add(total,area.hit_damage);area.damage_dealt=wrapping_add(area.damage_dealt,area.hit_damage);
        if(area.damage_limit>0&&area.damage_dealt>=area.damage_limit){area.hit_damage=0;total=wrapping_sub(total,wrapping_sub(area.damage_dealt,area.damage_limit));}
        if(!area.suppress_effect){++state.effect_counter;if(!(state.effect_counter&3)&&actions)actions->effect(3,position);}
        if(state.bomb&&bomb_hit)*bomb_hit=1;
    }
    if(state.youkai_bonus&&total)total=wrapping_add(0,signed_bits(u32(total)*106))/100;return total;
}
}
