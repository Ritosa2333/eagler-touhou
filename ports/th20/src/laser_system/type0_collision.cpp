#include "type0.hpp"
#include "type1.hpp"
#include "../player_entity/player.hpp"
#include "../player_entity/events.hpp"
#include "../runtime_state/state.hpp"
#include <algorithm>
namespace th20::source::laser {
int Type0Laser::cancel_all(int preview){
    auto& segment=*static_cast<Segment*>(allocated_6d8);animation.base.flags[2]&=~0x1c00u;
    const bool valid=(flags&0x40)?bool(segment.flags&8):collision_segment(nullptr)!=0;if(!valid)return 0;field_84=0x3f800000u;
    auto* player=context->objects_04[0];const int collision=player_entity::collide_rectangle(player,segment.position,segment.angle,segment.size.y,segment.size.x,preview);
    if(collision==1)cancel_rectangle(player_entity::position(player),sprite::Vec3{32,32,0},0,0,1);
    else if(collision==2){
        ++field_6d4;if(timer_38.current%8==0){sprite::Vec3 projected{};project_to_segment(segment.position,segment.size.x,segment.angle,player_entity::position(player),projected);player_entity::graze(player,projected,field_6d0);}
        if(!(active_commands&(std::uint64_t{1}<<33))){const auto color=std::clamp(recovered::signed_bits(208u-field_6d4*2u),96,240);animation.base.flags[2]=(animation.base.flags[2]&~0x1c00u)|0x400u;animation.base.field_494=0xffff0080u|(std::uint32_t(color)<<8);}
        recovered::timer_tick(timer_38,state::timer_rate);
    }else field_6d4=0;return 0;
}
}
