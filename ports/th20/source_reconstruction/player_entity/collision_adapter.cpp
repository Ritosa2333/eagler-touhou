#include "player.hpp"
#include "../gameplay/enemy_frame.hpp"
namespace th20::source::player_entity {
namespace {
class GameCollisionServices final:public CollisionServices {
public:
    const void* boss_hud() override{return gameplay::unrecovered::boss_hud_005c06a4();}
    void hit(void* player) override{unrecovered::player_hit_004f86f0(player);}
};
}
CollisionServices& collision_services(){static GameCollisionServices host;return host;}
}
namespace th20::source::gameplay::unrecovered {
int player_circle_004f8ff0(void* player,const sprite::Vec3& p,float radius,int preview){return player_entity::collide_circle(player,p,radius,preview);}
int player_rectangle_004f91d0(void* player,const sprite::Vec3& p,float angle,float width,float height,int preview){return player_entity::collide_rectangle(player,p,angle,width,height,preview);}
}
