#include "ContinueTh14.hpp"
#include "ContinueWordsTh14.hpp"
#include "HudScalarTh14.hpp"
namespace th11 {
#if PORT_TITLE == 14
bool th14_continue_apply(GameEconomy& e,i32& continues,ContinueActionsTh14& actions)noexcept{
    i32* words[]={&e.score_units,&continues,&e.power,&e.max_power,&e.power_step,
        &e.lives,&e.life_fragments,&e.th14_bombs,&e.th14_bomb_fragments};
    return th14_continue_words([&](ContinueAccessTh14,ContinueWordTh14 word){return th14_hud_load(*words[u32(word)]);},
        [&](ContinueAccessTh14,ContinueWordTh14 word,i32 value){th14_hud_store(*words[u32(word)],value);},
        [&](ContinueAccessTh14 access,i32 first,i32 second){
            switch(access){
            case ContinueAccessTh14::InitialBombs:case ContinueAccessTh14::DisplayBombs:return actions.bombs(first,second);
            case ContinueAccessTh14::PowerNotice:return actions.power_notice();
            case ContinueAccessTh14::Options:return actions.options();
            default:return actions.lives(first,second);
            }
        });
}
bool th14_continue_economy(GameEconomy& e,i32& continues)noexcept{
    // Compatibility entry for counter-only consumers. Live GameSession uses
    // th14_continue_apply so its real callbacks run at the original boundaries.
    struct Counters:ContinueActionsTh14 {bool capped=false;bool power_notice()noexcept override{capped=true;return true;}} actions;
    (void)th14_continue_apply(e,continues,actions);return actions.capped;
}
#endif
}
