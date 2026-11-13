#pragma once
#include "Rng.hpp"
#if PORT_TITLE == 14
#include "StageRestoreMemoryTh14.hpp"
#endif

namespace th11 {
#if PORT_TITLE == 14
enum class ReplayRngRestorePointTh14 : u32 {
    GameplaySeedStored, StageSeedRecorded, ScriptSeedStored, GameplayCounterCleared
};

// Optional scoped instruction observer. The ordinary game installs no observer;
// a controlled scheduler can execute main-thread work between worker stores.
// RNG and replay layouts remain unchanged.
struct ReplayRngRestoreScopeTh14 {
    using Callback=void(*)(void*,ReplayRngRestorePointTh14,Rng&,Rng*);
    Rng* gameplay;void* owner;Callback callback;
    ReplayRngRestoreScopeTh14* previous;bool running=false;
#if defined(TH14_CPP_STAGE_WORKER)
    static inline thread_local ReplayRngRestoreScopeTh14* active=nullptr;
#else
    static inline ReplayRngRestoreScopeTh14* active=nullptr;
#endif
    ReplayRngRestoreScopeTh14(Rng& rng,void* context,Callback function)noexcept:
        gameplay(&rng),owner(context),callback(function),previous(active){active=this;}
    ~ReplayRngRestoreScopeTh14(){active=previous;}
    ReplayRngRestoreScopeTh14(const ReplayRngRestoreScopeTh14&)=delete;
    ReplayRngRestoreScopeTh14& operator=(const ReplayRngRestoreScopeTh14&)=delete;
    static void checkpoint(ReplayRngRestorePointTh14 point,Rng& rng,Rng* script)noexcept{
        for(auto* scope=active;scope;scope=scope->previous){
            if(scope->gameplay!=&rng)continue;
            if(scope->running||!scope->callback)return;
            scope->running=true;scope->callback(scope->owner,point,rng,script);
            scope->running=false;return;
        }
    }
};

enum class ReplayRngRestoreResultTh14 : u32 {Waiting,Ready,Failed};

// A waiting host leaves the next store untouched. Completed stores, the
// recording path's saved AX and owner identities survive the next main tick.
class ReplayRngRestoreJobTh14 {
    enum class Kind:u32 {Unset,Initial,Playback,Record};
    Kind kind=Kind::Unset;Rng* owner=nullptr;Rng* other=nullptr;const u32* entry=nullptr;
    u32 step=0,store_count=0;u16 captured=0;bool running=false,failed=false;
    ReplayRngRestorePointTh14 last=ReplayRngRestorePointTh14::GameplaySeedStored;
    template<class Ready,class Committed>
    ReplayRngRestoreResultTh14 advance(Kind requested,Rng& gameplay,Rng* script,const u32& seed,u32* recording,Ready&& readiness,Committed&& committed)noexcept{
        using Result=ReplayRngRestoreResultTh14;using Point=ReplayRngRestorePointTh14;
        if(failed)return Result::Failed;
        if(running)return Result::Waiting;
        if(kind==Kind::Unset){kind=requested;owner=&gameplay;other=script;entry=&seed;}
        else if(kind!=requested||owner!=&gameplay||other!=script||entry!=&seed){failed=true;return Result::Failed;}
        struct Guard {bool& value;explicit Guard(bool& v):value(v){value=true;}~Guard(){value=false;}} guard(running);
        while(step<3){
            if(step==1&&(kind==Kind::Initial||!script)){++step;continue;}
            const Point point=step==0?(kind==Kind::Record?Point::StageSeedRecorded:Point::GameplaySeedStored):
                step==1?Point::ScriptSeedStored:Point::GameplayCounterCleared;
            const auto ready=readiness(point);
            if(ready!=Result::Ready){if(ready==Result::Failed)failed=true;return ready;}
            if(step==0){
                if(kind==Kind::Record)captured=th14_record_gameplay_seed(gameplay,*recording);
                else th14_playback_gameplay_seed(gameplay,seed);
            }else if(step==1){
                // Playback456115 reloads the immutable replay word. Record
                //4560a7 keeps its captured AX even if gameplay advances.
                if(kind==Kind::Record)th14_record_script_seed(*script,captured);
                else th14_playback_script_seed(*script,seed);
            }else th14_clear_gameplay_calls(gameplay);
            ++step;++store_count;last=point;
            ReplayRngRestoreScopeTh14::checkpoint(point,gameplay,script);
            if(!committed(point)){failed=true;return Result::Failed;}
        }
        return Result::Ready;
    }
public:
    u32 stores()const noexcept{return store_count;}
    bool complete()const noexcept{return step==3&&!failed;}
    ReplayRngRestorePointTh14 last_point()const noexcept{return last;}
    template<class Ready>
    ReplayRngRestoreResultTh14 playback(Rng& gameplay,Rng* script,const u32& seed,bool first,Ready&& readiness)noexcept{
        return playback(gameplay,script,seed,first,readiness,[](ReplayRngRestorePointTh14){return true;});
    }
    template<class Ready,class Committed>
    ReplayRngRestoreResultTh14 playback(Rng& gameplay,Rng* script,const u32& seed,bool first,Ready&& readiness,Committed&& committed)noexcept{
        return advance(first?Kind::Initial:Kind::Playback,gameplay,script,seed,nullptr,readiness,committed);
    }
    template<class Ready>
    ReplayRngRestoreResultTh14 record(Rng& gameplay,Rng& script,u32& seed,Ready&& readiness)noexcept{
        return record(gameplay,script,seed,readiness,[](ReplayRngRestorePointTh14){return true;});
    }
    template<class Ready,class Committed>
    ReplayRngRestoreResultTh14 record(Rng& gameplay,Rng& script,u32& seed,Ready&& readiness,Committed&& committed)noexcept{
        return advance(Kind::Record,gameplay,&script,seed,&seed,readiness,committed);
    }
};

inline void th14_restore_replay_rng(Rng& gameplay,Rng* script,const u32& entry_seed,bool first)noexcept{
    ReplayRngRestoreJobTh14 job;
    job.playback(gameplay,script,entry_seed,first,[](ReplayRngRestorePointTh14){return ReplayRngRestoreResultTh14::Ready;});
}

inline void th14_record_stage_rng(Rng& gameplay,Rng& script,u32& entry_seed)noexcept{
    ReplayRngRestoreJobTh14 job;
    job.record(gameplay,script,entry_seed,[](ReplayRngRestorePointTh14){return ReplayRngRestoreResultTh14::Ready;});
}
#endif
}
