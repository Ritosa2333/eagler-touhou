#pragma once
#include "GameResources.hpp"
#include "StageCppWorkerTh14.hpp"
#include "BootFileMailboxTh14.hpp"
#include "LoadingWordTh14.hpp"
#include <array>
namespace th11 {
// Retail459570 owns a separate title thread. Main keeps all mutable ANM
// manager/device operations. Ordered retirement publication is a host boundary.
class TitleCppResourcesTh14 {
    GameResources reader;
    const std::array<AnmResource*,2> files;
    std::atomic<u32> signature_present{0},signature_draws{0},host_flags{0};
    const u32* live_host_flags=nullptr;
    const std::atomic<u32>* live_signature_draws=nullptr;
#if defined(TH14_CPP_STAGE_DECODE)
    LoadingWordTh14<u32>* effect_stop=nullptr;
#endif
    BootFileMailboxTh14 mailbox;
    StageCppWorkerTh14 worker;
    std::atomic<u32> resources_ready{0},retirement_requested{0},title_ready{0},stopping{0};
    bool started=false;
    std::string error;
    u32 current_host_flags()const noexcept{
        return live_host_flags?__atomic_load_n(live_host_flags,__ATOMIC_ACQUIRE):host_flags.load(std::memory_order_acquire);
    }
    u32 current_signature_draws()const noexcept{
        // Retail45959a/4595c5 reads Boot+60c itself, not a later Main copy.
        return live_signature_draws?live_signature_draws->load(std::memory_order_acquire)
            :signature_draws.load(std::memory_order_acquire);
    }
    void sleep16()noexcept{
#if defined(__wasm__)
        // Draw publications never notify this word. Only cancellation wakes it.
        __builtin_wasm_memory_atomic_wait32(reinterpret_cast<int*>(&stopping),0,16000000);
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
#endif
    }
public:
#if defined(TH14_CPP_STAGE_DECODE)
    TitleCppResourcesTh14(std::array<AnmResource*,2> resources,const u32* live_flags=nullptr,LoadingWordTh14<u32>* stop=nullptr,const std::atomic<u32>* live_draws=nullptr):files(resources),live_host_flags(live_flags),live_signature_draws(live_draws),effect_stop(stop),mailbox(live_flags,host_flags){}
#else
    TitleCppResourcesTh14(std::array<AnmResource*,2> resources,const u32* live_flags=nullptr,const std::atomic<u32>* live_draws=nullptr):files(resources),live_host_flags(live_flags),live_signature_draws(live_draws),mailbox(live_flags,host_flags){}
#endif
    ~TitleCppResourcesTh14(){cancel_and_join();}
    void publish_loading_state(bool present,u32 draws,u32 flags)noexcept{
        signature_present.store(present,std::memory_order_release);
        signature_draws.store(draws,std::memory_order_release);
        host_flags.store(flags,std::memory_order_release);
    }
    bool begin(const u8* bytes,u32 size){
        if(started||!files[0]||!files[1])return false;
        started=true;
        return worker.start([this,bytes,size](){
            if(!reader.open_archive(bytes,size)){error=reader.error();return false;}
            constexpr const char* names[]={"title.anm","title_v.anm"};
            for(u32 i=0;i<2;++i){
                if(stopping.load(std::memory_order_acquire))return false;
                if(!reader.open_anm(names[i],*files[i])){error=reader.error();return false;}
                if(!mailbox.publish_file(*files[i],u16(16+i)))return false;
            }
            resources_ready.store(1,std::memory_order_release);
#if defined(TH14_CPP_STAGE_DECODE)
            if(effect_stop)*effect_stop=0; // Native4596b4, before signature wait.
#endif
            if(signature_present.load(std::memory_order_acquire)){
                // Original checks draws before entering its Sleep16 loop and
                // bypasses that sleep when either live host bit80/100 is set.
                while(!stopping.load(std::memory_order_acquire)
                   &&signed_bits(current_signature_draws())<180
                   &&!(current_host_flags()&0x180))sleep16();
                if(stopping.load(std::memory_order_acquire))return false;
                retirement_requested.store(1,std::memory_order_release);
                // No host round trip exists between retail retirement and
                // title enable. Main consumes this request before title ready.
            }
            if(stopping.load(std::memory_order_acquire))return false;
            title_ready.store(1,std::memory_order_release);return true;
        });
    }
    bool pending(AnmResource*& resource,u16& file)const noexcept{return mailbox.pending_file(resource,file);}
    bool complete(AnmResource& resource,u16 file)noexcept{return mailbox.complete_file(resource,file);}
    u32 publication(u32 word)const noexcept{
        return word==0?resources_ready.load(std::memory_order_acquire)
            :word==1?retirement_requested.load(std::memory_order_acquire):title_ready.load(std::memory_order_acquire);
    }
    u32 poll()const noexcept{return u32(worker.poll());}
    void join()noexcept{worker.join();}
    const std::string& last_error()const noexcept{return error;}
    void cancel_and_join()noexcept{
        stopping.store(1,std::memory_order_release);mailbox.cancel();
#if defined(__wasm__)
        __builtin_wasm_memory_atomic_notify(reinterpret_cast<int*>(&stopping),1);
#endif
        worker.cancel_and_join();
    }
};
}
