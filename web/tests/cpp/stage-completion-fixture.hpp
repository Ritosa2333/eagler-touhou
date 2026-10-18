#pragma once
#include "../../cpp/game/StageCompletion.hpp"
#include <vector>
namespace th11::test {
struct StageCompletionFixture:StageCompletionEffects {
    GameEconomy economy;ClearRecords records;float rate=1;
    StageCompletion completion{economy,records,*this,&rate};
    std::vector<i32> events;
    bool stage_result_animation()override{events.push_back(1);return true;}
    void recall_player_options()override{events.push_back(2);}
    void request_stage_exit(StageExit exit)override{events.push_back(10+i32(exit));}
    void start_ending_fade()override{events.push_back(3);}
};
}
