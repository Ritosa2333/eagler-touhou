#pragma once
#include "state_layout.hpp"
namespace th20::source::player_entity {
class ShotServices {
public:
    virtual ~ShotServices()=default;
    virtual void delete_animation(std::uint32_t&)=0;
    virtual void retire_damage(std::uint32_t&)=0;
    virtual void release(Shot*)=0;
};
void recycle_shot(ShotController&,Shot&) noexcept;           //506ab0
void retire_shot(Shot&,ShotServices&);                       //506af0
ShotServices& shot_services();
inline void retire_shot(Shot& shot){retire_shot(shot,shot_services());}
}
