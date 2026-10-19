#include "UpdateChain.hpp"
namespace th10 {
void UpdateChainEntry::initialize() noexcept {callback=initialize_callback=notify_callback=0;priority=0;flags&=~1u;node.initialize(this);}
// 0x449aa0. Sentinel flags and owner fields retain their prior values.
void UpdateChain::initialize() noexcept {update.initialize();draw.initialize();}
// 0x449ae0 / 0x449b70. Equal priorities insert before existing entries.
i32 UpdateChain::insert(UpdateChainEntry& entry,i32 priority,bool drawing,UpdateChainEnvironment& env){
    i32 result=0;if(entry.initialize_callback){result=env.invoke(entry.initialize_callback,entry.owner);entry.initialize_callback=0;}
#ifdef TH_NATIVE_PLATFORM
    entry.function=entry.callback?env.resolve(entry.callback):NativeCallback{};
    entry.notification=entry.notify_callback?env.resolve(entry.notify_callback):NativeCallback{};
#endif
    env.lock();entry.priority=priority;auto* before=&(drawing?draw:update).node;
    while(before->next&&before->next->value->priority<priority)before=before->next;
    if(before->next){entry.node.next=before->next;before->next->previous=&entry.node;}
    before->next=&entry.node;entry.node.previous=before;env.unlock();return result;
}
// 0x449c00 / 0x449d40. Update-only results 6 and 7 restart traversal and issue
// the notification callback respectively. Result 2 repeats the current entry.
i32 UpdateChain::run(bool drawing,UpdateChainEnvironment& env){
    env.lock();i32 count=0;auto* next=(drawing?draw:update).node.next;
    while(next){auto* entry=next->value;next=next->next;if(!entry->callback)continue;
        bool restart=false;
        while(entry->flags&2){
            env.unlock();
#ifdef TH_NATIVE_PLATFORM
            const i32 result=entry->function(entry->owner);
#else
            const i32 result=env.invoke(entry->callback,entry->owner);
#endif
            env.lock();
            if(result==2)continue;
            if(result==0)remove(entry,env);
            else if(result==3||result==4||result==5){env.unlock();return result==3?1:result==4?0:-1;}
            else if(!drawing&&result==6){count=0;next=update.node.next;restart=true;}
            else if(!drawing&&result==7&&entry->notify_callback){
#ifdef TH_NATIVE_PLATFORM
                entry->notification(entry->owner);
#else
                env.invoke(entry->notify_callback,entry->owner);
#endif
            }
            break;
        }
        if(!restart)count=wrapping_add(count,1);
    }
    env.unlock();return count;
}
// 0x449f60. Removal never invokes the notification callback. The embedded
// sentinel itself cannot be removed because it has no predecessor.
void UpdateChain::remove(UpdateChainEntry* entry,UpdateChainEnvironment& env){
    if(!entry)return;ListNode<UpdateChainEntry>* found=nullptr;
    for(auto* node=&update.node;node;node=node->next)if(node->value==entry){found=node;break;}
    if(!found)for(auto* node=&draw.node;node;node=node->next)if(node->value==entry){found=node;break;}
    if(!found||!found->previous)return;
    found->unlink();entry->callback=0;
    if(entry->flags&1){entry->callback=entry->initialize_callback=entry->notify_callback=0;env.release_entry(entry);}
}
// 0x449e50. Retain the successor before removal and lock separately per entry.
void UpdateChain::clear_list(UpdateChainEntry& sentinel,UpdateChainEnvironment& env){
    auto* next=sentinel.node.next;while(next){auto* entry=next->value;next=next->next;if(entry){env.lock();remove(entry,env);env.unlock();}}
}
// 0x449ed0.
UpdateChainEntry* UpdateChain::allocate(CallbackToken callback,UpdateChainEnvironment& env){
    auto* entry=env.allocate_entry();entry->initialize();entry->initialize_callback=entry->notify_callback=0;entry->callback=callback;entry->flags|=1;return entry;
}
// 0x449f20 and 0x44a000 / 0x44a030 / 0x44a060 / 0x44a090.
void UpdateChain::remove_locked(UpdateChainEntry* entry,UpdateChainEnvironment& env){if(entry){env.lock();remove(entry,env);env.unlock();}}
UpdateChainEntry* UpdateChain::add(CallbackToken callback,void* owner,i32 priority,bool drawing,bool unlocked,UpdateChainEnvironment& env){
    auto* entry=allocate(callback,env);entry->owner=owner;entry->flags=unlocked?entry->flags|2:entry->flags&~2u;insert(*entry,priority,drawing,env);return entry;
}
}
