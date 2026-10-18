#include "dialogue.hpp"
#include "../gameplay/enemy_entity.hpp"
#include "../gameplay/enemy_damage.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::hud {
void clear_dialogue_enemies(){
    auto& c=gameplay::enemy_controller();
    for(scheduler::Iterator it(c.enemies.sentinel.next);it.current;it.advance()){
        auto& enemy=*reinterpret_cast<gameplay::Enemy*>(it.current->value);auto& value=enemy.state;const auto first=value.fields_2c8[0],second=value.fields_2c8[1];
        if((!(first&0x4a0u)&&!(second&0xc0u))||(first&0x100u)){
            gameplay::reset_enemy_pattern(value.pattern_1a8);value.vector_6c.x=0;value.vector_6c.y=192;value.fields_250[13]=0; //Enemy+30c
            gameplay::unrecovered::defeat_enemy_004a5640(&enemy);
            for(auto& animation:value.animations)sprite::request_animation_deletion(environment::sprites(),animation.handle);
            value.animations.clear();value.fields_2c8[1]|=0x200u;
        }
    }
    recovered::timer_tick(c.data.timer_8c,state::timer_rate);
}
}
