#pragma once
#include "enemy_state.hpp"
namespace th20::source::gameplay {
class EnemyUpdateServices {
public:
    virtual ~EnemyUpdateServices()=default;
    virtual const float* timer_rate()=0;
    virtual float script_delta(const recovered::Timer&)=0;   //456240 indexed original clock
    virtual sprite::Animation* animation(std::uint32_t)=0;    //44cd00, no handle mutation
    virtual int move(EnemyState&)=0;                         //4a7710
    virtual int run_scripts(void* entity,float delta)=0;      //53e2b0 on real entity ECL manager
    virtual int damage(EnemyState&)=0;                       //4a5df0
    virtual void mesh(EnemyState&)=0;                        //4a4190
};
int advance_enemy_scripts(EnemyState&,EnemyUpdateServices&);  //4ab4c0
int update_enemy_state(EnemyState&,EnemyUpdateServices&);     //4a8260
EnemyUpdateServices& enemy_update_services();
namespace unrecovered {
int tick_enemy_scripts_0053e2b0(void* entity,float delta);
void update_enemy_mesh_004a4190(EnemyState&);
}
}
