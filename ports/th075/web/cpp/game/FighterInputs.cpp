#include "FighterInputs.hpp"
namespace th075 {
void update_character_input(CharacterController& character,const FighterActor& opponent,const ControlCounts& live,GameMode mode,ReplayStreams& replay,CharacterAIContext& ai,u8 difficulty,i32 cancelCount){
    auto& a=character.actor;
    if(!a.combat.storyOpponent){
        if(replay.mode==ReplayMode::Playback){a.controls.raw.horizontal=a.rawInputs[0];a.controls.raw.vertical=a.rawInputs[1];for(u32 i=0;i<4;i++)a.controls.raw.buttons[i]=a.rawInputs[2+i];a.controls.replay_frame(replay.input(0),a.motion.facing,a.animation.cursor.action);a.rawInputs[0]=a.controls.raw.horizontal;a.rawInputs[1]=a.controls.raw.vertical;for(u32 i=0;i<4;i++)a.rawInputs[2+i]=a.controls.raw.buttons[i];character.recognize_inputs();}
        else{a.rawInputs[6]=cancelCount;replay.input(live.held());character.player_input(live);}
        a.confirmCount=a.controls.current.buttons[0];
    }else{
        a.controls.buffered.clear();a.controls.current.clear();a.command=0;
        if(replay.mode==ReplayMode::Playback){
            const u8 held=replay.input(0);a.command=replay.auxiliary(0);a.commandTimers[2]=replay.control_timer(a.commandTimers[2]);
            // CPU recordings carry one-frame choices. Opposite direction bits
            // use the original CPU branch's positive-bit priority.
            a.controls.current.horizontal=held&2?1:held&1?-1:0;a.controls.current.vertical=held&8?1:held&4?-1:0;
            for(u32 i=0;i<4;i++)a.controls.current.buttons[i]=(held&(16u<<i))?1:0;
        }else if(replay.mode==ReplayMode::Recording){
            if(mode==GameMode::Training){
                switch(a.training.direction){case 0:a.controls.current.vertical=0;break;case 1:a.controls.current.vertical=1;break;case 2:a.controls.current.vertical=-1;break;case 3:character.choose_ai_input(opponent,false,replay,ai);break;}
                if(a.training.direction!=3){if(i8(a.training.position)<1)a.controls.current.horizontal=0;else{if(a.motion.x<620)a.controls.current.horizontal=1;if(a.motion.x>660)a.controls.current.horizontal=-1;}}
            }else character.choose_ai_input(opponent,mode==GameMode::Story,replay,ai);
            if(difficulty<2&&replay.random.next()>u32(difficulty*3+1)*2048){a.controls.current.buttons.fill(0);a.command=0;}
            for(auto& b:a.controls.current.buttons)if(b!=0)b=1;
            replay.input(a.controls.current.held());replay.auxiliary(a.command);replay.control_timer(a.commandTimers[2]);
        }
        a.confirmCount=0;
    }
}
void restrict_battle_input(FighterActor& a,BattlePhase phase){
    if(phase==BattlePhase::Active)return;
    for(u32 i=0;i<3;i++)a.controls.current.buttons[i]=0;a.controls.buffered.clear();
    switch(phase){
        case BattlePhase::Intro:case BattlePhase::RoundKnockout:case BattlePhase::MatchKnockout:case BattlePhase::VictoryText:case BattlePhase::StoryDialogue:case BattlePhase::StoryPlayerKnockout:case BattlePhase::StoryStageKnockout:case BattlePhase::StoryRestart:
            a.controls.current.clear();a.reset_command_history();a.command=0;break;
        default:break;
    }
    if(a.command>9||a.combat.storyOpponent)a.command=0;
}
}
