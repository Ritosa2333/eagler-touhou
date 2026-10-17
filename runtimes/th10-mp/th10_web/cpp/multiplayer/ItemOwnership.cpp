#include "ItemOwnership.hpp"
#include <limits>

namespace th10::multiplayer {
int select_item_collector(ItemOwnership& claim,const Item& item,const ItemCandidate* pilots,u32 count) noexcept {
    if(count<2||count>3)return -1;
    if(claim.recipient>=0){
        const auto seat=u32(claim.recipient);
        return seat<count&&pilots[seat].eligible?int(seat):-1;
    }
    if(claim.homing>=0){
        const auto seat=u32(claim.homing);
        if(seat<count&&pilots[seat].eligible)return int(seat);
        claim.homing=-1;
    }
    int selected=-1,selected_priority=-1;
    double nearest=std::numeric_limits<double>::infinity();
    for(u32 seat=0;seat<count;++seat){
        const auto& p=pilots[seat];if(!p.eligible)continue;
        const int priority=p.pickup.contains(item.position)?3:
            p.auto_collect||p.position.y<128?2:
            (p.focused?p.slow:p.fast).contains(item.position)?1:0;
        const double dx=double(p.position.x)-item.position.x,dy=double(p.position.y)-item.position.y;
        const double distance=dx*dx+dy*dy;
        if(priority>selected_priority||(priority==selected_priority&&distance<nearest)){
            selected=int(seat);selected_priority=priority;nearest=distance;
        }
    }
    return selected;
}
}
