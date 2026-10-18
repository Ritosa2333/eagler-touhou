#pragma once
#include "enemy_shot.hpp"
#include "../laser_system/laser.hpp"
namespace th20::source::gameplay {
class EnemyLaserOpcodeServices {
public:
    virtual ~EnemyLaserOpcodeServices()=default;
    virtual EnemyShotOpcodeServices& shots()=0;
    virtual void create(game_session::Context&,unsigned,const void*)=0;
    virtual laser::Laser* find(game_session::Context&,int)=0;
    virtual float animation_angle(std::uint32_t&)=0;
    virtual void cancel_rectangle(game_session::Context&,const sprite::Vec3&,const sprite::Vec3&,float)=0;
};
EnemyLaserOpcodeServices& enemy_laser_opcode_services();
EnemyOpcodeResult execute_enemy_laser_opcode(EnemyOpcodeReader&,EnemyLaserOpcodeServices&);
}
