#include "state_layout.hpp"
#include <cstring>
namespace th20::source::player_entity {
void construct_option(Option& option) noexcept{std::memset(&option,0,sizeof(option));}
void construct_shot(Shot& shot) noexcept{
    std::memset(&shot,0,0x115);std::memset(reinterpret_cast<std::uint8_t*>(&shot)+0x118,0,sizeof(shot)-0x118);
    shot.fields_98[3]=1;
}
void construct_shot_controller(ShotController& owner) noexcept{
    for(auto& shot:owner.pool)construct_shot(shot);
    owner.timer_12400=owner.timer_12410=owner.timer_12420={};
    owner.active.sentinel={};owner.active.tail=&owner.active.sentinel;owner.free.sentinel={};owner.free.tail=&owner.free.sentinel;
    owner.field_12460=owner.field_12464=0;for(auto& counter:owner.counters_12468)counter=0;for(auto& counter:owner.counters_124e0)counter=0;
    owner.field_12558=owner.field_1255c=owner.handle_12560=owner.field_12564=0;owner.timer_12568={};owner.field_12578=0;owner.byte_1257c=0;
    owner.timer_12580={};owner.view_index=0;owner.context=nullptr;
}
void construct_feedback(Feedback& feedback) noexcept{
    std::memset(&feedback,0,0x51);std::memset(reinterpret_cast<std::uint8_t*>(&feedback)+0x54,0,sizeof(feedback)-0x54);
}
}
