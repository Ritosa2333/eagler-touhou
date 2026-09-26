#pragma once
#include "EnemyJournal.hpp"
#include "PoolsJournal.hpp"
#include "ResourcesJournal.hpp"
#include "../game/ReplayRecording.hpp"
#include <array>
#include <deque>
#include <vector>

namespace th08 {
class BrowserRuntime;class GameApplication;
namespace multiplayer {
// In-process native stage-state journal. Device audio/files/presentation are
// separate confirmed-output owners and are not restored by copying this type.
// Until those fences are integrated, production NetplayRuntime stays exact-only.
class WorldJournal {
public:
    static constexpr std::size_t History=8,GroupCount=12;
    enum Group:u8 {Session,Player,Enemy,Scene,Clock,Render,Recording,Screens,Ecl,Pools,Resources,Composite};
    WorldJournal()=default;~WorldJournal(){Clear();}
    WorldJournal(const WorldJournal&)=delete;
    WorldJournal& operator=(const WorldJournal&)=delete;
    bool Bind(BrowserRuntime&);
    bool BeginFrame(u32 frame);
    bool EndFrame();
    bool UndoTo(u32 frame);
    void DiscardBefore(u32 frame);
    void Clear();
    bool CanAdvance()const;
#ifdef TH_MULTIPLAYER_FIXTURES
    bool DiagnosticInputSampler();
    std::array<std::size_t,5> DiagnosticBytesForFrame(u32 frame)const;
    std::size_t DiagnosticBlockCount()const{return blocks.size();}
    std::size_t DiagnosticBlockBytes(u32 index)const{return index<blocks.size()?blocks[index].bytes:0;}
#endif
    bool Failed()const{return failed||main.Failed()||enemies.Failed()||pools.Failed()||resources.Failed();}
    bool HasHistory()const{return !records.empty();}
    const char* Error()const{return error;}
    u32 OwnerFaults()const{return (main.Failed()?1u:0u)|(enemies.Failed()?2u:0u)|(pools.Failed()?4u:0u)|(resources.Failed()?8u:0u)|(failed?16u:0u);}
    std::size_t BytesForFrame(u32 frame)const;
    std::array<u32,GroupCount> AuditHash()const;
    std::vector<u32> BlockHashes()const;
    const char* BlockName(u32 index)const{return index<blocks.size()?blocks[index].name:"outside inventory";}
private:
    struct Block {void* address;std::size_t bytes;Group group;const char* name;};
    struct Record {u32 frame;std::unique_ptr<ReplayRecording> replay;};
    BrowserRuntime* runtime=nullptr;GameApplication* app=nullptr;
    i32 stage=-1;const void *stdData=nullptr,*quads=nullptr,*msgData=nullptr;
    std::size_t stdSize=0,quadCount=0,msgSize=0;
    EnemyJournal enemies;PoolsJournal pools;ResourcesJournal resources;Netplay::RollbackJournal main;
    std::vector<Block> blocks;std::deque<Record> records;
    bool failed=false;const char* error="";
    template<class T>void Add(T& field,Group group,const char* name){
        static_assert(std::is_trivially_copyable_v<T>,"Use an owning snapshot, not raw bytes, for this field");
        Raw(&field,sizeof(field),group,name);
    }
    void Raw(void*,std::size_t,Group,const char*);
    bool Inventory();bool StableGraph()const;void RebasePresentation();
    bool Fail(const char* reason){failed=true;error=reason;return false;}
};
}
}
