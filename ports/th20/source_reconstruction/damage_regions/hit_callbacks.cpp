#include "hit_callbacks.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include <cstring>
#include <emmintrin.h>
namespace th20::source::damage {
namespace {
template<class T>T read(const void* object,std::size_t offset){T value;std::memcpy(&value,static_cast<const std::uint8_t*>(object)+offset,sizeof(value));return value;}
template<class T>void write(void* object,std::size_t offset,T value){std::memcpy(static_cast<std::uint8_t*>(object)+offset,&value,sizeof(value));}
}
void* find_player_shot(Region& region){
    if(!region.field_90)return nullptr;
    auto& list=*reinterpret_cast<scheduler::List*>(static_cast<std::uint8_t*>(region.context->objects_04[0])+0x22b4+0x12430);
    for(scheduler::Iterator iterator(list.sentinel.next);iterator.current;iterator.advance())if(read<int>(iterator.current->value,0x14)==region.field_90)return iterator.current->value;
    return nullptr;
}
int default_shot_hit(void* shot,sprite::Controller& sprites){
    if(!read<std::uint8_t>(shot,0x114)){
        auto handle=read<unsigned>(shot,0x18);auto* animation=sprite::resolve_animation_handle(sprites,handle);write(shot,0x18,handle);
        write(shot,0x58,.1f);sprite::set_animation_interrupt(*animation,1);write(shot,0x9c,2);
        write(shot,0x68,_mm_cvtss_f32(_mm_div_ss(_mm_set_ss(read<float>(shot,0x68)),_mm_set_ss(8))));
        animation->vector_5bc=read<sprite::Vec3>(shot,0x50);
        auto damage_handle=read<unsigned>(shot,0xdc);retire_handle(damage_handle);write(shot,0xdc,0u);
    }
    return read<int>(shot,0xac);
}
int shot_hit_callback(Region& region,const sprite::Vec3& position,const sprite::Vec2* size,float angle,float radius,sprite::Controller& sprites){
    auto* shot=find_player_shot(region);
#ifdef TH_SDL3
    // Stored callbacks are __fastcall (Shot*,void* dummy,...) on Windows;
    // wasm has no register conventions, so the dummy is a real argument here.
    using Callback=int(*)(void*,void*,const sprite::Vec3*,const sprite::Vec2*,float,float);
    const auto callback=read<Callback>(shot,0x4c);return callback?callback(shot,nullptr,&position,size,angle,radius):default_shot_hit(shot,sprites);
#else
    using Callback=int(__thiscall*)(void*,const sprite::Vec3*,const sprite::Vec2*,float,float);
    const auto callback=read<Callback>(shot,0x4c);return callback?callback(shot,&position,size,angle,radius):default_shot_hit(shot,sprites);
#endif
}
int diminish_shot_hit(Region& region,const sprite::Vec3& position){
    auto* shot=find_player_shot(region);
    // Original computes and discards the midpoint before changing the damage.
    volatile float midpoint[3];for(unsigned i=0;i<3;++i)midpoint[i]=_mm_cvtss_f32(_mm_div_ss(_mm_add_ss(_mm_set_ss((&region.motion.position.x)[i]),_mm_set_ss((&position.x)[i])),_mm_set_ss(2.f)));midpoint[2]=0;
    const auto damage=read<int>(shot,0xac);write(shot,0xac,1);return damage;
}
}
