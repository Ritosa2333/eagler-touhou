#include "item.hpp"
#include "../ecl_vm/math.hpp"
#include "../sprite_renderer/pool.hpp"
#include <emmintrin.h>
#include <stdexcept>
namespace th20::source::item {
namespace {
float add(float a,float b){return recovered::add32(a,b);}float mul(float a,float b){return recovered::mul32(a,b);}
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
int increment(int value){return recovered::signed_bits(static_cast<unsigned>(value)+1);}
constexpr int scripts[16][2]{{-1,-1},{115,137},{116,138},{117,139},{118,140},{119,141},{120,142},{121,143},{122,144},{123,-1},{124,-1},{125,-1},{126,-1},{127,-1},{128,-1},{-1,-1}};
Item* take(scheduler::List& list,scheduler::List& active){auto* link=list.sentinel.next;if(!link)return nullptr;auto* item=reinterpret_cast<Item*>(link->value);scheduler::unlink(item->link);scheduler::insert_after(active.sentinel,item->link);item->link.owner=&active;if(active.tail==&active.sentinel)active.tail=&item->link;return item;}
}
Item* spawn(ItemInf& owner,int type,const sprite::Vec3& position,std::uint32_t color,float angle,float speed,int delay,std::uint32_t extra,int sound,Environment& host){
    auto& player=*game_session::context(0).current_player;
    if(type==1&&player.bytes_a4[1]){
        owner.bonus_counter=increment(owner.bonus_counter);const int interval=player.bytes_2c[3]?14:10;
        if(owner.bonus_counter%interval==interval-1){host.bonus_notification();const auto y=add(mul(state::signed_unit(state::random_streams[0]),16),position.y);const auto x=add(mul(state::signed_unit(state::random_streams[0]),16),position.x);spawn(owner,1,{x,y,0},color,angle,speed,recovered::signed_bits(static_cast<unsigned>(delay)+16),extra,sound,host);}
    }
    if(type>=16)return nullptr;
    owner.spawn_counter=increment(owner.spawn_counter);
    if(type>=9&&type<=13){
        auto* item=take(owner.special_free,owner.active);if(!item)return nullptr;item->generation=owner.generation;
        if(owner.special_count<256)item->delay=owner.spawn_counter%4;else if(owner.special_count<512)item->delay=owner.spawn_counter%8+4;else if(owner.special_count<1024)item->delay=owner.spawn_counter%16+8;else item->delay=owner.spawn_counter%32+16;
        item->state=5;item->type=type;item->draw_state=0;item->position=position;item->position.z=0;ecl::math::polar(item->velocity.x,item->velocity.y,angle,speed);item->velocity.z=0;recovered::timer_set(item->timer,0);item->angle=angle;item->speed=speed;item->extra=extra;item->sound=sound;select_context(*item,owner.view_index);return item;
    }
    if(type==15){owner.point_counter=recovered::signed_bits(static_cast<unsigned>(owner.point_counter)+game_session::session.player_table.field_1e0+1);if(owner.point_counter<10)return nullptr;owner.point_counter=0;type=2;}
    auto* item=take(owner.ordinary_free,owner.active);if(!item)return nullptr;
    item->state=1;item->position=position;item->position.z=0;if(item->position.x>-192){if(item->position.x>=192)item->position.x=192;}else item->position.x=-192;
    if(type==14)type=6;ecl::math::polar(item->velocity.x,item->velocity.y,angle,speed);item->velocity.z=0;recovered::timer_set(item->timer,0);item->speed=0;item->attraction_speed=0;item->delay=delay;item->type=type;item->sound=sound;select_context(*item,owner.view_index);
    if(delay==0)host.spawn_effect(*item);item->draw_state=0;
    if(type<0)throw std::out_of_range("Original Item script table index");host.bind_animation(owner,item->animation,scripts[type][0]);host.bind_animation(owner,item->secondary_animation,scripts[type][1]);item->attachment=0;sprite::set_animation_color(item->animation,color);item->extra=extra;return item;
}
void spawn_many(ItemInf& owner,const sprite::Vec3& position,int count,int type,Environment& host){constexpr float pi=3.1415927410125732421875f;for(;count>0;--count){const auto angle=sub(mul(state::signed_unit(state::random_streams[0]),mul(div(pi,180),10)),div(pi,2));spawn(owner,type,position,0xffffffffu,angle,2,0,0,-1,host);}}
}
