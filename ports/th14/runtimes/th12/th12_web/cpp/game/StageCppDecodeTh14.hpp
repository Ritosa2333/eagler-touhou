#pragma once
#include "PortTitle.hpp"
#if PORT_TITLE==14 && defined(TH14_CPP_STAGE_DECODE)
#include <atomic>
#include <functional>
#include <cstdint>
#include "SceneCameraReadJobTh14.hpp"
#include "Stage.hpp"
#if !defined(__wasm__)
#include <chrono>
#include <thread>
#endif
namespace th11 {
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
class AnmResource;
#endif
// The loading worker proceeds directly from HUD into private Scene work.
// The same worker publishes the empty enemy owner before continuing Timeline. Completed
// phase data is immutable; each release publishes only that phase's fields.
class StageCppDecodeTh14 {
    std::atomic<int> requested{0},ready{0},initial{0},cancelled{0},failed{0};
    u32 selection=0;
    bool scene_follows_hud=false; // Set and read only by the owning worker.
    SceneCameraReadJobTh14 scene_camera;
    // Never visible to Main before Scene Ready. Failure/cancellation retains
    // this owner until the worker has joined, so its destructor runs on Main.
    std::unique_ptr<Stage> scene_owner;
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
    // 0 idle, 1 immutable file offered to Main, 2 Main completed its templates.
    std::atomic<int> file_state{0};
    AnmResource* file_resource=nullptr;
    u16 file_id=0;
#endif
    static void notify(std::atomic<int>& word)noexcept{
#if defined(__wasm__)
        __builtin_wasm_memory_atomic_notify(reinterpret_cast<int*>(&word),1);
#endif
    }
public:
    bool create_scene_owner(AnmManager& manager,AnmResource& background,AnmResource& text,
                            u32& color,u32 number,SceneCamera& world){
        if(scene_owner||number<1||number>6||cancelled.load(std::memory_order_acquire)||failure())return false;
        scene_owner=std::make_unique<Stage>(manager,background,text,color,u16((number&1)+3));
        scene_owner->active_camera=&world;return true;
    }
    bool initialize_scene_owner(const StageResource& resource,u32 number){
        if(!scene_owner||scene_camera.copied_words()!=75||cancelled.load(std::memory_order_acquire))return false;
        // initialize(false) copies private state and binds a timer; it does
        // not start VMs, touch ANM lists, upload textures, or publish camera.
        return scene_owner->initialize(resource,i32(number),scene_camera.snapshot(),false);
    }
    std::unique_ptr<Stage> take_scene_owner(){
        if(ready.load(std::memory_order_acquire)<2||cancelled.load(std::memory_order_acquire)||failure())return {};
        return std::move(scene_owner);
    }
    // Called only by the owning worker before initialized() releases access
    // to Main. Retail435f32 returns from RNG restoration straight into the
    // HUD loader; there is no first-phase request from the Main pump.
    bool begin_hud_before_initialized(u32 choice)noexcept{
        if(choice>=6||initial.load(std::memory_order_acquire)||
           cancelled.load(std::memory_order_acquire)||failure()||requested.load(std::memory_order_acquire)!=0)return false;
        selection=choice;scene_follows_hud=true;requested.store(1,std::memory_order_release);return true;
    }
    // Retail40d445 follows the blocking background loader on this same
    // worker. Capture here, before publishing Scene Ready to Main; scheduling
    // another host worker after Main observes Ready changes that read point.
    bool prepare_scene_storage(u32 count){
        if(!scene_owner||scene_camera.copied_words()!=0||cancelled.load(std::memory_order_acquire)||failure())return false;
        return scene_owner->prepare_object_storage(count);
    }
    bool capture_scene_camera(const SceneCamera& source){
        if(scene_camera.copied_words()!=0)return false;
        using Work=prototype::Work;
        return scene_camera.advance(source,[this](u32){
            return cancelled.load(std::memory_order_acquire)?Work::Failed:Work::Ready;
        },[&source](u32,u32 offset,u32& value){
            // Aligned wasm word loads retain the original ordered live reads.
            // This target-specific shared-memory path permits a mixed camera;
            // it does not promise a whole-camera atomic snapshot.
            value=__atomic_load_n(reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(&source)+offset),__ATOMIC_RELAXED);
            return Work::Ready;
        })==Work::Ready;
    }
    bool scene_camera_word(u32 index,u32 offset,u32& value)const noexcept{
        if(index>=75||offset!=SceneCameraReadJobTh14::source_offset(index)||
           ready.load(std::memory_order_acquire)<2||cancelled.load(std::memory_order_acquire)||
           scene_camera.copied_words()!=75)return false;
        std::memcpy(&value,reinterpret_cast<const u8*>(&scene_camera.snapshot())+offset,4);
        return true;
    }
    void initialized()noexcept{initial.store(1,std::memory_order_release);}
    bool initialization_ready()const noexcept{return initial.load(std::memory_order_acquire)!=0;}
    bool failure()const noexcept{return failed.load(std::memory_order_acquire)!=0;}
    void cancel()noexcept{cancelled.store(1,std::memory_order_release);notify(cancelled);requested.store(4,std::memory_order_release);notify(requested);
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
        file_state.store(3,std::memory_order_release);
#endif
    }
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
    bool publish_file(AnmResource& resource,u16 file){
        if(cancelled.load(std::memory_order_acquire)||failure()||file_state.load(std::memory_order_acquire)!=0)return false;
        file_resource=&resource;file_id=file;file_state.store(1,std::memory_order_release);
        for(;;){
            if(cancelled.load(std::memory_order_acquire))return false;
            // Retail47d0f0..47d107 sleeps for 1 ms before checking the
            // newly published file, even if Main already completed it.
            // Completion is a polled flag, not a wakeup event. Only
            // cancellation may interrupt this worker-owned timed wait.
#if defined(__wasm__)
            __builtin_wasm_memory_atomic_wait32(reinterpret_cast<int*>(&cancelled),0,1000000);
#else
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
#endif
            if(cancelled.load(std::memory_order_acquire))return false;
            const auto value=file_state.load(std::memory_order_acquire);
            if(value==2){file_state.store(0,std::memory_order_release);return true;}
            if(value!=1)return false;
        }
    }
    bool pending_file(AnmResource*& resource,u16& file)const noexcept{
        if(cancelled.load(std::memory_order_acquire)||file_state.load(std::memory_order_acquire)!=1)return false;
        resource=file_resource;file=file_id;return true;
    }
    bool complete_file(AnmResource& resource,u16 file)noexcept{
        if(cancelled.load(std::memory_order_acquire)||file_state.load(std::memory_order_acquire)!=1||file_resource!=&resource||file_id!=file)return false;
        file_state.store(2,std::memory_order_release);return true;
    }
#endif
    // 0 Waiting, 1 Ready, 2 Failed, matching StageResourceEffects::DecodeResult.
    u32 request(u32 phase,u32 choice)noexcept{
        if(phase>=3||cancelled.load(std::memory_order_acquire)||failure())return 2;
        const int wanted=int(phase+1),current=requested.load(std::memory_order_acquire);
        if(current>=wanted){if(phase==0&&selection!=choice)return 2;}
        else{
            if(current!=wanted-1||ready.load(std::memory_order_acquire)!=current)return 2;
            if(phase==0)selection=choice;
            requested.store(wanted,std::memory_order_release);notify(requested);
        }
        return ready.load(std::memory_order_acquire)>=wanted?1:0;
    }
    bool run(const std::function<bool(u32,u32)>& decode){
        for(int phase=1;phase<=3;++phase){
            for(;;){
                const int value=requested.load(std::memory_order_acquire);
                if(cancelled.load(std::memory_order_acquire))return false;
                if(value==phase)break;
                if(value!=phase-1){failed.store(1,std::memory_order_release);return false;}
#if defined(__wasm__)
                __builtin_wasm_memory_atomic_wait32(reinterpret_cast<int*>(&requested),value,1000000000);
#else
                std::this_thread::yield();
#endif
            }
            const u32 choice=selection;
            if(!decode(u32(phase-1),choice)){failed.store(1,std::memory_order_release);return false;}
            ready.store(phase,std::memory_order_release);
            if((phase==1||phase==2)&&scene_follows_hud){
                // Retail435f3d..435f4b calls Scene immediately after HUD.
                // A simultaneous Main Scene request is equivalent; never
                // overwrite a later request or the cancellation sentinel.
                // Scene also continues directly into the manager/timeline
                // loader; a Main readiness poll must not gate that work.
                int previous=phase;
                requested.compare_exchange_strong(previous,phase+1,std::memory_order_acq_rel);
            }
        }
        return true;
    }
};
}
#endif
