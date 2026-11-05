#include "EnemyCallbacks.hpp"
namespace th15 {
void inherit_nearby_bullet_motion(BulletManager& bullets)noexcept{
    for(u32 outer=bullets.first_active();outer!=BulletManager::none;outer=bullets.next_active(outer)){
        const auto& source=*bullets.state(outer);if(source.step_limit!=1)continue;
        for(u32 inner=bullets.first_active();inner!=BulletManager::none;inner=bullets.next_active(inner)){
            auto& target=*bullets.state(inner);if(target.step_limit!=0)continue;
            const float dx=float(target.motion.position.x-source.motion.position.x),dy=float(target.motion.position.y-source.motion.position.y);
            const float distance=float(float(dx*dx)+float(dy*dy));if(!(400.f>=distance))continue;
            target.step_limit=2;target.motion.velocity=source.motion.velocity;target.motion.speed=source.motion.speed;target.motion.angle=source.motion.angle;target.transform_index=8;
        }
    }
}
void scale_bullets_near_player(BulletManager& bullets,const PlayerCollision& player,float factor,bool dialogue)noexcept{
    for(u32 index=bullets.first_active();index!=BulletManager::none;index=bullets.next_active(index)){
        auto& b=*bullets.state(index);if(b.step_limit!=1)continue;
        const float dx=float(player.position.x-b.motion.position.x),dy=float(player.position.y-b.motion.position.y),distance=float(float(dx*dx)+float(dy*dy));
        float radius=player.radius;if(player.enlarged)radius=float(float(player.size_multiplier*3.6f)*radius);
        const float hitbox_squared=float(b.hitbox.x*b.hitbox.x),inner=float(float(radius*radius)+hitbox_squared);
        if(!(distance>=inner)){if(dialogue)continue;}
        else{float padding=float(b.hitbox.x/2.5f);if(40.f>padding)padding=40.f;const float sum=float(padding+radius),threshold=float(float(sum*sum)+hitbox_squared);if(distance>=threshold)continue;}
        b.motion.velocity.x=float(b.motion.velocity.x*factor);b.motion.velocity.y=float(b.motion.velocity.y*factor);b.motion.velocity.z=float(b.motion.velocity.z*factor);
    }
}
bool cancel_unmarked_bullets(BulletScene& bullets,const Vec3& position,float radius){const float radius_squared=float(radius*radius);for(u32 index=bullets.manager.first_active();index!=BulletManager::none;){const u32 next=bullets.manager.next_active(index);auto& b=*bullets.manager.state(index);const float dx=float(position.x-b.motion.position.x),dy=float(position.y-b.motion.position.y),distance=float(float(dx*dx)+float(dy*dy));if(b.step_limit==0&&radius_squared>distance){b.cancellation_script=-1;if(!bullets.cancel_slot(index,0))return false;b.motion.velocity.x=float(b.motion.velocity.x*0.f);b.motion.velocity.y=float(b.motion.velocity.y*0.f);b.motion.velocity.z=float(b.motion.velocity.z*0.f);}index=next;}return true;}
bool enemy_update_rule(EnemyState& enemy,BulletScene& bullets,LaserScene& lasers,const PlayerCollision& player,bool dialogue){
    switch(enemy.update_rule){
        case EnemyUpdateRule::none:return true;
        case EnemyUpdateRule::inherit_nearby_motion:inherit_nearby_bullet_motion(bullets.manager);return true;
        case EnemyUpdateRule::cancel_unmarked:return cancel_unmarked_bullets(bullets,enemy.motion.position,enemy.float_registers[0]);
        case EnemyUpdateRule::offset_lasers:lasers.offset_origins(enemy.float_registers[0],enemy.float_registers[1]);return true;
        case EnemyUpdateRule::scale_near_player:scale_bullets_near_player(bullets.manager,player,enemy.float_registers[0],dialogue);return true;
    }return false;
}
bool enemy_additional_damage(EnemyState& enemy,i32 incoming,i32& result)noexcept{
    switch(enemy.damage_rule){case EnemyDamageRule::none:result=0;return true;case EnemyDamageRule::one_shot_register:result=incoming;if(enemy.integer_registers[3]>0){result=wrapping_add(result,enemy.integer_registers[3]);enemy.integer_registers[3]=0;}return true;case EnemyDamageRule::animation_regions:return false;}return false;
}
}
