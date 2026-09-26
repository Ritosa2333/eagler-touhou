#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <set>
// Shared gesture policy, adapted from eagler-th07 Touch.cpp (CC0). Only the
// adapter supplies game state and consumes movement; no score or stage rule
// lives here. The original 60Hz game determines the actual movement distance.
namespace touhou::input {
struct TouchState {int context=0,instance=0;bool ready=false;float x=0,y=0,fast=0,slow=0,min_x=0,min_y=0,max_x=640,max_y=480;};
struct TouchSample {bool keys[256]{};int motion=0;bool stick=false;float x=0,y=0;};
class TouchController {
    struct Gesture {bool active=false;int id=0,count=0;float x=0,y=0,last_x=0,last_y=0;std::uint64_t start=0;};
    Gesture menu,dialogue,tap;std::set<int> fingers;int primary=0,instance=0,context=-1;
    bool dragging=false,tap_armed=false,tap_moved=false;float previous_x=0,previous_y=0,target_x=0,target_y=0,tap_x=0,tap_y=0;
    std::uint64_t tap_time=0;int confirm_ticks=0,bomb_ticks=0,escape_ticks=0;std::uint32_t bomb_serial=0,escape_serial=0;
public:
    bool enabled=true,unlimited=false,fire=false,focus=false,two_finger=false,double_tap=false;
    float sensitivity=1,stick_x=0,stick_y=0;int mode=0;
    void clear_motion(){dragging=false;primary=instance=0;}
    void cancel(){clear_motion();fingers.clear();menu={};dialogue={};tap={};tap_armed=false;}
    void reset(){cancel();confirm_ticks=bomb_ticks=escape_ticks=0;}
    void controls(bool shoot,bool slow,std::uint32_t bomb,std::uint32_t escape,float x,float y){fire=shoot;focus=slow;stick_x=std::clamp(x/32767.f,-1.f,1.f);stick_y=std::clamp(y/32767.f,-1.f,1.f);if(bomb!=bomb_serial){bomb_ticks=3;bomb_serial=bomb;}if(escape!=escape_serial){escape_ticks=3;escape_serial=escape;}}
    void pointer(int type,int id,float x,float y,std::uint64_t now,const TouchState& s,bool key_slow){
        if(!enabled)return;if(context!=s.context){cancel();context=s.context;}const float px=x*640,py=y*480;
        if(type==0&&fingers.count(id)){fingers.erase(id);if(primary==id)clear_motion();if(menu.id==id)menu={};if(dialogue.id==id)dialogue={};}
        if(type==2){fingers.erase(id);
            if(dialogue.active&&dialogue.id==id){if(s.context==2&&now-dialogue.start<500&&std::abs(px-dialogue.x)<=24&&std::abs(py-dialogue.y)<=24)confirm_ticks=3;dialogue={};}
            if(menu.active&&(menu.id==id||menu.count>=2)){if(std::abs(menu.last_x-menu.x)<=24&&std::abs(menu.last_y-menu.y)<=24){if(menu.count>=2)escape_ticks=3;else confirm_ticks=3;}menu={};}
            if(tap.active&&tap.id==id){const float dx=px-tap.x,dy=py-tap.y;tap_armed=double_tap&&s.context==1&&now-tap.start<=220&&!tap_moved&&dx*dx+dy*dy<=576;tap_x=px;tap_y=py;tap_time=now;tap={};}
            if(primary==id)clear_motion();return;
        }
        if(type==0){fingers.insert(id);if(s.context!=1){tap_armed=false;if(s.context==2){if(!dialogue.active)dialogue={true,id,1,px,py,px,py,now};}else if(!menu.active)menu={true,id,1,px,py,px,py,now};else menu.count++;return;}
            if(fingers.size()>=4){escape_ticks=3;cancel();return;}const float dx=px-tap_x,dy=py-tap_y;
            if(double_tap&&tap_armed&&now-tap_time<=320&&dx*dx+dy*dy<=38.4f*38.4f){bomb_ticks=3;tap_armed=false;tap={};return;}
            tap_armed=false;if(double_tap){tap={true,id,1,px,py,px,py,now};tap_moved=false;}
            if(!s.ready||mode>=2||dragging)return;dragging=true;primary=id;previous_x=x;previous_y=y;target_x=s.x;target_y=s.y;instance=s.instance;return;
        }
        if(menu.active&&menu.id==id){menu.last_x=px;menu.last_y=py;return;}
        if(tap.active&&tap.id==id){const float dx=px-tap.x,dy=py-tap.y;if(dx*dx+dy*dy>576)tap_moved=true;}
        if(id==primary){if(!dragging||s.context!=1||!s.ready||instance!=s.instance){clear_motion();return;}
            const bool slow=focus||(two_finger&&fingers.size()>1)||key_slow;const float scale=!unlimited&&slow&&s.fast?s.slow/s.fast:1;
            target_x=std::clamp(target_x+std::clamp((x-previous_x)*640,-640.f,640.f)*sensitivity*scale,s.min_x,s.max_x);
            target_y=std::clamp(target_y+std::clamp((y-previous_y)*480,-480.f,480.f)*sensitivity*scale,s.min_y,s.max_y);previous_x=x;previous_y=y;
        }
    }
    TouchSample sample(const TouchState& s,std::uint64_t now,bool key_slow,bool arrows){
        TouchSample out;if(context!=s.context){cancel();context=s.context;}if(!s.ready||arrows)clear_motion();
        if(confirm_ticks>0){out.keys[90]=true;--confirm_ticks;}if(escape_ticks>0){out.keys[27]=true;--escape_ticks;}
        // Actions remain available while movement is blocked (deathbomb).
        // Consume the pulse every tick so a late press cannot wait for respawn.
        const bool bomb=bomb_ticks>0;if(bomb_ticks>0)--bomb_ticks;
        if(!enabled)return out;
        if(s.context==2&&dialogue.active&&now-dialogue.start>=500)out.keys[17]=true;
        if(s.context!=1&&menu.active){const float dx=menu.last_x-menu.x,dy=menu.last_y-menu.y;if(std::abs(dx)>24||std::abs(dy)>24){if(std::abs(dx)>std::abs(dy))out.keys[dx>0?39:37]=true;else out.keys[dy>0?40:38]=true;}}
        if(s.context==1){out.keys[90]=fire||out.keys[90];out.keys[16]=focus||(two_finger&&fingers.size()>1);out.keys[88]=bomb;
          if(s.ready){
            if(!arrows&&mode==3&&(stick_x||stick_y)){const float speed=key_slow||out.keys[16]?s.slow:s.fast;out.motion=1;out.stick=true;out.x=s.x+stick_x*speed;out.y=s.y+stick_y*speed;}
            else if(mode>=2){out.keys[37]=stick_x<-.25f;out.keys[39]=stick_x>.25f;out.keys[38]=stick_y<-.25f;out.keys[40]=stick_y>.25f;}
            if(dragging&&instance==s.instance){out.motion=unlimited?2:1;out.stick=false;out.x=target_x;out.y=target_y;}
          }
        }else if(s.context!=3){out.keys[37]=out.keys[37]||stick_x<-.25f;out.keys[39]=out.keys[39]||stick_x>.25f;out.keys[38]=out.keys[38]||stick_y<-.25f;out.keys[40]=out.keys[40]||stick_y>.25f;}
        return out;
    }
    int current_context()const{return context;}bool active()const{return dragging;}
};
}
