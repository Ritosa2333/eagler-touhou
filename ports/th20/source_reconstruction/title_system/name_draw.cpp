#include "name_draw.hpp"
#include "name_data.hpp"
#include "rank_entry.hpp"
#include "../pause_system/menu_support.hpp"
#include <cstring>
namespace th20::source::title {
namespace n=recovered;namespace d=name_data;
void draw_entered_name(TitleInf& o,text::Renderer& r,sprite::Vec3 position){
    r.color=0xffffffff;r.write_ascii_format(position,"%s",entered_name(o));
    position.x=n::add32(position.x,n::int_float(n::signed_bits(o.word56e0*9u)));if(o.word56e0==8)position.x=n::add32(position.x,-9.f);
    r.color=0xffffff00;r.write_ascii_format(position,"_");r.color=0xffffffff;
}
void draw_name_keyboard(TitleInf& o,text::Renderer& r){
    const auto* characters=pause::name_characters();const int count=static_cast<int>(std::strlen(characters));sprite::Vec3 position{d::f_00575674,d::f_0056fe88,0};
    for(int i=0;i<count;++i){r.color=o.cursor56e8.selected(i)?0xffffff00:0xff808080;const int value=i<count-3?static_cast<std::int8_t>(characters[i]):i==count-3?0x81:i==count-2?0x7f:0x80;r.write_ascii_format(position,d::s_005753a4,value);
        if(i%13==12){position.x=n::add32(d::f_0056f10c,-108.f);position.y=n::add32(position.y,16.f);}else position.x=n::add32(position.x,18.f);
    }
    r.color=0xffffffff;
}
}
