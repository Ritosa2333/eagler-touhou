#include "loadout.hpp"
#include "pages.hpp"
#include "progress_queries.hpp"
#include "../stone_menu/update.hpp"
#include "../text_renderer/text.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../gameplay/stage_data.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include "../hud_system/dialogue.hpp"
namespace th20::source::title {
namespace {
struct Production final:LoadoutEnvironment {
    Production():LoadoutEnvironment(selection_environment()){}
    int clear_count(int difficulty,int character,int profile)override{return profile_clear_count(*progress::manager,difficulty,character,profile);}
    void open_stones()override{stone_menu::open(*stone_menu::controller,0);}
    int stone_display_state()override{const auto& o=*stone_menu::controller;if(o.category.current==1){if(o.state==4)return 2;if(o.state==3)return 1;}return 0;}
    int stone_visibility()override{return stone_menu::controller->visible;}
    bool effects_ready()override{return effects::controller(0)->ready!=0;}
    void loading_transition()override{
        text::renderer->create_loading_text(480.f,392.f); //readonly56cda8/570388
        auto& handle=program_entry::graphics_state.unknown_01c4;handle=effects::controller(0)->spawn(0,nullptr,nullptr,true);
        sprite::interrupt_animation_children(*program_entry::sprite_controller,handle,7);hud::fade_stage_track(0.05f); //56fe5c
    }
    void request_start(int stage)override{gameplay::select_stage(selection.main.session.player_table,stage);gameplay::replay_selection=-1;program_entry::graphics_state.field_0b0c=7;}
};
}
LoadoutEnvironment& loadout_environment(){static Production value;return value;}
namespace unrecovered {void update_loadout_0052b8e0(TitleInf& o){update_loadout(o,loadout_environment());}}
}
