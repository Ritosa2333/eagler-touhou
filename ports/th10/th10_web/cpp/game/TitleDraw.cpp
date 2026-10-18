#include "TitleDraw.hpp"
namespace th10 {
// 0x431410. The stage high scores share the original eight-byte statistics
// slots with the unlock flags. The chosen row blinks during confirmation.
i32 draw_practice_stages(const TitleMenu& title,ResultsDrawEnvironment& env){
    if(title.phase<2||title.phase>3)return 1;
    *env.text_mode=1;
    if(title.elapsed.current>=10||title.phase==3){
        Vec3 position{env.game->character?296.0f:168.0f,216,0};
        for(i32 stage=1;stage<7;++stage){
            const auto& game=*env.game;const auto* record=(*env.scores)->characters[game.character*3+game.shot_type].statistics;
            const auto* slot=record+0x14+(game.difficulty*6+stage)*8;
            const bool unlocked=slot[5]!=0;
            *env.color=title.menu.selected!=stage-1?0xff808080u:!unlocked?0xffdfdfdfu:title.phase==3&&title.elapsed.current%4>=2?0xff000000u:0xffffff00u;
            const u32 name=reinterpret_cast<uintptr_t>(env.stage_names[stage]);
            if(!unlocked)env.print(position,"%s  ---------",{name});
            else{u32 score;__builtin_memcpy(&score,slot,4);env.print(position,"%s  %.8d0",{name,score});}
            position.y=Scalar::add(position.y,18.0f);
        }
    }
    *env.color=0xffffffff;*env.text_mode=0;return 1;
}
}
