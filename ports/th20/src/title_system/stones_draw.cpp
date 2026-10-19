#include "stones.hpp"
#include "stones_data.hpp"
namespace th20::source::title {
int draw_stones(TitleInf& o,text::Renderer& r,StonesEnvironment& e){
    if(o.phase!=1)return 1;using namespace stones_data;
    sprite::Vec3 p{f_00575664,f_00570ad0,0};const float left=p.x;r.fields_1a1d4[3]=4;r.fields_1a1d4[2]=1;
    for(int i=0;i<41;++i){r.color=o.cursor.selected(i)?0xffffffffu:0xff707070u;if(e.achieved(unsigned(i)))r.write_padded_integer(p,i+1,2,' ');else r.write_ascii(p,"--");if(i%10==9){p.x=left;p.y+=f_00575654;}else p.x+=f_00575658;}
    r.fields_1a1d4[3]=0;r.fields_1a1d4[2]=0;const bool achieved=e.achieved(o.words58d8[10]);r.field_1a1c8=0xffffffff;r.color=achieved?0xffffffffu:0xff808080u;r.shadow_color=0xff000000;r.fields_1a1d4[4]=5;r.fields_1a1d4[8]=0;r.fields_1a1d4[9]=1;
    p={f_0056f10c,f_00575688,0};if(achieved)r.write_text(p,"%s",e.title(int(o.words58d8[10])));else r.write_text(p,locked_title);
    const float ys[]{f_0057568c,f_00572660,f_00575694};for(int row=0;row<3;++row){p={f_0056f10c,ys[row],0};r.write_text(p,"%s",e.description(int(o.words58d8[10]),achieved,row));}
    //4e67e0 restores all original text properties, not every Renderer field.
    r.color=r.field_1a1c8=0xffffffff;r.shadow_color=0xff000000;r.fields_1a1d4[1]=0;r.font_width=9;r.scale_x=r.scale_y=1;r.fields_1a1d4[2]=r.fields_1a1d4[3]=0;r.fields_1a1d4[4]=2;r.fields_1a1d4[6]=r.fields_1a1d4[7]=0;r.fields_1a1d4[8]=r.fields_1a1d4[9]=1;r.rotation=0;r.fields_1a1d4[10]=0;return 1;
}
}
