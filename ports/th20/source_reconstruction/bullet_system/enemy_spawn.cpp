#include "command.hpp"
#include "../gameplay/enemy_spawn.hpp"
namespace th20::source::bullet::unrecovered {
void spawn_extended_enemy(Bullet& bullet,const Command& command){ //47dcf0 case24
    gameplay::SpawnParameters p;gameplay::construct_spawn_parameters(p);p.position=bullet.position;p.health=10000;
    for(unsigned i=0;i<4;++i){p.variables[i]=command.words[i+4];p.variables[i+4]=command.words[i];}
    gameplay::spawn_enemy(*static_cast<gameplay::EnemyController*>(bullet.context->objects_04[1]),reinterpret_cast<const char*>(command.words[10]),p,nullptr);
}
}
