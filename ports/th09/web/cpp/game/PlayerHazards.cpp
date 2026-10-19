#include "PlayerHazards.hpp"
namespace th09 {
void PlayerHazards::circle(const Vec3& p,float radius,Bullet* source){if(count>=capacity)return;auto& h=entries[count++];h.position=p;h.radius=radius;h.bullet=source;}
void PlayerHazards::box(const Vec3& p,const Vec3& extent,Bullet* source){if(count>=capacity)return;auto& h=entries[count++];h.position=p;h.half_extent={extent.x*.5f,extent.y*.5f,extent.z};h.radius=0;h.angle=0;h.bullet=source;}
void PlayerHazards::rotated_box(const Vec3& p,const Vec3& extent,const Vec3& pivot,float angle,Bullet* source){if(count>=capacity)return;auto& h=entries[count++];h.position=p;h.pivot=pivot;h.half_extent={extent.x*.5f,extent.y*.5f,extent.z};h.radius=0;h.angle=angle;h.bullet=source;}
PlayerHazard* PlayerHazards::collision(HazardPlayer& player,float additional){
    const auto& p=player.position;const auto& size=player.half_extent;player.hit_bounds={{p.x-size.x,p.y-size.y,p.z-size.z},{p.x+size.x,p.y+size.y,p.z+size.z}};
    for(u32 i=0;i<count;++i){auto& h=entries[i];
        if(h.radius!=0){if(intersects_circle({p.x,p.y},player.hit_radius,{h.position.x,h.position.y},h.radius+additional))return &h;}
        else if(h.angle!=0){if(intersects_rotated_box({p.x,p.y},{size.x,size.y},{h.position.x,h.position.y},{h.half_extent.x,h.half_extent.y},{h.pivot.x,h.pivot.y},h.angle,false))return &h;}
        else if(intersects_box(player.hit_bounds,{h.position.x,h.position.y},{h.half_extent.x,h.half_extent.y}))return &h;
    }return nullptr;
}
PlayerHazard* PlayerHazards::first_hit(HazardPlayer& player){if(player.state||player.invulnerability.current>0)return nullptr;return collision(player,0);}
PlayerHazard* PlayerHazards::sample(HazardPlayer& player,const Vec3& position,const Vec3& extent,float additional){
    if(player.state||player.invulnerability.current>0)return nullptr;const Vec3 old_position=player.position,old_extent=player.half_extent;player.position=position;player.half_extent=extent;
    auto result=collision(player,additional);player.position=old_position;player.half_extent=old_extent;return result;
}
}
