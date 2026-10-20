#pragma once
#include "CharacterController.hpp"
#include "BattleContacts.hpp"
#include "FighterInputs.hpp"
#include "BodySeparation.hpp"
#include "SharedEffects.hpp"
namespace th075 {
struct BattleSimulationSignals {
    virtual ~BattleSimulationSignals()=default;
    virtual void sound(i16)=0;
    virtual void character_sound(u8 side,i16 id){sound(id);}
    virtual void combo(u8 side,i16 hits,i16 damage,i32 scale)=0;
    virtual void spell_attempt(u8 side,u8 card)=0;
    virtual void advance_stage()=0;
};
// The original battle frame is expressed in character and scene interfaces.
// No native addresses or native-memory layouts participate in gameplay.
class BattleSimulation {
    class Events;class Objects;
    BattleSimulationSignals& signals;
public:
    std::array<CharacterController*,2> characters;
    std::array<OwnedObjects,2> objects;
    ReplayStreams replay;SharedEffects effects;BattleContacts contacts;
    RoundRules rules;BodySeparation bodies;CharacterAIContext ai;StoryEnvironment story;
    GameMode mode=GameMode::Versus;u8 replayMode=0,difficulty=2;
    BattleSimulation(const std::array<CharacterController*,2>&,const EffectPattern&,BattleSimulationSignals&);
    void reset_fighter(u8 side);
    void update_inputs(const std::array<ControlCounts,2>&,const std::array<i32,2>& cancel);
    void advance(StoryEvents&);
private:
    void separate_bodies();void advance_counters();
};
}
