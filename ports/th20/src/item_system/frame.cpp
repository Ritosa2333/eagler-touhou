#include "item.hpp"
#include "../ecl_vm/math.hpp"
#include "../gameplay/gameplay.hpp"
#include <cstring>
#include <cmath>
#include <emmintrin.h>
namespace th20::source::item {
namespace {
float a(float x,float y){return recovered::add32(x,y);}float m(float x,float y){return recovered::mul32(x,y);}
float s(float x,float y){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(x),_mm_set_ss(y)));}float d(float x,float y){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(x),_mm_set_ss(y)));}
int increment(int value){return recovered::signed_bits(static_cast<unsigned>(value)+1);}int decrement(int value){return recovered::signed_bits(static_cast<unsigned>(value)-1);}
template<class T>T read(const void* object,std::size_t offset){T value;std::memcpy(&value,static_cast<const std::uint8_t*>(object)+offset,sizeof(value));return value;}
bool animation_active(const sprite::Animation& animation){return (animation.base.flags[0]&0x10000u)!=0;}
float distance_squared(const sprite::Vec3& first,const sprite::Vec3& second){const auto x=s(first.x,second.x),y=s(first.y,second.y);return a(m(x,x),m(y,y));}
void move(Item& item,float scale,float extra=1){for(unsigned i=0;i<3;++i)(&item.position.x)[i]=a((&item.position.x)[i],m(m((&item.velocity.x)[i],scale),extra));}
float player_angle(const void* player,const sprite::Vec3& position){const auto target=read<sprite::Vec3>(player,0x614);const auto x=s(target.x,position.x),y=s(target.y,position.y);return x==0&&y==0?d(3.1415927410125732421875f,2):ecl::math::arctangent(y,x);}
bool bomb_collecting(const void* bomb){return read<int>(bomb,0x18)==1&&read<int>(bomb,0x20)<60;}
void pursue(Item& item,const void* player){ecl::math::polar(item.velocity.x,item.velocity.y,player_angle(player,item.position),item.attraction_speed);move(item,state::clock_scale);if(item.attraction_speed<12)item.attraction_speed=a(item.attraction_speed,.2f);if(read<int>(player,0x10)==4){item.state=1;item.velocity.x=0;item.velocity.y=0;}}
}
void retire(Item& item,Environment& host){item.state=0;host.retire_attachment(item.attachment);scheduler::unlink(item.link);auto& destination=*item.free_list;scheduler::insert_after(destination.sentinel,item.link);item.link.owner=&destination;if(destination.tail==&destination.sentinel)destination.tail=&item.link;}
int update(ItemInf& owner,Environment& host){
    host.select_view(owner);owner.special_count=0;owner.processed=0;auto* player=owner.context->objects_04[0];
    for(scheduler::Iterator iterator(owner.active.sentinel.next);iterator.current;iterator.advance()){
        auto& item=*reinterpret_cast<Item*>(iterator.current->value);if(!item.state)continue;
        if(item.state==5){item.delay=decrement(item.delay);if(item.delay<0)host.activate_special(item);continue;}
        auto forced_collect=[&](){const auto mode=read<int>(player,0x10);const auto position=read<sprite::Vec3>(player,0x614);return (mode!=2&&mode!=4&&!(read<float>(player,0x2090)<=position.y))||bomb_collecting(owner.context->objects_04[5])||host.boss_collecting();};
        if(item.state==1){
            if(item.delay>0){item.delay=decrement(item.delay);if(item.delay<1)host.spawn_effect(item);continue;}
            if(forced_collect()){item.attraction_speed=read<float>(player,0x2080);item.state=3;pursue(item,player);}
            else {
                move(item,state::clock_scale,owner.speed_scale);item.velocity.y=a(item.velocity.y,m(m(state::clock_scale,.03f),owner.speed_scale));
                if(item.velocity.y>=0)item.velocity.x=0;if(item.velocity.y>2)item.velocity.y=2;
                if(!(item.position.y<=472)||!(std::fabs(item.position.x)<200)){retire(item,host);continue;}
            }
        } else if(item.state==2){
            move(item,state::clock_scale);item.velocity.y=a(item.velocity.y,m(state::clock_scale,.03f));if(item.velocity.y>=0)item.state=1;
            if(item.position.y>472||std::fabs(item.position.x)>=200){retire(item,host);continue;}
        } else if(item.state==3)pursue(item,player);
        else if(item.state==4){if(forced_collect()){item.attraction_speed=read<float>(player,0x2080);item.state=3;}pursue(item,player);}
        if(read<int>(player,0x10)!=2){
            const auto player_position=read<sprite::Vec3>(player,0x614);const auto collect_radius=read<float>(player,0x2084);
            const bool collect=distance_squared(player_position,item.position)<m(collect_radius,collect_radius)||(owner.attract&&distance_squared(owner.attraction_center,item.position)<256);
            if(collect){host.collect(item);host.collect_sound(item);retire(item,host);continue;}
            if(item.state!=4&&item.state!=3&&item.state!=2){const auto radius=read<float>(player,0x2088);if(distance_squared(player_position,item.position)<m(radius,radius)){item.attraction_speed=d(read<float>(player,0x2080),3);item.state=4;}}
        }
        if(animation_active(item.animation))host.update_animation(item.animation);if(animation_active(item.secondary_animation))host.update_animation(item.secondary_animation);host.move_attachment(item.attachment,item.position);recovered::timer_tick(item.timer,state::timer_rate);owner.processed=increment(owner.processed);
    }
    if(owner.speed_scale<1)owner.speed_scale=a(owner.speed_scale,.1f);owner.attract=0;return 1;
}
int draw(ItemInf& owner,int layer,Environment& host){
    host.configure_layer(owner,layer==1?12:6);
    for(scheduler::Iterator iterator(owner.active.sentinel.next);iterator.current;iterator.advance()){
        auto& item=*reinterpret_cast<Item*>(iterator.current->value);if(!item.state||!animation_active(item.animation)||item.delay>0||layer==0)continue;
        item.animation.vector_5bc=item.position;item.secondary_animation.vector_5bc=item.position;
        if(!(item.animation.base.vector_2c.y < -8)){host.draw_animation(item.animation);item.draw_state=0;}
        else {
            if(animation_active(item.secondary_animation)){
                const auto shifted=a(item.secondary_animation.base.vector_2c.y,8);item.secondary_animation.base.vector_2c.y=8;
                const auto alpha=shifted<32?static_cast<std::uint8_t>(_mm_cvtt_ss2si(_mm_set_ss(m(d(shifted,32),255)))):std::uint8_t(255);
                item.secondary_animation.base.field_490=(item.secondary_animation.base.field_490&0x00ffffffu)|(static_cast<std::uint32_t>(alpha)<<24);host.draw_animation(item.secondary_animation);
            }
            item.draw_state=1;
        }
    }
    return 1;
}
int update_callback(ItemInf& owner){if(gameplay::controller&&(gameplay::controller->update_suppressed()||(gameplay::controller->game_flags&0x800u)))return 1;return update(owner);}
int draw_callback(ItemInf& owner,int layer){if(gameplay::controller&&(gameplay::controller->game_flags&4u))return 1;return draw(owner,layer);}
}
