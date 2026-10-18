#include "FrameScheduler.hpp"
namespace th15 {
i32 FrameScheduler::add(FrameCallback& c,FramePass pass,i32 priority){
    if(c.scheduler)return -1;
    i32 result=0;
    if(c.initialize){result=c.initialize(c.owner);c.initialize=nullptr;}
    c.priority=priority;c.pass=pass;c.scheduler=this;
    auto& head=pass==FramePass::Update?updates:draws;
    FrameCallback* before=nullptr;auto* after=head;
    while(after&&after->priority<priority){before=after;after=after->next;}
    c.previous=before;c.next=after;
    if(before)before->next=&c;else head=&c;
    if(after)after->previous=&c;
    return result;
}
void FrameScheduler::remove(FrameCallback& c)noexcept{
    if(c.scheduler!=this)return;
    if(continuation==&c)continuation=c.next;
    auto& head=c.pass==FramePass::Update?updates:draws;
    if(c.previous)c.previous->next=c.next;else head=c.next;
    if(c.next)c.next->previous=c.previous;
    c.previous=nullptr;c.next=nullptr;c.scheduler=nullptr;c.run=nullptr;
}
void FrameScheduler::clear(FramePass pass)noexcept{
    auto*& head=pass==FramePass::Update?updates:draws;
    while(head)remove(*head);
}
i32 FrameScheduler::execute(FramePass pass){
    auto*& head=pass==FramePass::Update?updates:draws;
    i32 count=0;auto* current=head;
    while(current){
        continuation=current->next;
        if(current->run){
            if(current->enabled){
                for(;;){
                    if(pass==FramePass::Update&&closing){
                        if(current->cleanup)current->cleanup(current->owner);
                        break;
                    }
                    const i32 action=current->run(current->owner);
                    switch(FrameAction(action)){
                    case FrameAction::Remove:remove(*current);break;
                    case FrameAction::Repeat:if(current->enabled)continue;break;
                    case FrameAction::StopSuccess:return 1;
                    case FrameAction::Stop:return 0;
                    case FrameAction::Error:return -1;
                    case FrameAction::Restart:
                        if(pass==FramePass::Update){count=0;current=head;goto restart;}break;
                    case FrameAction::Cleanup:
                        if(pass==FramePass::Update&&current->cleanup)current->cleanup(current->owner);break;
                    case FrameAction::StopAlternate:if(pass==FramePass::Update)return 0;break;
                    default:break;
                    }
                    break;
                }
            }
            ++count;
        }
        current=continuation;
        restart:;
    }
    return count;
}
}
