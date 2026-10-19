#include "BattleHud.hpp"
#include <cmath>
namespace th075 {
namespace {
struct DigitAtlas {i16 width,height,advance;u8 image;CollisionRectangle glyphs[10];};
constexpr std::array<DigitAtlas,14> digits{{
#include "HudDigits.inc"
}};
}
void BattleHud::number(ImageRenderer& images,float x,float y,u32 color,i32 value,u8 style,i32 count){
    if(value<0||style>=digits.size())return;const auto& font=digits[style];
    auto quad=rectangle_quad(float(double(x)-double(font.width)),y,x,float(double(font.height)+double(y)));
    i32 remainder=value;
    for(;;){images.quad(font.image,quad,font.glyphs[u32(remainder%10)],color);
        if(count>0){remainder/=10;if(--count==0)break;}
        else{if(remainder<10)break;remainder/=10;}
        translate_quad(quad,float(-i32(font.advance)),0,0);
    }
}
void ComboCounter::update(i16 hitCount,i16 totalDamage,i16 scale){
    if(hits<hitCount||hitCount==1){alpha=255;hits=hitCount;damage=totalDamage;rate=scale;wait=60;
        if(hits==2){slideAngle=90;const float angle=float(double(slideAngle)*double(0.017453289f));slide=float(-std::sin(double(angle))*200.0);}
    }
}
void BattleHud::render_combo(ImageRenderer& images,ComboCounter& counter){
    if(!counter.alpha||counter.hits<2)return;
    const i16 hits=counter.hits>99?99:counter.hits;const u32 color=u32(counter.alpha)<<24|0xffffff;
    const double x=counter.facing<1?-double(counter.slide)+494.0:double(counter.slide)+10.0;
    images.quad(0,rectangle_quad(float(x),132,float(x+136.0),202),{640,54,776,124},color);
    number(images,float(x+65.0),116,color,hits,0);number(images,float(x+60.0),169,color,counter.damage,1);number(images,float(x+67.0),187,color,counter.rate,1);
    if(counter.slideAngle<1){counter.wait=i16(u16(counter.wait)-1);if(counter.wait<0){counter.alpha=u8(counter.alpha-25);if(counter.alpha<6)counter.alpha=0;}}
    else{counter.slideAngle=i16(u16(counter.slideAngle)-10);const float angle=float(double(counter.slideAngle)*double(0.017453289f));counter.slide=float(-std::sin(double(angle))*200.0);}
}
void BattleHud::render_bonus_banner(ImageRenderer& images){
    const u8 alpha=bannerTimer<51?u8(bannerTimer*5):255;const u32 color=u32(alpha)<<24|0xffffff;
    images.quad(9,rectangle_quad(64,120,576,162),{0,0,512,42},color);
    // The original asks its decimal logarithm helper for the right alignment.
    const float input=float(bonusNumber);const i8 places=i8(i32(std::log10(double(input))));
    const u8 alignment=u8(places+1);number(images,float(u32(alignment)*19+320),150,color,bonusNumber,13);
}
}
