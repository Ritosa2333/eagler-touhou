#include "EclVm.hpp"
#include <cmath>
namespace th08 {
void EclVm::refresh_position()noexcept {
    resolved_position={Scalar::add(position.x,position_offset.x),Scalar::add(position.y,position_offset.y),Scalar::add(position.z,position_offset.z)};
}
i32 EclVm::familiar_count()const noexcept {
    const auto* owner=parent?parent:this;
    if(owner->parent)return 0;
    i32 count=0;for(auto* p=owner;p->next_familiar&&count<481;p=p->next_familiar)++count;
    return wrapping_add(count,owner->practice_familiar);
}
// Original writable resolvers: 0x41fe10 (integer), 0x420950 (float).
// Read-only variables intentionally do not return a writable field.
i32* EclVm::integer_field(i32 id)noexcept {
    if(id>=10008&&id<=10015)return shared_integer+id-10008;
    if(auto* local=context().integer_variable(id))return local;
    if(environment){
        if(id>=10061&&id<=10064)return environment->integer_arguments+id-10061;
        if(id==10040)return reinterpret_cast<i32*>(&environment->difficulty);
        if(id==10041)return environment->rank_value?environment->rank_value:&environment->rank;
    }
    switch(id){case 10049:return &lifetime.current;case 10051:return &life;case 10092:return &item_reward;case 10093:return &score_reward;default:return nullptr;}
}
float* EclVm::float_field(i32 id)noexcept {
    if(id>=10024&&id<=10031)return shared_real+id-10024;
    if(auto* local=context().float_variable(id))return local;
    if(environment){
        if(id>=10065&&id<=10068)return environment->float_arguments+id-10065;
        switch(id){case 10045:return &environment->player.x;case 10046:return &environment->player.y;case 10047:return &environment->player.z;}
    }
    switch(id){
    case 10042:return &position.x;case 10043:return &position.y;case 10044:return &position.z;
    case 10069:return &direction.z;case 10070:return &angular_velocity;case 10071:return &speed;case 10072:return &acceleration;case 10073:return &orbit_radius;
    case 10074:return &origin.x;case 10075:return &origin.y;case 10076:return &origin.z;
    case 10077:return &orbit_angle;case 10078:return &orbit_velocity;
    case 10079:return &target.x;case 10080:return &target.y;case 10081:return &target.z;
    default:return nullptr;
    }
}
void EclVm::write_int(i32 id,i32 value)noexcept {if(auto* field=integer_field(id))*field=value;}
void EclVm::write_float(i32 id,float value)noexcept {if(auto* field=float_field(id))*field=value;}
// These reads are numeric conversions, not reinterpretation of float bits.
i32 EclVm::read_int(i32 id)const noexcept {
    auto& self=*const_cast<EclVm*>(this);
    if(auto* field=self.integer_field(id))return *field;
    // The original integer reader has no TARGET_X/Y/Z or RAND_ANGLE cases.
    if(id>=10079&&id<=10082)return id;
    if(random)switch(id){case 10032:return i32(random->next32()&0x7fffffff);case 10033:return random->unit().truncate_int();case 10034:return signed_bits(random->next32());case 10035:return random->signed_unit().truncate_int();}
    if(id==10052&&environment)return environment->shot;
    if(id==10083)return last_damage;
    if(id==10084)return u8(boss_id);
    if(id>=10088&&id<=10091)return life_thresholds[id-10088];
    if(id==10096)return familiar_count();
    if(id==10097&&environment)return environment->youkai;
    if(id==10098&&environment){
        const auto* values=environment->values;
        const i32 current=values?values->time_orbs:0,quota=values?values->last_spell_requirement:0;
        const auto* live=environment->live_values;
        return wrapping_add(wrapping_add(current,live?live->pending_time():environment->pending_time),live?live->uncollected_time_items():environment->uncollected_time_items)<quota?0:2;
    }
    if(id==10099&&environment)return (environment->spell_flags&((environment->spell_flags&1)?4:512))!=0;
    // The original 0041fdd0 reads Spellcard::spell_remaining.current at
    // 004ea670+0x108. This is a frame timer, not the GUI's rounded seconds.
    if(id==10100&&environment)return environment->spell_remaining.current;
    if(self.float_field(id)||id==10048||id==10050||(id>=10085&&id<=10087))return resolve_float(float(id)).truncate_int();
    return id;
}
float EclVm::read_float(i32 id)const noexcept{return resolve_float(Extended::from_int(id).to_float()).to_float();}
Extended EclVm::resolve_float(float value)const noexcept {
    const i32 id=Scalar::truncate(value);auto& self=*const_cast<EclVm*>(this);
    if(id>=10042&&id<=10044)return number(id==10042?resolved_position.x:id==10043?resolved_position.y:resolved_position.z);
    if(auto* field=self.float_field(id))return number(*field);
    if(auto* field=self.integer_field(id))return Extended::from_int(*field);
    if(random)switch(id){case 10032:return Extended::from_int(i32(random->next32()&0x7fffffff));case 10033:return random->unit();case 10034:return Extended::from_int(signed_bits(random->next32()));case 10035:return random->signed_unit();case 10082:return random->range(6.283185482025147f)-number(3.1415927410125732f);}
    if(environment&&(id==10048||id==10050)){
        const auto& player=environment->player;
        const float x=(number(player.x)-resolve_float(10042.f)).to_float(),y=(number(player.y)-resolve_float(10043.f)).to_float();
        if(id==10048){if(x==0&&y==0)return number(1.5707963705062866f);return Extended::from_double(std::atan2(double(y),double(x)));}
        const float z=(number(player.z)-resolve_float(10044.f)).to_float();
        return number((number(x)*number(x)+number(y)*number(y)+number(z)*number(z)).to_float()).square_root();
    }
    if(id==10052&&environment)return Extended::from_int(environment->shot);
    if(id==10083)return Extended::from_int(last_damage);
    if(id==10084)return Extended::from_int(u8(boss_id));
    if(id>=10085&&id<=10087)return number(id==10085?last_delta.x:id==10086?last_delta.y:last_delta.z);
    if(id>=10088&&id<=10091)return Extended::from_int(life_thresholds[id-10088]);
    if(id==10096)return Extended::from_int(familiar_count());
    if(id==10097&&environment)return Extended::from_int(environment->youkai);
    if(id==10099&&environment)return Extended::from_int(read_int(id));
    return number(value);
}
}
