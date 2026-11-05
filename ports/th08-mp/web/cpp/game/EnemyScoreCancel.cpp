#include "EnemyPopulation.hpp"
#include "EnemyScoreCancel.hpp"
#include "EnemyFamiliars.hpp"
namespace th08 {
bool EnemyPopulation::cancel_for_score(i32 maximum,i32& score,EnemyScoreActions& actions){
    i32 value=2000;
    const auto reward=[&](const Vec3& position){
        actions.item(position,6,1);actions.score_popup(position,value,value<maximum?0xffffffffu:0xffffff00u);
        score=wrapping_add(score,value);value=wrapping_add(value,30);if(value>maximum)value=maximum;
    };
    for(u32 index=0;index<480;++index)if(auto* enemy=at(index);enemy&&(enemy->flags&1)&&!(enemy->flags&2)&&!(enemy->flags2&0x40)){
        enemy->life=0;
        if(enemy->flags&0x80){
            enemy->refresh_position();reward(enemy->resolved_position);
            if(enemy->trail.flags){if(enemy->trail.length>96){enemy->invalid=true;return false;}for(i32 i=0;i<enemy->trail.length;i+=6)reward(enemy->trail.points[i].position);}
        }
        unlink_familiar(*enemy);if(enemy->invalid)return false;
        if(enemy->death_subroutine>=0){if(!executor.switch_main(*enemy,enemy->death_subroutine))return false;enemy->death_subroutine=-1;}
    }
    return true;
}
}
