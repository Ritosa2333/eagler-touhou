#include "../../cpp/game/PauseActions.hpp"
struct PauseActionsFixture final:PauseRestorationServices,PauseGameplayActions,PauseActionServices {
 PauseState state;GameRun run;PlayerLifeSession player;ItemScoreState score;AnmManager& animations;i32 skip=7;std::vector<i32> events;
 PauseRestoration restoration{state,run,skip,*this};PauseActions actions{state,run,player,score,animations,restoration,*this,*this,skip};
 std::array<i32,19> projection{};
 explicit PauseActionsFixture(AnmManager& a):animations(a){}
 bool event(std::initializer_list<i32> e){events.insert(events.end(),e);return true;}
 bool restart_elapsed_play_time()override{return event({1});}
 bool interrupt_pause_visual(u32 h,i32 label)override{return animations.interrupt(h,label);}
 bool resume_dialogue()override{return event({2});}bool resume_chapter_result()override{return event({3});}
 bool resume_looping_sounds()override{return event({4});}bool resume_music()override{return event({5});}
 bool resume_saved_music(const std::string& file,double value)override{return file=="native-current.wav"&&value==123.125&&event({6});}
 bool options_changed()override{return event({7});}bool life_hud(i32 n,i32 p)override{return event({8,n,p});}bool bomb_hud(i32 n,i32 p)override{return event({9,n,p});}bool power_notice()override{return event({10});}
 bool reset_audio_slots()override{return event({11});}bool destination(PauseDestination d)override{run.screen=i32(d);return true;}
 void configure(i32 screen,i32 selected,bool replay,i32 stage,i32 maximum,i32 step,i32 continues,bool result){
  state=PauseState{};run=GameRun{};player=PlayerLifeSession{};score=ItemScoreState{};skip=7;events.clear();restoration.error.clear();actions.error.clear();
  state.screen=PauseScreen(screen);state.menu.count=5;state.menu.select(selected);state.saved_rate=.75f;state.saved_frame_skip=3;state.saved_music="native-current.wav";state.saved_music_position=123.125;state.result_mode=result;
  run.practice_record=replay;run.stage=stage;run.rate=1;run.scene_flags=0x4090;run.scoring.continues=continues;run.ranking_default=4;
  player.extra_lives=5;player.life_pieces=2;player.bombs=4;player.bomb_pieces=3;player.power=24;player.power_step=step;score.max_power=maximum;score.score=1234567;
 }
 const i32* values(){projection={i32(run.scene_flags),i32(float_to_bits(run.rate)),skip,run.screen,player.extra_lives,player.life_pieces,player.bombs,player.bomb_pieces,player.power,player.power_step,score.max_power,score.score,run.scoring.continues,run.ranking_default,i32(state.screen),i32(state.previous),state.phase,i32(state.snapshot_animation),i32(state.menu_animation)};return projection.data();}
};
API(pause_actions_create) PauseActionsFixture* pause_actions_create(AnmManagerFixture* a){return new PauseActionsFixture(a->manager);}
API(pause_actions_delete) void pause_actions_delete(PauseActionsFixture* p){delete p;}
API(pause_actions_configure) void pause_actions_configure(PauseActionsFixture* p,i32 screen,i32 selected,bool replay,i32 stage,i32 maximum,i32 step,i32 continues,bool result){p->configure(screen,selected,replay,stage,maximum,step,continues,result);}
API(pause_actions_run) bool pause_actions_run(PauseActionsFixture* p,bool selection){if(!p->actions.execute(selection))return false;p->state.select(PauseScreen::Inactive);return true;}
API(pause_actions_values) const i32* pause_actions_values(PauseActionsFixture* p){return p->values();}
API(pause_actions_events) const i32* pause_actions_events(PauseActionsFixture* p){return p->events.data();}
API(pause_actions_event_count) u32 pause_actions_event_count(PauseActionsFixture* p){return p->events.size();}
API(pause_actions_error) const char* pause_actions_error(PauseActionsFixture* p){return p->actions.error.c_str();}
