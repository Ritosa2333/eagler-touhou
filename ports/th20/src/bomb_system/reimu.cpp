#include "reimu.hpp"
#include "character_environment.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../runtime_state/state.hpp"
#include "../gameplay/enemy_variables.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../screen_effect/effect.hpp"
#include "../damage_regions/regions.hpp"
#include "../ecl_vm/math.hpp"
#include <cmath>
#include <cstring>
#include <new>
namespace th20::source::bomb {
namespace env=character_environment;namespace pe=program_entry;namespace math=ecl::math;namespace n=recovered;
namespace {
constexpr float pi=3.1415927410125732f;
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
void place_animation(std::uint32_t handle,const sprite::Vec3& position){if(auto* animation=sprite::find_animation(*pe::sprite_controller,handle))animation->vector_5bc=position;} //4645e0 ->450270, no fallback
}
ReimuBomb::ReimuBomb():orbs{}{}
ReimuBomb* create_reimu_bomb(){auto* memory=::operator new(sizeof(ReimuBomb),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(ReimuBomb));return new(memory)ReimuBomb;}
int ReimuBomb::start(std::int32_t){
    motion.position=env::player_position();pe::thread_registry.request_effect(49,0);
    notify_bomb_start(game_session::context(0).objects_04[3]);add_enemy_bomb_counter(gameplay::enemy_controller(0),1);
    sprite::spawn_named_animation(*pe::sprite_controller,env::player_animation(),handle_74,"pl00",61,&motion.position,0,-1,0);
    n::timer_set(timer,0);return 0;
}
void initialize_reimu_orb(ReimuOrb& orb,std::int32_t ordinal,const sprite::Vec3& center,std::int32_t damage_value){
    orb.motion.vector_38=center;
    // The first ANM/damage position is the existing motion.position. The
    // supplied center is a separate field used by the subsequent orbit step.
    sprite::spawn_named_animation(*pe::sprite_controller,env::player_animation(),orb.animation_handle,"pl00",46,&orb.motion.position,0,-1,0);
    orb.active=1;n::timer_set(orb.timer,0);orb.ordinal=ordinal;
    orb.damage_handle=damage::create_circle(*damage::controller(0),orb.motion.position,56,0,9999,damage_value);
    auto* region=damage::find_handle(orb.damage_handle);region->period=3;damage::activate(*region);
}
void retire_reimu_orb(ReimuOrb& orb){
    if(orb.quiet_retirement){sprite::request_animation_deletion(*pe::sprite_controller,orb.animation_handle);return;}
    if(orb.active){
        pe::thread_registry.request_effect_at(27,orb.motion.position.x);
        unrecovered::cancel_circle_0047cf60(game_session::context(0).primary_owner,orb.motion.position,128,0,99999,0);
        unrecovered::cancel_circle_004caad0(game_session::context(0).objects_04[4],orb.motion.position,128,1,1);
        auto handle=damage::create_circle(*damage::controller(0),orb.motion.position,64,8,11,100);damage::activate_handle(handle);
    }
    env::interrupt(orb.animation_handle);orb.active=0;if(orb.damage_handle)damage::retire_handle(orb.damage_handle);orb.damage_handle=0;
}
void retire_reimu_orbs(ReimuOrb (&orbs)[24]){for(auto& orb:orbs)retire_reimu_orb(orb);}
void update_reimu_orb(ReimuOrb& orb){
    if(orb.timer.current!=orb.timer.previous){
        const auto launch_time=n::signed_bits(static_cast<std::uint32_t>(orb.ordinal)*10u+90u);
        if(orb.timer.current<90){orb.motion.vector_38=env::player_position();orb.motion.field_20=n::add32(orb.motion.field_20,1.5f);orb.motion.angle_1c=math::wrap_angle(n::add32(orb.motion.angle_1c,orb.turn_rate));}
        else if(orb.timer.current<launch_time){orb.motion.vector_38=env::player_position();orb.motion.angle_1c=math::wrap_angle(n::add32(orb.motion.angle_1c,orb.turn_rate));}
        else if(orb.timer.current==launch_time){
            orb.motion.field_44&=~15u;orb.motion.angle_1c=math::wrap_angle(math::arctangent(orb.last_delta.y,orb.last_delta.x));
            orb.motion.field_18=math::square_root(n::add32(n::mul32(orb.last_delta.x,orb.last_delta.x),n::mul32(orb.last_delta.y,orb.last_delta.y)));
        }else{
            auto* enemy=static_cast<gameplay::EnemyController*>(game_session::context(0).objects_04[1]);
            if(enemy)orb.target_identifier=gameplay::nearest_enemy_identifier(*enemy,{orb.motion.position.x,orb.motion.position.y},512);
            if(!orb.target_identifier){if(state::outside_motion_bounds(orb.motion,0,224,320,384))orb.motion.field_18=n::mul32(orb.motion.field_18,.9f);}
            else{
                orb.target=enemy?gameplay::find_enemy_in_list(enemy->enemies,orb.target_identifier):nullptr;
                if(!gameplay::enemy_excluded(orb.target)){
                    const auto target=gameplay::enemy_position(orb.target);
                    const auto bearing=math::arctangent(sub(target.y,orb.motion.position.y),sub(target.x,orb.motion.position.x));
                    const auto difference=math::angle_difference(bearing,orb.motion.angle_1c);auto speed=orb.motion.field_18;
                    if(std::fabs(difference)<div(n::mul32(pi,1),4)){
                        if(std::fabs(difference)<div(pi,12)){speed=n::add32(speed,.2f);if(speed>8)speed=8;}
                    }else{speed=sub(speed,.7f);if(speed<1)speed=1;}
                    orb.motion.angle_1c=math::wrap_angle(n::add32(orb.motion.angle_1c,n::mul32(difference,.1f)));orb.motion.field_18=speed;
                }
            }
        }
    }
    const auto previous=orb.motion.position;state::update_motion(orb.motion,state::clock_scale);place_animation(orb.animation_handle,orb.motion.position);
    orb.last_delta={sub(orb.motion.position.x,previous.x),sub(orb.motion.position.y,previous.y),sub(orb.motion.position.z,previous.z)};n::timer_tick(orb.timer,state::timer_rate);
}
int ReimuBomb::update(){
    env::set_invulnerability(40);place_animation(handle_74,env::player_position());
    if(timer.current>=120){bool any=false;for(auto& orb:orbs)if(sprite::resolve_animation_handle(*pe::sprite_controller,orb.animation_handle)){any=true;break;}
        if(!any){env::interrupt(handle_74);return -1;}}
    if(timer.current==240){retire_reimu_orbs(orbs);env::interrupt(handle_74);}
    else{
        if(timer.current!=timer.previous&&(timer.current==0||timer.current==40)){
            const auto second=timer.current==40;float angle=0;if(second)pe::thread_registry.request_effect_at(44,env::player_position().x);
            for(int index=0;index<8;++index){auto& orb=orbs[(second?8:0)+index];initialize_reimu_orb(orb,index,env::player_position(),second?7:15);
                orb.motion.field_44=(orb.motion.field_44&~15u)|2u;orb.motion.field_20=0;orb.motion.angle_1c=math::wrap_angle(angle);
                orb.motion.field_24=div(second?-pi:pi,64);orb.turn_rate=div(second?-pi:pi,30);angle=math::wrap_angle(n::add32(angle,div(n::mul32(pi,2),8)));}
        }
        for(auto& orb:orbs)if(orb.active){update_reimu_orb(orb);if(damage::find_handle(orb.damage_handle)){
            if(damage::find_handle(orb.damage_handle)->total_damage<300)damage::set_handle_position(orb.damage_handle,orb.motion.position);
            else{retire_reimu_orb(orb);pe::thread_registry.request_effect_at(27,orb.motion.position.x);screen::create_effect(1,4,6,6,0,109);}
        }}
        for(unsigned index=0;index<24;++index){auto& orb=orbs[index];if(orb.active&&timer.current%8==static_cast<int>(index%8)){
            unrecovered::cancel_circle_0047cf60(game_session::context(0).primary_owner,orb.motion.position,64,0,99999,0);
            unrecovered::cancel_circle_004caad0(game_session::context(0).objects_04[4],orb.motion.position,64,0,1);
        }}
    }
    n::timer_tick(timer,state::timer_rate);return 0;
}
int ReimuBomb::finish(){retire_reimu_orbs(orbs);env::interrupt(handle_74);retire_bomb(this);return 0;}
}
namespace th20::source::bomb::unrecovered {Bomb* create_00479140(){return create_reimu_bomb();}}
