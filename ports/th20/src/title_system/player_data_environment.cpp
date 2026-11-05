#include "player_data.hpp"
#include "stage_select.hpp"
#include "pages.hpp"
#include "../stone_menu/update.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../input/input.hpp"
namespace th20::source::title {
namespace {
class SourceUnlock final:public UnlockEnvironment {
    int read_keyboard(std::span<std::uint8_t,256> keys) override{return read_shortcut_keyboard(keys);}
    void unlock_progress() override{unlock_all_data(*progress::manager,state::random_streams[1]);}
    void sound(int id) override{program_entry::thread_registry.request_effect(id,0);}
} unlock_environment;
class SourceData final:public DataEnvironment {
public:SourceData():DataEnvironment(selection_environment()){}
private:
    void open_stones() override{stone_menu::open(*stone_menu::controller,2);}
    void select_stone(int index) override{sprite::execute_animation_interrupt(*program_entry::sprite_controller,stone_menu::controller->animation_handles[8],index+7);}
    void hide_stones() override{stone_menu::hide(*stone_menu::controller);}
    void update_unlock_sequence() override{if(selection.main.pressed(0x8010f))data_unlock_state.matched=data_unlock_state.idle=0;advance_data_unlock(data_unlock_state,unlock_environment);}
} environment;
}
UnlockEnvironment& data_unlock_environment(){return unlock_environment;}
DataEnvironment& data_environment(){return environment;}
namespace unrecovered {
void update_player_data_005265a0(TitleInf& o){update_player_data_menu(o,selection_environment());}
void update_data_detail_00524c10(TitleInf& o){update_player_data_detail(o,data_environment());}
void draw_data_detail_005257f0(TitleInf& o){draw_player_data_detail(o,*text::renderer,*progress::find_profile(progress::manager->current,o.cursor.current/8,o.cursor.current%8),progress::fallback_profile(),stone_menu::controller->names[(o.cursor.current%8)*4]);}
}
}
