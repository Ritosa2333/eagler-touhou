#include "PlayerMovement.hpp"
namespace th08 {
i32 player_direction(u16 buttons)noexcept{
    if((buttons&0x50)==0x50)return 5;
    if((buttons&0x60)==0x60)return 7;
    if((buttons&0x90)==0x90)return 6;
    if((buttons&0xa0)==0xa0)return 8;
    if(buttons&0x20)return 2;if(buttons&0x10)return 1;if(buttons&0x40)return 3;if(buttons&0x80)return 4;return 0;
}
void move_player(PlayerMovementState& s,const ShotProfile& human,const ShotProfile& focused,
                 bool focus,u8 character,u16 buttons,const Vec2& minimum,const Vec2& extent,
                 const FrameTiming& timing,PlayerMovementActions* actions,bool record_history){
    s.direction=player_direction(buttons);
    const float straight=focus?focused.focus_speed:human.normal_speed;
    const float diagonal=focus?focused.focus_diagonal:human.normal_diagonal;
    float x=0,y=0;
    switch(s.direction){
    case 1:y=-straight;break;case 2:y=straight;break;case 3:x=-straight;break;case 4:x=straight;break;
    case 5:x=y=-diagonal;break;case 6:x=diagonal;y=-diagonal;break;case 7:x=-diagonal;y=diagonal;break;case 8:x=y=diagonal;break;
    }
    if(actions&&actions->movement(s,straight,timing,x,y))s.direction=player_direction((x<0?64:x>0?128:0)|(y<0?16:y>0?32:0));
    x=Scalar::mul(x,s.multiplier.x);y=Scalar::mul(y,s.multiplier.y);
    const i32 base=(character<4?focus:character&1)?5:0;
    if(x<0&&s.delta.x>=0){if(actions)actions->pose(base+1);}
    else if(x==0&&s.delta.x<0){if(actions)actions->pose(base+2);}
    if(x>0&&s.delta.x<=0){if(actions)actions->pose(base+3);}
    else if(x==0&&s.delta.x>0){if(actions)actions->pose(base+4);}
    s.delta={x,y};s.velocity.x=Scalar::mul(x,timing.rate);s.velocity.y=Scalar::mul(y,timing.rate);
    s.position.x=Scalar::add(s.position.x,s.velocity.x);s.position.y=Scalar::add(s.position.y,s.velocity.y);
    const auto clamp=[](float& position,float minimum,float extent){if(position<minimum)position=minimum;else if(number(minimum)+number(extent)<number(position))position=Scalar::add(minimum,extent);};
    clamp(s.position.x,minimum.x,extent.x);clamp(s.position.y,minimum.y,extent.y);
    for(u32 i=0;i<3;++i){const auto& half=s.half_boxes[i];s.bounds[i*2]={Scalar::sub(s.position.x,half.x),Scalar::sub(s.position.y,half.y),Scalar::sub(s.position.z,half.z)};s.bounds[i*2+1]={Scalar::add(s.position.x,half.x),Scalar::add(s.position.y,half.y),Scalar::add(s.position.z,half.z)};}
    if(record_history)record_player_position(s);
}
void record_player_position(PlayerMovementState& s)noexcept{if(s.delta.x!=0||s.delta.y!=0){for(i32 i=15;i>0;--i)s.history[i]=s.history[i-1];s.history[0]=s.position;}}
}
