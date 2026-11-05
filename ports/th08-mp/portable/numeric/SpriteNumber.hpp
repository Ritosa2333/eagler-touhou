#pragma once
#include <cmath>
#include <initializer_list>
namespace touhou::numeric {
// Only selected for 24-bit nearest-even geometry, with bounded operands.
// Separate f32 operations retain original rounding order (-ffp-contract=off).
// Other arithmetic modes and unusual scales use the original Extended path.
struct SpriteNumber {
    float value;
    SpriteNumber():value(0){}
    explicit SpriteNumber(float v):value(v){}
    static SpriteNumber from_double(double v){return SpriteNumber(float(v));}
    float to_float()const{return value;}
    double to_double()const{return value;}
    SpriteNumber operator-()const{return SpriteNumber(-value);}
    friend SpriteNumber operator+(SpriteNumber a,SpriteNumber b){return SpriteNumber(a.value+b.value);}
    friend SpriteNumber operator-(SpriteNumber a,SpriteNumber b){return SpriteNumber(a.value-b.value);}
    friend SpriteNumber operator*(SpriteNumber a,SpriteNumber b){return SpriteNumber(a.value*b.value);}
    friend SpriteNumber operator/(SpriteNumber a,SpriteNumber b){return SpriteNumber(a.value/b.value);}
};
inline bool sprite_range(std::initializer_list<float> values){
    for(float v:values){const float a=std::fabs(v);if(a!=0&&!(a>=0x1p-20f&&a<=0x1p20f))return false;}
    return true;
}
}
