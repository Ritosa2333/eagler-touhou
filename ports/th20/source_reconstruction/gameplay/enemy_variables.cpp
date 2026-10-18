#include "enemy_variables.hpp"
#include "enemy_state.hpp"
#ifdef TH_SDL3
#include "enemy_entity.hpp"
#endif
#include <cstring>
#include <stdexcept>
#include <immintrin.h>
namespace th20::source::gameplay {
std::uint32_t enemy_script_globals[4]={};
namespace {
template<class T>T read(const void* p,std::size_t offset) noexcept {T value;std::memcpy(&value,static_cast<const std::uint8_t*>(p)+offset,sizeof(value));return value;}
std::uint32_t* address(void* p,std::size_t offset) noexcept {return reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(p)+offset);}
void* controller_for(void* entity) {
#ifdef TH_SDL3
    // Enemy embeds std::function; libc++ lays it out differently from the
    // original MSVC x86 object. Use the actual Context member in the web build.
    auto* context=static_cast<Enemy*>(entity)->context;
    return context?context->objects_04[1]:nullptr;
#else
    return read<void*>(read<void*>(entity,0x424),8);
#endif
}
void* selected_or_self(void* entity) {
    if(!selected_enemy(controller_for(entity),0))return entity;
    return selected_enemy(controller_for(entity),0);          //original performs the lookup again
}
}
void* find_enemy_in_list(scheduler::List& list,std::uint32_t identifier) {
    if(!identifier)return nullptr;
    for(scheduler::Iterator iterator(list.sentinel.next);iterator.current;iterator.advance())
        if(read<std::uint32_t>(iterator.current->value,0x88)==identifier)return iterator.current->value;
    return nullptr;
}
void* selected_enemy(void* owner,unsigned slot) {
    if(slot>=16)throw std::out_of_range("Original Enemy handle array index");
    const auto handle=read<std::uint32_t>(owner,0x54+slot*4);
    auto* first_player=game_session::context(0).objects_04[1];
    if(!first_player)return nullptr;
    return find_enemy_in_list(*reinterpret_cast<scheduler::List*>(static_cast<std::uint8_t*>(first_player)+0x108),handle);
}
sprite::Vec3 enemy_position(const void* entity) noexcept {return read<sprite::Vec3>(entity,0x198);}
std::uint32_t enemy_identifier(const void* entity) noexcept {return read<std::uint32_t>(entity,0x88);}
bool enemy_state_excluded(const EnemyState& state) noexcept {
    return (state.fields_2c8[0]&0x21u)!=0 || (state.fields_2c8[1]&0xc00u)!=0;
}
bool enemy_excluded(const void* entity) noexcept {
    return (read<std::uint32_t>(entity,0x350)&0x21u)!=0 || (read<std::uint32_t>(entity,0x354)&0xc00u)!=0;
}
void set_enemy_bomb_mark(void* entity,std::uint32_t value) noexcept {
    *address(entity,0x358)=(read<std::uint32_t>(entity,0x358)&~0x20u)|((value&1u)<<5);
}
std::uint32_t nearest_enemy_identifier(scheduler::List& list,const sprite::Vec2& origin,float radius) {
    float nearest=recovered::mul32(radius,radius);
    void* selected=nullptr;
    {
        scheduler::Iterator iterator(list.sentinel.next); //412280; destroyed before reading result
        while(iterator.current) {
            auto* entity=iterator.current->value;
            if(!enemy_excluded(entity)) {
                const auto position=enemy_position(entity);
                const auto x=_mm_sub_ss(_mm_set_ss(origin.x),_mm_set_ss(position.x));
                const auto y=_mm_sub_ss(_mm_set_ss(origin.y),_mm_set_ss(position.y));
                const float distance=_mm_cvtss_f32(_mm_add_ss(_mm_mul_ss(x,x),_mm_mul_ss(y,y))); //4560d0
                if(distance<nearest) {nearest=distance;selected=entity;}
            }
            iterator.advance();
        }
    }
    return selected?enemy_identifier(selected):0;
}
std::uint32_t* enemy_integer_destination(void* entity,std::int32_t variable) {
    const auto code=static_cast<std::uint32_t>(variable);
    if(code>=0xffffd8ffu&&code<=0xffffd902u)return address(entity,0x100+(code-0xffffd8ffu)*4);
    if(code>=0xffffd923u&&code<=0xffffd925u)return address(controller_for(entity),0x48+(code-0xffffd923u)*4);
    if(code>=0xffffd929u&&code<=0xffffd92cu)return address(selected_or_self(entity),0x100+(code-0xffffd929u)*4);
    if(code>=0xffffd93au&&code<=0xffffd93du)return address(controller_for(entity),0x10+(code-0xffffd93au)*4);
    if(code>=0xffffd959u&&code<=0xffffd95cu)return &enemy_script_globals[code-0xffffd959u];
    return nullptr;
}
std::uint32_t* enemy_float_destination(void* entity,std::int32_t variable) {
    const auto code=static_cast<std::uint32_t>(variable);
    if(code>=0xffffd8f5u&&code<=0xffffd8f8u) {
        const auto index=code-0xffffd8f5u;
        return address(read<void*>(entity,0x1e4),(index/2)*0x184+(index%2)*4); //48bd10
    }
    if(code>=0xffffd903u&&code<=0xffffd906u)return address(entity,0x110+(code-0xffffd903u)*4);
    if(code>=0xffffd92du&&code<=0xffffd930u)return address(selected_or_self(entity),0x110+(code-0xffffd92du)*4);
    if(code>=0xffffd931u&&code<=0xffffd934u)return address(entity,0x120+(code-0xffffd931u)*4);
    if(code>=0xffffd93eu&&code<=0xffffd945u)return address(controller_for(entity),0x20+(code-0xffffd93eu)*4);
    return nullptr;
}
}
