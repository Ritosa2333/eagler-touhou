#include "bullet.hpp"
#include "../damage_regions/geometry.hpp"
#include "../runtime_state/state.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../effect_system/effect.hpp"
#include "../audio_runtime/audio.hpp"
namespace th20::source::bullet {
namespace n=recovered;namespace pe=program_entry;
namespace {float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}}
int cancel(Bullet& b,std::int32_t drop_mode){
    auto& sprites=*pe::sprite_controller;const auto view=b.view_index;sprites.field_6c4=static_cast<std::uint32_t>(view);
    b.animation->base.field_438=1;sprite::execute_animation(*b.animation);
    sprite::interrupt_animation_children(sprites,b.animation_handle,1);
    if(!(b.flags&0x200u)){
        if(b.cancel_script>=0){
            auto& owner=*static_cast<Controller*>(b.context->primary_owner);std::uint32_t handle;
            sprite::spawn_named_animation(sprites,*owner.file,handle,"bullet",b.cancel_script,&b.position,0,-1,0);owner.handles[b.index]=handle;
            const auto kind=(b.flags>>11)&3u;
            if(kind==1)sprite::interrupt_animation_children(sprites,handle,3);
            if(kind==2){effects::Parameters parameters;effects::construct_parameters(parameters);parameters.vector_00={b.position.x,b.position.y,0};parameters.value_20=b.handle;static_cast<effects::Controller*>(b.context->objects_04[7])->spawn(4,&parameters);}
        }
        pe::thread_registry.request_effect_at(71,b.position.x);drop_items(b,b.position,drop_mode);
    }
    b.position.x=n::add32(b.position.x,div(n::mul32(b.velocity.x,state::clock_scale),2));
    b.position.y=n::add32(b.position.y,div(n::mul32(b.velocity.y,state::clock_scale),2));
    b.position.z=n::add32(b.position.z,div(n::mul32(b.velocity.z,state::clock_scale),2));
    b.state=4;assign_timer_float(b.timer_4d8,1);assign_timer_float(b.timer_4e8,1);n::timer_set(b.timer_4f8,0);
    sprites.field_6c4=static_cast<std::uint32_t>(view);return 0;
}
std::int32_t cancel_rectangle(Controller& owner,const sprite::Vec3& center,const sprite::Vec3& size,float angle,std::int32_t drop_mode,std::uint32_t kind){
    std::uint32_t count=0;
    scheduler::Iterator iterator(owner.active.sentinel.next);
    for(;iterator.current;iterator.advance()){
        auto& b=*reinterpret_cast<Bullet*>(iterator.current->value);if((b.state!=1&&b.state!=2)||b.field_18)continue;
        const auto radius=n::mul32(b.size.x,b.scale);
        if(!geometry::rectangle_circle(center.x,center.y,size.x,size.y,angle,b.position.x,b.position.y,radius)||!geometry::rectangle_circle(0,224,384,448,0,b.position.x,b.position.y,radius))continue;
        ++count;++owner.cancel_counter;b.flags=(b.flags&~0x1800u)|((kind&3u)<<11);cancel(b,drop_mode);
    }
    return n::signed_bits(count);
}
std::int32_t cancel_circle(Controller& owner,const sprite::Vec3& center,float radius,std::int32_t drop_mode,std::int32_t limit,std::uint32_t kind){
    std::uint32_t count=0;
    scheduler::Iterator iterator(owner.active.sentinel.next);
    for(;iterator.current;iterator.advance()){
        auto& b=*reinterpret_cast<Bullet*>(iterator.current->value);if((b.state!=1&&b.state!=2)||b.field_18||!in_circle(b.position,center,n::add32(div(b.size.x,2),radius)))continue;
        b.flags=(b.flags&~0x1800u)|((kind&3u)<<11);cancel(b,drop_mode);limit=n::signed_bits(static_cast<std::uint32_t>(limit)-1u);++owner.cancel_counter;++count;if(limit<1)return n::signed_bits(count);
    }
    return n::signed_bits(count);
}
}
