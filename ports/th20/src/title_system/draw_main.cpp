#include "title.hpp"
#include "data.hpp"
#include "../text_renderer/menu_style.hpp"
namespace th20::source::title {
void draw_main_menu(TitleInf& o,text::Renderer& r){
    if(o.phase!=2&&o.phase!=3&&o.phase!=4)return;
    sprite::Vec3 p{data::f_00575670,data::f_00573fdc,0};r.fields_1a1d4[3]=7;r.fields_1a1d4[8]=0;r.fields_1a1d4[9]=1;r.fields_1a1d4[2]=0;
    for(int i=0;i<10;++i){auto saved=p;text::style_menu_line(r,p,i,o.cursor,o.flash_age.current,o.selection_age.current);r.write_ascii_format(p,data::main_labels[i]);p=saved;p.y+=data::f_0056f7b0;}
    r.fields_1a1d4[8]=r.fields_1a1d4[9]=1;r.fields_1a1d4[3]=0;r.color=0xffffffff;r.shadow_color=0xff000000;
}
}
