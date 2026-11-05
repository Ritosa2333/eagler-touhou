#include "BulletScoreCancel.hpp"
#include "GameMath.hpp"
namespace th08 {
bool cancel_projectiles_for_score(BulletManagerState& state,i32 maximum,bool laser_items,const i32& cancel_item,BulletScoreCancelActions& actions,i32& score){
    score=0;i32 value=2000;
    for(u32 i=0;i<1536;++i){auto& bullet=state.bullets[i];if(!bullet.state)continue;
        const i32 barrier=actions.barrier(bullet);actions.drop(bullet.position,barrier==2?cancel_item:state.bonus_item,1);
        actions.score_popup(bullet.position,value,value<maximum?0xffffffffu:0xffffff00u);
        score=wrapping_add(score,value);value=wrapping_add(value,20);if(value>maximum)value=maximum;
        bullet.state=5;
    }
    for(auto& laser:state.lasers){if(!laser.in_use)continue;
        if(laser.state<2){
            laser.state=2;laser.timer.set(0);laser.width=laser.width2;
            if(laser_items){
                actions.drop(laser.position,state.bonus_item,1);
                float offset=laser.start_offset;const float x=cosine(laser.angle).to_float(),y=sine(laser.angle).to_float();
                while(offset<laser.end_offset){
                    const Vec3 position{(number(x)*number(offset)+number(laser.position.x)).to_float(),(number(y)*number(offset)+number(laser.position.y)).to_float(),0};
                    actions.drop(position,state.bonus_item,1);const float next=Scalar::add(offset,32);if(next==offset)return false;offset=next;
                }
            }
        }
        laser.hitbox_stop=0;
    }
    state.cancel_frames=10;return true;
}
}
