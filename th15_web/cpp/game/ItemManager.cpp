#include "ItemManager.hpp"
#include <cmath>
namespace th15 {
ItemManager::ItemManager(AnmManager& a,EffectManager& e,i32 id):animations(a),effects(e),resource(id){for(u32 i=0;i<pool_size;i++)free_slots[i>=ordinary_count].push_back(i);}
ItemManager::~ItemManager(){for(auto& slot:slots){if(slot.body)animations.registry.destroy_tree(*slot.body);if(slot.arrow)animations.registry.destroy_tree(*slot.arrow);}}
bool ItemManager::reset(){
    for(auto& slot:slots){
        if((slot.body&&!animations.registry.destroy_tree(*slot.body))||(slot.arrow&&!animations.registry.destroy_tree(*slot.arrow))){error=animations.registry.error;return false;}
        slot.state={};slot.body.reset();slot.arrow.reset();slot.arrow_ended=false;
    }
    for(auto& free:free_slots)free.clear();for(u32 i=0;i<pool_size;i++)free_slots[i>=ordinary_count].push_back(i);
    // The original clears the request/stamp counters and motion scale here.
    // Density, last active count and the run-owned piece cycle survive.
    requests=cancel_stamp=0;motion_scale=1;return true;
}
bool ItemManager::appearance(Slot& slot,i32 kind){
    constexpr i32 scripts[][2]={{-1,-1},{112,126},{113,127},{114,128},{115,129},{116,130},{117,131},{118,132},{119,133},{120,-1},{121,-1},{122,-1},{-1,-1},{123,-1},{124,-1},{125,-1}};
    if(kind<1||kind>15||scripts[kind][0]<0){error="Item animation kind unavailable";return false;}if(!slot.body)slot.body=std::make_unique<AnmVm>();
    if(!animations.bind_template(*slot.body,resource,scripts[kind][0])||animations.tick_instance(*slot.body)<0){error=animations.error;return false;}
    if(scripts[kind][1]>=0){slot.arrow_ended=false;if(!slot.arrow)slot.arrow=std::make_unique<AnmVm>();if(!animations.bind_template(*slot.arrow,resource,scripts[kind][1])||animations.tick_instance(*slot.arrow)<0){error=animations.error;return false;}}return true;
}
bool ItemManager::piece_effect(const ItemState& item){if(item.kind!=4&&item.kind!=5&&item.kind!=6&&item.kind!=7&&item.kind!=12)return true;if(!effects.create(101,item.position)){error=effects.error;return false;}if(!sound||!sound(item.kind==4||item.kind==5?74:48)){error="Item piece sound unavailable";return false;}return true;}
u32 ItemManager::spawn(i32 kind,const Vec3& position,float angle,float speed){
    requests=wrapping_add(requests,1);if(kind<1||kind>15){error="Invalid item kind";return 0;}const bool cancel=kind==9||kind==10||kind==11||kind==13;auto& free=free_slots[cancel];if(free.empty())return 0;const u32 index=free.back();auto& slot=slots[index];auto& state=slot.state;
    state.position=position;state.velocity={float(std::cos(double(angle))*double(speed)),float(std::sin(double(angle))*double(speed)),0};state.reserved=0;state.age.set(0);
    if(cancel){state.stamp=cancel_stamp;const i32 divisor=cancel_density<256?4:cancel_density<512?8:cancel_density<1024?16:32,offset=cancel_density<256?0:cancel_density<512?4:cancel_density<1024?8:16;state.delay=wrapping_add(requests%divisor,offset);state.state=5;state.kind=state.appearance=kind;}
    else{state.state=1;state.position.x=state.position.x<=-192.f?-192.f:state.position.x>=192.f?192.f:state.position.x;if(kind==12){i32& counter=alternating_counter?*alternating_counter:alternating_pieces;counter=wrapping_add(counter,1);alternating_pieces=counter;kind=counter%5==0?4:6;}state.kind=kind;state.appearance=0;state.attraction_speed=0;state.delay=0;if(!piece_effect(state)||!appearance(slot,kind))return 0;slot.body->visual.color=0xffffffff;}
    free.pop_back();return index+1;
}
bool ItemManager::release(u32 id){auto* slot=find(id);if(!slot){error="Item slot unavailable";return false;}const u32 index=id-1;if(slot->state.state==0){error="Item slot already free";return false;}slot->state.state=0;free_slots[index>=ordinary_count].push_back(index);return true;}
bool ItemManager::falling(u32 id){auto* slot=find(id);if(!slot){error="Item slot unavailable";return false;}slot->state.state=2;if(!slot->body)slot->body=std::make_unique<AnmVm>();constexpr i32 scripts[]={-1,112,113,114,115,116,117,118,119,120,121,122,-1,123,124,125};const i32 kind=slot->state.kind;if(kind<1||kind>15||scripts[kind]<0){error="Falling item animation unavailable";return false;}if(!animations.bind_template(*slot->body,resource,scripts[kind])||animations.tick_instance(*slot->body)<0){error=animations.error;return false;}slot->arrow_ended=true;if(slot->arrow){slot->arrow->visual.flags&=~1u;slot->arrow->instruction_offset=-1;}return true;}
}
