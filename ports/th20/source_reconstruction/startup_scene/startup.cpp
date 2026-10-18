#include "startup.hpp"
#include "data_constants.hpp"
#ifdef TH_SDL3
#include "platform/Time.hpp"
#include "platform/Files.hpp"
#endif
#include "../platform_window/graphics_callbacks.hpp"
#include "../platform_window/frame_statistics.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../audio_runtime/audio.hpp"
#include "../game_session/session.hpp"
#include "../text_renderer/text.hpp"
#include <atomic>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <new>

namespace th20::source::startup {
namespace pe=program_entry;
namespace pw=platform_window;
LoadingScene* loading_scene=nullptr;
#ifdef TH_SDL3
static std::chrono::steady_clock::time_point startup_image_at;
static bool startup_resources_prepared=false;
#endif
namespace {
int __cdecl update_callback(void* object) {return static_cast<LoadingScene*>(object)->update();} //4d85a0
int __cdecl draw_callback(void* object) {return static_cast<LoadingScene*>(object)->draw();} //4d85b0
bool file_exists(const char* name) {
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(2));
#ifdef TH_SDL3
    // Loose game files live under the launcher-mounted /game tree; route
    // through the file host instead of the bare process CWD.
    return web::files::exists(name);
#else
    return std::filesystem::exists(name);
#endif
}
int finish_loading() {
    audio::bind_game_services();auto& audio=pe::thread_registry;
    if(audio.load_formats("../../bgm/thbgm.fmt")!=0) runtime::log_printf(pe::log_buffer,data::audio_format_error);
    audio.initialize(pe::window_state.window,*audio.context);audio.apply_configuration();
    if(file_exists("thbgm.dat")) {
        if(!(pe::graphics_state.configuration.flags&0x10)) audio.start_stream("thbgm.dat");
        else strcpy_s(audio.music_file,0x20,"thbgm.dat");
    }
    initialize_shared_scene_resources();
#ifdef TH_SDL3
    while(!sprite::animation_files_ready(*pe::sprite_controller,pe::graphics_state.event_flags))
        pw::unrecovered::pump_background_jobs(*pe::sprite_controller);
#else
    while(!sprite::animation_files_ready(*pe::sprite_controller,pe::graphics_state.event_flags)) Sleep(1);
#endif
    scheduler::enable(*loading_scene->update_node);
    auto* statistics=static_cast<pw::FrameStatistics*>(pw::unrecovered::scheduler_object_005c4a00);
    statistics->end_times[0]=std::chrono::steady_clock::now().time_since_epoch().count();
    return 0;
}
}
LoadingScene::LoadingScene() {
    field_20=0;sprite::construct_animation(animation);animation_handle=0;
    signature_file=nullptr;signature_ready=0;text_ready=0;draw_frames=0;
    loading_scene=this;flags|=2;
#ifdef TH_SDL3
    startup_resources_prepared=false;
#endif
}
LoadingScene::~LoadingScene() {
    runtime::join_worker(worker);
    scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);
    scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);
    release_shared_scene_resources();sprite::unload_animation_file(*pe::sprite_controller,1);
    loading_scene=nullptr;runtime::retire_callback_owner(text::renderer);
    sprite::unload_animation_file(*pe::sprite_controller,0);unrecovered::release_loading_cache();
    sprite::file_environment::destroy_animation_contents(animation);
    // Real Worker and CallbackOwner destructors follow this body, in original order.
}
int LoadingScene::register_callbacks() {
    update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,7,update_callback,this,false,false);
    draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,0x58,draw_callback,this,true,false);
    unrecovered::initialize_loading_cache();
    {
        std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(6));
        {
            std::lock_guard<std::recursive_mutex> nested(runtime::shared_locks().slot(6));
            if(worker.thread.joinable()) worker.thread.detach();
        }
        worker.close_requested.store(false,std::memory_order_seq_cst);
        worker.thread=TH20_WORKER_THREAD("startup-load",[](LoadingScene*) {load_worker();},this);
    }
    game_session::bind_default_player();return 0;
}
int LoadingScene::update() {
    if(flags&2) {
#ifdef TH_SDL3
        // The inline browser loader used to finish before its first draw.
        // Give the newly spawned signature/text one completed frame, then
        // prepare everything else and enter Title as soon as it is ready.
        if(pe::graphics_state.field_0b0c==3)return 1;
        if(signature_ready!=2||text_ready!=2||draw_frames<2)return 1;
        if(!startup_resources_prepared){finish_loading();startup_resources_prepared=true;}
        if(std::chrono::steady_clock::now()-startup_image_at<std::chrono::seconds(2))return 1;
#endif
        pw::acquire_render_surfaces(pe::graphics_state);pe::window_state.input_latch=1;
        text::renderer->enable_callbacks();pe::graphics_state.event_flags&=~0x200u;
        pe::graphics_state.field_0b0c=4;flags&=~2u;
    }
    return 1;
}
int LoadingScene::draw() {
    // Original readiness words cross the loading/draw threads. atomic_ref
    // retains their four-byte storage while making those handoffs valid C++.
    if(std::atomic_ref(signature_ready).load(std::memory_order_acquire)==1) {
        animation_handle=sprite::spawn_named_animation(*pe::sprite_controller,*signature_file,"sig",0,-1);
        std::atomic_ref(signature_ready).fetch_add(1,std::memory_order_release);
    }
    if(std::atomic_ref(text_ready).load(std::memory_order_acquire)==1) {
        text::renderer->create_loading_text(data::text_x,data::text_y);
        std::atomic_ref(text_ready).fetch_add(1,std::memory_order_release);
    }
#ifdef TH_SDL3
    if(draw_frames==0)startup_image_at=std::chrono::steady_clock::now();
#endif
    ++draw_frames;return 1;
}
LoadingScene* create_loading_scene() {
    void* memory=::operator new(sizeof(LoadingScene),std::nothrow);if(!memory) return nullptr;
    std::memset(memory,0,sizeof(LoadingScene));LoadingScene* object;
    try {object=::new(memory) LoadingScene;} catch(...) {::operator delete(memory);throw;}
    if(object->register_callbacks()!=0) {runtime::retire_callback_owner(object);return nullptr;}
    return object;
}
int load_worker() {
#ifdef TH_SDL3
    // The original stalls two seconds on a real thread before loading; the
    // browser runtime has no blockable thread and starts loading immediately.
    web::time::sleep(2000);
#else
    Sleep(2000);
#endif
    auto& self=*loading_scene;
    self.signature_file=sprite::load_animation_file(*pe::sprite_controller,1,"sig.anm",pe::log_buffer,pe::graphics_state.event_flags);
    if(self.signature_file) {
        scheduler::enable(*self.draw_node);std::atomic_ref(self.signature_ready).store(1,std::memory_order_release);
        pe::graphics_state.surface_animation=sprite::load_animation_file(*pe::sprite_controller,0,"text.anm",pe::log_buffer,pe::graphics_state.event_flags);
        if(pe::graphics_state.surface_animation) {
            if(text::create_renderer()) {
                std::atomic_ref(self.text_ready).store(1,std::memory_order_release);
#ifdef TH_SDL3
                // Only the two startup banks must be ready before the first
                // visible frame. Shared/game resources are prepared afterwards.
                while(!sprite::animation_files_ready(*pe::sprite_controller,pe::graphics_state.event_flags))
                    pw::unrecovered::pump_background_jobs(*pe::sprite_controller);
                scheduler::enable(*self.update_node);return 0;
#else
                return finish_loading();
#endif
            }
            runtime::log_printf(pe::log_buffer,data::text_renderer_error);
        }
    }
    pe::graphics_state.field_0b0c=3;scheduler::enable(*self.update_node);return 0;
}
int initialize_shared_scene_resources() {
    if(unrecovered::initialize_resource_004b5900()!=0) return -1;
    if(!unrecovered::create_resource_0049e0c0(0)) return -1;
    if(!unrecovered::create_resource_00534dd0(0)) return -1;
    if(!unrecovered::create_resource_0051cc20(0)) return -1;
    // Original4a7700 is exactly `xor eax,eax; ret`; its three calls have no state effects.
    return unrecovered::initialize_resource_0052e210()!=0?-1:0;
}
int release_shared_scene_resources() {
    unrecovered::release_resource_0049deb0(0);unrecovered::release_resource_00534110(0);
    unrecovered::release_resource_0051b6d0();unrecovered::release_resource_004b6560();
    unrecovered::release_resource_0052e730();return 0;
}
void shutdown_scene_objects() {
    game_session::clear_game_mode_flags();auto& flags=pe::graphics_state.event_flags;
    if(((flags>>5)&3)==0) flags=(flags&~0x60u)|0x20;
    runtime::retire_callback_owner(unrecovered::owner_005c4d28);
    runtime::retire_callback_owner(unrecovered::owner_005c60b8);
    pw::unrecovered::destroy_game();runtime::retire_callback_owner(pw::unrecovered::menu_scene);
    runtime::retire_callback_owner(loading_scene);runtime::retire_callback_owner(pw::unrecovered::ending_scene);
    runtime::retire_callback_owner(unrecovered::owner_005c60fc);
    unrecovered::release_resource_0049deb0(0);unrecovered::release_resource_00534110(0);
    runtime::retire_callback_owner(unrecovered::owner_005c4d24);runtime::retire_callback_owner(unrecovered::owner_005c5b38);
    runtime::retire_callback_owner(unrecovered::owner_005c6114);
}
}
namespace th20::source::platform_window::unrecovered {
runtime::CallbackOwner* create_startup_scene() {return startup::create_loading_scene();}
void shutdown_scene_objects() {startup::shutdown_scene_objects();}
}
