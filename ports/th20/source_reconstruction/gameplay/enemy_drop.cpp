#include "enemy_drop.hpp"
#include "enemy_opcode_data.hpp"
#include "../ecl_vm/math.hpp"
#include <algorithm>
namespace th20::source::gameplay {
namespace n=th20::recovered;
namespace {
float divide(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
int add(int a,int b){return n::signed_bits(unsigned(a)+unsigned(b));}
int subtract(int a,int b){return n::signed_bits(unsigned(a)-unsigned(b));}
}
void reset_enemy_drop_counts(EnemyPatternState& p) noexcept {
    std::fill_n(p.fields_00+3,16,0u);p.field_8c=0;n::timer_set(p.timer_90,0);
}
void emit_enemy_pattern_items(EnemyPatternState& p,const sprite::Vec3& origin,EnemyDropServices& env){
    float angle=env.random_angle(); //Consumed even when no item count is positive.
    for(int i=0;i<15;++i){
        int count=n::signed_bits(p.fields_00[3+i]);const int duration=n::signed_bits(p.field_8c);
        if(duration&&p.timer_90.current<duration){
            const int extra=n::signed_bits(p.fields_00[19+i]);
            if(extra>0){const int product=n::signed_bits(unsigned(p.timer_90.current)*unsigned(extra));count=add(count,subtract(extra,product/duration));}
        }
        for(int j=0;j<count;++j){
            sprite::Vec3 position{n::mul32(ecl::math::cosine(angle),p.field_a0),n::mul32(ecl::math::sine(angle),p.field_a4),0};
            const float scale=n::add32(n::mul32(env.random_unit(),opcode_data::f_0056e0ec),opcode_data::f_0056e0ec);
            position.x=n::add32(n::mul32(position.x,scale),origin.x);position.y=n::add32(n::mul32(position.y,scale),origin.y);position.z=origin.z;
            env.spawn(n::signed_bits(p.fields_00[0]),i+1,position);
            const float next=n::add32(divide(opcode_data::f_0056e0f0,opcode_data::f_0056c8d0),angle);
            angle=ecl::math::wrap_angle(n::add32(next,divide(env.random_angle(),opcode_data::f_0056c8e0)));
        }
    }
    std::fill_n(p.fields_00+3,16,0u);
}
void emit_enemy_drop(EnemyPatternState& p,const sprite::Vec3& origin,bool bomb_mark,EnemyDropServices& env){
    const auto kind=(p.fields_00[2]&&!bomb_mark)?p.fields_00[2]:p.fields_00[1];
    if(kind)env.spawn(n::signed_bits(p.fields_00[0]),n::signed_bits(kind),origin);
    emit_enemy_pattern_items(p,origin,env);p.fields_00[1]=0;
}
}
