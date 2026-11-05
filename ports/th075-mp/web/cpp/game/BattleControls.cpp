#include "BattleControls.hpp"
namespace th075 {
namespace {
void training_spell_selection(FighterActor& player,const FighterActor& opponent){const i32 guard=i8(opponent.training.guard);player.combat.spellPhase=u8(2-guard/3);player.spellChoices.fill(i16(guard%3));}
void replay_event(u32 id,FighterActor& a,FighterActor& b,BattleProgress& progress){
    switch(id){
        case 1:b.combat.health=1;break;
        case 2:a.combat.spellPhase=b.combat.spellPhase=2;break;
        case 3:if(i8(a.combat.spellPhase)>1)--a.combat.spellPhase;if(i8(b.combat.spellPhase)>1)--b.combat.spellPhase;break;
        case 4:progress.trainingInstantRecovery=false;break;
        case 5:progress.trainingInstantRecovery=true;break;
        case 6:for(auto* p:{&a,&b}){p->combat.spellExtent=float(double(p->combat.spellExtent)+10.0);if(p->combat.spellExtent>900)p->combat.spellExtent=900;}break;
        case 7:for(auto* p:{&a,&b}){p->combat.spellExtent=float(double(p->combat.spellExtent)-10.0);if(p->combat.spellExtent<0)p->combat.spellExtent=0;}break;
        default:break;
    }
}
}
void BattleControls::poll_pause(const BattleSystemInput& input,BattleProgress& progress,GameMode mode,BattleControlSignals& signals){
    if(input.keys[0x1c]&&(input.keys[0x38]||input.keys[0xb8]))signals.toggle_fullscreen();
    escapeHeld=input.keys[1]?increment_wrapped(escapeHeld,1):0;
    const auto phase=progress.rules.phase;
    if((escapeHeld==3||input.cancel[0]==1||input.cancel[1]==1)&&phase!=BattlePhase::VictoryText&&phase!=BattlePhase::Intro&&phase!=BattlePhase::StoryDialogue&&mode!=GameMode::Demonstration){progress.paused=!progress.paused;if(progress.paused)progress.pauseChoice=0;signals.sound(48);}
}
void BattleControls::pause_menu(const BattleSystemInput& input,FighterActor& a,FighterActor& b,BattleProgress& progress,BattleSession& session,BattleControlSignals& signals){
    if(session.mode==GameMode::Training){training_menu(input,a,b,progress,signals);return;}
    auto& phase=progress.rules.phase;
    if(input.players[0].buttons[0]==1||input.players[1].buttons[0]==1){
        progress.paused=false;signals.sound(45);
        switch(progress.pauseChoice){
            case 0:if(phase==BattlePhase::Continue&&session.continueCredits>0){a.combat.spellPhase=2;a.score=0;--session.continueCredits;session.continued=true;progress.rules.remainingLives=2;phase=BattlePhase::StoryRestart;progress.rules.countdown=299;}break;
            case 1:if(phase==BattlePhase::Continue){session.replayDisposition=0;phase=BattlePhase::ReturnToMenu;}else phase=BattlePhase::Exit;break;
            case 2:if(phase==BattlePhase::Continue){session.replayDisposition=1;phase=BattlePhase::ReturnToMenu;}else phase=BattlePhase::StoryResult;break;
            default:break;
        }
    }
    if(phase!=BattlePhase::VictoryText&&phase!=BattlePhase::Continue&&(input.players[0].buttons[1]==1||input.players[1].buttons[1]==1))progress.paused=false;
    if(input.players[0].vertical==1||input.players[1].vertical==1){signals.sound(39);progress.pauseChoice=i8(u8(progress.pauseChoice)+1);}
    if(input.players[0].vertical==-1||input.players[1].vertical==-1){signals.sound(39);progress.pauseChoice=i8(u8(progress.pauseChoice)-1);}
    if(session.replayMode==2){if(progress.pauseChoice<0)progress.pauseChoice=1;if(progress.pauseChoice>1)progress.pauseChoice=0;}
    if(phase==BattlePhase::VictoryText){if(progress.pauseChoice<1)progress.pauseChoice=2;if(progress.pauseChoice>2)progress.pauseChoice=1;}
    else if(phase==BattlePhase::Continue&&session.continueCredits==0)progress.pauseChoice=1;
    else if(session.mode==GameMode::Story&&(session.continued||phase==BattlePhase::StoryDialogue)){if(progress.pauseChoice<0)progress.pauseChoice=1;if(progress.pauseChoice>1)progress.pauseChoice=0;}
    else{if(progress.pauseChoice<0)progress.pauseChoice=2;if(progress.pauseChoice>2)progress.pauseChoice=0;}
}
void BattleControls::training_menu(const BattleSystemInput& input,FighterActor& a,FighterActor& b,BattleProgress& progress,BattleControlSignals& signals){
    if(!progress.paused)return;
    const i32 horizontal=input.players[0].horizontal;
    if(horizontal==1||horizontal==-1){
        signals.sound(39);const auto adjust=[&](u8& value,i32 count){value=u8((i32(i8(value))+count+horizontal)%count);};
        switch(progress.pauseChoice){case 0:adjust(b.training.guard,6);break;case 1:adjust(b.trainingSpell,10);break;case 2:adjust(b.training.counter,4);break;case 3:adjust(b.training.recovery,2);break;case 4:adjust(b.training.direction,4);break;case 5:adjust(b.training.position,2);break;default:break;}
    }
    if(input.players[0].buttons[0]==1){switch(progress.pauseChoice){case 6:a.combat.spellTimer=1;signals.sound(45);break;case 7:progress.paused=false;signals.sound(45);break;case 8:progress.paused=false;signals.sound(45);progress.rules.phase=BattlePhase::Exit;break;default:break;}}
    if(input.players[0].buttons[1]==1)progress.paused=false;
    if(input.players[0].vertical==1||input.players[1].vertical==1){signals.sound(39);progress.pauseChoice=i8((i32(progress.pauseChoice)+1)%9);}
    if(input.players[0].vertical==-1||input.players[1].vertical==-1){signals.sound(39);progress.pauseChoice=i8((i32(progress.pauseChoice)+8)%9);}
    training_spell_selection(a,b);
}
void BattleControls::training_shortcuts(const BattleSystemInput& input,FighterActor& a,FighterActor& b){
    u32 key=0;while(key<8&&!input.keys[0x3b+key])++key;
    if(key==8)trainingKeyHeld=0;
    else{if(trainingKeyHeld==1){switch(key){case 0:b.training.guard=u8((i32(i8(b.training.guard))+1)%6);break;case 1:b.trainingSpell=u8((i32(i8(b.trainingSpell))+9)%10);break;case 2:b.trainingSpell=u8((i32(i8(b.trainingSpell))+1)%10);break;case 3:b.training.counter=u8((i32(i8(b.training.counter))+1)%4);break;case 4:b.training.recovery=u8((i32(i8(b.training.recovery))+1)%2);break;case 5:b.training.direction=u8((i32(i8(b.training.direction))+1)%4);break;case 6:b.training.position=u8((i32(i8(b.training.position))+1)%2);break;case 7:a.combat.spellTimer=1;break;default:break;}}trainingKeyHeld=increment_wrapped(trainingKeyHeld,1);}
    training_spell_selection(a,b);
}
void BattleControls::frame_events(const BattleSystemInput& input,FighterActor& a,FighterActor& b,BattleProgress& progress,BattleSession& session,ReplayStreams& replay,u32 replayFrame){
    if(session.mode==GameMode::Training){training_shortcuts(input,a,b);return;}
    if(session.replayMode==2){
        if(replay.round){const auto& events=replay.round->events;while(replay.eventCursor+1<events.size()&&events[replay.eventCursor]==replayFrame){const u32 id=events[replay.eventCursor+1];replay.eventCursor+=2;replay_event(id,a,b,progress);}}
    }else{
        if(!input.keys[0x3d])reduceLivesHeld=0;
        if(session.mode!=GameMode::Story&&session.mode!=GameMode::Arcade){
            const auto apply=[&](u32 id){if(session.replayMode==1)replay.append_event(replayFrame,id);replay_event(id,a,b,progress);};
            if(input.keys[0x3c])apply(2);
            if(input.keys[0x3d]){if(reduceLivesHeld==0)apply(3);reduceLivesHeld=increment_wrapped(reduceLivesHeld,1);}
            for(u32 key=0x3e;key<=0x41;key++)if(input.keys[key])apply(key-0x3a);
        }
    }
    const bool control=input.keys[0x1d]||input.keys[0x9d];
    if(session.replayMode==2&&control){const std::array<i32,6> speeds{8,6,4,3,2,1};u32 key=0;while(key<speeds.size()&&!input.keys[3+key])++key;replayIntervalMs=key<speeds.size()?speeds[key]:16;}
    if(session.replayMode==1||session.replayMode==3)recordingFastForward=control||u32(input.players[0].buttons[3])>=6;
}
}
