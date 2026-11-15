#pragma once
#include "Th14LifeFragments.hpp"
namespace th11 {
#if PORT_TITLE==14
enum class HudCounterReadTh14:u32 {
    HighContinues,High,Continues,Displayed,AwardsWidth,AwardsLife,
    LifeFragments,AwardsRequirement,AwardsBomb,BombFragments,
    PowerInteger,StepInteger,PowerFraction,StepModulo,StepFraction,
    MaxPower,StepMaximum,PointValue,Graze
};
enum class HudCounterTextTh14:u32 {
    High,Score,Life,LifeRequirement,LifeUnavailable,Bombs,BombRequirement,
    BombUnavailable,PowerInteger,PowerFraction,PowerMaximum,PowerZeros,Points,Graze
};
// Retail431475..431896: read live words in instruction order. In particular,
// four award reads may disagree while the stage worker restores the globals.
// Emit is the ASCII queue boundary; neither callbacks nor expected game state
// are stored in the engine. Division guards retain the existing failure path.
template<class Read,class Emit> bool th14_hud_counter_words(Read&& read,Emit&& emit){
    using R=HudCounterReadTh14;using T=HudCounterTextTh14;
    const i32 high_continues=read(R::HighContinues),high=read(R::High);
    if(!emit(T::High,high,high_continues,620.f))return false;
    const i32 continues=read(R::Continues),displayed=read(R::Displayed);
    if(!emit(T::Score,displayed,continues,620.f))return false;
    const float life_x=th14_life_fragment_requirement(read(R::AwardsWidth))>=100?569.f:576.f;
    if(th14_life_fragment_requirement(read(R::AwardsLife))<999999){
        if(!emit(T::Life,read(R::LifeFragments),0,life_x))return false;
        if(!emit(T::LifeRequirement,th14_life_fragment_requirement(read(R::AwardsRequirement)),0,life_x+21.f))return false;
    }else if(!emit(T::LifeUnavailable,0,0,life_x))return false;
    if(th14_life_fragment_requirement(read(R::AwardsBomb))<999999){
        if(!emit(T::Bombs,read(R::BombFragments),0,576.f)||!emit(T::BombRequirement,8,0,597.f))return false;
    }else if(!emit(T::BombUnavailable,0,0,576.f))return false;
    const i32 power_integer=read(R::PowerInteger),step_integer=read(R::StepInteger);
    if(!step_integer||!emit(T::PowerInteger,power_integer/step_integer,0,540.f))return false;
    const i32 power_fraction=read(R::PowerFraction),step_modulo=read(R::StepModulo);
    if(!step_modulo)return false;
    const i32 fraction=signed_bits(u32(power_fraction%step_modulo)*100u),step_fraction=read(R::StepFraction);
    if(!step_fraction||!emit(T::PowerFraction,fraction/step_fraction,0,560.f))return false;
    const i32 maximum=read(R::MaxPower),step_maximum=read(R::StepMaximum);
    if(!step_maximum||!emit(T::PowerMaximum,maximum/step_maximum,0,574.f))return false;
    if(!emit(T::PowerZeros,0,0,606.f))return false;
    const i32 points=read(R::PointValue)/100;
    if(!emit(T::Points,points-points%10,0,620.f))return false;
    return emit(T::Graze,read(R::Graze),0,620.f);
}
#endif
}
