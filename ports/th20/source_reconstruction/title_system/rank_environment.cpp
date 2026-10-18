#include "rank_entry.hpp"
#include "pages.hpp"
#include "../progress_state/manager.hpp"
#include "../pause_system/menu_support.hpp"
#include "../stone_menu/update.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../program_entry/program_entry.hpp"
#include "../gameplay/stage_data.hpp"
#include "../replay_system/replay.hpp"
#include "../audio_runtime/audio.hpp"
#include "../hud_system/hud.hpp"
#include "../hud_system/dialogue.hpp"
namespace th20::source::title {
namespace {
struct Production final:RankEnvironment {
    Production():RankEnvironment(selection_environment()){}
    void play_score_music()override{program_entry::thread_registry.enqueue(1,0,"th128_08.wav");hud::play_stage_track(0,17);} //readonly572628
    void play_title_music()override{play_music("th20_01");}
    void open_stones()override{stone_menu::open(*stone_menu::controller,2);}
    void select_stone(int value)override{sprite::execute_animation_interrupt(*program_entry::sprite_controller,stone_menu::controller->animation_handles[8],value+7);}
    void hide_stones()override{stone_menu::hide(*stone_menu::controller);}
    int insert_score()override{return pause::insert_high_score(*progress::current_profile());}
    progress::Profile& profile()override{return *progress::current_profile();}
    char* default_name()override{return reinterpret_cast<char*>(progress::manager->current.metadata.bytes+12);}
    void select_stage_zero()override{gameplay::select_stage(selection.main.session.player_table,0);}
    void retire_replay()override{runtime::retire_callback_owner(replay::controller());}
};
}
RankEnvironment& rank_environment(){static Production value;return value;}
namespace unrecovered {void update_spell_005223e0(TitleInf& o){update_rank_entry(o,rank_environment());}}
}
