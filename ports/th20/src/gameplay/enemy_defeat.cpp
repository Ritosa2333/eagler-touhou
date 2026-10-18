#include "enemy_defeat.hpp"
#include "enemy_variables.hpp"
#include "player_state.hpp"
#include "../ecl_vm/math.hpp"
#include <bit>
namespace th20::source::gameplay {
void add_defeat_combo(player_entity::Feedback& f,int time,const float* rate){
    recovered::timer_add(f.timers[0],recovered::int_float(time),rate);if(f.timers[0].current>30)recovered::timer_set(f.timers[0],30);
    ++f.fields_30[2];f.fields_30[0]=f.fields_30[2];if(recovered::signed_bits(f.fields_30[3])<recovered::signed_bits(f.fields_30[0]))f.fields_30[3]=f.fields_30[0];
}
int defeat_enemy(Enemy& enemy,EnemyDefeatServices& env){
    for(scheduler::Iterator it(enemy.children.sentinel.next);it.current;it.advance()){auto& child=*reinterpret_cast<Enemy*>(it.current->value);defeat_enemy(child,env);child.state.fields_2c8[1]|=0x200u;}
    scheduler::unlink(enemy.parent_link);auto& s=enemy.state;auto& c=*enemy.context;const auto position=enemy_position(&enemy);
    if(recovered::signed_bits(s.fields_250[0])>=0)env.sound(recovered::signed_bits(s.fields_250[0]),position.x);
    const auto& before=s.vector_6c;const float dx=before.x-position.x,dy=before.y-position.y;const float angle=.2f*.2f<=dx*dx+dy*dy?ecl::math::arctangent(position.y-before.y,position.x-before.x):-3.1415927410125732f/2.f;
    if(recovered::signed_bits(s.fields_250[1])>=0)env.effect(enemy,s.fields_250[2],recovered::signed_bits(s.fields_250[1]),position,angle);
    auto& owner=*static_cast<EnemyController*>(c.objects_04[1]);owner.field_128=std::bit_cast<unsigned>(ecl::math::wrap_angle(angle));
    env.drop(s.pattern_1a8,position,((s.fields_2c8[2]>>5)&1u)!=0);
    if(s.fields_2c8[2]&16u){
        auto& global=env.session().contexts[0];auto& feedback=*reinterpret_cast<player_entity::Feedback*>(static_cast<std::uint8_t*>(global.objects_04[0])+0x2244);
        add_defeat_combo(feedback,recovered::signed_bits(s.fields_250[7]),env.timer_rate());
        constexpr int multipliers[]{100,150,200,250,300,400,500,600,700,800};const int count=recovered::signed_bits(feedback.fields_30[2]);if(count<0)throw std::out_of_range("Original defeat combo indexes before multiplier table");const int multiplier=multipliers[count<10?count:9];const int amount=recovered::signed_bits(s.fields_250[10]*unsigned(multiplier))/100;
        auto& player=*global.current_player;env.special_items(player,amount);if(!env.special_active())env.meter(player,amount);
        auto color=env.random()%5;env.reward(global.overlay_owner,position,2000,int(color+9));
        if(!env.special_active()){if(color==4)color=env.random()%8;else{constexpr unsigned offsets[]{0xc,0x14,0x10,0x18};color=player_state::read<unsigned>(player,offsets[color]);}env.counter(player,color,amount);}
    }
    if(s.fields_250[11]!=0xffffffffu){env.death_script(enemy,recovered::signed_bits(s.fields_250[11]));s.fields_250[11]=0xffffffffu;if(s.fields_2c8[1]&0x08000000u)return 0;}
    if(enemy.callback)enemy.callback(&enemy);return 1;
}
}
