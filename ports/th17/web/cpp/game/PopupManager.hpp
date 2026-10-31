#pragma once
#include "AnmManager.hpp"
#include "HudDrawData.hpp"
namespace th17 {
struct PopupEntry {
    std::array<u8,12> digits{};
    Vec3 position{};
    float velocity=0;
    u32 color=0;
    Timer age{0,0,0,0,0};
    u32 reserved[2]{};
    u8 active=0,digit_count=0,padding[2]{};
    i32 bonus=0;
    float multiplier=0;
};
static_assert(sizeof(PopupEntry)==72);
struct PopupDrawServices {
    virtual ~PopupDrawServices()=default;
    virtual bool popup_glyph(AnmVm&)=0;
    virtual bool popup_text(const HudTextDraw&)=0;
};
// The original has ten rotating score/graze slots, three reserved ordinary
// slots and five bonus slots. Their lifetimes and drawing order are distinct.
class PopupManager {
    AnmManager& animations;
    AnmVm glyph;
    u32 next_number=0;
public:
    std::array<PopupEntry,18> entries{};
    std::string error;
    explicit PopupManager(AnmManager& manager):animations(manager){}
    bool initialize(i32 ascii_bank);
    void number(const Vec3&,i32 value,u32 color);
    void update(float rate);
    bool draw(PopupDrawServices&,const Vec3& player);
    u32 cursor()const noexcept{return next_number;}
};
}
