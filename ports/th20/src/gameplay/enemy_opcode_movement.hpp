#pragma once
#include "enemy_opcode.hpp"
namespace th20::source::gameplay {
class EnemyMovementOpcodeServices {
public:
    virtual ~EnemyMovementOpcodeServices()=default;
    virtual float clock_scale()=0;
    virtual float random_angle()=0; //4297d0, actual shared stream0
    virtual std::uint32_t random_integer()=0; //497470
    virtual sprite::Vec3 player_position(game_session::Context&)=0; //40ff90/460850
    virtual Enemy* selected(EnemyController&,unsigned)=0;
    virtual Enemy* find(EnemyController&,unsigned)=0;
};
EnemyMovementOpcodeServices& enemy_movement_opcode_services();
EnemyOpcodeResult execute_enemy_movement_opcode(EnemyOpcodeReader&,EnemyMovementOpcodeServices&);
}
