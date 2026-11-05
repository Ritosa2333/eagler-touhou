#include "enemy_movement.hpp"
#include "enemy_interpolation.hpp"
#include "enemy_variables.hpp"
#include "../runtime_state/motion.hpp"
#include "../ecl_vm/math.hpp"
#include <cmath>
#include <cstring>
#include <stdexcept>
namespace th20::source::gameplay {
namespace {
float subtract(float a,float b) noexcept{return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float divide(float a,float b) noexcept{return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
sprite::Vec3 position(const EnemyMotion& motion) noexcept {sprite::Vec3 result;std::memcpy(&result,motion.words,12);return result;}
sprite::Vec3 add(sprite::Vec3 a,sprite::Vec3 b) noexcept {return{recovered::add32(a.x,b.x),recovered::add32(a.y,b.y),recovered::add32(a.z,b.z)};}
sprite::Vec3 sub(sprite::Vec3 a,sprite::Vec3 b) noexcept {return{subtract(a.x,b.x),subtract(a.y,b.y),subtract(a.z,b.z)};}
float field(const std::uint32_t& value) noexcept {float result;std::memcpy(&result,&value,4);return result;}
}
void combine_enemy_movements(EnemyState& enemy,float clock_scale) {
    sprite::Vec3 total{};
    for(auto& movement:enemy.movements)total=add(total,position(movement.motion));
    th20::source::state::Motion motion;std::memcpy(&motion,&enemy.motion_110,sizeof(motion));
    motion.vector_38=sub(total,motion.position);             //47a5e0
    th20::source::state::update_motion_position(motion,clock_scale);
    if(enemy.fields_2c8[1]&2u) {
        const auto clamp=[](float value,float center,float extent) {
            const auto lower=subtract(center,divide(extent,2));
            if(lower>value)return lower;                    //COMISS/JBE skips clamp for unordered
            const auto upper=recovered::add32(divide(extent,2),center);
            return upper<value?upper:value;
        };
        motion.position.x=clamp(motion.position.x,field(enemy.fields_178[0]),field(enemy.fields_178[2]));
        motion.position.y=clamp(motion.position.y,field(enemy.fields_178[1]),field(enemy.fields_178[3]));
        auto first=motion.position;
        for(std::size_t i=1;i<enemy.movements.size();++i)first=sub(first,position(enemy.movements[i].motion));
        if(enemy.movements.empty())throw std::out_of_range("Bounded Enemy requires first movement record");
        std::memcpy(enemy.movements[0].motion.words,&first,12); //4393f0
    }
    std::memcpy(&enemy.motion_110,&motion,sizeof(motion));
}
int update_enemy_movement(EnemyState& enemy,EnemyMovementServices& host) {
    enemy.motion_c8=enemy.motion_110;
    if(enemy.fields_2c8[2]&8u) {
        scheduler::List* parent_list;std::memcpy(&parent_list,static_cast<std::uint8_t*>(enemy.entity)+0x3f0,4); //4aad90
        if(!parent_list||enemy.movements.empty())throw std::logic_error("Parent-following Enemy requires parent and first movement");
        const auto origin=enemy_position(parent_list->sentinel.value);
        std::memcpy(enemy.movements[0].motion.words,&origin,12); //477b80
    }
    for(auto& record:enemy.movements) {
        th20::source::state::Motion motion;std::memcpy(&motion,&record.motion,sizeof(motion));
        if(record.scalar_ac.duration&&(motion.field_44&15u)!=2&&(motion.field_44&15u)!=3)
            motion.angle_1c=ecl::math::wrap_angle(ecl::math::wrap_angle(sample_enemy_scalar_interpolation(record.scalar_ac,host.timer_rate()))); //438540 then47a560->452f20
        if(record.scalar_d8.duration)motion.field_18=sample_enemy_scalar_interpolation(record.scalar_d8,host.timer_rate());
        if(record.vector_104.duration) {
            const auto values=sample_enemy_vector_interpolation(record.vector_104,host.timer_rate());motion.field_20=values.x;motion.field_24=values.y;
        }
        if(record.position.duration)motion.vector_38=sub(sample_enemy_motion_interpolation(record.position,host.timer_rate()),motion.position);
        else th20::source::state::update_motion_velocity(motion,host.clock_scale());
        th20::source::state::update_motion_position(motion,host.clock_scale());
        if(enemy.fields_2c8[1]&0x400u)motion.position=add(motion.position,host.viewport_offset());
        std::memcpy(&record.motion,&motion,sizeof(motion));
    }
    combine_enemy_movements(enemy,host.clock_scale());
    const auto resolve=[&](std::uint32_t& handle){auto* result=host.animation(handle);if(!result)handle=0;return result;};
    if(enemy.animations.empty())throw std::out_of_range("Enemy movement requires first animation record");
    if(enemy.fields_2c8[1]&0x10u) {
        const float velocity=field(enemy.motion_110.words[0x38/4]);
        //Original JA/JBE branch treats unordered velocity as neutral.
        const int direction=velocity<-.03f?-1:velocity>.03f?1:0;
        auto& old=enemy.fields_1c[(0x2c-0x1c)/4];
        if(recovered::signed_bits(old)!=direction) {
            int transition=0;
            switch(recovered::signed_bits(old)) {
            case -1:transition=direction==0?3:2;break;
            case 0:transition=direction==-1?1:2;break;
            case 1:transition=direction==0?4:1;break;
            }
            auto* previous=resolve(enemy.animations[0].handle);
            sprite::Vec3 origin{};
            auto& file=host.animation_file(enemy,enemy.fields_1c[1]); //+20, lookup occurs before old handle retirement
            if(previous){origin=previous->base.vector_2c;host.delete_animation(enemy.animations[0].handle);}
            const auto script=recovered::signed_bits(enemy.fields_1c[3]+static_cast<std::uint32_t>(transition));
            const auto layer=recovered::signed_bits(enemy.fields_1c[6]+7u);
            enemy.animations[0].handle=host.spawn_animation(file,script,origin,layer);
            old=static_cast<std::uint32_t>(direction);
        }
    }
    if(auto* animation=resolve(enemy.animations[0].handle)) {
        enemy.vector_170.x=static_cast<float>(std::fabs(static_cast<double>(host.animation_height(*animation))));
        enemy.vector_170.y=static_cast<float>(std::fabs(static_cast<double>(host.animation_width(*animation))));
    }
    const auto p=position(enemy.motion_110);const auto halfx=divide(enemy.vector_170.x,2),halfy=divide(enemy.vector_170.y,2);
    if(-192.0f>recovered::add32(p.x,halfx)||subtract(p.x,halfx)>192.0f) {
        enemy.fields_2c8[2]&=~0x40u;
        if((enemy.fields_2c8[1]&1u)&&!(enemy.fields_2c8[0]&4u))return -1;
    } else if(0.0f>recovered::add32(p.y,halfy)||subtract(p.y,halfy)>448.0f) {
        enemy.fields_2c8[2]&=~0x40u;
        if((enemy.fields_2c8[1]&1u)&&!(enemy.fields_2c8[0]&8u))return -1;
    } else {enemy.fields_2c8[1]|=1u;enemy.fields_2c8[2]|=0x40u;}
    return 0;
}
}
