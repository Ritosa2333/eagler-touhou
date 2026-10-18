#include "menu_support.hpp"
#include "../platform_services/services.hpp"
#include "../progress_state/manager.hpp"
#include "../gameplay/player_state.hpp"
#include "../platform_window/platform_window.hpp"
#include "../platform_window/frame_statistics.hpp"
#include "../hud_system/hud.hpp"
#include <algorithm>
#include <ctime>
#include <cstring>
namespace th20::source::pause {
namespace ps=gameplay::player_state;namespace pr=progress;
void initialize_name(PauseInf& o){
    strcpy_s(o.player_name,reinterpret_cast<const char*>(pr::manager->current.metadata.bytes+12));o.name_length=0;
    if(std::strcmp(o.player_name,"        ")!=0)o.name_cursor.move(-1);
    int count=8;while(count>0&&o.player_name[count-1]==' ')--count;o.name_length=count;
}
namespace {
std::uint64_t score(){return ps::score(game_session::session.player_table.players[0]);}
}
int insert_high_score(pr::Profile& profile){ //50f170
    auto& session=game_session::session;auto* list=profile.bytes+0x18+ps::difficulty(session.player_table)*400;int index=0;
    while(index<10&&pr::read<std::uint64_t>(list,index*40)>score())++index;if(index>=10)return -1;
    for(int i=9;i>index;--i)std::memcpy(list+i*40,list+(i-1)*40,40);
    auto* record=list+index*40;pr::write(record,0,score());record[9]=static_cast<std::uint8_t>(continue_count(session));record[8]=static_cast<std::uint8_t>(ps::stage(session.player_table));
    pr::write(record,24,_time64(nullptr));strcpy_s(reinterpret_cast<char*>(record+10),10,"        ");
    const auto& stats=*static_cast<platform_window::FrameStatistics*>(platform_window::unrecovered::scheduler_object_005c4a00);
    // The browser adapter reports a consumed unlimited drag here; the protocol
    // forces the recorded processing-drop rate to 100% for such a run.
    const float percentage=platform::unlimited_touch_used()?100.f:
        100.f-static_cast<float>(stats.actual_frames/stats.target_frames)*100.f;pr::write(record,32,percentage);return index;
}
namespace {
void save_practice_score(pr::Profile& profile){ //50f2f0
    auto& table=game_session::session.player_table;auto* record=profile.bytes+0x76f8+ps::difficulty(table)*0x90+(ps::stage(table)-1)*0x10;
    if(pr::read<std::uint64_t>(record,0)<score())pr::write(record,0,score());
}
}
void update_ranking(PauseInf& o){
    auto& session=game_session::session;if(session.mode==2){o.field_e4=1;return;}
    if(session.mode!=0){save_practice_score(*pr::current_profile());o.field_e4=1;return;}
    if(ps::stage(session.player_table)==7&&o.completed)ps::write(session.player_table,0x1f4,9);
    const int rank=insert_high_score(*pr::current_profile());
    if(ps::stage(session.player_table)==9&&o.completed)ps::write(session.player_table,0x1f4,7);
    if(rank<0){o.field_e4=1;return;}
    o.cursor.count=25;o.cursor.wrapping=1;o.cursor.select(rank);o.name_cursor.select(0);o.name_cursor.count=static_cast<int>(std::strlen(name_characters()));o.name_cursor.wrapping=1;initialize_name(o);o.field_e4=0;
}
}
