#include "GameHud.hpp"
namespace th15 {
bool GameHud::stage_clear(){chapter_notice=create(front,113);if(!chapter_notice)return false;clear_bonus=wrapping_mul(progress.stage,5000000);const u32 value=u32(score.score)+u32(clear_bonus/10);score.score=signed_bits(value>=1000000000u?999999999u:value);flags|=0x100;intro_age.set(0);return true;}
bool GameHud::prepare_spell(bool start){for(auto handle:boss_icons)if(!cue(handle,start?2:3,true))return false;return true;}
bool GameHud::reset_for_retry(){
 animations.registry.retire_resource(animations.resource(logo));for(auto& handle:result_banners)if(!check(animations.retire(handle)))return false;
 for(auto& handle:boss_stars)if(!check(animations.retire(handle)))return false;for(auto& handle:bonus_digits)if(!check(animations.retire(handle)))return false;boss.displayed_segments=0;
 for(u32 i=0;i<3;i++){auto& track=boss.health[i];track.segments[0].fraction=0;for(u32 j=2;j<6;j++)track.segments[j].fraction=0;if(!retire_boss(i,false))return false;}
 if(!check(animations.retire(background_notice)))return false;for(auto handle:boss_icons)if(!check(animations.pause(handle,true)))return false;last_countdown=-1;return true;
}
}
