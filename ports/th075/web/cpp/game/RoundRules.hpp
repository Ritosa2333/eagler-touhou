#pragma once
#include "GameMode.hpp"
#include "FighterCombat.hpp"
#include "BattleGeometry.hpp"
namespace th075 {
enum class BattlePhase:u8 {Intro=0,Active=1,RoundKnockout=2,MatchKnockout=3,RoundRecovery=4,VictoryText=5,Exit=6,StoryDialogue=7,StoryResult=8,StoryRecovery=9,StoryPlayerKnockout=10,StoryStageKnockout=11,Continue=12,StoryRestart=13,ReturnToMenu=14};
struct StoryCardRecord {i32 maximumScore=0;i16 captures=0,attempts=0;bool perfect=false;};
struct RoundFighterState {
    FighterCombat combat;FighterMotion motion;
    i32 score=0,confirmCount=0;bool damageDisabled=false,transitionReady=true,perfectSpell=false,dialogueFinished=false;
    u8 selectedSpell=2,storyPhaseCount=2,trainingSpell=0;std::array<i32,7> rawInputs{};
    std::vector<std::string> victoryLines;std::vector<i8> victoryPortraits;i8 noticePortrait=-1;std::array<i32,12> phaseBonuses{};
    std::array<i16,12> phaseMaximum{};std::array<u8,13> phaseKinds{};std::array<u8,12> cardIds{};
};
struct SpellPresentation {
    u8 portraitAlpha=0,ringAlpha=0,textAlpha=0;
    float scale=0,angle=0,velocity=0,spread=0;i32 initialTime=0;
};
struct RoundRules {
    BattlePhase phase=BattlePhase::Intro;i32 countdown=200;i16 freezeFrames=0;
    u8 winner=0;float sceneFade=1;i32 bonusValue=0,bonusTimer=0;
    std::array<SpellPresentation,2> spellPresentation{};std::array<i16,2> spellElapsed{};
    u32 highScore=0;i32 awardedSpellTime=0;i8 remainingLives=0;
    std::array<StoryCardRecord,128> storyCards{};
    void detect_knockout(RoundFighterState&,RoundFighterState&,GameMode,u8 replayMode);
    void advance_spell_presentation(RoundFighterState&,RoundFighterState&);
};
}
