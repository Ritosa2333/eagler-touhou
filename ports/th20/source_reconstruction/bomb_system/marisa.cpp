#include "marisa.hpp"
#include "character_environment.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../runtime_state/state.hpp"
#include "../gameplay/enemy.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../screen_effect/effect.hpp"
#include "../damage_regions/regions.hpp"
#include "../ecl_vm/math.hpp"
#include <cstring>
#include <new>
namespace th20::source::bomb {
namespace env=character_environment;namespace pe=program_entry;namespace math=ecl::math;
MarisaBomb::MarisaBomb():position{},angle(0),beam_handle(0){}
MarisaBomb* create_marisa_bomb(){auto* memory=::operator new(sizeof(MarisaBomb),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(MarisaBomb));return new(memory)MarisaBomb;}
int __cdecl update_marisa_animation(sprite::Animation* animation){animation->vector_5bc=reinterpret_cast<MarisaBomb*>(animation->field_5c8)->position;return 0;}
int MarisaBomb::start(std::int32_t){
    position=env::player_position();angle=-1.5707963705062866f; //56e0f8 /56c8d0
    pe::thread_registry.request_effect(49,0);
    sprite::spawn_named_animation(*pe::sprite_controller,env::player_animation(),beam_handle,"pl01",51,&position,0,-1,0);
    env::animation_or_fallback(beam_handle).field_5c8=reinterpret_cast<std::uint32_t>(this);
    env::animation_or_fallback(beam_handle).field_5dc=reinterpret_cast<std::uint32_t>(&update_marisa_animation);
    sprite::spawn_named_animation(*pe::sprite_controller,env::player_animation(),handle_74,"pl01",65,&position,0,-1,0);
    env::animation_or_fallback(handle_74).field_5c8=reinterpret_cast<std::uint32_t>(this);
    env::animation_or_fallback(handle_74).field_5dc=reinterpret_cast<std::uint32_t>(&update_marisa_animation);
    notify_bomb_start(game_session::context(0).objects_04[3]);
    env::set_invulnerability(120);add_enemy_bomb_counter(gameplay::enemy_controller(0),1);
    screen::create_effect(8,3,60,240,30,109);env::set_bomb_player_flag(true);return 0;
}
int MarisaBomb::update(){
    auto* beam=sprite::resolve_animation_handle(*pe::sprite_controller,beam_handle);env::set_invulnerability(40);
    if(!beam){env::interrupt(handle_74);return -1;}
    if(timer.current<=300){
        if(timer.current==300){env::interrupt(beam_handle);env::interrupt(handle_74);env::set_movement_scale(1);env::set_bomb_player_flag(false);}
        beam->base.vector_38.z=angle;beam->base.flags[1]|=2u;
        const auto horizontal=env::player_horizontal_motion();constexpr float step=0.0026179938577115536f; // roundedfloat(pi/1200)
        if(horizontal<0)angle=math::wrap_angle(_mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(angle),_mm_set_ss(step))));
        else if(horizontal>0)angle=math::wrap_angle(recovered::add32(angle,step));
        env::set_movement_scale(.5f);position=env::player_position();
        if(timer.current!=timer.previous&&timer.current%3==0){
            constexpr float distance[]{208,240,304},height[]{32,128,256};constexpr int damage[]{50,15,15};
            sprite::Vec3 center{}; // Original reuses z across all three polar calls.
            for(unsigned index=0;index<3;++index){math::polar(center.x,center.y,angle,distance[index]);center={recovered::add32(center.x,position.x),recovered::add32(center.y,position.y),recovered::add32(center.z,position.z)};
                auto handle=damage::create_rectangle(*damage::controller(0),center,512,height[index],angle,0,damage[index]);damage::activate_handle(handle);}
        }
    }
    for(int ordinal=0;;++ordinal){
        auto* current=sprite::resolve_animation_handle(*pe::sprite_controller,beam_handle);auto* child=current?env::animation_child(*current,57,ordinal):nullptr;if(!child)break;
        const sprite::Vec3 size{recovered::mul32(beam->base.vector_50.x,48),recovered::mul32(beam->base.vector_50.y,160),0};const auto center=env::animation_world_position(*child);
        unrecovered::cancel_rectangle_0047cc90(game_session::context(0).primary_owner,center,size,angle,0,0);
        unrecovered::cancel_rectangle_004c9db0(game_session::context(0).objects_04[4],center,size,angle,0,1);
    }
    recovered::timer_tick(timer,state::timer_rate);return 0;
}
int MarisaBomb::finish(){env::interrupt(handle_74);env::interrupt(beam_handle);retire_bomb(this);return 0;}
}
namespace th20::source::bomb::unrecovered {Bomb* create_004783a0(){return create_marisa_bomb();}}
