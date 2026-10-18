#include "replay.hpp"
#include "../program_entry/program_entry.hpp"
#include "../gameplay/gameplay.hpp"
#include "../gameplay/player_state.hpp"
#include "../overlay_system/overlay.hpp"
#include "../overlay_system/environment.hpp"
#include "../runtime_state/state.hpp"
#include <algorithm>
#include <new>
namespace th20::source::replay {
namespace pe=program_entry;namespace ps=gameplay::player_state;
namespace {
template<class T>T* allocate(){auto* memory=runtime::allocate_bytes(sizeof(T));if(!memory)throw std::bad_alloc();return new(memory)T;}
int __cdecl record_callback(void* o){return update_recording(*static_cast<ReplayInf*>(o));}
int __cdecl playback_callback(void* o){return update_playback(*static_cast<ReplayInf*>(o));}
int __cdecl extra_callback(void* o){if(gameplay::controller&&(gameplay::controller->game_flags&4u))return 1;return update_fast_forward(*static_cast<ReplayInf*>(o));}
int __cdecl draw_callback(void* o){if(gameplay::controller&&(gameplay::controller->game_flags&4u))return 1;return draw(*static_cast<ReplayInf*>(o));}
void register_callbacks(ReplayInf& o,scheduler::Callback update){
    o.update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,21,update,&o,false,false);
    o.additional_update=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,46,extra_callback,&o,false,false);
    o.draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,91,draw_callback,&o,true,false);
}
}
int initialize(ReplayInf& o,int mode,const char* path){
    o.mode=mode;auto& session=game_session::session;auto& table=session.player_table;
    if(mode==0){
        const auto stage=ps::stage(table);auto& link=o.recordings[stage];link.next=link.previous=nullptr;link.owner=nullptr;link.iterator=nullptr;
        o.header=allocate<FileHeader>();o.header->header_size=0x30;o.header->user_size=0x100;o.header->stage_size=0x2a0;o.user=allocate<UserHeader>();o.stages[stage]=allocate<StageRecord>();auto& record=*o.stages[stage];
        auto& player=*game_session::context(0).current_player;o.user->fields_d0[2]=player.fields_00[2];
        for(int slot=0;slot<4;++slot){o.user->stones[slot]=overlay::selected_stone(player,slot);o.user->inherited[slot]=overlay::inherited_stone(player,slot);}
        o.user->difficulty=ps::difficulty(table);o.user->flags=(o.user->flags&~1u)|(session.mode!=0);o.user->flags=(o.user->flags&~2u)|((session.mode==2?1u:0u)<<1);o.user->spell=ps::spell(table);
        if(gameplay::controller)o.user->configuration=gameplay::controller->configuration;
        record.stage=static_cast<std::int16_t>(ps::stage(table));record.seed=state::random_streams[0].last;state::random_streams[0].field_00=0;record.flags=(record.flags&~1u)|(pe::graphics_state.field_0b18&1u);
        if(pe::graphics_state.field_0b18)record.fixed_x=record.fixed_y=0;
        record.player_table=table;for(unsigned i=0;i<20;++i)record.capture_times[i]=recovered::signed_bits(i*0xdeaddeadu);
        table.continue_count=std::clamp(table.continue_count,0,9);o.user->field_f8=table.continue_count;
        register_callbacks(o,record_callback);o.active_stage=ps::stage(table);o.frame=-1;
    }else if(mode==1){
        if(load(o,path)!=0)return -1;
        gameplay::controller->configuration=o.user->configuration;auto& cursor=o.playback[ps::stage(table)];auto& record=*cursor.stage;cursor.rewind(false);
        auto& player=*game_session::context(0).current_player;player.fields_00[2]=o.user->fields_d0[2];constexpr unsigned offsets[]{0x1c,0x24,0x20,0x28};for(int slot=0;slot<4;++slot)ps::write(player,offsets[slot],o.user->stones[slot]);
        overlay::refresh_selection(*overlay::controller(),static_cast<int>(player.fields_00[2]),overlay::environment());table.field_1e0=o.user->difficulty;
        state::seed(state::random_streams[0],record.seed);state::random_streams[0].field_00=0;table=record.player_table;
        const int next_mode=ps::spell(table)<0?0:2;if(session.mode!=2)ps::write(table,0x204,-1);session.mode=next_mode;
        register_callbacks(o,playback_callback);o.active_stage=-1;
    }else if(mode==2){if(load(o,path)!=0)return -1;}
    return 0;
}
}
