#include <eagler/gameplay/CooperativeRules.hpp>
#include "ItemOwnership.hpp"
#include <limits>

namespace th10::multiplayer {
int select_item_collector(ItemOwnership& claim,Item& item,const ItemCandidate* pilots,u32 count,float rate) noexcept {
    if(count<2||count>3)return -1;
    const bool power=item.kind==1||item.kind==4||item.kind==10||item.kind==11;
    const auto nearest_power=[&](bool only_needy){
        return eagler::cooperative::NearestPowerRecipient(count,
            [&](unsigned s){return pilots[s].eligible&&(!only_needy||pilots[s].power<100);},
            [&](unsigned s){return pilots[s].power<100;},
            [&](unsigned s){const auto& p=pilots[s].position;
                const double dx=double(p.x)-item.position.x,dy=double(p.y)-item.position.y;return dx*dx+dy*dy;});
    };
    const auto overflow=[&](int seat){
        if(seat<0||pilots[seat].power<100||!power)return seat;
        const int target=nearest_power(true);
        if(target>=0){claim.recipient=-1;claim.homing=std::int8_t(target);item.state=3;item.attraction_speed=8;return target;}
        return seat;
    };
    if(claim.recipient>=0){
        const auto seat=u32(claim.recipient);
        return seat<count&&pilots[seat].eligible?overflow(int(seat)):-1;
    }
    if(claim.homing>=0){
        const auto seat=u32(claim.homing);
        if(seat<count&&pilots[seat].eligible)return overflow(int(seat));
        claim.homing=-1;
    }
    int selected=-1,selected_priority=-1;
    double nearest=std::numeric_limits<double>::infinity();
    for(u32 seat=0;seat<count;++seat){
        const auto& p=pilots[seat];if(!p.eligible)continue;
        Vec3 contact=item.position;
        if(item.state==2||(item.state==1&&!p.auto_collect&&p.position.y>=128)){
            contact.x=(number(contact.x)+number(rate)*number(item.velocity.x)).to_float();
            contact.y=(number(contact.y)+number(rate)*number(item.velocity.y)).to_float();
            contact.z=Scalar::add(contact.z,Scalar::mul(rate,item.velocity.z));
        }
        const int priority=p.pickup.contains(contact)?3:
            p.auto_collect||p.position.y<128?2:
            (p.focused?p.slow:p.fast).contains(contact)?1:0;
        const double dx=double(p.position.x)-item.position.x,dy=double(p.position.y)-item.position.y;
        const double distance=dx*dx+dy*dy;
        if(priority>selected_priority||(priority==selected_priority&&distance<nearest)){
            selected=int(seat);selected_priority=priority;nearest=distance;
        }
    }
    if(power&&selected_priority==2){
        selected=nearest_power(false);
        if(selected>=0){claim.homing=std::int8_t(selected);item.state=3;item.attraction_speed=8;}
        return selected;
    }
    return selected_priority>0?overflow(selected):selected;
}
}
