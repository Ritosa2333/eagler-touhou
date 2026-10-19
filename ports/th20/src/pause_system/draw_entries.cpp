#include "draw.hpp"
#include "draw_constants.hpp"
#include "menu_support.hpp"
#include "../startup_scene/startup.hpp"
#include "../progress_state/records.hpp"
#include <ctime>
#include <cstring>
#include <stdexcept>
namespace th20::source::pause {
namespace d=draw_data;namespace n=recovered;
namespace {
template<std::size_t N>const char* label(const char*const(&values)[N],int index){if(index<0||static_cast<std::size_t>(index)>=N)throw std::out_of_range("Pause display index outside original table");return values[index];}
}
void draw_name_editor(PauseInf& o,sprite::Vec3 position){
    auto& renderer=*text::renderer;const int count=static_cast<int>(std::strlen(d::name_characters));
    renderer.write_ascii_format(position,d::s_0056e0e4,o.player_name);
    position.x=n::add32(n::int_float(n::signed_bits(static_cast<std::uint32_t>(o.name_length)*9u)),position.x);
    if(o.name_length==8)position.x=n::add32(position.x,-d::f_0056f7a4);
    renderer.color=0xffffff00;renderer.write_ascii_format(position,d::s_00572524);renderer.color=0xffffffff;
    sprite::Vec3 key{d::f_00572658,d::f_0056f10c,0};
    for(int i=0;i<count;++i){renderer.color=o.name_cursor.selected(i)?0xffffff00:0xff808080;
        const char character=i<count-3?d::name_characters[i]:i==count-3?char(0x81):i==count-2?char(0x7f):char(0x80);
        renderer.write_character(key,character);
        if(i%13==12){key.x=d::f_00572658;key.y=n::add32(key.y,d::f_0056cd90);}else key.x=n::add32(key.x,d::f_00572644);
    }
    renderer.color=0xffffffff;
}
void draw_replay_entry(int index,const sprite::Vec3& position,const replay::UserHeader& user){
    std::tm date{};_localtime64_s(&date,&user.timestamp);auto& renderer=*text::renderer;
    const auto* character=label(d::characters,static_cast<int>(user.fields_d0[2]));
    if(!(user.flags&2))renderer.write_ascii_format(position,d::s_00572528,n::signed_bits(static_cast<std::uint32_t>(index)+1u),reinterpret_cast<const char*>(&user),date.tm_year%100,date.tm_mon+1,date.tm_mday,character,label(d::difficulties,user.difficulty),label(d::replay_stages,user.finished_stage));
    else renderer.write_ascii_format(position,d::s_0057254c,n::signed_bits(static_cast<std::uint32_t>(index)+1u),reinterpret_cast<const char*>(&user),date.tm_year%100,date.tm_mon+1,date.tm_mday,character,n::signed_bits(static_cast<std::uint32_t>(user.spell)+1u));
}
void draw_replay_slots(PauseInf& o){
    auto& renderer=*text::renderer;sprite::Vec3 position{d::f_0056fb7c,d::f_0056fa28,0};
    for(int i=0;i<25;++i){renderer.color=o.cursor.selected(i)?0xffffff00:0xff808080;
        if(auto* metadata=static_cast<replay::ReplayInf*>(o.metadata[i]))draw_replay_entry(i,position,*metadata->user);
        else renderer.write_ascii_format(position,d::s_00572570,i+1);
        position.y=n::add32(position.y,d::f_0056f2fc);
    }
    renderer.fields_1a1d4[3]=0;renderer.color=0xffffffff;
}
void draw_replay_name(PauseInf& o){
    sprite::Vec3 position{d::f_0056fb7c,d::f_0056fa28,0};const int selected=o.cursor.current;
    if(o.age.current<10){const float row=n::add32(n::mul32(n::int_float(selected),d::f_0056f2fc),d::f_0056fa28);const float delta=n::add32(d::f_0056ed14,-row);position.y=n::add32(_mm_cvtss_f32(_mm_div_ss(_mm_set_ss(n::mul32(delta,n::int_float(o.age.current))),_mm_set_ss(d::f_0056e734))),row);}
    else position.y=d::f_0056ed14;
    position.x=n::add32(position.x,d::f_00572650);draw_name_editor(o,position);position.x=d::f_0056fb7c;
    auto& user=*static_cast<replay::ReplayInf*>(startup::unrecovered::owner_005c60fc)->user;
    strcpy_s(reinterpret_cast<char*>(&user),9,"        ");draw_replay_entry(selected,position,user);
}
}


