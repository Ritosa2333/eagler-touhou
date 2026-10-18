#pragma once
#include "rounded_panel.hpp"
namespace th20::source::effects {
class ConvergingParticles final:public AttachedCallback { //45c1f0 / vtable56ec14
public:
    std::uint32_t handles[200]; //08
    sprite::Vec3 targets[200],tangents[200]; //328,c88
    std::int32_t stages[200]; //15e8
    sprite::Vec3 near_center,far_center,center; //1908,1914,1920
    std::uint32_t field_192c;
    recovered::Timer age; //1930
    explicit ConvergingParticles(sprite::Animation&);
    int initialize(const Parameters&,int); //45cfe0, both arguments unused, RET8
    std::int32_t update() override; //45c360
    void draw() override {} //412540, children render through their own ANMs
    void retire() override; //45ceb0
    void interrupt(std::int32_t) override; //45d060
};
static_assert(sizeof(ConvergingParticles)==0x1940&&offsetof(ConvergingParticles,age)==0x1930&&offsetof(ConvergingParticles,stages)==0x15e8);
}
