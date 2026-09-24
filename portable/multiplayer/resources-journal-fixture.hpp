#pragma once
#ifndef TH_MULTIPLAYER_FIXTURES
#error Native resource ownership fixture must not enter production
#endif
#include "../../th08_web/cpp/multiplayer/ResourcesJournal.hpp"

namespace th08::multiplayer::fixture {
inline std::vector<u8> resource_journal_anm(){
    // Valid native empty-atlas resource. Two script entries intentionally alias
    // one body to exercise deduplicated interval capture after template clone.
    std::vector<u8> bytes(184,0);
    const auto put=[&](u32 offset,u32 value){std::memcpy(bytes.data()+offset,&value,4);};
    const auto real=[&](u32 offset,float value){std::memcpy(bytes.data()+offset,&value,4);};
    put(0,1);put(4,2);put(12,16);put(16,16);put(28,84);put(40,3);
    put(64,120);put(68,0);put(72,160);put(76,1);put(80,160);
    std::memcpy(bytes.data()+84,"@rollback",10);
    put(120,0);real(124,0);real(128,0);real(132,16);real(136,16);
    AnmRawInstr write{};write.opcode=37;write.instructionSize=16;write.time=0;
    std::memcpy(bytes.data()+160,&write,8);put(168,7);put(172,42);
    AnmRawInstr end{};end.opcode=-1;end.instructionSize=8;end.time=0;
    std::memcpy(bytes.data()+176,&end,8);return bytes;
}
inline const u32* resources_journal_probe(TextureStore& textures){
    static u32 result[10]{};std::fill(result,result+10,0);result[0]=1;
    const auto require=[&](bool condition,u32 step){if(!condition)result[2]=step;return condition;};
    AnmLibrary library(textures);const auto source=resource_journal_anm();
    if(!require(library.preload(source.data(),u32(source.size())),1))return result;
    auto* file=library.load(69,source.data(),u32(source.size()));
    if(!require(file&&file->scriptCount==2&&file->scripts[0]==file->scripts[1]&&library.preload_hits()==1,2))return result;
    ResourcesJournal journal;if(!require(journal.Bind(library),3))return result;
    Rng rng{1234,1234,0};AnmExecutor executor(rng);AnmVm vm{};
    const auto before=journal.AuditHash();
    const auto run=[&](){
        executor.start(*file,vm,file->scripts[0]);
        u32 literal=0;std::memcpy(&literal,reinterpret_cast<const u8*>(file->scripts[0])+8,4);
        if(literal!=42||executor.invalid)return false;
        file->sprites[0].widthPx=33;return true;
    };
    if(!require(journal.BeginFrame(0)&&run()&&journal.EndFrame(),4))return result;
    const auto after=journal.AuditHash();result[3]=u32(journal.BytesForFrame(0));
    if(!require(before!=after&&journal.UndoTo(0)&&journal.AuditHash()==before,5))return result;
    result[4]=1;
    vm={};
    if(!require(journal.BeginFrame(0)&&run()&&journal.EndFrame()&&journal.AuditHash()==after,6))return result;
    if(!require(journal.UndoTo(0),7))return result;
    result[5]=1;
    if(!require(journal.BeginFrame(1)&&journal.EndFrame(),8))return result;
    library.release(69);
    if(!require(journal.Failed()&&library.get(69)==file&&journal.UndoTo(1)&&!journal.Failed(),9))return result;
    result[6]=1;
    if(!require(journal.BeginFrame(2)&&journal.EndFrame(),10))return result;
    if(!require(!library.load(69,source.data(),u32(source.size()))&&library.get(69)==file&&journal.UndoTo(2),11))return result;
    result[7]=1;
    journal.Clear();library.release(69);
    if(!require(!library.get(69),12))return result;
    auto* deferred=library.load(69,source.data(),u32(source.size()),true);
    if(!require(deferred&&deferred->numberEntriesToBeLoaded&&!journal.Bind(library),13))return result;
    if(!require(library.service()&&journal.Bind(library),14))return result;
    result[8]=1;
    journal.Clear();result[9]=1;result[1]=1;return result;
}
}
