#include "collect.hpp"
namespace th20::source::item {
void activate_special_item(Item& item,Environment& host){
    item.state=2;
    if(item.type==13){
        if(state::next(state::random_streams[0])%8<5){const auto choice=state::next(state::random_streams[0])%4;constexpr unsigned indices[]{3,5,4,6};item.type=static_cast<int>(game_session::context(0).current_player->fields_00[indices[choice]])/2+9;}
        else item.type=state::next(state::random_streams[0])%4+9;
    }
    if(item.type>=9&&item.type<=12)host.bind_special_animation(item.animation,item.type+28);
    item.animation.vector_5bc=item.position;item.secondary_animation.base.fields_10_28[6]=0xffffffffu;item.secondary_animation.base.flags[0]&=~0x10000u;
}
}
