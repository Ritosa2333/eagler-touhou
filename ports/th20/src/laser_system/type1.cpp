#include "type1.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../bullet_system/style.hpp"
#include "../runtime_core/runtime_core.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../ecl_vm/math.hpp"
#include <new>
#include <cstring>
namespace th20::source::laser {
Type1Laser::Type1Laser():field_76c(0){sprite::construct_animation(animation);sprite::construct_animation(origin_animation);}
Type1Laser::~Type1Laser(){sprite::destroy_animation_contents(origin_animation);sprite::destroy_animation_contents(animation);}
Type1Laser* create_type1(){void* p=::operator new(sizeof(Type1Laser),std::nothrow);if(!p)return nullptr;std::memset(p,0,sizeof(Type1Laser));return new(p)Type1Laser;}
std::uint32_t spawn_type1(Controller& owner,const Type1Parameters& parameters){
    if(recovered::signed_bits(owner.count)>511)return 0;++owner.next_handle;if(recovered::signed_bits(owner.next_handle)<0x10000)owner.next_handle=0x10000;
    auto* l=create_type1();if(!l)return 0;l->handle=owner.next_handle;if(l->initialize(parameters)<0)return 0;owner.attach(*l);return owner.next_handle;
}
void Type1Laser::set_position(const sprite::Vec3& p) noexcept {position=parameters.position=p;}
std::int32_t __cdecl remap_sprite(sprite::Animation* a,std::int32_t id){
    const auto& l=*reinterpret_cast<const Laser*>(a->field_5c8);const auto& style=bullet::styles[l.field_6e4];
    if(recovered::signed_bits(style.colors[0].words[0])>=0)return recovered::signed_bits(style.colors[l.field_6e8].words[id]);return id;
}
int Type1Laser::initialize(const Type1Parameters& input){
    parameters=input;state=3;field_24=1;field_6e4=parameters.type;field_6e8=parameters.color;select_context(parameters.view_index);field_6d0=0xffd08080;
    const auto& style=bullet::styles[field_6e4];if(style.cancel_type==6)field_6d0=style.colors[field_6e8].words[4];
    sprite::reset_animation_state(animation);animation.field_5e0=reinterpret_cast<std::uintptr_t>(&remap_sprite);animation.field_5c8=reinterpret_cast<std::uintptr_t>(this);
    auto& file=*static_cast<Controller*>(context->objects_04[4])->file;
    bullet::restart_animation(file,animation,recovered::signed_bits(style.script));sprite::set_animation_interrupt(animation,2);sprite::execute_animation(animation);
    animation.base.flags[0]=(animation.base.flags[0]&~0xffffu)|0x101u;animation.base.flags[4]=0;animation.base.flags[5]=2;animation.base.flags[2]=(animation.base.flags[2]&~0x3000000u)|0x1000000u;
    sprite::bind_animation_script(file,origin_animation,recovered::signed_bits(parameters.color+0x3au),nullptr);sprite::set_animation_interrupt(origin_animation,2);sprite::execute_animation(origin_animation);
    origin_animation.base.flags[0]=(origin_animation.base.flags[0]&~0xffffu)|0x101u;origin_animation.base.flags[2]=(origin_animation.base.flags[2]&~0x3000000u)|0x1000000u;
    if(parameters.sound>=0)program_entry::thread_registry.request_effect_at(parameters.sound,0);
    position=parameters.position;if(parameters.radial_offset!=0){sprite::Vec3 offset{};ecl::math::polar(offset.x,offset.y,parameters.angle,parameters.radial_offset);position.x=recovered::add32(position.x,offset.x);position.y=recovered::add32(position.y,offset.y);}
    field_74=parameters.length;speed=2;field_7c=parameters.growth_speed;angle=parameters.angle;command_index=parameters.command_index;handle=parameters.handle;recovered::timer_set(timer_38,0);field_76c=0;
    // Match the shared base destructor and Type2's raw segment-array ownership.
    void* memory=runtime::allocate_bytes(sizeof(Segment));allocated_6d8=memory?new(memory)Segment:nullptr;field_6dc=1;if(field_6e4==38)flags|=0x80;return 0;
}
}
