#include "enemy_reads.hpp"
#include "enemy_variables.hpp"
#include "../player_entity/player.hpp"
#include <algorithm>
#include <cstring>
namespace th20::source::gameplay {
namespace {
float subtract(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
template<class T>T get(const void* object,std::size_t offset){T value;std::memcpy(&value,static_cast<const std::uint8_t*>(object)+offset,sizeof(value));return value;}
std::int32_t clamp(void* object,std::size_t offset,std::int32_t low,std::int32_t high){auto value=get<std::int32_t>(object,offset);value=(std::clamp)(value,low,high);std::memcpy(static_cast<std::uint8_t*>(object)+offset,&value,4);return value;}
struct Value {
    enum class Kind {signed_integer,unsigned_integer,floating};
    std::uint32_t bits;Kind kind;
    static Value integer(std::uint32_t value){return {value,Kind::signed_integer};}
    static Value unsigned_integer(std::uint32_t value){return {value,Kind::unsigned_integer};}
    static Value real(float value){return {ecl::float_bits(value),Kind::floating};}
    std::int32_t as_integer() const{return kind==Kind::floating?ecl::truncate_float(ecl::bits_float(bits)):ecl::bits_int(bits);}
    float as_float() const{return kind==Kind::floating?ecl::bits_float(bits):kind==Kind::unsigned_integer?static_cast<float>(static_cast<double>(bits)):static_cast<float>(ecl::bits_int(bits));}
};
EnemyController& owner(Enemy& entity){return *static_cast<EnemyController*>(entity.context->objects_04[1]);}
Enemy* selected(EnemyReadEnvironment& environment,EnemyController& host,unsigned slot=0){
    const auto* first=static_cast<EnemyController*>(environment.session.contexts[0].objects_04[1]);
    return first?static_cast<Enemy*>(find_enemy_in_list(const_cast<scheduler::List&>(first->enemies),host.data.handles_44[slot])):nullptr;
}
float direction(const void* entity){return ecl::math::arctangent(get<float>(entity,0x1d4),get<float>(entity,0x1d0));} //497540 ->456210
float distance(const void* a,const void* b,std::size_t b_position){
    const float dx=subtract(get<float>(a,0x198),get<float>(b,b_position));
    const float dy=subtract(get<float>(a,0x19c),get<float>(b,b_position+4));
    return ecl::math::square_root(recovered::add32(recovered::mul32(dx,dx),recovered::mul32(dy,dy)));
}
std::uint32_t count_enemies(EnemyController& host,bool marked_only){
    std::uint32_t count=0;
    for(scheduler::Iterator iterator(host.enemies.sentinel.next);iterator.current;iterator.advance()){
        const auto* entity=reinterpret_cast<const Enemy*>(iterator.current->value);
        if(marked_only){if(entity->state.fields_2c8[1]&0x10000000u)++count;}
        else if(!(entity->state.fields_2c8[0]&0x31u)&&entity->state.timer_288.current<=0)++count;
    }
    return count;
}
Value query(Enemy& entity,std::uint32_t code,bool floating,EnemyReadEnvironment& environment){
    using V=Value;auto& session=environment.session;
    if(code>=0xffffd8ffu&&code<=0xffffd902u)return V::integer(get<std::uint32_t>(&entity,0x100+(code-0xffffd8ffu)*4));
    if(code>=0xffffd903u&&code<=0xffffd906u)return V::real(get<float>(&entity,0x110+(code-0xffffd903u)*4));
    if(code>=0xffffd923u&&code<=0xffffd925u)return V::integer(get<std::uint32_t>(&owner(entity),0x48+(code-0xffffd923u)*4));
    if(code>=0xffffd929u&&code<=0xffffd930u){
        if(!selected(environment,owner(entity)))return V::integer(0);
        const auto* target=selected(environment,owner(entity));const auto index=code-0xffffd929u;
        return index<4?V::integer(get<std::uint32_t>(target,0x100+index*4)):V::real(get<float>(target,0x100+index*4));
    }
    if(code>=0xffffd931u&&code<=0xffffd934u)return V::real(get<float>(&entity,0x120+(code-0xffffd931u)*4));
    if(code>=0xffffd93au&&code<=0xffffd93du)return V::integer(get<std::uint32_t>(&owner(entity),0x10+(code-0xffffd93au)*4));
    if(code>=0xffffd93eu&&code<=0xffffd945u)return V::real(get<float>(&owner(entity),0x20+(code-0xffffd93eu)*4));
    if(code>=0xffffd959u&&code<=0xffffd95cu)return V::integer(floating?0:enemy_script_globals[code-0xffffd959u]);
    switch(code){
    case 0xffffd8f0:return V::integer(state::next(environment.random)&0x7fffffffu);
    case 0xffffd8f1:return V::real(state::unit(environment.random));
    case 0xffffd8f2:return floating?V::real(recovered::mul32(state::signed_unit(environment.random),3.1415927410125732421875f)):V::integer(0);
    case 0xffffd8f3:case 0xffffd8f4:return V::real(get<float>(&entity,0x198+(code-0xffffd8f3u)*4));
    case 0xffffd8f5:case 0xffffd8f6:case 0xffffd8f7:case 0xffffd8f8:{const auto index=code-0xffffd8f5u;return V::real(get<float>(&entity.state.movements.at(index/2).motion,(index%2)*4));}
    case 0xffffd8f9:case 0xffffd8fa:return V::real(get<float>(entity.context->objects_04[0],0x614+(code-0xffffd8f9u)*4));
    case 0xffffd8fb:return floating?V::real(player_entity::angle_to_player(entity.context->objects_04[0],enemy_position(&entity))):V::integer(0);
    case 0xffffd8fc:return floating?V::real(get<float>(&entity,0x138)):V::integer(get<std::uint32_t>(&entity,0x134));
    case 0xffffd8fd:return V::real(state::signed_unit(environment.random));
    case 0xffffd8fe:return V::integer((entity.state.fields_2c8[1]>>8)&1u);
    case 0xffffd907:case 0xffffd908:{const auto axis=(code-0xffffd907u)*4;return V::real(subtract(get<float>(&entity,0x198+axis),get<float>(&entity,0x150+axis)));}
    case 0xffffd909:case 0xffffd90a:case 0xffffd90b:case 0xffffd90c:{const auto index=code-0xffffd909u;return V::real(get<float>(&entity.state.movements.at(index/2).motion,0x38+(index%2)*4));}
    case 0xffffd90d:case 0xffffd90e:return V::real(get<float>(&entity.state.movements.at(code-0xffffd90du).motion,0x1c));
    case 0xffffd90f:case 0xffffd910:return V::real(get<float>(&entity.state.movements.at(code-0xffffd90fu).motion,0x18));
    case 0xffffd911:case 0xffffd912:return V::real(get<float>(&entity.state.movements.at(code-0xffffd911u).motion,0x20));
    case 0xffffd913:case 0xffffd914:return V::real(get<float>(entity.context->objects_04[0],0x614+(code-0xffffd913u)*4));
    case 0xffffd915:case 0xffffd916:{
        if(floating&&!selected(environment,owner(entity)))return V::real(code==0xffffd915u?0.f:128.f);
        return V::real(get<float>(selected(environment,owner(entity)),0x198+(code-0xffffd915u)*4));
    }
    case 0xffffd917:{if(floating)return V::integer(0);auto* animation=environment.animation(entity.state.animations.at(0).handle);return V::integer(get<std::int16_t>(animation,0x440));}
    case 0xffffd919:return V::integer(get<std::uint32_t>(&entity,0xd8));
    case 0xffffd91a:return V::real(direction(&entity));
    case 0xffffd91b:return V::integer(1);
    case 0xffffd91c:case 0xffffd91d:{if(!floating)return V::integer(0);const auto position=get<sprite::Vec3>(&entity.state.movements.at(code-0xffffd91cu).motion,0);return V::real(player_entity::angle_to_player(entity.context->objects_04[0],position));}
    case 0xffffd91e:return V::integer(get<std::uint32_t>(&entity,0x214));
    case 0xffffd91f:case 0xffffd920:case 0xffffd921:case 0xffffd922:return V::integer(session.player_table.field_1e0==code-0xffffd91fu);
    case 0xffffd926:return V::integer(owner(entity).field_124);
    case 0xffffd927:return V::integer(get<std::uint32_t>(entity.context->current_player,8));
    case 0xffffd928:return V::real(distance(&entity,entity.context->objects_04[0],0x614));
    case 0xffffd935:return V::unsigned_integer(previous_enemy_generation);
    case 0xffffd936:return V::integer(clamp(entity.context->current_player,0x30,0,400));
    case 0xffffd939:return V::integer(environment.restart_mode==0&&environment.new_game_state!=0);
    case 0xffffd946:return V::unsigned_integer(entity.state.identifier);
    case 0xffffd949:case 0xffffd94a:{if(!selected(environment,owner(entity)))return V::integer(0);auto* target=selected(environment,owner(entity));return V::real(code==0xffffd949u?direction(target):get<float>(&target->state.movements.at(0).motion,0x18));}
    case 0xffffd94b:return V::unsigned_integer(entity.state.field_04);
    case 0xffffd94c:return V::integer(count_enemies(owner(entity),false));
    case 0xffffd94d:return V::integer(clamp(&session.player_table,0x204,-1,9999));
    case 0xffffd94e:return V::integer((entity.state.fields_2c8[1]>>3)&1u);
    case 0xffffd94f:return V::integer(clamp(&session.player_table,0x1fc,0,999));
    case 0xffffd950:return floating?V::integer(0):V::integer(clamp(&session.player_table,0x208,0,99999));
    case 0xffffd955:return V::integer(environment.replay_selection);
    case 0xffffd956:return V::integer(get<std::uint32_t>(entity.context->primary_owner,0x44));
    case 0xffffd958:return V::integer(floating?0:owner(entity).field_e0);
    case 0xffffd95f:return V::integer(floating?0:count_enemies(owner(entity),true));
    case 0xffffd961:case 0xffffd962:return V::integer(get<std::uint32_t>(&owner(entity),0x40+(code-0xffffd961u)*4));
    case 0xffffd963:return V::integer(session.player_table.field_1e0);
    case 0xffffd964:case 0xffffd965:return V::real(get<float>(session.contexts[1-entity.player_index].objects_04[0],0x614+(code-0xffffd964u)*4));
    case 0xffffd966:return V::integer(entity.player_index);
    case 0xffffd967:return V::integer(clamp(session.contexts[0].current_player,0x5c,0,10000));
    case 0xffffd968:{const auto value=get<std::int32_t>(session.contexts[0].current_player,0xc);return floating?V::real(static_cast<float>(value)/2.f):V::integer(value/2);}
    case 0xffffd969:return V::integer((entity.state.fields_2c8[2]>>6)&1u);
    case 0xffffd96a:return V::integer(state::next(environment.random)%2?0xffffffffu:1u);
    case 0xffffd96b:return V::integer(get<std::uint32_t>(session.contexts[0].primary_owner,0x44));
    case 0xffffd96c:return V::integer(get<std::uint32_t>(session.contexts[0].objects_04[3],0x78)&1u);
    case 0xffffd96d:case 0xffffd96e:{
        auto& first=*static_cast<EnemyController*>(session.contexts[0].objects_04[1]);
        for(unsigned slot=0;slot<16;++slot)if(first.data.handles_44[slot])return code==0xffffd96du?V::integer(1):V::real(distance(&entity,selected(environment,first,slot),0x198));
        return V::integer(code==0xffffd96du?0:0xffffffffu);
    }
    default:return V::integer(0);
    }
}
}
std::int32_t read_enemy_integer(Enemy& entity,std::int32_t variable,EnemyReadEnvironment& environment){return query(entity,static_cast<std::uint32_t>(variable),false,environment).as_integer();}
float read_enemy_float(Enemy& entity,std::int32_t variable,EnemyReadEnvironment& environment){return query(entity,static_cast<std::uint32_t>(variable),true,environment).as_float();}
}
