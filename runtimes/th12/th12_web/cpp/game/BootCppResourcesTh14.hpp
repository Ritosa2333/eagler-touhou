#pragma once
#include "GameSession.hpp"
#include "StageCppWorkerTh14.hpp"
#include "BootFileMailboxTh14.hpp"
#include <array>
namespace th11 {
// External resources have stable addresses through cancel/join and VM retirement.
// Only Main prepares templates and touches the compositor or graphics device.
class BootCppResourcesTh14 {
    GameSession& session;
    GameResources reader;
    const std::array<AnmResource*,6> files;
    const char* const ascii_name;
    BootFileMailboxTh14 mailbox;
    StageCppWorkerTh14 worker;
    std::atomic<u32> signature_callback{0},signature_ready{0},ascii_ready{0},common_ready{0},stopping{0};
    bool started=false,score_defaults_initialized=false;
    std::string error;
    std::function<bool(GameResources&,std::string&)> after_text;
public:
    static constexpr u16 ids[]={1,2,0,5,7,8};
    BootCppResourcesTh14(GameSession& value,std::array<AnmResource*,6> resources,
        std::function<bool(GameResources&,std::string&)> audio_finish={}):session(value),files(resources),
        ascii_name(value.resources.ascii_anm_name()),mailbox(value.stage_load_host_flags),after_text(std::move(audio_finish)){}
    ~BootCppResourcesTh14(){cancel_and_join();}
    bool begin(const u8* bytes,u32 size){
        if(started)return false;
        for(auto* file:files)if(!file)return false;
        started=true;
        return worker.start([this,bytes,size](){
            if(!reader.open_archive(bytes,size)){error=reader.error();return false;}
            const char* names[]={"sig.anm",ascii_name,"text.anm","front.anm","bullet.anm","effect.anm"};
            for(u32 i=0;i<files.size();++i){
                if(stopping.load(std::memory_order_acquire))return false;
                if(!reader.open_anm(names[i],*files[i])){error=reader.error();return false;}
                if(!mailbox.publish_file(*files[i],ids[i]))return false;
                if(i==0){signature_callback.store(1,std::memory_order_release);signature_ready.store(1,std::memory_order_release);}
                if(i==1)ascii_ready.store(1,std::memory_order_release);
                // Retail444230 reads thbgm.fmt after Text completion, then
                // 46e240 signals and joins the early audio workers.
                if(i==2&&after_text&&!after_text(reader,error))return false;
            }
            // Retail4442a8 enables the Main priority3 initializer before
            // 4442af constructs scores. Main can run while defaults are built.
            common_ready.store(1,std::memory_order_release);
            if(stopping.load(std::memory_order_acquire))return false;
            session.initialize_score_defaults();score_defaults_initialized=true;
            return true;
        });
    }
    bool pending(AnmResource*& resource,u16& id)const noexcept{return mailbox.pending_file(resource,id);}
    bool complete(AnmResource& resource,u16 id)noexcept{return mailbox.complete_file(resource,id);}
    u32 publication(u32 word)const noexcept{
        return word==0?signature_callback.load(std::memory_order_acquire):word==1?signature_ready.load(std::memory_order_acquire):word==2?ascii_ready.load(std::memory_order_acquire):common_ready.load(std::memory_order_acquire);
    }
    // Call only after join, which retires the sole writer.
    bool scores_initialized_after_join()const noexcept{return score_defaults_initialized;}
    const std::string& last_error()const noexcept{return error;}
    u32 poll()const noexcept{return u32(worker.poll());}
    void join()noexcept{worker.join();}
    void cancel_and_join()noexcept{stopping.store(1,std::memory_order_release);mailbox.cancel();worker.cancel_and_join();}
};
}
