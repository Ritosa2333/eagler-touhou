#include "enemy_frame.hpp"
#include <cstring>
#include <stdexcept>
#include <immintrin.h>
namespace th20::source::gameplay {
namespace {
template<class T>T read(const void* object,std::size_t offset) noexcept {
    T result;std::memcpy(&result,static_cast<const std::uint8_t*>(object)+offset,sizeof(result));return result;
}
template<class T>void write(void* object,std::size_t offset,T value) noexcept {
    std::memcpy(static_cast<std::uint8_t*>(object)+offset,&value,sizeof(value));
}
}
void store_boss_time(void* hud,std::int32_t seconds,std::int32_t hundredths) noexcept {
    // The second field tests seconds, not hundredths; negative values survive.
    write(hud,0x1c4,seconds<100?seconds:99);
    write(hud,0x1c8,seconds<100?hundredths:99);
}
float primary_entity_scale(const void* entity) noexcept {return read<float>(entity,0x223c);}
void set_primary_entity_scale(void* entity,float value) noexcept {write(entity,0x223c,value);}
void set_primary_entity_flag(void* entity,std::uint32_t value) noexcept {
    write(entity,0x14,(read<std::uint32_t>(entity,0x14)&~0x20u)|((value&1u)<<5));
}
int update_enemy_with_time_scale(void* entity,EnemyFrameServices& host) {
    const float slowdown=read<float>(entity,0xc4);
    // COMISS 0,slowdown / JB also selects this path for unordered operands.
    const bool slowed=!(slowdown<=0.0f);
    const auto update_animations=[&](bool clear) {
        auto* begin=read<EnemyAnimationLink*>(entity,0x98);
        const auto* end=read<EnemyAnimationLink*>(entity,0x9c);
        for(auto* link=begin;link!=end;++link) {
            if(auto* animation=host.animation(link->handle))
                write(animation,0x560,clear?0.0f:read<float>(entity,0xc4)); //45d130
            else link->handle=0;                            //44ced0 invalidates stale handles
        }
    };
    if(slowed) {
        const float previous=host.clock_scale();
        const float product=recovered::mul32(previous,slowdown);
        float next=_mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(previous),_mm_set_ss(product)));
        if(next>1.0f)next=1.0f;else if(next<0.0f)next=0.0f;
        host.clock_scale()=next;update_animations(false);
        const int result=host.update_entity_state(static_cast<std::uint8_t*>(entity)+0x88);
        host.clock_scale()=previous;
        write(entity,0x354,read<std::uint32_t>(entity,0x354)|0x10000u);
        return result;
    }
    if(read<std::uint32_t>(entity,0x354)&0x10000u)update_animations(true);
    // Original leaves bit16 set in the zero/negative slowdown branch.
    return host.update_entity_state(static_cast<std::uint8_t*>(entity)+0x88);
}
int update_enemy_controller(EnemyController& enemy,EnemyFrameServices& host) {
    enemy.services->sprites().field_6c4=static_cast<std::uint32_t>(enemy.player_index); //4776a0
    enemy.data.field_9c=enemy.data.field_a0=0; //absolute+ac/+b0
    if(recovered::signed_bits(enemy.field_cc)>0) {
        if(enemy.timer_d0.current>0) {
            recovered::timer_add(enemy.timer_d0,-1.0f,host.timer_rate()); //461070,429420,429e70,4297a0
            if(enemy.timer_d0.current<=0) {enemy.field_cc=0;recovered::timer_set(enemy.timer_d0,0);}
        }
        const int remainder=enemy.timer_d0.current%60,seconds=enemy.timer_d0.current/60;
        host.update_boss_time(seconds,(remainder*100)/60); //472040,4a3f00,4ab5b0
    }
    {
        //412280 constructs directly into the live iterator (no44b9f0 copy).
        scheduler::Iterator iterator(enemy.enemies.sentinel.next);
        while(iterator.current) {
            auto* entity=iterator.current->value;
            if((read<std::uint32_t>(entity,0x354)&(1u<<9)) || update_enemy_with_time_scale(entity,host)!=0){
                // Nested native-style queries can replace the link's single
                // observer. Do not let advance() touch this object after free.
                // The cached successor still receives normal unlink repairs.
                iterator.current=nullptr;
                enemy.services->retire_entity(entity); //4a2720, actual Enemy/ECL-manager destructor
            }
            else write(entity,0x354,read<std::uint32_t>(entity,0x354)&~4u);
            iterator.advance();
        }
    }
    host.update_special_objects();
    if(!enemy.context->objects_04[0])throw std::logic_error("Enemy frame requires actual Context+4 primary entity");
    set_primary_entity_flag(enemy.context->objects_04[0],primary_entity_scale(enemy.context->objects_04[0])>1.01f?1u:0u);
    set_primary_entity_scale(enemy.context->objects_04[0],1.0f);
    recovered::timer_tick(enemy.data.timer_8c,host.timer_rate());
    return 1; //004a5300 MOV EAX,1; Ghidra incorrectly inferred void.
}
}
