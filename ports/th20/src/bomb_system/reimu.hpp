#pragma once
#include "bomb.hpp"
namespace th20::source::bomb {
struct ReimuOrb {
    std::uint32_t animation_handle;
    Motion motion;
    sprite::Interpolation<sprite::Vec3> interpolation;
    std::uint32_t active;
    recovered::Timer timer;
    sprite::Vec3 last_delta;
    std::uint32_t target_identifier;
    void* target;
    std::int32_t ordinal;
    std::uint32_t damage_handle,quiet_retirement;
    float turn_rate;
};
static_assert(sizeof(ReimuOrb)==0xd8&&offsetof(ReimuOrb,active)==0xa0&&offsetof(ReimuOrb,damage_handle)==0xcc);
class ReimuBomb final:public Bomb {
public:
    ReimuOrb orbs[24];
    ReimuBomb();                                            //479280/4792f0/4791d0
    ~ReimuBomb() override=default;                           //479320->4785c0
    int start(std::int32_t) override;                        //479fd0
    int update() override;                                  //479860
    int draw() override {return 1;}                          //478bf0
    int finish() override;                                  //47a1b0, deletes this
};
static_assert(sizeof(ReimuBomb)==0x14f8&&offsetof(ReimuBomb,orbs)==0xb8);
ReimuBomb* create_reimu_bomb();                              //479140/47a610
void initialize_reimu_orb(ReimuOrb&,std::int32_t,const sprite::Vec3&,std::int32_t damage); //47a090
void update_reimu_orb(ReimuOrb&);                            //479360
void retire_reimu_orb(ReimuOrb&);                            //479e10
void retire_reimu_orbs(ReimuOrb (&)[24]);                     //479f90
}
