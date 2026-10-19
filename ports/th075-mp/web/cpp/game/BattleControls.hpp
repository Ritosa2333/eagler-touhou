#pragma once
#include "BattleProgress.hpp"
#include "FighterActor.hpp"
#include "ReplayFile.hpp"
namespace th075 {
struct BattleSystemInput {std::array<u8,256> keys{};std::array<ControlCounts,2> players{};std::array<i32,2> cancel{};};
struct BattleControlSignals {virtual ~BattleControlSignals()=default;virtual void sound(u16)=0;virtual void toggle_fullscreen()=0;};
class BattleControls {
public:
    i32 escapeHeld=0,reduceLivesHeld=0,trainingKeyHeld=0,replayIntervalMs=16;
    bool recordingFastForward=false;
    void poll_pause(const BattleSystemInput&,BattleProgress&,GameMode,BattleControlSignals&);
    void pause_menu(const BattleSystemInput&,FighterActor&,FighterActor&,BattleProgress&,BattleSession&,BattleControlSignals&);
    void frame_events(const BattleSystemInput&,FighterActor&,FighterActor&,BattleProgress&,BattleSession&,ReplayStreams&,u32 replayFrame);
private:
    void training_menu(const BattleSystemInput&,FighterActor&,FighterActor&,BattleProgress&,BattleControlSignals&);
    void training_shortcuts(const BattleSystemInput&,FighterActor&,FighterActor&);
};
}
