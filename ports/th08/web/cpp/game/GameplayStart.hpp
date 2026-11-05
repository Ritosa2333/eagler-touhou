#pragma once
#include "GameplaySession.hpp"
#include "GameplayControl.hpp"
namespace th08 {
struct GameplayStartActions {
    virtual ~GameplayStartActions()=default;
    virtual void save_score()=0;
    virtual std::vector<u8> read_score()=0;
    virtual void preload_music(i32 slot,const char* path)=0;
};
// Original GameplaySetupThread's game data changes are split at its player and
// resource-registration boundaries. The caller executes the real owners there.
class GameplayStart {
    GameplaySession& session;EclGlobals& game;GameplayControlState& control;MenuContext& menu;GameplayStartActions& actions;
    bool fresh=false;
    void initialize_score();void initialize_rank();
public:
    GameplayStart(GameplaySession& s,EclGlobals& g,GameplayControlState& c,MenuContext& m,GameplayStartActions& a):session(s),game(g),control(c),menu(m),actions(a){}
    bool before_player(bool initial);
    void after_player(float initial_bombs);
    void after_replay();
    void after_resources(bool keep,const char (&songs)[3][128],i32& background_state);
};
}
