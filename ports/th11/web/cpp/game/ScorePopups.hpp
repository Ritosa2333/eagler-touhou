#pragma once
#include "AnmRenderer.hpp"
#include <array>

namespace th11 {
struct ScorePopup {
    u8 digits[12]{};Vec3 position;u32 color=0;
    Timer age{0,0,0,nullptr,0};u32 reserved30=0,reserved34=0;u8 active=0,length=0,reserved3a[6]{};
};
static_assert(sizeof(ScorePopup)==0x40);
struct ScoreGlyph {i32 sprite;float x,y;u32 color;};
// Original score effects 438440/438020/4380d0. Ten rotating slots;
// the native update and draw iterate all thirteen reserved slots.
class ScorePopups {
public:
    std::array<ScorePopup,13> entries{};u32 cursor=0;
    void add(Vec3,i32,u32,const float* rate);
    void update(float rate);
    std::vector<ScoreGlyph> glyphs(Vec3 player)const;
    bool draw(AnmRenderer&,AnmResource&,Vec3 player);
};
}
