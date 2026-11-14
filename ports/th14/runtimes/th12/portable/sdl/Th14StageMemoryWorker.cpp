#define TH14_STAGE_MEMORY_WORKER 1
#include "../../th12_web/cpp/game/StageRestoreMemoryTh14.hpp"
#define API(name) extern "C" __attribute__((export_name(#name)))
using namespace th11;
API(stage_memory_layout) u32 stage_memory_layout(){return th14_stage_memory_layout();}
API(stage_memory_packet_size) u32 stage_memory_packet_size(){return sizeof(StageRestoreMemoryTh14);}
API(stage_memory_entry_size) u32 stage_memory_entry_size(){return sizeof(ReplayStageState);}
API(stage_memory_economy_size) u32 stage_memory_economy_size(){return sizeof(GameEconomy);}
API(stage_memory_playback) void stage_memory_playback(const StageRestoreMemoryTh14* packet){
    th14_playback_gameplay_seed(*packet->gameplay,packet->entry->seed);
    th14_playback_script_seed(*packet->script,packet->entry->seed);
    th14_clear_gameplay_calls(*packet->gameplay);
    th14_project_stage_globals(*packet->entry,*packet->economy,packet->practice,packet->continues,packet->minimum_point_value,
        packet->character,packet->subtype);
}
API(stage_memory_record) void stage_memory_record(const StageRestoreMemoryTh14* packet){
    const auto captured=th14_record_gameplay_seed(*packet->gameplay,*packet->recording_seed);
    th14_record_script_seed(*packet->script,captured);
    th14_clear_gameplay_calls(*packet->gameplay);
}
