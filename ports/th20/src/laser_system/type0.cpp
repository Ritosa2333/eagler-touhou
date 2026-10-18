#include "type0.hpp"
#include "type1.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../bullet_system/style.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../runtime_core/runtime_core.hpp"
#include "../ecl_vm/math.hpp"
#include <new>
#include <cstring>
namespace th20::source::laser {
Type0Laser::Type0Laser(){sprite::construct_animation(animation);sprite::construct_animation(origin_animation);sprite::construct_animation(tip_animation);}
Type0Laser::~Type0Laser(){sprite::destroy_animation_contents(tip_animation);sprite::destroy_animation_contents(origin_animation);sprite::destroy_animation_contents(animation);}
Type0Laser* create_type0(){void* p=::operator new(sizeof(Type0Laser),std::nothrow);if(!p)return nullptr;std::memset(p,0,sizeof(Type0Laser));return new(p)Type0Laser;}
void Type0Laser::tip_position(sprite::Vec3& p) const {ecl::math::polar(p.x,p.y,angle,field_74);p.z=0;p.x=recovered::add32(p.x,position.x);p.y=recovered::add32(p.y,position.y);}
int Type0Laser::initialize(const Type0Parameters& input){
    parameters=input;state=2;field_24=0;field_6e4=parameters.type;field_6e8=parameters.color;select_context(parameters.view_index);field_6d0=0xffd08080;
    const auto& style=bullet::styles[field_6e4];if(style.cancel_type==6)field_6d0=style.colors[field_6e8].words[4];
    sprite::reset_animation_state(animation);animation.field_5e0=reinterpret_cast<std::uintptr_t>(&remap_sprite);animation.field_5c8=reinterpret_cast<std::uintptr_t>(this);auto& file=*static_cast<Controller*>(context->objects_04[4])->file;
    bullet::restart_animation(file,animation,recovered::signed_bits(style.script));sprite::set_animation_interrupt(animation,2);sprite::execute_animation(animation);
    animation.base.flags[0]=(animation.base.flags[0]&~0xffffu)|0x101u;animation.base.flags[4]=0;animation.base.flags[5]=2;animation.base.flags[2]=(animation.base.flags[2]&~0x3000000u)|0x1000000u;
    sprite::bind_animation_script(file,origin_animation,recovered::signed_bits(parameters.color+0x3a),nullptr);sprite::set_animation_interrupt(origin_animation,2);sprite::execute_animation(origin_animation);
    origin_animation.base.flags[0]=(origin_animation.base.flags[0]&~0xffffu)|0x101u;origin_animation.base.flags[2]=(origin_animation.base.flags[2]&~0x3000000u)|0x1000000u;
    if(recovered::signed_bits(field_6e4)<18||field_6e4==38){sprite::bind_animation_script(file,tip_animation,recovered::signed_bits(parameters.color+0x5d),nullptr);tip_animation.base.flags[0]=(tip_animation.base.flags[0]&~0xff00u)|0x100u;}
    else sprite::bind_animation_script(file,tip_animation,recovered::signed_bits(parameters.color+0x55),nullptr);
    tip_animation.base.flags[2]=(tip_animation.base.flags[2]&~0x3000000u)|0x1000000u;recovered::timer_set(timer_6ac,30);recovered::timer_set(timer_6bc,3);
    if(parameters.field_48>=0)program_entry::thread_registry.request_effect_at(parameters.field_48,0);recovered::timer_set(timer_38,0);recovered::timer_set(timer_48,0);
    position=parameters.position;if(parameters.radial_offset!=0){sprite::Vec3 offset{};ecl::math::polar(offset.x,offset.y,parameters.angle,parameters.radial_offset);position.x=recovered::add32(position.x,offset.x);position.y=recovered::add32(position.y,offset.y);}
    field_74=parameters.field_14;speed=parameters.width;field_7c=parameters.speed;angle=parameters.angle;field_80=field_74>parameters.length?0.009999999776482582f:0.0f;
    ecl::math::polar(velocity.x,velocity.y,angle,field_7c);command_index=parameters.command_index;field_88=field_84=0x3f800000u;
    // Base Laser owns this raw segment buffer and releases it via free.
    void* memory=runtime::allocate_bytes(sizeof(Segment));allocated_6d8=memory?new(memory)Segment:nullptr;field_6dc=1;return 0;
}
std::uint32_t spawn_type0(Controller& owner,const Type0Parameters& parameters){
    if(recovered::signed_bits(owner.count)>511)return 0;++owner.next_handle;if(recovered::signed_bits(owner.next_handle)<0x10000)owner.next_handle=0x10000;
    auto* l=create_type0();if(!l)return 0;l->handle=owner.next_handle;if(l->initialize(parameters)<0)return 0;owner.attach(*l);return owner.next_handle;
}
}
