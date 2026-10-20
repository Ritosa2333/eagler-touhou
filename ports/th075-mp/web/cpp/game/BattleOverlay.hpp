#pragma once
#include "ImageRenderer.hpp"
namespace th075 {
class BattleOverlay {
public:
    i32 timer=0,frame=0;u8 kind=0;
    float horizontal=430,vertical=350,label=300,secondaryLabel=-300,pulse=0,pulseVelocity=0,ring=0;
    void begin(bool knockout){kind=knockout?1:0;timer=150;}
    void advance();void render(ImageRenderer&);
};
}
