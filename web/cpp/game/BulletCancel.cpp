#include "BulletCancel.hpp"
#include "GameMath.hpp"
namespace th08 {
void cancel_projectiles_near(BulletManagerState& state,const Vec3& p,float radius,BulletCancelActions& actions){
    const float squared=Scalar::mul(radius,radius);
    for(u32 i=0;i<1536;++i){auto& bullet=state.bullets[i];if(!bullet.state||bullet.state==5)continue;
        const auto x=number(Scalar::sub(bullet.position.x,p.x)),y=number(Scalar::sub(bullet.position.y,p.y)),z=number(Scalar::sub(bullet.position.z,p.z));
        if(!(number(squared)<x*x+y*y+z*z)){actions.drop(bullet.position,6,1);std::memset(&bullet,0,sizeof(bullet));}
    }
}
bool cancel_projectiles(BulletManagerState& state,i32 mode,const i32& cancel_item,BulletCancelActions* actions){
    for(u32 i=0;i<1536;++i){auto& bullet=state.bullets[i];if(!bullet.state||bullet.state==5)continue;
        if(actions)actions->barrier(bullet);const i32 result=actions?actions->barrier(bullet):0;
        if(result==2){if(actions)actions->drop(bullet.position,cancel_item,1);std::memset(&bullet,0,sizeof(bullet));}
        else if(mode==4)bullet.state=5;
        else{if(actions)actions->drop(bullet.position,state.bonus_item,mode);std::memset(&bullet,0,sizeof(bullet));}
    }
    for(auto& laser:state.lasers){
        if(!laser.in_use||((laser.flags&4)&&mode!=4))continue;
        if(laser.state<2){
            laser.state=2;laser.timer.set(0);laser.width=laser.width2;
            if(mode!=4){
                float offset=laser.start_offset;const float x=cosine(laser.angle).to_float(),y=sine(laser.angle).to_float();
                while(offset<laser.end_offset){
                    const Vec3 position{(number(x)*number(offset)+number(laser.position.x)).to_float(),(number(y)*number(offset)+number(laser.position.y)).to_float(),0};
                    if(actions)actions->drop(position,state.bonus_item,mode);
                    const float next=Scalar::add(offset,32);if(next==offset)return false;offset=next;
                }
            }
        }
        laser.hitbox_stop=0;
    }
    state.cancel_frames=10;return true;
}
}
