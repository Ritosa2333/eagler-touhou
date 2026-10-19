#include "replay_save.hpp"
#include "pages.hpp"
#include "../progress_state/manager.hpp"
#include "../gameplay/stage_data.hpp"
namespace th20::source::title {
namespace {
struct Production final:ReplaySaveEnvironment {
    Production():ReplaySaveEnvironment(selection_environment()){}
    void select_stage_eight()override{
        // 526b1a passes 8 through 4be360 as the post-clear result marker.
        // There is no eighth playable stage resource. Only update the session
        // marker here: selecting stages[8] either throws or creates an invalid
        // resource pointer, before the replay slots can even be displayed.
        gameplay::player_state::write(selection.main.session.player_table,0x1f4,8);
    }
    runtime::CallbackOwner* read_metadata(const char* filename)override{return replay::read_metadata(filename);}
    void retire(runtime::CallbackOwner* value)override{runtime::retire_callback_owner(value);}
    void retire_replay()override{runtime::retire_callback_owner(replay::controller());}
    void prepare_save()override{replay::prepare_save(*replay::controller(),1);}
    char* default_name()override{return reinterpret_cast<char*>(progress::manager->current.metadata.bytes+12);}
    void save(const char* filename,const char* name)override{replay::save(*replay::controller(),filename,name,1,0);}
    void play_title_music()override{play_music("th20_01");}
};
}
ReplaySaveEnvironment& replay_save_environment(){static Production value;return value;}
namespace unrecovered {void update_data_page_00526a90(TitleInf& o){update_replay_save(o,replay_save_environment());}}
}
