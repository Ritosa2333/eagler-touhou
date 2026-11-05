#include "ScreenEffects.hpp"
#include <algorithm>
namespace th09 {
bool ScreenEffects::update_one(ScreenEffect& e,const ScreenEffectContext& c){
    auto& p=e.request;auto& t=e.time;
    switch(p.kind){
    case 0:case 2:case 4:
        if(p.kind&&c.application_transition)return false;
        if(p.duration){const float value=t.value()*255.f/float(p.duration);e.opacity=std::max(i32(p.kind?value:255.f-value),0);}
        if(t.current>=p.duration)return false;
        if(!p.kind||(!c.paused&&!c.game_over))t.tick(c.timing);return true;
    case 1:{
        if((p.side>=0&&p.side<3&&(c.field_flags[p.side]&1))||(c.game_flags&0x1800))return true;
        if(c.application_transition)return false;t.tick(c.timing);if(t.current>=p.duration)return false;
        const i32 first=signed_bits(p.color),last=signed_bits(p.secondary);
        const float amplitude=t.value()*float(wrapping_sub(last,first))/float(p.duration)+float(first);
        const auto axis=[&](){const u32 v=random.bounded32(3);return v==0?0.f:v==1?amplitude:-amplitude;};
        const float x=axis(),y=axis();output.shake(p.side,x,y);return true;
    }
    case 3:
        if(c.application_transition)return false;
        if(t.current<p.duration){const i32 alpha=i32(p.secondary>>24);e.opacity=std::max(alpha-i32(t.value()*float(alpha)/float(p.duration)),0);}
        else{e.opacity=0;p.color-=1;if(signed_bits(p.color)<1)return false;t.reset();}
        t.tick(c.timing);return true;
    case 5:case 6:
        if(e.release){if(t.current>8)return false;e.opacity=128-i32(t.value()*31.875f);}
        else e.opacity=p.duration&&t.current<=p.duration?i32(t.value()*255.f/float(p.duration)):255;
        t.tick(c.timing);return true;
    case 7:{
        if(c.active_frames<2)return false;t.tick(c.timing);
        const i32 rise=signed_bits(p.color),plateau=wrapping_add(rise,signed_bits(p.secondary)),end=wrapping_add(plateau,p.delay);float proportion;
        if(t.current<rise)proportion=t.value()/float(rise);
        else if(t.current<plateau)proportion=1;
        else if(t.current<end)proportion=(float(u32(end))-t.value())/float(u32(p.delay));
        else return false;
        const float amplitude=float(p.duration)*proportion;
        const auto axis=[&](){const u32 v=random.bounded32(3);return v==0?0.f:v==1?amplitude:-amplitude;};
        const float x=axis(),y=axis();output.shake(3,x,y);return true;
    }
    default:return false;
    }
}
void ScreenEffects::draw_one(const ScreenEffect& e){
    const auto& p=e.request;if(p.kind==1||p.kind==7)return;
    const u32 color=(u32(e.opacity)<<24)|(p.kind==3?p.secondary&0xffffff:p.color);
    if(p.kind==2||p.kind==3)output.rectangle(32,16,416,464,color,false);
    else if(p.kind==6){output.rectangle(16,16,304,464,color,false);output.rectangle(336,16,624,464,color,false);}
    else output.rectangle(0,0,640,480,color,p.kind==0||p.kind==4);
}
}
