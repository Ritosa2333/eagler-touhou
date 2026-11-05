#pragma once
#include "enemy_opcode.hpp"
#include "enemy_cleanup.hpp"
#include "../hud_system/hud.hpp"
namespace th20::source::gameplay {
// Boundary calls have source implementations in enemy_opcode_state_adapter.cpp.
// Isolated CPU fixtures can record external effects without inventing gameplay.
class EnemyStateOpcodeServices {
public:
    virtual ~EnemyStateOpcodeServices()=default;
    virtual game_session::Session& session()=0;
    virtual GameController& game()=0;
    virtual hud::FrontInf& hud()=0;
    virtual void visibility(std::uint32_t,bool)=0;
    virtual void drop(EnemyPatternState&,const sprite::Vec3&,bool)=0;
    virtual void sound(int,float)=0;
    virtual void shake(int,int,int)=0;
    virtual void dialogue(int)=0;
    virtual void cancel_bullets(game_session::Context&)=0;
    virtual void erase_lasers(game_session::Context&,bool reset)=0;
    virtual void clear(EnemyController&,EnemyClearKind,int)=0;
    virtual void card_start(game_session::Context&,int,const char*,int,int)=0;
    virtual void card_finish(game_session::Context&)=0;
    virtual void delete_animation(std::uint32_t&)=0;
    virtual void clock_scale(float)=0;
    virtual void stage_title()=0;
    virtual void fog(int,int,std::uint32_t,float,float)=0;
    virtual void death_effect(EnemyController&,unsigned file,int script,const sprite::Vec3&,float)=0;
    virtual int defeat(Enemy&)=0;
    virtual Enemy* selected(EnemyController&,unsigned)=0;
};
void add_enemy_reward_total(game_session::Player&,int) noexcept; //497cc0, Player+dc
void configure_enemy_phase(EnemyState&,unsigned,int,int,const char*); //4ab650
void configure_enemy_timeout(EnemyState&,unsigned,const char*); //4ab780
EnemyOpcodeResult execute_enemy_state_opcode(EnemyOpcodeReader&,EnemyStateOpcodeServices&);
EnemyStateOpcodeServices& enemy_state_opcode_services();
}
