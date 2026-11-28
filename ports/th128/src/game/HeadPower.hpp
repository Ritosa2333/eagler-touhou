#pragma once
#include <cstdint>
#include <optional>
#include <vector>
namespace th128::hud {
struct HeadGlyph {int sprite=0;float x=0,y=0;};
struct HeadPower {int percent=0;std::uint32_t color=0;float scale=1,x=0,y=0;std::vector<HeadGlyph> glyphs;};
std::optional<HeadPower> headPower(int power,int held,float x,float y);
}
