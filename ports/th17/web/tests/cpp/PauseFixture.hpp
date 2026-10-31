#include "../../cpp/game/PauseMenu.hpp"
#include "../../cpp/game/PauseActions.hpp"
#include "../../cpp/game/PauseActivation.hpp"
struct PauseMenuFixture final:PauseMenuServices {
    AnmManager& animations;PauseState pause;GameRun progress;PlayerLifeSession player;ItemScoreState score;Rng random;RecordStore records;PauseMenu controller;
    std::vector<i32> sounds;u32 reads=0,preparations=0,opened=0,closed=0,actions=0;bool options_done=false;std::array<u8,0x3f8> projection{};
    explicit PauseMenuFixture(AnmManager& a):animations(a),controller(pause,progress,player,score,records,a,*this){records.reset(random);}
    bool sound(i32 id)override{sounds.push_back(id);return true;}
    bool capture_score_details(PauseScoreDetails& value)override{value.timestamp={0,0};value.delivered=value.requested=1;return true;}
    bool synchronize_result_score()override{if(u32(score.score)>u32(progress.scoring.high_score))progress.scoring.high_score=score.score;return true;}
    bool read_replay_slot(i32,std::shared_ptr<Replay>& value)override{reads++;value.reset();return true;}
    bool save_named_replay(i32,const std::array<char,9>&)override{return true;}
    bool prepare_replay_save(bool)override{preparations++;return true;}
    bool open_options(float)override{opened++;return true;}bool options_finished()const override{return options_done;}
    bool close_options()override{closed++;options_done=false;return true;}bool finish_pause_selection()override{actions++;return true;}
    bool configure(i32 phase,i32 age,i32 screen,i32 slot,u32 mode,i32 continues,bool replay,u32 flags,i32 budget,i32 script){
        progress=GameRun{};progress.scoring.mode_flags=mode;progress.scoring.continues=continues;progress.practice_record=replay;progress.ranking_default=budget;progress.character=0;progress.animal=0;
        pause=PauseState{};pause.screen=PauseScreen(screen);pause.flags=flags;pause.phase=phase;pause.age.set(age);pause.front_bank=5;pause.menu.count=5;pause.menu.wrap=1;pause.menu.select(slot);pause.name.fill(' ');pause.name[8]=0;
        score=ItemScoreState{};random=Rng{};records.reset(random);pause.menu_animation=animations.create_overlay(5,script);return pause.menu_animation!=0;
    }
    const u8* bytes(){
        projection.fill(0);auto put=[&](u32 at,const auto& value){std::memcpy(projection.data()+at,&value,sizeof value);};
        put(0xc,pause.age);put(0x20,pause.selection_age);put(0x34,pause.menu);put(0x10c,pause.names);put(0x1e4,pause.menu_animation);put(0x1e8,pause.snapshot_animation);put(0x1ec,pause.screen);put(0x1f0,pause.previous);put(0x1f4,pause.phase);put(0x1f8,pause.name_length);put(0x1fc,pause.result_mode);
        put(0x200,i32(pause.name_not_required));put(0x204,i32(pause.retry_controls));put(0x208,pause.saved_frame_skip);std::memcpy(projection.data()+0x2d4,pause.name.data(),9);put(0x2e0,pause.saved_rate);put(0x2e8,pause.saved_music_position);put(0x3f0,pause.flags);return projection.data();
    }
};
API(pause_menu_create) PauseMenuFixture* pause_menu_create(AnmManagerFixture* a){return new PauseMenuFixture(a->manager);}
API(pause_menu_delete) void pause_menu_delete(PauseMenuFixture* p){delete p;}
API(pause_menu_configure) bool pause_menu_configure(PauseMenuFixture* p,i32 phase,i32 age,i32 screen,i32 slot,u32 mode,i32 continues,bool replay,u32 flags,i32 budget,i32 script){return p->configure(phase,age,screen,slot,mode,continues,replay,flags,budget,script);}
API(pause_menu_update) bool pause_menu_update(PauseMenuFixture* p,u32 pressed,u32 repeated){p->sounds.clear();return p->controller.update(pressed,repeated);}
API(pause_menu_tick) void pause_menu_tick(PauseMenuFixture* p,float rate){p->pause.age.tick(&rate);p->pause.selection_age.tick(&rate);}
API(pause_menu_state) const u8* pause_menu_state(PauseMenuFixture* p){return p->bytes();}
API(pause_name_blocks) const u8* pause_name_blocks(PauseMenuFixture* p,u32 settings){const auto& f=p->records.export_blocks();return settings?f.settings.data():f.characters[0].data();}
API(pause_name_sounds) const i32* pause_name_sounds(PauseMenuFixture* p){return p->sounds.data();}
API(pause_name_sound_count) u32 pause_name_sound_count(PauseMenuFixture* p){return p->sounds.size();}
API(pause_menu_options_done) void pause_menu_options_done(PauseMenuFixture* p,bool done){p->options_done=done;}
API(pause_menu_count) u32 pause_menu_count(PauseMenuFixture* p,u32 kind){switch(kind){case 0:return p->reads;case 1:return p->preparations;case 2:return p->opened;case 3:return p->closed;default:return p->actions;}}
API(pause_menu_error) const char* pause_menu_error(PauseMenuFixture* p){return p->controller.error.c_str();}
API(pause_menu_points) void pause_menu_points(PauseMenuFixture* p,u32 points){p->score.score=signed_bits(points);}
API(pause_menu_choose) void pause_menu_choose(PauseMenuFixture* p,i32 selected){p->pause.menu.select(selected);}
API(pause_menu_name_choose) void pause_menu_name_choose(PauseMenuFixture* p,i32 selected){p->pause.names.select(selected);}
