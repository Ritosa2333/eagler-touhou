#include "PlayerMotion.hpp"
#include <algorithm>
namespace th15 {
void PlayerMotion::configure(const ShtHeader& shot)noexcept{normal_speed=truncate_int(float(shot.speed*128.f));focus_speed=truncate_int(float(shot.focus_speed*128.f));normal_diagonal_speed=truncate_int(float(shot.diagonal_speed*128.f));focus_diagonal_speed=truncate_int(float(shot.focus_diagonal_speed*128.f));}
bool PlayerMotion::update(u32 input,bool focus_allowed,float rate,PlayerMotionVisuals& visuals){
    const u32 movement=input&0xf0;if((movement&0x50)==0x50)direction=5;else if((movement&0x60)==0x60)direction=7;else if((movement&0x90)==0x90)direction=6;else if((movement&0xa0)==0xa0)direction=8;else if(movement&0x20)direction=2;else if(movement&0x10)direction=1;else if(movement&0x40)direction=3;else if(movement&0x80)direction=4;else direction=0;
    if(!focus_allowed||input_frame<4){focus=0;option_follow_percentage=30;}else focus=(input>>3)&1;
    const float indicator_scale=behavior_flags&16?float(float(float(enlargement-1.f)*2.f)+1.f):1.f;if(!visuals.focus_begin(focus!=0,indicator_scale)){error="Player focus animation failed";return false;}
    constexpr i32 dx[]={0,0,0,-1,1,-1,1,-1,1},dy[]={0,-1,1,0,0,-1,-1,1,1};const i32 speed=focus?(direction<5?focus_speed:focus_diagonal_speed):(direction<5?normal_speed:normal_diagonal_speed);
    i32 moving_x=wrapping_mul(speed,dx[direction]),moving_y=wrapping_mul(speed,dy[direction]);
    if(touch.mode){
        if(!touch.valid()){error="Invalid continuous movement request";return false;}
        double x=double(touch.x)*128-position_fixed.x,y=double(touch.y)*128-position_fixed.y;
        if(touch.mode!=2){const double limit=focus?focus_speed:normal_speed,length=std::sqrt(x*x+y*y);if(length>limit&&length>0){x*=limit/length;y*=limit/length;}}
        moving_x=truncate_int(x);moving_y=truncate_int(flip_vertical_step?-y:y);const u32 bits=(moving_x<0?0x40:moving_x>0?0x80:0)|(moving_y<0?0x10:moving_y>0?0x20:0);
        direction=bits==0x50?5:bits==0x90?6:bits==0x60?7:bits==0xa0?8:bits==0x10?1:bits==0x20?2:bits==0x40?3:bits==0x80?4:0;
    }
    const i32 x=truncate_int(float(float(wrapping_sub(moving_x,truncate_int(float(external_velocity.x*-128.f))))*movement_scale)),y=truncate_int(float(float(wrapping_sub(moving_y,truncate_int(float(external_velocity.y*-128.f))))*movement_scale));
    i32 script=-1;if(x<0&&last_step.x>=0)script=1;else if(x>0&&last_step.x<=0)script=3;else if(!x&&last_step.x<0)script=2;else if(!x&&last_step.x>0)script=4;if(script>=0&&!visuals.pose(script)){error="Player directional animation failed";return false;}
    last_step={x,y};velocity.x=float(float(x)*rate);velocity.y=float(float(y)*rate);if(direction)last_direction=velocity;step={truncate_int(velocity.x),truncate_int(velocity.y)};
    position_fixed.x=std::clamp(wrapping_add(position_fixed.x,step.x),-0x5c00,0x5c00);position_fixed.y=std::clamp(flip_vertical_step?wrapping_sub(position_fixed.y,step.y):wrapping_add(position_fixed.y,step.y),0x1000,0xd800);position.x=float(float(position_fixed.x)*.0078125f);position.y=float(float(position_fixed.y)*.0078125f);visuals.focus_position(position);
    if(behavior_flags&2)collapse_frame=wrapping_add(collapse_frame,1);
    for(u32 i=0;i<options.size();i++){auto& option=options[i];if(!option.active)continue;
        if(!(behavior_flags&2)){const auto& offset=focus?option.focus_offset:option.normal_offset;option.target={wrapping_add(position_fixed.x,offset.x),wrapping_add(position_fixed.y,offset.y)};if(option.movement_callback&&!option.movement_callback(option)){error="Player option movement failed";return false;}}
        else{option.target=position_fixed;if(collapse_frame>29){option.active=0;if(!visuals.option_remove(i)){error="Player option removal failed";return false;}option_count=0;continue;}}
        if(option.snap){option.snap=0;option.position=option.target;}else if(option_follow_percentage>29){const i32 ox=wrapping_mul(wrapping_sub(option.target.x,option.position.x),option_follow_percentage)/100,oy=wrapping_mul(wrapping_sub(option.target.y,option.position.y),option_follow_percentage)/100;if(!ox&&!oy)option.position=option.target;else option.position={wrapping_add(option.position.x,ox),wrapping_add(option.position.y,oy)};}
        visuals.option_position(i,{float(float(option.position.x)*.0078125f),float(float(option.position.y)*.0078125f),0});
    }
    if(barrier_timer.current>0){if(!visuals.barrier_position(position)){error="Player barrier animation failed";return false;}barrier_timer.decrement(&rate);if(barrier_timer.current<1){if(!visuals.barrier_remove()){error="Player barrier removal failed";return false;}barrier_timer.set(0);}}
    return true;
}
}
