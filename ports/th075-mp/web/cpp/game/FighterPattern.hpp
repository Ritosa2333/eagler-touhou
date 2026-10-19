#pragma once
#include "Types.hpp"
namespace th075 {
struct Rectangle { std::array<u32,4> bits{}; };
struct FrameVisual {
    u8 mode=0,alpha=0,red=0,green=0,blue=0;
    std::array<i16,3> projection{};
    std::array<float,3> scale{},rotation{};
};
struct ImageFrame {
    u16 sprite=0;i16 originX=0,originY=0;u16 duration=0;
    bool basicVisual=false,hasVisual=false;FrameVisual visual{};
};
bool read_image_frame(ByteReader&,ImageFrame&);
struct PatternFrame:ImageFrame {
    i16 baseDamage=0;float proration=0;
    std::array<i16,10> combat{};
    std::array<float,2> motion{};
    std::array<i16,3> events{};
    std::array<u32,2> flags{};
    Rectangle baseBounds{};
    std::vector<Rectangle> bodyBounds,attackBounds;
};
struct PatternSequence { bool present=false;u16 mode=0,next=0;std::vector<PatternFrame> frames; };
class FighterPattern {
public:
    std::vector<PatternSequence> sequences;std::string error;
    bool load(const void*,std::size_t);
};
}

