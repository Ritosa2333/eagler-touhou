#include "stage_select.hpp"
#include "stage_select_data.hpp"
namespace th20::source::title {
void draw_stage(TitleInf& o,text::Renderer& r,const progress::Profile& profile,int difficulty){
    if(o.phase!=2&&o.phase!=3)return;
    sprite::Vec3 p{stage_select_data::f_0056fa30,stage_select_data::f_0056fa30,0};r.fields_1a1d4[3]=7;
    if(o.age.current>=10||o.phase==3){
        p.x=stage_select_data::f_00575684;p.y=stage_select_data::f_00575668;
        for(int i=0;i<6;++i){const bool unlocked=stage_available(profile,difficulty,i);
            r.color=o.cursor.current!=i?0xff808080:!unlocked?0xffdfdfdf:o.phase==3&&o.age.current%4>=2?0xff000000:0xffffff00;
            r.write_ascii_format(p,stage_select_data::s_0056e0e4,stage_select_data::labels[i]);p.x=recovered::add32(p.x,stage_select_data::f_00570ad4);
            if(unlocked)r.write_ascii_format(p,stage_select_data::s_00575130,progress::read<std::int64_t>(&profile,0x76f8+difficulty*0x90+i*16));else r.write_ascii_format(p,stage_select_data::s_00575124);
            p.x=_mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(p.x),_mm_set_ss(stage_select_data::f_00570ad4)));p.y=recovered::add32(p.y,stage_select_data::f_00575654);
        }
    }
    r.color=0xffffffff;r.fields_1a1d4[2]=0;r.fields_1a1d4[3]=0;
}
}
