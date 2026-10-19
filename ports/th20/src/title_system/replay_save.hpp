#pragma once
#include "rank_entry.hpp"
#include "../replay_system/replay.hpp"
namespace th20::source::title {
struct ReplaySaveEnvironment {
    SelectionEnvironment& selection;
    explicit ReplaySaveEnvironment(SelectionEnvironment& value):selection(value){}
    virtual ~ReplaySaveEnvironment()=default;
    virtual void select_stage_eight()=0;
    virtual runtime::CallbackOwner* read_metadata(const char*)=0;
    virtual void retire(runtime::CallbackOwner*)=0;
    virtual void retire_replay()=0;
    virtual void prepare_save()=0;
    virtual char* default_name()=0;
    virtual void save(const char*,const char*)=0;
    virtual void play_title_music()=0;
};
ReplaySaveEnvironment& replay_save_environment();
void update_replay_save(TitleInf&,ReplaySaveEnvironment&); //526a90
void draw_replay_save(TitleInf&); //5277f0
}
