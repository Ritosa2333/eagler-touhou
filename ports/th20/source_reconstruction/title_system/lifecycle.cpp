#include "lifecycle.hpp"
#include "../program_entry/program_entry.hpp"
#ifdef TH_SDL3
#include "platform/Time.hpp"
#endif
#include "../platform_window/graphics_callbacks.hpp"
#include "../startup_scene/startup.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../input/input.hpp"
#include "../gameplay/frame.hpp"
#include "data.hpp"
#include <cstring>
#include <new>
namespace th20::source::platform_window::unrecovered {
runtime::CallbackOwner* menu_scene=nullptr;
std::uint32_t menu_selection=0;
void create_menu(){title::create();}
}
namespace th20::source::title {
namespace pe=program_entry;
TitleInf* controller(){return static_cast<TitleInf*>(platform_window::unrecovered::menu_scene);}
namespace {
int __cdecl update_callback(void* value){return update(*static_cast<TitleInf*>(value));} //52c1e0
int __cdecl draw_callback(void* value){return draw(*static_cast<TitleInf*>(value));} //52c210
}
namespace {
struct Environment final:LifecycleEnvironment {
    void publish(TitleInf* o)override{platform_window::unrecovered::menu_scene=o;}
    scheduler::Node* register_callback(TitleInf& o,int priority,bool draw)override{return scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,priority,draw?draw_callback:update_callback,&o,draw,false);}
    sprite::AnimationFile* load_file(int slot,const char* name)override{return sprite::load_animation_file(*pe::sprite_controller,slot,name,pe::log_buffer,pe::graphics_state.event_flags);}
    void load_error()override{runtime::log_printf(pe::log_buffer,data::s_0056f610);}
    void remove(scheduler::Node* node)override{scheduler::remove(*pe::function_controller,pe::scheduler_environment,node);}
    void unload_file(int slot)override{sprite::unload_animation_file(*pe::sprite_controller,slot);}
    void retire(runtime::CallbackOwner* p)override{runtime::retire_callback_owner(p);}
    void interrupt(std::uint32_t handle)override{sprite::interrupt_animation_children(*pe::sprite_controller,handle,1);}
    void release_mesh(sprite::RenderMesh* p)override{sprite::destroy_render_mesh(p);}
    void rebuild_input()override{input::controller->rebuild_devices();}
};
}
LifecycleEnvironment& lifecycle_environment(){static Environment e;return e;}
int initialize(TitleInf& o){return initialize(o,lifecycle_environment());}
int load_worker(){
    if(initialize(*controller())==0){
#ifdef TH_SDL3
        // Threadless build: pump the per-frame background jobs inline until
        // the staged files are ready (no loader thread exists).
        while(!sprite::animation_files_ready(*pe::sprite_controller,pe::graphics_state.event_flags))
            platform_window::unrecovered::pump_background_jobs(*pe::sprite_controller);
#else
        while(!sprite::animation_files_ready(*pe::sprite_controller,pe::graphics_state.event_flags))Sleep(1);
#endif
        if(startup::loading_scene){
#ifdef TH_SDL3
            // The original waits for 180 drawn logo frames; the inline worker
            // occupies the render thread, so the brand delay cannot be
            // observed and is skipped (loading is already complete).
            // Cancel the pending logo draw before releasing its ANM. With an
            // inline worker, draw() has not consumed signature_ready yet; the
            // next frame would spawn from freed memory, corrupting whichever
            // allocation reused it (notably the title mesh's strip handles).
            auto& loading=*startup::loading_scene;
            scheduler::disable(*loading.draw_node);
            std::atomic_ref(loading.signature_ready).store(0,std::memory_order_release);
            std::atomic_ref(loading.text_ready).store(0,std::memory_order_release);
            loading.signature_file=nullptr;
#else
            while(startup::loading_scene->draw_frames<180&&!(pe::graphics_state.event_flags&0x60))Sleep(16);
#endif
            sprite::unload_animation_file(*pe::sprite_controller,1);}
        scheduler::enable(*controller()->update_node);pe::window_state.input_latch=1;
    }else gameplay::request_scene(3);
    return 0;
}
TitleInf* create(){
    void* memory=nullptr;
    {std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));memory=::operator new(sizeof(TitleInf),std::nothrow);}
    if(!memory)return nullptr;std::memset(memory,0,sizeof(TitleInf));auto* o=new(memory)TitleInf;
    pe::window_state.input_latch=0;
    {std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(6));runtime::detach_worker(o->worker);o->worker.close_requested.store(false,std::memory_order_seq_cst);o->worker.thread=TH20_WORKER_THREAD("title-load",[]{load_worker();});}
    return o;
}
}

