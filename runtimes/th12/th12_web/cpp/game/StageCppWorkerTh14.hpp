#pragma once
#include "PortTitle.hpp"
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER)
#include "ReplayRngRestoreTh14.hpp"
#include <atomic>
#include <cstdlib>
#include <functional>
#include <pthread.h>
#include <cstdint>
#if defined(__EMSCRIPTEN__)
#include <cerrno>
#endif
#if !defined(__wasm__)
#include <chrono>
#include <thread>
#endif

namespace th11 {
// Only this session's initialization callback bypasses main-thread polling.
// Other sessions and the main thread retain their ordinary boundary hooks.
class StageCppWorkerScopeTh14 {
    const void* previous;
    static inline thread_local const void* active=nullptr;
public:
    explicit StageCppWorkerScopeTh14(const void* session)noexcept:previous(active){active=session;}
    ~StageCppWorkerScopeTh14(){active=previous;}
    static bool owns(const void* session)noexcept{return active==session;}
    StageCppWorkerScopeTh14(const StageCppWorkerScopeTh14&)=delete;
};

#if defined(TH14_CPP_STAGE_DECODE) && defined(TH14_WORKER_RETURN_OBSERVER)
class StageCppWorkerTh14;
extern "C" void th14_observer_worker_return(const StageCppWorkerTh14*)noexcept;
#endif
// The language runtime owns the worker's stack/TLS. It executes the actual C++
// function on the shared heap, without a second leaf-store Wasm module.
class StageCppWorkerTh14 {
    enum class State:unsigned {Idle,Running,Ready,Failed,Cancelled};
    std::atomic<State> state{State::Idle};
    std::atomic<int> stopping{0};
#if defined(TH14_CPP_STAGE_DECODE)
    // Supervisor+99c and+998 are separate native DWORD publications.
    // They describe the loading body, not pthread return or owner retirement.
    std::atomic<u32> loading_busy{0},loading_finished{0};
#endif
    std::int64_t minimum_wait_ns=0;
    pthread_t thread{};
    bool joinable=false;
    std::function<bool()> work;
#if defined(TH14_CPP_STAGE_DECODE)
    std::function<void()> entered;
#endif
    std::function<ReplayRngRestoreResultTh14()> prerequisite;
    static void wait(StageCppWorkerTh14& job,std::int64_t ns)noexcept{
#if defined(__wasm__)
        __builtin_wasm_memory_atomic_wait32(reinterpret_cast<int*>(&job.stopping),0,ns);
#else
        std::this_thread::sleep_for(std::chrono::nanoseconds(ns));
#endif
    }
    static void* run(void* context)noexcept{
        auto& job=*static_cast<StageCppWorkerTh14*>(context);
#if defined(TH14_CPP_STAGE_DECODE)
        if(job.entered)job.entered();
#endif
        // Retail435acf..435b02 polls the live first capture record on this
        // worker. The conditional60ms sleep starts only after it retires.
        while(!job.stopping.load(std::memory_order_acquire)&&job.prerequisite){
            const auto result=job.prerequisite();
            if(result==ReplayRngRestoreResultTh14::Ready)break;
            if(result==ReplayRngRestoreResultTh14::Failed){
#if defined(TH14_CPP_STAGE_DECODE)
                job.publish_loading_completion();
#endif
                job.state.store(State::Failed,std::memory_order_release);return nullptr;
            }
            wait(job,1000000);
        }
        if(!job.stopping.load(std::memory_order_acquire)&&job.minimum_wait_ns>0){
            // Sleep on the worker, independent of main-frame/RNG callbacks.
            // Only cancellation notifies this private wait word.
            wait(job,job.minimum_wait_ns);
        }
        if(job.stopping.load(std::memory_order_acquire)){
#if defined(TH14_CPP_STAGE_DECODE)
            job.publish_loading_completion();
#endif
            job.state.store(State::Cancelled,std::memory_order_release);
        }else{
            const bool succeeded=job.work();
#if defined(TH14_CPP_STAGE_DECODE) && defined(TH14_WORKER_RETURN_OBSERVER)
            // Diagnostic-only boundary after all work, before State/return.
            th14_observer_worker_return(&job);
#endif
#if defined(TH14_CPP_STAGE_DECODE)
            // Successful production callbacks publish at436067/436071,
            // before their copy interrupts. Retain a terminal fallback for
            // failed/cancelled callbacks that never reach that success tail.
            job.publish_loading_completion();
#endif
            job.state.store(succeeded?State::Ready:State::Failed,std::memory_order_release);
        }
        return nullptr;
    }
public:
    ~StageCppWorkerTh14(){cancel_and_join();}
    bool start(std::function<bool()> callback,std::int64_t minimum_ns=0,
               std::function<ReplayRngRestoreResultTh14()> before_wait={}
#if defined(TH14_CPP_STAGE_DECODE)
               ,std::function<void()> on_enter={}
#endif
               ){
        if(state.load(std::memory_order_acquire)!=State::Idle||stopping.load(std::memory_order_acquire))return false;
        if(minimum_ns<0)return false;
#if defined(TH14_CPP_STAGE_DECODE)
        entered=std::move(on_enter);
#endif
        work=std::move(callback);prerequisite=std::move(before_wait);minimum_wait_ns=minimum_ns;state.store(State::Running,std::memory_order_release);
#if defined(TH14_CPP_STAGE_DECODE)
        loading_busy.store(1,std::memory_order_seq_cst);loading_finished.store(0,std::memory_order_seq_cst);
#endif
        if(pthread_create(&thread,nullptr,run,this)!=0){
#if defined(TH14_CPP_STAGE_DECODE)
            publish_loading_completion();
#endif
            state.store(State::Failed,std::memory_order_release);return false;
        }
        joinable=true;return true;
    }
    ReplayRngRestoreResultTh14 poll()const noexcept{
        const auto value=state.load(std::memory_order_acquire);
        return value==State::Ready?ReplayRngRestoreResultTh14::Ready:
            value==State::Idle||value==State::Running?ReplayRngRestoreResultTh14::Waiting:ReplayRngRestoreResultTh14::Failed;
    }
    bool joined()const noexcept{return !joinable&&state.load(std::memory_order_acquire)!=State::Idle;}
#if defined(TH14_CPP_STAGE_DECODE)
    bool loading_callback_busy()const noexcept{return loading_busy.load(std::memory_order_seq_cst)!=0;}
    u32 loading_finished_word()const noexcept{return loading_finished.load(std::memory_order_seq_cst);}
    bool loading_callback_allows_close()const noexcept{
        // An allocated Idle job has not entered the Native loading request.
        return state.load(std::memory_order_acquire)!=State::Idle&&!loading_callback_busy();
    }
    void publish_loading_completion()noexcept{
        if(!loading_callback_busy())return;
        // Native436067 then436071; never substitute callback State::Ready.
        loading_busy.store(0,std::memory_order_seq_cst);loading_finished.store(1,std::memory_order_seq_cst);
    }
    bool stop_requested()const noexcept{return stopping.load(std::memory_order_acquire)!=0;}
    bool wait_for(std::int64_t nanoseconds)noexcept{
        if(nanoseconds<0||stop_requested())return false;
        wait(*this,nanoseconds);return !stop_requested();
    }
#endif
    void join()noexcept{
        if(!joinable)return;
        // An unjoined writer must never outlive its actual C++ owners.
#if defined(__EMSCRIPTEN__)
        // Emscripten's blocking API warns even when the target has exited.
        // Its tryjoin performs no blocking check on the completed path.
        const int result=pthread_tryjoin_np(thread,nullptr);
        if(result!=0&&(result!=EBUSY||pthread_join(thread,nullptr)!=0))std::abort();
#else
        if(pthread_join(thread,nullptr)!=0)std::abort();
#endif
        joinable=false;
    }
    void cancel_and_join()noexcept{
        stopping.store(1,std::memory_order_release);
#if defined(__wasm__)
        __builtin_wasm_memory_atomic_notify(reinterpret_cast<int*>(&stopping),1);
#endif
        State idle=State::Idle;state.compare_exchange_strong(idle,State::Cancelled,std::memory_order_acq_rel);
        join();
    }
    StageCppWorkerTh14()=default;
    StageCppWorkerTh14(const StageCppWorkerTh14&)=delete;
};
}
#endif
