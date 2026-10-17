#pragma once
#include "../sprite_renderer/animation.hpp"
#include "../game_session/session.hpp"
namespace th20::source::player_entity {
// Non-owning access to the real PlayerInf (size1485c, Context+4). Full owner
// construction is recovered separately; these views allocate no replacement.
sprite::Vec3 position(const void*) noexcept;                  //460850 +614
float angle_to_player(const void*,const sprite::Vec3&);        //4ff350
bool focused(const void*) noexcept;                          //4ff5e0 +204c
bool expanded_collision(const void*) noexcept;               //4ff800 +14 bit4
std::int32_t collision_percent(game_session::Player&) noexcept; //4ff490 +e8 clamp0..100
class CollisionServices {
public:
    virtual ~CollisionServices()=default;
    virtual const void* boss_hud()=0;                         //nullable5c06a4
    virtual void hit(void* player)=0;                        //4f86f0
};
int collide_axis_aligned(void*,const sprite::Vec3&,const sprite::Vec2&,int preview,CollisionServices&); //4f8ce0
int collide_circle(void*,const sprite::Vec3&,float radius,int preview,CollisionServices&); //4f8ff0
int collide_rectangle(void*,const sprite::Vec3&,float angle,float width,float height,int preview,CollisionServices&); //4f91d0; original parameter order
CollisionServices& collision_services();
inline int collide_axis_aligned(void* player,const sprite::Vec3& p,const sprite::Vec2& size,int preview){return collide_axis_aligned(player,p,size,preview,collision_services());}
inline int collide_circle(void* player,const sprite::Vec3& p,float radius,int preview){return collide_circle(player,p,radius,preview,collision_services());}
inline int collide_rectangle(void* player,const sprite::Vec3& p,float angle,float width,float height,int preview){return collide_rectangle(player,p,angle,width,height,preview,collision_services());}
namespace unrecovered {void player_hit_004f86f0(void*);}
}
