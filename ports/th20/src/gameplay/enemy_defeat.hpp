#pragma once
#include "enemy_entity.hpp"
#include "../player_entity/state_layout.hpp"
namespace th20::source::gameplay {
class EnemyDefeatServices {
public:
    virtual ~EnemyDefeatServices()=default;
    virtual game_session::Session& session()=0;
    virtual const float* timer_rate()=0;
    virtual void sound(int,float)=0;
    virtual void effect(Enemy&,unsigned file,int script,const sprite::Vec3&,float)=0;
    virtual void drop(EnemyPatternState&,const sprite::Vec3&,bool)=0;
    virtual bool special_active()=0;
    virtual unsigned random()=0;
    virtual void special_items(game_session::Player&,int)=0;
    virtual void meter(game_session::Player&,int)=0;
    virtual void counter(game_session::Player&,unsigned stone,int)=0;
    virtual void reward(runtime::CallbackOwner*,const sprite::Vec3&,int,int)=0;
    virtual void death_script(Enemy&,int)=0; //clear/reset/select index/tick0
};
int defeat_enemy(Enemy&,EnemyDefeatServices&); //4a5640, EAX=1 except revived bit27 path=0
void add_defeat_combo(player_entity::Feedback&,int,const float*); //4fb3a0
EnemyDefeatServices& enemy_defeat_services();
inline int defeat_enemy(Enemy& e){return defeat_enemy(e,enemy_defeat_services());}
}
