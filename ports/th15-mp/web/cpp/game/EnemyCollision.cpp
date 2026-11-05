#include "EnemyCollision.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
DamageQuery enemy_shot_query(const EnemyState& enemy,bool preview)noexcept{
    DamageQuery query;query.position=enemy.motion.position;query.target=signed_bits(enemy.id);query.preview=preview;
    query.rectangle=enemy.flags&0x1000;
    if(query.rectangle){query.size=enemy.hitbox;query.angle=enemy.rotation_angle;}
    else query.radius=float(enemy.hitbox.x*.5f);
    return query;
}
EnemyContactShape enemy_contact_shape(const EnemyState& enemy,const AnmVm* animation)noexcept{
    EnemyContactShape shape;shape.position=enemy.motion.position;shape.rectangle=enemy.flags&0x1000;
    if(!shape.rectangle){shape.radius=float(enemy.hurtbox.x*.5f);return shape;}
    shape.angle=enemy.rotation_angle;shape.length=enemy.hurtbox.y;shape.width=enemy.hurtbox.x;
    const float half=float(shape.length*.5f);float x=0,y=half;
    if(animation){
        const float angle=normalize_angle(float(animation->variables.rotation.z+1.57079637050628662109375f));
        const float sine=float(std::sin(double(angle))),cosine=float(std::cos(double(angle)));
        x=float(float(cosine*0.f)-float(sine*half));
        y=float(float(cosine*half)+float(sine*0.f));
    }
    shape.position.x=float(shape.position.x+x);shape.position.y=float(shape.position.y+y);shape.position.z=float(shape.position.z+0.f);
    return shape;
}
PlayerContact enemy_player_contact(const EnemyState& enemy,const AnmVm* animation,const PlayerCollision& player,PlayerDamageHost* damage){
    const auto shape=enemy_contact_shape(enemy,animation);const Vec2 position={shape.position.x,shape.position.y};
    return shape.rectangle?player.laser(position,shape.angle,shape.length,shape.width,false,damage):player.circle(position,shape.radius,false,damage);
}
}
