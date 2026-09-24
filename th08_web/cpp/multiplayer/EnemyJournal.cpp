#include "EnemyJournal.hpp"
#include <cstddef>

namespace th08::multiplayer {
bool EnemyJournal::Bind(EnemyPopulation& value){
    if(population||value.journal)return false;
    Netplay::RollbackJournalConfig config;config.maxFrames=History;
    config.maxBytesPerFrame=24*1024*1024;config.maxBlocksPerFrame=4096;
    if(!bytes.Reset(config))return false;
    population=&value;program=&value.program;
    program_data=program->mutable_data();program_size=program->size();
    value.journal=this;failed=false;return true;
}
bool EnemyJournal::BeginFrame(u32 frame){
    if(!population||Failed()||bytes.IsFrameOpen()||records.size()>=History||
       program->mutable_data()!=program_data||program->size()!=program_size)return Fail();
    if(!bytes.BeginFrame(frame))return Fail();
    records.emplace_back();auto& record=records.back();record.frame=frame;
    // ECL can write literal bytecode arguments. The loaded buffer is an
    // authoritative mutable owner even though its allocation remains stable.
    if(program_size&&!bytes.Touch(program_data,program_size))return Fail();
    if(!bytes.Touch(&population->spawn_failed,sizeof(population->spawn_failed))||
       !bytes.Touch(&population->replay_flags,sizeof(population->replay_flags))||
       !bytes.Touch(&population->initial_time_items,sizeof(population->initial_time_items))||
       !bytes.Touch(&population->practice_familiar,sizeof(population->practice_familiar)))return Fail();
    for(u32 slot=0;slot<population->enemies.size();++slot){
        const auto& enemy=population->enemies[slot];record.existed[slot]=bool(enemy);
        if(enemy&&(enemy->flags&1)&&!Capture(slot))return false;
    }
    return true;
}
bool EnemyJournal::Capture(u32 slot){
    if(!population||Failed())return false;
    if(!bytes.IsFrameOpen())return records.empty()?true:Fail();
    if(slot>=population->enemies.size()||!population->enemies[slot])return Fail();
    auto& record=records.back();if(record.captured[slot])return true;
    auto& vm=*population->enemies[slot];
    if(vm.active_context||vm.active_slot!=-1)return Fail();
    ContextOwners owners;owners.slot=slot;
    for(u32 i=0;i<4;++i){
        owners.contexts[i]=vm.asynchronous[i];
        if(owners.contexts[i]&&!bytes.Touch(owners.contexts[i].get(),sizeof(EclContext)))return Fail();
    }
    // Exclude shared_ptr ownership words. Every surrounding field is native
    // state; program/random/environment/familiar pointers stay stage-local.
    constexpr auto before=offsetof(EclVm,asynchronous);
    constexpr auto after=offsetof(EclVm,asynchronous_generations);
    if(!bytes.Touch(&vm,before)||
       !bytes.Touch(reinterpret_cast<u8*>(&vm)+after,sizeof(EclVm)-after))return Fail();
    record.owners.push_back(std::move(owners));record.captured[slot]=true;return true;
}
bool EnemyJournal::EndFrame(){return !Failed()&&bytes.EndFrame();}
bool EnemyJournal::UndoTo(u32 frame){
    if(!population||Failed()||bytes.IsFrameOpen())return false;
    auto target=records.begin();while(target!=records.end()&&target->frame!=frame)++target;
    if(target==records.end())return false;
    const auto existed=target->existed;
    if(program->mutable_data()!=program_data||program->size()!=program_size||!bytes.UndoTo(frame))return Fail();
    // Restore in reverse time so the oldest handle wins. Payloads were restored
    // above while all generations were pinned and therefore still alive.
    while(!records.empty()&&records.back().frame>=frame){
        for(auto& owner:records.back().owners){
            auto& vm=population->enemies[owner.slot];if(!vm)return Fail();
            for(u32 i=0;i<4;++i)vm->asynchronous[i]=owner.contexts[i];
        }
        records.pop_back();
    }
    for(u32 slot=0;slot<existed.size();++slot)if(!existed[slot])population->enemies[slot].reset();
    return true;
}
void EnemyJournal::DiscardBefore(u32 frame){
    if(bytes.IsFrameOpen())return;
    bytes.DiscardBefore(frame);
    while(!records.empty()&&records.front().frame<frame)records.pop_front();
}
void EnemyJournal::Clear(){
    if(population&&population->journal==this)population->journal=nullptr;
    population=nullptr;program=nullptr;program_data=nullptr;program_size=0;
    records.clear();bytes.Clear();failed=false;
}
u32 EnemyJournal::AuditHash()const{
    if(!population)return 0;
    u32 hash=2166136261u;
    const auto part=[&](const void* p,std::size_t n){auto* data=static_cast<const u8*>(p);for(std::size_t i=0;i<n;++i){hash^=data[i];hash*=16777619u;}};
    part(program->data(),program->size());
    for(u32 slot=0;slot<population->enemies.size();++slot){
        const auto& vm=population->enemies[slot];const bool exists=bool(vm);part(&exists,sizeof(exists));if(!vm)continue;
        part(vm.get(),offsetof(EclVm,asynchronous));
        constexpr auto after=offsetof(EclVm,asynchronous_generations);
        part(reinterpret_cast<const u8*>(vm.get())+after,sizeof(EclVm)-after);
        for(const auto& context:vm->asynchronous){const bool present=bool(context);part(&present,sizeof(present));if(context)part(context.get(),sizeof(EclContext));}
    }
    return hash;
}
}
