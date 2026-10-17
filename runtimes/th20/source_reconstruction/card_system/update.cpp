#include "card.hpp"
#include "data_constants.hpp"
#include "../program_entry/program_entry.hpp"
#include "../stage_background/background.hpp"
#include "../player_entity/player.hpp"
#include "../bomb_system/bomb.hpp"
#include "../gameplay/enemy_variables.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../runtime_state/state.hpp"
#include <cstring>
#include <stdexcept>
namespace th20::source::card {
namespace n=recovered;
int update(CardInf& o){
    if(!(o.flags&1u))return 1;
    ++o.frames;
    if(o.age.current>=60&&!(o.flags&0x200u))background::primary->state_flags&=~1u;
    if(o.age.current>=300&&!(o.flags&8u)){
        const int numerator=n::signed_bits(std::uint32_t(o.initial_bonus)-std::uint32_t(o.initial_bonus/3));
        const int denominator=n::signed_bits(std::uint32_t(o.duration)-300u);
        if(!denominator||(numerator==INT32_MIN&&denominator==-1))throw std::domain_error("Card bonus original IDIV trap");
        o.bonus=n::signed_bits(std::uint32_t(o.bonus)-std::uint32_t(numerator/denominator));o.bonus-=o.bonus%10;
    }
    n::timer_tick(o.age,state::timer_rate);
    auto& sprites=*program_entry::sprite_controller;
    if(o.age.current>=120){
        const auto y=player_entity::position(o.context->objects_04[0]).y;
        const bool reversed=(o.flags&0x100u)!=0;
        if(o.flags&4u){
            if((!reversed&&y>data::f_0056cd98)||(reversed&&y<n::add32(data::f_0056d7c0,-data::f_0056cd98))){for(auto h:o.info_handles)sprite::interrupt_animation_children(sprites,h,2);o.flags&=~4u;}
        }else if((!reversed&&y<data::f_0056fe7c)||(reversed&&y>n::add32(data::f_0056d7c0,-data::f_0056fe7c))){for(auto h:o.info_handles)sprite::interrupt_animation_children(sprites,h,3);o.flags|=4u;}
    }
    auto* enemy=gameplay::selected_enemy(static_cast<gameplay::EnemyController*>(o.context->objects_04[1]),0);
    if(enemy){const auto target=gameplay::enemy_position(enemy);
        o.position.x=n::add32(o.position.x,n::mul32(n::add32(target.x,-o.position.x),data::f_0056fe5c));
        o.position.y=n::add32(o.position.y,n::mul32(n::add32(target.y,-o.position.y),data::f_0056fe5c));
        o.position.z=n::add32(o.position.z,n::mul32(n::add32(target.z,-o.position.z),data::f_0056fe5c));
    }
    if(auto* a=sprite::find_animation(sprites,o.effect_handle))a->vector_5bc=o.position;
    if((o.flags&0x20u)&&!static_cast<bomb::Controller*>(o.context->objects_04[5])->active())o.flags&=~0x20u;
    return 1;
}
}
