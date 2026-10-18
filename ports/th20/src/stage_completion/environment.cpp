#include "completion.hpp"
#include "dependencies.hpp"
#include "progress.hpp"
#include "playtime.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include "../gameplay/stage_data.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_services/services.hpp"
#include "../player_entity/stage_reset.hpp"
#include "../hud_system/hud.hpp"
namespace th20::source::gameplay {
namespace pe=program_entry;namespace deps=stage_completion::unrecovered;
namespace {
class GameCompletionEnvironment final:public CompletionEnvironment {
public:
 GameController& game()override{return *controller;}
 game_session::Session& session()override{return game_session::session;}
 progress::Profile& profile()override{return *progress::current_profile();}
 bool replay_has_stage(int stage)override{return deps::replay_has_stage(stage);}
 void finish_player()override{player_entity::finish_stage_visibility(*static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]));}
 void update_playtime()override{accumulate_playtime();}
 void finish_practice()override{deps::finish_practice();}
 void finish_replay()override{deps::finish_replay();}
 void mark_hud_clear()override{hud::controller->flags|=0x10u;}
 void achievement(int index)override{deps::announce_achievement(index);}
 int stone_count(unsigned index)override{return static_cast<int>(progress::manager->stone_count(index));}
 void notify_unlock(unsigned index)override{progress::notify_unlock(*progress::manager,index);}
 void grant_stone(unsigned index)override{progress::grant_stone(*progress::manager,index);}
 bool unlock_flag(unsigned index)override{return progress::unlock_flag(*progress::manager,index);}
 int& replay_selection()override{return gameplay::replay_selection;}
 void select_scene(int scene,bool guarded)override{pe::graphics_state.field_0b0c=guarded&&(pe::graphics_state.event_flags&0x200u)?2:scene;}
 void advance_stage()override{auto& table=game_session::session.player_table;int value=player_state::read<int>(table,0x1f4);if(value<7)++value;player_state::write(table,0x1f4,value);selected_stage=&stages[value];}
};
}
CompletionEnvironment& completion_environment(){static GameCompletionEnvironment host;return host;}
void accumulate_playtime(){accumulate_playtime(game_session::session,*progress::manager,controller->restart(),pe::graphics_state.field_0b0c,[]{return platform::read_clock(pe::window_state);});}
}
namespace th20::source::hud::unrecovered {void complete_stage_004bc570(){gameplay::complete_stage(*gameplay::controller);}}
