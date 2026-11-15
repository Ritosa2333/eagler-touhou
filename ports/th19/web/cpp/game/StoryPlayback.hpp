#pragma once
#include "StorySession.hpp"
#include <vector>
namespace th19 {
// Timed MSG subset. Text/portrait page snapshots remain separate from the
// event clock, so an actor entry is not collapsed into a page change.
struct StoryPlayback {
    const StoryScene* scene=nullptr;
    i32 time=0,cursor=0,page_index=-1,music=-1;
    bool waiting=false,finished=false,bubble=false;
    bool visible[2]={false,false};
    bool spawn=false,signal=false;
    std::vector<StoryEvent> emitted;
    void begin(const StoryScene* s){*this={};scene=s;page_index=-1;music=-1;finished=!s;}
    const StoryPage* page()const{return scene&&page_index>=0&&page_index<scene->count?scene->pages+page_index:nullptr;}
    // Native MSG dispatcher 0x50fff0: only an accepted input at opcode 11
    // plays sound 0 (FUN_004bb8d0(0,0)). Opcode 10 is a flag.
    bool confirm(){if(!waiting||finished)return false;waiting=false;return true;}
    void tick(){
        spawn=signal=false;
        emitted.clear();
        if(!scene||finished||waiting)return;
        while(cursor<scene->event_count&&scene->events[cursor].time<=u32(time)){
            const auto&e=scene->events[cursor++];
            emitted.push_back(e);
            switch(e.op){
                case 0:finished=true;return;
                case 1:case 2:visible[e.op-1]=true;break;
                case 4:case 5:visible[e.op-4]=false;break;
                case 6:visible[0]=visible[1]=false;bubble=false;break;
                case 11:if(bubble&&e.arg<0){waiting=true;return;}break;
                case 12:signal=true;break;
                case 17:page_index=e.arg;bubble=true;break;
                case 18:bubble=false;break;
                case 19:music=e.arg;break;
                case 55:spawn=true;break;
            }
        }
        if(cursor>=scene->event_count)finished=true;
        else ++time;
    }
};
}
