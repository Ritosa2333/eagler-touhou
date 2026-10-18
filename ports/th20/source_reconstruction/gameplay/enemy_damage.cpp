#include "enemy_damage.hpp"
#include "enemy_variables.hpp"
#include "../ecl_vm/math.hpp"
#include <cstring>
#include <stdexcept>
namespace th20::source::gameplay {
namespace {
template<class T>T get(const void* p,std::size_t offset){T value;std::memcpy(&value,static_cast<const std::uint8_t*>(p)+offset,sizeof(value));return value;}
template<class T>void put(void* p,std::size_t offset,T value){std::memcpy(static_cast<std::uint8_t*>(p)+offset,&value,sizeof(value));}
std::int32_t add(std::int32_t a,std::int32_t b){return recovered::signed_bits(static_cast<std::uint32_t>(a)+static_cast<std::uint32_t>(b));}
std::int32_t subtract(std::int32_t a,std::int32_t b){return recovered::signed_bits(static_cast<std::uint32_t>(a)-static_cast<std::uint32_t>(b));}
std::int32_t multiply(std::int32_t a,std::int32_t b){return recovered::signed_bits(static_cast<std::uint32_t>(a)*static_cast<std::uint32_t>(b));}
std::int32_t clamp_field(void* p,std::size_t offset,std::int32_t maximum){auto value=get<std::int32_t>(p,offset);if(value<0)value=0;if(value>maximum)value=maximum;put(p,offset,value);return value;}
game_session::Context& context(EnemyState& s){return *reinterpret_cast<game_session::Context*>(s.fields_2c8[9]);}
bool bomb_active(EnemyState& s){return get<std::int32_t>(context(s).objects_04[5],0x18)==1;} //424010,478130
std::uint32_t secondary_flags(EnemyState& s){return get<std::uint32_t>(context(s).objects_04[3],0x78);}
sprite::Vec3 position(const EnemyState& s){sprite::Vec3 result;std::memcpy(&result,s.motion_110.words,12);return result;}
sprite::Animation* resolve(EnemyState& s,EnemyDamageServices& host){auto& handle=s.animations.at(0).handle;auto* result=host.animation(handle);if(!result)handle=0;return result;}
void flash(sprite::Animation& animation,std::uint32_t color){set_enemy_hit_color(animation,color);auto flags=get<std::uint8_t>(&animation,0x4a1);put(&animation,0x4a1,static_cast<std::uint8_t>((flags&0xe3u)|4));}
void clear_flash(sprite::Animation& animation){auto flags=get<std::uint8_t>(&animation,0x4a1);put(&animation,0x4a1,static_cast<std::uint8_t>(flags&0xe3u));}
int phase_script(EnemyState& s,const char* name,EnemyDamageServices& host){
    recovered::timer_set(s.timer_a8,0);host.clear_scripts(s.entity);host.select_script(s.entity,name);
    if(s.timer_a8.flags&6u)throw std::out_of_range("Invalid Enemy script clock mode");
    const auto* rate=host.timer_rate();if(!rate)throw std::logic_error("Enemy script clock is null");return host.run_scripts(s.entity,*rate);
}
int query_damage(EnemyState& s,std::uint32_t& flag,int preview,EnemyDamageServices& host){
    const bool rectangle=(s.fields_2c8[0]&0x1000u)!=0;
    return host.calculate_damage(context(s),position(s),rectangle?&s.bounds_5c:nullptr,rectangle?get<float>(&s,0x38):0.0f,
        rectangle?0.0f:s.bounds_5c.x/2.0f,&flag,&s.vector_6c,preview,enemy_identifier(s.entity));
}
}
std::int32_t enemy_phase_stage(game_session::Session& session) noexcept{return clamp_field(&session.player_table,0x1fc,999);}
std::int32_t enemy_damage_bonus(game_session::Player& player) noexcept{return clamp_field(&player,0xac,100);}
void add_phase_reward(game_session::Player& player,std::int32_t delta) noexcept {put(&player,0xe0,add(get<std::int32_t>(&player,0xe0),delta));clamp_field(&player,0xe0,999999);}
const char* enemy_life_phase(EnemyState& s,EnemyDamageServices& host){
    auto& health=s.auxiliary_18c;health.words[2]=health.words[0];health.words[4]=0;
    for(auto& record:s.auxiliary){
        const auto threshold=recovered::signed_bits(record.values[0]);if(threshold<0)continue;
        health.words[2]=health.words[0]-record.values[0];health.words[4]=record.values[0];
        if(threshold<recovered::signed_bits(health.words[0]))return nullptr;
        if(s.fields_250[13]&&recovered::signed_bits(s.fields_250[12])==enemy_phase_stage(host.session())){
            add_phase_reward(*context(s).current_player,recovered::signed_bits(s.fields_250[13]));s.fields_250[13]=0;
        }
        health.words[0]=record.values[0];record.values[0]=0xffffffff;recovered::timer_set(s.timer_a8,0);s.fields_2c8[1]&=~0x100u;
        return reinterpret_cast<const char*>(record.values.data()+2);
    }
    return nullptr;
}
const char* enemy_time_phase(EnemyState& s,EnemyDamageServices& host){
    for(auto& record:s.auxiliary){
        const auto threshold=recovered::signed_bits(record.values[0]),duration=recovered::signed_bits(record.values[1]);
        if(threshold<0||duration<=0)continue;
        if(s.fields_2c8[1]&0x80u){const auto remaining=subtract(duration,s.timer_a8.current);store_boss_time(host.boss_hud(),remaining/60,multiply(remaining%60,100)/60);}
        if(s.timer_a8.current<duration)return nullptr;
        s.auxiliary_18c.words[0]=record.values[0];record.values[0]=0xffffffff;recovered::timer_set(s.timer_a8,0);s.fields_2c8[1]|=0x100u;
        auto* secondary=context(s).objects_04[3];
        if(!(secondary_flags(s)&8u)){
            put(secondary,0x78,secondary_flags(s)|0x80u);host.notify_timeout(secondary);
            put(context(s).objects_04[1],0x50,std::uint32_t{0});
        }else if(secondary_flags(s)&1u){
            s.fields_2c8[1]&=~0x100u;add_phase_reward(*context(s).current_player,recovered::signed_bits(s.fields_250[13]));
        }
        s.fields_250[13]=0;return reinterpret_cast<const char*>(record.values.data()+18);
    }
    return nullptr;
}
int update_enemy_damage(EnemyState& s,EnemyDamageServices& host){
    if(!resolve(s,host))return 0;
    auto& primary=s.fields_2c8[0];auto& flags=s.fields_2c8[1];s.fields_2c8[2]&=~0x10u;
    const auto change_bomb_animation=[&](bool active){
        if(get<std::int32_t>(&s,0x54)>=0){put(&s,0x28,get<std::uint32_t>(&s,active?0x54:0x58));host.replace_animation(s.animations.at(0).handle,get<std::int32_t>(&s,0x28));}
        if(active){flags|=0x2000u;primary|=1u;}else{flags&=~0x2000u;primary&=~1u;}
    };
    if(!(flags&0x1000u)){if(!bomb_active(s)&&(flags&0x2000u))change_bomb_animation(false);}
    else if(bomb_active(s)&&!(flags&0x2000u))change_bomb_animation(true);
    else if(!bomb_active(s)&&(flags&0x2000u))change_bomb_animation(false);
    if(primary&0x800u){
        std::uint32_t flag=0;flags&=~0x2000000u;const int damage=query_damage(s,flag,1,host);
        if(damage&&flag){const int result=host.defeat(s.entity);if(result)return result;}
    }
    if(const auto* phase=enemy_time_phase(s,host)){const int result=phase_script(s,phase,host);if(result)return result;}
    std::uint32_t hit=0;flags&=~0x20u;
    if(!(primary&0x21u)){
        int amount=0;
        if(s.bounds_5c.x>0.0f){
            amount=query_damage(s,hit,0,host);amount=recovered::truncate32(recovered::mul32(recovered::int_float(amount),primary_entity_scale(context(s).objects_04[0])));
            if(flags&0x40000000u){
                auto& global=host.session().contexts[0];const int bonus=enemy_damage_bonus(*global.current_player);
                if(bonus>0&&amount>0){amount=add(amount,multiply(enemy_damage_bonus(*global.current_player),amount)/100);auto* feedback=get<void*>(global.overlay_owner,0x34);put(feedback,0x35,std::uint8_t{1});}
            }
        }
        if(s.fields_2c8[5]){using Callback=int(__thiscall*)(EnemyState*,int);const int extra=reinterpret_cast<Callback>(s.fields_2c8[5])(&s,amount);amount=add(amount,extra);}
        if(get<std::int32_t>(&s,0x48)>0){amount=add(amount,get<std::int32_t>(&s,0x48));put(&s,0x48,std::uint32_t{0});}
        auto* player=context(s).objects_04[0];const int state=get<std::int32_t>(player,0x10);if(state==2||state==0)amount/=5;
        if(boss_damage_suppressed(host.boss_hud()))amount=0;
        if(amount>0){
            auto* owner=static_cast<EnemyController*>(context(s).objects_04[1]);
            if(!hit)owner->data.field_a0+=static_cast<std::uint32_t>(amount);
            else{const int current=recovered::signed_bits(s.auxiliary_18c.words[0]);const int credit=amount<current?amount:add(current,subtract(amount,current)/4);owner->data.field_9c+=static_cast<std::uint32_t>(credit);}
        }
        const auto bomb_scale=get<float>(&s,0x4c);
        if(bomb_active(s)&&bomb_scale<1.0f){
            if(amount&&bomb_scale<=0.0f)host.sound(0x24,position(s).x);
            amount=recovered::truncate32(recovered::mul32(recovered::int_float(amount),bomb_scale));
        }
        if(amount){
            auto* feedback=static_cast<std::uint8_t*>(host.session().contexts[0].objects_04[0])+0x2244;
            extend_enemy_hit_feedback(feedback,recovered::signed_bits(s.fields_250[6]),5,host.timer_rate());
            if((secondary_flags(s)&0x21u)==0x21u)amount/=30;
            if(!(primary&0x10u)&&s.timer_288.current<=0)apply_enemy_damage(s.auxiliary_18c,amount);else record_enemy_damage(s.auxiliary_18c,amount);
            recovered::timer_set(s.timer_2a8,30);
            if(const auto* phase=enemy_life_phase(s,host)){
                //40e5e0 reached when hit!=0 is an actual no-effect release-build body.
                const int result=phase_script(s,phase,host);if(result)return result;
            }
            if(!(primary&0x80u)&&!enemy_health_positive(s.auxiliary_18c)){const int result=host.defeat(s.entity);if(result)return result;}
            flags|=0x20u;
        }
    }
    if(enemy_health_forced_end(s.auxiliary_18c)){const int result=host.defeat(s.entity);if(result)return result;}
    if(!(primary&0x22u)&&s.timer_298.current<=0&&!(flags&0x400u)&&
       (static_cast<EnemyController*>(context(s).objects_04[1])->field_c8==0||(flags&0x80u))){
        if(s.fields_2c8[7]){using Callback=void(__thiscall*)(EnemyState*);reinterpret_cast<Callback>(s.fields_2c8[7])(&s);}
        else{
            auto* player=context(s).objects_04[0];int result;
            if(!(primary&0x1000u))result=host.player_circle(player,position(s),s.bounds_64.x/2.0f,0);
            else{
                sprite::Vec3 offset{};
                if(auto* animation=resolve(s,host)){
                    const auto angle=ecl::math::wrap_angle(recovered::add32(animation->base.vector_38.z,0x1.921fb6p+1f/2.0f));
                    ecl::math::rotate(offset.x,offset.y,angle);
                }
                const auto origin=position(s);offset.x=recovered::add32(offset.x,origin.x);offset.y=recovered::add32(offset.y,origin.y);offset.z=recovered::add32(offset.z,origin.z);
                result=host.player_rectangle(player,offset,get<float>(&s,0x38),s.bounds_64.y,s.bounds_64.x,0);
            }
            if((primary&0x200u)&&result==2&&s.timer_a8.current%6==0)host.player_graze(player,get<sprite::Vec3>(player,0x614),0);
        }
    }
    if(auto* animation=resolve(s,host)){
        auto& cooldown=s.fields_250[3];
        if(cooldown){clear_flash(*animation);--cooldown;}
        else{
            if(flags&0x8000u){if(s.timer_a8.current%4==0)flash(*animation,0xffff00ffu);else clear_flash(*animation);}
            if(!(flags&0x20u)||(primary&0x2000u)){
                if(s.timer_a8.current%4!=0)clear_flash(*animation);
                else if(flags&0x4080u){
                    const auto secondary=secondary_flags(s);const int phase_health=recovered::signed_bits(s.auxiliary_18c.words[2]);
                    if(!((secondary&1u)&&(secondary&8u))&&((secondary&1u)?phase_health<100:phase_health<500))flash(*animation,0xff0000ffu);
                }
            }else{
                flash(*animation,0xff0000ffu);cooldown=4;
                int effect=recovered::signed_bits(s.fields_250[5]);
                if(effect<0){effect=0x22;if(flags&0x4080u){const auto secondary=secondary_flags(s);const int phase_health=recovered::signed_bits(s.auxiliary_18c.words[2]);
                    if(!((secondary&1u)&&(secondary&8u))&&((secondary&1u)?phase_health<200:phase_health<900))effect=0x23;}}
                host.sound(effect,position(s).x);
            }
        }
    }
    if(s.timer_2a8.current>0)recovered::timer_add(s.timer_2a8,-1.0f,host.timer_rate());
    return 0;
}
}
