#include "ItemManager.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th17 {
namespace {
constexpr i32 scripts[][2]={{-1,-1},{112,134},{113,135},{114,136},{115,137},{116,138},{117,139},{118,140},{119,141},{120,-1},{121,-1},{122,-1},{123,-1},{124,-1},{125,-1}};
}
ItemManager::ItemManager(AnmManager& a,EffectManager& e,i32 id):animations(a),effects(e),resource(id){for(u32 i=0;i<pool_size;i++)free_slots[i>=ordinary_count].push_back(i);}
ItemManager::~ItemManager(){for(auto& slot:slots){if(slot.body)animations.registry.destroy_tree(*slot.body);if(slot.arrow)animations.registry.destroy_tree(*slot.arrow);}}
bool ItemManager::reset(){
    for(auto& slot:slots){if((slot.body&&!animations.registry.destroy_tree(*slot.body))||(slot.arrow&&!animations.registry.destroy_tree(*slot.arrow))){error=animations.registry.error;return false;}slot.state={};slot.body.reset();slot.arrow.reset();slot.arrow_ended=false;}
    for(auto& free:free_slots)free.clear();for(u32 i=0;i<pool_size;i++)free_slots[i>=ordinary_count].push_back(i);
    requests=cancel_stamp=0;motion_scale=1;return true;
}
bool ItemManager::appearance(Slot& slot,i32 kind){
    if(kind<1||kind>14){error="Item animation kind unavailable";return false;}if(!slot.body)slot.body=std::make_unique<AnmVm>();
    if(!animations.bind_template(*slot.body,resource,scripts[kind][0])||animations.tick_instance(*slot.body)<0){error=animations.error;return false;}
    if(!slot.arrow)slot.arrow=std::make_unique<AnmVm>();slot.arrow_ended=false;
    if(!animations.bind_template(*slot.arrow,resource,scripts[kind][1])||animations.tick_instance(*slot.arrow)<0){error=animations.error;return false;}return true;
}
bool ItemManager::birth_effect(const ItemState& item){
    if(item.kind==4||item.kind==5||item.kind==6||item.kind==7||item.kind==15){
        if(!effects.create(101,item.position)){error=effects.error;return false;}
        if(!sound||!sound(item.kind==4||item.kind==5?74:48)){error="Item birth audio unavailable";return false;}
    }else if(item.kind==1||item.kind==2){
        if(!effects.create(101,item.position)){error=effects.error;return false;}
        if(item.sound>=0&&(!sound||!sound(item.sound))){error="Item birth audio unavailable";return false;}
    }return true;
}
u32 ItemManager::spawn(i32 kind,const Vec3& position,float angle,float speed,i32 delay,i32 sound_id,ItemSpawnContext* context){
    error.clear();requests=wrapping_add(requests,1);
    if(kind==33){if(!context){error="Random animal spirit requires the game RNG";return 0;}kind=i32(context->game_random.next32()%3)+30;}
    if(kind>=16&&kind<=32){
        if(!context||!context->spirit){error="Animal spirit spawn service unavailable";return 0;}
        const float dx=float(context->player.x-position.x),dy=float(context->player.y-position.y);
        const float aiming=float(std::atan2(double(dy),double(dx)));
        float direction=0;u32 attempts=0;
        for(;;){if(((direction<-.34906584f||direction>.34906584f)&&(direction>-2.792527f&&direction<2.792527f)&&(direction<-1.9198623f||direction>-1.2217305f)&&(direction<1.2217305f||direction>1.9198623f))||attempts>99)break;direction=normalize_angle(float(double(context->game_random.signed_unit())*double(1.0471976f)+double(aiming)));attempts++;}
        if(!context->spirit(kind-15,position,direction))error="Animal spirit creation failed";return 0;
    }
    if(kind<1||kind>15){error="Invalid item kind";return 0;}const bool cancel=kind>=9&&kind<=14;auto& free=free_slots[cancel];if(free.empty())return 0;
    const u32 index=free.back();auto& slot=slots[index];auto& state=slot.state;
    state.position=position;state.velocity={float(std::cos(double(angle))*double(speed)),float(std::sin(double(angle))*double(speed)),0};state.age.set(0);state.flags=0;state.sound=sound_id;
    if(cancel){state.speed=speed;state.angle=angle;state.stamp=cancel_stamp;const i32 divisor=cancel_density<256?4:cancel_density<512?8:cancel_density<1024?16:32,offset=cancel_density<256?0:cancel_density<512?4:cancel_density<1024?8:16;state.delay=wrapping_add(requests%divisor,offset);state.state=5;state.kind=state.appearance=kind;}
    else{state.state=1;state.position.x=state.position.x<=-192.f?-192.f:state.position.x>=192.f?192.f:state.position.x;state.speed=0;state.attraction_speed=0;state.delay=delay;state.kind=kind==15?6:kind;state.appearance=0;if((delay==0&&!birth_effect(state))||!appearance(slot,state.kind))return 0;slot.body->visual.color=0xffffffff;}
    free.pop_back();return index+1;
}
bool ItemManager::release(u32 id){auto* slot=find(id);if(!slot||!slot->state.state){error="Item release slot unavailable";return false;}slot->state.state=0;free_slots[id-1>=ordinary_count].push_back(id-1);return true;}
bool ItemManager::falling(u32 id){auto* slot=find(id);if(!slot){error="Item falling slot unavailable";return false;}slot->state.state=2;if(!slot->body)slot->body=std::make_unique<AnmVm>();const i32 kind=slot->state.kind;if(kind<1||kind>14){error="Item falling animation unavailable";return false;}
    if(!animations.bind_template(*slot->body,resource,scripts[kind][0])||animations.tick_instance(*slot->body)<0){error=animations.error;return false;}
    slot->arrow_ended=true;if(slot->arrow){slot->arrow->visual.flags&=~1u;slot->arrow->instruction_offset=-1;}return true;
}
bool ItemManager::draw(AnmRenderer& renderer){
    for(auto& slot:slots){auto& s=slot.state;if(!s.state||!slot.body||!slot.body->visual.visible()||s.delay>0)continue;slot.body->visual.translation=s.position;if(slot.arrow)slot.arrow->visual.translation=s.position;
        if(slot.body->variables.position.y<-8.f){if(slot.arrow&&slot.arrow->visual.visible()){auto& arrow=*slot.arrow;const float y=float(arrow.variables.position.y+8.f);arrow.variables.position.y=8;const u32 alpha=y>=32.f?255u:u32(u8(truncate_int(float(float(y*.03125f)*255.f))));arrow.visual.color=(arrow.visual.color&0xffffffu)|(alpha<<24);if(renderer.draw(arrow)==-2){error=renderer.error;return false;}}s.appearance=1;}
        else{if(renderer.draw(*slot.body)==-2){error=renderer.error;return false;}s.appearance=0;}
    }return true;
}
}
