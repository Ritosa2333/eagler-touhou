#pragma once
#include "BattleFrame.hpp"
#include "BattleControls.hpp"
#include "BattleSimulation.hpp"
namespace th075 {
class BattleScene;
struct BattleScenePlatform:BattleSimulationSignals,BattleControlSignals,BattleFlowSignals {
    void sound(u16 id)override{sound(i16(id));}
    virtual void sound(i16)override=0;
    virtual void render_battle(BattleScene&,bool stageGeometry)=0;
    virtual void present_battle()=0;
};
class BattleScene {
    class Frame;class Flow;
    BattleScenePlatform& platform;
public:
    BattleSimulation simulation;BattleProgress progress;BattleSession session;
    BattleControls controls;BattleFrameState timing;u8 foreground=0;
    BattleScene(const std::array<CharacterController*,2>&,const EffectPattern&,BattleScenePlatform&);
    void initialize(const BattleSession&);
    BattleExit advance(const BattleSystemInput&,StoryEvents&);
    void render();
};
}
