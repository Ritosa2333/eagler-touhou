#include "Chain.hpp"
#include <vector>
namespace th08 {
ChainElement::~ChainElement(){if(deleted)deleted(argument);previous=nullptr;next=nullptr;callback=nullptr;added=nullptr;deleted=nullptr;}
ChainElement* Chain::create(JobCallback callback){auto* element=new ChainElement();element->set_callback(callback);element->flags|=1;return element;}
i32 Chain::add(ChainElement* element,i32 priority,bool draw){
    i32 result=0;
    if(element->added){result=element->added(element->argument);element->added=nullptr;}
    auto* current=draw?&drawing:&calculation;element->priority=i16(priority);
    while(current->next){if(current->priority>priority)break;current=current->next;}
    if(current->priority>priority){
        element->next=current;element->previous=current->previous;
        if(element->previous)element->previous->next=element;
        current->previous=element;
    }else{element->next=nullptr;element->previous=current;current->next=element;}
    return result;
}
bool Chain::contains(ChainElement* element) const noexcept {
    for(const ChainElement* root:{&calculation,&drawing})for(auto* p=root;p;p=p->next)if(p==element)return true;
    return false;
}
void Chain::cut(ChainElement* element){
    if(!element||!contains(element)||!element->previous)return;
    element->callback=nullptr;element->previous->next=element->next;
    if(element->next)element->next->previous=element->previous;
    element->previous=nullptr;element->next=nullptr;
    if(element->flags&1)delete element;
    else if(element->deleted){auto callback=element->deleted;element->deleted=nullptr;callback(element->argument);}
}
i32 Chain::run(bool draw){
    auto* root=draw?&drawing:&calculation;auto* current=root;i32 count=0;
    while(current){
        if(current->callback){
            const JobResult result=current->callback(current->argument);
            switch(result){
            case JobResult::Remove:{auto* removed=current;current=current->next;cut(removed);++count;continue;}
            case JobResult::Again:continue;
            case JobResult::Break:return 1;
            case JobResult::Exit:return 0;
            case JobResult::Error:return -1;
            case JobResult::Restart:if(!draw){current=root;count=0;continue;}break;
            default:break;
            }
            ++count;
        }
        current=current->next;
    }
    return count;
}
void Chain::release(){
    for(auto* root:{&calculation,&drawing}){
        // Snapshot first: deletion callbacks may remove other jobs or append jobs.
        std::vector<ChainElement*> pending;
        for(auto* current=root;current;current=current->next)pending.push_back(current);
        for(auto* element:pending)cut(element);
    }
}
}
