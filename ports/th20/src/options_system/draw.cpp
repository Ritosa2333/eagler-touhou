#include "options.hpp"
#include "data.hpp"
#include "../text_renderer/menu_style.hpp"
namespace th20::source::options {
int draw(OptionInf& o,text::Renderer& r,const platform::Configuration& configuration,int display_mode,bool key_active){
    if(key_active)return 1;
    namespace d=data;
    r.fields_1a1d4[3]=7;
    sprite::Vec3 position=o.position;
    r.fields_1a1d4[8]=0;r.fields_1a1d4[9]=1;
    position.x+=d::f_00571058;r.color=0xffffffff;r.shadow_color=0xff000000;r.write_ascii_format(position,"Option");
    position.y+=d::f_0056fd48;r.color=0xffffffff;position.x-=d::f_00571058;position.y+=d::f_0056f7a8;
    r.fields_1a1d4[8]=r.fields_1a1d4[9]=1;
    for(int i=0;i<6;++i){
        const auto saved=position;text::style_menu_line(r,position,i,o.cursor,o.key_config_age.current,o.selection_age.current);
        r.write_ascii_format(position,d::labels[i]);position.x+=d::f_0056f108;r.fields_1a1d4[8]=0;r.fields_1a1d4[9]=1;
        switch(i){
        case 0:if(display_mode>=0&&display_mode<=2)r.write_ascii_format(position,"FullScreen");else if(display_mode>=3&&display_mode<=7)r.write_ascii_format(position,"Window");else if(display_mode==8)r.write_ascii_format(position,"BorderlessDBD");else if(display_mode==9)r.write_ascii_format(position,"Borderless");break;
        case 1:r.write_ascii_format(position,"%d%%",static_cast<int>(static_cast<std::int8_t>(configuration.value_7e)));break;
        case 2:r.write_ascii_format(position,"%d%%",static_cast<int>(static_cast<std::int8_t>(configuration.value_7f)));break;
        }
        r.fields_1a1d4[8]=r.fields_1a1d4[9]=1;position=saved;position.y+=d::f_0056fd48;
    }
    r.fields_1a1d4[3]=0;r.color=0xffffffff;r.fields_1a1d4[4]=3;r.color=0xffc0c080;r.field_1a1c8=0xffffffff;r.shadow_color=0xff000000;r.fields_1a1d4[8]=r.fields_1a1d4[9]=0;
    const sprite::Vec3 description{o.position.x+d::f_00571058,o.position.y+d::f_0056ec9c,0.f};
    switch(o.cursor.current){
    case 0:r.write_text_literal(description,d::s_0057228c);r.write_text_literal({o.position.x+d::f_00571058,o.position.y+d::f_0056f10c,0.f},d::screens[display_mode]);break;
    case 1:r.write_text_literal(description,d::s_005722a4);break;case 2:r.write_text_literal(description,d::s_005722bc);break;
    case 3:r.write_text_literal(description,d::s_005722d8);break;case 4:r.write_text_literal(description,d::s_00570f94);break;case 5:r.write_text_literal(description,d::s_005722fc);break;
    }
    r.color=0xffffffff;r.scale_x=r.scale_y=1.f;r.fields_1a1d4[8]=r.fields_1a1d4[9]=1;return 1;
}
}
