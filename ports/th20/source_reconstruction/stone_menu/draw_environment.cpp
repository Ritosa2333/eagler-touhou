#include "draw.hpp"
#include "../text_renderer/text.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../program_entry/program_entry.hpp"
#include "../progress_state/manager.hpp"
namespace th20::source::stone_menu {
namespace pe=program_entry;
namespace {
struct GameDrawEnvironment final:DrawEnvironment {
    int character() override{return static_cast<int>(game_session::context(0).current_player->fields_00[2]);}
    int difficulty() override{return static_cast<int>(game_session::session.player_table.field_1e0);}
    int selected_profile(int slot) override{return progress::manager->selected_profile(slot,character());}
    unsigned stone_count(unsigned index) override{return progress::manager->stone_count(index);}
    unsigned used_stone_count(unsigned index) override{return progress::manager->used_stone_count(index);}
    bool extra_unlocked(unsigned index) override{return progress::manager->extra_unlocked(character(),index);}
    float screen_scale() override{return pe::window_state.scale;}
    sprite::Vec3 animation_position(std::uint32_t& handle) override{auto& sprites=*pe::sprite_controller;auto* animation=sprite::resolve_animation_handle(sprites,handle);return sprite::animation_position(animation?*animation:sprites.animation_dc);} //44cf10 fallback
    void set_animation_position(std::uint32_t handle,const sprite::Vec3& position) override{if(auto* animation=sprite::find_animation(*pe::sprite_controller,handle))animation->vector_5bc=position;}
    void set_animation_scale(std::uint32_t handle,float x,float y) override{if(auto* animation=sprite::find_animation(*pe::sprite_controller,handle))animation->base.vector_50={x,y};}
    void text_style(unsigned x,unsigned y) override{text::renderer->fields_1a1d4[8]=x;text::renderer->fields_1a1d4[9]=y;}
    void text_field(TextField field,unsigned value) override{switch(field){case TextField::color:text::renderer->color=value;break;case TextField::shadow:text::renderer->shadow_color=value;break;case TextField::blend:text::renderer->field_1a1c8=value;break;case TextField::font:text::renderer->fields_1a1d4[4]=value;break;}}
    void clear_ascii_lines() override{text::renderer->line_count=0;}
    void write_text(const sprite::Vec3& position,const char* line) override{text::renderer->write_text_literal(position,line);}
};
}
DrawEnvironment& draw_environment(){static GameDrawEnvironment value;return value;}
}
