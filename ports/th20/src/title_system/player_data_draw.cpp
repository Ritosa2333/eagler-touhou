#include "player_data.hpp"
#include "player_data_constants.hpp"
#include "practice_data.hpp"
#include "../pause_system/draw.hpp"
#include <cstring>
#include <ctime>
#include <stdexcept>
namespace th20::source::title {
namespace d=player_data_constants;
int draw_player_data_detail(TitleInf& o,text::Renderer& r,const progress::Profile& profile,const progress::Profile& fallback,const char* stone_name){
    if(o.phase!=2)return 1;
    auto header_style=[&]{r.field_1a1c8=0xffffffff;r.shadow_color=0xff000000;r.fields_1a1d4[4]=4;r.fields_1a1d4[8]=0;r.fields_1a1d4[9]=1;};
    const int character=o.cursor.current/8,difficulty=o.cursorbc.current;if(character<0||character>=2||difficulty<0||difficulty>=5)throw std::out_of_range("Player data selection");
    header_style();r.write_text_literal({d::f_0056f10c,d::f_0056fa30,0},d::characters[character]);r.write_text_literal({d::f_0056f10c,d::f_0056fe7c,0},stone_name);pause::reset_pause_text(r);
    sprite::Vec3 pos{d::f_0056fb7c,d::f_0057566c,0};
    if(o.words58d8[8]==0){
        r.fields_1a1d4[2]=1;
        if(o.cursor108.current==0)for(int row=0;row<10;++row){
            r.color=0xffffff00u|unsigned(255-row*16);const auto* record=profile.bytes+0x18+difficulty*400+row*0x28;
            const auto score=progress::read<std::uint64_t>(record,0);const int digit=std::int8_t(record[9]);const auto* name=reinterpret_cast<const char*>(record+10);const auto timestamp=progress::read<__time64_t>(record,0x18);
            if(timestamp==0)r.write_ascii_format(pos,d::s_00575300,row+1,name,score,digit);
            else {
                const int stage=std::int8_t(record[8]);if(stage<0||stage>=9)throw std::out_of_range("Player data score stage");std::tm time{};_localtime64_s(&time,&timestamp);
                r.write_ascii_format(pos,d::s_005752c8,row+1,name,score,digit,time.tm_year+1900,time.tm_mon+1,time.tm_mday,time.tm_hour,time.tm_min,d::stages[stage],double(progress::read<float>(record,0x20)));
            }
            pos.y+=d::f_00572644;
        }
    }else{
        int id=0,skipped=0;const int offset=o.cursor108.current*10;
        while(skipped<offset){if(id>=113)throw std::out_of_range("Player data card page");if(practice_data::difficulties[id]==difficulty)++skipped;++id;}
        header_style();pos.x=d::f_0056cda0;
        for(int row=0;row<10;++row){
            r.field_1a1c8=0xffffff00u|unsigned(255-row*16);while(id<113&&practice_data::difficulties[id]!=difficulty)++id;if(id>=113)break;
            const auto* shared=fallback.bytes+0xb08+id*0xe0;const auto* own=profile.bytes+0xb08+id*0xe0;const int number=id+1;
            const char* hundreds=number/100?d::digits[number/100]:d::s_00575334;const char* tens=(number/10)%10==0&&number/100==0?d::s_00575334:d::digits[(number/10)%10];const char* units=d::digits[number%10];
            if(progress::read<unsigned>(shared,0xc8)==0)r.write_text(pos,d::s_00575350,hundreds,tens,units);
            else{
                char name[165];strcpy_s(name,sizeof(name),reinterpret_cast<const char*>(shared));auto length=std::strlen(name);
                while(length<42){strcpy_s(name+length,sizeof(name)-length,d::s_00575334);length+=2;}name[length]='\0';
                r.write_text(pos,d::s_00575338,hundreds,tens,units,name);
            }
            if(o.age.current>=row)r.write_text({pos.x+d::f_0056ec9c,pos.y,0},d::s_00575348,progress::read<int>(own,0xc0),progress::read<int>(own,0xc8));
            ++id;pos.y+=d::f_00572644;
        }
    }
    r.color=0xffffffff;pos={d::f_00575680,d::f_00575690,0};r.write_ascii_format(pos,d::s_00575388,progress::read<int>(profile.bytes,0x76a8));pos.y+=d::f_00572644;
    const auto seconds=progress::read<std::uint64_t>(profile.bytes,0x76b0)/100,minutes=seconds/60,hours=minutes/60;
    r.write_ascii_format(pos,d::s_00575390,hours,minutes%60,seconds%60);pos.y+=d::f_00572644;
    r.write_ascii_format(pos,d::s_00575388,progress::read<int>(profile.bytes,0x76b8+difficulty*4));pos.y+=d::f_00572644;r.color=0xffffffff;r.fields_1a1d4[2]=0;return 1;
}
}
