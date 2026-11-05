#include "CombatRuntime.hpp"
#include <cmath>

namespace th08 {
void CombatRuntime::reset() noexcept {frame=0;enemies.fill({});bullets.fill({});lasers.fill({});items.fill({});player=CombatPlayer{};stats=CombatStats{};}
CombatEnemy* CombatRuntime::spawn_enemy(u8 kind,const Vec3& position,float life) noexcept {for(auto& enemy:enemies)if(!enemy.active){enemy=CombatEnemy{};enemy.active=1;enemy.kind=kind;enemy.position=position;enemy.life=life;++stats.spawned_enemies;return &enemy;}return nullptr;}
CombatBullet* CombatRuntime::spawn_bullet(u8 kind,const Vec3& position,float angle,float speed,u8 color) noexcept {for(auto& bullet:bullets)if(!bullet.active){bullet=CombatBullet{};bullet.active=1;bullet.kind=kind;bullet.color=color;bullet.position=position;bullet.angle=angle;bullet.speed=speed;bullet.velocity={std::cos(angle)*speed,std::sin(angle)*speed,0};++stats.spawned_bullets;return &bullet;}return nullptr;}
CombatLaser* CombatRuntime::spawn_laser(u8 kind,const Vec3& position,float angle,float length,float width) noexcept {for(auto& laser:lasers)if(!laser.active){laser=CombatLaser{};laser.active=1;laser.kind=kind;laser.position=position;laser.angle=angle;laser.length=length;laser.width=width;++stats.spawned_lasers;return &laser;}return nullptr;}
CombatItemState* CombatRuntime::spawn_item(CombatItem type,const Vec3& position) noexcept {for(auto& item:items)if(!item.active){item=CombatItemState{};item.active=1;item.type=u8(type);item.position=position;++stats.spawned_items;return &item;}return nullptr;}
void CombatRuntime::cancel_bullets(float radius) noexcept {for(auto& bullet:bullets)if(bullet.active&&(radius<0||std::hypot(bullet.position.x-player.position.x,bullet.position.y-player.position.y)<=radius)){bullet.active=0;++stats.cancelled_bullets;}}
void CombatRuntime::cancel_lasers() noexcept {for(auto& laser:lasers)laser.active=0;}
void CombatRuntime::clear_enemies() noexcept {for(auto& enemy:enemies)enemy.active=0;}
void CombatRuntime::update(float rate) noexcept {
    ++frame;const float step=rate<=0?1:rate;
    for(auto& enemy:enemies)if(enemy.active){enemy.position.x+=enemy.velocity.x*step;enemy.position.y+=enemy.velocity.y*step;if(enemy.life<=0||enemy.position.x<Left-64||enemy.position.x>Right+64||enemy.position.y<Top-64||enemy.position.y>Bottom+64)enemy.active=0;}
    for(auto& bullet:bullets)if(bullet.active){bullet.position.x+=bullet.velocity.x*step;bullet.position.y+=bullet.velocity.y*step;const float dx=bullet.position.x-player.position.x,dy=bullet.position.y-player.position.y,d=std::sqrt(dx*dx+dy*dy);if(d<=player.hitbox+bullet.hitbox){++stats.hits;bullet.active=0;continue;}if(!bullet.graze&&d<=player.grazebox){bullet.graze=1;++stats.grazes;}if(bullet.position.x<Left-32||bullet.position.x>Right+32||bullet.position.y<Top-32||bullet.position.y>Bottom+32)bullet.active=0;}
    for(auto& laser:lasers)if(laser.active){laser.timer+=u32(step);if(laser.duration&&laser.timer>=laser.duration)laser.active=0;}
    for(auto& item:items)if(item.active){item.velocity.y+=0.08f*step;item.position.x+=item.velocity.x*step;item.position.y+=item.velocity.y*step;if(player.position.y<item.position.y&&std::hypot(item.position.x-player.position.x,item.position.y-player.position.y)<64)item.state=1;if(item.state==1){const float dx=player.position.x-item.position.x,dy=player.position.y-item.position.y,item_distance=std::sqrt(dx*dx+dy*dy);if(item_distance>0){item.velocity.x=dx/item_distance*6;item.velocity.y=dy/item_distance*6;}}if(std::hypot(item.position.x-player.position.x,item.position.y-player.position.y)<player.hitbox+4)item.active=0;if(item.position.y>Bottom+48)item.active=0;}
}
}
