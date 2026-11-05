#include "RoundRules.hpp"
namespace th075 {
namespace {
i16 narrow(i32 v){const u16 bits=u16(v);i16 result;std::memcpy(&result,&bits,2);return result;}
void end_spells(RoundFighterState& a,RoundFighterState& b){a.combat.spellTimer=b.combat.spellTimer=0;a.combat.spellEnded=b.combat.spellEnded=0;}
void lose_round(RoundFighterState& loser,RoundFighterState& winner){loser.combat.spellPhase=u8(loser.combat.spellPhase-1);loser.transitionReady=false;loser.damageDisabled=true;winner.damageDisabled=false;}
}
void RoundRules::detect_knockout(RoundFighterState& a,RoundFighterState& b,GameMode mode,u8 replayMode){
    if(phase!=BattlePhase::Active)return;
    if(mode==GameMode::Story){
        if(b.combat.health<1){
            a.damageDisabled=false;b.damageDisabled=true;
            if(b.combat.spellValue>0){
                if(replayMode==1&&b.combat.spellPhase<b.cardIds.size()){const u8 id=b.cardIds[b.combat.spellPhase];if(id<storyCards.size()){auto& record=storyCards[id];if(a.perfectSpell)record.perfect=true;record.captures=narrow(i32(record.captures)+1);if(record.maximumScore<b.combat.spellValue)record.maximumScore=b.combat.spellValue;}}
                awardedSpellTime=increment_wrapped(awardedSpellTime,b.combat.spellFrames);a.score=increment_wrapped(a.score,b.combat.spellValue);if(highScore<u32(a.score))highScore=u32(a.score);bonusTimer=120;bonusValue=b.combat.spellValue;b.combat.spellValue=0;
            }
            if(a.combat.spellTimer>0)a.combat.spellExtent=0;end_spells(a,b);b.combat.spellPhase=u8(b.combat.spellPhase-1);b.transitionReady=false;b.combat.maximumHealth=0;a.perfectSpell=true;if(a.combat.health<1)a.combat.health=1;
            if(i8(b.combat.spellPhase)<1){phase=BattlePhase::StoryStageKnockout;countdown=180;freezeFrames=60;}
            else{phase=BattlePhase::StoryRecovery;countdown=147;const u32 current=b.combat.spellPhase;if(current<b.phaseMaximum.size()&&current+1<b.phaseKinds.size()&&b.phaseKinds[current+1]==0){b.combat.health=b.combat.maximumHealth=b.phaseMaximum[current];b.combat.spellFrames=0;}}
        }else if(a.combat.health<1){
            const u32 p=b.combat.spellPhase;if(p<b.phaseMaximum.size())b.combat.maximumHealth=b.phaseMaximum[p];a.combat.spellTimer=0;a.combat.spellEnded=0;a.combat.spellPhase=u8(a.combat.spellPhase-1);a.damageDisabled=true;b.damageDisabled=false;a.transitionReady=false;a.perfectSpell=false;
            if(i8(a.combat.spellPhase)==0){if(remainingLives<1){phase=BattlePhase::StoryStageKnockout;countdown=180;freezeFrames=60;}else{--remainingLives;a.combat.spellPhase=2;phase=BattlePhase::StoryPlayerKnockout;countdown=300;freezeFrames=60;}}
            else{a.combat.health=a.combat.maximumHealth=10000;phase=BattlePhase::StoryPlayerKnockout;countdown=300;freezeFrames=60;}
        }
        return;
    }
    if(a.combat.health>0&&b.combat.health>0)return;end_spells(a,b);freezeFrames=60;
    if(a.combat.health<1&&b.combat.health<1){a.combat.spellPhase=b.combat.spellPhase=1;phase=BattlePhase::RoundKnockout;a.transitionReady=b.transitionReady=false;a.damageDisabled=b.damageDisabled=true;countdown=300;return;}
    if(a.combat.health<1){lose_round(a,b);if(i8(a.combat.spellPhase)==0){winner=1;phase=BattlePhase::MatchKnockout;countdown=180;}else{a.combat.health=a.combat.maximumHealth=10000;phase=BattlePhase::RoundKnockout;countdown=300;}}
    else{lose_round(b,a);if(i8(b.combat.spellPhase)==0){winner=0;phase=BattlePhase::MatchKnockout;countdown=180;}else{b.combat.health=b.combat.maximumHealth=10000;phase=BattlePhase::RoundKnockout;countdown=300;}}
}
void RoundRules::advance_spell_presentation(RoundFighterState& a,RoundFighterState& b){
    if(double(sceneFade)>double(0.33f)&&(a.combat.spellTimer>0||b.combat.spellTimer>0))sceneFade=float(double(sceneFade)-double(0.02f));
    const std::array<RoundFighterState*,2> fighters{&a,&b};
    for(u32 side=0;side<2;side++){
        auto& fighter=*fighters[side];if(fighter.combat.spellTimer<=0)continue;auto& elapsed=spellElapsed[side];auto& visual=spellPresentation[side];
        if(elapsed<80){
            if(elapsed==0){visual.portraitAlpha=15;visual.ringAlpha=visual.textAlpha=5;visual.angle=351;visual.spread=12;visual.scale=7;visual.velocity=20;visual.initialTime=fighter.combat.spellTimer;}
            if(visual.velocity>0&&elapsed>39){visual.angle=float(double(visual.angle)-double(visual.velocity));visual.velocity=float(double(visual.velocity)-double(0.75f));}
            if(visual.portraitAlpha!=255)visual.portraitAlpha=u8(visual.portraitAlpha+12);
            if(visual.ringAlpha!=255)visual.ringAlpha=u8(visual.ringAlpha+25);
            if(visual.textAlpha!=255&&elapsed>19)visual.textAlpha=u8(visual.textAlpha+25);
            if(visual.spread>0&&elapsed>29)visual.spread=float(double(visual.spread)-double(1.2f));
            if(visual.scale>1){visual.scale=float(double(visual.scale)-double(0.2f));if(visual.scale<1)visual.scale=1;}
        }
        elapsed=narrow(i32(elapsed)+1);
    }
}
}
