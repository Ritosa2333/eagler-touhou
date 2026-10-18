#pragma once
#include "rounded_panel.hpp"
namespace th20::source::effects {
struct SelectionPulse {std::uint8_t enabled,padding[3];float radius;SelectionPulse():enabled(0),radius(0){}};
struct SelectionButton {std::uint8_t pressed,padding[3];std::int32_t remaining;};
class StoneSelection final:public AttachedCallback { //461730 /vtable56f014
public:
    sprite::Vec2 basis[12],points[12]; //08,68
    std::uint32_t colors[12],draw_colors[12]; //c8,f8
    std::uint32_t handles[4],center_handle,background_handle; //128,138,13c
    std::uint32_t field_140;
    float radius; //144
    std::uint32_t color; //148
    recovered::Timer age,remaining; //14c,15c
    std::int32_t selection; //16c
    std::uint8_t expanded,padding_171[3];
    SelectionPulse pulses[4]; //174
    SelectionButton buttons[4]; //194
    explicit StoneSelection(sprite::Animation&);
    int initialize(const Parameters&,int); //462c80
    std::int32_t update() override; //4618c0
    void draw() override; //461b80
    void retire() override; //462c10
    void interrupt(std::int32_t) override; //463310
};
static_assert(sizeof(StoneSelection)==0x1b4&&offsetof(StoneSelection,handles)==0x128&&offsetof(StoneSelection,pulses)==0x174&&offsetof(StoneSelection,buttons)==0x194);
namespace stone_selection_environment {
bool updates_enabled(); //nullable5ba828 ->464350
bool button(unsigned); //464230->464270/464480/4642e0/4643b0
int stone(unsigned); //464080->4641d0
bool alternate(unsigned); //464080->464420
int selected_profile(unsigned); //464100 using current player's character
sprite::AnimationFile& file(); //51b960->411700
}
}
