#include "EnemyContact.hpp"
namespace th08 {
namespace {
Vec3 divide(const Vec3& value,float divisor){const float inverse=Scalar::div(1,divisor);return {Scalar::mul(value.x,inverse),Scalar::mul(value.y,inverse),Scalar::mul(value.z,inverse)};}
}
void enemy_contact(EclVm& enemy,u8 character,const Vec3& position,const Vec3& size,EnemyContactActions& actions){
    const Vec3 graze_size=divide(size,.7f);
    if((enemy.flags&0x80)&&enemy.lifetime.changed()&&enemy.lifetime.current%6==0)actions.graze(position,graze_size);
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if((character==0||character==4)&&enemy.parent)return;
#else
    (void)character;
#endif
    const Vec3 hit_size=divide(size,1.5f);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const i32 result=actions.hit(position,hit_size,enemy.parent!=nullptr);
#else
    const i32 result=actions.hit(position,hit_size);
#endif
    if(result==1&&!(enemy.flags&2)&&!(enemy.flags&0x80))enemy.life=wrapping_sub(enemy.life,10);
}
void damage_familiar_parent(EclVm& enemy,i32 damage,bool bomb){
    auto* parent=enemy.parent;if(!parent||bomb)return;
    i32 minimum=0;for(const i32 threshold:parent->life_thresholds)if(minimum<threshold)minimum=threshold;
    i32 amount=damage/2;if(parent->damage_protection.current>0)amount=(parent->flags&2)?amount/9:0;
    if(amount){parent->life=wrapping_sub(parent->life,amount);if(parent->life<=minimum)parent->life=minimum;}
}
bool contact_enemy_and_trail(EclVm& enemy,u8 character,EnemyContactActions& actions){
    if(!(enemy.flags&4))return true;
    enemy_contact(enemy,character,enemy.resolved_position,enemy.hitbox,actions);
    if(!enemy.trail.flags)return true;
    if(enemy.trail.collision_length>96){enemy.invalid=true;return false;}
    Vec3 size=enemy.hitbox;
    for(i32 i=1;i<enemy.trail.collision_length;i+=6){
        if(enemy.trail.flags&2){
            // Three vector operations, each storing single-precision results.
            const float index=Extended::from_int(i).to_float();
            const Vec3 scaled{Scalar::mul(enemy.hitbox.x,index),Scalar::mul(enemy.hitbox.y,index),Scalar::mul(enemy.hitbox.z,index)};
            const Vec3 shrink=divide(scaled,Extended::from_int(enemy.trail.collision_length).to_float());
            size={Scalar::sub(enemy.hitbox.x,shrink.x),Scalar::sub(enemy.hitbox.y,shrink.y),Scalar::sub(enemy.hitbox.z,shrink.z)};
        }
        enemy_contact(enemy,character,enemy.trail.points[i].position,size,actions);
    }
    return !enemy.invalid;
}
}
