#include "EclVariables.hpp"
#include "Arithmetic.hpp"
#include <cmath>
namespace th09 {
i32* EclLocals::integer(i32 id) noexcept {
    if(id>=10000&&id<10008)return integers+id-10000;
    if(id>=10036&&id<10040)return counters+id-10036;
    if(id>=10053&&id<10057)return integer_arguments+id-10053;return nullptr;
}
float* EclLocals::real(i32 id) noexcept {
    if(id>=10016&&id<10024)return floats+id-10016;
    if(id>=10094&&id<10096)return extra+id-10094;
    if(id>=10057&&id<10061)return float_arguments+id-10057;return nullptr;
}
i32* EclVariables::integer_field(i32 id) noexcept {
    if(id>=10008&&id<10016)return shared_integer+id-10008;
    if(locals)if(auto p=locals->integer(id))return p;
    if(field&&id>=10061&&id<10065)return field->integer_arguments+id-10061;
    if(world){if(id==10040)return &world->difficulty;if(id==10041)return &world->rank;}
    switch(id){case 10049:return &lifetime.current;case 10051:return &life;case 10092:return &item_reward;case 10093:return &score_reward;default:return nullptr;}
}
float* EclVariables::float_field(i32 id) noexcept {
    if(id>=10024&&id<10032)return shared_real+id-10024;
    if(locals)if(auto p=locals->real(id))return p;
    if(field){
        if(id>=10065&&id<10069)return field->float_arguments+id-10065;
        switch(id){case 10045:return &field->player.x;case 10046:return &field->player.y;case 10047:return &field->player.z;}
    }
    switch(id){
    case 10042:return &position.x;case 10043:return &position.y;case 10044:return &position.z;
    case 10069:return &direction;case 10070:return &angular_velocity;case 10071:return &speed;case 10072:return &acceleration;case 10073:return &orbit_radius;
    case 10074:return &origin.x;case 10075:return &origin.y;case 10076:return &origin.z;
    case 10077:return &orbit_angle;case 10078:return &orbit_velocity;
    case 10079:return &target.x;case 10080:return &target.y;case 10081:return &target.z;default:return nullptr;
    }
}
i32 EclVariables::read_int(i32 id) noexcept {
    if(auto p=integer_field(id))return *p;
    // The original integer resolver deliberately lacks target/RAND_ANGLE cases.
    if(id>=10079&&id<=10082)return id;
    if(world)switch(id){
    case 10032:return i32(world->random.next32()&0x7fffffff);
    case 10033:return truncate(world->random.unit());
    case 10034:return signed_bits(world->random.next32());
    case 10035:return truncate(world->random.signed_unit());
    }
    if(id==10052&&field)return field->character;
    if(id==10083)return last_damage;if(id==10084)return boss_id;
    if(id>=10088&&id<10092)return life_thresholds[id-10088];
    if(id==10101&&opponent)return opponent->attack_levels[(flags&0xc00)==0xc00];
    if(float_field(id)||id==10048||id==10050||(id>=10085&&id<=10087))return truncate(read_value(float(id)));
    return id;
}
float EclVariables::read_float(float value) noexcept {return float(read_value(value));}
double EclVariables::read_value(float value) noexcept {
    const i32 id=truncate(value);
    switch(id){case 10042:return resolved_position.x;case 10043:return resolved_position.y;case 10044:return resolved_position.z;}
    if(auto p=float_field(id))return *p;
    if(auto p=integer_field(id))return double(*p);
    if(world)switch(id){
    case 10032:return double(world->random.next32()&0x7fffffff);
    case 10033:return float(world->random.unit());
    case 10034:return double(signed_bits(world->random.next32()));
    case 10035:return float(world->random.signed_unit());
    case 10082:return float(world->random.range(6.283185482025147f))-3.1415927410125732f;
    }
    if(field&&(id==10048||id==10050)){
        const float x=field->player.x-resolved_position.x,y=field->player.y-resolved_position.y;
        if(id==10048)return x==0&&y==0?1.5707963705062866:std::atan2(double(y),double(x));
        const float z=field->player.z-resolved_position.z;
        return float(std::sqrt(double((x*x+y*y)+z*z)));
    }
    if(id==10052&&field)return double(field->character);
    if(id==10083)return double(last_damage);if(id==10084)return double(boss_id);
    switch(id){case 10085:return last_delta.x;case 10086:return last_delta.y;case 10087:return last_delta.z;}
    if(id>=10088&&id<10092)return double(life_thresholds[id-10088]);
    if(id==10101&&opponent)return double(opponent->attack_levels[(flags&0xc00)==0xc00]);
    return value;
}
}
