#include "ChapterReward.hpp"
namespace th15 {
bool ChapterReward::hide(i32 script){auto* parent=animations.registry.find(state.animation);if(!parent){error="Chapter reward animation is unavailable";return false;}auto* child=animations.registry.find_child_script(*parent,script,0);if(!child){error="Chapter reward child animation is unavailable";return false;}child->visual.flags&=~1u;child->instruction_offset=-1;return true;}
bool ChapterReward::complete(bool boss,i32 chapter_deaths){
    if(!enemies.chapter_total)return true;if(!animations.retire(state.animation)){error=animations.error;return false;}state.animation=animations.create(front,241,-1,0);if(!state.animation){error=animations.error;return false;}
    state.grazes=score.graze_chapter;const float ratio=float(float(enemies.chapter_defeated)/float(enemies.chapter_total));state.percent=float(ratio*100.f);state.display_percent=state.percent;state.display_bonus=0;
    const float value=float(float(float(state.grazes)*state.percent)*50.f);state.bonus=wrapping_mul(truncate_int(value)/10,10);state.base_bonus=state.percent>=1.f?truncate_int(float(float(state.bonus)/state.percent)):0;
    const u32 new_score=u32(score.score)+u32(state.bonus/10);score.score=signed_bits(new_score>=1000000000u?999999999u:new_score);const i32 increase=truncate_int(float(float(wrapping_mul(state.bonus/50000,10))*100.f));score.point_value=wrapping_add(score.point_value,increase);if(score.point_value>score.max_point_value)score.point_value=score.max_point_value;
    state.flags=(state.flags&~0x1000u)|0x800;state.duration=boss?220:290;state.age.set(boss?89:0);state.deaths=chapter_deaths;
    if(state.bonus>=1000000){ItemState item;const bool bomb_piece=(player.mode_flags&0x300)!=0;item.kind=bomb_piece?6:4;if(!collection.award(item)){error=collection.error;return false;}return hide(bomb_piece?243:242);}
    return hide(242)&&hide(243);
}
}
