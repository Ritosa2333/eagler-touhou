#include "FrameState.hpp"
#include <algorithm>
namespace th15::multiplayer {
bool FrameState::capture(const FrameScheduler& scheduler){
    error.clear();ready_=false;entries_.clear();for(auto& group:order_)group.clear();
    const std::array<FrameCallback*,2> heads{scheduler.updates,scheduler.draws};
    for(size_t group=0;group<heads.size();++group)for(auto* c=heads[group];c;c=c->next){
        if(c->scheduler!=&scheduler||std::any_of(entries_.begin(),entries_.end(),[&](const Entry& e){return e.original==c;})){error="Invalid rollback callback graph";return false;}
        order_[group].push_back(entries_.size());entries_.push_back({c,c->run,c->initialize,c->cleanup,c->owner,c->enabled,c->priority,c->pass});
    }
    continuation_=scheduler.continuation;closing_=scheduler.closing;
    if(continuation_&&std::none_of(entries_.begin(),entries_.end(),[&](const Entry& e){return e.original==continuation_;})){error="Rollback continuation is not linked";return false;}
    ready_=true;return true;
}
void FrameState::detach(FrameScheduler& scheduler)noexcept{
    for(auto* head:{scheduler.updates,scheduler.draws})for(auto* c=head;c;){auto* next=c->next;c->previous=c->next=nullptr;c->scheduler=nullptr;c=next;}
    scheduler.updates=scheduler.draws=scheduler.continuation=nullptr;
}
bool FrameState::restore(FrameScheduler& scheduler,const CallbackResolver& callbacks,const OwnerResolver& owners)const{
    error.clear();if(!ready_){error="No callback frame snapshot";return false;}
    std::vector<FrameCallback*> targets;std::vector<void*> owners_resolved;
    targets.reserve(entries_.size());owners_resolved.reserve(entries_.size());
    for(const auto& e:entries_){
        auto* target=callbacks?callbacks(e.original):e.original;void* owner=e.owner&&owners?owners(e.owner):e.owner;
        if(!target||(e.owner&&!owner)||std::find(targets.begin(),targets.end(),target)!=targets.end()){error="Rollback callback owner was not rebound";return false;}
        targets.push_back(target);owners_resolved.push_back(owner);
    }
    detach(scheduler);
    for(size_t i=0;i<entries_.size();++i){const auto& e=entries_[i];auto& target=*targets[i];
        target.run=e.run;target.initialize=e.initialize;target.cleanup=e.cleanup;target.owner=owners_resolved[i];target.enabled=e.enabled;target.priority=e.priority;target.pass=e.pass;target.scheduler=&scheduler;
    }
    std::array<FrameCallback*,2> heads{};
    for(size_t group=0;group<order_.size();++group){FrameCallback* previous=nullptr;
        for(auto index:order_[group]){auto* target=targets[index];target->previous=previous;if(previous)previous->next=target;else heads[group]=target;previous=target;}
        if(previous)previous->next=nullptr;
    }
    scheduler.updates=heads[0];scheduler.draws=heads[1];scheduler.continuation=nullptr;scheduler.closing=closing_;
    if(continuation_)for(size_t i=0;i<entries_.size();++i)if(entries_[i].original==continuation_){scheduler.continuation=targets[i];break;}
    return true;
}
}
