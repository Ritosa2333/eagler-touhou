#pragma once
#include "rounded_panel.hpp"
namespace th20::source::effects {
class WaveringTrail final:public AttachedCallback { //4672e0 / vtable56f288
public:
    sprite::Vec2 positions[30]; //08
    std::uint32_t colors[30]; //f8
    float angle,amplitude; //170,174
    recovered::Timer age; //178
    std::int32_t view_index; //188
    sprite::Vec2 extent,current; //18c,194
    explicit WaveringTrail(sprite::Animation&);
    int initialize(const Parameters&,int); //467990
    std::int32_t update() override; //467430 (inside Ghidra's 4673f0 range)
    void draw() override; //467820
    void retire() override {} //412540
    void interrupt(std::int32_t) override {} //414b50
};
static_assert(sizeof(WaveringTrail)==0x19c&&offsetof(WaveringTrail,age)==0x178&&offsetof(WaveringTrail,extent)==0x18c);
}
