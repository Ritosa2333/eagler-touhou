#include "dialogue.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../bullet_system/bullet.hpp"
#include "../laser_system/type0.hpp"
#include "../laser_system/type1.hpp"
#include "../laser_system/type2.hpp"
#include "../laser_system/type3.hpp"
#include <cstring>
#include <new>
#include <stdexcept>
namespace th20::source::hud {
namespace {
int __cdecl follow_callback(sprite::Animation* a){follow_dialogue_box(*controller->collecting,*a);return 0;} //4ac290/4afee0
void cancel_bullets(){auto& c=*bullet::controller();for(scheduler::Iterator it(c.active.sentinel.next);it.current;it.advance()){auto& b=*reinterpret_cast<bullet::Bullet*>(it.current->value);if(b.state!=0&&b.state!=3)bullet::cancel(b,0);}}
void cancel_lasers(){auto& c=*laser::controller();for(scheduler::Iterator it(c.active.sentinel.next);it.current;it.advance()){
    auto& beam=*reinterpret_cast<laser::Laser*>(it.current->value);if(beam.state==1)continue;
    if(auto* value=dynamic_cast<laser::Type0Laser*>(&beam))value->erase(0,0);
    else if(auto* value=dynamic_cast<laser::Type1Laser*>(&beam))value->erase(0,0);
    else if(auto* value=dynamic_cast<laser::Type2Laser*>(&beam))value->erase(0,0);
    else if(auto* value=dynamic_cast<laser::Type3Laser*>(&beam))value->request_delete(0,0);
    else throw std::logic_error("Unrecognized concrete Laser in dialogue clear");
}}
}
Dialogue::Dialogue(std::uint8_t* value):state(0),timers{},portraits{},portrait_overlays{},handles{},fields_74{},cursor{},field_c8(0),script(value),vectors_d0{},field_100(0),fields_108{},vector_128{},field_134(0),fields_138{}{
    flags&=~0x7fu;for(auto& timer:timers)recovered::timer_set(timer,0);
    auto& c=environment::sprites();auto& file=*program_entry::graphics_state.surface_animation;
    for(unsigned pair=0;pair<2;++pair){const unsigned i=1+pair*2;handles[i]=sprite::spawn_named_animation(c,file,"text",20+pair);handles[i+1]=sprite::spawn_named_animation(c,file,"text",20+pair);sprite::execute_animation_interrupt(c,handles[i+1],0);
        for(unsigned j=i;j<=i+1;++j){auto& a=*sprite::resolve_animation_handle(c,handles[j]);a.field_578=a.field_579=21;}
    }
    for(unsigned i=1;i<=4;++i){auto& a=*sprite::resolve_animation_handle(c,handles[i]);a.base.flags[1]&=~0x400u;}
    for(unsigned i=1;i<=4;++i)sprite::resolve_animation_handle(c,handles[i])->field_5dc=reinterpret_cast<std::uint32_t>(&follow_callback);
    for(auto& vector:vectors_d0)vector={16,0,0};cancel_bullets();cancel_lasers();clear_dialogue_enemies();vector_128={384,640,0};field_134=320;flags&=~0x40u;fields_108[2]=0;
}
Dialogue* create_dialogue(std::uint8_t* script){auto* memory=::operator new(sizeof(Dialogue),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(Dialogue));try{return new(memory)Dialogue(script);}catch(...){::operator delete(memory);throw;}}
}
