#include "BattleProgress.hpp"
namespace th075 {
namespace {
i16 narrow(i32 value){const u16 bits=u16(value);i16 result;std::memcpy(&result,&bits,2);return result;}
void end_spells(RoundFighterState& a,RoundFighterState& b){a.combat.spellTimer=b.combat.spellTimer=0;a.combat.spellEnded=b.combat.spellEnded=0;}
void restore_spirit(RoundFighterState& f){f.combat.spirit=1000;f.combat.spiritBreak=0;}
}
void BattleProgress::show_line(RoundFighterState& winner,BattleFlowSignals& signals){
    const u32 line=u8(victoryLine);if(line<winner.victoryLines.size())signals.show_victory_line(winner.victoryLines[line],255);
    if(line<winner.victoryPortraits.size())winner.noticePortrait=winner.victoryPortraits[line];
}
BattleExit BattleProgress::advance(RoundFighterState& a,RoundFighterState& b,BattleSession& session,BattleFlowSignals& signals){
    auto& r=rules;const std::array<RoundFighterState*,2> fighters{&a,&b};const bool story=session.mode==GameMode::Story;
    if(r.highScore<u32(a.score))r.highScore=u32(a.score);
    const auto fade=[&](i32 step,u32 completion){signals.fade_music(-10000,step,1,completion);};
    const auto normalize_replay=[&]{if(session.replayMode==4){session.replayMode=2;session.playbackDialogue=true;}else if(session.replayMode==3)session.replayMode=1;};
    switch(r.phase){
        case BattlePhase::Intro:
            if(victorySlide<0)victorySlide=float(double(victorySlide)+5);
            if(r.countdown==150){if(!story)signals.begin_round_recording();signals.banner(false);}if(r.countdown==20)r.phase=BattlePhase::Active;break;
        case BattlePhase::RoundKnockout:if(r.countdown==300)signals.banner(true);if(r.countdown==10){r.phase=BattlePhase::RoundRecovery;r.countdown=160;}break;
        case BattlePhase::MatchKnockout:
            if(r.countdown==180)signals.banner(true);
            if(r.countdown==10){
                end_spells(a,b);
                if(session.replayMode==2){if(session.roundIndex!=session.replayRound&&session.mode!=GameMode::Versus&&session.mode!=GameMode::VersusCpu){fade(-200,0);return BattleExit::LoadBattle;}r.phase=BattlePhase::Exit;}
                else{victoryLine=0;signals.dialogue_origin(35,360,1);signals.prepare_victory(*fighters[r.winner]);show_line(*fighters[r.winner],signals);const u8 winner=session.characters[r.winner],loser=session.characters[1-r.winner];if(winner<11&&loser<11)session.victories[winner][loser]=increment_wrapped(session.victories[winner][loser],1);r.phase=BattlePhase::VictoryText;r.countdown=160;}
            }break;
        case BattlePhase::RoundRecovery:
            if(!a.transitionReady||!b.transitionReady)r.countdown=increment_wrapped(r.countdown,1);
            if(r.countdown==150){end_spells(a,b);signals.banner(false);}
            if(u32(r.countdown)<146){for(auto* f:fighters){f->combat.maximumHealth=narrow(i32(f->combat.maximumHealth)+600);if(f->combat.maximumHealth>9999){f->combat.maximumHealth=10000;f->combat.health=narrow(i32(f->combat.health)+200);if(f->combat.health>9999)f->combat.health=10000;}f->combat.spellCharge=f->combat.spellExtent=0;restore_spirit(*f);}}
            if(u32(r.countdown)<21){a.damageDisabled=b.damageDisabled=false;r.phase=BattlePhase::Active;}break;
        case BattlePhase::VictoryText:
            if(victorySlide>-200)victorySlide=float(double(victorySlide)-5);if(textSlide<0)textSlide=float(double(textSlide)+10);
            if(a.confirmCount==1||b.confirmCount==1){
                victoryLine=i8(u8(victoryLine)+1);auto& winner=*fighters[r.winner];
                if(i8(victoryLine)<i8(winner.victoryLines.size()))show_line(winner,signals);
                else if(session.mode==GameMode::Arcade){
                    if(!a.damageDisabled){
                        if(session.replayMode!=2){signals.finish_replay_round();if(session.roundIndex==session.totalRounds){signals.stop_music(40);fade(-160,1);session.replayDisposition=-1;return BattleExit::Result;}fade(-200,0);return BattleExit::LoadBattle;}
                        if(session.roundIndex!=session.replayRound){fade(-200,0);return BattleExit::LoadBattle;}r.phase=BattlePhase::Exit;
                    }else if(session.replayMode==2)r.phase=BattlePhase::Exit;else{paused=true;pauseChoice=true;}
                }else if(session.replayMode==2)r.phase=BattlePhase::Exit;else{paused=true;pauseChoice=true;}
            }break;
        case BattlePhase::Exit:signals.stop_music(40);fade(-160,1);return session.replayMode==2||session.replayMode==4?BattleExit::ReplayMenu:BattleExit::CharacterSelect;
        case BattlePhase::StoryDialogue:
            if(a.dialogueFinished){a.dialogueFinished=false;dialogueProgress=i8(u8(dialogueProgress)+1);if(dialogueProgress==1){r.countdown=200;r.phase=BattlePhase::Intro;normalize_replay();}else if(dialogueProgress==2){fade(-160,0);signals.end_story_dialogue();signals.render_before_story_load();signals.release_battle_loader();signals.stage_story_completed();normalize_replay();if(session.replayMode==1)signals.finish_replay_round();return BattleExit::NextStoryStage;}}break;
        case BattlePhase::StoryResult:signals.stop_music(40);fade(-160,1);signals.finish_replay_round();return BattleExit::Result;
        case BattlePhase::StoryRecovery:
            if((!a.transitionReady||!b.transitionReady)&&u32(r.countdown)<25)r.countdown=increment_wrapped(r.countdown,1);
            if(r.countdown==150)signals.banner(false);
            if(u32(r.countdown)<146){
                if(a.damageDisabled){a.combat.maximumHealth=narrow(i32(a.combat.maximumHealth)+600);a.combat.health=narrow(i32(a.combat.health)+200);if(a.combat.maximumHealth>10000)a.combat.maximumHealth=10000;if(a.combat.health>10000)a.combat.health=10000;a.combat.spellCharge=0;restore_spirit(a);a.selectedSpell=2;}
                if(b.damageDisabled){const u32 current=b.combat.spellPhase;b.combat.maximumHealth=narrow(i32(b.combat.maximumHealth)+600);if(current<b.phaseMaximum.size()&&b.phaseMaximum[current]<=b.combat.maximumHealth){b.combat.maximumHealth=b.phaseMaximum[current];b.combat.health=narrow(i32(b.combat.health)+200);if(b.phaseMaximum[current]<b.combat.health)b.combat.health=b.phaseMaximum[current];}b.combat.spellCharge=0;b.combat.spellExtent=900;restore_spirit(b);a.combat.spellEnded=0;}
            }
            if(u32(r.countdown)<21){a.damageDisabled=b.damageDisabled=false;r.phase=BattlePhase::Active;}break;
        case BattlePhase::StoryPlayerKnockout:if(r.countdown==300)signals.banner(true);if(r.countdown==10){r.phase=BattlePhase::StoryRecovery;r.countdown=160;}break;
        case BattlePhase::StoryStageKnockout:
            if(r.countdown==180)signals.banner(true);
            if(r.countdown==10){end_spells(a,b);if(session.replayMode==2){if(!a.damageDisabled){r.phase=BattlePhase::StoryDialogue;session.replayMode=4;session.playbackDialogue=true;}else r.phase=BattlePhase::Exit;}
                else if(!a.damageDisabled){r.phase=BattlePhase::StoryDialogue;if(session.replayMode==1)session.replayMode=3;}
                else{r.phase=BattlePhase::Continue;pauseChoice=session.continueCredits<1;r.countdown=100;}}
            break;
        case BattlePhase::Continue:if(session.replayMode==2)r.phase=BattlePhase::Exit;else if(u32(r.countdown)<2)paused=true;break;
        case BattlePhase::StoryRestart:
            if(r.countdown==300)signals.banner(true);
            else if(u32(r.countdown)<200&&u32(r.countdown)>149){
                session.restartFade=u8(i32(255.0-double(u32(r.countdown)-150u)*double(5.1f)));session.storyOverlay=0;
                if(r.countdown==150){b.combat.spellValue=b.combat.spellFrames=0;b.combat.spellTimer=0;b.combat.spellEnded=0;for(u32 side=0;side<2;side++){signals.reset_fighter(side,*fighters[side]);fighters[side]->damageDisabled=true;fighters[side]->motion.x=float(side*320+480);fighters[side]->motion.y=0;}a.combat.spellPhase=2;b.combat.spellPhase=b.storyPhaseCount;const u32 p=b.combat.spellPhase;b.combat.maximumHealth=1;if(p<b.phaseMaximum.size())b.combat.maximumHealth=b.phaseMaximum[p];signals.reset_stage_effects();session.maximumStoryBonus=0;for(u32 i=1;i<=b.storyPhaseCount&&i<b.phaseBonuses.size();i++)session.maximumStoryBonus=increment_wrapped(session.maximumStoryBonus,b.phaseBonuses[i]);}
            }else if(u32(r.countdown)<50){session.restartFade=u8(i32(double(u32(r.countdown))*double(5.1f)));if(r.countdown==0){session.restartFade=0;r.phase=BattlePhase::StoryRecovery;r.countdown=160;}}break;
        case BattlePhase::ReturnToMenu:signals.stop_music(40);fade(-160,1);if(session.replayMode==2||session.replayMode==4)return BattleExit::ReplayMenu;if(session.replayDisposition!=0)signals.finish_replay_round();if(story&&session.scoreToBeat<u32(a.score))return BattleExit::HighScore;return BattleExit::CharacterSelect;
        default:break;
    }
    if(overlayTime>0)signals.advance_overlay();r.advance_spell_presentation(a,b);
    if(trainingInstantRecovery){for(auto* f:fighters)if(f->combat.action<50){f->combat.health=f->combat.maximumHealth=10000;f->combat.spellCharge=1000;f->combat.spellEnded=0;f->selectedSpell=2;}}
    if(session.mode==GameMode::Training){for(auto* f:fighters)if(f->combat.action<50){f->combat.health=f->combat.maximumHealth=10000;f->combat.spellExtent=float(i32(i8(b.trainingSpell))*100);f->combat.spellEnded=0;f->selectedSpell=2;}}
    if(session.mode==GameMode::Demonstration){demoFrames=increment_wrapped(demoFrames,1);if(u32(demoFrames)>3600){signals.stop_music(40);fade(-160,1);return BattleExit::Title;}}
    if(r.countdown!=0)r.countdown=increment_wrapped(r.countdown,-1);if(r.bonusTimer>0)--r.bonusTimer;
    if(session.overlayPulse>0){session.overlayPulse=float(double(session.overlayPulse)-double(0.01f));if(session.overlayPulse<0)session.overlayPulse=0;}
    if(a.combat.spellPhase==2)healthFade[0]=128;else if(healthFade[0]!=255)healthFade[0]=u8(healthFade[0]+1);
    if(!story){if(b.combat.spellPhase==2)healthFade[1]=128;else if(healthFade[1]!=255)healthFade[1]=u8(healthFade[1]+1);}
    if(session.replayMode==2&&session.replayExhausted){if(story||session.mode==GameMode::Arcade){if(session.replayRound<=session.roundIndex)r.phase=BattlePhase::Exit;}else r.phase=BattlePhase::Exit;}
    if(paused||pauseFade){savedInputs[0]=a.rawInputs;savedInputs[1]=b.rawInputs;}return BattleExit::Stay;
}
}
