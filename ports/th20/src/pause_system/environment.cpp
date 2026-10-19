#include <cstdio>
#include "pause.hpp"
#include "capture.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_services/services.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include "../stage_completion/playtime.hpp"
#include "../progress_state/records.hpp"
#include "../runtime_state/state.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../audio_runtime/music_stream.hpp"
#include "../hud_system/dialogue.hpp"
#include "../input/input.hpp"
#include "../replay_system/replay.hpp"
namespace th20::source::pause {
namespace pe=program_entry;
namespace {
class GameServices final:public Services {
public:
    gameplay::GameController& game()override{return *gameplay::controller;}
    game_session::Session& session()override{return game_session::session;}
    std::uint32_t& input_latch()override{return pe::window_state.input_latch;}
    std::uint32_t graphics_flags()override{return pe::graphics_state.event_flags;}
    int& replay_selection()override{return gameplay::replay_selection;}
    float clock_scale()override{return state::clock_scale;}
    void set_clock_scale(float value)override{state::set_clock_scale(value);}
    bool pressed(std::uint32_t mask)override{auto* buttons=input::button_slot(0);return buttons&&(buttons->pressed&mask)!=0;}
    void select_scene(int value,bool guarded)override{pe::graphics_state.field_0b0c=guarded&&(pe::graphics_state.event_flags&0x200u)?2:value;}
    void update_playtime()override{gameplay::accumulate_playtime();}
    void reset_playtime_origin()override{if(game().restart()==0)progress::write(&session(),0x2b0,platform::read_clock(pe::window_state));}
    sprite::AnimationFile* hud_file()override{return hud::controller->front_file;}
    std::uint32_t spawn_panel(sprite::AnimationFile& file,int script)override{std::uint32_t handle;sprite::spawn_named_animation(*pe::sprite_controller,file,handle,"front",script,nullptr,0.f,-1,4);return handle;}
    void delete_animation(std::uint32_t& handle)override{sprite::request_animation_deletion(*pe::sprite_controller,handle);}
    void interrupt_animation(std::uint32_t& handle,int event)override{sprite::interrupt_animation_children(*pe::sprite_controller,handle,event);}
    void capture_background(PauseInf& o)override{pause::capture_background(o);}
    void capture_practice_background(PauseInf& o)override{pause::capture_practice_background(o);}
    void stop_effects()override{pe::thread_registry.requests[0].id=-1;pe::thread_registry.stop_effects(-1);}
    void effect(int id)override{pe::thread_registry.request_effect(id,0);}
    void pause_music()override{pe::thread_registry.enqueue(6,0,"Pause");}
    int poll_audio()override{return pe::thread_registry.poll();}
    const char* music_name()override{return pe::thread_registry.queued_track;}
    double music_position()override{
#ifdef TH_SDL3
        if(!pe::thread_registry.stream){std::fprintf(stderr,"music_position: stream is null\n");return 0;}
#endif
        return pe::thread_registry.stream->playback_seconds();}
    void play_game_over_music()override{pe::thread_registry.enqueue(1,0,"th128_08.wav");hud::play_stage_track(0,0);}
    bool dialogue_present()override{return hud::controller->collecting!=nullptr;}
    void show_dialogue(bool visible)override{auto& d=*hud::controller->collecting;for(unsigned i=0;i<4;++i){show_animation(d.portraits[i],visible);show_animation(d.portrait_overlays[i],visible);}for(unsigned i:{0u,1u,2u,3u,4u,5u,7u,6u})show_animation(d.handles[i],visible);}
    void show_hud_message(bool visible)override{show_animation(hud::controller->handles_f8[8],visible);}
    void hide_hud_numbers()override{for(auto* a:hud::controller->number_animations)sprite::hide_animation_tree(*a);}
    bool replay_finished()override{return (replay::controller()->flags&2u)!=0;}
    void mark_replay_finished()override{replay::controller()->flags|=2;}
};
}
Services& services(){static GameServices value;return value;}
}
namespace th20::source::stage_completion::unrecovered {
void finish_practice(){pause::finish_practice(*pause::controller(),pause::services());}
void finish_replay(){pause::finish_replay(*pause::controller(),pause::services());}
}
namespace th20::source::hud::unrecovered {void finish_spell_004e5bd0(){pause::finish_game(*pause::controller(),pause::services());}}
namespace th20::source::gameplay::unrecovered {
runtime::CallbackOwner* create_004e6b80(){return pause::create();}
void finish_replay_004e5f30(){pause::finish_replay(*pause::controller(),pause::services());}
}
