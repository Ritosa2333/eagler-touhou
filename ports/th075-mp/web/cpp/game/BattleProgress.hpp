#pragma once
#include "RoundRules.hpp"
namespace th075 {
enum class BattleExit:u16 {Stay=0,LoadBattle=0x2201,Title=0x2202,CharacterSelect=0x2203,ReplayMenu=0x2205,Result=0x2207,HighScore=0x220a,NextStoryStage=0xff06};
struct BattleSession {
    GameMode mode=GameMode::Versus;u8 replayMode=0;std::array<u8,2> characters{};
    i8 roundIndex=0,totalRounds=1,replayRound=0,continueCredits=0,replayDisposition=0;
    bool playbackDialogue=false,replayExhausted=false,continued=false;u8 restartFade=0;
    float overlayPulse=0,storyOverlay=0;u32 scoreToBeat=0;i32 maximumStoryBonus=0;
    std::array<std::array<i32,11>,11> victories{};
};
struct BattleFlowSignals {
    virtual ~BattleFlowSignals()=default;
    virtual void begin_round_recording()=0;virtual void banner(bool knockout)=0;
    virtual void fade_music(i32 target,i32 step,u32 interval,u32 completion)=0;
    virtual void stop_music(i32 step)=0;virtual void finish_replay_round()=0;
    virtual void dialogue_origin(float x,float y,u32 layout)=0;
    virtual void prepare_victory(RoundFighterState& winner)=0;
    virtual void show_victory_line(const std::string&,u8 alpha)=0;
    virtual void end_story_dialogue()=0;virtual void render_before_story_load()=0;
    virtual void release_battle_loader()=0;virtual void stage_story_completed()=0;
    virtual void reset_fighter(u32 side,RoundFighterState&)=0;
    virtual void reset_stage_effects()=0;virtual void advance_overlay()=0;
};
class BattleProgress {
public:
    explicit BattleProgress(RoundRules& state):rules(state){}
    RoundRules& rules;float victorySlide=0,textSlide=-300;
    i8 dialogueProgress=0,victoryLine=0;bool paused=false,trainingInstantRecovery=false;i8 pauseChoice=0;u8 pauseFade=0;
    i32 overlayTime=0,demoFrames=0;std::array<u8,2> healthFade{};std::array<std::array<i32,7>,2> savedInputs{};
    BattleExit advance(RoundFighterState&,RoundFighterState&,BattleSession&,BattleFlowSignals&);
private:
    void show_line(RoundFighterState&,BattleFlowSignals&);
};
}
