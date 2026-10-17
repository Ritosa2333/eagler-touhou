#include "replay.hpp"
#include "input.hpp"
#include "../program_entry/program_entry.hpp"
#include "../gameplay/player_state.hpp"
#include "../player_entity/owner.hpp"
#include "../player_entity/power.hpp"
#include "../runtime_state/state.hpp"
#include <cstring>
#include <new>
#include <emmintrin.h>
namespace th20::source::replay {
void ReplayInf::reset_stage(){
    if(mode!=0&&mode!=1)return;
    const auto index=gameplay::player_state::stage(game_session::session.player_table);
    if(mode==0){
        auto* memory=runtime::allocate_bytes(sizeof(StageRecord));if(!memory)throw std::bad_alloc();auto* record=new(memory)StageRecord;stages[index]=record;
        record->seed=state::random_streams[0].last;state::seed(state::random_streams[1],record->seed);state::random_streams[0].field_00=0;
        record->stage=static_cast<std::int16_t>(gameplay::player_state::stage(game_session::session.player_table));record->flags=(record->flags&~1u)|(program_entry::graphics_state.field_0b18&1u);
    }else if(mode==1){auto& cursor=playback[index];cursor.rewind(false);state::seed(state::random_streams[0],cursor.stage->seed);state::seed(state::random_streams[1],cursor.stage->seed);state::random_streams[0].field_00=0;}
}
void ReplayInf::enable_callbacks(){
    if(update_node)scheduler::enable(*update_node);if(additional_update)scheduler::enable(*additional_update);if(draw_node)scheduler::enable(*draw_node);
    reset_input(input::shared_state().slots[0]);auto& table=game_session::session.player_table;
    if(mode!=0&&mode!=1){frame=0;return;}
    const int index=gameplay::player_state::stage(table);auto& player=*static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);
    if(mode==0){auto& record=*stages[index];clear_recording(index);active_chunk=add_recording_chunk(index);record.player_table=table;record.fixed_x=player.fixed_position.x;record.fixed_y=player.fixed_position.y;record.field_18=player.focused_204c;frame=0;}
    else if(mode==1){
        auto& record=*playback[index].stage;active_stage=index;table=record.player_table;
        player.fixed_position={record.fixed_x,record.fixed_y};
        player.position_614.x=_mm_cvtss_f32(_mm_div_ss(_mm_cvtsi32_ss(_mm_setzero_ps(),record.fixed_x),_mm_set_ss(128)));
        player.position_614.y=_mm_cvtss_f32(_mm_div_ss(_mm_cvtsi32_ss(_mm_setzero_ps(),record.fixed_y),_mm_set_ss(128)));
        player_entity::mark_options_changed(player,1);player.focused_204c=record.field_18!=0;
        for(auto& point:player.vectors_20fc)std::memcpy(&point,&player.fixed_position,sizeof(point));
        playback[index].rewind(true);player_entity::refresh_power(player,-1);flags&=~2u;
    }
    frame=0;
}
}
