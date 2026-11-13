#pragma once
#include "PowerAddWordsTh14.hpp"
namespace th11 {
#if PORT_TITLE==14
enum class ContinueWordTh14:u32 {Score,Continues,Power,Maximum,Step,Lives,LifeFragments,Bombs,BombFragments};
enum class ContinueAccessTh14:u32 {
    SetLives,SetLifeFragments,SetBombs,InitialBombFragments,InitialBombs,
    Maximum,Step,ClearBombFragments,NegativePower,NonnegativePower,
    AddInitial,AddMaximum,AddSum,AddClamp,PowerNotice,AddFinal,AddStep,
    Options,LifeFragments,Lives,DisplayLives,BombFragments,Bombs,DisplayBombs,
    Continues,SetContinues,ClearScore
};
// 44ae10..44aee9: preserve both argument read order and what each callback
// observes. Credit ownership and the modal/respawn tail remain in the caller.
template<class Read,class Write,class Action> inline bool th14_continue_words(Read&& read,Write&& write,Action&& action){
    using A=ContinueAccessTh14;using W=ContinueWordTh14;
    write(A::SetLives,W::Lives,i32(2));write(A::SetLifeFragments,W::LifeFragments,i32(0));write(A::SetBombs,W::Bombs,i32(2));
    const i32 initial_fragments=read(A::InitialBombFragments,W::BombFragments);
    if(!action(A::InitialBombs,2,initial_fragments))return false;
    const i32 maximum=read(A::Maximum,W::Maximum),step=read(A::Step,W::Step);
    write(A::ClearBombFragments,W::BombFragments,i32(0));
    if(maximum<0)write(A::NegativePower,W::Power,maximum);
    else write(A::NonnegativePower,W::Power,step>0?step:0);
    bool changed=false;
    if(!th14_add_power_words(signed_bits(u32(step)*4u),changed,[&](PowerAddAccessTh14 access){
        switch(access){
        case PowerAddAccessTh14::Initial:return read(A::AddInitial,W::Power);
        case PowerAddAccessTh14::Maximum:return read(A::AddMaximum,W::Maximum);
        case PowerAddAccessTh14::Final:return read(A::AddFinal,W::Power);
        default:return read(A::AddStep,W::Step);
        }
    },[&](PowerAddAccessTh14 access,i32 value){write(access==PowerAddAccessTh14::StoreSum?A::AddSum:A::AddClamp,W::Power,value);},
      [&](PowerAddAccessTh14){return action(A::PowerNotice,0,2);}))return false;
    if(!action(A::Options,0,0))return false;
    const i32 life_fragments=read(A::LifeFragments,W::LifeFragments),lives=read(A::Lives,W::Lives);
    if(!action(A::DisplayLives,lives,life_fragments))return false;
    const i32 bomb_fragments=read(A::BombFragments,W::BombFragments),bombs=read(A::Bombs,W::Bombs);
    if(!action(A::DisplayBombs,bombs,bomb_fragments))return false;
    const i32 continues=wrapping_add(read(A::Continues,W::Continues),1);
    write(A::SetContinues,W::Continues,continues>=10?9:continues);
    write(A::ClearScore,W::Score,i32(0));return true;
}
#endif
}
