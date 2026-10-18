#pragma once
#include "bomb.hpp"
namespace th20::source::bomb {
// Name is retained in the original diagnostic string56fb30.
class MarisaBomb final:public Bomb {
public:
    sprite::Vec3 position;
    float angle;
    std::uint32_t beam_handle;
    MarisaBomb();                                            //4784e0
    ~MarisaBomb() override=default;                          //4785c0 invokes only base destructor
    int start(std::int32_t) override;                        //478c00 ignores its stack argument
    int update() override;                                  //478680
    int draw() override {return 1;}                          //478bf0
    int finish() override;                                  //478e40, deletes this
};
static_assert(sizeof(MarisaBomb)==0xcc&&offsetof(MarisaBomb,position)==0xb8);
MarisaBomb* create_marisa_bomb();                             //4783a0/479120
int __cdecl update_marisa_animation(sprite::Animation*);     //478380 ->478610
}
