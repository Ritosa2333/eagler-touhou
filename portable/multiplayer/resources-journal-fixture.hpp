#pragma once
#ifndef TH_MULTIPLAYER_FIXTURES
#error Native resource ownership fixture must not enter production
#endif
#include "../../th08_web/cpp/multiplayer/ResourcesJournal.hpp"

namespace th08::multiplayer::fixture {
inline std::vector<u8> resource_journal_anm(i16 opcode=37,u16 mask=0,u32 destination=7,u32 input=42){
    // Valid native empty-atlas resource. Two script entries intentionally alias
    // one body to exercise deduplicated interval capture after template clone.
    std::vector<u8> bytes(192,0);
    const auto put=[&](u32 offset,u32 value){std::memcpy(bytes.data()+offset,&value,4);};
    const auto real=[&](u32 offset,float value){std::memcpy(bytes.data()+offset,&value,4);};
    put(0,1);put(4,2);put(12,16);put(16,16);put(28,84);put(40,3);
    put(64,120);put(68,0);put(72,160);put(76,1);put(80,160);
    std::memcpy(bytes.data()+84,"@rollback",10);
    put(120,0);real(124,0);real(128,0);real(132,16);real(136,16);
    AnmRawInstr write{};write.opcode=opcode;write.varMask=mask;write.instructionSize=24;write.time=0;
    std::memcpy(bytes.data()+160,&write,8);put(168,destination);put(172,input);
    AnmRawInstr end{};end.opcode=-1;end.instructionSize=8;end.time=0;
    std::memcpy(bytes.data()+184,&end,8);return bytes;
}
inline u32 resource_journal_float_bits(float value){u32 bits;std::memcpy(&bits,&value,4);return bits;}
inline u32 resource_journal_literal(const AnmLoaded& file){
    u32 value;std::memcpy(&value,reinterpret_cast<const u8*>(file.scripts[0])+8,4);return value;
}
inline bool resource_script_write_case(TextureStore& textures,i16 opcode,u16 mask,u32 destination,
    u32 input,u32 expected_literal,bool real,bool variable){
    AnmLibrary library(textures);const auto source=resource_journal_anm(opcode,mask,destination,input);
    auto* file=library.load(69,source.data(),u32(source.size()));if(!file)return false;
    ResourcesJournal journal;if(!journal.Bind(library))return false;
    Rng random{1234,1234,0};AnmExecutor executor(random);AnmVm first{},second{};
    const auto before=journal.AuditHash();
    if(!journal.BeginFrame(0)||!journal.EndFrame())return false;
    const auto metadata_bytes=journal.BytesForFrame(0);
    if(!journal.UndoTo(0)||journal.AuditHash()!=before)return false;
    const auto run=[&](){
        executor.invalid=false;
        executor.start(*file,first,file->scripts[0]);if(executor.invalid)return false;
        executor.start(*file,second,file->scripts[1]);if(executor.invalid)return false;
        if(resource_journal_literal(*file)!=expected_literal)return false;
        if(variable){
            const u32 a=real?resource_journal_float_bits(first.floatVar0):u32(first.intVar0);
            const u32 b=real?resource_journal_float_bits(second.floatVar0):u32(second.intVar0);
            if(a!=input||b!=input)return false;
        }
        return true;
    };
    // Compare with the old eager owner on the very same allocation. The hash
    // independently covers the whole raw buffer and unchanged sprite metadata.
    if(!journal.SetScriptFirstWrite(false)||!journal.BeginFrame(0)||!run()||!journal.EndFrame())return false;
    const auto reference=journal.AuditHash();const auto eager_bytes=journal.BytesForFrame(0);
    if(!journal.UndoTo(0)||journal.AuditHash()!=before||resource_journal_literal(*file)!=destination)return false;
    if(!journal.SetScriptFirstWrite(true)||!journal.BeginFrame(0)||!run()||!journal.EndFrame())return false;
    const auto sparse_bytes=journal.BytesForFrame(0);
    if(journal.AuditHash()!=reference||sparse_bytes!=metadata_bytes+(variable?0:4)||sparse_bytes>=eager_bytes)return false;
    return journal.UndoTo(0)&&journal.AuditHash()==before&&resource_journal_literal(*file)==destination;
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
    if(!require(deferred->script_write_context==&journal&&deferred->script_before_write,15))return result;
    const auto stable_hash=journal.AuditHash();
    // A clone of a live file owns its private script bytes, never its source's
    // callback. Exercise it while the original has a closed checkpoint.
    AnmResource clone;clone.clone_from(*library.resource(69),70);
    if(!require(!clone.view().script_write_context&&!clone.view().script_before_write&&
        clone.view().rawData!=deferred->rawData&&journal.BeginFrame(3)&&journal.EndFrame(),16))return result;
    executor.invalid=false;executor.start(clone.view(),vm,clone.view().scripts[0]);
    if(!require(!executor.invalid&&resource_journal_literal(clone.view())==42&&
        journal.AuditHash()==stable_hash&&journal.UndoTo(3),17))return result;
    // A rejected write must leave the literal untouched. Like a rejected
    // resource load, its failure flag remains recoverable by the older owner.
    if(!require(journal.BeginFrame(4)&&journal.EndFrame(),18))return result;
    executor.invalid=false;executor.start(*deferred,vm,deferred->scripts[0]);
    if(!require(executor.invalid&&journal.Failed()&&resource_journal_literal(*deferred)==7,19))return result;
    if(!require(journal.UndoTo(4)&&!journal.Failed()&&journal.AuditHash()==stable_hash&&
        deferred->script_write_context==&journal&&deferred->script_before_write,20))return result;
    std::size_t metadata_bytes=0;
    const auto extend=[&](){
        executor.invalid=false;
        if(!journal.BeginFrame(5)||!journal.EndFrame())return false;
        metadata_bytes=journal.BytesForFrame(5);
        if(!journal.BeginFrame(6,true))return false;
        executor.start(*deferred,vm,deferred->scripts[0]);
        if(executor.invalid||!journal.EndFrame()||!journal.BeginFrame(7,true))return false;
        executor.start(*deferred,vm,deferred->scripts[1]);
        return !executor.invalid&&journal.EndFrame();
    };
    if(!require(extend()&&journal.BytesForFrame(5)==metadata_bytes+4,21))return result;
    const auto extended_hash=journal.AuditHash();
    if(!require(!journal.SetScriptFirstWrite(false)&&journal.UndoTo(5)&&
        journal.AuditHash()==stable_hash&&journal.SetScriptFirstWrite(false),22))return result;
    if(!require(extend()&&journal.AuditHash()==extended_hash&&journal.UndoTo(5)&&
        journal.AuditHash()==stable_hash&&journal.SetScriptFirstWrite(true),23))return result;
    journal.Clear();
    if(!require(!deferred->script_write_context&&!deferred->script_before_write,24))return result;
    // Reject an occupied later file without partially installing earlier ones.
    auto* occupied=library.load(70,source.data(),u32(source.size()));if(!require(occupied!=nullptr,25))return result;
    const auto foreign_hook=+[](void*,const AnmLoaded&,void*,std::size_t){return true;};
    occupied->script_write_context=&library;occupied->script_before_write=foreign_hook;
    if(!require(!journal.Bind(library)&&!deferred->script_write_context&&!deferred->script_before_write&&
        occupied->script_write_context==&library&&occupied->script_before_write==foreign_hook,26))return result;
    journal.Clear();occupied->ClearScriptWriteHook();
    if(!require(journal.Bind(library),27))return result;
    const auto owner_hash=journal.AuditHash();journal.Clear();
    ResourcesJournal replacement;
    if(!require(replacement.Bind(library)&&replacement.AuditHash()==owner_hash,28))return result;
    replacement.Clear();
    if(!require(!deferred->script_write_context&&!deferred->script_before_write&&
        !occupied->script_write_context&&!occupied->script_before_write,29))return result;
    // A new load also drops runtime hook state instead of retaining old owners.
    clone.view().script_write_context=&library;clone.view().script_before_write=foreign_hook;
    if(!require(clone.load(70,source.data(),u32(source.size()))&&
        !clone.view().script_write_context&&!clone.view().script_before_write,30))return result;
    const u32 real_input=resource_journal_float_bits(42.5f);
    if(!require(resource_script_write_case(textures,37,0,7,42,42,false,false),31))return result;
    if(!require(resource_script_write_case(textures,38,0,resource_journal_float_bits(7.25f),real_input,real_input,true,false),32))return result;
    if(!require(resource_script_write_case(textures,37,1,10000,42,10000,false,true),33))return result;
    if(!require(resource_script_write_case(textures,38,1,resource_journal_float_bits(10004.f),real_input,resource_journal_float_bits(10004.f),true,true),34))return result;
    if(!require(resource_script_write_case(textures,37,1,12345,42,42,false,false),35))return result;
    if(!require(resource_script_write_case(textures,38,1,resource_journal_float_bits(12345.f),real_input,real_input,true,false),36))return result;
    // Opcode 5 rewrites one literal repeatedly before exiting, then the alias
    // writes it once more. The retained before-image is still exactly 4 B.
    if(!require(resource_script_write_case(textures,5,0,2,0,0xffffffffu,false,false),37))return result;
    result[9]=1;result[1]=1;return result;
}
}
