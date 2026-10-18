#include "music.hpp"
#include "music_data.hpp"
#include <algorithm>
#include <stdexcept>
namespace th20::source::title {
void draw_music(TitleInf& o,text::Renderer& r,const progress::Metadata& metadata){
    if(o.phase!=1&&o.phase!=2)return;
    sprite::Vec3 p{music_data::f_0056fa28,music_data::f_0056fe7c,0};r.field_1a1c8=0xffffffff;r.shadow_color=0xff000000;r.fields_1a1d4[4]=5;
    for(int row=0;row<std::min(o.age.current,10);++row){const int index=recovered::signed_bits(o.word56d0+unsigned(row));if(index>=recovered::signed_bits(o.words47c[0]))break;if(index<0||index>=32)throw std::out_of_range("Music row outside32 records");
        r.color=o.cursor.current==index?0xffffff00:0xffb0b0b0;
        if(metadata.bytes[0x36+index])r.write_text_literal(p,reinterpret_cast<const char*>(o.datac90)+index*0x42);else r.write_text(p,music_data::s_00575508,index+1);p.y=recovered::add32(p.y,music_data::f_0056ec8c);
    }
    r.shadow_color=0xff000000;r.color=0xffffffff;p.x=music_data::f_0056f090;p.y=music_data::f_0056f10c;r.fields_1a1d4[4]=4;
    const int playing=recovered::signed_bits(o.words47c[2]),rows=recovered::signed_bits(o.words47c[1]);if(playing<0||playing>=32||rows>8)throw std::out_of_range("Music comments outside32x8 records");
    for(int row=0;row<rows;++row){const char* line=!metadata.bytes[0x36+playing]&&o.words47c[4]?music_data::warnings[row]:reinterpret_cast<const char*>(o.data14d0)+playing*0x210+row*0x42;r.write_text_literal(p,line);p.y=recovered::add32(p.y,music_data::f_00572644);}
    r.field_1a1c8=0xffffffff;r.shadow_color=0xff000000;r.fields_1a1d4[4]=2;
}
}
