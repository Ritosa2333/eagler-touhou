#pragma once
#include "GameProgression.hpp"
#include "RoaringSystem.hpp"
namespace th17 {
struct GameStartHost {
    virtual ~GameStartHost()=default;
    virtual bool previous_bomb_display(i32 count,i32 pieces)=0;
    virtual bool create_player()=0;
    virtual bool configure_player()=0;
};
// New-run state from the initialization block of the TH17 scene factory.
// Retained-stage entry preserves the current run and only raises its high score.
class GameStart {
    GameRun& run;GameRecords& records;ItemScoreState& score;
    PlayerLifeSession& player;RoaringState& roaring;i32& grazes;GameStartHost& host;
    bool check(bool,const char*);
public:
    std::string error;
    GameStart(GameRun& r,GameRecords& d,ItemScoreState& s,PlayerLifeSession& p,RoaringState& a,i32& g,GameStartHost& h):run(r),records(d),score(s),player(p),roaring(a),grazes(g),host(h){}
    bool begin(bool fresh);
};
}
