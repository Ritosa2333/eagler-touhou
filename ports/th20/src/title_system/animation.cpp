#include "title.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
namespace th20::source::title {
void spawn(TitleInf& o,int index){o.handles[index]=sprite::spawn_named_animation(*program_entry::sprite_controller,*o.files[0],"title",index);}
void activate(TitleInf& o,int index){sprite::execute_animation_interrupt(*program_entry::sprite_controller,o.handles[index],2);}
void deactivate(TitleInf& o,int index){sprite::execute_animation_interrupt(*program_entry::sprite_controller,o.handles[index],3);}
void retire_animation(TitleInf& o,int index){sprite::interrupt_animation_children(*program_entry::sprite_controller,o.handles[index],1);o.handles[index]=0;}
bool animation_exists(TitleInf& o,int index){return sprite::find_animation(*program_entry::sprite_controller,o.handles[index])!=nullptr;}
}
