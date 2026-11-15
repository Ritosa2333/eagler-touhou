#pragma once
#include "EnemyState.hpp"
#include "Timer.hpp"
namespace th11 {
#if PORT_TITLE == 14
// Retail 414510 fields of one 0xd0 Reimu seal. The native movement's
// anchor is also its linear velocity; its z component survives detachment.
struct Th14ReimuOrb {
    Vec3 position{},anchor{},last_delta{};
    float speed=0,angle=0,radius=0,turn=0;
    u32 flags=0,index=0,animation=0,area=256;
    bool active=false;Timer timer{0,0,0,nullptr,0};Enemy* target=nullptr;
};
void th14_reimu_orb_tick(Th14ReimuOrb&,Vec3 player,EnemyLink*,bool manager_present,const float* rate)noexcept;
#endif
}
