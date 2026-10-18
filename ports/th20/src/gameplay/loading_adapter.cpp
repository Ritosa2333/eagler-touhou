#include "loading_dependencies.hpp"
#ifdef TH_SDL3
#include "platform/Time.hpp"
#endif
#include "enemy.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_window/graphics_callbacks.hpp"
#include "../platform_window/frame_statistics.hpp"
#include "../sprite_renderer/file_tasks.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../platform_services/services.hpp"
#include "../audio_runtime/audio.hpp"
#include "../stage_background/background.hpp"
#include <atomic>
#include <string>
#include <stdexcept>
namespace pe=th20::source::program_entry;
namespace pw=th20::source::platform_window;
namespace th20::source::gameplay {
std::int32_t replay_selection=-1;
char replay_file[256]={};
std::uint32_t slowdown_frames=0;
std::uint32_t loading_overlay_state=0;
std::uint32_t loading_animation_handles[3]={};
namespace {
pw::FrameStatistics& statistics() {
    auto* result=static_cast<pw::FrameStatistics*>(pw::unrecovered::scheduler_object_005c4a00);
    if(!result)throw std::logic_error("Game loading requires live FrameStatistics");return *result;
}
int __cdecl game_update(void* game) {return unrecovered::update_game(*static_cast<GameController*>(game));}
int __cdecl game_draw(void* game) {return unrecovered::draw_game(*static_cast<GameController*>(game));}
class GameLoadingServices final:public LoadingServices {
public:
    game_session::Session& session() override {return game_session::session;}
    void begin_frame_interval() override {sprite::begin_frame_interval(statistics(),0);}
    bool sprite_task_pending() override {return static_cast<std::int32_t>(std::atomic_ref(pe::sprite_controller->draw_state[0][0]).load(std::memory_order_relaxed))>=0;}
    std::uint32_t graphics_events() override {return std::atomic_ref(pe::graphics_state.event_flags).load(std::memory_order_relaxed);}
    std::uint32_t new_game_state() override {return pe::graphics_state.field_0b18;}
    void sleep(std::uint32_t milliseconds) override {
#ifdef TH_SDL3
        // Inline loading worker: waits are satisfied by the per-frame
        // background jobs, so pump one round before yielding.
        pw::unrecovered::pump_background_jobs(*pe::sprite_controller);
        web::time::sleep(milliseconds);
#else
        Sleep(milliseconds);
#endif
    }
    void interrupt_surface(unsigned index,bool execute) override {
        auto& animation=*pe::graphics_state.surface_sprites[index];sprite::set_animation_interrupt(animation,2);
        if(execute)sprite::execute_animation(animation);
    }
    std::int32_t selected_profile(int slot,int character) override {return unrecovered::selected_profile(slot,character);}
    Profile& profile(int character,int index) override {return unrecovered::progress_profile(character,index);}
    void configure_overlay(int character) override {unrecovered::configure_overlay(character);}
    player_state::BombObserver* bomb_observer() override {return unrecovered::bomb_observer();}
    std::int32_t replay_selection() override {return gameplay::replay_selection;}
    const char* replay_path() override {return replay_file;}
    void register_callbacks(GameController& game) override {
        game.update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,0x13,game_update,&game,false,false);
        game.draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,2,game_draw,&game,true,false);
    }
    const platform::Configuration& configuration() override {return pe::graphics_state.configuration;}
    bool create(Entity entity,int index,const char* path) override {
        switch(entity) {
        case Entity::fn_004ffff0:return unrecovered::create_004ffff0(index)!=nullptr;
        case Entity::fn_0050a930:return unrecovered::create_0050a930(index,path)!=nullptr;
        case Entity::fn_00477880:return background::create_background(path,index)!=nullptr;
        case Entity::fn_004b9090:return unrecovered::create_004b9090()!=nullptr;
        case Entity::fn_00486630:return unrecovered::create_00486630(index)!=nullptr;
        case Entity::fn_004c5010:return unrecovered::create_004c5010(index)!=nullptr;
        case Entity::fn_004d7e40:return unrecovered::create_004d7e40(index)!=nullptr;
        case Entity::fn_004e6b80:return unrecovered::create_004e6b80()!=nullptr;
        case Entity::fn_005108d0:return unrecovered::create_005108d0(index)!=nullptr;
        case Entity::fn_004aba70:return create_enemy_controller(enemy_services(),index,path)!=nullptr;
        case Entity::fn_00478300:return unrecovered::create_00478300(index)!=nullptr;
        case Entity::fn_00488b20:return unrecovered::create_00488b20(index)!=nullptr;
        }
        throw std::logic_error("Unrecognized gameplay factory");
    }
    void create_auxiliary_owner() override {unrecovered::create_auxiliary_owner();}
    void reset_replay_owner() override {unrecovered::reset_replay_owner();}
    void reset_hud_owner() override {unrecovered::reset_hud_owner();}
    void reset_existing_stage() override {enemy_controller().reset_for_stage();}
    void commit_progress() override {unrecovered::commit_progress();}
    void queue_track(int index,const char* track) override {
        std::string name(track);name+=".wav";pe::thread_registry.enqueue(1,index,name.c_str());
    }
    void reset_frame_statistics() override {statistics().target_frames=0.0;statistics().actual_frames=0.0;}
    bool audio_commands_pending() override {return std::atomic_ref(pe::thread_registry.commands[0].type).load(std::memory_order_relaxed)!=0;}
    bool effects_ready() override {return unrecovered::effects_ready();}
    void finish_entity_initialization() override {unrecovered::finish_entity_initialization();}
    void finish_loading_display(bool success) override {
        auto& graphics=pe::graphics_state;
        if(graphics.field_0db4==1) {
            for(auto handle:loading_animation_handles)sprite::interrupt_animation_children(*pe::sprite_controller,handle,success?1:2);
            for(auto& handle:loading_animation_handles)handle=0;
            graphics.field_0db4=success?0:2;
        }
        if(loading_overlay_state)loading_overlay_state=0;
    }
    void clear_slowdown_counter() override {slowdown_frames=0;}
    double read_clock() override {return platform::read_clock(pe::window_state);}
};
}
LoadingServices& loading_services() {static GameLoadingServices services;return services;}
}
