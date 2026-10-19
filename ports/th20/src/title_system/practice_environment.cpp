#include "practice.hpp"
#include "practice_data.hpp"
#include "pages.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../gameplay/stage_data.hpp"
#include <cstring>
#include <stdexcept>
namespace th20::source::title {
namespace {
class SourcePractice final:public PracticeEnvironment {
public:SourcePractice():PracticeEnvironment(selection_environment(),stage_environment(),practice_resume_stage,practice_resume_difficulty,practice_resume_boss){}
private:
    void delete_animation(TitleInf& o,int index) override{sprite::request_animation_deletion(sprite::find_animation(*program_entry::sprite_controller,o.handles[index]));}
    bool group_available(int stage,int boss) override{return practice_group_available(progress::fallback_profile(),stage,boss);}
    bool card_playable(int id) override{
        if(id<0||id>=113)throw std::out_of_range("Spell practice card");
        unsigned attempts[2];std::memcpy(attempts,progress::current_profile()->bytes+0xb08+id*0xe0+0xc8,sizeof(attempts));return attempts[0]!=0||attempts[1]!=0;
    }
    void refresh_cards(TitleInf& o,int stage,int boss,int selected) override{refresh_practice_cards(o,*text::renderer,progress::fallback_profile(),*progress::current_profile(),stage,boss,selected,*this);}
    void select_card(TitleInf& o,int selected) override{select_practice_card(o,selected,*this);}
    bool card_exists(TitleInf& o,int slot) override{return sprite::resolve_animation_handle(*program_entry::sprite_controller,o.handles394[slot])!=nullptr;}
    void card_interrupt(TitleInf& o,int slot,int event) override{sprite::interrupt_animation_children(*program_entry::sprite_controller,o.handles394[slot],event);}
    void launch(int stage,int card) override{
        if(card<0||card>=113)throw std::out_of_range("Spell practice launch card");
        auto& table=game_session::session.player_table;gameplay::select_stage(table,stage);gameplay::player_state::write(table,0x204,card);gameplay::player_state::spell(table);table.field_1e0=practice_data::difficulties[card];program_entry::graphics_state.field_0b0c=7;
    }
} environment;
}
PracticeEnvironment& practice_environment(){return environment;}
namespace unrecovered {
void update_gallery_00528fc0(TitleInf& o){update_practice_stage(o,practice_environment());}
void update_gallery_page_005284f0(TitleInf& o){update_practice_boss(o,practice_environment());}
void update_gallery_choice_00527f00(TitleInf& o){update_practice_difficulty(o,practice_environment());}
void draw_gallery_00528a50(TitleInf& o){draw_practice_scores(o,*text::renderer,progress::fallback_profile(),*progress::current_profile());}
}
}
