#pragma once
#include "AudioDevice.hpp"
#include "../game/StageCppWorkerTh14.hpp"
#if defined(__EMSCRIPTEN__)
#include <emscripten/eventloop.h>
#endif
namespace th11::sdl {
// Native46d980 starts before Sig, and waits for the resource worker's
// 46e240 startup flag after loading the 65 sample files. SDL stays on Main.
class AudioCppBootTh14 {
    GameResources reader;
    AudioDevice prepared;
    StageCppWorkerTh14 loader;
    std::atomic<int> stopping{0};
    std::atomic<u32> startup{0},samples_ready{0},device_request{0};
    std::string loader_error,device_error;
    void sleep_one_ms()noexcept{
#if defined(__wasm__)
        __builtin_wasm_memory_atomic_wait32(reinterpret_cast<int*>(&stopping),0,1000000);
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
#endif
    }
public:
    ~AudioCppBootTh14(){
        signal_cancel();loader.cancel_and_join();
#if defined(__EMSCRIPTEN__)
        emscripten_runtime_keepalive_pop();
#endif
    }
    bool begin(const u8* bytes,u32 size){
        return loader.start([this,bytes,size](){
            if(!reader.open_archive(bytes,size)){loader_error=reader.error();return false;}
            if(!prepared.prepare_cpu(reader,[this](){return stopping.load(std::memory_order_acquire)!=0;})){
                loader_error=prepared.error;return false;
            }
            samples_ready.store(1,std::memory_order_release);
            // Publication of startup does not wake this real Sleep1 early.
            while(!startup.load(std::memory_order_acquire)&&!stopping.load(std::memory_order_acquire))sleep_one_ms();
            return !stopping.load(std::memory_order_acquire)&&startup.load(std::memory_order_acquire)==1;
        });
    }
    bool finish_after_text(GameResources& source,std::string& error){
        std::vector<u8> formats;
        if(!source.read("thbgm.fmt",formats)){
            error="Missing TH14 music layout";signal_cancel();loader.join();return false;
        }
        u32 expected=0;startup.compare_exchange_strong(expected,1,std::memory_order_acq_rel);
        // This join executes on the boot resource worker, never Main.
        loader.join();
        if(stopping.load(std::memory_order_acquire)||loader.poll()!=ReplayRngRestoreResultTh14::Ready){error=loader_error;return false;}
        prepared.prepare_formats(std::move(formats));
        device_request.store(1,std::memory_order_release);
        while(device_request.load(std::memory_order_acquire)==1&&!stopping.load(std::memory_order_acquire))sleep_one_ms();
        if(stopping.load(std::memory_order_acquire))return false;
        if(device_request.load(std::memory_order_acquire)!=2){error=device_error;return false;}
        return true;
    }
    void poll_main(AudioDevice& live){
        if(device_request.load(std::memory_order_acquire)!=1||stopping.load(std::memory_order_acquire))return;
        const bool ok=live.adopt_prepared(prepared);
        if(!ok)device_error=live.error;
        device_request.store(ok?2:3,std::memory_order_release);
    }
    u32 publication(u32 word)const noexcept{
        return word==0?samples_ready.load(std::memory_order_acquire):word==1?startup.load(std::memory_order_acquire):device_request.load(std::memory_order_acquire);
    }
    void signal_cancel()noexcept{
        stopping.store(1,std::memory_order_release);startup.store(2,std::memory_order_release);
#if defined(__wasm__)
        __builtin_wasm_memory_atomic_notify(reinterpret_cast<int*>(&stopping),2);
#endif
    }
    // Main must first signal_cancel, then join the parent resource worker,
    // and only then destroy this owner. There is exactly one loader joiner.
    AudioCppBootTh14(){
#if defined(__EMSCRIPTEN__)
        // A worker-side pthread_join posts an asynchronous cleanup callback
        // to Main. Keep the hosted runtime alive between RAF ccalls until
        // Main has joined the parent boot worker and destroys this owner.
        emscripten_runtime_keepalive_push();
#endif
    }
    AudioCppBootTh14(const AudioCppBootTh14&)=delete;
};
}
