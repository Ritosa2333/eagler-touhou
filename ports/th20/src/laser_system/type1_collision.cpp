#include "type1.hpp"
#include "../player_entity/player.hpp"
#include "../player_entity/events.hpp"
#include "../runtime_state/state.hpp"
#include "../ecl_vm/math.hpp"
#include <algorithm>
namespace th20::source::laser {
namespace n=recovered;namespace m=ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
//454d80: project into the centered segment, return the actual XY distance.
float project(const sprite::Vec3& center,float length,float angle,const sprite::Vec3& p,sprite::Vec3& output){
    sprite::Vec3 relative{sub(p.x,center.x),sub(p.y,center.y),sub(p.z,center.z)},local{};m::rotate(relative.x,relative.y,-angle);
    if(relative.x>div(-length,2)&&div(length,2)>relative.x)local.x=relative.x;else if(relative.x>0)local.x=div(length,2);else if(0>relative.x)local.x=div(-length,2);
    const float x=sub(local.x,relative.x),y=sub(local.y,relative.y);const float distance=m::square_root(n::add32(n::mul32(x,x),n::mul32(y,y)));
    m::rotate(local.x,local.y,angle);output={n::add32(center.x,local.x),n::add32(center.y,local.y),n::add32(center.z,local.z)};return distance;
}
}
float project_to_segment(const sprite::Vec3& center,float length,float angle,const sprite::Vec3& p,sprite::Vec3& output){return project(center,length,angle,p,output);}
int Type1Laser::cancel_all(int preview){
    auto& segment=*static_cast<Segment*>(allocated_6d8);animation.base.flags[2]&=~0x1c00u;
    const bool valid=(flags&0x40)?bool(segment.flags&8):collision_segment(nullptr)!=0;if(!valid)return 0;
    auto* player=context->objects_04[0];const int collision=player_entity::collide_rectangle(player,segment.position,segment.angle,segment.size.y,segment.size.x,preview);
    if(collision==1){cancel_rectangle(player_entity::position(player),sprite::Vec3{32,32,0},0,0,1);}
    else if(collision==2){
        ++field_6d4;if(timer_38.current%8==0){sprite::Vec3 projected{};const auto player_position=player_entity::position(player);project(segment.position,segment.size.x,segment.angle,player_position,projected);
            const sprite::Vec3 middle{div(n::add32(projected.x,player_position.x),2),div(n::add32(projected.y,player_position.y),2),div(n::add32(projected.z,player_position.z),2)};
            player_entity::graze(player,middle,field_6d0);
        }
        if(!(active_commands&(std::uint64_t{1}<<33))){const auto color=std::clamp(n::signed_bits(208u-field_6d4*2u),96,240);animation.base.flags[2]=(animation.base.flags[2]&~0x1c00u)|0x400u;animation.base.field_494=0xffff0080u|(std::uint32_t(color)<<8);}
        n::timer_tick(timer_38,state::timer_rate);
    }else field_6d4=0;return 0;
}
}
