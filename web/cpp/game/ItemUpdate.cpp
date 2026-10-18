#include "ItemUpdate.hpp"
#include "GameMath.hpp"
#include <cmath>
namespace th08 {
namespace {
void move(ItemState& item,float rate){
    item.position.x=(number(item.position.x)+number(Scalar::mul(item.velocity.x,rate))).to_float();
    item.position.y=(number(item.position.y)+number(Scalar::mul(item.velocity.y,rate))).to_float();
    item.position.z=(number(item.position.z)+number(Scalar::mul(item.velocity.z,rate))).to_float();
}
float aim(const Vec3& player,const Vec3& p){
    const float x=Scalar::sub(player.x,p.x),y=Scalar::sub(player.y,p.y);
    return x==0&&y==0?1.5707963705062866f:float(std::atan2(double(y),double(x)));
}
}
void ItemUpdate::update(){
    i32 sound=0;state.count=0;const Vec3 size{human.item_radius,human.item_radius,16};
    const float falling=Scalar::mul(context.focused?focused.item_fall_speed:human.item_fall_speed,timing.rate);
    for(auto* p=state.head.next;p;p=p->next){++state.count;bool common_motion=true,can_collect=true;
        if(p->state==2){
            if(p->timer.current<60){
                const float f=(p->timer.value()/number(60)).to_float(),inverse=Scalar::sub(1,f);
                const auto blend=[&](float target,float start){return (number(Scalar::mul(target,f))+number(Scalar::mul(start,inverse))).to_float();};
                p->position={blend(p->target.x,p->velocity.x),blend(p->target.y,p->velocity.y),blend(p->target.z,p->velocity.z)};common_motion=false;
            }else if(p->timer.current==60){p->velocity={0,0,0};p->state=0;}
        }else if(p->state==3){
            p->velocity.y=(number(.05f)*number(timing.rate)+number(p->velocity.y)).to_float();
            if(p->velocity.y>0||context.shooting.current<0)p->state=1;
            if(context.player_state==2){p->state=0;p->velocity={0,-.7f,0};}
        }else if(p->state==5){
            p->velocity.y=(number(.05f)*number(timing.rate)+number(p->velocity.y)).to_float();move(*p,falling);
            if(p->velocity.y>0){p->state=1;if(context.player_state==2){p->state=0;p->velocity={0,-.7f,0};}}
            else{common_motion=false;can_collect=false;}
        }else if(p->state==1||(context.player.y<human.item_collect_line&&(context.power>=128||context.focused||context.character==1||context.character==6))){
            if(context.player_state!=2&&context.player_state!=1){
                const float angle=aim(context.player,p->position);p->velocity.x=(cosine(angle)*number(human.item_homing_speed)).to_float();p->velocity.y=(sine(angle)*number(human.item_homing_speed)).to_float();
                p->state=1;move(*p,timing.rate);common_motion=false;
            }else{p->velocity.y=-.7f;p->state=0;}
        }else{p->velocity.x=p->velocity.z=0;if(p->velocity.y<-2.2f)p->velocity.y=-2.2f;}
        if(common_motion){
            move(*p,falling);
            const auto limit=number(context.height)+number(16);if(p->state==0&&(limit<number(p->position.y)||limit==number(p->position.y))){actions.subtract_rank(3);pool.remove(*p);continue;}
            if(!(p->velocity.y<3))p->velocity.y=3;else p->velocity.y=(number(.03f)*number(falling)+number(p->velocity.y)).to_float();
        }
        if(can_collect&&p->state!=3&&actions.touching(p->position,size)){
            context.replay_flags|=0x40;actions.collect(*p);if(sound<22)sound=p->max_value?44:21;pool.remove(*p);
        }else{p->timer.tick(timing);if(p->animation.currentInstruction)actions.animation_step(p->animation);}
    }
    if(sound)actions.sound(sound,0);
    if(context.gauge_lock.current!=0){context.gauge_lock.decrement(1,timing);if(context.gauge_lock.current<=0)context.gauge_lock.set(0);}
}
}
