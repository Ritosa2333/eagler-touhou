#include "stones.hpp"
#include "stones_data.hpp"
#include "pages.hpp"
#include "player_data.hpp"
#include "stage_select.hpp"
#include "../trophy_system/trophy.hpp"
#include "../ending_scene/ending.hpp"
#include "../effect_system/effect.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_window/platform_window.hpp"
#include "../platform_window/graphics_callbacks.hpp"
#include "../gameplay/stage_data.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include "../hud_system/dialogue.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
namespace th20::source::title {
namespace {struct Host final:StonesEnvironment {
    Host():StonesEnvironment(main_environment(),last_stone_record,stones_unlock_state){}
    const char* title(int index)override{if(index<0||index>=128)throw std::out_of_range("Trophy title index");auto& message=trophy::messages[index];return message.id==index?trophy::decode_string(message.title):nullptr;}
    const char* description(int index,bool achieved,int line)override{if(index<0||index>=128||line<0||line>=3)throw std::out_of_range("Trophy description index");auto& message=trophy::messages[index];return message.id==index?trophy::decode_string(message.description[achieved][line]):nullptr;}
    bool achieved(unsigned id)override{return trophy::achieved(id);}
    void background(TitleInf& o)override{o.handle390=sprite::spawn_named_animation(*program_entry::sprite_controller,*text::renderer->animation_file,nullptr,19);}
    void heading(TitleInf& o,bool visible)override{if(visible)spawn(o,47);else retire_animation(o,47);}
    void fade(float duration)override{hud::fade_stage_track(duration);}
    void ending(int id)override{ending::selected_gallery_ending=id;main.sound(7);fade(stones_data::f_0056e0ec);ending::create();}
    bool effects_ready()override{return effects::controller(0)->ready!=0;}
    void loading()override{text::renderer->create_loading_text(stones_data::f_0056cda8,stones_data::f_00570388);program_entry::graphics_state.unknown_01c4=effects::controller(0)->spawn(0,nullptr,nullptr,true);sprite::interrupt_animation_children(*program_entry::sprite_controller,program_entry::graphics_state.unknown_01c4,7);}
    void prepare_replay(TitleInf& o)override{
        auto& s=main.session;auto& p=*s.contexts[0].current_player;platform_window::unrecovered::menu_selection=6;data_character=int(p.fields_00[2]);data_profile=int(p.fields_00[3]);
        const int selection=o.cursor.current-18;set_character(s,selection/8);p.fields_00[3]=unsigned(selection%8);progress::manager->select_profile(0,int(p.fields_00[2]),int(p.fields_00[3]));s.player_table.fields_1ec[(0x204-0x1ec)/4]=0xffffffffu;
        gameplay::replay_selection=1;gameplay::select_stage(s.player_table,7);set_state(o,2);last_record=o.cursor.current;program_entry::graphics_state.field_0b0c=7;
    }
    int keyboard(std::span<std::uint8_t,256> keys)override{return read_shortcut_keyboard(keys);}
    void unlock_progress()override{unlock_all_data(*progress::manager,state::random_streams[1]);}
};}
StonesEnvironment& stones_environment(){static Host host;return host;}
namespace unrecovered {
void update_stones_0052a8a0(TitleInf& o){update_stones(o,stones_environment());}
void draw_stones_0052b480(TitleInf& o){draw_stones(o,*text::renderer,stones_environment());}
}
}
