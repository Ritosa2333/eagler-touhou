#pragma once
#include "FighterActor.hpp"
#include "TitleScene.hpp"
#include "BattleProgress.hpp"
#include "ReplayFile.hpp"
#include "MenuDrawing.hpp"
namespace th075 {
struct ResultSignals:SceneSignals {virtual void render_result()=0;virtual void present_result()=0;virtual void select_music(i16)=0;};
class ResultScene {
public:
 i32 payoutStep=0,base=0,combo=0,capture=0,lives=0,pending=0,timer=0;float backgroundScale=1,comboMultiplier=1,captureMultiplier=1,lifeMultiplier=1;u8 alpha=0,soundCycle=0,phase=0;std::array<i16,5> slide{900,900,900,900,1800};
 void initialize(FighterActor&,const FighterActor&,const RoundRules&,const BattleSession&,i16 sessionCombo);
 u16 advance(bool confirm,FighterActor&,RoundRules&,BattleSession&,u8 difficulty,u8 nextStage,u8 replayStages,ResultSignals&,bool practice=false);
 void render(ImageRenderer&,u32 captureTexture,const FighterActor& first,u8 difficulty)const;
};
}
