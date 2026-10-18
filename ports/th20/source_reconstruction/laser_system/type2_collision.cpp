#include "type2.hpp"
#include "type1.hpp"
#include "../player_entity/player.hpp"
#include "../player_entity/events.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::laser {
int Type2Laser::cancel_all(int preview){
    auto* first=static_cast<Segment*>(allocated_6d8);if(!(flags&0x40))compute_segments(nullptr);if(!field_6e0)return 0;
    bool grazed=false;auto* selected=first;sprite::Vec3 player_position{};auto* player=context->objects_04[0];
    for(std::int32_t i=0;i<recovered::signed_bits(field_6e0);++i){const auto& s=first[i];const int collision=player_entity::collide_rectangle(player,s.position,s.angle,s.size.y,s.size.x,preview);
        if(collision==1)cancel_rectangle(player_entity::position(player),sprite::Vec3{32,32,0},0,0,1);
        else if(collision!=2||grazed)field_6d4=0;
        else{grazed=true;selected=first+i;player_position=player_entity::position(player);}
    }
    if(grazed&&timer_38.current%8==0){sprite::Vec3 projected{};project_to_segment(selected->position,selected->size.x,selected->angle,player_position,projected);player_entity::graze(player,projected,field_6d0);++field_6d4;}
    recovered::timer_tick(timer_38,state::timer_rate);return 0;
}
}
