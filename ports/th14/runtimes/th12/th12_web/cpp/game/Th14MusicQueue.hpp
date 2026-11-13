#pragma once
#include "PortTitle.hpp"
#if PORT_TITLE == 14
#include "Types.hpp"
#include <array>
#include <atomic>
#include <cstring>
#include <mutex>
#include <string>
namespace th11 {
// Retail46ef90 / audioCS4f57d8. Thirty-one admitted requests and a final
// empty slot. Queue head.command alone is read by the loading worker.
class Th14MusicQueue {
    friend struct Th14MusicQueueProbe;
    struct Slot {
        std::atomic<u32> command{0};u32 argument=0,progress=0;
        std::array<char,256> file{};
    };
    static_assert(sizeof(Slot)==0x10c);
    std::array<Slot,32> slots{};
    mutable std::recursive_mutex critical;
    void pop_locked(){
        // Match46e8d0: shift through the first empty slot, retaining bytes
        // in any subsequent unused slot. Do not batch-dispatch the new head.
        for(u32 i=0;i<31&&slots[i].command.load();++i){
            slots[i].command.store(slots[i+1].command.load());
            slots[i].argument=slots[i+1].argument;slots[i].progress=slots[i+1].progress;
            slots[i].file=slots[i+1].file;
        }
    }
public:
    struct Request {u32 command,argument,progress;const char* file;};
    enum class Result {Done,Advance,Keep,Failed};
    bool enqueue(u32 command,u32 argument,const char* file){
        if(!file||std::strlen(file)>=256)return false;
        const std::lock_guard<std::recursive_mutex> lock(critical);
        for(u32 i=0;i<31;++i)if(!slots[i].command.load()){
            auto& s=slots[i];s.command.store(command);s.argument=argument;
            std::memcpy(s.file.data(),file,std::strlen(file)+1);s.progress=0;return true;
        }
        return false; // Retail silently drops a request when every slot is full.
    }
    void enqueue_initial(u32 mode_flags,bool direct_sound,const char* first,const char* second){
        if(mode_flags&0x40)return;
        if((mode_flags&0x30)!=0x20)enqueue(direct_sound?4:3,0,"dummy");
        const char* names[]={first,second};
        for(u32 slot=0;slot<2;++slot){const std::string file=std::string(names[slot])+".wav";enqueue(1,slot,file.c_str());}
    }
    u32 head_command()const noexcept{return slots[0].command.load();}
    template<class Consumer> bool consume_one(Consumer&& consumer){
        const std::lock_guard<std::recursive_mutex> lock(critical);auto& s=slots[0];const u32 command=s.command.load();
        if(!command)return true;
        const auto result=consumer(Request{command,s.argument,s.progress,s.file.data()});
        if(result==Result::Done)pop_locked();else if(result==Result::Advance)++s.progress;
        return result!=Result::Failed;
    }
};
}
#endif
