#include "StageConfiguration.hpp"
namespace th17 {
namespace {
const std::array<StageConfiguration,7>& configurations(){
    static const auto values=[](){
        std::array<StageConfiguration,7> result;
        static constexpr const char* stage_music[]={"th17_03","th17_05","th17_07","th17_09","th17_11","th17_14","th17_17"};
        static constexpr const char* boss_music[]={"th17_04","th17_06","th17_08","th17_10","th17_12","th17_13","th17_18"};
        static constexpr StageVisualSet visuals[7][3]={
            {{3,10,0,3,18,3,15,3,9,0},{3,10,0,3,18,-1,-1,-1,-1,-1},{}},
            {{3,11,0,3,19,3,16,3,10,1},{3,11,0,3,19,-1,-1,-1,-1,-1},{}},
            {{3,10,0,3,17,3,14,3,9,2},{3,10,0,-1,-1,-1,-1,-1,-1,-1},{}},
            {{3,9,0,3,16,3,13,3,8,3},{3,9,0,-1,-1,-1,-1,-1,-1,-1},{}},
            {{3,13,0,3,21,3,18,3,12,4},{3,13,0,-1,-1,-1,-1,-1,-1,-1},{}},
            {{3,13,0,3,20,3,17,3,12,5},{-1,-1,0,-1,-1,-1,-1,-1,-1,4},{}},
            {{3,10,0,3,19,3,16,3,9,6},{4,10,1,4,17,-1,-1,4,9,2},{3,11,1,3,19,3,16,3,9,6}}
        };
        for(i32 index=0;index<7;index++){
            auto& value=result[index];value.stage=index+1;const std::string prefix="st0"+std::to_string(value.stage);
            value.background=prefix+".std";value.enemy_script=prefix+".ecl";value.stage_music=stage_music[index];value.boss_music=boss_music[index];value.logo=prefix+"logo.anm";
            value.stage_music_index=index*2+1;value.boss_music_index=index*2+2;
            for(i32 loadout=0;loadout<9;loadout++)value.messages[loadout]=prefix+char('a'+loadout)+".msg";
            // Unused native visual rows are zero-filled. They are retained as
            // data because scene scripts select a row explicitly.
            value.visuals.fill({0,0,0,0,0,0,0,0,0,0});
            for(i32 row=0;row<(index==6?3:2);row++)value.visuals[row]=visuals[index][row];
        }return result;
    }();return values;
}
}
const StageConfiguration* StageConfiguration::get(i32 stage)noexcept{return stage>=1&&stage<=7?&configurations()[stage-1]:nullptr;}
const std::string* StageConfiguration::message(i32 character,i32 animal)const noexcept{return u32(character)<3&&u32(animal)<3?&messages[character*3+animal]:nullptr;}
}
