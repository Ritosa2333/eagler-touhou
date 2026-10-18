#include "EnemyVisibility.hpp"
namespace th08 {
bool intersects_playfield(const Vec3& p,float width,float height)noexcept{
    const auto half_x=number(width)/number(2),half_y=number(height)/number(2);
    if(half_x+number(p.x)<number(0))return false;
    if(number(384)<number(p.x)-half_x)return false;
    if(half_y+number(p.y)<number(0))return false;
    if(number(448)<number(p.y)-half_y)return false;
    return true;
}
bool update_enemy_visibility(EclVm& enemy)noexcept{
    const auto* sprite=enemy.animation[0].loadedSprite;if(!sprite)enemy.flags|=0x10;
    if(!(enemy.flags&0x10)&&!(enemy.flags&0x1000000)&&intersects_playfield(enemy.resolved_position,sprite->widthPx,sprite->heightPx)){
        enemy.flags|=0x1000000;return true;
    }
    if(!(enemy.flags&0x1000000)||(enemy.flags&0x10000000))return true;
    if(!sprite){enemy.invalid=true;enemy.failure=EclVm::Failure::MissingAnimation;return false;}
    if(intersects_playfield(enemy.resolved_position,sprite->widthPx,sprite->heightPx))return true;
    if(enemy.trail.flags){
        if(enemy.trail.length<1||enemy.trail.length>96){enemy.invalid=true;return false;}
        if(intersects_playfield(enemy.trail.points[enemy.trail.length-1].position,sprite->widthPx,sprite->heightPx))return true;
    }
    enemy.flags&=~1u;return false;
}
}
