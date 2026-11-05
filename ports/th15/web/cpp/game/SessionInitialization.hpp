#pragma once
#include "SessionState.hpp"
namespace th15 {
struct SessionInitializationServices {
    virtual ~SessionInitializationServices()=default;
    virtual bool bomb_hud(i32 bombs,i32 pieces)=0;
    virtual bool create_player()=0;virtual bool configure_player()=0;
    virtual bool register_session()=0;
    virtual bool create_replay()=0;virtual bool reset_replay()=0;
    virtual bool create_background()=0;virtual bool create_gui()=0;virtual bool reset_gui()=0;
    virtual bool create_bullets()=0;virtual bool create_items()=0;virtual bool create_lasers()=0;
    virtual bool create_pause_menu()=0;virtual bool create_popups()=0;virtual bool create_checkpoint_storage()=0;
    virtual bool create_enemies()=0;virtual bool restore_enemies()=0;
    virtual bool create_bomb()=0;virtual bool create_spell()=0;
    virtual bool load_stage_music()=0;virtual bool load_player_music(bool boss)=0;
    virtual bool finish_scene()=0;
};
class SessionInitialization {
    SessionState& state;PlayerLifeSession& player;ItemScoreState& score;EnemyWorldState& enemies;
    SessionRecords& records;SessionInitializationServices& host;
    bool check(bool,const char*);
public:
    std::string error;
    SessionInitialization(SessionState& s,PlayerLifeSession& p,ItemScoreState& v,EnemyWorldState& e,SessionRecords& r,SessionInitializationServices& h):state(s),player(p),score(v),enemies(e),records(r),host(h){}
    bool initialize();
};
}
