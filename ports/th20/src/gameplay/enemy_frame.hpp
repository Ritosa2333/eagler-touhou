#pragma once
#include "enemy.hpp"
namespace th20::source::gameplay {
// Non-owning views below refer to real, still partly recovered entity objects.
// They do not invent complete entity layouts or allocate substitute storage.
struct EnemyAnimationLink {std::uint32_t handle=0;float offset[3]{};std::int32_t parent=-1;}; //48b3d0
static_assert(sizeof(EnemyAnimationLink)==20);
class EnemyFrameServices {
public:
    virtual ~EnemyFrameServices()=default;
    virtual const float* timer_rate()=0;                      //5aefe0
    virtual float& clock_scale()=0;                          //5aefe4
    virtual void update_boss_time(std::int32_t seconds,std::int32_t hundredths)=0; //4ab5b0, HUD5c06a4
    virtual int update_entity_state(void* state_88)=0;         //4a8260
    virtual void update_special_objects()=0;                 //513dd0->5120d0
    virtual sprite::Animation* animation(std::uint32_t)=0;    //44ced0
};
// Entire original4a5040 control flow; object behavior below4a8260 remains an
// explicit required service, as does the special-object manager5120d0.
int update_enemy_controller(EnemyController&,EnemyFrameServices&);
int update_enemy_with_time_scale(void* entity,EnemyFrameServices&); //4a8760
void store_boss_time(void* hud,std::int32_t,std::int32_t) noexcept; //4ab5b0
float primary_entity_scale(const void*) noexcept;             //4aaa40 +223c
void set_primary_entity_scale(void*,float) noexcept;           //4ab590
void set_primary_entity_flag(void*,std::uint32_t) noexcept;    //4ab850 +14 bit5
EnemyFrameServices& enemy_frame_services();
namespace unrecovered {
void* boss_hud_005c06a4();                                    //actual HUD owner, no fabricated global
void update_special_objects_005120d0();
}
}
