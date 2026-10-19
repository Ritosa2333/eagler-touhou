#include "stage_select.hpp"
#include "stage_select_data.hpp"
#include "pages.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_window/graphics_callbacks.hpp"
#include "../input/input.hpp"
#include "../audio_runtime/audio.hpp"
#include "../effect_system/effect.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../hud_system/dialogue.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include "../gameplay/stage_data.hpp"
#include <algorithm>
namespace th20::source::title {
std::array<std::uint8_t,256> shortcut_keyboard{};
int read_shortcut_keyboard(std::span<std::uint8_t,256> keys){
    std::fill(keys.begin(),keys.end(),std::uint8_t{0});if(!program_entry::window_state.active)return 0;
#ifdef TH_SDL3
    return input::read_scan_keyboard(keys.data());
#else
    auto* keyboard=input::controller->keyboard;const auto result=keyboard->GetDeviceState(256,keys.data());if(result!=0)keyboard->Acquire();return 1;
#endif
}
int read_shortcut_keyboard(){return read_shortcut_keyboard(shortcut_keyboard);}
namespace {
class SourceStage final:public StageEnvironment {
public:SourceStage():StageEnvironment(game_session::session,platform_window::unrecovered::menu_selection,title::last_stage){}
private:
    bool pressed(unsigned mask) override{const auto* b=input::button_slot(0);return b&&(b->pressed&mask)!=0;}
    bool repeated(unsigned mask) override{const auto* b=input::button_slot(0);return b&&((b->pressed|b->repeat8)&mask)!=0;}
    const progress::Profile& profile() override{return *progress::current_profile();}
    int keyboard_number() override{const bool direct=read_shortcut_keyboard()!=0;for(int number=1;number<=9;++number)if(shortcut_keyboard[(direct?1:0x30)+number]&0x80)return number;return 0;}
    void spawn_heading(TitleInf& o) override{spawn(o,43);}
    void retire_heading(TitleInf& o) override{retire_animation(o,43);}
    void sound(int id) override{program_entry::thread_registry.request_effect(id,0);}
    void fade(float duration) override{hud::fade_stage_track(duration);}
    bool effects_ready() override{return effects::controller(0)->ready!=0;}
    void loading() override{text::renderer->create_loading_text(stage_select_data::f_0056cda8,stage_select_data::f_00570388);program_entry::graphics_state.unknown_01c4=effects::controller(0)->spawn(0,nullptr,nullptr,true);sprite::interrupt_animation_children(*program_entry::sprite_controller,program_entry::graphics_state.unknown_01c4,7);}
    void request_start(int stage) override{gameplay::select_stage(session.player_table,stage);gameplay::replay_selection=-1;program_entry::graphics_state.field_0b0c=7;}
} environment;
}
StageEnvironment& stage_environment(){return environment;}
namespace unrecovered {
void update_stage_00529430(TitleInf& o){update_stage(o,stage_environment());}
void draw_stage_00529b40(TitleInf& o){draw_stage(o,*text::renderer,*progress::current_profile(),gameplay::player_state::difficulty(game_session::session.player_table));}
}
}
