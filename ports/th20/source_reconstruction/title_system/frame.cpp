#include "frame.hpp"
#include "pages.hpp"
#include "data.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_window/graphics_callbacks.hpp"
#include "../gameplay/frame.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include "../gameplay/stage_data.hpp"
#include "../stone_menu/stone.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../runtime_state/state.hpp"
#include "../progress_state/manager.hpp"
#include "../audio_runtime/audio.hpp"
#include "../hud_system/dialogue.hpp"
#include "../screen_effect/effect.hpp"
#include <random>
namespace th20::source::title {
namespace pe=program_entry;namespace pw=platform_window::unrecovered;
int data_character=0,data_profile=0,last_profile=0;
void stop_music(){pe::thread_registry.enqueue((pe::graphics_state.configuration.flags&16)?4:3,0,"dummy");}
void play_music(const char* name){std::string filename(name);filename+=".wav";pe::thread_registry.enqueue(1,0,filename.c_str());hud::play_stage_track(0,0);}
namespace {
struct Environment final:FrameEnvironment {
    Environment():FrameEnvironment(main_environment(),pw::menu_selection,gameplay::slowdown_frames,gameplay::replay_selection,gameplay::replay_file,pe::graphics_state.field_0b0c,pe::thread_registry.queued_track[0],title::data_character,title::data_profile){}
    ending::EndingInf* current_ending()override{return ending::controller();}
    const input::ButtonState* buttons()override{return input::button_slot(0);}
    replay::ReplayInf* read_replay(const char* name)override{return replay::read_metadata(name);}
    void retire(runtime::CallbackOwner* p)override{runtime::retire_callback_owner(p);}
    void select_stage(int stage)override{gameplay::select_stage(main.session.player_table,stage);}
    void select_profile(int character,int profile)override{progress::manager->select_profile(0,character,profile);}
    void stop_music()override{title::stop_music();}
    void play_music(const char* name)override{title::play_music(name);}
    std::uint32_t entropy()override{return std::random_device{}();}
    void seed(int index,std::uint32_t value)override{state::seed(state::random_streams[index],value);}
    void enable_stones()override{stone_menu::controller->enable_callbacks();}
    void release_mesh(sprite::RenderMesh* p)override{sprite::destroy_render_mesh(p);}
    void hide_file(int index)override{sprite::mark_file_animations(*pe::sprite_controller,pe::sprite_controller->files[index],false);}
    void clear_loading()override{auto& handle=text::renderer->loading_handle;sprite::interrupt_animation_children(*pe::sprite_controller,handle,1);if(auto* animation=sprite::find_animation(*pe::sprite_controller,handle))sprite::hide_animation_tree(*animation);sprite::request_animation_deletion(*pe::sprite_controller,handle);}
    void background(TitleInf& o)override{initialize_background(o);}
    void transition_effect()override{screen::create_effect(9,30,0,0,0,109);}
    void request_scene(int scene)override{gameplay::request_scene(scene);}
    void update_page(TitleInf& o,int page)override{
        switch(page){
        case 1:update_main_menu(o,main);break;case 3:update_options(o);break;
        case 5:unrecovered::update_difficulty_00521bc0(o);break;case 6:unrecovered::update_character_00521060(o);break;case 7:unrecovered::update_loadout_0052b8e0(o);break;
        case 8:unrecovered::update_stage_00529430(o);break;case 10:unrecovered::update_player_data_005265a0(o);break;case 11:unrecovered::update_data_detail_00524c10(o);break;
        case 12:unrecovered::update_replay_00523440(o);break;case 14:unrecovered::update_music_005205d0(o);break;case 15:unrecovered::update_spell_005223e0(o);break;case 16:unrecovered::update_data_page_00526a90(o);break;
        case 17:update_help(o);break;case 18:unrecovered::update_gallery_00528fc0(o);break;case 19:unrecovered::update_gallery_page_005284f0(o);break;case 20:unrecovered::update_gallery_choice_00527f00(o);break;case 23:unrecovered::update_stones_0052a8a0(o);break;
        }
    }
    void draw_page(TitleInf& o,int page)override{
        switch(page){case 1:draw_main_menu(o,*text::renderer);break;case 8:unrecovered::draw_stage_00529b40(o);break;case 11:unrecovered::draw_data_detail_005257f0(o);break;case 12:unrecovered::draw_replay_005240d0(o);break;case 14:unrecovered::draw_music_00520c80(o);break;case 15:unrecovered::draw_spell_00522e20(o);break;case 16:unrecovered::draw_data_page_005277f0(o);break;case 19:case 20:unrecovered::draw_gallery_00528a50(o);break;case 23:unrecovered::draw_stones_0052b480(o);break;}
    }
};
}
FrameEnvironment& frame_environment(){static Environment e;return e;}
int update(TitleInf& o){return update(o,frame_environment());}
int draw(TitleInf& o){return draw(o,frame_environment());}
}

