#include "PlayerMotion.hpp"
#include <cmath>
namespace th09 {
namespace {
Bounds bounds(Vec3 position,Vec3 extent){return {{position.x-extent.x,position.y-extent.y,position.z-extent.z},{position.x+extent.x,position.y+extent.y,position.z+extent.z}};}
}
// Movement, focus transitions, option-update order and position history from
// TH09 1.50a 0x41c170. Animation/effect ownership stays in named game managers.
void PlayerMotion::update(u16 keys,bool automatic_focus,u16 fire_frames,const MovementSpeeds& speeds,const PlayfieldLimits& field,float rate,MotionEffects& effects){
    if(health==0)return;
    if((keys&0x50)==0x50)direction=5;
    else if((keys&0x60)==0x60)direction=7;
    else if((keys&0x90)==0x90)direction=6;
    else if((keys&0xa0)==0xa0)direction=8;
    else if(keys&0x20)direction=2;
    else if(keys&0x10)direction=1;
    else if(keys&0x40)direction=3;
    else if(keys&0x80)direction=4;
    else direction=0;
    const bool focused=automatic_focus?fire_frames>7:bool(keys&4);
    flags=(flags&~3u)|u32(focused);
    if(focused&&!focus_effects){effects.begin_focus(position,player,character);focus_effects=true;}
    else if(!focused&&focus_effects){effects.end_focus();focus_effects=false;}
    const float speed=focused?speeds.focused:speeds.normal,diagonal=focused?speeds.focused_diagonal:speeds.diagonal;
    float dx=0,dy=0;
    switch(direction){
    case 1:angle=-0x1.921fb6p0f;dy=-speed;break;
    case 2:angle=0x1.921fb6p0f;dy=speed;break;
    case 3:angle=0x1.921fb6p1f;dx=-speed;break;
    case 4:angle=0;dx=speed;break;
    case 5:angle=-0x1.2d97c8p1f;dx=-diagonal;dy=dx;break;
    case 6:angle=-0x1.921fb6p-1f;dx=diagonal;dy=-dx;break;
    case 7:angle=0x1.2d97c8p1f;dy=diagonal;dx=-dy;break;
    case 8:angle=0x1.921fb6p-1f;dx=diagonal;dy=dx;break;
    }
    if(effects.movement(speed,rate,dx,dy)){direction=dx<0?(dy<0?5:dy>0?7:3):dx>0?(dy<0?6:dy>0?8:4):dy<0?1:dy>0?2:0;if(dx||dy)angle=std::atan2(dy,dx);}
    dx=float(float(effect_scale.x)*float(base_scale.x)*float(dx));
    dy=float(float(effect_scale.y)*float(base_scale.y)*float(dy));
    if(dx<0&&velocity.x>=0)effects.animation_interrupt(1);
    else if(dx==0&&velocity.x<0)effects.animation_interrupt(2);
    if(dx>0&&velocity.x<=0)effects.animation_interrupt(3);
    else if(dx==0&&velocity.x>0)effects.animation_interrupt(4);
    velocity={dx,dy};step={rate*dx,rate*dy};
    position.x+=step.x;position.y+=step.y;
    // Preserve the original addition before the playfield-bound comparison.
    if(position.x<field.origin.x)position.x=field.origin.x;
    else if(float(field.extent.x)+field.origin.x<position.x)position.x=float(float(field.extent.x)+field.origin.x);
    if(position.y<field.origin.y)position.y=field.origin.y;
    else if(float(field.extent.y)+field.origin.y<position.y)position.y=float(float(field.extent.y)+field.origin.y);
    hit_bounds=bounds(position,hit_extent);graze_bounds=bounds(position,graze_extent);item_bounds=bounds(position,item_extent);
    effects.update_options();
    if(dx!=0||dy!=0){for(u32 i=15;i>0;--i)history[i]=history[i-1];history[0]=position;}
}
}
