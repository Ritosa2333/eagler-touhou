#include "frame_helpers.hpp"
#include "../damage_regions/regions.hpp"
#include "../ecl_vm/math.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <cstring>
namespace th20::source::player_entity {
void advance_fixed_motion(Player& player,std::int32_t x,std::int32_t y,float clock_scale){
    player.vector_20c4.x=float(x)*clock_scale;player.vector_20c4.y=float(y)*clock_scale;if(player.fields_674[2])player.vector_20d0=player.vector_20c4;
    const auto dx=recovered::truncate32(player.vector_20c4.x),dy=recovered::truncate32(player.vector_20c4.y);std::memcpy(&player.vector_20dc.x,&dx,4);std::memcpy(&player.vector_20dc.y,&dy,4);
    player.fixed_position.x=recovered::signed_bits(std::uint32_t(player.fixed_position.x)+std::uint32_t(dx));player.fixed_position.y=recovered::signed_bits(std::uint32_t(player.fixed_position.y)+std::uint32_t(dy));
    if(((player.entity_flags>>6)&3u)==0){player.fixed_position.x=std::clamp(player.fixed_position.x,-0x5c00,0x5c00);player.fixed_position.y=std::clamp(player.fixed_position.y,0x1000,0xd800);}
    player.position_614.x=float(player.fixed_position.x)/128.0f;player.position_614.y=float(player.fixed_position.y)/128.0f;
}
void update_feedback(Feedback& feedback,ShotCallbackEnvironment& env){
    const float rate=env.firing().clock_rate();
    if(feedback.timers[1].current>0){recovered::timer_add(feedback.timers[1],-1,&rate);if(feedback.timers[1].current<=0)feedback.fields_30[1]=0;}
    if(feedback.timers[2].current!=0)recovered::timer_add(feedback.timers[2],-1,&rate);
    const auto& position=static_cast<Player*>(env.firing().session().contexts[0].objects_04[0])->position_614;
    if(!feedback.enabled){if(position.x< -140.0f&&position.y<80.0f)feedback.enabled=1;}else if(!(position.x<=-120.0f&&position.y<=96.0f))feedback.enabled=0;
    recovered::timer_add(feedback.timers[0],-1,&rate);
    if(feedback.timers[0].current<=0){if(auto* animation=env.find_animation(feedback.handle_4c))animation->base.vector_50={0,0};feedback.fields_30[2]=0;recovered::timer_set(feedback.timers[0],0);}
    else if(auto* animation=env.find_animation(feedback.handle_4c))animation->base.vector_50={feedback.timers[0].current_f/30.0f,1};
}
bool shot_corners_outside(const sprite::Vec3(&corners)[4],std::int32_t x,std::int32_t y){
    const float left=float(x)-192.0f,right=float(x)+192.0f,top=float(y),bottom=float(y)+448.0f;
    // Every corner uses COMISS/JAE rejection: unordered values do not reject.
    for(const auto& p:corners)if(!(p.x<=left||right<=p.x||p.y<=top||bottom<=p.y))return false;
    return true;
}
void update_shot(Shot& shot,ShotFrameServices& services){
    constexpr float pi=0x1.921fb6p+1f;auto& env=services.callbacks();auto& host=env.firing();const auto& row=shot_record(*static_cast<Player*>(shot.context->objects_04[0]),shot.fields_b8[8]);
    const int duration=std::bit_cast<int>(row.field_34);if(duration>0&&shot.timer_1c.current>=duration){env.interrupt(shot.handle_18,1);recovered::timer_set(shot.timer_1c,-999);}
#ifdef TH_SDL3
    // Stored update is __fastcall (Shot*,void* dummy); wasm needs the dummy.
    if(shot.fields_3c[2]&&reinterpret_cast<int(*)(Shot*,void*)>(shot.fields_3c[2])(&shot,nullptr)){host.retire(shot);return;}
#else
    if(shot.fields_3c[2]&&reinterpret_cast<int(__thiscall*)(Shot*)>(shot.fields_3c[2])(&shot)){host.retire(shot);return;}
#endif
    if(shot.fields_98[1]!=2){shot.motion.angle_1c=ecl::math::wrap_angle(shot.motion.angle_1c+row.field_18);shot.motion.field_18+=std::bit_cast<float>(row.fields_20[0]);}
    state::update_motion(shot.motion,host.clock_rate());auto* animation=env.find_animation(shot.handle_18);if(!animation){host.retire(shot);return;}
    if(row.type!=2&&row.type!=8){sprite::Vec3 corners[4]{};services.corners(*animation,corners);if(shot.timer_1c.current>=15&&shot_corners_outside(corners,services.viewport_x(),services.viewport_y())){host.retire(shot);return;}}
    if(shot.fields_b8[9]&&(shot.fields_3c[0]&1u))if(auto* region=host.damage(shot.fields_b8[9])){region->motion.position=shot.motion.position;region->angle=ecl::math::wrap_angle(shot.motion.angle_1c);region->size=shot.vector_b0;region->damage=std::bit_cast<int>(shot.fields_98[5]);}
    animation->vector_5bc=shot.motion.position;const auto orientation=(animation->base.flags[2]>>21)&7u;
    auto angle=[&](float value){animation->base.vector_38.z=value;animation->base.flags[1]|=2;};
    switch(orientation){
    case 1:angle(shot.motion.angle_1c);break;
    case 2:if(shot.motion.angle_1c>=-pi/2&&shot.motion.angle_1c<=pi/2){angle(shot.motion.angle_1c);animation->base.vector_50.x=std::fabs(animation->base.vector_50.x);}else{angle(ecl::math::wrap_angle(shot.motion.angle_1c+pi));animation->base.vector_50.x=-std::fabs(animation->base.vector_50.x);}animation->base.flags[1]|=4;break;
    case 3:angle(ecl::math::wrap_angle(ecl::math::wrap_angle(shot.motion.angle_1c+pi)));break;
    case 4:angle(ecl::math::wrap_angle(ecl::math::wrap_angle(shot.motion.angle_1c+pi/2)));break;
    case 5:angle(ecl::math::wrap_angle(ecl::math::wrap_angle(ecl::math::angle_difference(shot.motion.angle_1c,pi/2))));break;
    default:break;
    }
    const float rate=host.clock_rate();recovered::timer_tick(shot.timer_1c,&rate);
}
}
