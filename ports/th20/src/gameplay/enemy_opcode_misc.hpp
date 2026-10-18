#pragma once
#include "enemy_opcode.hpp"
namespace th20::source::gameplay {
class EnemyMiscOpcodeServices {
public:
    virtual ~EnemyMiscOpcodeServices()=default;
    virtual Enemy* find(EnemyController&,unsigned)=0;
    virtual Enemy* selected(EnemyController&,unsigned)=0;
    virtual void select_script(Enemy&,const char*)=0;
    virtual void attach_stone(unsigned enemy,int color)=0;
};
EnemyMiscOpcodeServices& enemy_misc_opcode_services();
EnemyOpcodeResult execute_enemy_misc_opcode(EnemyOpcodeReader&,EnemyMiscOpcodeServices&);
}
