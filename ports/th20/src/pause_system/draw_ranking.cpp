#include "draw.hpp"
#include "draw_constants.hpp"
#include "../progress_state/manager.hpp"
#include "../gameplay/player_state.hpp"
#include <ctime>
#include <stdexcept>
namespace th20::source::pause {
namespace d=draw_data;namespace n=recovered;namespace pr=progress;
void draw_score_ranking(PauseInf& o){
    auto& renderer=*text::renderer;sprite::Vec3 position{d::f_0056fb7c,d::f_0056fa28,0};int selected=o.cursor.current;
    renderer.write_ascii_format(position,d::s_00572598);
    position.y=n::add32(n::mul32(n::int_float(selected),d::f_00572644),d::f_0056fe7c);position.x=n::add32(position.x,d::f_00572648);
    if(o.field_e4==0)draw_name_editor(o,position);else selected=-1;
    position.x=d::f_0056fb7c;position.y=d::f_0056fe7c;std::tm date{};
    for(int i=0;i<10;++i){renderer.color=i==selected?0xffffff00:0xff808080;
        const int difficulty=gameplay::player_state::difficulty(game_session::session.player_table);
        if(difficulty<0||difficulty>=5)throw std::out_of_range("Pause ranking difficulty outside original records");
        const auto* record=pr::current_profile()->bytes+0x18+difficulty*400+i*0x28;
        const auto stamp=pr::read<__time64_t>(record,0x18);const auto score=pr::read<std::uint64_t>(record,0);const int digit=static_cast<std::int8_t>(record[9]);
        if(stamp){_localtime64_s(&date,&stamp);const int stage=static_cast<std::int8_t>(record[8]);if(stage<0||stage>=10)throw std::out_of_range("Pause ranking stage outside original labels");renderer.write_ascii_format(position,d::s_005725b4,i+1,reinterpret_cast<const char*>(record+10),score,digit,date.tm_year%100,date.tm_mon+1,date.tm_mday,d::score_stages[stage]);}
        else renderer.write_ascii_format(position,d::s_005725d8,i+1,reinterpret_cast<const char*>(record+10),score,digit);
        position.y=n::add32(position.y,d::f_00572644);
    }
    renderer.color=0xffffffff;
}
}

