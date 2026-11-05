#pragma once
#include "owner.hpp"
#include <array>
namespace th20::source::damage {struct Region;}
namespace th20::source::player_entity {
#pragma pack(push,1)
struct ShotRecord { // SHT record, stride0x78. Offsets used by504d40/504e90.
    std::int8_t period,phase;
    std::int16_t damage;
    sprite::Vec2 offset,size;
    float angle,field_18,speed;
    std::uint32_t fields_20[2];
    std::int8_t source,type;
    std::int16_t animation,sound;
    std::uint16_t field_2e,field_30;
    std::int8_t secondary_period,secondary_phase;
    std::uint32_t field_34;
    std::int32_t group;
    std::uint32_t fields_3c[3],callbacks[4],fields_58[8];
};
#pragma pack(pop)
static_assert(sizeof(ShotRecord)==0x78&&offsetof(ShotRecord,source)==0x28&&offsetof(ShotRecord,callbacks)==0x48);
using ShotInitializeCallback=int(__thiscall*)(Shot*,int);
// The final field is the 4c hit callback consumed by DamageRegion, not a destructor.
struct ShotCallbacks {std::uintptr_t initialize,update,extra,hit;};
class FiringServices {
public:
    virtual ~FiringServices()=default;
    virtual game_session::Session& session()=0;
    virtual float clock_rate()=0;
    virtual float signed_random()=0;
    virtual Shot* create_heap_shot()=0;                       //5041b0
    virtual ShotCallbacks callbacks(const ShotRecord&)=0;   //573178/573200/5731fc/573248
    virtual std::uint32_t spawn_animation(sprite::AnimationFile&,int)=0;
    virtual sprite::Animation& animation(std::uint32_t&)=0;
    virtual std::uint32_t create_damage(game_session::Context&,const Shot&)=0;
    virtual damage::Region* damage(std::uint32_t&)=0;         //4c0e10 always context0
    virtual void retire(Shot&)=0;
    virtual void sound_at(int,float)=0;
};
Option& shot_option(Player&,int) noexcept;                   //5060a0
Fixed2& shot_option_position(Player&,int) noexcept;           //5062f0
float shot_option_angle(Player&,int) noexcept;               //506310
const ShotRecord& shot_record(const Player&,std::uint32_t) noexcept; //506480
void advance_shot_handle(ShotController&) noexcept;           //505fc0
Shot* allocate_shot(ShotController&,FiringServices&);          //506210
int initialize_shot(Shot&,std::uint32_t,int,const sprite::Vec3&,Option*,int,FiringServices&); //504e90
int create_shot(ShotController&,std::uint32_t,int,const sprite::Vec3&,Option*,FiringServices&); //505d90
int fire_shots(ShotController&,int frame,int secondary_frame,int pattern,FiringServices&); //504d40
FiringServices& firing_services();
inline int fire_shots(ShotController& owner,int frame,int secondary_frame,int pattern){return fire_shots(owner,frame,secondary_frame,pattern,firing_services());}
namespace unrecovered {
// Required source callback table; no original function pointer is accepted.
ShotCallbacks shot_callbacks(const ShotRecord&);
}
}
