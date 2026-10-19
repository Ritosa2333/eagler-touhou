#include "player.hpp"
#include "../ecl_vm/math.hpp"
#include "../damage_regions/geometry.hpp"
#include <cstring>
namespace th20::source::player_entity {
namespace {
template<class T>T get(const void* p,std::size_t offset){T value;std::memcpy(&value,static_cast<const std::uint8_t*>(p)+offset,sizeof(value));return value;}
float subtract(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float divide(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
bool suppressed(CollisionServices& host){const auto* hud=host.boss_hud();return hud&&get<std::uint32_t>(hud,0x1bc)!=0;}
bool invalid_state(const void* player){const int state=get<std::int32_t>(player,0x10);return state==2||state==4||state==3;}
bool invulnerable(const void* player){return get<recovered::Timer>(player,0x2050).current>0;}
game_session::Player& record(void* player){return *get<game_session::Context*>(player,0x14858)->current_player;}
float base_radius(const void* player){return get<float>(player,focused(player)?0x2098:0x2094);}
int overlap_result(void* player,int preview,bool invulnerable_is_hit,CollisionServices& host){
    if(suppressed(host))return 0;if(preview)return 2;if(invalid_state(player))return 0;
    if(invulnerable(player))return invulnerable_is_hit?1:0;
    host.hit(player);return 1;
}
}
sprite::Vec3 position(const void* player) noexcept{return get<sprite::Vec3>(player,0x614);}
float angle_to_player(const void* player,const sprite::Vec3& origin){
    const auto p=position(player);const float dx=subtract(p.x,origin.x),dy=subtract(p.y,origin.y);
    if(dy==0.0f&&dx==0.0f)return 0x1.921fb6p+0f;
    return ecl::math::arctangent(dy,dx);
}
bool focused(const void* player) noexcept{return get<std::uint8_t>(player,0x204c)!=0;}
bool expanded_collision(const void* player) noexcept{return (get<std::uint32_t>(player,0x14)&0x10u)!=0;}
std::int32_t collision_percent(game_session::Player& player) noexcept{
    int value=get<std::int32_t>(&player,0xe8);if(value<0)value=0;if(value>100)value=100;
    std::memcpy(reinterpret_cast<std::uint8_t*>(&player)+0xe8,&value,4);return value;
}
int collide_axis_aligned(void* player,const sprite::Vec3& center,const sprite::Vec2& size,int preview,CollisionServices& host){
    const auto p=position(player),extent=get<sprite::Vec3>(player,focused(player)?0x20a8:0x209c);
    const float left=subtract(p.x,extent.x),top=subtract(p.y,extent.y),right=recovered::add32(p.x,extent.x),bottom=recovered::add32(p.y,extent.y);
    const auto overlap=[&](float width,float height){
        const float lo_x=subtract(center.x,divide(width,2)),lo_y=subtract(center.y,divide(height,2));
        const float hi_x=recovered::add32(divide(width,2),center.x),hi_y=recovered::add32(divide(height,2),center.y);
        //JA/JBE comparisons treat unordered coordinates as overlapping.
        return !(left>hi_x||top>hi_y||lo_x>right||lo_y>bottom);
    };
    if(!overlap(size.x,size.y))return overlap(48.0f,48.0f)?2:0;
    return overlap_result(player,preview,true,host);
}
int collide_circle(void* player,const sprite::Vec3& center,float radius,int preview,CollisionServices& host){
    const auto p=position(player);const float dx=subtract(p.x,center.x),dy=subtract(p.y,center.y);
    const float distance=recovered::add32(recovered::mul32(dx,dx),recovered::mul32(dy,dy));
    float hit_radius=divide(recovered::mul32(recovered::int_float(collision_percent(record(player))),base_radius(player)),100.0f);
    if(expanded_collision(player))hit_radius=recovered::mul32(recovered::mul32(get<float>(player,0x2238),3.5999999046325684f),hit_radius);
    //COMISS distance,threshold followed byJB includes unordered IEEE inputs.
    if(!(distance>=recovered::add32(recovered::mul32(hit_radius,hit_radius),recovered::mul32(radius,radius))))return overlap_result(player,preview,true,host);
    float graze_radius=divide(radius,2.5f);if(graze_radius<40.0f)graze_radius=40.0f;
    const auto total=recovered::add32(hit_radius,graze_radius);
    return !(distance>=recovered::add32(recovered::mul32(total,total),recovered::mul32(radius,radius)))?2:0;
}
int collide_rectangle(void* player,const sprite::Vec3& center,float angle,float width,float height,int preview,CollisionServices& host){
    const float hit_radius=divide(recovered::mul32(recovered::int_float(collision_percent(record(player))),base_radius(player)),100.0f);
    const float graze_radius=recovered::add32(base_radius(player),30.0f);const auto p=position(player);
    //4f91d0 passes its height argument first to457610. It intentionally does
    //not apply4ff800's expanded radius and invulnerable contacts return zero.
    if(!geometry::rectangle_circle(center.x,center.y,height,width,angle,p.x,p.y,graze_radius))return 0;
    if(!geometry::rectangle_circle(center.x,center.y,height,width,angle,p.x,p.y,hit_radius))return 2;
    return overlap_result(player,preview,false,host);
}
}
