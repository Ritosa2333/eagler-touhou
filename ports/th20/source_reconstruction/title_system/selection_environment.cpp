#include "selection.hpp"
#include "pages.hpp"
#include "progress_queries.hpp"
#include "../platform_window/graphics_callbacks.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../overlay_system/overlay.hpp"
#include "../effect_system/effect.hpp"
namespace th20::source::title {
namespace pe=program_entry;
namespace {
std::uint32_t child_handle(TitleInf& o,int index,int child){auto* a=sprite::find_animation_child(*pe::sprite_controller,o.handles[index],child,0);return a?a->handle:0;}
struct Production final:SelectionEnvironment {
    Production():SelectionEnvironment(main_environment(),platform_window::unrecovered::menu_selection,title::last_profile){}
    void spawn_text_overlay(TitleInf& o)override{o.handle390=sprite::spawn_named_animation(*pe::sprite_controller,*text::renderer->animation_file,nullptr,19);}
    void signal(TitleInf& o,int index,int event,bool execute)override{if(execute)sprite::execute_animation_interrupt(*pe::sprite_controller,o.handles[index],event);else sprite::interrupt_animation_children(*pe::sprite_controller,o.handles[index],event);}
    void child_visible(TitleInf& o,int index,int child,bool show)override{auto handle=child_handle(o,index,child);auto* a=sprite::find_animation(*pe::sprite_controller,handle);if(a){if(show)effects::enable_animation_tree(*a);else sprite::hide_animation_tree(*a);}}
    void child_signal(TitleInf& o,int index,int child,int event,bool execute)override{const auto handle=child_handle(o,index,child);if(execute)sprite::execute_animation_interrupt(*pe::sprite_controller,handle,event);else sprite::interrupt_animation_children(*pe::sprite_controller,handle,event);}
    bool all_cleared(int difficulty,int character)override{return character<0?difficulty_all_cleared(*progress::manager,difficulty):character_all_cleared(*progress::manager,difficulty,character);}
    bool character_extra_unlocked(int character)override{return title::character_extra_unlocked(*progress::manager,character);}
    int selected_profile(int slot,int character)override{return progress::manager->selected_profile(slot,character);}
    void commit_progress()override{progress::manager->commit();}
    void set_weapon(int slot,int character,int profile)override{auto& o=*overlay::controller(0);auto& e=overlay::environment();switch(slot){case 0:overlay::select_main(o,character,profile,e);break;case 1:overlay::select_unfocused(o,character,profile,e);break;case 2:overlay::select_focused(o,character,profile,e);break;case 3:overlay::select_passive(o,character,profile,e);break;}}
};
}
SelectionEnvironment& selection_environment(){static Production value;return value;}
namespace unrecovered {void update_difficulty_00521bc0(TitleInf& o){update_difficulty(o,selection_environment());}void update_character_00521060(TitleInf& o){update_character(o,selection_environment());}}
}
