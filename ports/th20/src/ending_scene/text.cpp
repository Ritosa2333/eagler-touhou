#include "ending.hpp"
#include "../program_entry/program_entry.hpp"
#include "../hud_system/dialogue.hpp"
#include "../text_renderer/text.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/pool.hpp"
#include <cstdlib>
#include <stdexcept>
#include <string>
namespace th20::source::ending {
void queue_line(Script& o){
    auto& sprites=*program_entry::sprite_controller;std::string value(hud::decode_dialogue_text(o.instruction+4));
    if(o.line==0)for(unsigned i=0;i<5;++i){sprite::execute_animation_interrupt(sprites,o.text_handles[i],3);sprite::execute_animation_interrupt(sprites,o.ruby_handles[i],3);}
    if(o.line>=5)throw std::out_of_range("Ending line index outside5 ANM handles");
    sprite::execute_animation_interrupt(sprites,o.text_handles[o.line],3);sprite::execute_animation_interrupt(sprites,o.ruby_handles[o.line],3);
    const bool ruby=!value.empty()&&value[0]=='|';int x=0,spacing=0;
    if(ruby){const auto first=value.find(',',1),last=first==std::string::npos?std::string::npos:value.find(',',first+1);if(last==std::string::npos)throw std::invalid_argument("Ending ruby text lacks two commas");x=std::atoi(value.c_str()+1);spacing=std::atoi(value.c_str()+first+1);value.erase(0,last+1);}
    auto* animation=sprite::resolve_animation_handle(sprites,(ruby?o.ruby_handles:o.text_handles)[o.line]);
    text::renderer->enqueue_task([&o,animation,ruby,x,spacing,value=std::move(value)]{
        //477450: interrupt execution on the captured actual ANM, without the
        //manager wrapper's pre-draw callback.
        text::write_animation_text(*program_entry::sprite_controller,*animation,ruby?o.background:o.foreground,ruby?0x20ffffff:o.background,ruby?18:4,x,spacing,nullptr,[animation]{animation->base.field_438=2;sprite::execute_animation(*animation);},value.c_str());
    });
    if(!ruby){++o.line;if(static_cast<int>(o.line)>4)o.line=0;}
}
}
