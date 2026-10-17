#pragma once
#include "rounded_panel.hpp"
namespace th20::source::effects {
class TransitionPanels final:public AttachedCallback { //4646e0 /vtable56f098
public:
    sprite::Animation panels[4]; //08
    sprite::Animation mask; //1798
    std::int32_t mode,frames,closing; //1d7c,1d80,1d84
    explicit TransitionPanels(sprite::Animation&);
    ~TransitionPanels() override; //464790
    int initialize(const Parameters&,int); //4657d0 RET8
    std::int32_t update() override; //464840
    void draw() override; //4648e0
    void retire() override {} //412540
    void interrupt(std::int32_t) override; //4659a0
};
static_assert(sizeof(TransitionPanels)==0x1d88&&offsetof(TransitionPanels,mask)==0x1798&&offsetof(TransitionPanels,mode)==0x1d7c);
}
