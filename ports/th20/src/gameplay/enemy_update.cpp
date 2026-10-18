#include "enemy_update.hpp"
#include "../ecl_vm/math.hpp"
#include <cmath>
#include <cstring>
#include <stdexcept>
namespace th20::source::gameplay {
namespace {
float motion_float(const EnemyMotion& motion,unsigned offset) noexcept {
    float value;std::memcpy(&value,&motion.words[offset/4],4);return value;
}
sprite::Vec3 motion_position(const EnemyMotion& motion) noexcept {
    sprite::Vec3 value;std::memcpy(&value,motion.words,12);return value;
}
sprite::Animation* resolve(EnemyUpdateServices& host,std::uint32_t& handle) {
    auto* result=host.animation(handle);if(!result)handle=0;return result; //44ced0
}
void set_angle(sprite::Animation& animation,float angle) noexcept {
    animation.base.vector_38.z=angle;animation.base.flags[1]|=2u; //450590->4505c0
}
void update_animation_transform(EnemyState& state,EnemyAnimationLink& link,EnemyUpdateServices& host) {
    auto* animation=resolve(host,link.handle);if(!animation)return;
    const auto motion=motion_position(state.motion_110);
    sprite::Vec3 position{recovered::add32(link.offset[0],motion.x),recovered::add32(link.offset[1],motion.y),recovered::add32(link.offset[2],motion.z)};
    if(link.parent>=0) {
        if(static_cast<unsigned>(link.parent)>=state.animations.size())throw std::out_of_range("Original Enemy parent animation index");
        if(auto* parent=resolve(host,state.animations[link.parent].handle)) {
            position.x=recovered::add32(position.x,parent->base.vector_2c.x);
            position.y=recovered::add32(position.y,parent->base.vector_2c.y);
            position.z=recovered::add32(position.z,parent->base.vector_2c.z);
        }
    }
    animation->vector_5bc=position;                          //45dd60
    const auto mode=(static_cast<std::uint8_t>(animation->base.flags[2]>>16)>>5);
    if(mode<1||mode>5)return;
    const float angle=ecl::math::arctangent(motion_float(state.motion_110,0x3c),motion_float(state.motion_110,0x38));
    constexpr float pi=3.1415927410125732421875f,half_pi=1.57079637050628662109375f;
    switch(mode) {
    case 1:set_angle(*animation,angle);break;
    case 2: {
        const float wrapped=ecl::math::wrap_angle(angle);    //429210 angle constructor
        //Both COMISS/JB comparisons also select reversal for unordered input.
        const bool reversed=!(wrapped>=-half_pi)||!(wrapped<=half_pi);
        set_angle(*animation,reversed?ecl::math::wrap_angle(recovered::add32(wrapped,pi)):wrapped); //452fc0->429210
        float scale=static_cast<float>(std::fabs(static_cast<double>(animation->base.vector_50.x))); //47a630
        if(reversed) {std::uint32_t bits;std::memcpy(&bits,&scale,4);bits^=0x80000000u;std::memcpy(&scale,&bits,4);}
        animation->base.vector_50.x=scale;animation->base.flags[1]|=4u; //4777f0
        break;
    }
    case 3:set_angle(*animation,ecl::math::wrap_angle(recovered::add32(angle,pi)));break;
    case 4:set_angle(*animation,ecl::math::wrap_angle(recovered::add32(angle,half_pi)));break;
    case 5:set_angle(*animation,ecl::math::wrap_angle(recovered::add32(angle,-half_pi)));break;
    }
    std::memcpy(&state.fields_1c[(0x38-0x1c)/4],&animation->base.vector_38.z,4); //437930
}
}
int advance_enemy_scripts(EnemyState& state,EnemyUpdateServices& host) {
    auto& flags=state.fields_2c8[1];
    if(flags&0x4000000u)return 0;
    flags|=0x4000000u;
    if(host.move(state)!=0)return -1;
    if(host.run_scripts(state.entity,host.script_delta(state.timer_a8))!=0)return -1;
    if(state.fields_2c8[6]) {
#ifdef TH_SDL3
        // Stored callbacks are __fastcall (EnemyState*,void* dummy); wasm
        // lowers conventions away, so the dummy is a real argument here.
        const auto callback=reinterpret_cast<int(*)(EnemyState*,void*)>(state.fields_2c8[6]);
        if(callback(&state,nullptr)!=0)return -1;
#else
        const auto callback=reinterpret_cast<int(__thiscall*)(EnemyState*)>(state.fields_2c8[6]);
        if(callback(&state)!=0)return -1;                    //004ab54e ECX=EnemyState
#endif
    }
    return 0;
}
int update_enemy_state(EnemyState& state,EnemyUpdateServices& host) {
    auto& flags=state.fields_2c8[1];
    if(flags&4u)return 0;
    flags|=4u;
    if(advance_enemy_scripts(state,host)!=0||host.damage(state)!=0)return -1;
    host.mesh(state);
    if(!(flags&0x400u)) {
        for(auto& link:state.animations)update_animation_transform(state,link,host);
    } else {
        //4645e0->4502c0->450270 calls non-mutating44cd00; stale handles
        // survive here, and neither per-link offsets nor parents are applied.
        for(auto& link:state.animations)if(auto* animation=host.animation(link.handle))animation->vector_5bc=motion_position(state.motion_110);
    }
    recovered::timer_tick(state.pattern_1a8.timer_90,host.timer_rate()); //4a5320
    if(state.timer_288.current>0)recovered::timer_add(state.timer_288,-1.0f,host.timer_rate());
    if(state.timer_298.current>0)recovered::timer_add(state.timer_298,-1.0f,host.timer_rate());
    recovered::timer_tick(state.timer_b8,host.timer_rate());
    recovered::timer_tick(state.timer_a8,host.timer_rate());
    flags&=~0x4000000u;
    return 0;
}
}
