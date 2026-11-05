#include "title.hpp"
#include "progress_queries.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../startup_scene/startup.hpp"
#include "../input/input.hpp"
#include "../audio_runtime/audio.hpp"
#include "../notice_system/notice.hpp"
namespace th20::source::title {
int last_difficulty=1;
int last_character=0;
namespace {
struct Production final:MainEnvironment {
    Production():MainEnvironment(game_session::session,title::last_difficulty,title::last_character){}
    bool pressed(std::uint32_t mask)override{auto* b=input::button_slot(0);return b&&(b->pressed&mask)!=0;}
    bool repeated(std::uint32_t mask)override{auto* b=input::button_slot(0);return b&&((b->pressed|b->repeat8)&mask)!=0;}
    bool extra_unlocked()override{return any_extra_unlocked(*progress::manager);}
    bool notice_present()override{return startup::unrecovered::owner_005c5b38!=nullptr;}
    bool notice_finished()override{return notice::controller()->finished!=0;}
    bool notice_pending()override{return title::notice_pending(*progress::manager);}
    void create_notice()override{notice::create(pop_notice(*progress::manager));}
    void retire_notice()override{runtime::retire_callback_owner(startup::unrecovered::owner_005c5b38);}
    void sound(int id)override{program_entry::thread_registry.request_effect(id,0);}
    void spawn(TitleInf& o,int index)override{title::spawn(o,index);}
    void interrupt(TitleInf& o,int index,int event,bool clear)override{if(clear){sprite::interrupt_animation_children(*program_entry::sprite_controller,o.handles[index],event);o.handles[index]=0;}else sprite::execute_animation_interrupt(*program_entry::sprite_controller,o.handles[index],event);}
    bool exists(std::uint32_t handle)override{return sprite::find_animation(*program_entry::sprite_controller,handle)!=nullptr;}
    bool decoration_exists(TitleInf& o)override{return sprite::resolve_animation_handle(*program_entry::sprite_controller,o.handle474)!=nullptr;}
    void interrupt_handle(std::uint32_t handle,int event)override{sprite::interrupt_animation_children(*program_entry::sprite_controller,handle,event);}
    void spawn_decoration(TitleInf& o)override{o.handle474=sprite::spawn_named_animation(*program_entry::sprite_controller,*o.files[1],nullptr,0);}
};
}
MainEnvironment& main_environment(){static Production value;return value;}
}
