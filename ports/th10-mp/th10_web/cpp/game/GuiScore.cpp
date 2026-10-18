#include "Gui.hpp"
namespace th10 {
static i32 difference(i32 left,i32 right){const u32 bits=static_cast<u32>(left)-static_cast<u32>(right);i32 result;std::memcpy(&result,&bits,4);return result;}
// 0x417040. Score animation accelerates toward the stored score. Extra lives
// follow the displayed score, one threshold per call, including negative deltas.
void Gui::update_score(GuiScoreEnvironment& env){
    auto& game=*env.game;const auto before=displayed_score;
    if(game.score!=displayed_score){
        i32 step=difference(game.score,displayed_score)/32;if(step>=0x8d55e)step=0x8d55e;else if(!step)step=1;
        if(score_step<step)score_step=step;const auto remaining=difference(game.score,displayed_score);if(remaining<score_step)score_step=remaining;
        displayed_score=wrapping_add(displayed_score,score_step);if(displayed_score>=game.score)score_step=0;
        if(displayed_score>=100000000&&before<100000000)score_digits[9].pending_interrupt=4;
    }
    if(game.high_score<displayed_score){game.high_score=displayed_score;game.high_score_units=game.score_units;game.flags|=4;}
    if(elapsed.current>=20){if(game.high_score>=100000000&&displayed_high_score<100000000)high_score_digits[9].pending_interrupt=4;displayed_high_score=game.high_score;}
    i32 high=game.high_score,score=displayed_score;
    for(u32 i=1;i<10;++i){env.bind_digit(*animations,high_score_digits[i],high%10+8);high/=10;env.bind_digit(*animations,score_digits[i],score%10+8);score/=10;env.update_animation(high_score_digits[i]);env.update_animation(score_digits[i]);}
    env.bind_digit(*animations,high_score_digits[0],wrapping_add(game.high_score_units,8));env.bind_digit(*animations,score_digits[0],wrapping_add(game.score_units,8));env.update_animation(high_score_digits[0]);env.update_animation(score_digits[0]);
    if(!(display_flags&0x20)){const auto* thresholds=game.difficulty==4?env.extra_extends:env.normal_extends;if(displayed_score>=thresholds[game.extend_index]){env.add_life();game.extend_index=wrapping_add(game.extend_index,1);}}
}
}
