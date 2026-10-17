#include "StageRuntime.hpp"
namespace th08 {
AnmVm* StageRuntime::moon(){requests.push_back({StageRequest::Moon,{}});return &moon_vm;}
void StageRuntime::move_effects(const Vec3& offset){requests.push_back({StageRequest::MoveEffects,offset});}
void StageRuntime::sparkle(const Vec3& position){requests.push_back({StageRequest::Sparkle,position});}
void StageRuntime::reset(){
    loaded=false;script.release();state=BackgroundState{};context=BackgroundContext{};
    moon_vm=AnmVm{};requests.clear();executor.invalid=false;script.invalid=false;
    animations.release(4);animations.release(0);
}
bool StageRuntime::load_animation(i32 slot,const u8* bytes,u32 size){
    // A live ANM contains pointers retained by the background VMs. Replacement
    // is allowed only between stages, after reset has released those VMs.
    if(loaded||(slot!=0&&slot!=4))return false;
    return animations.load(slot,bytes,size)!=nullptr;
}
bool StageRuntime::load(const u8* bytes,u32 size,i32 index,bool practice){
    auto* background=animations.get(4);auto* text=animations.get(0);
    if(loaded||index<0||index>=9||!background||!text)return false;
    state=BackgroundState{};context={index,false,practice,false};state.stage=index;
    executor.invalid=false;requests.clear();moon_vm=AnmVm{};
    loaded=script.load(bytes,size,*background,*text);
    if(loaded)script.reset_camera();
    else{script.release();state=BackgroundState{};}
    return loaded;
}
bool StageRuntime::update(const FrameTiming& timing,bool paused,bool youkai){
    requests.clear();if(!loaded)return false;
    executor.timing=timing;context.paused=paused;context.youkai=youkai;
    const auto result=script.update();
    if(result==JobResult::Error||result==JobResult::Exit||script.invalid||executor.invalid){loaded=false;return false;}
    return true;
}
}
