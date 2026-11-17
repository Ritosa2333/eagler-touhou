#include "ItemManager.hpp"
#include <cmath>
namespace th17 {
namespace {
bool contains(const PlayerBox& b,const Vec3& p)noexcept{return float(p.x+0.f)>=b.minimum.x&&float(p.y+0.f)>=b.minimum.y&&float(p.x-0.f)<=b.maximum.x&&float(p.y-0.f)<=b.maximum.y;}
void translate(ItemState& s,float rate,float scale=1)noexcept{s.position.x=float(s.position.x+float(float(s.velocity.x*rate)*scale));s.position.y=float(s.position.y+float(float(s.velocity.y*rate)*scale));s.position.z=float(s.position.z+float(float(s.velocity.z*rate)*scale));}
void approach(ItemState& s,const Vec3& player,float rate)noexcept{const float dy=float(player.y-s.position.y),dx=float(player.x-s.position.x),angle=dy==0.f&&dx==0.f?1.5707963705062866f:float(std::atan2(double(dy),double(dx)));s.velocity={float(std::cos(double(angle))*double(s.attraction_speed)),float(std::sin(double(angle))*double(s.attraction_speed)),0};translate(s,rate);if(s.attraction_speed<12.f)s.attraction_speed=float(s.attraction_speed+.2f);}
bool collectible(const ItemFrameContext& c){return c.active&&c.player_state!=2&&c.player_state!=4;}
float distance_squared(const ItemFrameContext& c,const ItemState& item){
    const float x=c.player.position.x-item.position.x,y=c.player.position.y-item.position.y;
    return x*x+y*y;
}
bool auto_collect(const ItemFrameContext& c){
    return (c.player_state!=2&&c.player_state!=4&&c.player.position.y<float(c.character==1?148:128))||
        (c.bomb_state==1&&c.bomb_frame<60)||c.hud_collect;
}
ItemFrameContext* power_recipient(ItemFrameContext& collector,ItemFrameContext& primary,
                                 const std::array<ItemFrameContext*,2>& partners,const ItemState& item){
    if((item.kind!=1&&item.kind!=3&&item.kind!=8)||collector.collection.needs_power())return nullptr;
    ItemFrameContext* recipient=nullptr;
    float nearest=0;
    for(auto* lane:{&primary,partners[0],partners[1]}){
        if(!lane||!collectible(*lane)||!lane->collection.needs_power())continue;
        const float distance=distance_squared(*lane,item);
        if(!recipient||distance<nearest){recipient=lane;nearest=distance;}
    }
    return recipient;
}
void forward_power(ItemState& item){
    if(item.state!=3)item.attraction_speed=8;
    item.state=3;
}
ItemFrameContext& select_player(ItemFrameContext& main,const std::array<ItemFrameContext*,2>& partners,ItemState& item){
    if(!partners[0]&&!partners[1])return main;
    auto* selected=&main;
    i32 priority=0;
    float nearest=0;
    for(auto* lane:{&main,partners[0],partners[1]}){
        if(!lane||!collectible(*lane))continue;
        const auto rank=contains(lane->bounds.item_near,item.position)?4:
            auto_collect(*lane)?3:contains(lane->held&8?lane->bounds.graze:lane->bounds.item_full,item.position)?2:1;
        const float distance=distance_squared(*lane,item);
        if(rank>priority||(rank==priority&&distance<nearest)){
            selected=lane;
            priority=rank;
            nearest=distance;
        }
    }
    const bool attracted=item.state==3||item.state==4;
    const bool reaches_collector=item.state==1&&item.delay<=0&&priority>=2;
    if(priority&&(attracted||reaches_collector)){
        if(auto* recipient=power_recipient(*selected,main,partners,item)){
            forward_power(item);
            return *recipient;
        }
    }
    return *selected;
}
}
bool ItemManager::update(ItemFrameContext& primary,const std::array<ItemFrameContext*,2>& partners){
    error.clear();if(primary.character<0||primary.character>2){error="Invalid item frame character";return false;}animations.rate=primary.rate;cancel_density=0;active_count=0;
    const i32 seats=1+(partners[0]!=nullptr)+(partners[1]!=nullptr);
    for(u32 index=next_used(0);index<pool_size;index=next_used(index+1)){
        auto& slot=slots[index];auto& s=slot.state;if(!s.state)continue;
        if(s.state==5){s.delay=wrapping_sub(s.delay,1);if(s.delay<0&&!falling(index+1))return false;continue;}
        const auto seat=s.gift_seat();
        auto* receiver=seat?std::array{&primary,partners[0],partners[1]}[seat-1]:nullptr;
        if(seat&&(!receiver||!receiver->active)){
            s.gift_seat(0);s.state=1;s.velocity={};
            receiver=nullptr;
        }
        auto& c=receiver?*receiver:select_player(primary,partners,s);
        if(s.state==1){
            if(s.delay>0){s.delay=wrapping_sub(s.delay,1);if(s.delay<1&&!birth_effect(s))return false;continue;}
            if(auto_collect(c)){s.state=3;s.attraction_speed=c.attraction_speed;}
            else{translate(s,c.rate,motion_scale);s.velocity.y=float(s.velocity.y+float(float(c.rate*.03f)*motion_scale));
                if(s.age.current>31){if(!slot.body){error="Active item animation unavailable";return false;}auto& body=*slot.body;if(motion_scale>=1.f)body.visual.color=0xffffffff;else{const float y=c.visual_random.signed_unit(),x=c.visual_random.signed_unit();body.variables.position={x,y,0};body.visual.color=0xffffa0ff;}}
                if(s.velocity.y>=0)s.velocity.x=0;if(s.velocity.y>2)s.velocity.y=2;
                if(s.position.y>472.f||std::fabs(s.position.x)>=200.f){if(!release(index+1))return false;continue;}}
        }else if(s.state==2){translate(s,c.rate);s.velocity.y=float(s.velocity.y+float(c.rate*.03f));if(s.velocity.y>=0){s.state=3;s.attraction_speed=c.attraction_speed;}else if(s.position.y>472.f||std::fabs(s.position.x)>=200.f){if(!release(index+1))return false;continue;}}
        else if(s.state==4){if(auto_collect(c)){s.state=3;s.attraction_speed=c.attraction_speed;}else{approach(s,c.player.position,c.rate);if(c.player_state==4){s.velocity.x=s.velocity.y=0;s.state=1;}}}
        if(s.state==3){if(!receiver||collectible(c))approach(s,c.player.position,c.rate);if(!receiver&&c.player_state==4){s.velocity.x=s.velocity.y=0;s.state=1;}}
        if(receiver?collectible(c):c.active&&c.player_state!=2){
            if(receiver?distance_squared(c,s)<=64:contains(c.bounds.item_near,s.position)){
                if(!receiver&&seats>1&&power_recipient(c,primary,partners,s)){
                    forward_power(s);
                }else{
                    if(seats>1&&(s.kind==4||s.kind==6)){
                        for(auto* lane:{&primary,partners[0],partners[1]}){
                            if(lane&&!lane->collection.award(s)){error=lane->collection.error;return false;}
                        }
                    }else if(!c.collection.award(s,seats)){error=c.collection.error;return false;}
                    if(!immediate_sound||!immediate_sound(37)){error="Item pickup sound unavailable";return false;}
                    if(!release(index+1))return false;
                    continue;
                }
            }
            if(s.state!=3&&s.state!=4&&(s.kind<9||s.kind>14)&&contains(c.held&8?c.bounds.graze:c.bounds.item_full,s.position)){s.state=4;s.attraction_speed=float(c.attraction_speed/3.f);}
        }
        for(AnmVm* vm:{slot.body.get(),slot.arrow.get()})if(vm&&vm->visual.visible()&&animations.tick_instance(*vm)<0){error=animations.error;return false;}
        s.age.tick(&c.rate);active_count=wrapping_add(active_count,1);
    }
    if(motion_scale<1.f)motion_scale=float(motion_scale+.1f);return true;
}
}
