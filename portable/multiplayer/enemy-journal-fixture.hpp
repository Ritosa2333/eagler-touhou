#pragma once
#ifndef TH_MULTIPLAYER_FIXTURES
#error Native ownership fixture must not enter production
#endif
#include "../../th08_web/cpp/multiplayer/EnemyJournal.hpp"
#include <memory>

namespace th08::multiplayer::fixture {
inline u32 hash_vm(const EclVm& vm){
    u32 hash=2166136261u;
    const auto bytes=[&](const void* p,std::size_t size){auto* q=static_cast<const u8*>(p);for(std::size_t i=0;i<size;++i){hash^=q[i];hash*=16777619u;}};
    bytes(&vm,offsetof(EclVm,asynchronous));
    constexpr auto after=offsetof(EclVm,asynchronous_generations);
    bytes(reinterpret_cast<const u8*>(&vm)+after,sizeof(EclVm)-after);
    for(const auto& context:vm.asynchronous){const bool present=bool(context);bytes(&present,sizeof(present));if(context)bytes(context.get(),sizeof(EclContext));}
    return hash;
}
inline std::vector<u8> journal_ecl(){
    // The real ECL executor creates/replaces an async context, then that
    // context replaces itself, then the new context returns. Opcode 6 also
    // writes a literal instruction argument, exercising mutable program bytes.
    EclHeader header{};header.version=0x800;header.sub_count=4;header.timeline_count=1;
    std::vector<u8> data(sizeof(header)+4*4);u32 subs[4]{};
    const auto instruction=[&](i32 time,i16 opcode,std::initializer_list<u32> args){
        EclInstruction op{time,opcode,i16(12+4*args.size()),0,15,0};
        const auto at=data.size();data.resize(at+op.size);std::memcpy(data.data()+at,&op,12);
        std::copy(args.begin(),args.end(),reinterpret_cast<u32*>(data.data()+at+12));
    };
    subs[0]=u32(data.size());instruction(0,135,{0,1});instruction(1,6,{0,42});instruction(1,135,{0,2});instruction(100,-1,{});
    subs[1]=u32(data.size());instruction(0,0,{});instruction(100,-1,{});
    subs[2]=u32(data.size());instruction(0,135,{0,3});instruction(100,-1,{});
    subs[3]=u32(data.size());instruction(0,53,{});instruction(100,-1,{});
    header.timeline_offsets[0]=u32(data.size());EclTimelineInstruction stop{-1,0,8,15};
    data.resize(data.size()+sizeof(stop));std::memcpy(data.data()+header.timeline_offsets[0],&stop,sizeof(stop));
    header.timeline_offsets[1]=u32(data.size());std::memcpy(data.data(),&header,sizeof(header));
    std::memcpy(data.data()+sizeof(header),subs,sizeof(subs));return data;
}
inline const u32* enemy_journal_probe(){
    static u32 result[10]{};std::fill(result,result+10,0);result[0]=1;
    const auto require=[&](bool condition,u32 step){if(!condition)result[2]=step;return condition;};
    EclProgram program;const auto input=journal_ecl();
    if(!require(program.load(input.data(),u32(input.size())),1))return result;
    auto globals=std::make_unique<EclGlobals>();globals->difficulty_mask=2;
    Rng rng{1234,1234,0};FrameTiming timing{};EclExecutor executor(rng,timing,*globals);
    EnemyPopulation population(executor,program);
    TimelineSpawn spawn{};spawn.subroutine=0;spawn.life=20;spawn.item=-1;spawn.score=100;
    auto* first=population.spawn(spawn);
    if(!require(first&&first->asynchronous[0]&&!first->invalid,2))return result;
    const u32 before=hash_vm(*first);const std::vector<u8> pristine(program.data(),program.data()+program.size());
    std::weak_ptr<EclContext> old=first->asynchronous[0];auto* old_address=first->asynchronous[0].get();
    EnemyJournal journal;if(!require(journal.Bind(population)&&journal.BeginFrame(0),3))return result;
    if(!require(executor.step(*first)&&first->asynchronous[0]&&first->asynchronous[0]->subroutine==3,4))return result;
    const u32 after=hash_vm(*first);result[3]=u32(journal.BytesForFrame(0));
    if(!require(journal.EndFrame()&&!old.expired(),5))return result;
    if(!require(journal.BeginFrame(1)&&executor.step(*first)&&!first->asynchronous[0]&&journal.EndFrame(),6))return result;
    if(!require(journal.UndoTo(0)&&first->asynchronous[0].get()==old_address&&hash_vm(*first)==before,7))return result;
    if(!require(std::equal(pristine.begin(),pristine.end(),program.data()),8))return result;
    result[4]=1; // native async replace/return and mutable bytecode restored
    if(!require(journal.BeginFrame(0)&&executor.step(*first)&&hash_vm(*first)==after&&journal.EndFrame(),9))return result;
    if(!require(journal.UndoTo(0),10))return result;
    result[5]=1; // corrected execution reproduces the first pass
    if(!require(journal.BeginFrame(2),11))return result;
    first->flags=0;auto* reused=population.spawn(spawn);
    if(!require(reused==first&&journal.EndFrame()&&journal.UndoTo(2)&&hash_vm(*first)==before,12))return result;
    result[6]=1; // same native slot, recreated VM and destroyed contexts
    if(!require(!population.at(1)&&journal.BeginFrame(3),13))return result;
    auto* added=population.spawn(spawn);
    if(!require(added&&added!=first&&journal.EndFrame()&&journal.UndoTo(3)&&!population.at(1),14))return result;
    result[7]=1; // allocated-after-checkpoint VM removed on undo
    if(!require(journal.BeginFrame(4),15))return result;
    first->asynchronous[0].reset();
    if(!require(journal.EndFrame()&&!old.expired(),16))return result;
    journal.DiscardBefore(5);if(!require(old.expired(),17))return result;
    result[8]=1; // confirmed retirement releases the old owning contexts
    for(u32 frame=5;frame<5+EnemyJournal::History;++frame)
        if(!require(journal.BeginFrame(frame)&&journal.EndFrame(),18))return result;
    if(!require(!journal.BeginFrame(5+EnemyJournal::History)&&journal.Failed(),19))return result;
    result[9]=1; // no silent overwrite of a still-needed rollback checkpoint
    journal.Clear();population.reset();result[1]=1;return result;
}
}
