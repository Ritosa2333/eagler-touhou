#include "replay_save.hpp"
#include "name_data.hpp"
#include "name_draw.hpp"
#include "pages.hpp"
#include "../pause_system/draw_constants.hpp"
#include "../progress_state/records.hpp"
#include <ctime>
#include <stdexcept>
namespace th20::source::title {
namespace d=name_data;namespace n=recovered;namespace pr=progress;
namespace {
template<std::size_t N>const char* label(const char* const(&table)[N],int index){if(index<0||static_cast<std::size_t>(index)>=N)throw std::out_of_range("Replay save label");return table[index];}
void slot(text::Renderer& r,sprite::Vec3 position,int index,const replay::UserHeader& u){
    std::tm date{};_localtime64_s(&date,&u.timestamp);
    //5279xx passes four labels to a three-label format: %f therefore consumes
    //the stage pointer as its low word and the slowdown double's low word as
    //its high word. Spell out that x86 consumption instead of mismatching a
    //C++ variadic argument type. The stage label is not printed by the original.
    const double slowdown=static_cast<double>(pr::read<float>(&u,0xd0));
    std::uint64_t slowdown_bits;std::memcpy(&slowdown_bits,&slowdown,8);
    const auto* stage=label(pause::draw_data::replay_stages,u.finished_stage);
    const std::uint64_t consumed=(slowdown_bits<<32)|reinterpret_cast<std::uintptr_t>(stage);
    double displayed;std::memcpy(&displayed,&consumed,8);
    r.write_ascii_format(position,d::s_005753a8,index+1,reinterpret_cast<const char*>(&u),date.tm_year%100,date.tm_mon+1,date.tm_mday,date.tm_hour,date.tm_min,
        label(d::raw_character,static_cast<int>(u.fields_d0[2])),label(d::stone,static_cast<int>(u.stones[0])),label(d::raw_character,u.difficulty+4),displayed);
}
}
void draw_replay_save(TitleInf& o){
    auto& r=*text::renderer;
    if(o.phase==2){
        sprite::Vec3 position{d::f_0056ed10,d::f_0056fa30,0};r.fields_1a1d4[2]=1;
        for(int i=0;i<25;++i){r.color=o.cursor.selected(i)?0xffffff00:0xff808080;if(auto* value=static_cast<replay::ReplayInf*>(o.metadata[i]))slot(r,position,i,*value->user);else r.write_ascii_format(position,d::s_00575180,i+1);position.y=n::add32(position.y,15.f);}
        r.color=0xffffffff;r.fields_1a1d4[2]=0;
    }else if(o.phase==3){
        sprite::Vec3 position{d::f_0056ed10,d::f_0056f108,0};if(o.age.current<10){const float delta=n::add32(n::int_float(n::signed_bits(o.words5734[1]*15u+80u)),-d::f_0056f108);position.y=n::add32(n::mul32(delta,n::add32(10.f,-o.age.current_f))/10.f,d::f_0056f108);}
        auto& u=*replay::controller()->user;std::tm date{};_localtime64_s(&date,&u.timestamp);
        r.write_ascii_format(position,d::s_005753a8,n::signed_bits(o.words5734[1]+1u),"        ",date.tm_year%100,date.tm_mon+1,date.tm_mday,date.tm_hour,date.tm_min,
            label(d::raw_character,n::signed_bits(u.fields_d0[2]*9u+u.stones[0])),label(d::raw_character,u.difficulty+4),pause::draw_data::replay_stages[8],static_cast<double>(pr::read<float>(&u,0xd0)));
        if(o.age.current>=10){draw_entered_name(o,r,{d::f_0056f090,d::f_0056f108,0});draw_name_keyboard(o,r);r.color=0xffffffff;}
        r.color=0xffffffff;r.fields_1a1d4[2]=0;
    }
}
namespace unrecovered {void draw_data_page_005277f0(TitleInf& o){draw_replay_save(o);}}
}
