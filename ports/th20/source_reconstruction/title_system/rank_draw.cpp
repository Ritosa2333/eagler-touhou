#include "rank_entry.hpp"
#include "name_draw.hpp"
#include "name_data.hpp"
#include "pages.hpp"
#include "../pause_system/draw.hpp"
#include "../pause_system/draw_constants.hpp"
#include "../progress_state/manager.hpp"
#include "../stone_menu/stone.hpp"
#include <ctime>
#include <stdexcept>
namespace th20::source::title {
namespace d=name_data;namespace n=recovered;namespace pr=progress;
void draw_rank_entry(TitleInf& o){
    if(o.phase!=2)return;auto& r=*text::renderer;auto& session=game_session::session;auto& player=*session.contexts[0].current_player;const int character=static_cast<int>(player.fields_00[2]),profile=static_cast<int>(player.fields_00[3]);
    if(character<0||character>=2||profile<0||profile>=22)throw std::out_of_range("Rank title character/stone");
    r.field_1a1c8=0xffffffff;r.shadow_color=0xff000000;r.fields_1a1d4[4]=4;r.fields_1a1d4[8]=0;r.fields_1a1d4[9]=1;
    r.write_text_literal({d::f_0056f10c,d::f_0056fa30,0},d::japanese_character[character]);r.write_text_literal({d::f_0056f10c,d::f_0056fe7c,0},stone_menu::controller->names[profile*4]);pause::reset_pause_text(r);
    sprite::Vec3 position{d::f_0056fc04,d::f_0056fb80,0};const int difficulty=static_cast<int>(session.player_table.field_1e0);r.fields_1a1d4[2]=1;
    for(int i=0;i<10;++i){
        r.color=o.word56e4==0?(o.cursor.current==i?0xffffffffu:0xff808040u):(255u-unsigned(i)*16u)|0xffffff00u;
        auto* record=pr::current_profile()->bytes+0x18+difficulty*400+i*40;const auto stamp=pr::read<__time64_t>(record,24);const auto score=pr::read<std::uint64_t>(record,0);const int digit=static_cast<std::int8_t>(record[9]);
        if(stamp){std::tm date{};_localtime64_s(&date,&stamp);const int stage=static_cast<std::int8_t>(record[8]);if(stage<0||stage>=10)throw std::out_of_range("Rank stage label");r.write_ascii_format(position,d::s_005752c8,i+1,reinterpret_cast<const char*>(record+10),score,digit,date.tm_year+1900,date.tm_mon+1,date.tm_mday,date.tm_hour,date.tm_min,pause::draw_data::score_stages[stage],static_cast<double>(pr::read<float>(record,32)));}
        else r.write_ascii_format(position,d::s_00575300,i+1,reinterpret_cast<const char*>(record+10),score,digit);
        position.y=n::add32(position.y,18.f);
    }
    if(o.word56e4==0){draw_entered_name(o,r,{d::f_00575660,n::add32(n::mul32(n::int_float(o.cursor.current),18.f),d::f_0056fb80),0});draw_name_keyboard(o,r);r.fields_1a1d4[2]=0;}
}
namespace unrecovered {void draw_spell_00522e20(TitleInf& o){draw_rank_entry(o);}}
}
